#pragma once

#include <stdlib.h>
#include "hash_table.h"
#include "array_list.h"
#include "../helpers.h"
#include "physics.h"
#define CELL_SIZE 32.0f //smaller = more cells and fewer checks. and vice versa

typedef struct {
    HashTable* table;
} SpatialHash;



SpatialHash* spatialHashCreate(void);
void spatialHashDestroy(SpatialHash* sh);


void spatialHashClear(SpatialHash* sh);
void spatialHashInsert(SpatialHash* sh, Body* body);
void spatialHashQuery(SpatialHash* sh,vec2 pos, float radius,Array_List* outResults);
