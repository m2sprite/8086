typedef float  f32;
typedef double f64;

typedef int8_t  s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef s32 b32;

#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))
#define SHIFT_ARGS( ArgCount , Args ) (assert((ArgCount)>0), (ArgCount)--, *(Args)++)

