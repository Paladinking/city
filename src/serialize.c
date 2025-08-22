#include "serialize.h"
#include <SDL3/SDL.h>

void read_world(LineSet** lines, Storage* storage) {
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "reading world.dat...");
    SDL_IOStream* io = SDL_IOFromFile("world.dat", "rb");
    Storage newstore;
    LineSet* newlines;
    LIST_CREATE(newlines, newstore, LineSet);

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
    if (tot_size >= 0x10000000) {
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

    uint64_t entries = *(uint64_t*)(buf);
    if (entries >= 0x1000000) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Too many lines in world");
        SDL_aligned_free(buf);
        return;
    }

    uint64_t offset = 8;
    LIST_RESERVE(newlines, newstore, entries);

    for (uint64_t ix = 0; ix < entries; ++ix) {
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
        newstore.size += 1;
    }

    SDL_aligned_free(buf);

    for (uint32_t ix = 0; ix < storage->size; ++ix) {
        SDL_free((*lines)[ix].points);
    }

    LIST_FREE(*lines, *storage);
    *lines = newlines;
    *storage = newstore;

    return;
fail:
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Invalid world file");
    SDL_aligned_free(buf);
    for (uint32_t i = 0; i < newstore.size; ++i) {
        SDL_free(newlines[i].points);
    }
}

void write_world(LineSet* lines, uint32_t line_count) {
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Writing world.dat...");
    SDL_IOStream* io = SDL_IOFromFile("world.dat.tmp", "wb");
    if (io == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed saving world: %s",
                     SDL_GetError());
        return;
    }
    uint64_t tot_size = sizeof(uint64_t);
    for (uint64_t ix = 0; ix < line_count; ++ix) {
        tot_size += sizeof(uint64_t) + lines[ix].len * sizeof(b2Vec2);
    }

    if (SDL_WriteIO(io, &tot_size, sizeof(uint64_t)) < sizeof(uint64_t)) {
        goto fail;
    }

    uint64_t tot_count = line_count;
    if (SDL_WriteIO(io, &tot_count, sizeof(uint64_t)) < sizeof(uint64_t)) {
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
