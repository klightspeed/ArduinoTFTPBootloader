#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>

union twobyte {
    uint8_t byte[2];
    uint16_t word;
};

union fourbyte {
    uint8_t byte[4];
    uint16_t word[2];
    uint32_t dword;
};

#define BIGENDWORDBYTES(x) { ((x) >> 8) & 0xFF, (x) & 0xFF }

#define BUILD_BUG_ON_ZERO(e) (sizeof(struct { int: -!!(e); }))
#define BUILD_BUG_ON(e) do { struct {int: -!!(e); } x __attribute__((__unused__)); } while (0)

#define member_expression(type,member) (((type *)0)->member)
#define member_offset(member) ((unsigned int) &(member))

#define SWAP8(x,y) \
    do { \
        uint8_t tmp = x; \
        x = y; \
        y = tmp; \
    } while (0)

#define BYTESWAP(x) SWAP8(*((uint8_t *)(x)), *((uint8_t *)(x)+1))

#define getregbyte(r) \
    ({ \
        register uint8_t __retval asm(#r); \
        asm volatile ("" : "=r" (__retval)); \
        __retval; \
    })

#define getregword(rlo,rhi) \
    ({ \
        register uint8_t __retvallo asm(#rlo); \
        register uint8_t __retvalhi asm(#rhi); \
        union twobyte __retval; \
        asm volatile ("" : "=r" (__retvallo), "=r" (__retvalhi)); \
        __retval.byte[0] = __retvallo; \
        __retval.byte[1] = __retvalhi; \
        __retval.word; \
    })

#define setregbyte(r,v) \
    do { \
        register uint8_t __arg asm(#r) = (v); \
        asm volatile ("" : "+r" (__arg)); \
        v = __arg; \
    } while(0)

#define setregword(rlo,rhi,v) \
    do { \
        register uint8_t __arglo asm(#rlo); \
        register uint8_t __arghi asm(#rhi); \
        union twobyte __arg; \
        __arg.word = v; \
        asm volatile ("" : "+r" (__arglo), "=r" (__arghi)); \
        __arg.byte[0] = __arglo; \
        __arg.byte[1] = __arghi; \
        v = __arg.word; \
    } while(0)

#define callfunc(f) asm volatile ("call " #f)
#define push(v) asm volatile ("push %0" : "+r" (v))
#define pop(v) asm volatile ("pop %0" : "=r" (v))

#define MAKEWORD(l,h) \
    ({ \
        union twobyte __val; \
        __val.byte[0] = (l); \
        __val.byte[1] = (h); \
        __val.word; \
    })

#define LO8(v) \
    ({ \
        union twobyte __val; \
        __val.word = (v); \
        __val.byte[0]; \
    })

#define HI8(v) \
    ({ \
        union twobyte __val; \
        __val.word = (v); \
        __val.byte[1]; \
    })

#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)

#define write_x(x,v) \
    do { \
        uint8_t __tmpreg = (v); \
        asm volatile ("st X+, %[val]" : "+x" (x) : [val] "d" (__tmpreg)); \
    } while (0)

#define write_rx(x,v,r) \
    do { \
        register uint8_t __tmpreg asm(#r) = (v); \
        asm volatile ("st X+, %[val]" : "+x" (x) : [val] "d" (__tmpreg)); \
    } while (0)

#define write_zx_rel(x,z,o) asm volatile ("ldd __tmp_reg__, Z + %[ofs]\r\nst X+, __tmp_reg__" : "+x" (x) : "z" (z), [ofs] "I" (o))

#define write_zero_x(x) asm volatile ("st X+, __zero_reg__" : "+x" (x))

#define compare_const_zx(x,z,len) \
    ({ \
        void *__x = (void *)(x); \
        const void *__z = (const void *)(z); \
        register uint8_t __len asm("r23") = (len) + 1; \
        register uint8_t __res asm("r21"); \
        asm volatile ( \
            "call __compare_const_zx" \
            : "+x" (__x), "+z" (__z), "+r" (__len), "=r" (__res) \
        ); \
        __res; \
    })

#define compare_const_zx_str(x,z,len) \
    ({ \
        void *__x = (void *)(x); \
        const void *__z = (const void *)(z); \
        register uint8_t __len asm("r23") = (len) + 1; \
        register uint8_t __res asm("r21"); \
        asm volatile ( \
            "call __compare_const_zx_str" \
            : "+x" (__x), "+z" (__z), "+r" (__len), "=r" (__res) \
        ); \
        __res; \
    })

#define compare_zx(x,z,len) \
    ({ \
        void *__x = (void *)(x); \
        void *__z = (void *)(z); \
        register uint8_t __len asm("r23") = (len) + 1; \
        register uint8_t __res asm("r21"); \
        asm volatile ( \
            "call __compare_zx" \
            : "+x" (__x), "+z" (__z), "+r" (__len), "=r" (__res) \
        ); \
        __res; \
    })

#define compare_zx_str(x,z,len) \
    ({ \
        void *__x = (void *)(x); \
        void *__z = (void *)(z); \
        register uint8_t __len asm("r23") = (len) + 1; \
        register uint8_t __res asm("r21"); \
        asm volatile ( \
            "call __compare_zx_str" \
            : "+x" (__x), "+z" (__z), "+r" (__len), "=r" (__res) \
        ); \
        __res; \
    })

#define copy_const_zx_constz(x,z,len) \
    do { \
        const void *__z = (const void *)(z); \
        register uint8_t __len asm("r23") = (len) + 1; \
        asm volatile ( \
            "call __copy_const_zx" \
            : "+x" (x), "+z" (__z), "+r" (__len) \
        ); \
    } while (0)

#define copy_const_zx_str_constz(x,z,len) \
    do { \
        const void *__z = (const void *)(z); \
        register uint8_t __len asm("r23") = (len) + 1; \
        asm volatile ( \
            "call __copy_const_zx_str" \
            : "+x" (x), "+z" (__z), "+r" (__len) \
        ); \
    } while (0)

#define copy_zx_constz(x,z,len) \
    do { \
        void *__z = (void *)(z); \
        register uint8_t __len asm("r23") = (len) + 1; \
        asm volatile ( \
            "call __copy_zx" \
            : "+x" (x), "+z" (__z), "+r" (__len) \
        ); \
    } while (0)

#define copy_zx_str_constz(x,z,len) \
    do { \
        void *__z = (void *)(z); \
        register uint8_t __len asm("r23") = (len) + 1; \
        asm volatile ( \
            "call __copy_zx_str" \
            : "+x" (x), "+z" (__z), "+r" (__len) \
        ); \
    } while (0)

#define copy_const_zx_const(x,z,len) \
    do { \
        void *__x = (void *)x; \
        const void *__z = (const void *)(z); \
        register uint8_t __len asm("r23") = (len) + 1; \
        asm volatile ( \
            "call __copy_const_zx" \
            : "+x" (__x), "+z" (__z), "+r" (__len) \
        ); \
    } while (0)

#define copy_const_zx_str_const(x,z,len) \
    do { \
        void *__x = (void *)x; \
        const void *__z = (const void *)(z); \
        register uint8_t __len asm("r23") = (len) + 1; \
        asm volatile ( \
            "call __copy_const_zx_str" \
            : "+x" (__x), "+z" (__z), "+r" (__len) \
        ); \
    } while (0)

#define copy_zx_const(x,z,len) \
    do { \
        void *__x = (void *)x; \
        void *__z = (void *)z; \
        register uint8_t __len asm("r23") = (len) + 1; \
        asm volatile ( \
            "call __copy_zx" \
            : "+x" (__x), "+z" (__z), "+r" (__len) \
        ); \
    } while (0)

#define copy_zx_str_const(x,z,len) \
    do { \
        void *__x = (void *)x; \
        void *__z = (void *)z; \
        register uint8_t __len asm("r23") = (len) + 1; \
        asm volatile ( \
            "call __copy_zx_str" \
            : "+x" (__x), "+z" (__z), "+r" (__len) \
        ); \
    } while (0)

#define zero_x_const(x,len) \
    do { \
        void *__x = (void *)x; \
        register uint8_t __len asm("r23") = (len) + 1; \
        asm volatile ( \
            "call __zero_x" \
            : "+x" (__x), "+r" (__len) \
        ); \
    } while (0)

#define copy_const_zx(x,z,len) \
    do { \
        register uint8_t __len asm("r23") = (len) + 1; \
        asm volatile ( \
            "call __copy_const_zx" \
            : "+x" (x), "+z" ((const void *)(z)), "+r" (__len) \
        ); \
    } while (0)

#define copy_const_zx_str(x,z,len) \
    do { \
        register uint8_t __len asm("r23") = (len) + 1; \
        asm volatile ( \
            "call __copy_const_zx_str" \
            : "+x" (x), "+z" ((const void *)(z)), "+r" (__len) \
        ); \
    } while (0)

#define copy_zx(x,z,len) \
    do { \
        register uint8_t __len asm("r23") = (len) + 1; \
        asm volatile ( \
            "call __copy_zx" \
            : "+x" (x), "+z" (z), "+r" (__len) \
        ); \
    } while (0)

#define copy_zx_str(x,z,len) \
    do { \
        register uint8_t __len asm("r23") = (len) + 1; \
        asm volatile ( \
            "call __copy_zx_str" \
            : "+x" (x), "+z" (z), "+r" (__len) \
        ); \
    } while (0)

#define zero_x(x,len) \
    do { \
        register uint8_t __len asm("r23") = (len) + 1; \
        asm volatile ( \
            "call __zero_x" \
            : "+x" (x), "+r" (__len) \
        ); \
    } while (0)

#define read_x(x) \
    ({ \
        uint8_t __retval; \
        asm volatile ("ld %[val], X+" : [val] "=r" (__retval), "+x" (optptr)); \
        __retval; \
    })

#define read_rx(x,r) \
    ({ \
        register uint8_t __retval asm(#r); \
        asm volatile ("ld %[val], X+" : [val] "=r" (__retval), "+x" (optptr)); \
        __retval; \
    })

#define copy_xz_constz(x,z,len) \
    do { \
            void *__z = (void *)(z); \
        register uint8_t __len asm("r23") = (len) + 1; \
        asm volatile ( \
            "call __copy_xz" \
            : "+x" (x), "+z" (__z), "+r" (__len) \
        ); \
    } while (0)

#define copy_xz_str_constz(x,z,len) \
    do { \
            void *__z = (void *)(z); \
        register uint8_t __len asm("r23") = (len) + 1; \
        asm volatile ( \
            "call __copy_xz_str" \
            : "+x" (x), "+z" (__z), "+r" (__len) \
        ); \
    } while (0)

#define copy_xz(x,z,len) \
    do { \
        register uint8_t __len asm("r23") = (len) + 1; \
        asm volatile ( \
            "call __copy_xz" \
            : "+x" (x), "+z" (z), "+r" (__len) \
        ); \
    } while (0)

#define copy_xz_str(x,z,len) \
    do { \
        register uint8_t __len asm("r23") = (len) + 1; \
        asm volatile ( \
            "call __copy_xz_str" \
            : "+x" (x), "+z" (z), "+r" (__len) \
        ); \
    } while (0)

#endif
