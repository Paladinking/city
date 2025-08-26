#include "render.h"
#include "polygon.h"
#include <stdbool.h>

void find_close_corner(b2Vec2 prev, b2Vec2 cur, b2Vec2 next, b2Vec2* p1, b2Vec2* p2, float width) {
    b2Vec2 v1 = b2Normalize(b2Sub(cur, prev));
    b2Vec2 v2 = b2Normalize(b2Sub(next, cur));

    b2Vec2 ort1 = b2MulSV(width, b2LeftPerp(v1));
    b2Vec2 ort2 = b2MulSV(width, b2LeftPerp(v2));

    b2Vec2 pa1 = b2Add(prev, ort1);
    b2Vec2 pa2 = b2Add(cur, ort1);

    b2Vec2 pb1 = b2Add(cur, ort2);
    b2Vec2 pb2 = b2Add(next, ort2);

    if (!line_intersects(pa1, pa2, pb1, pb2, p1)) {
        // The lines are parallel
        *p1 = pa2;
    }
    pa1 = b2Sub(prev, ort1);
    pa2 = b2Sub(cur, ort1);

    pb1 = b2Sub(cur, ort2);
    pb2 = b2Sub(next, ort2);
    if (!line_intersects(pa1, pa2, pb1, pb2, p2)) {
        *p2 = pa2;
    }
}

void render_lines(SDL_Renderer* renderer, const b2Vec2* points, int n_points,
                  SDL_Color color, float width, Camera camera) {
    SDL_FColor fcolor = {color.r / 255.0f, color.g / 255.0f, color.b / 255.0f,
                         color.a / 255.0f};
    if (n_points < 3) {
        if (n_points == 0) {
            return;
        }
        if (n_points == 1) {
            SDL_SetRenderDrawColor(renderer, color.r, color.g, 
                                   color.b, color.a);
            SDL_FRect r = {UNIT_PIXEL_X(points[0].x - width / 2.0f, camera), 
                           UNIT_PIXEL_Y(points[0].y - width / 2.0f, camera),
                           UNIT_LENGTH_PIXEL_X(width, camera),
                           UNIT_LENGTH_PIXEL_Y(width, camera)};
            SDL_RenderFillRect(renderer, &r);
        } else {
            SDL_Vertex verts[6];
            b2Vec2 v = b2Normalize(b2Sub(points[1], points[0]));
            b2Vec2 ort = b2MulSV(width, b2LeftPerp(v));
            b2Vec2 c1 = b2Add(points[0], ort);
            b2Vec2 c2 = b2Sub(points[0], ort);
            b2Vec2 c3 = b2Add(points[1], ort);
            b2Vec2 c4 = b2Sub(points[1], ort);
            for (uint32_t ix = 0; ix < 6; ++ix) {
                verts[ix].color = fcolor;
            }

            verts[0].position = (SDL_FPoint) 
                {VEC_PIXEL_X(c1, camera), VEC_PIXEL_Y(c1, camera) };
            verts[1].position = (SDL_FPoint)
                {VEC_PIXEL_X(c2, camera), VEC_PIXEL_Y(c2, camera) };
            verts[2].position = (SDL_FPoint)
                {VEC_PIXEL_X(c3, camera), VEC_PIXEL_Y(c3, camera) };
            verts[3].position = (SDL_FPoint)
                {VEC_PIXEL_X(c3, camera), VEC_PIXEL_Y(c3, camera) };
            verts[4].position = (SDL_FPoint)
                {VEC_PIXEL_X(c4, camera), VEC_PIXEL_Y(c4, camera) };
            verts[5].position = (SDL_FPoint)
                {VEC_PIXEL_X(c2, camera), VEC_PIXEL_Y(c2, camera) };

            SDL_RenderGeometry(renderer, NULL, verts, 6, NULL, 0);
        }
        return;
    }
    // 2 verts per corner
    SDL_Vertex* verts = alloca(2 * n_points * sizeof(SDL_Vertex));
    // 2 triangles per line segment
    int* indicies = alloca(6 * n_points * sizeof(int));

    b2Vec2 prev = points[n_points - 1];
    b2Vec2 cur = points[0];
    b2Vec2 next = points[1];

    b2Vec2 c1, c2;
    verts[0].color = fcolor;
    verts[1].color = fcolor;
    find_close_corner(prev, cur, next, &c1, &c2, width);
    verts[0].position = (SDL_FPoint) 
        {VEC_PIXEL_X(c1, camera), VEC_PIXEL_Y(c1, camera)};
    verts[1].position = (SDL_FPoint) 
        {VEC_PIXEL_X(c2, camera), VEC_PIXEL_Y(c2, camera)};

    for (uint32_t ix = 1; ix < n_points - 1; ++ix) {
        prev = points[ix - 1];
        cur = points[ix];
        next = points[ix + 1];
        verts[2 * ix].color = fcolor;
        verts[2 * ix + 1].color = fcolor;
        find_close_corner(prev, cur, next, &c1, &c2, width);
        verts[2 * ix].position = (SDL_FPoint)
            {VEC_PIXEL_X(c1, camera), VEC_PIXEL_Y(c1, camera)};
        verts[2 * ix + 1].position = (SDL_FPoint)
            {VEC_PIXEL_X(c2, camera), VEC_PIXEL_Y(c2, camera)};
    }

    prev = points[n_points - 2];
    cur = points[n_points - 1];
    next = points[0];
    verts[2 * (n_points - 1)].color = fcolor;
    verts[2 * (n_points - 1) + 1].color = fcolor;
    find_close_corner(prev, cur, next, &c1, &c2, width);
    verts[2 * (n_points - 1)].position = (SDL_FPoint)
        {VEC_PIXEL_X(c1, camera), VEC_PIXEL_Y(c1, camera)};
    verts[2 * (n_points - 1) + 1].position = (SDL_FPoint)
        {VEC_PIXEL_X(c2, camera), VEC_PIXEL_Y(c2, camera)};

    for (uint32_t ix = 0; ix < n_points - 1; ++ix) {
        indicies[ix * 6] = ix * 2;
        indicies[ix * 6 + 1] = ix * 2 + 1;
        indicies[ix * 6 + 2] = (ix + 1) * 2;

        indicies[ix * 6 + 3] = ix * 2 + 1;
        indicies[ix * 6 + 4] = (ix + 1) * 2;
        indicies[ix * 6 + 5] = (ix + 1) * 2 + 1;
    }
    indicies[(n_points - 1) * 6] = (n_points - 1) * 2;
    indicies[(n_points - 1) * 6 + 1] = (n_points - 1) * 2 + 1;
    indicies[(n_points - 1) * 6 + 2] = 0;

    indicies[(n_points - 1) * 6 + 3] = (n_points - 1) * 2 + 1;
    indicies[(n_points - 1) * 6 + 4] = 0;
    indicies[(n_points - 1) * 6 + 5] = 1;

    SDL_RenderGeometry(renderer, NULL, verts, 2 * n_points,
                       indicies, 6 * n_points);
}


void render_polygon_shape(SDL_Renderer* renderer, b2Vec2* points, int n_points, b2Transform t,
                          SDL_Color color, Camera camera) {
    SDL_assert(n_points >= 3);
    SDL_Vertex* verts = alloca(n_points * sizeof(SDL_Vertex));
    int* indicies = alloca(3 * (n_points - 2) * sizeof(int));

    for (uint32_t ix = 0; ix < n_points; ++ix) {
        verts[ix].color.r = color.r / 255.0f;
        verts[ix].color.g = color.g / 255.0f;
        verts[ix].color.b = color.b / 255.0f;
        verts[ix].color.a = color.a / 255.0f;

        b2Vec2 v = b2TransformPoint(t, points[ix]);
        verts[ix].position.x = VEC_PIXEL_X(v, camera);
        verts[ix].position.y = VEC_PIXEL_Y(v, camera);
    }

    for (uint32_t ix = 1; ix < n_points - 1; ++ix) {
        indicies[(ix - 1) * 3] = 0;
        indicies[(ix - 1) * 3 + 1] = ix;
        indicies[(ix - 1) * 3 + 2] = ix + 1;
    }
    SDL_RenderGeometry(renderer, NULL, verts, n_points, indicies, 3 * (n_points - 2));
}

void render_triangles(SDL_Renderer* renderer, const Triangle* triangles, int count,
                      SDL_Color color, Camera camera) {
    SDL_Vertex* verts = alloca(count * 3 * sizeof(SDL_Vertex));

    SDL_FColor fcolor = {color.r / 255.0f, color.g / 255.0f, color.b / 255.0f,
                         color.a / 255.0f};
    for (uint32_t ix = 0; ix < count; ++ix) {
        verts[ix * 3].color = fcolor;
        b2Vec2 v1 = triangles[ix].p1;
        verts[ix * 3].position.x = VEC_PIXEL_X(v1, camera);
        verts[ix * 3].position.y = VEC_PIXEL_Y(v1, camera);
        b2Vec2 v2 = triangles[ix].p2;
        verts[ix * 3 + 1].color = fcolor;
        verts[ix * 3 + 1].position.x = VEC_PIXEL_X(v2, camera);
        verts[ix * 3 + 1].position.y = VEC_PIXEL_Y(v2, camera);
        b2Vec2 v3 = triangles[ix].p3;
        verts[ix * 3 + 2].color = fcolor;
        verts[ix * 3 + 2].position.x = VEC_PIXEL_X(v3, camera);
        verts[ix * 3 + 2].position.y = VEC_PIXEL_Y(v3, camera);
        fcolor.r += 0.25;
        if (fcolor.r > 1.0) {
            fcolor.r -= 1.0;
        }
    }
    SDL_RenderGeometry(renderer, NULL, verts, count * 3, NULL, 0);
}
