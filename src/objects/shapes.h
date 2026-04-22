#ifndef SHAPES_H
#define SHAPES_H

#include <linmath.h>
#include "../types.h"
#include <stdbool.h>
#include <Glad/glad.h>
#include "../engine/physics.h"

#define PI 3.14159
 
typedef enum {
  SHAPE_QUAD,
  SHAPE_CIRCLE
} ShapeType;
    
struct Shape {
    struct Body* body;
    ShapeType shape_type;
    Type type;
    vec2 pos;
    vec4 color;          
    GLuint texture;
    bool physics_enabled;
    union {
        struct {
            vec2 size;
            float rotation_angle;
        } quad;
        struct {
            float radius;
            float origin;  
        } circle;
    } data;
};


enum ShapeResults{
    SHAPE_OK,
    SHAPE_ERR_INVALID_SIZE,
    SHAPE_ERR_INVALID_COLOR,
    SHAPE_ERR_INTERNAL
}  ;

struct ShapeUnion {
    enum  ShapeResults result;
    //return a potiner to the allocated object
    struct Shape* shape;    
};

void
initShapesPool(void);//initiate the object pool for quad objects


//allocoate memorycreates the quad with the given param
//position and size are needed but color can be NULL
struct ShapeUnion
shapeQuadCreate(vec2 pos, vec2 size , vec4* color, Type t, bool t_physics);

struct ShapeUnion
shapeCircleCreate(vec2 pos, real32 radius , vec4* color, Type t, bool t_physics);


void
shapeAddPhysics(struct Shape *shape);

//change the position of the quad  
void
shapeMove(struct Shape *shape, real32 posx, real32 posy);

//set the size of the quad
void
quadSetSize(struct Shape *quad, real32 width, real32 height);

//set the size of the quad
void
circleSetRadius(struct Shape *shape, real32 width, real32 height);

//set the hitbox size of the quad
void
quadSetHitBoxSize(struct Shape *shape, real32 width, real32 height);

//set the color of the quad
void
shapeSetColor(struct Shape *shape, real32 c1, real32 c2, real32 c3, real32 c4) ;

//delete the quad and free the allocated memory
void
shapeDelete(struct Shape *shape);


#endif
