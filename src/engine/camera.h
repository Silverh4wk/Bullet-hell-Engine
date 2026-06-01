#ifndef CAMERA_H
#define CAMERA_H

#include <linmath.h>
#include "../types.h"

typedef struct Camera {
    vec2 position;      // world position 
    vec2 offset;        // additional offset if needed
    real32 zoom;         // zoom factor (1.0 = normal, >1 = zoomed out, <1 = zoomed in)
    real32 follow_speed; // follow speed 
    
    vec2*  target_pos;    
    real32 target_zoom;
    real32 rotate;
    mat4x4 view; //need to be init to identity matrix first
    uint32 follow_mode;         /* LINEAR: 0             
				   /* EASE_IN:       1  
				   /* EASE_IN_SINE:  2	      
				   /* EASE_OUT:      3	      
				   /* EASE_OUT_SINE: 4  
				   /* EASE_IN_OUT:   5      
				   /* EASE_IN_OUT_BACK: 6	  
				   /* SPIKE:	      7 */

				  
    
} Camera;

// init a camera default 
void
camera_init(Camera* cam);

// make the camera follow a world position like a player position or anything really
void
camera_follow_raw(Camera* cam, vec2* pos, real32 follow_speed, uint32 follow_mode);

// follows an entity position across the world
void
camera_follow_entity(Camera* cam, Entity entity, real32 follow_speed, uint32 follow_mode);
// set camera position directly with no smoothing 
void
camera_set_position(Camera* cam, real32 x, real32 y);

// set zoom level (1.0 = default)
void
camera_set_zoom(Camera* cam, real32 zoom);

// angle in rads
void
camera_set_rotation(Camera* cam, real32 angle);

// add an offset ,useful for height adjustment or look‑ahead (like pan to boss then pan back)
void
camera_set_offset(Camera* cam, real32 x, real32 y);

// update the camera
void
camera_update(Camera* cam, real32 delta_time);

// Get the final view matrix (upload to shader)
mat4x4*
camera_get_view_matrix(Camera* cam);

//sets the view using the renderer shader
void
camera_apply(const Camera* cam);

#endif
