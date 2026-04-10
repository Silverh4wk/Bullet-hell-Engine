#ifndef CAMERA_H
#define CAMERA_H

#include <linmath.h>
#include "../types.h"

typedef struct Camera {
    vec2 position;      // world position 
    vec2 offset;        // additional offset if needed
    real32 zoom;         // zoom factor (1.0 = normal, >1 = zoomed out, <1 = zoomed in)
    real32 follow_speed; // follow speed 

    vec2 target_pos;    
    real32 target_zoom;
    mat4x4 view; //need to be init to identity matrix first
} Camera;

// init a camera default 
void
camera_init(Camera* cam);

// make the camera follow a world position like a player position or anything really
void
camera_follow(Camera* cam, vec2 target, real32 smooth_speed);

// set camera position directly with no smoothing 
void
camera_set_position(Camera* cam, real32 x, real32 y);

// set zoom level (1.0 = default)
void
camera_set_zoom(Camera* cam, float zoom);

// add an offset ,useful for height adjustment or look‑ahead (like pan to boss then pan back)
void
camera_set_offset(Camera* cam, float x, float y);

// update the camera
void
camera_update(Camera* cam, float delta_time);

// Get the final view matrix (upload to shader)
mat4x4*
camera_get_view_matrix(Camera* cam);

//sets the view using the renderer shader
void
camera_apply(const Camera* cam);

#endif
