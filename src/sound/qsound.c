/***************************************************************************

  Capcom System QSound(tm)
  ========================

  Driver by Paul Leaman (paul@vortexcomputing.demon.co.uk)
		and Miguel Angel Horna (mahorna@teleline.es)

  A 16 channel stereo sample player.

  QSpace position is simulated by panning the sound in the stereo space.

  Register
  0	 xxbb	xx = unknown bb = start high address
  1	 ssss	ssss = sample start address
  2	 pitch
  3	 unknown (always 0x8000)
  4	 loop offset from end address
  5	 end
  6	 master channel volume
  7	 not used
  8	 Balance (left=0x0110  centre=0x0120 right=0x0130)
  9	 unknown (most fixed samples use 0 for this register)

  Many thanks to CAB (the author of Amuse), without whom this probably would
  never have been finished.

  If anybody has some information about this hardware, please send it to me
  to mahorna@teleline.es or 432937@cepsz.unizar.es.
  http://teleline.terra.es/personal/mahorna

***************************************************************************/

#include <math.h>
#include "driver.h"

/* Typedefs & defines */

#define QSOUND_CLOCKDIV 166			 /* Clock divider */
#define QSOUND_CHANNELS 16

struct qsound_channel
{
	int bank;	   /* bank (x16)	*/
	int address;	/* start address */
	int pitch;	  /* pitch */
	int reg3;	   /* unknown (always 0x8000) */
	int loop;	   /* loop address */
	int end;		/* end address */
	int vol;		/* master volume */
	int pan;		/* Pan value */
	int echo;	   /* echo send level (register 0xba+ch); 0 = no echo */

	/* Work variables */
	int key;		/* Key on / key off */

	int lvol;	   /* left volume */
	int rvol;	   /* right volume */
	int pan_index; /* cached pan-table index, 0..32 (for spatial path) */
	int lastdt;	 /* last sample value */
	int offset;	 /* current offset counter */
};


/* Private variables */
static struct QSound_interface *intf;	/* Interface  */
static int qsound_stream;				/* Audio stream */
static struct qsound_channel qsound_channel[QSOUND_CHANNELS];
static int qsound_data;				  /* register latch data */
int8_t *qsound_sample_rom;	/* Q sound sample ROM */

static int qsound_pan_table[33];		 /* Pan volume table */
static float qsound_frq_ratio;		   /* Frequency ratio */

/* QSound echo effect state.  The DL-1425 DSP applies a feedback echo
 * to the summed voice contributions (each voice's send level is its
 * register 0xba+ch).  The delay-line ring buffer holds the last N
 * echo samples (int16 stored as the high half of a 32-bit accumulator
 * so unbounded growth through the feedback path is impossible); per
 * output sample the DSP averages the two-most-recent ring entries,
 * scales them by the feedback gain, adds the current accumulated
 * voice contribution, writes the high half back to the ring, and
 * mixes the averaged value into the output.  Without the FIR filter
 * (the wet path is filtered, the dry path is not) we mix the
 * averaged echo into both L and R equally; this loses the subtle
 * L-dry/R-wet asymmetry of the real DSP but keeps the stereo image
 * balanced.
 *
 * Register layout:
 *   0x93 -> qsound_echo_feedback   (feedback gain, 16-bit)
 *   0xd9 -> qsound_echo_end_pos    (delay-line end, length = end - 0x554)
 *   0xba+ch -> qsound_channel[ch].echo  (per-voice send level) */
#define QSOUND_DELAY_BASE_OFFSET 0x554
#define QSOUND_DELAY_BUFFER_LEN  1024
static int16_t qsound_echo_buffer[QSOUND_DELAY_BUFFER_LEN];
static int qsound_echo_pos;        /* current ring read/write position */
static int qsound_echo_length;     /* current ring length (samples) */
static int qsound_echo_end_pos;    /* raw value written to reg 0xd9 */
static int qsound_echo_feedback;   /* feedback gain (16-bit signed) */
static int qsound_echo_last;       /* previous ring entry (for 2-tap avg) */

/* QSound output-stage post-processing: pan-mix
 * tables (dry and wet) plus default-mode FIR coefficient sets, all
 * applied through a dry/wet split per voice followed by a per-channel
 * delay line.  When the core option is off (default) this entire
 * stage is bypassed and the legacy sqrt-equal-power pan is used; when
 * on, voices are routed into separate dry and wet accumulators with
 * the spatial-audio coefficient curves, the wet path runs through the
 * 95-tap firmware FIR (channel-specific taps), echo is routed
 * asymmetrically (left dry / right wet -- the actual algorithm's
 * "outside-the-speakers" trick), both paths run through their own
 * delay rings with the mode-1 default offsets (wet 0, dry 46 left /
 * 48 right), and the two are summed at the output with DSP-round
 * rounding to the high word.
 *
 * The constant arrays below are reproduced from the qsound-hle project
 *   https://github.com/ValleyBell/qsound-hle
 *   Copyright (c) 2018, ValleyBell, Ian Karlsson  (BSD 3-Clause)
 * which derived them by disassembling and analysing the DL-1425 DSP
 * program ROM.  They are the firmware's own pan-curve and filter
 * coefficient values, included here as data (not code) under the
 * BSD-3-Clause license reproduced in full below.
 *
 *   Copyright (c) 2018, ValleyBell, Ian Karlsson
 *   All rights reserved.
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *   * Neither the name of the copyright holder nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *   POSSIBILITY OF SUCH DAMAGE.
 *
 * The dry table is the per-voice coefficient curve for the unfiltered
 * audio path: indexed by pan position 0..32, table[i] gives the L
 * channel coefficient and table[32-i] gives the R coefficient (so
 * symmetric pan positions produce mirrored output amplitudes).  The
 * wet table is the same shape for the FIR-filtered path; its curve is
 * very different from the dry one and is what gives the QSound chip
 * its characteristic spatial colour. */
#define QSOUND_FIR_TAPS  95
#define QSOUND_DELAY_TAPS 51

static const int16_t qsound_dry_mix_table[33] = {
	-16384,-16384,-16384,-16384,-16384,-16384,-16384,-16384,
	-16384,-16384,-16384,-16384,-16384,-16384,-16384,-16384,
	-16384,-14746,-13107,-11633,-10486, -9175, -8520, -7209,
	 -6226, -5226, -4588, -3768, -3277, -2703, -2130, -1802,
	     0
};

static const int16_t qsound_wet_mix_table[33] = {
	     0, -1638, -1966, -2458, -2949, -3441, -4096, -4669,
	 -4915, -5120, -5489, -6144, -7537, -8831, -9339, -9830,
	-10240,-10322,-10486,-10568,-10650,-11796,-12288,-12288,
	-12534,-12648,-12780,-12829,-12943,-13107,-13418,-14090,
	-16384
};

/* Default left-channel FIR taps. */
static const int16_t qsound_fir_taps_l[QSOUND_FIR_TAPS] = {
	    0,   0,   0,  85,  24, -76,-123, -86, -29, -14, -20,  -7,   6, -28, -87, -89,  -5, 100, 154, 160,
	  150, 118,  41, -48, -78, -23,  59,  83,  -2,-176,-333,-344,-203, -66, -39,   2, 224, 495, 495, 280,
	  432,1340,2483,5377,1905, 658,   0,  97, 347, 285,  35, -95, -78, -82,-151,-192,-171,-149,-147,-113,
	  -22,  71, 118, 129, 127, 110,  71,  31,  20,  36,  46,  23, -27, -63, -53, -21, -19, -60, -92, -69,
	  -12,  25,  29,  30,  40,  41,  29,  30,  46,  39, -15, -74,   0,   0,   0
};

/* Default right-channel FIR taps. */
static const int16_t qsound_fir_taps_r[QSOUND_FIR_TAPS] = {
	    0,   0,   0,  23,  42,  47,  29,  10,   2, -14, -54, -92, -93, -70, -64, -77, -57,  18,  94, 113,
	   87,  69,  67,  50,  25,  29,  58,  62,  24, -39,-131,-256,-325,-234, -45,  58,  78, 223, 485, 496,
	  127,   6, 857,2283,2683,4928,1328, 132,  79, 314, 189, -80, -90,  35, -21,-186,-195, -99,-136,-258,
	 -189,  82, 257, 185,  53,  41,  84,  68,  38,  63,  77,  14, -60, -71, -71,-120,-151, -84,  14,  29,
	   -8,   7,  66,  69,  12,  -3,  54,  92,  52,  -6, -15,  -2,   0,   0,   0
};

struct qsound_fir_state
{
	int16_t        delay_line[QSOUND_FIR_TAPS];
	int            pos;        /* write position; reads walk backward from here */
	int            silent_run; /* consecutive zero inputs; convolution is provably zero when this reaches TAPS */
	const int16_t *taps;       /* coefficient set -- points at qsound_fir_taps_l/r */
};

struct qsound_delay_state
{
	int16_t delay_line[QSOUND_DELAY_TAPS];
	int     write_pos;
	int     read_pos;
	int     delay;      /* desired read-pos lag behind write_pos */
	int     volume;     /* attenuation applied to each output sample */
};

static struct qsound_fir_state   qsound_fir[2];        /* [0]=L, [1]=R */
static struct qsound_delay_state qsound_delay_wet[2];  /* [0]=L, [1]=R */
static struct qsound_delay_state qsound_delay_dry[2];  /* [0]=L, [1]=R */

/* Output-stage post-processing enable flag.  Set by libretro.c from
 * the core option mame2000-qsound_output_filter (default disabled).
 * When zero, the spatial-audio path is bypassed entirely and the
 * simple legacy stereo mix is used.  When one, voices are routed
 * through the dry/wet split with the firmware pan curves, the wet
 * path through the 95-tap FIR, both through their channel delays,
 * and combined at the output.  The libretro layer also gates the
 * menu visibility of the core option on whether the active driver
 * uses SOUND_QSOUND. */
int qsound_output_filter_enabled = 0;

/* Apply the echo to a single accumulated voice-input sample, return
 * the echoed output to be mixed into the L/R outputs.  The ring stores
 * the HIGH 16 bits of the 32-bit feedback sum -- this is what bounds
 * the feedback path and prevents the ring from saturating into noise
 * the moment any voice writes a non-zero echo-send register.  Returns
 * the 2-tap-averaged delay-line value already in int16 range. */
static int16_t qsound_echo_apply(int32_t input)
{
	int32_t old_sample = qsound_echo_buffer[qsound_echo_pos]; /* sign-extend int16 -> int32 */
	int32_t last       = qsound_echo_last;
	int32_t new_sample;

	qsound_echo_last = (int16_t)old_sample;
	/* 2-tap moving average over delay-line output */
	old_sample = (old_sample + last) >> 1;

	/* Feedback path: add the feedback-attenuated average back to the
	 * current accumulated voice input.  old_sample is int16-bounded
	 * (since we read it from the int16 ring), feedback is int16, so
	 * the product is bounded +/-1G and fits int32. */
	new_sample = input + ((old_sample * qsound_echo_feedback) << 2);
	/* Truncate-store the HIGH 16 bits of the int32 sum into the ring.
	 * This keeps the ring values in int16 range no matter what
	 * input magnitude the voice loop produced, which is what kept
	 * the previous int32-ring formulation from working: input here
	 * is the sum of (vol-scaled-sample * echo-send) across up to
	 * 16 voices and can comfortably reach hundreds of millions; the
	 * >>16 dispatches that into the int16 ring slot without taking
	 * the feedback path into overflow on the next pass. */
	qsound_echo_buffer[qsound_echo_pos] = (int16_t)(new_sample >> 16);

	qsound_echo_pos++;
	if (qsound_echo_pos >= qsound_echo_length)
		qsound_echo_pos = 0;

	return (int16_t)old_sample;
}

/* Convolve the input with the channel's fixed firmware tap set,
 * walking the delay line backward from the most recently written
 * sample.  Matches the algorithm's accumulation: per tap, subtract
 * (tap * delayed_sample) << 2 from the accumulator (the firmware's
 * taps were designed for this negative-accumulate convention and
 * for the pan tables' negative-coefficient output, so the two
 * negations cancel and the audible signal is positive).  Input
 * comes in pre-scaled (>>16 of the dry/wet accumulator at the
 * call site, putting it back in int16 range); the int32 output is
 * the convolution sum, which the caller passes through the delay
 * line and the final round/clip stage. */
static int32_t qsound_fir_apply(struct qsound_fir_state *s, int16_t input)
{
	int32_t acc = 0;
	int     pos;
	int     i;

	/* Fast path: once the input has been zero long enough for the
	 * full tap-count to have rolled through the delay line, every
	 * tap is being multiplied by zero and the convolution result
	 * is provably zero.  Skip the 95-iter loop entirely in that
	 * case -- the delay-line slot still has to advance to keep the
	 * ring position consistent, but the multiply-accumulate is
	 * avoided.  This is the common case during attract-mode silence
	 * and quiet stretches between sound effects. */
	if (input == 0 && s->silent_run >= QSOUND_FIR_TAPS)
	{
		s->delay_line[s->pos] = 0;
		s->pos++;
		if (s->pos >= QSOUND_FIR_TAPS) s->pos = 0;
		return 0;
	}
	if (input == 0) s->silent_run++;
	else            s->silent_run = 0;

	s->delay_line[s->pos] = input;
	pos = s->pos;
	for (i = 0; i < QSOUND_FIR_TAPS; i++)
	{
		acc -= ((int32_t)s->taps[i] * (int32_t)s->delay_line[pos]) << 2;
		if (pos == 0) pos = QSOUND_FIR_TAPS - 1;
		else          pos--;
	}

	s->pos++;
	if (s->pos >= QSOUND_FIR_TAPS) s->pos = 0;

	return acc;
}

/* Apply a fixed-offset delay with output volume gain: write the new
 * input's high-16 to the int16 ring, read the value at the lagging
 * position, scale by the channel's volume.  Matches the algorithm's
 * delay implementation.  Both write_pos and read_pos advance in
 * lockstep so the lag is preserved across calls. */
static int32_t qsound_delay_apply(struct qsound_delay_state *d, int32_t input)
{
	int32_t output;

	d->delay_line[d->write_pos] = (int16_t)(input >> 16);
	d->write_pos++;
	if (d->write_pos >= QSOUND_DELAY_TAPS) d->write_pos = 0;

	output = (int32_t)d->delay_line[d->read_pos] * d->volume;
	d->read_pos++;
	if (d->read_pos >= QSOUND_DELAY_TAPS) d->read_pos = 0;

	return output;
}

/* Recompute the delay read position from the current write position
 * and the desired lag.  Called once at init and whenever the host
 * programs a new delay value via the delay-update register. */
static void qsound_delay_recompute(struct qsound_delay_state *d)
{
	int new_read = d->write_pos - d->delay;
	while (new_read < 0) new_read += QSOUND_DELAY_TAPS;
	while (new_read >= QSOUND_DELAY_TAPS) new_read -= QSOUND_DELAY_TAPS;
	d->read_pos = new_read;
}

static void qsound_update( int num, int16_t **buffer, int length )
{
	/* Per-sample outer loop: precompute per-voice setup once, then for
	 * each output sample step through every active voice, accumulating
	 * its contribution into the L/R output samples.  This structure is
	 * the natural shape for any "post-process every voice's
	 * contribution" effect (echo, FIR); the old per-voice-outer
	 * structure made those harder to add. */
	int i,j;
	int16_t *pOutL;
	int16_t *pOutR;
	/* Per-voice state cached for the whole buffer */
	int32_t lvol[QSOUND_CHANNELS];
	int32_t rvol[QSOUND_CHANNELS];
	int8_t *pST[QSOUND_CHANNELS];

	if (Machine->sample_rate == 0) return;

	pOutL = buffer[0];
	pOutR = buffer[1];
	memset(pOutL, 0x00, length * sizeof(int16_t));
	memset(pOutR, 0x00, length * sizeof(int16_t));

	/* Set up per-voice constants: combined L/R volume scaling and the
	 * sample-ROM base pointer for this voice's bank.  Inactive voices
	 * are skipped via the .key flag inside the loop body. */
	for (i = 0; i < QSOUND_CHANNELS; i++)
	{
		if (qsound_channel[i].key)
		{
			lvol[i] = (qsound_channel[i].lvol * qsound_channel[i].vol) >> 8;
			rvol[i] = (qsound_channel[i].rvol * qsound_channel[i].vol) >> 8;
			pST[i]  = qsound_sample_rom + qsound_channel[i].bank;
		}
	}

	for (j = 0; j < length; j++)
	{
		struct qsound_channel *pC = &qsound_channel[0];
		int32_t lacc       = 0;
		int32_t racc       = 0;
		int32_t l_dry      = 0;
		int32_t r_dry      = 0;
		int32_t l_wet      = 0;
		int32_t r_wet      = 0;
		int32_t echo_in    = 0;
		int16_t echo_out;
		for (i = 0; i < QSOUND_CHANNELS; i++, pC++)
		{
			int count, v;
			if (!pC->key)
				continue;
			count = (pC->offset) >> 16;
			pC->offset &= 0xffff;
			if (count)
			{
				pC->address += count;
				if (pC->address >= pC->end)
				{
					if (!pC->loop)
					{
						/* Reached the end of a non-looped sample */
						pC->key = 0;
						continue;
					}
					/* Reached the end, restart the loop */
					pC->address = (pC->end - pC->loop) & 0xffff;
				}
				pC->lastdt = pST[i][pC->address];
			}
			v = pC->lastdt;
			if (qsound_output_filter_enabled)
			{
				/* Spatial path: approximate the algorithm's voice_output
				 * value (the 8-bit sample expanded to int16 then scaled by
				 * the voice volume) as (v * vol) >> 6, then accumulate
				 * into separate dry and wet accumulators using the
				 * firmware-derived pan tables for each channel.  The pan
				 * tables hold negative coefficients designed for the DSP's
				 * subtract-accumulate convention, so we use -= here. */
				int32_t vo = (v * pC->vol) >> 6;
				int     p  = pC->pan_index;
				l_dry -= (vo * qsound_dry_mix_table[p])      << 2;
				r_dry -= (vo * qsound_dry_mix_table[32 - p]) << 2;
				l_wet -= (vo * qsound_wet_mix_table[p])      << 2;
				r_wet -= (vo * qsound_wet_mix_table[32 - p]) << 2;
				if (pC->echo)
					echo_in += vo * pC->echo;
			}
			else
			{
				/* Legacy path: simple stereo mix using the sqrt-equal-
				 * power pan lookup, unchanged from the pre-spatial code. */
				lacc += (v * lvol[i]) >> 6;
				racc += (v * rvol[i]) >> 6;
				if (pC->echo)
					echo_in += ((v * pC->vol) >> 8) * pC->echo;
			}
			pC->offset += pC->pitch;
		}
		/* Run the echo state machine for this output sample.  Skip
		 * entirely if the delay line is disabled (length 0) or there
		 * is no feedback and no voice contributed input -- this is
		 * the common case for non-Q-Sound games (the channel struct
		 * is zero-initialized) and the QSound boot window before
		 * the game first programs the echo registers. */
		if (qsound_echo_length > 0)
			echo_out = qsound_echo_apply(echo_in);
		else
			echo_out = 0;

		if (qsound_output_filter_enabled)
		{
			/* Echo routing: in the algorithm, the echo output is mixed
			 * into the L channel's dry path and the R channel's wet
			 * path (i.e. asymmetric routing -- this is one of the key
			 * tricks behind the chip's "outside-the-speakers" spatial
			 * effect).  After echo, the wet accumulator runs through
			 * the channel-specific FIR, both paths run through their
			 * delay rings (wet at zero offset, dry at the mode-1
			 * default of 46/48 samples), and the two are summed with
			 * the chip's DSP-round (round the int32 sum to the high
			 * word and emit that as the int16 output). */
			int32_t l_wet_f, r_wet_f, l_out, r_out;

			l_dry += (int32_t)echo_out << 16;
			r_wet += (int32_t)echo_out << 16;

			l_wet_f = qsound_fir_apply(&qsound_fir[0], (int16_t)(l_wet >> 16));
			r_wet_f = qsound_fir_apply(&qsound_fir[1], (int16_t)(r_wet >> 16));

			l_out  = (qsound_delay_apply(&qsound_delay_wet[0], l_wet_f) +
			          qsound_delay_apply(&qsound_delay_dry[0], l_dry)) << 2;
			r_out  = (qsound_delay_apply(&qsound_delay_wet[1], r_wet_f) +
			          qsound_delay_apply(&qsound_delay_dry[1], r_dry)) << 2;

			/* DSP-round: round to nearest multiple of 0x10000 then
			 * take the high word as the final int16 output. */
			l_out = (l_out + 0x8000) & ~0xffff;
			r_out = (r_out + 0x8000) & ~0xffff;
			pOutL[j] = (int16_t)(l_out >> 16);
			pOutR[j] = (int16_t)(r_out >> 16);
		}
		else
		{
			lacc += echo_out;
			racc += echo_out;
			pOutL[j] = lacc;
			pOutR[j] = racc;
		}
	}
}

int qsound_sh_start(const struct MachineSound *msound)
{
	int i;

	if (Machine->sample_rate == 0) return 0;

	intf = (struct QSound_interface*)msound->sound_interface;

	qsound_sample_rom = (int8_t *)memory_region(intf->region);

	memset(qsound_channel, 0, sizeof(qsound_channel));

	/* Initialize global echo state.  Matches the DSP program's boot-
	 * time values: the delay-line ring is empty, the end-position
	 * register is set to BASE_OFFSET + 6 (so the initial length is 6
	 * samples until the game programs a real value via register
	 * 0xd9), feedback is zero (no echo), and the moving-average
	 * carry slot starts at zero.  Per-voice .echo defaults to 0 from
	 * the channel-struct memset above. */
	memset(qsound_echo_buffer, 0, sizeof(qsound_echo_buffer));
	qsound_echo_pos       = 0;
	qsound_echo_end_pos   = QSOUND_DELAY_BASE_OFFSET + 6;
	qsound_echo_length    = 6;
	qsound_echo_feedback  = 0;
	qsound_echo_last      = 0;

	/* Initialize the output-stage FIR + delay state for the spatial
	 * path.  Each FIR channel binds to its firmware tap set (mode-1
	 * defaults: left = ROM 0xdb2, right = ROM 0xe11), the FIR delay
	 * lines start cleared and in the silent-skip fast path; the wet
	 * delays start with zero offset and the dry delays with the
	 * algorithm's mode-1 default offsets (46 for left, 48 for right).
	 * Output volumes default to 0x3fff (~unity) on each delay so the
	 * path is audible without the game having to program them. */
	memset(&qsound_fir[0], 0, sizeof(qsound_fir[0]));
	memset(&qsound_fir[1], 0, sizeof(qsound_fir[1]));
	qsound_fir[0].taps       = qsound_fir_taps_l;
	qsound_fir[1].taps       = qsound_fir_taps_r;
	qsound_fir[0].silent_run = QSOUND_FIR_TAPS;
	qsound_fir[1].silent_run = QSOUND_FIR_TAPS;
	memset(&qsound_delay_wet[0], 0, sizeof(qsound_delay_wet[0]));
	memset(&qsound_delay_wet[1], 0, sizeof(qsound_delay_wet[1]));
	memset(&qsound_delay_dry[0], 0, sizeof(qsound_delay_dry[0]));
	memset(&qsound_delay_dry[1], 0, sizeof(qsound_delay_dry[1]));
	qsound_delay_wet[0].delay  = 0;
	qsound_delay_wet[1].delay  = 0;
	qsound_delay_dry[0].delay  = 46;
	qsound_delay_dry[1].delay  = 48;
	qsound_delay_wet[0].volume = 0x3fff;
	qsound_delay_wet[1].volume = 0x3fff;
	qsound_delay_dry[0].volume = 0x3fff;
	qsound_delay_dry[1].volume = 0x3fff;
	qsound_delay_recompute(&qsound_delay_wet[0]);
	qsound_delay_recompute(&qsound_delay_wet[1]);
	qsound_delay_recompute(&qsound_delay_dry[0]);
	qsound_delay_recompute(&qsound_delay_dry[1]);

	qsound_frq_ratio = ((float)intf->clock / (float)QSOUND_CLOCKDIV) /
						(float) Machine->sample_rate;
	qsound_frq_ratio *= 16.0;

	/* Legacy pan table (sqrt equal-power), used only when the
	 * spatial-audio core option is disabled.  pan_index defaults
	 * to 16 (centre) for every voice via the channel-struct
	 * memset above. */
	for (i = 0; i < 33; i++)
		qsound_pan_table[i] = (int)((256 / sqrt(32)) * sqrt(i));
	for (i = 0; i < QSOUND_CHANNELS; i++)
		qsound_channel[i].pan_index = 16;
	{
		/* Allocate stream */
		char buf[2][40];
		const char *name[2];
		int  vol[2];
		name[0] = buf[0];
		name[1] = buf[1];
		sprintf( buf[0], "%s L", sound_name(msound) );
		sprintf( buf[1], "%s R", sound_name(msound) );
		vol[0]=MIXER(intf->mixing_level[0], MIXER_PAN_LEFT);
		vol[1]=MIXER(intf->mixing_level[1], MIXER_PAN_RIGHT);
		qsound_stream = stream_init_multi(2,
			name,
			vol,
			Machine->sample_rate,
			0,
			qsound_update );
	}

	return 0;
}

void qsound_sh_stop (void)
{
	if (Machine->sample_rate == 0) return;
}

WRITE_HANDLER( qsound_data_h_w )
{
	qsound_data=(qsound_data&0xff)|(data<<8);
}

WRITE_HANDLER( qsound_data_l_w )
{
	qsound_data=(qsound_data&0xff00)|data;
}

static void qsound_set_command(int data, int value)
{
	int ch=0,reg=0;
	if (data < 0x80)
	{
		ch=data>>3;
		reg=data & 0x07;
	}
	else
	{
		if (data < 0x90)
		{
			ch=data-0x80;
			reg=8;
		}
		else if (data == 0x93)
		{
			/* Global echo feedback gain */
			reg = 10;
		}
		else if (data == 0xd9)
		{
			/* Global echo delay-line end position; length is
			 * derived as (end_pos - QSOUND_DELAY_BASE_OFFSET) and
			 * is clamped into the local ring buffer below. */
			reg = 11;
		}
		else
		{
			if (data >= 0xba && data < 0xca)
			{
				ch=data-0xba;
				reg=9;
			}
			else
			{
				/* Unknown registers */
				ch=99;
				reg=99;
			}
		}
	}

	switch (reg)
	{
		case 0: /* Bank */
			ch=(ch+1)&0x0f;	/* strange ... */
			qsound_channel[ch].bank=(value&0x7f)<<16;
			break;
		case 1: /* start */
			qsound_channel[ch].address=value;
			break;
		case 2: /* pitch */
			qsound_channel[ch].pitch=(long)
					((float)value * qsound_frq_ratio );
			if (!value)
			{
				/* Key off */
				qsound_channel[ch].key=0;
			}
			break;
		case 3: /* unknown */
			qsound_channel[ch].reg3=value;
			break;
		case 4: /* loop offset */
			qsound_channel[ch].loop=value;
			break;
		case 5: /* end */
			qsound_channel[ch].end=value;
			break;
		case 6: /* master volume */
			if (value==0)
			{
				/* Key off */
				qsound_channel[ch].key=0;
			}
			else if (qsound_channel[ch].key==0)
			{
				/* Key on */
				qsound_channel[ch].key=1;
				qsound_channel[ch].offset=0;
				qsound_channel[ch].lastdt=0;
			}
			qsound_channel[ch].vol=value;
			break;

		case 7:  /* unused */
			break;
		case 8:
			{
			   int pandata=(value-0x10)&0x3f;
			   if (pandata > 32)
					pandata=32;
			   qsound_channel[ch].rvol=qsound_pan_table[pandata];
			   qsound_channel[ch].lvol=qsound_pan_table[32-pandata];
			   /* This core mirrors the L/R pan assignment relative to
			    * mame2000 (see the Balance-register comment at the top of
			    * the file).  Mirror the cached spatial-path index to match,
			    * so the firmware dry/wet pan tables drive the same physical
			    * channel the legacy lvol/rvol assignment above does -- i.e.
			    * the spatial and legacy paths agree on stereo orientation. */
			   qsound_channel[ch].pan_index = 32 - pandata;
			   qsound_channel[ch].pan = value;
			}
			break;
		 case 9: /* per-voice echo send (register 0xba+ch) */
			qsound_channel[ch].echo=value;
			break;
		 case 10: /* global echo feedback (register 0x93) */
			qsound_echo_feedback = (int16_t)value;
			break;
		 case 11: /* global echo end-position (register 0xd9) */
			qsound_echo_end_pos = value;
			{
				int len = value - QSOUND_DELAY_BASE_OFFSET;
				if (len < 0) len = 0;
				if (len > QSOUND_DELAY_BUFFER_LEN)
					len = QSOUND_DELAY_BUFFER_LEN;
				qsound_echo_length = len;
				/* Reset the ring position so the new delay window
				 * starts coherently rather than mid-tap. */
				if (qsound_echo_pos >= len)
					qsound_echo_pos = 0;
			}
			break;
	}
}



WRITE_HANDLER( qsound_cmd_w )
{
	qsound_set_command(data, qsound_data);
}

READ_HANDLER( qsound_status_r )
{
	/* Port ready bit (0x80 if ready) */
	return 0x80;
}
