#ifndef  AABB_H
#define  AABB_H

#include <linmath.h>

/** \brief axis-aligned bounding box

    Simple struct of four floats, divided into two vectors.
    
    coordinates (coords) - The center point of the bounding box
    dims (dimensions) - The half-width and half-height of the box
*/

typedef struct AABB
{
    vec2 coords;
    vec2 dims;
    int toggle;
}AABB;


AABB*
AABB_new(float x, float y, float hW, float hH);


/// Frees the passed AABB.
void AABB_free(AABB *a);

/// Checks if the point x,y lies within the passed AABB
int AABB_contains(AABB *a, float x, float y);

/// Checks if the two passed AABB's intersect
int AABB_intersects(AABB *a, AABB *b);

#endif
