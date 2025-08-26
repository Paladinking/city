#ifndef POLYGON_H
#define POLYGON_H
#include <SDL3/SDL.h>
#include <box2d/box2d.h>

typedef struct Collider {
    b2BodyId body;
    int16_t count;
    b2Vec2 points[];
} Collider;

typedef struct Polygon {
    b2Vec2* points;
    uint32_t count;
} Polygon;

typedef struct Triangle {
    b2Vec2 p1, p2, p3;
} Triangle;


extern void indented_polygon(const b2Vec2* points, uint32_t count, float width, 
                             b2Vec2* dest);

extern Collider* Polygon_create(b2WorldId world, const b2Vec2* points, 
                               int n_points, b2Transform t);

extern void Polygon_free(Collider* poly);

extern bool line_intersects(b2Vec2 pa1, b2Vec2 pa2, b2Vec2 pb1, b2Vec2 pb2, b2Vec2* intersect);

extern bool line_segments_intersects(b2Vec2 pa1, b2Vec2 pa2, b2Vec2 pb1, b2Vec2 pb2, b2Vec2* intersect);

extern bool triangle_contains_point(Triangle t, b2Vec2 p);

extern Triangle* lines_to_triangles(const b2Vec2* points, uint32_t size, uint32_t* out_size);

extern Polygon* triangles_to_polygons(const Triangle* triangles, uint32_t size, uint32_t* out_size);

#endif
