#include "spatial_hashing.h"
#include <linmath.h>
#include "stdlib.h"
#include "../physics.h"
#include "../../equations/equations.h"


//create and return spatial hash table
struct SpatialHash *spatialHashCreate(int spacing, int number_of_objects)
{
    struct SpatialHash* sh = (struct SpatialHash*) malloc(sizeof(struct SpatialHash));

    sh->spacing      = spacing;
    sh->table_size   = 2 * number_of_objects;

    sh->cell_start   = arrayListCreate(sizeof(int32), sh->table_size+1);
    sh->cell_entries = arrayListCreate(sizeof(int32),  number_of_objects);
    sh->query_Ids    = arrayListCreate(sizeof(int32),  number_of_objects);

    sh->query_size = 0;

    return sh;
}

void
spatialHashDestroy(struct SpatialHash* sh)
{
    arrayListDestroy(sh->cell_start);
    arrayListDestroy(sh->cell_entries);
    arrayListDestroy(sh->query_Ids);
    free(sh);
}

// Matthias Müller  function : https://github.com/matthias-research/pages/blob/master/tenMinutePhysics/11-hashing.html
static inline int
hashCoords(int xi,int yi, int zi,  int table_size) {
    int h = (xi * 92837111) ^ (yi * 689287499) ^ (zi * 283923481);	// fantasy function
    return abs(h) % table_size; 
}

//computes the coordinates of a cell that contains the object with given coordiantes 
static inline int
worldToCell(real32 coordinate, int spacing)
{
    return (int)floorf(coordinate / spacing);
};


static inline int
hashPos(int cx, int cy, int cz, int table_size, int spacing) {
    return hashCoords(
	worldToCell(cx,spacing),
	worldToCell(cy,spacing),
	worldToCell(cz,spacing),
	table_size);
}    

void                                                                //get from the global body list
spatialHashBuildAABB(struct SpatialHash* sh) {

    int num_bodies = MIN(physicsGetBodyCount(), sh->cell_entries->len);
    int max_objects = sh->cell_entries->len;
    if (num_bodies > max_objects) num_bodies = max_objects;
    
    //determine cell sizes
    
    //fill with zeroes

    for (int i = 0; i <= sh->table_size; i++) {
	if((int*)arrayListGet(sh->cell_start, i) == NULL)
	{
	    arrayListAppend(sh->cell_start,&(int){0});
	    continue;
	};
	*(int*)arrayListGet(sh->cell_start, i) = 0;
    }

    for (int i = 0; i <= sh->table_size; i++) {
	
	if((int*)arrayListGet(sh->cell_entries, i) == NULL)
	{
	    arrayListAppend(sh->cell_entries,&(int){0});
	    continue;
	};
	
	*(int*)arrayListGet(sh->cell_entries, i) = 0;
    }
    
    for (int i = 0; i < num_bodies; i++) {
        struct Body* b = physicsGetBody(i);
	int h = hashPos(b->aabb.coords[0],b->aabb.coords[1],1, sh->table_size,sh->spacing);
	*(int*)arrayListGet(sh->cell_start,h) +=1 ;
    }
    
    // determine cell starts
    int start = 0;
    for (int i = 0; i < sh->table_size; i++) {
        start = start + *(int*)arrayListGet(sh->cell_start, i);
	*(int*)arrayListGet(sh->cell_start,i) = start;
    }
    
    *(int*)arrayListGet(sh->cell_start, sh->table_size) = start;   // guard

    // fill in object ids

    for(int i =0; i < num_bodies; i++)
    {
	struct Body* b = physicsGetBody(i);
	int h = hashPos(b->aabb.coords[0],b->aabb.coords[1],1, sh->table_size,sh->spacing);
	*(int*)arrayListGet(sh->cell_start,h) -=1 ;
	*(int*)arrayListGet(sh->cell_entries,h) = i;
    }
}


/* void */
/* makeCellKey(int x, int y, char *out, size_t outSize){ */
/*     snprintf(out, outSize, "%d,%d", x, y); */
/* }; */


void
spatialHashQuery(struct SpatialHash *sh, struct Body *body,int object_id, int max_dist)
{
    int x0 = worldToCell(body->aabb.coords[0] - max_dist, sh->spacing);
    int y0 = worldToCell(body->aabb.coords[1] - max_dist, sh->spacing);
    
    int x1 = worldToCell(body->aabb.coords[0] + max_dist, sh->spacing);
    int y1 = worldToCell(body->aabb.coords[1] + max_dist, sh->spacing);

    sh->query_size = 0;
    
    for (int xi = x0; xi < x1; xi++)
    {
	for(int yi = y0; yi < y1; yi++)
	{
	    int h = hashPos(xi,yi,1, sh->table_size,sh->spacing);
	    int start = *(int*)arrayListGet(sh->cell_start,h); 
	    int end   = *(int*)arrayListGet(sh->cell_start,h+1);

	    for (int i = start; i < end ; i++)
	    {
		*(int*)arrayListGet(sh->query_Ids,sh->query_size) = *(int*)arrayListGet(sh->cell_entries,i);
		sh->query_size ++;
	    }
	}
    }
}

/* void spatialHashClear(struct SpatialHash* sh) */
/* { */
/*     HashTableIterator it = hashTableIterator(sh->table); */
/*     while (hashTableNext(&it)) { */
/*         struct Array_List* list = (struct Array_List*)it.value; */
/*         arrayListDestroy(list); */
/*     } */
/*     hashTableDestroy(sh->table); */
/*     sh->table = hashTableCreate(); */
/* } */


/* void spatialHashInsert(struct SpatialHash* sh, struct Body* body) */
/* { */
/*     vec2 minC, maxC; */
/*     minC[0] = worldToCell(body->aabb.coords[0] - body->aabb.dims[0]); */
/*     maxC[0] = worldToCell(body->aabb.coords[0] + body->aabb.dims[0]); */
/*     minC[1] = worldToCell(body->aabb.coords[1] - body->aabb.dims[1]); */
/*     maxC[1] = worldToCell(body->aabb.coords[1] + body->aabb.dims[1]); */

/*     char key[32]; */
/*     for (int cx = minC[0]; cx <= maxC[0]; cx++) { */
/*         for (int cy = minC[1]; cy <= maxC[1]; cy++) { */
/*             makeCellKey(cx, cy, key, sizeof(key)); */

/*             struct Array_List* list = (struct Array_List*)hashTableGet(sh->table, key); */
/*             if (!list) { */
/*                 list = arrayListCreate(sizeof(struct Body*), 4); */
/*                 hashTableSet(sh->table, key, list); */
/*             } */
/*             arrayListAppend(list, &body); */
/*         } */
/*     } */
/* } */

/* void spatialHashQuer(struct SpatialHash* sh,vec2 pos, real32 radius,struct Array_List* outResults) */
/* { */
/*     int minX = worldToCell(pos[0] - radius); */
/*     int maxX = worldToCell(pos[0] + radius); */
/*     int minY = worldToCell(pos[1] - radius); */
/*     int maxY = worldToCell(pos[1] + radius); */

/*     char key[32] = {0}; */

/*     for (int cx = minX; cx <= maxX; cx++) { */
/*         for (int cy = minY; cy <= maxY; cy++) { */
/*             makeCellKey(cx, cy, key, sizeof(key)); */
	    
/*             struct Array_List* list =  (struct Array_List*)hashTableGet(sh->table, key); */
/*             if (!list) continue; */

/*             for (size_t i = 0; i < list->len; i++) { */
   
/*                 struct Body* b = *(struct Body**)arrayListGet(list, i); */
/*                 arrayListAppend(outResults, &b); */
/*             } */
/*         } */
/*     } */
/* } */

