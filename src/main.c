#define SDL_ASSERT_LEVEL 2

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <box2d/box2d.h>
#include <stdio.h>
#include <stdlib.h>

#include "types.h"
#include "polygon.h"
#include "list.h"
#include "render.h"
#include "serialize.h"


typedef struct GameState {
    b2WorldId worldId;

    uint64_t stamp;

    Collider** collisions;
    Storage col_storage;

    LineSet* lines;
    Storage lines_storage;

    int64_t selected;
    Triangle* collected;
    uint32_t collected_size;

    Polygon* polygons;
    Storage polygon_storage;

    Camera camera;
    b2Vec2 panning_start;
    bool panning;

    b2Vec2* pending;
    Storage pending_storage;

    uint32_t visibilty;

    SDL_Renderer* renderer;

    bool running;

    SDL_Texture* tiles[TILE_W * TILE_H];
} GameState;

void render_polygon(SDL_Renderer* renderer, Collider* poly, SDL_Color color, Camera camera) {
    b2Transform t = b2Body_GetTransform(poly->body);

    render_polygon_shape(renderer, poly->points, poly->count, 
                         t, color, camera);

    b2Vec2* inner = alloca(poly->count * sizeof(b2Vec2));
    indented_polygon(poly->points, poly->count, 0.4, inner);
    color = (SDL_Color){0, 0, 0, 255};
    render_polygon_shape(renderer, inner, poly->count,
                         t, color, camera);
}


void render_game(GameState* g) {
    SDL_SetRenderDrawColor(g->renderer, 0x16, 0x16, 0x16, 0xff);
    SDL_RenderClear(g->renderer);

    Camera cam = g->camera;

    if (g->visibilty & SHOW_MAP) {
        float tx = UNIT_PIXEL_X(0.0, cam);
        float ty = UNIT_PIXEL_Y(0.0, cam);
        float tw = UNIT_LENGTH_PIXEL_X(64.0, cam);
        float th = UNIT_LENGTH_PIXEL_Y(64.0, cam);

        for (uint32_t x = 1; x < TILE_W; ++x) {
            for (uint32_t y = 0; y < TILE_H; ++y) {
                SDL_FRect r = {tx + (x - 1) * tw, ty + y * th, tw, th};
                SDL_Texture* tex = g->tiles[x + TILE_W * y];
                SDL_RenderTexture(g->renderer, tex, NULL, &r);
            }
        }
    }
    
    if (g->visibilty & SHOW_POLYGONS) {
        SDL_Color c = {0, 0, 0, 255};
        for (uint32_t ix = 0; ix < g->polygon_storage.size; ++ix) {
            b2Transform t = b2Transform_identity;
            render_polygon_shape(g->renderer, g->polygons[ix].points, g->polygons[ix].count,
                                 t, c, g->camera);
            //c.r += 25;
        }
    }
    if (g->visibilty & SHOW_LINES) {
        for (uint64_t ix = 0; ix < g->lines_storage.size; ++ix) {
            SDL_Color color = {255, 255, 255, 255};
            render_lines(g->renderer, g->lines[ix].points, 
                         g->lines[ix].len, color, 0.1f, cam);
        }
    }

    if (g->visibilty & SHOW_PENDING) {
        SDL_Color color = {255, 255, 255, 255};
        render_lines(g->renderer, g->pending, g->pending_storage.size,
                     color, 0.1f, cam);
    }

    if (g->visibilty & SHOW_CORNERS) {
        SDL_SetRenderDrawColor(g->renderer, 200, 50, 50, 100);
        for (uint32_t ix = 0; ix < g->lines_storage.size; ++ix) {
            LineSet* l = &g->lines[ix];
            for (uint32_t j = 0; j < l->len; ++j) {
                SDL_FRect r = {UNIT_PIXEL_X(l->points[j].x - CORNER_RAD, cam),
                               UNIT_PIXEL_Y(l->points[j].y - CORNER_RAD, cam),
                               UNIT_LENGTH_PIXEL_X(2 * CORNER_RAD, cam),
                               UNIT_LENGTH_PIXEL_Y(2 * CORNER_RAD, cam)};

                SDL_RenderFillRect(g->renderer, &r);
            }
            
        }

        if (g->selected >= 0) {
            LineSet* sel = &g->lines[g->selected];
            SDL_SetRenderDrawColor(g->renderer, 50, 200, 50, 100);
            for (uint32_t ix = 0; ix < sel->len; ++ix) {
                SDL_FRect r = {UNIT_PIXEL_X(sel->points[ix].x - CORNER_RAD, cam),
                               UNIT_PIXEL_Y(sel->points[ix].y - CORNER_RAD, cam),
                               UNIT_LENGTH_PIXEL_X(2 * CORNER_RAD, cam),
                               UNIT_LENGTH_PIXEL_Y(2 * CORNER_RAD, cam)};
                SDL_RenderFillRect(g->renderer, &r);
            }
        }

        if (g->collected != NULL) {
            SDL_Color c = {50, 200, 150, 100};
            render_triangles(g->renderer, g->collected, g->collected_size, c, g->camera);
        }
    }

    SDL_RenderPresent(g->renderer);
}

static LineSet* find_close_corner(GameState* g, b2Vec2* vec, uint32_t min_ix) {
    for (uint32_t ix = min_ix; ix < g->lines_storage.size; ++ix) {
        LineSet* l = &g->lines[ix];
        for (uint32_t j = 0; j < l->len; ++j) {
            float dx = SDL_fabsf(vec->x - l->points[j].x);
            float dy = SDL_fabsf(vec->y - l->points[j].y);
            if (dx < CORNER_RAD && dy < CORNER_RAD) {
                *vec = l->points[j];
                return l;
            }
        }
    }
    return NULL;
}

Triangle* collect_corners(GameState* g, b2Vec2 pos, uint32_t* size, uint8_t* visited) {
    *size = 0;
    b2Vec2* stack;
    Storage store;

    Triangle* dest;
    Storage dest_store;

    LIST_CREATE(dest, dest_store, Triangle);

    LIST_CREATE(stack, store, b2Vec2);
    LIST_APPEND(stack, store, pos);

    while (store.size > 0) {
        b2Vec2 v = stack[store.size - 1];
        --store.size;
        uint32_t n = 0;
        while (1) {
            b2Vec2 corner = v;
            LineSet* lines = find_close_corner(g, &corner, n);
            if (lines == NULL) {
                break;
            }
            uint64_t ix = lines - g->lines;
            n = ix + 1;
            if (visited[ix]) {
                continue;
            }
            visited[ix] = 1;
            for (uint32_t i = 0; i < lines->len; ++i) {
                LIST_APPEND(stack, store, lines->points[i]);
            }
            uint32_t count;
            Triangle* t = lines_to_triangles(lines->points, lines->len, &count);
            if (t != NULL) {
                for (uint32_t ix = 0; ix < count; ++ix) {
                    LIST_APPEND(dest, dest_store, t[ix]);
                }
                SDL_free(t);
            }
        }
    }

    LIST_FREE(stack, store);

    *size = dest_store.size;
    return dest;
}

static GameState gGame;

void mainloop() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
        case SDL_EVENT_QUIT:
            gGame.running = false;
            break;
        case SDL_EVENT_KEY_DOWN:
            if (e.key.key == SDLK_ESCAPE) {
                gGame.running = false;
                break;
            } else if (e.key.key == SDLK_Z && (e.key.mod & SDL_KMOD_CTRL)) {
                if (gGame.pending_storage.size > 0) {
                    gGame.pending_storage.size--;
                }
            } else if (e.key.key == SDLK_S && (e.key.mod & SDL_KMOD_CTRL)) {
                WorldData data;
                data.lines = gGame.lines;
                data.lines_store = gGame.lines_storage;
                data.polys = gGame.polygons;
                data.polys_store = gGame.polygon_storage;
                write_world(&data);
            } else if (e.key.key == SDLK_RETURN && gGame.pending_storage.size > 2) {
                uint32_t n_points = gGame.pending_storage.size;
                b2Vec2* points = SDL_malloc(n_points * sizeof(b2Vec2));
                SDL_assert_release(points != NULL);
                SDL_memcpy(points, gGame.pending, n_points * sizeof(b2Vec2));
                LineSet l = {n_points, points};
                LIST_APPEND(gGame.lines, gGame.lines_storage, l);
                gGame.pending_storage.size = 0;
            } else if (e.key.key == SDLK_DELETE) {
                if (gGame.selected >= 0) {
                    SDL_free(gGame.lines[gGame.selected].points);
                    gGame.lines[gGame.selected] = gGame.lines[gGame.lines_storage.size - 1];
                    --gGame.lines_storage.size;
                    gGame.selected = -1;
                }
            } else if (e.key.key == SDLK_P) {
                if (gGame.collected_size > 0 && gGame.collected != NULL) {
                    uint32_t count;
                    Polygon* poly = triangles_to_polygons(gGame.collected, 
                            gGame.collected_size, &count);
                    LIST_RESERVE_GROWTH(gGame.polygons, gGame.polygon_storage, count);
                    for (uint32_t ix = 0; ix < count; ++ix) {
                        gGame.polygons[gGame.polygon_storage.size + ix] = poly[ix];
                    }
                    gGame.polygon_storage.size += count;
                    printf("Got polygons: %p, %u\n", poly, count);
                    SDL_free(gGame.collected);
                    SDL_free(poly);
                    gGame.collected = NULL;
                    gGame.collected_size = 0;
                }
            } else if (e.key.key == SDLK_T) {
                if (gGame.selected > 0) {
                    LineSet* l = &gGame.lines[gGame.selected];
                    if (gGame.collected != NULL) {
                        SDL_free(gGame.collected);
                        gGame.collected_size = 0;
                    }
                    gGame.collected = lines_to_triangles(l->points, l->len, &gGame.collected_size);
                }
            } else if (e.key.key == SDLK_1) {
                gGame.visibilty ^= SHOW_MAP;
            } else if (e.key.key == SDLK_2) {
                gGame.visibilty ^= SHOW_CORNERS;
            } else if (e.key.key == SDLK_3) {
                gGame.visibilty ^= SHOW_LINES;
            } else if (e.key.key == SDLK_4) {
                gGame.visibilty ^= SHOW_POLYGONS;
            } else if (e.key.key == SDLK_5) {
                gGame.visibilty ^= SHOW_PENDING;
            } else if (e.key.key == SDLK_LEFT) {
                gGame.camera.pos.x += 8.0f;
            } else if (e.key.key == SDLK_RIGHT) {
                gGame.camera.pos.x -= 8.0f;
            } else if (e.key.key == SDLK_UP) {
                gGame.camera.pos.y += 8.0f;
            } else if (e.key.key == SDLK_DOWN) {
                gGame.camera.pos.y -= 8.0f;
            }
            break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            if (e.button.button == SDL_BUTTON_LEFT ||
                e.button.button == SDL_BUTTON_MIDDLE) {
                b2Vec2 vec = {PIXEL_UNIT_X(e.button.x, gGame.camera),
                              PIXEL_UNIT_Y(e.button.y, gGame.camera)};
                printf("Pressed %f, %f\n", vec.x, vec.y);

                gGame.panning_start = (b2Vec2){e.button.x, e.button.y};
                gGame.panning = true;
            } else if (e.button.button == SDL_BUTTON_RIGHT) {
                if (gGame.collected != NULL) {
                    SDL_free(gGame.collected);
                    gGame.collected = NULL;
                    gGame.collected_size = 0;
                }
                b2Vec2 vec = {PIXEL_UNIT_X(e.button.x, gGame.camera),
                              PIXEL_UNIT_Y(e.button.y, gGame.camera)};
                const bool* keys = SDL_GetKeyboardState(NULL);
                bool shift = keys[SDL_SCANCODE_RSHIFT] || keys[SDL_SCANCODE_LSHIFT];
                if (shift) {

                    uint8_t *visited = SDL_calloc(gGame.lines_storage.size, 1);
                    SDL_assert_release(visited != NULL);
                    gGame.collected = collect_corners(&gGame, vec, &gGame.collected_size,
                                                      visited);
                    SDL_free(visited);
                    printf("%p: %u\n", gGame.collected, gGame.collected_size);
                } else {
                    int64_t min_ix = gGame.selected;
                    gGame.selected = -1;
                    LineSet* l = find_close_corner(&gGame, &vec, min_ix + 1);
                    if (l != NULL) {
                        gGame.selected = l - gGame.lines;
                    }
                }
            }
            break;
        case SDL_EVENT_MOUSE_BUTTON_UP:
            if (e.button.button == SDL_BUTTON_LEFT) {
                if (gGame.panning && gGame.panning_start.x == e.button.x &&
                    gGame.panning_start.y == e.button.y) {
                    b2Vec2 vec = {PIXEL_UNIT_X(e.button.x, gGame.camera),
                                  PIXEL_UNIT_Y(e.button.y, gGame.camera)};
                    if (gGame.visibilty & SHOW_CORNERS) {
                        find_close_corner(&gGame, &vec, 0);
                    }
                    LIST_APPEND(gGame.pending, gGame.pending_storage, vec);
                }
                gGame.panning = false;
            } else if (e.button.button == SDL_BUTTON_MIDDLE) {
                gGame.panning = false;
            }
            break;
        case SDL_EVENT_MOUSE_MOTION:
            if (gGame.panning) {
                gGame.camera.pos.x += e.motion.x - gGame.panning_start.x;
                gGame.camera.pos.y += e.motion.y - gGame.panning_start.y;
                gGame.panning_start = (b2Vec2){e.motion.x, e.motion.y};
            }
            break;
        case SDL_EVENT_MOUSE_WHEEL: 
            {
                b2Vec2 mouse = {PIXEL_UNIT_X(e.wheel.mouse_x, gGame.camera),
                                PIXEL_UNIT_Y(e.wheel.mouse_y, gGame.camera)};
                gGame.camera.scale += e.wheel.y * 0.2;
                if (gGame.camera.scale < 0.2) {
                    gGame.camera.scale = 0.2;
                } else if (gGame.camera.scale > 5.0) {
                    gGame.camera.scale = 5.0;
                }
                b2Vec2 nmouse = {PIXEL_UNIT_X(e.wheel.mouse_x, gGame.camera),
                                 PIXEL_UNIT_Y(e.wheel.mouse_y, gGame.camera)};
                gGame.camera.pos.x -= UNIT_LENGTH_PIXEL_X(mouse.x - nmouse.x, gGame.camera);
                gGame.camera.pos.y -= UNIT_LENGTH_PIXEL_Y(mouse.y - nmouse.y, gGame.camera);

                break;
            }
        }
    }

    uint64_t now = SDL_GetTicks();
    uint64_t delta = now - gGame.stamp;
    gGame.stamp = now;
    if (delta == 0) {
        return;
    }
    double d = delta / 1000.0;
    const double timestep = 1 / 60.0;

    while (d > timestep) {
        b2World_Step(gGame.worldId, timestep, 4);
        d -= timestep;
    }

    render_game(&gGame);
}

void run() {
    while (gGame.running) {
        mainloop();
    }
}

SDL_Texture* load_image(SDL_Renderer* renderer, uint32_t x, uint32_t y);

int main() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Failed initializing SDL3: %s", SDL_GetError());
    }

    SDL_Renderer* renderer;
    SDL_Window* window;
    if (!SDL_CreateWindowAndRenderer("City", 1920, 1080, SDL_WINDOW_BORDERLESS, &window, &renderer)) {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Failed creating window: %s", SDL_GetError());
    }

    SDL_SetRenderVSync(renderer, 1);

    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity.y = 0.0f;
    gGame.worldId = b2CreateWorld(&worldDef);
    gGame.renderer = renderer;
    gGame.camera = (Camera){{0.0, 0.0}, 2.0};
    gGame.stamp = SDL_GetTicks();
    gGame.selected = -1;

    LIST_CREATE(gGame.collisions, gGame.col_storage, Collider*);
    LIST_CREATE(gGame.pending, gGame.pending_storage, b2Vec2);

    WorldData data;
    read_world(&data);
    gGame.lines = data.lines;
    gGame.lines_storage = data.lines_store;
    gGame.polygons = data.polys;
    gGame.polygon_storage = data.polys_store;

    gGame.running = true;
    gGame.panning = false;
    gGame.visibilty = SHOW_MAP | SHOW_CORNERS | SHOW_LINES | 
                      SHOW_PENDING | SHOW_POLYGONS;
    gGame.collected = NULL;
    gGame.collected_size = 0;

    for (uint32_t x = 0; x < TILE_W; ++x) {
        for (uint32_t y = 0; y < TILE_H; ++y) {
            char filename[1024];
            sprintf(filename, "map/png/tile_%u_%u.png", x, y);
            SDL_Surface* s = IMG_Load(filename);
            if (s == NULL) {
                printf("Failed loading '%s': %s\n", filename, SDL_GetError());
                return 1;
            }
            for (uint32_t px = 0; px < s->w; ++px) {
                for (uint32_t py = 0; py < s->h; ++py) {
                    uint8_t r, g, b, a;
                    SDL_ReadSurfacePixel(s, px, py, &r, &g, &b, &a);

                    SDL_WriteSurfacePixel(s, px, py, 255 - r, 255 - g, 255 - b, a);
                }
            }

            gGame.tiles[x + TILE_W * y] = SDL_CreateTextureFromSurface(renderer, s);
            if (gGame.tiles[x + TILE_W * y] == NULL) {
                printf("Failed loading '%s': %s\n", filename, SDL_GetError());
                return 1;
            }
            SDL_DestroySurface(s);
        }
    }

    run();

    SDL_Quit();
    return 0;
}
