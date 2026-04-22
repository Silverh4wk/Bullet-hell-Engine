#ifndef ECS_INTERNAL_H
#define ECS_INTERNAL_H

#include "entity.h"
#include "component.h"
#include "../types.h"
#include "physics.h"

#define MAX_ENTITIES 1024

// helpers
#define HAS_COMPONENT(e, t) (g_component_mask[e] & (1ULL << (t)))
#define ADD_COMPONENT(e, t) (g_component_mask[e] |= (1ULL << (t)))
#define REMOVE_COMPONENT(e, t) (g_component_mask[e] &= ~(1ULL << (t)))

// storage for each component arrays
extern struct Transform      g_transforms[MAX_ENTITIES];
extern struct Sprite         g_sprites[MAX_ENTITIES];
extern struct BulletSpawner  g_spawners[MAX_ENTITIES];
extern        uint64         g_component_mask[MAX_ENTITIES];
extern        Entity         g_next_free;
extern        Type           g_type[MAX_ENTITIES];
extern struct ShapeComponent g_shapes[MAX_ENTITIES];
extern        int32          g_body_indices[MAX_ENTITIES]; // its so scuffed but to track what body belong to what

#endif
