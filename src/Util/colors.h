#ifndef COLORS_H
#define COLORS_H

#include <linmath.h>

// ==========================================================
// BASIC COLORS
// ==========================================================
static  vec4 COL_WHITE       = {1.0f, 1.0f, 1.0f, 1.0f};
static  vec4 COL_BLACK       = {0.0f, 0.0f, 0.0f, 1.0f};
static  vec4 COL_GRAY        = {0.5f, 0.5f, 0.5f, 1.0f};
static  vec4 COL_LIGHT_GRAY  = {0.75f, 0.75f, 0.75f, 1.0f};
static  vec4 COL_DARK_GRAY   = {0.2f, 0.2f, 0.2f, 1.0f};

// ==========================================================
// REDS / PINKS
// ==========================================================
static  vec4 COL_RED         = {1.0f, 0.0f, 0.0f, 1.0f};
static  vec4 COL_CRIMSON     = {1.0f, 0.1f, 0.2f, 1.0f};
static  vec4 COL_MAROON      = {0.5f, 0.0f, 0.0f, 1.0f};
static  vec4 COL_SCARLET     = {1.0f, 0.14f, 0.0f, 1.0f};
static  vec4 COL_PINK        = {1.0f, 0.4f, 0.7f, 1.0f};
static  vec4 COL_HOT_PINK    = {1.0f, 0.1f, 0.6f, 1.0f};
static  vec4 COL_ROSE        = {1.0f, 0.0f, 0.5f, 1.0f};

// ==========================================================
// ORANGES / YELLOWS
// ==========================================================
static  vec4 COL_ORANGE      = {1.0f, 0.5f, 0.0f, 1.0f};
static  vec4 COL_TANGERINE   = {1.0f, 0.6f, 0.1f, 1.0f};
static  vec4 COL_AMBER       = {1.0f, 0.75f, 0.0f, 1.0f};
static  vec4 COL_GOLD        = {1.0f, 0.85f, 0.0f, 1.0f};
static  vec4 COL_YELLOW      = {1.0f, 1.0f, 0.0f, 1.0f};
static  vec4 COL_LEMON       = {1.0f, 0.97f, 0.2f, 1.0f};

// ==========================================================
// GREENS
// ==========================================================
static  vec4 COL_GREEN       = {0.0f, 1.0f, 0.0f, 1.0f};
static  vec4 COL_LIME        = {0.5f, 1.0f, 0.0f, 1.0f};
static  vec4 COL_EMERALD     = {0.0f, 0.8f, 0.5f, 1.0f};
static  vec4 COL_FOREST      = {0.1f, 0.45f, 0.1f, 1.0f};
static  vec4 COL_OLIVE       = {0.5f, 0.5f, 0.0f, 1.0f};
static  vec4 COL_MINT        = {0.6f, 1.0f, 0.7f, 1.0f};

// ==========================================================
// CYANS / BLUES
// ==========================================================
static  vec4 COL_CYAN        = {0.1f, 0.8f, 1.0f, 1.0f};
static  vec4 COL_AQUA        = {0.0f, 1.0f, 1.0f, 1.0f};
static  vec4 COL_SKY         = {0.4f, 0.7f, 1.0f, 1.0f};
static  vec4 COL_BLUE        = {0.0f, 0.4f, 1.0f, 1.0f};
static  vec4 COL_ROYAL_BLUE  = {0.25f, 0.41f, 0.88f, 1.0f};
static  vec4 COL_NAVY        = {0.0f, 0.0f, 0.5f, 1.0f};
static  vec4 COL_TEAL        = {0.0f, 0.5f, 0.5f, 1.0f};

// ==========================================================
// PURPLES
// ==========================================================
static  vec4 COL_VIOLET      = {0.7f, 0.2f, 1.0f, 1.0f};
static  vec4 COL_PURPLE      = {0.6f, 0.0f, 0.8f, 1.0f};
static  vec4 COL_LAVENDER    = {0.8f, 0.6f, 1.0f, 1.0f};
static  vec4 COL_INDIGO      = {0.29f, 0.0f, 0.51f, 1.0f};
static  vec4 COL_MAGENTA     = {1.0f, 0.0f, 0.7f, 1.0f};

// ==========================================================
// BROWNS
// ==========================================================
static  vec4 COL_BROWN       = {0.55f, 0.27f, 0.07f, 1.0f};
static  vec4 COL_CHOCOLATE   = {0.48f, 0.25f, 0.0f, 1.0f};
static  vec4 COL_TAN         = {0.82f, 0.71f, 0.55f, 1.0f};
static  vec4 COL_BEIGE       = {0.96f, 0.96f, 0.86f, 1.0f};

// ==========================================================
// SPECIAL / UI COLORS
// ==========================================================
static  vec4 COL_TRANSPARENT = {0.0f, 0.0f, 0.0f, 0.0f};

// ==========================================================
// NEON COLORS
// ==========================================================
static  vec4 COL_NEON_RED    = {1.0f, 0.1f, 0.1f, 1.0f};
static  vec4 COL_NEON_GREEN  = {0.2f, 1.0f, 0.2f, 1.0f};
static  vec4 COL_NEON_BLUE   = {0.2f, 0.7f, 1.0f, 1.0f};
static  vec4 COL_NEON_PINK   = {1.0f, 0.0f, 0.8f, 1.0f};
static  vec4 COL_NEON_PURPLE = {0.7f, 0.2f, 1.0f, 1.0f};
static  vec4 COL_NEON_CYAN   = {0.0f, 1.0f, 1.0f, 1.0f};

// ==========================================================
// PASTELS
// ==========================================================
static  vec4 COL_PASTEL_RED    = {1.0f, 0.7f, 0.7f, 1.0f};
static  vec4 COL_PASTEL_GREEN  = {0.7f, 1.0f, 0.7f, 1.0f};
static  vec4 COL_PASTEL_BLUE   = {0.7f, 0.8f, 1.0f, 1.0f};
static  vec4 COL_PASTEL_YELLOW = {1.0f, 1.0f, 0.7f, 1.0f};
static  vec4 COL_PASTEL_PURPLE = {0.85f, 0.7f, 1.0f, 1.0f};
static  vec4 COL_PASTEL_PINK   = {1.0f, 0.75f, 0.9f, 1.0f};

#endif
