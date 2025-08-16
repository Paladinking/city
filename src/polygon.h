#ifndef POLYGON_H
#define POLYGON_H
#include <SDL3/SDL.h>
#include <box2d/box2d.h>

typedef struct Polygon {
    b2BodyId body;
    int16_t count;
    b2Vec2 points[];
} Polygon;

extern Polygon* Polygon_create(b2WorldId world, const b2Vec2* points, 
                               int n_points, b2Transform t);

extern void Polygon_free(Polygon* poly);

#endif
