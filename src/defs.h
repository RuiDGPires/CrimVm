#ifndef __DEFS_H__
#define __DEFS_H__

#define NNULL __attribute__((nonnull))
#define NNULL2(a, b) __attribute__((nonnull(a, b)))

#include <stdint.h>
typedef int32_t i32;
typedef uint32_t u32;
typedef uint8_t u8;

// Smallest size possible for bool
typedef uint8_t bool;

#define TRUE 1
#define FALSE 0

#endif
