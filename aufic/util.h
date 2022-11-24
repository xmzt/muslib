#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>
#include <stdalign.h>

#define parentof(ptr, type, member) ((type*)((char*)(ptr) - offsetof(type, member)))

static inline size_t align_size_t(size_t x, size_t align) {
	return (x + (align-1)) & ~(align-1);
}

#endif
