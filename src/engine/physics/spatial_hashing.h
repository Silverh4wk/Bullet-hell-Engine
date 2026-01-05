#pragma once
#include <stdlib.h>
#include "../hash_table.h"
#include "../array_list.h"
#include "../../helpers.h"
#include "../physics.h"

typedef struct {
    HashTable* table;
} SpatialHash;


SpatialHash* spatialHashCreate(void);
void spatialHashDestroy(SpatialHash* sh);
static inline int worldToCell(real32 coordinate);
static void makeCellKey(vec2 pos, char *out, size_t outSize);

void spatialHashClear(SpatialHash* sh);
void spatialHashInsert(SpatialHash* sh, Body* body);
void spatialHashQuery(SpatialHash* sh,vec2 pos, float radius,Array_List* outResults);
