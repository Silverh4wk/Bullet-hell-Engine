#ifndef SPATIAL_HASHING_H
#define SPATIAL_HASHING_H

#include <stdlib.h>
#include "../hash_table.h"
#include "../array_list.h"
#include "../../helpers.h"
#include "../physics.h"

struct SpatialHash{
    HashTable* table;
 } ;


struct SpatialHash*
spatialHashCreate(void);

void
spatialHashDestroy(struct SpatialHash* sh);

static inline int
worldToCell(real32 coordinate);

static void
makeCellKey(int x, int y, char *out, size_t outSize);

void
spatialHashClear(struct SpatialHash* sh);

void
spatialHashInsert(struct SpatialHash* sh, struct Body* body);

void
spatialHashQuery(struct SpatialHash* sh,vec2 pos, float radius,struct Array_List* outResults);

#endif
