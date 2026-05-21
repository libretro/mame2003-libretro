/**********************************************************************************************
 *
 *   Data East BSMT2000 driver
 *   by Aaron Giles
 *
 **********************************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "driver.h"



/**********************************************************************************************

     CONSTANTS

***********************************************************************************************/

#define BACKEND_INTERPOLATE		1
#define LOG_COMMANDS			0
#define MAKE_WAVS				0

#if MAKE_WAVS
#include "wavwrite.h"
#endif


#define MAX_SAMPLE_CHUNK		10000

#define FRAC_BITS				14
#define FRAC_ONE				(1 << FRAC_BITS)
#define FRAC_MASK				(FRAC_ONE - 1)

#define REG_CURRPOS				0
#define REG_UNKNOWN1			1
#define REG_RATE				2
#define REG_LOOPEND				3
#define REG_LOOPSTART			4
#define REG_BANK				5
#define REG_RIGHTVOL			6
#define REG_LEFTVOL				7
#define REG_TOTAL				8

#define REG_ALT_RIGHTVOL		8



/**********************************************************************************************

     INTERNAL DATA STRUCTURES

***********************************************************************************************/

/* struct describing a single playing voice */
struct BSMT2000Voice
{
	/* external state */
	uint16_t		reg[REG_TOTAL];			/* 9 registers */
	uint32_t		position;				/* current position */
	uint32_t		loop_start_position;	/* loop start position */
	uint32_t		loop_stop_position;		/* loop stop position */
	uint32_t		adjusted_rate;			/* adjusted rate */
};

struct BSMT2000Chip
{
	int			stream;					/* which stream are we using */
	int8_t *		region_base;			/* pointer to the base of the region */
	int			total_banks;			/* number of total banks in the region */
	int			voices;					/* number of voices */
	double 		master_clock;			/* master clock frequency */

	int32_t		output_step;			/* step value for frequency conversion */
	int32_t		output_pos;				/* current fractional position */
	int32_t		last_lsample;			/* last sample output */
	int32_t		last_rsample;			/* last sample output */
	int32_t		curr_lsample;			/* current sample target */
	int32_t		curr_rsample;			/* current sample target */

	struct BSMT2000Voice *voice;		/* the voices */
	struct BSMT2000Voice compressed;	/* the compressed voice */
	
#if MAKE_WAVS
	void *		wavraw;					/* raw waveform */
	void *		wavresample;			/* resampled waveform */
#endif
};



/**********************************************************************************************

     GLOBALS

***********************************************************************************************/

static struct BSMT2000Chip bsmt2000[MAX_BSMT2000];
static int32_t *accumulator;
static int32_t *scratch;



/**********************************************************************************************

     interpolate
     backend_interpolate -- interpolate between two samples

***********************************************************************************************/

#define interpolate(sample1, sample2, accum)										\
		(sample1 * (int32_t)(0x10000 - (accum & 0xffff)) + 							\
		 sample2 * (int32_t)(accum & 0xffff)) >> 16;

#define interpolate2(sample1, sample2, accum)										\
		(sample1 * (int32_t)(0x8000 - (accum & 0x7fff)) + 							\
		 sample2 * (int32_t)(accum & 0x7fff)) >> 15;

#if BACKEND_INTERPOLATE
#define backend_interpolate(sample1, sample2, position)								\
		(sample1 * (int32_t)(FRAC_ONE - position) + 									\
		 sample2 * (int32_t)position) >> FRAC_BITS;
#else
#define backend_interpolate(sample1, sample2, position)	sample1
#endif



/**********************************************************************************************

     generate_samples -- generate samples for all voices at the chip's frequency

***********************************************************************************************/

static void generate_samples(struct BSMT2000Chip *chip, int32_t *left, int32_t *right, int samples)
{
	struct BSMT2000Voice *voice;
	int v;

	/* skip if nothing to do */
	if (!samples)
		return;

	/* clear out the accumulator */
	memset(left, 0, samples * sizeof(left[0]));
	memset(right, 0, samples * sizeof(right[0]));

	/* loop over voices */
	for (v = 0; v < chip->voices; v++)
	{
		voice = &chip->voice[v];
		
		/* compute the region base */
		if (voice->reg[REG_BANK] < chip->total_banks)
		{
			int8_t *base = &chip->region_base[voice->reg[REG_BANK] * 0x10000];
			int32_t *lbuffer = left, *rbuffer = right;
			uint32_t rate = voice->adjusted_rate;
			uint32_t pos = voice->position;
			int32_t lvol = voice->reg[REG_LEFTVOL];
			int32_t rvol = voice->reg[REG_RIGHTVOL];
			int remaining = samples;

			/* loop while we still have samples to generate */
			while (remaining--)
			{
				/* fetch two samples */
				int32_t val1 = base[pos >> 16];
				int32_t val2 = base[(pos >> 16) + 1];
				pos += rate;

				/* interpolate */
				val1 = interpolate(val1, val2, pos);

				/* apply volumes and add */
				*lbuffer++ += val1 * lvol;
				*rbuffer++ += val1 * rvol;

				/* check for loop end */
				if (pos >= voice->loop_stop_position)
					pos += voice->loop_start_position - voice->loop_stop_position;
			}

			/* update the position */
			voice->position = pos;
		}
	}

	/* compressed voice (11-voice model only) */
	voice = &chip->compressed;
	if (chip->voices == 11 && voice->reg[REG_BANK] < chip->total_banks)
	{
		int8_t *base = &chip->region_base[voice->reg[REG_BANK] * 0x10000];
		int32_t *lbuffer = left, *rbuffer = right;
		uint32_t rate = voice->adjusted_rate;
		uint32_t pos = voice->position;
		int32_t lvol = voice->reg[REG_LEFTVOL];
		int32_t rvol = voice->reg[REG_RIGHTVOL];
		int remaining = samples;

		/* loop while we still have samples to generate */
		while (remaining-- && pos < voice->loop_stop_position)
		{
			/* fetch two samples -- note: this is wrong, just a guess!!!*/
			int32_t val1 = (int8_t)((base[pos >> 16] << ((pos >> 13) & 4)) & 0xf0);
			int32_t val2 = (int8_t)((base[(pos + 0x8000) >> 16] << (((pos + 0x8000) >> 13) & 4)) & 0xf0);
			pos += rate;

			/* interpolate */
			val1 = interpolate2(val1, val2, pos);

			/* apply volumes and add */
			*lbuffer++ += val1 * lvol;
			*rbuffer++ += val1 * rvol;
		}

		/* update the position */
		voice->position = pos;
	}
}



/**********************************************************************************************

     bsmt2000_update -- update the sound chip so that it is in sync with CPU execution

***********************************************************************************************/

static void bsmt2000_update(int num, int16_t **buffer, int length)
{
	struct BSMT2000Chip *chip = &bsmt2000[num];
	int32_t *lsrc = scratch, *rsrc = scratch;
	int32_t lprev = chip->last_lsample;
	int32_t rprev = chip->last_rsample;
	int32_t lcurr = chip->curr_lsample;
	int32_t rcurr = chip->curr_rsample;
	int16_t *ldest = buffer[0];
	int16_t *rdest = buffer[1];
	int32_t interp;
	int remaining = length;
	int samples_left = 0;

#if MAKE_WAVS
	/* start the logging once we have a sample rate */
	if (chip->output_step)
	{
		if (!chip->wavraw)
		{
			int sample_rate = (int)((double)Machine->sample_rate / (double)(1 << FRAC_BITS) * (double)chip->output_step);
			chip->wavraw = wav_open("raw.wav", sample_rate, 2);
		}
		if (!chip->wavresample)
			chip->wavresample = wav_open("resamp.wav", Machine->sample_rate, 2);
	}
#endif

	/* then sample-rate convert with linear interpolation */
	while (remaining > 0)
	{
		/* if we're over, grab the next samples */
		while (chip->output_pos >= FRAC_ONE)
		{
			/* do we have any samples available? */
			if (samples_left == 0)
			{
				/* compute how many new samples we need */
				uint32_t final_pos = chip->output_pos + (remaining - 1) * chip->output_step;
				samples_left = final_pos >> FRAC_BITS;
				if (samples_left > MAX_SAMPLE_CHUNK)
					samples_left = MAX_SAMPLE_CHUNK;

				/* determine left/right source data */
				lsrc = scratch;
				rsrc = scratch + samples_left;
				generate_samples(chip, lsrc, rsrc, samples_left);

#if MAKE_WAVS
				/* log the raw data */
				if (chip->wavraw)
					wav_add_data_32lr(chip->wavraw, lsrc, rsrc, samples_left, 4);
#endif
			}

			/* adjust the positions */
			chip->output_pos -= FRAC_ONE;
			lprev = lcurr;
			rprev = rcurr;

			/* fetch new samples */
			lcurr = *lsrc++ >> 9;
			rcurr = *rsrc++ >> 9;
			samples_left--;
		}

		/* interpolate between the two current samples */
		while (remaining > 0 && chip->output_pos < FRAC_ONE)
		{
			/* left channel */
			interp = backend_interpolate(lprev, lcurr, chip->output_pos);
         MAME_CLAMP_SAMPLE(interp);
			*ldest++ = interp;

			/* right channel */
			interp = backend_interpolate(rprev, rcurr, chip->output_pos);
         MAME_CLAMP_SAMPLE(interp);
			*rdest++ = interp;

			/* advance */
			chip->output_pos += chip->output_step;
			remaining--;
		}
	}

	/* remember the last samples */
	chip->last_lsample = lprev;
	chip->last_rsample = rprev;
	chip->curr_lsample = lcurr;
	chip->curr_rsample = rcurr;

#if MAKE_WAVS
	/* log the resampled data */
	if (chip->wavresample)
		wav_add_data_16lr(chip->wavresample, buffer[0], buffer[1], length);
#endif
}



/**********************************************************************************************

     BSMT2000_sh_start -- start emulation of the BSMT2000

***********************************************************************************************/

static INLINE void init_voice(struct BSMT2000Voice *voice)
{
	memset(&voice->reg, 0, sizeof(voice->reg));
	voice->position = 0;
	voice->adjusted_rate = 0;
	voice->reg[REG_LEFTVOL] = 0x7fff;
	voice->reg[REG_RIGHTVOL] = 0x7fff;
}


static INLINE void init_all_voices(struct BSMT2000Chip *chip)
 {
 	int i;
 
 	/* init the voices */
 	for (i = 0; i < chip->voices; i++)
 		init_voice(&chip->voice[i]);
 
 	/* init the compressed voice (runs at a fixed rate of ~8kHz?) */
 	init_voice(&chip->compressed);
 	chip->compressed.adjusted_rate = 0x02aa << 4;
 }
 
int BSMT2000_sh_start(const struct MachineSound *msound)
{
	const struct BSMT2000interface *intf = msound->sound_interface;
	char stream_name[2][40];
	const char *stream_name_ptrs[2];
	int vol[2];
	int i;
	
	/* initialize the chips */
	memset(&bsmt2000, 0, sizeof(bsmt2000));
	for (i = 0; i < intf->num; i++)
	{
		/* allocate the voices */
		bsmt2000[i].voices = intf->voices[i];
		bsmt2000[i].voice = malloc(bsmt2000[i].voices * sizeof(struct BSMT2000Voice));
		if (!bsmt2000[i].voice)
			return 1;

		/* generate the name and create the stream */
		sprintf(stream_name[0], "%s #%d Ch1", sound_name(msound), i);
		sprintf(stream_name[1], "%s #%d Ch2", sound_name(msound), i);
		stream_name_ptrs[0] = stream_name[0];
		stream_name_ptrs[1] = stream_name[1];

		/* set the volumes */
		vol[0] = MIXER(intf->mixing_level[i], MIXER_PAN_LEFT);
		vol[1] = MIXER(intf->mixing_level[i], MIXER_PAN_RIGHT);

		/* create the stream */
		bsmt2000[i].stream = stream_init_multi(2, stream_name_ptrs, vol, Machine->sample_rate, i, bsmt2000_update);
		if (bsmt2000[i].stream == -1)
			return 1;

		/* initialize the regions */
		bsmt2000[i].region_base = (int8_t *)memory_region(intf->region[i]);
		bsmt2000[i].total_banks = memory_region_length(intf->region[i]) / 0x10000;

		/* initialize the rest of the structure */
		bsmt2000[i].master_clock = (double)intf->baseclock[i];
		bsmt2000[i].output_step = (int)((double)intf->baseclock[i] / 1024.0 * (double)(1 << FRAC_BITS) / (double)Machine->sample_rate);

		/* init the voices */
		init_all_voices(&bsmt2000[i]);
	}

	/* allocate memory */
	accumulator = malloc(sizeof(accumulator[0]) * 2 * MAX_SAMPLE_CHUNK);
	scratch = malloc(sizeof(scratch[0]) * 2 * MAX_SAMPLE_CHUNK);
	if (!accumulator || !scratch)
		return 1;

	/* success */
	return 0;
}



/**********************************************************************************************

     BSMT2000_sh_stop -- stop emulation of the BSMT2000

***********************************************************************************************/

void BSMT2000_sh_stop(void)
{
	int i;

	/* free memory */
	if (accumulator)
		free(accumulator);
	accumulator = NULL;

	if (scratch)
		free(scratch);
	scratch = NULL;

	for (i = 0; i < MAX_BSMT2000; i++)
	{
		if (bsmt2000[i].voice)
			free(bsmt2000[i].voice);
		bsmt2000[i].voice = NULL;
		
#if MAKE_WAVS
		if (bsmt2000[i].wavraw)
			wav_close(bsmt2000[i].wavraw);
		if (bsmt2000[i].wavresample)
			wav_close(bsmt2000[i].wavresample);
#endif
	}
}



/**********************************************************************************************

     BSMT2000_sh_reset -- reset emulation of the BSMT2000

***********************************************************************************************/

void BSMT2000_sh_reset(void)
{
	int i;
	
	for (i = 0; i < MAX_BSMT2000; i++)
		init_all_voices(&bsmt2000[i]);
}



/**********************************************************************************************

     bsmt2000_reg_write -- handle a write to the selected BSMT2000 register

***********************************************************************************************/

static void bsmt2000_reg_write(struct BSMT2000Chip *chip, offs_t offset, data16_t data, data16_t mem_mask)
{
	struct BSMT2000Voice *voice = &chip->voice[offset % chip->voices];
	int regindex = offset / chip->voices;

#if LOG_COMMANDS
	logerror("BSMT#%d write: V%d R%d = %04X\n", chip - bsmt2000, offset % chip->voices, regindex, data);
#endif

	/* update the register */
	if (regindex < REG_TOTAL)
		COMBINE_DATA(&voice->reg[regindex]);

	/* force an update */
	stream_update(chip->stream, 0);
	
	/* update parameters for standard voices */
	switch (regindex)
	{
		case REG_CURRPOS:
			voice->position = voice->reg[REG_CURRPOS] << 16;
			break;

		case REG_RATE:
			voice->adjusted_rate = voice->reg[REG_RATE] << 5;
			break;

		case REG_LOOPSTART:
			voice->loop_start_position = voice->reg[REG_LOOPSTART] << 16;
			break;

		case REG_LOOPEND:
			voice->loop_stop_position = voice->reg[REG_LOOPEND] << 16;
			break;

		case REG_ALT_RIGHTVOL:
			COMBINE_DATA(&voice->reg[REG_RIGHTVOL]);
			break;
	}
	
	/* update parameters for compressed voice (11-voice model only) */
	if (chip->voices == 11 && offset >= 0x6d)
	{
		voice = &chip->compressed;
		switch (offset)
		{
			case 0x6d:
				COMBINE_DATA(&voice->reg[REG_LOOPEND]);
				voice->loop_stop_position = voice->reg[REG_LOOPEND] << 16;
				break;
				
			case 0x6f:
				COMBINE_DATA(&voice->reg[REG_BANK]);
				break;
			
			case 0x74:
				COMBINE_DATA(&voice->reg[REG_RIGHTVOL]);
				break;

			case 0x75:
				COMBINE_DATA(&voice->reg[REG_CURRPOS]);
				voice->position = voice->reg[REG_CURRPOS] << 16;
				break;

			case 0x78:
				COMBINE_DATA(&voice->reg[REG_LEFTVOL]);
				break;
		}
	}
}



/**********************************************************************************************

     BSMT2000_data_0_w -- handle a write to the current register

***********************************************************************************************/

WRITE16_HANDLER( BSMT2000_data_0_w )
{
	bsmt2000_reg_write(&bsmt2000[0], offset, data, mem_mask);
}
