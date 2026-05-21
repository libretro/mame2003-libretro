#ifndef _H_YM2413_
#define _H_YM2413_

/* select output bits size of output : 8 or 16 */
#define SAMPLE_BITS 16

/* Fall back to exact-width stdint types only if the canonical osd_cpu.h has
   not already been included. Do NOT redefine its OSD_CPU_H guard here: doing
   so would suppress the real header (and its PAIR/endianness definitions) if
   this file were ever included first. */
#ifndef OSD_CPU_H
#include <stdint.h>
typedef uint8_t  UINT8;
typedef uint16_t UINT16;
typedef uint32_t UINT32;
typedef int8_t   INT8;
typedef int16_t  INT16;
typedef int32_t  INT32;
#endif

#if (SAMPLE_BITS==16)
typedef INT16 SAMP;
#endif
#if (SAMPLE_BITS==8)
typedef INT8 SAMP;
#endif




int  YM2413Init(int num, int clock, int rate);
void YM2413Shutdown(void);
void YM2413ResetChip(int which);
void YM2413Write(int which, int a, int v);
unsigned char YM2413Read(int which, int a);
void YM2413UpdateOne(int which, INT16 **buffers, int length);

typedef void (*OPLL_UPDATEHANDLER)(int param,int min_interval_us);

void YM2413SetUpdateHandler(int which, OPLL_UPDATEHANDLER UpdateHandler, int param);


#endif /*_H_YM2413_*/
