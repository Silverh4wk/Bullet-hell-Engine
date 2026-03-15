#include "spatial_hashing.h"
#include "stdlib.h"

#define CELL_SIZE 32.0f //smaller = more cells and fewer checks. and vice versa

//create and return spatial hash table
struct SpatialHash *spatialHashCreate(void)
{
    struct SpatialHash* sh = (struct SpatialHash*) malloc(sizeof(struct SpatialHash));
    sh->table = hashTableCreate();
    return sh;
}

void spatialHashDestroy(struct SpatialHash* sh)
{
    // free ArrayLists stored as values
    HashTableIterator it = hashTableIterator(sh->table);
    while (hashTableNext(&it)) {
	struct Array_List* list = (struct Array_List*)it.value;
	arrayListDestroy(list);
    }
    hashTableDestroy(sh->table);
    free(sh);
}

static inline int worldToCell(real32 coordinate)
{
    return (int)floorf(coordinate / CELL_SIZE);
};

static void makeCellKey(int x, int y, char *out, size_t outSize){
    snprintf(out, outSize, "%d,%d", x, y);
};

void spatialHashClear(struct SpatialHash* sh)
{
    HashTableIterator it = hashTableIterator(sh->table);
    while (hashTableNext(&it)) {
        struct Array_List* list = (struct Array_List*)it.value;
        arrayListDestroy(list);
    }
    hashTableDestroy(sh->table);
    sh->table = hashTableCreate();
}


void spatialHashInsert(struct SpatialHash* sh, struct Body* body)
{
    vec2 minC, maxC;
    minC[0] = worldToCell(body->aabb.coords[0] - body->aabb.dims[0]);
    maxC[1] = worldToCell(body->aabb.coords[0] - body->aabb.dims[0]);
    minC[0] = worldToCell(body->aabb.coords[0] - body->aabb.dims[0]);
    maxC[1] = worldToCell(body->aabb.coords[0] - body->aabb.dims[0]);

    char key[32];

    for (int cx = minC[0]; cx <= maxC[0]; cx++) {
        for (int cy = minC[1]; cy <= maxC[1]; cy++) {
            makeCellKey(cx, cy, key, sizeof(key));

            struct Array_List* list = (struct Array_List*)hashTableGet(sh->table, key);
            if (!list) {
                list = arrayListCreate(sizeof(struct Body*), 4);
                hashTableSet(sh->table, key, list);
            }

            arrayListAppend(list, body);
        }
    }
}

void spatialHashQuery(struct SpatialHash* sh,vec2 pos, real32 radius,struct Array_List* outResults)
{
    int minX = worldToCell(pos[0] - radius);
    int maxX = worldToCell(pos[0] + radius);
    int minY = worldToCell(pos[1] - radius);
    int maxY = worldToCell(pos[1] + radius);

    char key[32];

    for (int cx = minX; cx <= maxX; cx++) {
        for (int cy = minY; cy <= maxY; cy++) {
            makeCellKey(cx, cy, key, sizeof(key));

            struct Array_List* list =  (struct Array_List*)hashTableGet(sh->table, key);
            if (!list) continue;

            for (size_t i = 0; i < list->len; i++) {
                struct Body* c = *(struct Body**)arrayListGet(list, i);
                arrayListAppend(outResults, &c);
            }
        }
    }
}

