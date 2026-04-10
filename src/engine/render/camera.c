#include "../camera.h"
#include "glad/glad.h"

void
camera_init(Camera* cam) {
    cam->position[0] = 0.0f;
    cam->position[1] = 0.0f;
    cam->offset[0] = 0.0f;
    cam->offset[1] = 0.0f;
    cam->zoom = 1.0f;
    cam->follow_speed = 0.0f; // instant by default
    cam->target_pos[0] = 0.0f;
    cam->target_pos[1] = 0.0f;
    cam->target_zoom = 1.0f;
    mat4x4_identity(cam->view);
}

void
camera_follow(Camera* cam, vec2 target, float smooth_speed) {
    cam->target_pos[0] = target[0];
    cam->target_pos[1] = target[1];
    cam->follow_speed = smooth_speed;
}

void
camera_set_position(Camera* cam, float x, float y) {
    cam->position[0] = x;
    cam->position[1] = y;
    cam->target_pos[0] = x;
    cam->target_pos[1] = y;
}

void
camera_set_zoom(Camera* cam, float zoom) {
    cam->target_zoom = zoom;
}

void
camera_set_offset(Camera* cam, float x, float y) {
    cam->offset[0] = x;
    cam->offset[1] = y;
}

void
camera_update(Camera* cam, float delta_time) {
    // will smoothly follow the target if the speed is greater than 0
    if (cam->follow_speed > 0.0f) {
        float t = cam->follow_speed * delta_time;
        if (t > 1.0f) t = 1.0f;
        cam->position[0] += (cam->target_pos[0] - cam->position[0]) * t;
        cam->position[1] += (cam->target_pos[1] - cam->position[1]) * t;
        cam->zoom += (cam->target_zoom - cam->zoom) * t;
    } else {
        cam->position[0] = cam->target_pos[0];
        cam->position[1] = cam->target_pos[1];
        cam->zoom = cam->target_zoom;
    }
    
    // add offset 
    float final_x = cam->position[0] + cam->offset[0];
    float final_y = cam->position[1] + cam->offset[1];
    
    // view matrix: translate world opposite to camera position, then scale by zoom
    mat4x4_identity(cam->view);
    mat4x4_translate(cam->view, -final_x, -final_y, 0.0f);
}

mat4x4
*camera_get_view_matrix(Camera *cam) { return &cam->view; }
