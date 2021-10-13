#ifndef VEC_H_
#define VEC_H_

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct vec
{
    void* buf;
    size_t size;
    size_t reserved;
    size_t obj_size;
} vec;

static inline void vec_reserve(vec* vec, size_t reserve)
{
    if (vec->reserved >= reserve)
        return;

    vec->reserved = reserve;
    vec->buf = realloc(vec->buf, vec->obj_size*vec->reserved);
}

static inline void vec_init(vec* vec, size_t obj_size)
{
    vec->size = 0;
    vec->reserved = 16;
    vec->obj_size = obj_size;

    vec->buf = malloc(obj_size * vec->reserved);
}

static inline void vec_destroy(vec* vec)
{
    free(vec->buf);
}

static inline void* vec_get(vec* vec, size_t i)
{
    return (char*)vec->buf + i*vec->obj_size;
}

static inline void vec_push_back(vec* vec, void* obj)
{
    if (vec->size + 1 > vec->reserved)
        vec_reserve(vec, vec->reserved * 2);

    memcpy(vec_get(vec, vec->size), obj, vec->obj_size);
    vec->size++;
}

static inline void vec_push_back_uninitialized(vec* vec)
{
    if (vec->size + 1 > vec->reserved)
        vec_reserve(vec, vec->reserved * 2);

    vec->size++;
}

static inline void vec_qsort(vec* vec, int (*cmpfunc)(const void*, const void*))
{
    qsort(vec->buf, vec->size, vec->obj_size, cmpfunc);
}

static inline void* vec_binary_range(vec* vec, const void* obj,
                                     long l, long r,
                                     int (*cmpfunc)(const void*, const void*))
{
    while (l <= r) {
        size_t m = l + (r - l) / 2;

        int cmp = cmpfunc(vec_get(vec, m), obj);
        // Check if x is present at mid
        if (cmp == 0)
            return vec_get(vec, m);

        // If x greater, ignore left half
        if (cmp < 0)
            l = m + 1;

        // If x is smaller, ignore right half
        else
            r = m - 1;
    }

    // if we reach here, then element was
    // not present
    return NULL;
}

static inline void* vec_binary(vec* vec, const void* obj,
                               int (*cmpfunc)(const void*, const void*))
{
    long l = 0;
    long r = vec->size;
    return vec_binary_range(vec, obj, l, r, cmpfunc);
}

static inline size_t vec_distance(vec* vec, void* begin, void* end)
{
    return ((char*)end - (char*)begin) / vec->obj_size;
}

static inline void* vec_next(vec* vec, void* it)
{
    return vec_get(vec, vec_distance(vec, vec->buf, it) + 1);
}

static inline void vec_print(vec* vec, const char* fmt)
{
    for (size_t i = 0; i < vec->size; i++)
    {
        printf(fmt, vec_get(vec, i));
    }
}

static inline void vec_unique(vec* vec, int (*cmpfunc)(const void*, const void*))
{
    void* first = vec->buf;
    void* last = vec_get(vec, vec->size - 1);

    if (first == last)
    {
        return;
    }

    void* result = first;
    first = vec_next(vec, first);
    while (first != last)
    {
        int cmp = cmpfunc(result, first);

        if (cmp != 0)
        {
            result = vec_next(vec, result);
            if(result != first)
            {
                memcpy(result, first, vec->obj_size);
            }
        }
        first = vec_next(vec, first);
    }
    result = vec_next(vec, result);
    vec->size = vec_distance(vec, vec->buf, result);
}

#endif // VEC_H_
