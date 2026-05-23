#include "spatial_hashing.h"
#include <linmath.h>
#include "stdlib.h"
#include "../physics.h"


//create and return spatial hash table
struct SpatialHash *spatialHashCreate(int spacing, int maximum_number_of_objects)
{
    struct SpatialHash* sh = (struct SpatialHash*) malloc(sizeof(struct SpatialHash));

    sh->spacing      = spacing;
    sh->table_size   = 2 *  maximum_number_of_objects;

    sh->cell_start   = arrayListCreate(sizeof(int32), sh->table_size+1);
    for (int i = 0; i <= sh->table_size; i++) {
	int zero = 0;
	arrayListAppend(sh->cell_start, &zero);
    }

    sh->cell_entries = arrayListCreate(sizeof(int32),  maximum_number_of_objects);
    for (int i = 0; i < maximum_number_of_objects; i++) {
	int zero = 0;
	arrayListAppend(sh->cell_entries, &zero);
    }

    sh->query_Ids    = arrayListCreate(sizeof(int32),  maximum_number_of_objects);
    for (int i = 0; i < maximum_number_of_objects; i++) {
	int zero = 0;
	arrayListAppend(sh->query_Ids, &zero);
    }

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

static inline int
worldToCell(real32 coordinate, int spacing)
{
    return (int)floorf(coordinate / spacing);
};

//im not smart enough for this, had ai help me 
void                                                                //get from the global body list
spatialHashBuildAABB(struct SpatialHash* sh) {

    int num_bodies = physicsGetBodyCount();
    int max_objects = sh->cell_entries->len;
    if (num_bodies > max_objects) num_bodies = max_objects;
    
    //determine cell sizes
    for (int i = 0; i <= sh->table_size; i++) {
	*(int*)arrayListGet(sh->cell_start, i) = 0;
    }
    
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
	       (*(int*)arrayListGet(sh->cell_start, h))++;
	    }
	}
    }
    
    // determine cell starts
    int start = 0;
    for (int i = 0; i < sh->table_size; i++) {
        int* cs = (int*)arrayListGet(sh->cell_start, i);
	start += *cs;
	*cs = start;
    }
    
    *(int*)arrayListGet(sh->cell_start, sh->table_size) = start;   // guard

    
    int total_entries = start;
    if(total_entries > (int)sh->cell_entries->len)
    {
	int* new_entries = realloc(sh->cell_entries->items, total_entries*sizeof(int));

	if(!new_entries) return;
	sh->cell_entries->items = new_entries;
	sh->cell_entries->capacity = total_entries;
	sh->cell_entries->len = total_entries;

	int* new_query = realloc(sh->query_Ids->items, total_entries*sizeof(int));
	if(!new_query) return;
	sh->query_Ids->items =new_query;
	sh->query_Ids->capacity = total_entries;
	sh->query_Ids->len = total_entries;
    }
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
		int* start_ptr = (int*)arrayListGet(sh->cell_start, h);
		(*start_ptr)--;
		int idx = *start_ptr;
		*(int*)arrayListGet(sh->cell_entries,idx) = i; 
            }
        }
    }
}


/* void */
/* makeCellKey(int x, int y, char *out, size_t outSize){ */
/*     snprintf(out, outSize, "%d,%d", x, y); */
/* }; */


void
spatialHashQuery(struct SpatialHash * sh,struct Body* b, int object_id){

    int max_objects = sh->cell_entries->len;
	
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
            int start = *(int*)arrayListGet(sh->cell_start, h);
            int end =   *(int*)arrayListGet(sh->cell_start,h + 1);

	    for (int i = start; i < end; i++) {
		if(sh->query_size >= (int)sh->query_Ids->len)
		    break;
		int* qs = (int*)arrayListGet(sh->query_Ids, sh->query_size);
		*qs = *(int*)arrayListGet(sh->cell_entries, i);
		sh->query_size++;
		
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

