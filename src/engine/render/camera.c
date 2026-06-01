#include "../camera.h"
#include "glad/glad.h"
#include "../../equations/equations.h"
#include "../global.h"

void
camera_init(Camera* cam) {
    cam->position[0] = 0.0f;
    cam->position[1] = 0.0f;
    cam->offset[0] = 0.0f;
    cam->offset[1] = 0.0f;
    cam->zoom = 1.0f;
    cam->follow_speed = 0.0f; // instant by default
    cam->target_pos = NULL;
    cam->target_zoom = 1.0f;
    cam->follow_mode = 0;
    cam->rotate =0 ;
    mat4x4_identity(cam->view);
}

void
camera_follow_raw(Camera* cam, vec2* pos,  real32 follow_speed, uint32 follow_mode) {
    cam->target_pos = pos;
    cam->follow_speed = follow_speed;
    cam->follow_mode = follow_mode;
}

void
camera_follow_entity(Camera* cam, Entity entity, real32 follow_speed, uint32 follow_mode) {
    if (entity == 0 || entity >= MAX_ENTITIES) return;
    cam->target_pos = &g_transforms[entity].position;
    cam->follow_speed = follow_speed;
    cam->follow_mode = follow_mode;
}

void
camera_set_position(Camera* cam, real32 x, real32 y) {
    cam->position[0] = x;
    cam->position[1] = y;
}

void
camera_set_rotation(Camera* cam, real32 angle) {
    cam->rotate = angle;
}

void
camera_set_zoom(Camera* cam, real32 zoom) {
    cam->target_zoom = zoom;
}

void
camera_set_offset(Camera* cam, real32 x, real32 y) {
    cam->offset[0] = x;
    cam->offset[1] = y;
}

void
camera_update(Camera* cam, real32 delta_time) {

    if (cam->target_pos == NULL) return;
    
    // will smoothly follow the target if the speed is greater than 0
    if (cam->follow_speed > 0.0f)
    {
        real32 t = cam->follow_speed * delta_time;

	if (t > 1.0f) t = 1.0f; // clamping
	
	t = getTimingFactor(cam->follow_mode, t);
	
        cam->position[0] = lerp(cam->position[0],(*cam->target_pos)[0],t);
        cam->position[1] = lerp(cam->position[1],(*cam->target_pos)[1],t);;
        cam->zoom = lerp(cam->zoom, cam->target_zoom, t);

    }
    else
    {
        cam->position[0] = (*cam->target_pos)[0];
        cam->position[1] = (*cam->target_pos)[1];
        cam->zoom = cam->target_zoom;
    }
    
    // add offset
    real32 screen_center_x = (global.render.width  * 0.5f );
    real32 screen_center_y = (global.render.height * 0.5f );
    
    real32 final_x = (cam->position[0] + cam->offset[0]) ;
    real32 final_y = (cam->position[1] + cam->offset[1]) ;
    
    // view matrix: translate world opposite to camera position, then scale by zoom


    mat4x4 translate_center, scale_zoom, translate_final, rotation;

    
    mat4x4_identity(scale_zoom);
    mat4x4_identity(translate_center);
    mat4x4_identity(translate_final);
    mat4x4_identity(rotation);
    
    mat4x4_translate(translate_center, screen_center_x, screen_center_y, 0.0f);
    
    mat4x4_scale_aniso(scale_zoom, scale_zoom, cam->zoom, cam->zoom, 1.0f);

    mat4x4_rotate_Z(rotation, rotation, cam->rotate);
    
    // move camera to -ve of final[2]
    mat4x4_translate(translate_final, -final_x, -final_y, 0.0f);
    
    //  view = center * zoom * final
    mat4x4_mul(cam->view, translate_center, scale_zoom);      // translate_center * scale_zoom
    mat4x4_mul(cam->view, translate_center, rotation);        // view = translate_center * rotation
    mat4x4_mul(cam->view, cam->view, translate_final);        // view = view * translate_final
    
}

mat4x4
*camera_get_view_matrix(Camera *cam) { return &cam->view; }
