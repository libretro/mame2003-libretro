#ifndef YMF262_H
#define YMF262_H


#define BUILD_YMF262 (HAS_YMF262)


/* select number of output bits: 8 or 16 */
#define OPL3_SAMPLE_BITS 16

#include <stdint.h>



typedef void (*OPL3_TIMERHANDLER)(int channel,double interval_Sec);
typedef void (*OPL3_IRQHANDLER)(int param,int irq);
typedef void (*OPL3_UPDATEHANDLER)(int param,int min_interval_us);



#if BUILD_YMF262

int  YMF262Init(int num, int clock, int rate);
void YMF262Shutdown(void);
void YMF262ResetChip(int which);
int  YMF262Write(int which, int a, int v);
unsigned char YMF262Read(int which, int a);
int  YMF262TimerOver(int which, int c);
void YMF262UpdateOne(int which, int16_t **buffers, int length);

void YMF262SetTimerHandler(int which, OPL3_TIMERHANDLER TimerHandler, int channelOffset);
void YMF262SetIRQHandler(int which, OPL3_IRQHANDLER IRQHandler, int param);
void YMF262SetUpdateHandler(int which, OPL3_UPDATEHANDLER UpdateHandler, int param);

#endif


#endif /* YMF262_H */
