#ifndef RENDER_H_00
#define RENDER_H_00

#include <box2d/box2d.h>
#include <SDL3/SDL.h>
#include "polygon.h"

#define PIXEL_SCALE 8.0f
#define UNIT_PIXEL_X(coord, cam) (((coord) * 8.0f) * (cam).scale + (cam).pos.x)
#define UNIT_PIXEL_Y(coord, cam) (((coord) * 8.0f) * (cam).scale + (cam).pos.y)
#define VEC_PIXEL_X(coord, cam) (((coord).x * 8.0f) * (cam).scale + (cam).pos.x)
#define VEC_PIXEL_Y(coord, cam) (((coord).y * 8.0f) * (cam).scale + (cam).pos.y)
#define PIXEL_UNIT_X(coord, cam) (((coord) - (cam).pos.x) / (8.0f * (cam).scale))
#define PIXEL_UNIT_Y(coord, cam) (((coord) - (cam).pos.y) / (8.0f * (cam).scale))
#define PIXEL_VEC_X(coord, cam) (((coord).x - (cam).pos.x) / (8.0f * (cam).scale))
#define PIXEL_VEC_Y(coord, cam) (((coord).y - (cam).pos.y) / (8.0f * (cam).scale))
#define UNIT_LENGTH_PIXEL_X(coord, cam) ((coord) * 8.0f * (cam).scale)
#define UNIT_LENGTH_PIXEL_Y(coord, cam) ((coord) * 8.0f * (cam).scale)

#define CORNER_RAD 0.4f

#define SHOW_MAP 1
#define SHOW_CORNERS 2
#define SHOW_LINES 4
#define SHOW_POLYGONS 8
#define SHOW_PENDING 16

typedef struct Camera {
    b2Vec2 pos;
    float scale;
} Camera;


extern void render_lines(SDL_Renderer* renderer, const b2Vec2* points, int n_points,
                         SDL_Color color, float width, Camera camera);

extern void render_triangles(SDL_Renderer* renderer, const Triangle* triangles, int count,
                             SDL_Color color, Camera camera);

extern void render_polygon_shape(SDL_Renderer* renderer, b2Vec2* points, 
                                 int n_points, b2Transform t,
                                 SDL_Color color, Camera camera);

#endif
