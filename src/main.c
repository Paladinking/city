#define SDL_ASSERT_LEVEL 2

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <box2d/box2d.h>
#include <stdio.h>

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

#define TILE_W 16
#define TILE_H 11

typedef struct Camera {
    b2Vec2 pos;
    float scale;
} Camera;

typedef struct GameState {
    b2WorldId worldId;

    uint64_t stamp;

    Polygon** polygons;
    int poly_count;

    b2Vec2* selected;

    Camera camera;
    b2Vec2 panning_start;
    bool panning;

    SDL_Renderer* renderer;

    bool running;

    SDL_Texture* tiles[TILE_W * TILE_H];
} GameState;


static GameState gGame;

void render_polygon(SDL_Renderer* renderer, Polygon* poly, SDL_Color color, Camera camera) {
    SDL_assert(poly->count >= 3);
    SDL_Vertex* verts = alloca(poly->count * sizeof(SDL_Vertex));
    int* indicies = alloca(3 * (poly->count - 2) * sizeof(int));

    b2Transform t = b2Body_GetTransform(poly->body);

    for (uint32_t ix = 0; ix < poly->count; ++ix) {
        verts[ix].color.r = color.r / 255.0f;
        verts[ix].color.g = color.g / 255.0f;
        verts[ix].color.b = color.b / 255.0f;
        verts[ix].color.a = color.a / 255.0f;

        b2Vec2 v = b2TransformPoint(t, poly->points[ix]);
        verts[ix].position.x = VEC_PIXEL_X(v, camera);
        verts[ix].position.y = VEC_PIXEL_Y(v, camera);
    }

    for (uint32_t ix = 1; ix < poly->count - 1; ++ix) {
        indicies[(ix - 1) * 3] = 0;
        indicies[(ix - 1) * 3 + 1] = ix;
        indicies[(ix - 1) * 3 + 2] = ix + 1;
    }
    SDL_RenderGeometry(renderer, NULL, verts, poly->count, indicies, 3 * (poly->count - 2));
}

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
            }
            break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            if (e.button.button == SDL_BUTTON_LEFT) {
                b2Vec2 vec = {PIXEL_UNIT_X(e.button.x, gGame.camera),
                              PIXEL_UNIT_Y(e.button.y, gGame.camera)};

                gGame.panning_start = (b2Vec2){e.button.x, e.button.y};
                gGame.panning = true;
            }
            break;
        case SDL_EVENT_MOUSE_BUTTON_UP:
            if (e.button.button == SDL_BUTTON_LEFT) {
                gGame.selected = NULL;
                gGame.panning = false;
            }
            break;
        case SDL_EVENT_MOUSE_MOTION:
            if (gGame.selected != NULL) {
                b2Vec2 vec = {PIXEL_UNIT_X(e.motion.x, gGame.camera),
                              PIXEL_UNIT_Y(e.motion.y, gGame.camera)};
                *gGame.selected = vec;
            } else if (gGame.panning) {
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

    SDL_SetRenderDrawColor(gGame.renderer, 0x0, 0x0, 0x0, 0xff);
    SDL_RenderClear(gGame.renderer);

    float tx = UNIT_PIXEL_X(0.0, gGame.camera);
    float ty = UNIT_PIXEL_Y(0.0, gGame.camera);
    float tw = UNIT_LENGTH_PIXEL_X(64.0, gGame.camera);
    float th = UNIT_LENGTH_PIXEL_Y(64.0, gGame.camera);

    for (uint32_t x = 1; x < TILE_W; ++x) {
        for (uint32_t y = 0; y < TILE_H; ++y) {
            SDL_FRect r = {tx + (x - 1) * tw,
                           ty + y * th, tw, th};
            SDL_Texture* tex = gGame.tiles[x + TILE_W * y];
            SDL_RenderTexture(gGame.renderer, tex, NULL, &r);
        }
    }
    
    SDL_Color color = {150, 150, 150, 255};
    for (uint64_t ix = 0; ix < gGame.poly_count; ++ix) {
        render_polygon(gGame.renderer, gGame.polygons[ix], color, gGame.camera);
    }

    SDL_RenderPresent(gGame.renderer);
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
    if (!SDL_CreateWindowAndRenderer("City", 1920, 1089, SDL_WINDOW_BORDERLESS, &window, &renderer)) {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Failed creating window: %s", SDL_GetError());
    }

    SDL_SetRenderVSync(renderer, 1);

    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity.y = 0.0f;
    gGame.worldId = b2CreateWorld(&worldDef);
    gGame.renderer = renderer;
    gGame.camera = (Camera){{0.0, 0.0}, 2.0};
    gGame.stamp = SDL_GetTicks();
    gGame.poly_count = 1;
    gGame.polygons = SDL_malloc(sizeof(Polygon*) * 4);
    b2Vec2 verts[] = {
        {1.0, 0.0}, {1.0, 1.0}, {0.0, 1.0}, {0.0, 0.0}
    };
    b2Transform t = {{0.0, 0.0}, {1.0, 0.0}};

    gGame.polygons[0] = Polygon_create(gGame.worldId, verts, 4, t);
    gGame.running = true;
    gGame.selected = NULL;
    gGame.panning = false;

    for (uint32_t x = 0; x < TILE_W; ++x) {
        for (uint32_t y = 0; y < TILE_H; ++y) {
            char filename[1024];
            sprintf(filename, "map/png/tile_%u_%u.png", x, y);
            printf("Loading '%s'\n", filename);
            gGame.tiles[x + TILE_W * y] = load_image(renderer, x, y);
            if (gGame.tiles[x + TILE_W * y] == NULL) {
                printf("Failed loading '%s': %s\n", filename, SDL_GetError());
                return 1;
            }
        }
    }

    run();

    SDL_Quit();
    return 0;
}
