#ifndef TYPES_H_00
#define TYPES_H_00

#include <box2d/box2d.h>
#include "polygon.h"
#include "list.h"

#define TILE_W 16
#define TILE_H 11

typedef struct LineSet {
    uint32_t len;
    b2Vec2* points;
} LineSet;

typedef struct WorldData {
    LineSet* lines;
    Storage lines_store;
    Polygon* polys;
    Storage polys_store;
} WorldData;

#endif
