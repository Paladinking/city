#ifndef TYPES_H_00
#define TYPES_H_00

#include <box2d/box2d.h>

#define TILE_W 16
#define TILE_H 11

typedef struct LineSet {
    uint32_t len;
    b2Vec2* points;
} LineSet;

#endif
