#include "serialize.h"
#include <SDL3/SDL.h>

void read_world(WorldData* data) {
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "reading world.dat...");
    SDL_IOStream* io = SDL_IOFromFile("world.dat", "rb");
    Storage lines_store, polys_store;
    LineSet* newlines;
    Polygon* newpolys;

    if (io == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed loading world: %s",
                     SDL_GetError());
        return;
    }
    uint64_t tot_size;
    if (SDL_ReadIO(io, &tot_size, sizeof(uint64_t)) < sizeof(uint64_t)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "Failed reading world size");
        SDL_CloseIO(io);
        return;
    }
    if (tot_size < 16 || tot_size >= 0x10000000) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Invalid world size");
        SDL_CloseIO(io);
        return;
    }

    uint8_t* buf = SDL_aligned_alloc(tot_size, 8);
    SDL_assert_release(buf != NULL);
    if (SDL_ReadIO(io, buf, tot_size) < tot_size) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed loading world: %s",
                     SDL_GetError());
        SDL_CloseIO(io);
        SDL_aligned_free(buf);
        return;
    }

    SDL_CloseIO(io);

    uint64_t line_entries = *(uint64_t*)(buf);
    if (line_entries >= 0x1000000) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Too many lines in world");
        SDL_aligned_free(buf);
        return;
    }
    LIST_CREATE(newlines, lines_store, LineSet);
    LIST_CREATE(newpolys, polys_store, Polygon);

    uint64_t offset = 8;
    LIST_RESERVE(newlines, lines_store, line_entries);

    for (uint64_t ix = 0; ix < line_entries; ++ix) {
        if (offset + 8 > tot_size) {
            goto fail;
        }
        uint64_t count = *(uint64_t*)(buf + offset);
        offset += 8;
        if (offset + sizeof(b2Vec2) * count > tot_size) {
            goto fail;
        }
        b2Vec2* points = SDL_malloc(count * sizeof(b2Vec2));
        SDL_assert_release(points != NULL);
        SDL_memcpy(points, buf + offset, sizeof(b2Vec2) * count);
        offset += sizeof(b2Vec2) * count;
        newlines[ix].len = count;
        newlines[ix].points = points;
        lines_store.size += 1;
    }

    if (offset + 8 > tot_size) {
        goto end;
    }

    uint64_t poly_entries = *(uint64_t*)(buf + offset);
    if (poly_entries >= 0x1000000) {
        goto fail;
    }
    offset += 8;
    LIST_RESERVE(newpolys, polys_store, poly_entries);

    for (uint64_t ix = 0; ix < poly_entries; ++ix) {
        if (offset + 8 > tot_size) {
            goto fail;
        }
        uint64_t count = *(uint64_t*)(buf + offset);
        offset += 8;
        if (offset + sizeof(b2Vec2) * count > tot_size) {
            goto fail;
        }
        b2Vec2* points = SDL_malloc(count * sizeof(b2Vec2));
        SDL_assert_release(points != NULL);
        SDL_memcpy(points, buf + offset, sizeof(b2Vec2) * count);
        offset += sizeof(b2Vec2) * count;
        newpolys[ix].count = count;
        newpolys[ix].points = points;
        polys_store.size += 1;
    }
end:
    SDL_aligned_free(buf);

    data->lines = newlines;
    data->lines_store = lines_store;

    data->polys = newpolys;
    data->polys_store = polys_store;

    return;
fail:
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Invalid world file");
    SDL_aligned_free(buf);
    for (uint32_t i = 0; i < lines_store.size; ++i) {
        SDL_free(newlines[i].points);
    }
    LIST_FREE(newlines, lines_store);
    for (uint32_t i = 0; i < polys_store.size; ++i) {
        SDL_free(newpolys[i].points);
    }
    LIST_FREE(newpolys, polys_store);
}

void write_world(const WorldData* data) {
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Writing world.dat...");
    SDL_IOStream* io = SDL_IOFromFile("world.dat.tmp", "wb");
    if (io == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed saving world: %s",
                     SDL_GetError());
        return;
    }
    uint64_t line_count = data->lines_store.size;
    LineSet* lines = data->lines;
    uint64_t poly_count = data->polys_store.size;
    Polygon* polys = data->polys;

    uint64_t tot_size = 2 * sizeof(uint64_t);
    for (uint64_t ix = 0; ix < line_count; ++ix) {
        tot_size += sizeof(uint64_t) + lines[ix].len * sizeof(b2Vec2);
    }
    for (uint64_t ix = 0; ix < poly_count; ++ix) {
        tot_size += sizeof(uint64_t) + polys[ix].count * sizeof(b2Vec2);
    }

    if (SDL_WriteIO(io, &tot_size, sizeof(uint64_t)) < sizeof(uint64_t)) {
        goto fail;
    }

    if (SDL_WriteIO(io, &line_count, sizeof(uint64_t)) < sizeof(uint64_t)) {
        goto fail;
    }

    for (uint64_t ix = 0; ix < line_count; ++ix) {
        uint64_t count = lines[ix].len;
        if (SDL_WriteIO(io, &count, sizeof(uint64_t)) < sizeof(uint64_t)) {
            goto fail;
        }
        uint64_t size = count * sizeof(b2Vec2);
        if (SDL_WriteIO(io, lines[ix].points, size) < size) {
            goto fail;
        }
    }

    if (SDL_WriteIO(io, &poly_count, sizeof(uint64_t)) < sizeof(uint64_t)) {
        goto fail;
    }

    for (uint64_t ix = 0; ix < poly_count; ++ix) {
        uint64_t count = polys[ix].count;
        if (SDL_WriteIO(io, &count, sizeof(uint64_t)) < sizeof(uint64_t)) {
            goto fail;
        }
        uint64_t size = count * sizeof(b2Vec2);
        if (SDL_WriteIO(io, polys[ix].points, size) < size) {
            goto fail;
        }
    }

    SDL_CloseIO(io);

    if (!SDL_RenamePath("world.dat.tmp", "world.dat")) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed saving world: %s",
                     SDL_GetError());
        SDL_PathInfo i;
        if (SDL_GetPathInfo("world.dat.tmp", &i)) {
            SDL_RemovePath("world.dat.tmp");
        }
        return;
    }

    return;
fail:
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed saving world: %s", 
                 SDL_GetError());
    SDL_CloseIO(io);
    SDL_RemovePath("world.dat.tmp");
}
