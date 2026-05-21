/***************************************************************************

  Atari TIA video emulation

***************************************************************************/

#include "driver.h"
#include <math.h>

extern WRITE_HANDLER( tia_sound_w );

static uint32_t frame_cycles;
static uint32_t paddle_cycles;

static int horzP0;
static int horzP1;
static int horzM0;
static int horzM1;
static int horzBL;

static int current_bitmap;

static int prev_x;
static int prev_y;

static uint8_t VSYNC;
static uint8_t VBLANK;
static uint8_t COLUP0;
static uint8_t COLUP1;
static uint8_t COLUBK;
static uint8_t COLUPF;
static uint8_t CTRLPF;
static uint8_t GRP0;
static uint8_t GRP1;
static uint8_t REFP0;
static uint8_t REFP1;
static uint8_t HMP0;
static uint8_t HMP1;
static uint8_t HMM0;
static uint8_t HMM1;
static uint8_t HMBL;
static uint8_t VDELP0;
static uint8_t VDELP1;
static uint8_t VDELBL;
static uint8_t NUSIZ0;
static uint8_t NUSIZ1;
static uint8_t ENAM0;
static uint8_t ENAM1;
static uint8_t ENABL;
static uint8_t CXM0P;
static uint8_t CXM1P;
static uint8_t CXP0FB;
static uint8_t CXP1FB;
static uint8_t CXM0FB;
static uint8_t CXM1FB;
static uint8_t CXBLPF;
static uint8_t CXPPMM;
static uint8_t RESMP0;
static uint8_t RESMP1;
static uint8_t PF0;
static uint8_t PF1;
static uint8_t PF2;
static uint8_t INPT4;
static uint8_t INPT5;

static uint8_t prevGRP0;
static uint8_t prevGRP1;
static uint8_t prevENABL;

static struct mame_bitmap *helper[2];


static const int nusiz[8][3] =
{
	{ 1, 1, 0 },
	{ 2, 1, 1 },
	{ 2, 1, 3 },
	{ 3, 1, 1 },
	{ 2, 1, 7 },
	{ 1, 2, 0 },
	{ 3, 1, 3 },
	{ 1, 4, 0 }
};


PALETTE_INIT( tia_NTSC )
{
	int i, j;

	static const double color[16][2] =
	{
		{  0.000,  0.000 },
		{  0.144, -0.189 },
		{  0.231, -0.081 },
		{  0.243,  0.032 },
		{  0.217,  0.121 },
		{  0.117,  0.216 },
		{  0.021,  0.233 },
		{ -0.066,  0.196 },
		{ -0.139,  0.134 },
		{ -0.182,  0.062 },
		{ -0.175, -0.022 },
		{ -0.136, -0.100 },
		{ -0.069, -0.150 },
		{  0.005, -0.159 },
		{  0.071, -0.125 },
		{  0.124, -0.089 }
	};

	for (i = 0; i < 16; i++)
	{
		double I = color[i][0];
		double Q = color[i][1];

		for (j = 0; j < 8; j++)
		{
			double Y = j / 7.0;

			double R = Y + 0.956 * I + 0.621 * Q;
			double G = Y - 0.272 * I - 0.647 * Q;
			double B = Y - 1.106 * I + 1.703 * Q;

			R = pow(R, 0.9) / pow(1, 0.9);
			G = pow(G, 0.9) / pow(1, 0.9);
			B = pow(B, 0.9) / pow(1, 0.9);

			if (R < 0) R = 0;
			if (G < 0) G = 0;
			if (B < 0) B = 0;

			if (R > 1) R = 1;
			if (G > 1) G = 1;
			if (B > 1) B = 1;

			palette_set_color(8 * i + j,
				(uint8_t) (255 * R + 0.5),
				(uint8_t) (255 * G + 0.5),
				(uint8_t) (255 * B + 0.5));
		}
	}
}


PALETTE_INIT( tia_PAL )
{
	int i, j;

	static const double color[16][2] =
	{
		{  0.000,  0.000 },
		{  0.000,  0.000 },
		{ -0.222,  0.032 },
		{ -0.199, -0.027 },
		{ -0.173,  0.117 },
		{ -0.156, -0.197 },
		{ -0.094,  0.200 },
		{ -0.071, -0.229 },
		{  0.070,  0.222 },
		{  0.069, -0.204 },
		{  0.177,  0.134 },
		{  0.163, -0.130 },
		{  0.219,  0.053 },
		{  0.259, -0.042 },
		{  0.000,  0.000 },
		{  0.000,  0.000 }
	};

	for (i = 0; i < 16; i++)
	{
		double U = color[i][0];
		double V = color[i][1];

		for (j = 0; j < 8; j++)
		{
			double Y = j / 7.0;

			double R = Y + 1.403 * V;
			double G = Y - 0.344 * U - 0.714 * V;
			double B = Y + 1.770 * U;

			R = pow(R, 1.2) / pow(255, 1.2);
			G = pow(G, 1.2) / pow(255, 1.2);
			B = pow(B, 1.2) / pow(255, 1.2);

			if (R < 0) R = 0;
			if (G < 0) G = 0;
			if (B < 0) B = 0;

			if (R > 1) R = 1;
			if (G > 1) G = 1;
			if (B > 1) B = 1;

			palette_set_color(8 * i + j,
				(uint8_t) (255 * R + 0.5),
				(uint8_t) (255 * G + 0.5),
				(uint8_t) (255 * B + 0.5));
		}
	}
}


VIDEO_START( tia )
{
	int cx = Machine->drv->screen_width;
	int cy = Machine->drv->screen_height;

	helper[0] = auto_bitmap_alloc(cx, cy);
	helper[1] = auto_bitmap_alloc(cx, cy);

	if (helper[0] == NULL)
		return 1;
	if (helper[1] == NULL)
		return 1;

	return 0;
}


VIDEO_UPDATE( tia )
{
	copybitmap(bitmap, helper[1 - current_bitmap], 0, 0, 0, 0,
		cliprect, TRANSPARENCY_NONE, 0);
}


static void draw_sprite_helper(uint8_t* p, int horz,
	uint8_t GRP, uint8_t NUSIZ, uint8_t COLUP, uint8_t REFP)
{
	int num = nusiz[NUSIZ & 7][0];
	int siz = nusiz[NUSIZ & 7][1];
	int skp = nusiz[NUSIZ & 7][2];

	int i;
	int j;
	int k;

	if (REFP & 8)
	{
		GRP = BITSWAP8(GRP, 0, 1, 2, 3, 4, 5, 6, 7);
	}

	if (siz >= 2)
	{
		horz++; /* hardware oddity, see bridges in River Raid */
	}

	for (i = 0; i < num; i++)
	{
		for (j = 0; j < 8; j++)
		{
			for (k = 0; k < siz; k++)
			{
				if (GRP & (0x80 >> j))
				{
					p[horz % 160] = COLUP >> 1;
				}

				horz++;
			}
		}

		horz += 8 * skp;
	}
}


static void draw_missile_helper(uint8_t* p, int horz,
	uint8_t RESMP, uint8_t ENAM, uint8_t NUSIZ, uint8_t COLUM)
{
	int num = nusiz[NUSIZ & 7][0];
	int skp = nusiz[NUSIZ & 7][2];

	int width = 1 << ((NUSIZ >> 4) & 3);

	int i;
	int j;

	for (i = 0; i < num; i++)
	{
		for (j = 0; j < width; j++)
		{
			if ((ENAM & 2) && !(RESMP & 2))
			{
				p[horz % 160] = COLUM >> 1;
			}

			horz++;
		}

		horz += 8 * (skp + 1) - width;
	}
}


static void draw_playfield_helper(uint8_t* p, int horz, 
	uint8_t COLU, uint8_t REFPF)
{
	uint32_t PF =
		(BITSWAP8(PF0, 0, 1, 2, 3, 4, 5, 6, 7) << 0x10) |
		(BITSWAP8(PF1, 7, 6, 5, 4, 3, 2, 1, 0) << 0x08) |
		(BITSWAP8(PF2, 0, 1, 2, 3, 4, 5, 6, 7) << 0x00);

	int i;
	int j;

	if (REFPF)
	{
		uint32_t swap = 0;

		for (i = 0; i < 20; i++)
		{
			swap <<= 1;

			if (PF & 1)
			{
				swap |= 1;
			}

			PF >>= 1;
		}

		PF = swap;
	}

	for (i = 0; i < 20; i++)
	{
		for (j = 0; j < 4; j++)
		{
			if (PF & (0x80000 >> i))
			{
				p[horz] = COLU >> 1;
			}

			horz++;
		}
	}
}


static void draw_ball_helper(uint8_t* p, int horz, uint8_t ENAB)
{
	int width = 1 << ((CTRLPF >> 4) & 3);

	int i;

	for (i = 0; i < width; i++)
	{
		if (ENAB & 2)
		{
			p[horz % 160] = COLUPF >> 1;
		}

		horz++;
	}
}


static void drawS0(uint8_t* p)
{
	draw_sprite_helper(p, horzP0,
		(VDELP0 & 1) ? prevGRP0 : GRP0, NUSIZ0, COLUP0, REFP0);
}


static void drawS1(uint8_t* p)
{
	draw_sprite_helper(p, horzP1,
		(VDELP1 & 1) ? prevGRP1 : GRP1, NUSIZ1, COLUP1, REFP1);
}


static void drawM0(uint8_t* p)
{
	draw_missile_helper(p, horzM0, RESMP0, ENAM0, NUSIZ0, COLUP0);
}


static void drawM1(uint8_t* p)
{
	draw_missile_helper(p, horzM1, RESMP1, ENAM1, NUSIZ1, COLUP1);
}


static void drawBL(uint8_t* p)
{
	draw_ball_helper(p, horzBL, (VDELBL & 1) ? prevENABL : ENABL);
}


static void drawPF(uint8_t* p)
{
	draw_playfield_helper(p, 0,
		(CTRLPF & 2) ? COLUP0 : COLUPF, 0);

	draw_playfield_helper(p, 80,
		(CTRLPF & 2) ? COLUP1 : COLUPF, CTRLPF & 1);
}


static int collision_check(uint8_t* p1, uint8_t* p2, int x1, int x2)
{
	int i;

	for (i = x1; i < x2; i++)
	{
		if (p1[i] != 0xFF && p2[i] != 0xFF)
		{
			return 1;
		}
	}

	return 0;
}


static int current_x(void)
{
	return 3 * ((activecpu_gettotalcycles() - frame_cycles) % 76) - 68;
}


static int current_y(void)
{
	return (activecpu_gettotalcycles() - frame_cycles) / 76;
}


static void update_bitmap(int next_x, int next_y)
{
	int x;
	int y;

	uint8_t linePF[160];
	uint8_t lineP0[160];
	uint8_t lineP1[160];
	uint8_t lineM0[160];
	uint8_t lineM1[160];
	uint8_t lineBL[160];

	uint8_t temp[160];

	if (prev_y >= next_y && prev_x >= next_x)
	{
		return;
	}

	memset(linePF, 0xFF, sizeof linePF);
	memset(lineP0, 0xFF, sizeof lineP0);
	memset(lineP1, 0xFF, sizeof lineP1);
	memset(lineM0, 0xFF, sizeof lineM0);
	memset(lineM1, 0xFF, sizeof lineM1);
	memset(lineBL, 0xFF, sizeof lineBL);

	if (VBLANK & 2)
	{
		memset(temp, 0, 160);
	}
	else
	{
		drawPF(linePF);
		drawS0(lineP0);
		drawS1(lineP1);
		drawM0(lineM0);
		drawM1(lineM1);
		drawBL(lineBL);

		memset(temp, COLUBK >> 1, 160);

		if (CTRLPF & 4)
		{
			drawS1(temp);
			drawM1(temp);
			drawS0(temp);
			drawM0(temp);
			drawPF(temp);
			drawBL(temp);
		}
		else
		{
			drawPF(temp);
			drawBL(temp);
			drawS1(temp);
			drawM1(temp);
			drawS0(temp);
			drawM0(temp);
		}
	}

	for (y = prev_y; y <= next_y; y++)
	{
		uint16_t* p;

		int x1 = prev_x;
		int x2 = next_x;

		if (y != prev_y || x1 < 0)
		{
			x1 = 0;
		}
		if (y != next_y || x2 > 160)
		{
			x2 = 160;
		}

		if (collision_check(lineM0, lineP1, x1, x2))
			CXM0P |= 0x80;
		if (collision_check(lineM0, lineP0, x1, x2))
			CXM0P |= 0x40;
		if (collision_check(lineM1, lineP0, x1, x2))
			CXM1P |= 0x80;
		if (collision_check(lineM1, lineP1, x1, x2))
			CXM1P |= 0x40;
		if (collision_check(lineP0, linePF, x1, x2))
			CXP0FB |= 0x80;
		if (collision_check(lineP0, lineBL, x1, x2))
			CXP0FB |= 0x40;
		if (collision_check(lineP1, linePF, x1, x2))
			CXP1FB |= 0x80;
		if (collision_check(lineP1, lineBL, x1, x2))
			CXP1FB |= 0x40;
		if (collision_check(lineM0, linePF, x1, x2))
			CXM0FB |= 0x80;
		if (collision_check(lineM0, lineBL, x1, x2))
			CXM0FB |= 0x40;
		if (collision_check(lineM1, linePF, x1, x2))
			CXM1FB |= 0x80;
		if (collision_check(lineM1, lineBL, x1, x2))
			CXM1FB |= 0x40;
		if (collision_check(lineBL, linePF, x1, x2))
			CXBLPF |= 0x80;
		if (collision_check(lineP0, lineP1, x1, x2))
			CXPPMM |= 0x80;
		if (collision_check(lineM0, lineM1, x1, x2))
			CXPPMM |= 0x40;

		if (y >= helper[current_bitmap]->height)
		{
			continue;
		}

		p = helper[current_bitmap]->line[y];

		for (x = x1; x < x2; x++)
		{
			p[x] = temp[x];
		}
	}

	prev_x = next_x;
	prev_y = next_y;
}


static void button_callback(int dummy)
{
	int button0 = readinputport(4) & 0x80;
	int button1 = readinputport(5) & 0x80;

	if (VBLANK & 0x40)
	{
		INPT4 &= button0;
		INPT5 &= button1;
	}
	else
	{
		INPT4 = button0;
		INPT5 = button1;
	}
}


static WRITE_HANDLER( WSYNC_w )
{
	int cycles = activecpu_gettotalcycles() - frame_cycles;

	if (cycles % 76)
	{
		activecpu_adjust_icount(cycles % 76 - 76);
	}
}


static WRITE_HANDLER( VSYNC_w )
{
	if (data & 2)
	{
		if (!(VSYNC & 2))
		{
			update_bitmap(
				Machine->drv->screen_width,
				Machine->drv->screen_height);
			
			current_bitmap ^= 1;

			prev_y = 0;
			prev_x = 0;

			frame_cycles += 76 * current_y();
		}
	}

	VSYNC = data;
}


static WRITE_HANDLER( VBLANK_w )
{
	if (data & 0x80)
	{
		paddle_cycles = activecpu_gettotalcycles();
	}

	VBLANK = data;
}


static WRITE_HANDLER( HMOVE_w )
{
	int curr_x = current_x();
	int curr_y = current_y();

	horzP0 -= ((signed char) HMP0) >> 4;
	horzP1 -= ((signed char) HMP1) >> 4;
	horzM0 -= ((signed char) HMM0) >> 4;
	horzM1 -= ((signed char) HMM1) >> 4;
	horzBL -= ((signed char) HMBL) >> 4;

	if (horzP0 < 0)
		horzP0 += 160;
	if (horzP1 < 0)
		horzP1 += 160;
	if (horzM0 < 0)
		horzM0 += 160;
	if (horzM1 < 0)
		horzM1 += 160;
	if (horzBL < 0)
		horzBL += 160;

	horzP0 %= 160;
	horzP1 %= 160;
	horzM0 %= 160;
	horzM1 %= 160;
	horzBL %= 160;

	if (curr_x <= -8)
	{
		if (curr_y < helper[current_bitmap]->height)
		{
			memset(helper[current_bitmap]->line[curr_y], 0, 16);
		}

		prev_x = 8;
	}
}


static WRITE_HANDLER( RSYNC_w )
{
	/* this address is used in chip testing */
}


static WRITE_HANDLER( HMCLR_w )
{
	HMP0 = 0;
	HMP1 = 0;
	HMM0 = 0;
	HMM1 = 0;
	HMBL = 0;
}


static WRITE_HANDLER( CXCLR_w )
{
	CXM0P = 0;
	CXM1P = 0;
	CXP0FB = 0;
	CXP1FB = 0;
	CXM0FB = 0;
	CXM1FB = 0;
	CXBLPF = 0;
	CXPPMM = 0;
}


static WRITE_HANDLER( RESP0_w )
{
	horzP0 = current_x();

	if (horzP0 < 0)
	{
		horzP0 = 3;
	}
	else
	{
		horzP0 = (horzP0 + 5) % 160;
	}
}


static WRITE_HANDLER( RESP1_w )
{
	horzP1 = current_x();

	if (horzP1 < 0)
	{
		horzP1 = 3;
	}
	else
	{
		horzP1 = (horzP1 + 5) % 160;
	}
}


static WRITE_HANDLER( RESM0_w )
{
	horzM0 = current_x();

	if (horzM0 < 0)
	{
		horzM0 = 2;
	}
	else
	{
		horzM0 = (horzM0 + 4) % 160;
	}
}


static WRITE_HANDLER( RESM1_w )
{
	horzM1 = current_x();

	if (horzM1 < 0)
	{
		horzM1 = 2;
	}
	else
	{
		horzM1 = (horzM1 + 4) % 160;
	}
}


static WRITE_HANDLER( RESBL_w )
{
	horzBL = current_x();

	if (horzBL < 0)
	{
		horzBL = 2;
	}
	else
	{
		horzBL = (horzBL + 4) % 160;
	}
}


static WRITE_HANDLER( RESMP0_w )
{
	if (RESMP0 & 2)
	{
		horzM0 = (horzP0 + 4 * nusiz[NUSIZ0 & 7][1]) % 160;
	}

	RESMP0 = data;
}


static WRITE_HANDLER( RESMP1_w )
{
	if (RESMP1 & 2)
	{
		horzM1 = (horzP1 + 4 * nusiz[NUSIZ1 & 7][1]) % 160;
	}

	RESMP1 = data;
}


static WRITE_HANDLER( GRP0_w )
{
	prevGRP1 = GRP1;

	GRP0 = data;
}


static WRITE_HANDLER( GRP1_w )
{
	prevGRP0 = GRP0;

	GRP1 = data;

	prevENABL = ENABL;
}


static READ_HANDLER( INPT_r )
{
	uint32_t elapsed = activecpu_gettotalcycles() - paddle_cycles;

	return elapsed > 76 * readinputport(offset & 3) ? 0x80 : 0x00;
}


READ_HANDLER( tia_r )
{
	uint8_t data = 0;

	if (!(offset & 0x8))
	{
		update_bitmap(current_x(), current_y());
	}

	switch (offset & 0xF)
	{
	case 0x0:
		return data | CXM0P;
	case 0x1:
		return data | CXM1P;
	case 0x2:
		return data | CXP0FB;
	case 0x3:
		return data | CXP1FB;
	case 0x4:
		return data | CXM0FB;
	case 0x5:
		return data | CXM1FB;
	case 0x6:
		return data | CXBLPF;
	case 0x7:
		return data | CXPPMM;
	case 0x8:
		return data | INPT_r(0);
	case 0x9:
		return data | INPT_r(1);
	case 0xA:
		return data | INPT_r(2);
	case 0xB:
		return data | INPT_r(3);
	case 0xC:
		return data | INPT4;
	case 0xD:
		return data | INPT5;
	}

	return 0;
}


WRITE_HANDLER( tia_w )
{
	static const int delay[0x40] =
	{
		 0,	// VSYNC
		 0,	// VBLANK
		 0,	// WSYNC
		 0,	// RSYNC
		 0,	// NUSIZ0
		 0,	// NUSIZ1
		 0,	// COLUP0
		 0,	// COLUP1
		 0,	// COLUPF
		 0,	// COLUBK
		 0,	// CTRLPF
		 0,	// REFP0
		 0,	// REFP1
		 4,	// PF0
		 4,	// PF1
		 4,	// PF2
		 0,	// RESP0
		 0,	// RESP1
		 0,	// RESM0
		 0,	// RESM1
		 0,	// RESBL
		-1,	// AUDC0
		-1,	// AUDC1 
		-1,	// AUDF0
		-1,	// AUDF1
		-1,	// AUDV0
		-1,	// AUDV1
		 1,	// GRP0
		 1,	// GRP1
		 0,	// ENAM0 
		 0,	// ENAM1
		 0,	// ENABL
		-1,	// HMP0
		-1,	// HMP1
		-1,	// HMM0
		-1,	// HMM1
		-1,	// HMBL
		 0,	// VDELP0
		 0,	// VDELP1
		 0,	// VDELBL
		 0,	// RESMP0
		 0,	// RESMP1
		 3,	// HMOVE
		-1,	// HMCLR
		 0,	// CXCLR
	};

	int curr_x = current_x();
	int curr_y = current_y();

	offset &= 0x3F;

	if (offset >= 0x0D && offset <= 0x0F)
	{
		curr_x &= ~3;
	}

	if (delay[offset] >= 0)
	{
		update_bitmap(curr_x + delay[offset], curr_y);
	}

	switch (offset)
	{
	case 0x00:
		VSYNC_w(offset, data);
		break;
	case 0x01:
		VBLANK_w(offset, data);
		break;
	case 0x02:
		WSYNC_w(offset, data);
		break;
	case 0x03:
		RSYNC_w(offset, data);
		break;
	case 0x04:
		NUSIZ0 = data;
		break;
	case 0x05:
		NUSIZ1 = data;
		break;
	case 0x06:
		COLUP0 = data;
		break;
	case 0x07:
		COLUP1 = data;
		break;
	case 0x08:
		COLUPF = data;
		break;
	case 0x09:
		COLUBK = data;
		break;
	case 0x0A:
		CTRLPF = data;
		break;
	case 0x0B:
		REFP0 = data;
		break;
	case 0x0C:
		REFP1 = data;
		break;
	case 0x0D:
		PF0 = data;
		break;
	case 0x0E:
		PF1 = data;
		break;
	case 0x0F:
		PF2 = data;
		break;
	case 0x10:
		RESP0_w(offset, data);
		break;
	case 0x11:
		RESP1_w(offset, data);
		break;
	case 0x12:
		RESM0_w(offset, data);
		break;
	case 0x13:
		RESM1_w(offset, data);
		break;
	case 0x14:
		RESBL_w(offset, data);
		break;

	case 0x15: /* AUDC0 */
	case 0x16: /* AUDC1 */
	case 0x17: /* AUDF0 */
	case 0x18: /* AUDF1 */
	case 0x19: /* AUDV0 */
	case 0x1A: /* AUDV1 */
		tia_sound_w(offset, data);
		break;

	case 0x1B:
		GRP0_w(offset, data);
		break;
	case 0x1C:
		GRP1_w(offset, data);
		break;
	case 0x1D:
		ENAM0 = data;
		break;
	case 0x1E:
		ENAM1 = data;
		break;
	case 0x1F:
		ENABL = data;
		break;
	case 0x20:
		HMP0 = data;
		break;
	case 0x21:
		HMP1 = data;
		break;
	case 0x22:
		HMM0 = data;
		break;
	case 0x23:
		HMM1 = data;
		break;
	case 0x24:
		HMBL = data;
		break;
	case 0x25:
		VDELP0 = data;
		break;
	case 0x26:
		VDELP1 = data;
		break;
	case 0x27:
		VDELBL = data;
		break;
	case 0x28:
		RESMP0_w(offset, data);
		break;
	case 0x29:
		RESMP1_w(offset, data);
		break;
	case 0x2A:
		HMOVE_w(offset, data);
		break;
	case 0x2B:
		HMCLR_w(offset, data);
		break;
	case 0x2C:
		CXCLR_w(offset, 0);
		break;
	}
}


void tia_init(void)
{
	timer_pulse(TIME_IN_HZ(60), 0, button_callback);

	INPT4 = 0x80;
	INPT5 = 0x80;

	HMP0 = 0;
	HMP1 = 0;
	HMM0 = 0;
	HMM1 = 0;
	HMBL = 0;

	prev_x = 0;
	prev_y = 0;

	frame_cycles = 0;
}
