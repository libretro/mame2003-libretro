/*******************************************************************************
*																			   *
*	Define size independent data types and operations.						   *
*																			   *
*   The following types must be supported by all platforms:					   *
*																			   *
*	uint8_t  - Unsigned 8-bit Integer		int8_t  - Signed 8-bit integer           *
*	uint16_t - Unsigned 16-bit Integer	int16_t - Signed 16-bit integer          *
*	uint32_t - Unsigned 32-bit Integer	int32_t - Signed 32-bit integer          *
*	uint64_t - Unsigned 64-bit Integer	int64_t - Signed 64-bit integer          *
*																			   *
*																			   *
*   The macro names for the artithmatic operations are composed as follows:    *
*																			   *
*   XXX_R_A_B, where XXX - 3 letter operation code (ADD, SUB, etc.)			   *
*					 R   - The type	of the result							   *
*					 A   - The type of operand 1							   *
*			         B   - The type of operand 2 (if binary operation)		   *
*																			   *
*				     Each type is one of: U8,8,U16,16,U32,32,U64,64			   *
*																			   *
*******************************************************************************/


#ifndef OSD_CPU_H
#define OSD_CPU_H

#include <stdint.h>
#include <libretro.h>
#include <retro_inline.h>
#include <stdio.h>

#define ARRAY_LENGTH(x) (sizeof(x)/sizeof(x[0]))

/* ---------------------------------------------------------------------------
 * Endianness: MSB_FIRST is the single source of truth.
 *   defined   => big-endian host
 *   undefined => little-endian host
 * All endian-dependent code keys off MSB_FIRST at compile time; there are no
 * runtime byte-order checks and LSB_FIRST is never consulted. The build system
 * may pin MSB_FIRST for known big-endian targets; if it did not, derive it from
 * the compiler's byte-order macros here so this header alone decides.
 * --------------------------------------------------------------------------- */
#if !defined(MSB_FIRST)
#  if (defined(__BYTE_ORDER__) && defined(__ORDER_BIG_ENDIAN__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__) \
   || defined(__BIG_ENDIAN__) || defined(__ARMEB__) || defined(__MIPSEB__) || defined(__sparc__)
#    define MSB_FIRST 1
#  endif
   /* otherwise the host is little-endian and MSB_FIRST stays undefined */
#endif

#if 0
#define DEBUG_LOG 1
#endif

#ifdef __MWERKS__
#define __extension__
#endif

#ifdef _MSC_VER
#include <compat/msvc.h>

#ifndef S_ISDIR
#define S_ISDIR(mode)  (((mode) & S_IFMT) == S_IFDIR)
#endif

#ifndef S_ISREG
#define S_ISREG(mode)  (((mode) & S_IFMT) == S_IFREG)
#endif
#else
#if !defined(__PS3__)
#include <strings.h>
#endif
#endif

extern struct retro_perf_callback perf_cb;

/* Combine two 32-bit integers into a 64-bit integer */
#define COMBINE_64_32_32(A,B)     ((((uint64_t)(A))<<32) | (uint32_t)(B))
#define COMBINE_U64_U32_U32(A,B)  COMBINE_64_32_32(A,B)

/* Return upper 32 bits of a 64-bit integer */
#define HI32_32_64(A)		  (((uint64_t)(A)) >> 32)
#define HI32_U32_U64(A)		  HI32_32_64(A)

/* Return lower 32 bits of a 64-bit integer */
#define LO32_32_64(A)		  ((A) & 0xffffffff)
#define LO32_U32_U64(A)		  LO32_32_64(A)

#define DIV_64_64_32(A,B)	  ((A)/(B))
#define DIV_U64_U64_U32(A,B)  ((A)/(uint32_t)(B))

#define MOD_32_64_32(A,B)	  ((A)%(B))
#define MOD_U32_U64_U32(A,B)  ((A)%(uint32_t)(B))

#define MUL_64_32_32(A,B)	  ((A)*(int64_t)(B))
#define MUL_U64_U32_U32(A,B)  ((A)*(uint64_t)(uint32_t)(B))


/******************************************************************************
 * Union of uint8_t, uint16_t and uint32_t in native endianess of the target
 * This is used to access bytes and words in a machine independent manner.
 * The upper bytes h2 and h3 normally contain zero (16 bit CPU cores)
 * thus PAIR.d can be used to pass arguments to the memory system
 * which expects 'int' really.
 ******************************************************************************/
typedef union {
#ifdef MSB_FIRST
	struct { uint8_t h3,h2,h,l; } b;
	struct { uint16_t h,l; } w;
#else
	struct { uint8_t l,h,h2,h3; } b;
	struct { uint16_t l,h; } w;
#endif
	uint32_t d;
}	PAIR;

#endif	/* defined OSD_CPU_H */
