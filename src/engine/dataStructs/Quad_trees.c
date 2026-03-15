/*
  quadtree.c
  2014 JSK (kutani@projectkutani.com)

  Part of the Panic Panic project.

  Released to the public domain. See LICENSE for details.
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "../Quad_trees.h"

#include "../physics.h"

/// Default node size cap
#define QTREE_STDCAP 4


/// A function pointer def for determining if an element exists in a range
typedef int (*qtree_fnc)(void *ptr, AABB *range);


/// Quadtree node
typedef struct QuadTreeNode {
    uint16_t cnt;     ///< Number of elements in this node
    uint16_t maxnodecap; ///< Maximum element count per node
    AABB bound;       ///< Area this node covers (reminder that i replaced this one)
    void **elist;     ///< List of element pointers
    struct QuadTreeNode *nw; ///< NW quadrant of this node
    struct QuadTreeNode *ne; ///< NE quadrant of this node
    struct QuadTreeNode *sw; ///< SW quadrant of this node
    struct QuadTreeNode *se; ///< SE quadrant of this node
} QuadTreeNode;

/// Quadtree container
typedef struct _qtree {
    uint16_t maxnodecap; ///< Maximum element count per node
    QuadTreeNode *root;         ///< Root node
    qtree_fnc  cmpfnc;    ///< Element range compare function pointer
} _qtree;

typedef struct _qtree *qtree;

/// Simple container for returning found elements
typedef struct ReturnList {
    uint32_t cnt; ///< Number of elements found
    AABB range;   ///< Range to use for searching
    void **list;  ///< Array of pointers to found elements
} ReturnList;

static void
retlist_add(ReturnList *r, void *p) {
    r->list = realloc(r->list, sizeof(void*)*(r->cnt+1));
    r->list[r->cnt] = p;
    r->cnt++;
}

static uint16_t
qtree_getMaxNodeCnt(qtree q) {
    uint16_t r;
    r = q->maxnodecap;
    return r;
}

static QuadTreeNode*
qnode_new(qtree p, float x, float y, float hW, float hH) {
    QuadTreeNode *q = malloc(sizeof(QuadTreeNode));
    memset(q, 0, sizeof(QuadTreeNode));
    q->bound.coords[0] = x;
    q->bound.coords[1] = y;
    q->bound.dims[0] = hW;
    q->bound.dims[1] = hH;

#if QTREE_THREADSAFE == 1
    q->lock = (p->newfn)();
    q->atomlock = (p->newfn)();
#endif
    return q;
}

static void
qnode_free(qtree q, struct QuadTreeNode *qn) {
    if(qn->cnt)
	free(qn->elist);

    qn->cnt = 0;

    if(qn->nw) {
	qnode_free(q, qn->nw);
	qnode_free(q, qn->ne);
	qnode_free(q, qn->sw);
	qnode_free(q, qn->se);
    }
	
    free(qn);
}

#if QTREE_THREADSAFE == 1
static void
qnode_set_lock(qtree p, QuadTreeNode *q) {
    if(q->lock)
	(p->freefn)(q->lock);
    if(q->atomlock)
	(p->freefn)(q->atomlock);

    q->lock = (p->newfn)();
    q->atomlock = (p->newfn)();

    if(q->nw) {
	qnode_set_lock(p, q->nw);
	qnode_set_lock(p, q->ne);
	qnode_set_lock(p, q->sw);
	qnode_set_lock(p, q->se);
    }
}
#endif

static void
add(QuadTreeNode *q, void *p) {
    q->elist = realloc(q->elist, sizeof(void*)*(q->cnt+1));
    q->elist[q->cnt] = p;
    q->cnt++;
}

static void
drop(QuadTreeNode *q, uint16_t idx) {
    void **narry = malloc(sizeof(void*)*(q->cnt-1));
	
    // This is a little (lot) ugly; a pair of memcpy's would be
    // better, but I had some problems with it
    for(uint16_t i=0,skip=0; i<q->cnt; i++) {
	if(i == idx) { skip++; continue; }
	narry[i-skip] = q->elist[i];
    }
	
    void **old = q->elist;
    q->elist = narry;
    free(old);
    q->cnt--;
}

static void
subdivide(qtree p, QuadTreeNode *q) {
    float cx = q->bound.coords[0];
    float cy = q->bound.coords[1];
    float hw = q->bound.dims[0]/2;
    float hh = q->bound.dims[1]/2;

    q->nw = qnode_new(p, cx-hw, cy-hh, hw, hh);
    q->ne = qnode_new(p, cx+hw, cy-hh, hw, hh);
    q->sw = qnode_new(p, cx-hw, cy+hh, hw, hh);
    q->se = qnode_new(p, cx+hw, cy+hh, hw, hh);
}


static int
qnode_insert(qtree q, QuadTreeNode *qn, void *ptr) {

    int ret = 0;

    if(qn->cnt < qtree_getMaxNodeCnt(q)) {
	add(qn, ptr);
	ret = 1;
    }

    if(! qn->nw)
	subdivide(q, qn);

    if(qnode_insert(q,qn->nw,ptr))
	return 1;
    else if(qnode_insert(q,qn->ne,ptr))
	return 1;
    else if(qnode_insert(q,qn->sw,ptr))
	return 1;
    else if(qnode_insert(q,qn->se,ptr))
	return 1;

    return ret;
}

static void* 
qnode_remove(qtree q, QuadTreeNode *qn, void *ptr) {

    if(qn->cnt) {
	for(uint16_t i=0; i<qn->cnt; i++) {
	    if(qn->elist[i] == ptr) {
		drop(qn, i);
		ptr = NULL;
	    }
	}
    }

	
    if(! qn->nw)
	return NULL;

    if(qnode_remove(q, qn->nw, ptr)) return ptr;
    if(qnode_remove(q, qn->ne, ptr)) return ptr;
    if(qnode_remove(q, qn->sw, ptr)) return ptr;
    if(qnode_remove(q, qn->se, ptr)) return ptr;
    return NULL;
    return ptr;
}

static void
qnode_getInRange(qtree q, QuadTreeNode *qn, ReturnList *r) {
	
    if(qn->cnt) {
	if(! AABB_intersects(&qn->bound, &r->range))
		
	    for(uint16_t i=0; i<qn->cnt; i++)
		if((q->cmpfnc)(qn->elist[i], &r->range))
		    retlist_add(r, qn->elist[i]);
    }

    if(! qn->nw)


	qnode_getInRange(q, qn->nw, r);
    qnode_getInRange(q, qn->ne, r);
    qnode_getInRange(q, qn->sw, r);
    qnode_getInRange(q, qn->se, r);
    return;

}

/* exports */

qtree
qtree_new(float x, float y, float w, float h, qtree_fnc fnc) {
    qtree q = malloc(sizeof(_qtree));
    memset(q, 0, sizeof(_qtree));

    q->maxnodecap = QTREE_STDCAP;
    q->cmpfnc = fnc;
    q->root = qnode_new(q, x+(w/2),y+(h/2),w/2,h/2);

    return q;
}


void
qtree_free(qtree q) {
    void *m;	
    if(q->root) qnode_free(q, q->root);
	
    memset(q, 0, sizeof(_qtree));
	
    free(q);
}

void
qtree_insert(qtree q, void *ptr) {
    
    qnode_insert(q, q->root, ptr);
}

void
qtree_remove(qtree q, void *ptr) {
    qnode_remove(q, q->root, ptr);
}

void
qtree_setMaxNodeCnt(qtree q, uint16_t cnt) {
    q->maxnodecap = cnt || 1;
}

void
qtree_clear(qtree q) {

    float x = q->root->bound.coords[0];
    float y = q->root->bound.coords[1];
    float w = q->root->bound.dims[0];
    float h = q->root->bound.dims[1];
    QuadTreeNode *qn = q->root;
	
    q->root = qnode_new(q, x, y, w, h);

    qnode_free(q, qn);
}

void**
qtree_findInArea(qtree q, float x, float y, float w, float h, uint32_t *cnt) {
    float hw = w/2;
    float hh = h/2;

    ReturnList ret;
    memset(&ret, 0, sizeof(ReturnList));

    ret.range.coords[0] = x+hw;
    ret.range.coords[1] = y+hh;
    ret.range.dims[0] = hw;
    ret.range.dims[1] = hh;

    qnode_getInRange(q, q->root, &ret);

    *cnt = ret.cnt;
    return ret.list;
}
