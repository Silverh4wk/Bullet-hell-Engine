#ifndef SPATIAL_HASHING_H
#define SPATIAL_HASHING_H

#include <stdlib.h>
//#include "../hash_table.h"
#include "../array_list.h"
#include "../../helpers.h"
#include "../physics.h"


struct SpatialHash{
    int spacing; // its better to have the spacing h = 2r 
    int table_size;
    int query_size;
    int max_objects;
    int max_entries;  
    int32 *cell_start;
    int32 *cell_entries;
    int32 *query_Ids;
 } ;



struct SpatialHash
*spatialHashCreate(int spacing, int maximum_number_of_objects);

void
spatialHashDestroy(struct SpatialHash* sh);

void
spatialHashBuildPoints(struct SpatialHash *sh);

void
spatialHashQuery(struct SpatialHash *sh, 
		 struct Body *b, int object_id);

    /* void */
/* makeCellKey(int x, int y, char *out, size_t outSize); */

/* void */
/* spatialHashClear(struct SpatialHash* sh); */

/* void */
/* spatialHashInsert(struct SpatialHash* sh, struct Body* body); */

/* void */
/* spatialHashQuery(struct SpatialHash* sh,vec2 pos, float radius,struct Array_List* outResults); */

#endif
