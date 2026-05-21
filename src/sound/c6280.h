#ifndef _C6280_H_
#define _C6280_H_

#define MAX_C6280 2

struct C6280_interface
{
    int num;
    int volume[MAX_C6280];
    int clock[MAX_C6280];
};

typedef struct {
    uint16_t frequency;
    uint8_t control;
    uint8_t balance;
    uint8_t waveform[32];
    uint8_t index;
    int16_t dda;
    uint8_t noise_control;
    uint32_t noise_counter;
    uint32_t counter;
} t_channel;

typedef struct {
    uint8_t select;
    uint8_t balance;
    uint8_t lfo_frequency;
    uint8_t lfo_control;
    t_channel channel[8];
    int16_t volume_table[32];
    uint32_t noise_freq_tab[32];
    uint32_t wave_freq_tab[4096];
} c6280_t;

/* Function prototypes */
int c6280_sh_start(const struct MachineSound *msound);
void c6280_sh_stop(void);
WRITE_HANDLER( C6280_0_w );
WRITE_HANDLER( C6280_1_w );

#endif /* _C6280_H_ */
