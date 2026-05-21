#include "spatial_hashing.h"
#include <linmath.h>
#include "stdlib.h"
#include "../physics.h"


//create and return spatial hash table
struct SpatialHash *spatialHashCreate(int spacing, int maximum_number_of_objects)
{
    struct SpatialHash* sh = (struct SpatialHash*) malloc(sizeof(struct SpatialHash));

    sh->spacing = spacing;
    sh->table_size = 2 *  maximum_number_of_objects;
    sh->max_objects =  maximum_number_of_objects;
    sh->max_entries = maximum_number_of_objects * 9;
    sh->cell_start = (int32*)malloc(sizeof(int32) * (sh->table_size + 1));
    sh->cell_entries = (int32*)malloc(sizeof(int32) *  sh->max_entries);
    sh->query_Ids = (int32*)malloc(sizeof(int32) * sh->max_entries);

    sh->query_size = 0;

    return sh;
}

void
spatialHashDestroy(struct SpatialHash* sh)
{
    
    free(sh->cell_start);
    free(sh->cell_entries);
    free(sh->query_Ids);
    free(sh);
}

// Matthias Müller  function : https://github.com/matthias-research/pages/blob/master/tenMinutePhysics/11-hashing.html
static inline int
hashCoords(int xi,int yi, int zi,  int table_size) {
    int h = (xi * 92837111) ^ (yi * 689287499) ^ (zi * 283923481);	// fantasy function
    return abs(h) % table_size; 
}

static inline int
worldToCell(real32 coordinate, int spacing)
{
    return (int)floorf(coordinate / spacing);
};


void                                                                //get from the global body list
spatialHashBuildAABB(struct SpatialHash* sh) {

    int num_bodies = physicsGetBodyCount();
    if (num_bodies > sh->max_objects) num_bodies = sh->max_objects;

    //determine cell sizes
    memset(sh->cell_start, 0, sizeof(int32) * (sh->table_size +1));
    memset(sh->cell_entries, 0, sizeof(int32) * sh->max_entries);
    
    for (int i = 0; i < num_bodies; i++) {
        struct Body* b = physicsGetBody(i);

	real32 minX = b->aabb.coords[0] - b->aabb.dims[0];
	real32 maxX = b->aabb.coords[0] + b->aabb.dims[0];
	real32 minY = b->aabb.coords[1] - b->aabb.dims[1];
	real32 maxY = b->aabb.coords[1] + b->aabb.dims[1];
	
	int cx0 = worldToCell(minX, sh->spacing);
        int cx1 = worldToCell(maxX, sh->spacing);
        int cy0 = worldToCell(minY, sh->spacing);
        int cy1 = worldToCell(maxY, sh->spacing);

	// increment each bucket for each cell
	for (int cx = cx0; cx <= cx1; cx++) {
	    for (int cy = cy0; cy <= cy1; cy++) {
                int h = hashCoords(cx, cy, 0, sh->table_size);
                sh->cell_start[h]++;
            }
	}
    }
    
    // determine cell starts
    int start = 0;
    for (int i = 0; i < sh->table_size; i++) {
        start += sh->cell_start[i];
        sh->cell_start[i] = start;
    }
    
    sh->cell_start[sh->table_size] = start;   // guard


    //  fill in objectids

    for (int i = 0; i < num_bodies; i++) {
        struct Body* b = physicsGetBody(i);

	real32 minX = b->aabb.coords[0] - b->aabb.dims[0];
	real32 maxX = b->aabb.coords[0] + b->aabb.dims[0];
	real32 minY = b->aabb.coords[1] - b->aabb.dims[1];
	real32 maxY = b->aabb.coords[1] + b->aabb.dims[1];

        int cx0 = worldToCell(minX, sh->spacing);
        int cx1 = worldToCell(maxX, sh->spacing);
        int cy0 = worldToCell(minY, sh->spacing);
        int cy1 = worldToCell(maxY, sh->spacing);
	
	for (int cx = cx0; cx <= cx1; cx++) {
            for (int cy = cy0; cy <= cy1; cy++) {
		int h = hashCoords(cx, cy, 0, sh->table_size);
		sh->cell_start[h]--;
		int idx = sh->cell_start[h];
                if (idx < 0 || idx >= sh->max_entries) {
                    return;// return if exceed for now
                }
		sh->cell_entries[idx] = i;
            }
        }
    }
}


/* void */
/* makeCellKey(int x, int y, char *out, size_t outSize){ */
/*     snprintf(out, outSize, "%d,%d", x, y); */
/* }; */


void
spatialHashQuery(struct SpatialHash * sh,struct Body* b, int object_id) {

    real32 minX = b->aabb.coords[0] - b->aabb.dims[0];
    real32 maxX = b->aabb.coords[0] + b->aabb.dims[0];
    real32 minY = b->aabb.coords[1] - b->aabb.dims[1];
    real32 maxY = b->aabb.coords[1] + b->aabb.dims[1];

    int cx0 = worldToCell(minX, sh->spacing);
    int cx1 = worldToCell(maxX, sh->spacing);
    int cy0 = worldToCell(minY, sh->spacing);
    int cy1 = worldToCell(maxY, sh->spacing);

    sh->query_size = 0;

    for (int cx = cx0; cx <= cx1; cx++) {
        for (int cy = cy0; cy <= cy1; cy++) {
	    
            int h = hashCoords(cx, cy, 0, sh->table_size);
            int start = sh->cell_start[h];
            int end = sh->cell_start[h + 1];

	    for (int idx = start; idx < end; idx++) {
                int body_idx = sh->cell_entries[idx];
		//skip if same body
		if (body_idx == object_id) continue;
		if (sh->query_size < sh->max_entries) {
		    sh->query_Ids[sh->query_size++] = body_idx;
		}
		
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

/* void spatialHashQuery(struct SpatialHash* sh,vec2 pos, real32 radius,struct Array_List* outResults) */
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

