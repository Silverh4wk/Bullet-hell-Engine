#include <stdlib.h>
#include <math.h>

#include "../AABB.h"
#include "../pool_allocator.h"

static struct PoolAllocator AABB_pool;

static inline void initAABBListPool(void)
    {
	initPool(&AABB_pool, sizeof(AABB*), 256);
    }


AABB*
AABB_new(float x, float y, float hW, float hH) {
	AABB* a = allocatePool(&AABB_pool);
	a->coords[0] = x;
	a->coords[1] = y;
	a->dims[0] = hW;
	a->dims[1] = hH;
	return a;
}

void
AABB_free(AABB *a) {
	free(a);
}

int
AABB_contains(AABB *a, float x, float y) {
	return (x >= a->coords[0]-a->dims[0] &&
			x <= a->coords[0]+a->dims[0]) &&
		   (y >= a->coords[1]-a->dims[1] &&
			y <= a->coords[1]+a->dims[1]);
}

int
AABB_intersects(AABB *a, AABB *b) {
	return (fabsf(a->coords[0] - b->coords[0]) < (a->dims[0] + b->dims[0])) &&
		   (fabs(a->coords[1] - b->coords[1]) < (a->dims[1] + b->dims[1]));
}
