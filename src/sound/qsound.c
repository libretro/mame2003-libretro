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

/* QSound output-stage post-processing: 95-tap FIR filter (designed at
 * qsound_sh_start as a windowed-sinc lowpass) and 51-tap per-channel
 * output delay lines.  The DL-1425's output path applies a fixed FIR
 * to the mixed signal and routes it through a pair of delay lines
 * with channel-specific offsets; we approximate that here with a
 * single FIR per channel applied to the L/R mix after the voice loop
 * and echo, followed by a fixed-delay 51-tap ring per channel.  The
 * FIR is designed once at startup; the register dispatch arms below
 * accept writes to the table-position and volume registers so the
 * "unknown register" fall-through path doesn't fire spuriously, but
 * since our filter coefficients are fixed-at-startup the writes are
 * recorded for state-keeping only.  The mode-1 default delay offsets
 * (46 samples for L, 48 for R) come from the algorithm's
 * specification and are integer constants, not coefficient data. */
#define QSOUND_FIR_TAPS  95
#define QSOUND_DELAY_TAPS 51
static int16_t qsound_fir_taps[QSOUND_FIR_TAPS];

struct qsound_fir_state
{
	int16_t delay_line[QSOUND_FIR_TAPS];
	int     pos;        /* write position; reads walk backward from here */
	int     silent_run; /* consecutive zero inputs; when >= TAPS the delay line is fully decayed and the output is provably zero */
};

struct qsound_delay_state
{
	int16_t delay_line[QSOUND_DELAY_TAPS];
	int     write_pos;
	int     read_pos;
	int     delay;      /* desired read-pos lag behind write_pos */
};

static struct qsound_fir_state   qsound_fir[2];        /* [0]=L, [1]=R */
static struct qsound_delay_state qsound_delay_out[2];  /* [0]=L, [1]=R */

/* Output-stage post-processing enable flag.  Set by libretro.c from
 * the core option mame2000-qsound_output_filter (default disabled).
 * When zero, the FIR + delay calls in qsound_update are skipped and
 * the per-sample voice mix is written straight to pOutL/pOutR (the
 * pre-Tier-3 behavior).  The libretro layer also gates the menu
 * visibility of the core option on whether the active driver uses
 * SOUND_QSOUND, so the option only surfaces in the menu for the
 * games where it has an effect. */
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

/* Design a 95-tap lowpass FIR using the windowed-sinc method with a
 * Hamming window, normalised for unity DC gain.  The cutoff is set to
 * ~8 kHz at the libretro sample rate, which rolls off the harsh upper
 * band of the QSound output (the 8-bit ROM samples produce significant
 * ultrasonic-band aliasing energy on hard transients that benefits from
 * smoothing) without dulling the upper-midrange punch the music relies
 * on.  Taps are scaled so their sum is 16384 (2^14); the FIR apply
 * function below normalises the int32 accumulator by >>14, putting
 * the filtered output back in the same magnitude range as the input. */
static void qsound_design_fir(void)
{
	const double cutoff_hz   = 8000.0;
	const double sample_rate = (double)Machine->sample_rate;
	const double fc          = cutoff_hz / sample_rate; /* normalised, 0..0.5 */
	const int    M           = QSOUND_FIR_TAPS - 1;
	double       taps_f[QSOUND_FIR_TAPS];
	double       sum         = 0.0;
	int          n;

	for (n = 0; n < QSOUND_FIR_TAPS; n++)
	{
		double x   = (double)n - 0.5 * (double)M;
		double sinc;
		double win;

		if (x == 0.0)
			sinc = 2.0 * fc;
		else
			sinc = sin(2.0 * M_PI * fc * x) / (M_PI * x);
		/* Hamming window */
		win = 0.54 - 0.46 * cos(2.0 * M_PI * (double)n / (double)M);
		taps_f[n] = sinc * win;
		sum += taps_f[n];
	}
	/* Normalise so the taps sum to 16384 (the FIR apply below does
	 * >>14 to compensate, giving unity DC gain). */
	for (n = 0; n < QSOUND_FIR_TAPS; n++)
		qsound_fir_taps[n] = (int16_t)(taps_f[n] / sum * 16384.0);
}

/* Convolve the input with the static tap set, walking the delay line
 * backward from the most recently written sample.  Input is the
 * post-echo voice mix sample for one output channel; output is the
 * filtered value in the same magnitude range (unity DC gain).
 * Saturated int16 storage in the delay line keeps the accumulator
 * bounded under input overload. */
static int32_t qsound_fir_apply(struct qsound_fir_state *s, int32_t input)
{
	int32_t acc = 0;
	int     pos;
	int     i;

	/* Saturate input to int16 on the way into the delay line so
	 * loud transients don't wrap-truncate; the FIR's int32
	 * accumulator stays bounded ~1.5G even at int16 peak input
	 * times int16 peak taps summed over 95. */
	if (input > 32767)       input = 32767;
	else if (input < -32768) input = -32768;

	/* Fast path: once the input has been zero long enough for the
	 * full tap-count to have rolled through the delay line, every
	 * tap is being multiplied by zero and the convolution result
	 * is provably zero.  Skip the 95-iter loop entirely in that
	 * case -- the delay-line slot still has to advance to keep the
	 * ring position consistent, but the multiply-accumulate is
	 * avoided.  This is the common case during attract-mode silence
	 * and quiet stretches between sound effects; without this, the
	 * FIR pays full cost (95 mul-adds per sample per channel) even
	 * when the input is dead silent. */
	if (input == 0 && s->silent_run >= QSOUND_FIR_TAPS)
	{
		s->delay_line[s->pos] = 0;
		s->pos++;
		if (s->pos >= QSOUND_FIR_TAPS) s->pos = 0;
		return 0;
	}
	if (input == 0) s->silent_run++;
	else            s->silent_run = 0;

	s->delay_line[s->pos] = (int16_t)input;
	pos = s->pos;
	for (i = 0; i < QSOUND_FIR_TAPS; i++)
	{
		acc += (int32_t)s->delay_line[pos] * qsound_fir_taps[i];
		if (pos == 0) pos = QSOUND_FIR_TAPS - 1;
		else          pos--;
	}

	s->pos++;
	if (s->pos >= QSOUND_FIR_TAPS) s->pos = 0;

	/* Taps sum to 16384, so >>14 returns the FIR output to the
	 * same magnitude range as the input. */
	return acc >> 14;
}

/* Apply a fixed-offset delay: write the new input to the ring,
 * return the value at the read position (which lags the write
 * position by .delay samples).  Both positions advance in lockstep,
 * so the lag is preserved across calls. */
static int32_t qsound_delay_apply(struct qsound_delay_state *d, int32_t input)
{
	int32_t output;

	/* Saturate to int16 for ring storage */
	if (input > 32767)       input = 32767;
	else if (input < -32768) input = -32768;
	d->delay_line[d->write_pos] = (int16_t)input;
	d->write_pos++;
	if (d->write_pos >= QSOUND_DELAY_TAPS) d->write_pos = 0;

	output = d->delay_line[d->read_pos];
	d->read_pos++;
	if (d->read_pos >= QSOUND_DELAY_TAPS) d->read_pos = 0;

	return output;
}

/* Recompute the delay read position from the current write position
 * and the desired lag.  Called after the host updates the delay
 * value, or once at init. */
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
		int32_t lacc = 0;
		int32_t racc = 0;
		int32_t echo_in = 0;
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
			lacc += (v * lvol[i]) >> 6;
			racc += (v * rvol[i]) >> 6;
			/* Per-voice echo send: pre-pan, volume-scaled sample
			 * multiplied by the echo send level (register 0xba+ch).
			 * Voices with .echo == 0 contribute nothing, so the
			 * branch around this is left to the compiler. */
			if (pC->echo)
				echo_in += ((v * pC->vol) >> 8) * pC->echo;
			pC->offset += pC->pitch;
		}
		/* Run the echo state machine for this output sample.  Skip
		 * entirely if the delay line is disabled (length 0) or there
		 * is no feedback and no voice contributed input -- this is
		 * the common case for non-Q-Sound games (the channel struct
		 * is zero-initialized) and the QSound boot window before
		 * the game first programs the echo registers. */
		if (qsound_echo_length > 0)
		{
			echo_out = qsound_echo_apply(echo_in);
			lacc += echo_out;
			racc += echo_out;
		}
		/* Output-stage post-processing: FIR lowpass followed by the
		 * channel-specific output delay line, gated on the user
		 * option (default off).  When disabled the per-sample voice
		 * mix flows straight through to pOutL/pOutR, matching the
		 * pre-Tier-3 path. */
		if (qsound_output_filter_enabled)
		{
			lacc = qsound_fir_apply(&qsound_fir[0], lacc);
			racc = qsound_fir_apply(&qsound_fir[1], racc);
			lacc = qsound_delay_apply(&qsound_delay_out[0], lacc);
			racc = qsound_delay_apply(&qsound_delay_out[1], racc);
		}
		pOutL[j] = lacc;
		pOutR[j] = racc;
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

	/* Initialize the output-stage FIR + delay state.  The FIR taps
	 * are designed once here (windowed-sinc lowpass); both channels
	 * use the same coefficient set since the filter is symmetric.
	 * The delay-line offsets default to the algorithm's mode-1
	 * constants (46 for L, 48 for R out of the 51-tap ring). */
	qsound_design_fir();
	memset(&qsound_fir[0], 0, sizeof(qsound_fir[0]));
	memset(&qsound_fir[1], 0, sizeof(qsound_fir[1]));
	qsound_fir[0].silent_run = QSOUND_FIR_TAPS;
	qsound_fir[1].silent_run = QSOUND_FIR_TAPS;
	memset(&qsound_delay_out[0], 0, sizeof(qsound_delay_out[0]));
	memset(&qsound_delay_out[1], 0, sizeof(qsound_delay_out[1]));
	qsound_delay_out[0].delay = 46;
	qsound_delay_out[1].delay = 48;
	qsound_delay_recompute(&qsound_delay_out[0]);
	qsound_delay_recompute(&qsound_delay_out[1]);

	qsound_frq_ratio = ((float)intf->clock / (float)QSOUND_CLOCKDIV) /
						(float) Machine->sample_rate;
	qsound_frq_ratio *= 16.0;

	/* Create pan table */
	for (i=0; i<33; i++)
		qsound_pan_table[i]=(int)((256/sqrt(32)) * sqrt(i));
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
