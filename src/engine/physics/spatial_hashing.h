#ifndef SPATIAL_HASHING_H
#define SPATIAL_HASHING_H

#include <stdlib.h>
//#include "../hash_table.h"
#include "../array_list.h"
#include "../../helpers.h"

#define SPATIAL_HASH_SPACING 32 


struct SpatialHash{
    int spacing; // its better to have the spacing h = 2r 
    int table_size;
    int query_size;
    struct Array_List *cell_start;
    struct Array_List *cell_entries;
    struct Array_List *query_Ids;
 } ;



struct SpatialHash
*spatialHashCreate(int spacing, int maximum_number_of_objects);

void
spatialHashDestroy(struct SpatialHash* sh);

void
spatialHashBuildAABB(struct SpatialHash *sh);

void
spatialHashQuery(struct SpatialHash *sh, struct Body *body,int object_id, int max_dist);

/* void */
/* makeCellKey(int x, int y, char *out, size_t outSize); */

/* void */
/* spatialHashClear(struct SpatialHash* sh); */

/* void */
/* spatialHashInsert(struct SpatialHash* sh, struct Body* body); */

/* void */
/* spatialHashQuer(struct SpatialHash* sh,vec2 pos, float radius,struct Array_List* outResults); */

#endif
