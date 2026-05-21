#ifndef ECS_INTERNAL_H
#define ECS_INTERNAL_H

#include "entity.h"
#include "component.h"
#include "../types.h"
#include "physics.h"

#define MAX_ENTITIES 4096

// bitewise operations helpers
#define HAS_COMPONENT(e, t) (g_component_mask[e] & (1ULL << (t))) // check if this component mask is on or not 
#define ADD_COMPONENT(e, t) (g_component_mask[e] |= (1ULL << (t)))// turn on that component mask 
#define REMOVE_COMPONENT(e, t) (g_component_mask[e] &= ~(1ULL << (t))) // turn off that component mask 

struct PatternEntityState {
    vec4   color_override;
    vec2   size_override;
    real32 time_in_cmd;
    real32 speed_override, accel_x_override, accel_y_override, lifetime_override, total_time;//global overrides
    real32 fire_speed_override;
    real32 fire_accel_x_override, fire_accel_y_override;
    int    override_flags;
    uint32 current_cmd_idx, current_bullet_type_id,  repeats_remaining, bullets_fired_in_cmd;
    bool   pattern_started;     
};


#endif
