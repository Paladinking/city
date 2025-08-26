#ifndef LIST_H_00
#define LIST_H_00

#include <stdint.h>

typedef struct Storage {
    uint32_t size;
    uint32_t capacity;
} Storage;

#define LIST_APPEND(lst, storage, item) do {                                \
    if ((storage).size == (storage).capacity) {                             \
        void* list_p = SDL_realloc((lst), (storage).size * 2 * sizeof(item));/*NOLINT(bugprone-sizeof-expression)*/ \
        SDL_assert_release(list_p != NULL);                                 \
        (lst) = list_p;                                                     \
        (storage).capacity *= 2;                                            \
    }                                                                       \
    (lst)[(storage).size++] = (item);                                       \
} while (0)

#define LIST_CREATE(lst, storage, T) do {                                   \
    (lst) = SDL_malloc(8 * sizeof(T));                                      \
    SDL_assert_release((lst) != NULL);                                      \
    (storage).size = 0;                                                     \
    (storage).capacity = 8;                                                 \
} while (0)

#define LIST_RESERVE(lst, storage, count) if ((storage).capacity < (count)) { \
    void* list_p = SDL_realloc((lst), (count) * sizeof(*lst)); /*NOLINT(bugprone-sizeof-expression)*/ \
    SDL_assert_release(list_p != NULL);                                       \
    (lst) = list_p;                                                           \
    (storage).capacity = (count);                                             \
}

#define LIST_RESERVE_GROWTH(lst, storage, count) LIST_RESERVE(lst, storage, (storage).size + (count))

#define LIST_FREE(lst, storage) do {                                        \
    SDL_free(lst);                                                          \
    (lst) = NULL;                                                           \
    (storage).size = 0;                                                     \
    (storage).capacity = 0;                                                 \
} while (0)

#endif
