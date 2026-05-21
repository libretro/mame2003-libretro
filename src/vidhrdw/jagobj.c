/***************************************************************************

	Atari Jaguar object processor

****************************************************************************/

#ifndef INCLUDE_OBJECT_PROCESSOR
#error jagobj.c is not directly compilable!
#endif


#define LOG_OBJECTS			0


static uint16_t scanline[360];
static uint16_t *clutbase;
static uint8_t *blend_y, *blend_cc;



/*************************************
 *
 *	Object processor init
 *
 *************************************/

int jagobj_init(void)
{
	int i;

	/* allocate memory for tables */
	blend_y = auto_malloc(256 * 256);
	blend_cc = auto_malloc(256 * 256);
	if (!blend_y || !blend_cc)
		return 1;

	/* fill tables */
	for (i = 0; i < 256 * 256; i++)
	{
		int y = (i >> 8) & 0xff;
		int dy = (int8_t)i;
		int c1 = (i >> 8) & 0x0f;
		int dc1 = (int8_t)(i << 4) >> 4;
		int c2 = (i >> 12) & 0x0f;
		int dc2 = (int8_t)(i & 0xf0) >> 4;

		y += dy;
		if (y < 0) y = 0;
		else if (y > 0xff) y = 0xff;
		blend_y[i] = y;

		c1 += dc1;
		if (c1 < 0) c1 = 0;
		else if (c1 > 0x0f) c1 = 0x0f;
		c2 += dc2;
		if (c2 < 0) c2 = 0;
		else if (c2 > 0x0f) c2 = 0x0f;
		blend_cc[i] = (c2 << 4) | c1;
	}

	return 0;
}



/*************************************
 *
 *	Blending function
 *
 *************************************/

#define BLEND(dst, src)		\
	(dst) = (blend_cc[((dst) & 0xff00) | (((src) >> 8) & 0xff)] << 8) | blend_y[(((dst) & 0xff) << 8) | ((src) & 0xff)];



/*************************************
 *
 *	4bpp bitmap renderers
 *
 *************************************/

static INLINE void bitmap_4_draw(int32_t firstpix, int32_t iwidth, data32_t *src, int32_t xpos, uint8_t flags, int32_t dxpos)
{
	if (firstpix & 7)
	{
		uint32_t pixsrc = src[firstpix / 8];
		while (firstpix & 7)
		{
			int pix = (pixsrc >> (4 * (~firstpix & 7))) & 0x0f;
			if ((!(flags & 4) || pix) && (uint32_t)xpos < 360)
			{
				if (!(flags & 2))
					scanline[xpos] = clutbase[BYTE_XOR_BE(pix)];
				else
					BLEND(scanline[xpos], clutbase[BYTE_XOR_BE(pix)]);
			}
			xpos += dxpos;
			firstpix++;
		}
	}

	firstpix /= 8;
	iwidth /= 8;
	iwidth -= firstpix;

	while (iwidth-- > 0)
	{
		uint32_t pix = src[firstpix++];
		if (!(flags & 4) || pix)
		{
			if ((!(flags & 4) || (pix & 0xf0000000)) && (uint32_t)xpos < 360)
			{
				if (!(flags & 2))
					scanline[xpos] = clutbase[BYTE_XOR_BE(pix >> 28)];
				else
					BLEND(scanline[xpos], clutbase[BYTE_XOR_BE(pix >> 28)]);
			}
			xpos += dxpos;

			if ((!(flags & 4) || (pix & 0x0f000000)) && (uint32_t)xpos < 360)
			{
				if (!(flags & 2))
					scanline[xpos] = clutbase[BYTE_XOR_BE((pix >> 24) & 0x0f)];
				else
					BLEND(scanline[xpos], clutbase[BYTE_XOR_BE((pix >> 24) & 0x0f)]);
			}
			xpos += dxpos;

			if ((!(flags & 4) || (pix & 0x00f00000)) && (uint32_t)xpos < 360)
			{
				if (!(flags & 2))
					scanline[xpos] = clutbase[BYTE_XOR_BE((pix >> 20) & 0x0f)];
				else
					BLEND(scanline[xpos], clutbase[BYTE_XOR_BE((pix >> 20) & 0x0f)]);
			}
			xpos += dxpos;

			if ((!(flags & 4) || (pix & 0x000f0000)) && (uint32_t)xpos < 360)
			{
				if (!(flags & 2))
					scanline[xpos] = clutbase[BYTE_XOR_BE((pix >> 16) & 0x0f)];
				else
					BLEND(scanline[xpos], clutbase[BYTE_XOR_BE((pix >> 16) & 0x0f)]);
			}
			xpos += dxpos;

			if ((!(flags & 4) || (pix & 0x0000f000)) && (uint32_t)xpos < 360)
			{
				if (!(flags & 2))
					scanline[xpos] = clutbase[BYTE_XOR_BE((pix >> 12) & 0x0f)];
				else
					BLEND(scanline[xpos], clutbase[BYTE_XOR_BE((pix >> 12) & 0x0f)]);
			}
			xpos += dxpos;

			if ((!(flags & 4) || (pix & 0x00000f00)) && (uint32_t)xpos < 360)
			{
				if (!(flags & 2))
					scanline[xpos] = clutbase[BYTE_XOR_BE((pix >> 8) & 0x0f)];
				else
					BLEND(scanline[xpos], clutbase[BYTE_XOR_BE((pix >> 8) & 0x0f)]);
			}
			xpos += dxpos;

			if ((!(flags & 4) || (pix & 0x000000f0)) && (uint32_t)xpos < 360)
			{
				if (!(flags & 2))
					scanline[xpos] = clutbase[BYTE_XOR_BE((pix >> 4) & 0x0f)];
				else
					BLEND(scanline[xpos], clutbase[BYTE_XOR_BE((pix >> 4) & 0x0f)]);
			}
			xpos += dxpos;

			if ((!(flags & 4) || (pix & 0x0000000f)) && (uint32_t)xpos < 360)
			{
				if (!(flags & 2))
					scanline[xpos] = clutbase[BYTE_XOR_BE(pix & 0x0f)];
				else
					BLEND(scanline[xpos], clutbase[BYTE_XOR_BE(pix & 0x0f)]);
			}
			xpos += dxpos;
		}
		else
			xpos += 8 * dxpos;
	}
}

static void bitmap_4_0(int32_t firstpix, int32_t iwidth, data32_t *src, int32_t xpos)
{
	bitmap_4_draw(firstpix, iwidth, src, xpos, 0, 1);
}

static void bitmap_4_1(int32_t firstpix, int32_t iwidth, data32_t *src, int32_t xpos)
{
	bitmap_4_draw(firstpix, iwidth, src, xpos, 1, -1);
}

static void bitmap_4_2(int32_t firstpix, int32_t iwidth, data32_t *src, int32_t xpos)
{
	bitmap_4_draw(firstpix, iwidth, src, xpos, 2, 1);
}

static void bitmap_4_3(int32_t firstpix, int32_t iwidth, data32_t *src, int32_t xpos)
{
	bitmap_4_draw(firstpix, iwidth, src, xpos, 3, -1);
}

static void bitmap_4_4(int32_t firstpix, int32_t iwidth, data32_t *src, int32_t xpos)
{
	bitmap_4_draw(firstpix, iwidth, src, xpos, 4, 1);
}

static void bitmap_4_5(int32_t firstpix, int32_t iwidth, data32_t *src, int32_t xpos)
{
	bitmap_4_draw(firstpix, iwidth, src, xpos, 5, -1);
}

static void bitmap_4_6(int32_t firstpix, int32_t iwidth, data32_t *src, int32_t xpos)
{
	bitmap_4_draw(firstpix, iwidth, src, xpos, 6, 1);
}

static void bitmap_4_7(int32_t firstpix, int32_t iwidth, data32_t *src, int32_t xpos)
{
	bitmap_4_draw(firstpix, iwidth, src, xpos, 7, -1);
}

static void (*bitmap4[8])(int32_t, int32_t, data32_t *, int32_t) =
{
	bitmap_4_0,
	bitmap_4_1,
	bitmap_4_2,
	bitmap_4_3,
	bitmap_4_4,
	bitmap_4_5,
	bitmap_4_6,
	bitmap_4_7
};



/*************************************
 *
 *	8bpp bitmap renderers
 *
 *************************************/

static INLINE void bitmap_8_draw(int32_t firstpix, int32_t iwidth, data32_t *src, int32_t xpos, uint8_t flags, int32_t dxpos)
{
	if (firstpix & 3)
	{
		uint32_t pixsrc = src[firstpix / 4];
		while (firstpix & 3)
		{
			uint8_t pix = pixsrc >> (8 * (~firstpix & 3));
			if ((!(flags & 4) || pix) && (uint32_t)xpos < 360)
			{
				if (!(flags & 2))
					scanline[xpos] = clutbase[BYTE_XOR_BE(pix)];
				else
					BLEND(scanline[xpos], clutbase[BYTE_XOR_BE(pix)]);
			}
			xpos += dxpos;
			firstpix++;
		}
	}

	firstpix /= 4;
	iwidth /= 4;
	iwidth -= firstpix;

	while (iwidth-- > 0)
	{
		uint32_t pix = src[firstpix++];
		if (!(flags & 4) || pix)
		{
			if ((!(flags & 4) || (pix & 0xff000000)) && (uint32_t)xpos < 360)
			{
				if (!(flags & 2))
					scanline[xpos] = clutbase[BYTE_XOR_BE(pix >> 24)];
				else
					BLEND(scanline[xpos], clutbase[BYTE_XOR_BE(pix >> 24)]);
			}
			xpos += dxpos;

			if ((!(flags & 4) || (pix & 0x00ff0000)) && (uint32_t)xpos < 360)
			{
				if (!(flags & 2))
					scanline[xpos] = clutbase[BYTE_XOR_BE((pix >> 16) & 0xff)];
				else
					BLEND(scanline[xpos], clutbase[BYTE_XOR_BE((pix >> 16) & 0xff)]);
			}
			xpos += dxpos;

			if ((!(flags & 4) || (pix & 0x0000ff00)) && (uint32_t)xpos < 360)
			{
				if (!(flags & 2))
					scanline[xpos] = clutbase[BYTE_XOR_BE((pix >> 8) & 0xff)];
				else
					BLEND(scanline[xpos], clutbase[BYTE_XOR_BE((pix >> 8) & 0xff)]);
			}
			xpos += dxpos;

			if ((!(flags & 4) || (pix & 0x000000ff)) && (uint32_t)xpos < 360)
			{
				if (!(flags & 2))
					scanline[xpos] = clutbase[BYTE_XOR_BE(pix & 0xff)];
				else
					BLEND(scanline[xpos], clutbase[BYTE_XOR_BE(pix & 0xff)]);
			}
			xpos += dxpos;
		}
		else
			xpos += 4 * dxpos;
	}
}

static void bitmap_8_0(int32_t firstpix, int32_t iwidth, data32_t *src, int32_t xpos)
{
	bitmap_8_draw(firstpix, iwidth, src, xpos, 0, 1);
}

static void bitmap_8_1(int32_t firstpix, int32_t iwidth, data32_t *src, int32_t xpos)
{
	bitmap_8_draw(firstpix, iwidth, src, xpos, 1, -1);
}

static void bitmap_8_2(int32_t firstpix, int32_t iwidth, data32_t *src, int32_t xpos)
{
	bitmap_8_draw(firstpix, iwidth, src, xpos, 2, 1);
}

static void bitmap_8_3(int32_t firstpix, int32_t iwidth, data32_t *src, int32_t xpos)
{
	bitmap_8_draw(firstpix, iwidth, src, xpos, 3, -1);
}

static void bitmap_8_4(int32_t firstpix, int32_t iwidth, data32_t *src, int32_t xpos)
{
	bitmap_8_draw(firstpix, iwidth, src, xpos, 4, 1);
}

static void bitmap_8_5(int32_t firstpix, int32_t iwidth, data32_t *src, int32_t xpos)
{
	bitmap_8_draw(firstpix, iwidth, src, xpos, 5, -1);
}

static void bitmap_8_6(int32_t firstpix, int32_t iwidth, data32_t *src, int32_t xpos)
{
	bitmap_8_draw(firstpix, iwidth, src, xpos, 6, 1);
}

static void bitmap_8_7(int32_t firstpix, int32_t iwidth, data32_t *src, int32_t xpos)
{
	bitmap_8_draw(firstpix, iwidth, src, xpos, 7, -1);
}

static void (*bitmap8[8])(int32_t, int32_t, data32_t *, int32_t) =
{
	bitmap_8_0,
	bitmap_8_1,
	bitmap_8_2,
	bitmap_8_3,
	bitmap_8_4,
	bitmap_8_5,
	bitmap_8_6,
	bitmap_8_7
};



/*************************************
 *
 *	16bpp bitmap renderers
 *
 *************************************/

static INLINE void bitmap_16_draw(int32_t firstpix, int32_t iwidth, data32_t *src, int32_t xpos, uint8_t flags, int32_t dxpos)
{
	if (firstpix & 1)
	{
		uint16_t pix = src[firstpix / 2];
		if ((!(flags & 4) || pix) && (uint32_t)xpos < 360)
		{
			if (!(flags & 2))
				scanline[xpos] = pix;
			else
				BLEND(scanline[xpos], pix);
		}
		xpos += dxpos;
	}

	firstpix /= 2;
	iwidth /= 2;
	iwidth -= firstpix;

	while (iwidth-- > 0)
	{
		uint32_t pix = src[firstpix++];
		if (!(flags & 4) || pix)
		{
			if ((!(flags & 4) || (pix >> 16)) && (uint32_t)xpos < 360)
			{
				if (!(flags & 2))
					scanline[xpos] = pix >> 16;
				else
					BLEND(scanline[xpos], pix >> 16);
			}
			xpos += dxpos;

			if ((!(flags & 4) || (pix & 0xffff)) && (uint32_t)xpos < 360)
			{
				if (!(flags & 2))
					scanline[xpos] = pix;
				else
					BLEND(scanline[xpos], pix);
			}
			xpos += dxpos;
		}
		else
			xpos += 2 * dxpos;
	}
}

static void bitmap_16_0(int32_t firstpix, int32_t iwidth, data32_t *src, int32_t xpos)
{
	bitmap_16_draw(firstpix, iwidth, src, xpos, 0, 1);
}

static void bitmap_16_1(int32_t firstpix, int32_t iwidth, data32_t *src, int32_t xpos)
{
	bitmap_16_draw(firstpix, iwidth, src, xpos, 1, -1);
}

static void bitmap_16_2(int32_t firstpix, int32_t iwidth, data32_t *src, int32_t xpos)
{
	bitmap_16_draw(firstpix, iwidth, src, xpos, 2, 1);
}

static void bitmap_16_3(int32_t firstpix, int32_t iwidth, data32_t *src, int32_t xpos)
{
	bitmap_16_draw(firstpix, iwidth, src, xpos, 3, -1);
}

static void bitmap_16_4(int32_t firstpix, int32_t iwidth, data32_t *src, int32_t xpos)
{
	bitmap_16_draw(firstpix, iwidth, src, xpos, 4, 1);
}

static void bitmap_16_5(int32_t firstpix, int32_t iwidth, data32_t *src, int32_t xpos)
{
	bitmap_16_draw(firstpix, iwidth, src, xpos, 5, -1);
}

static void bitmap_16_6(int32_t firstpix, int32_t iwidth, data32_t *src, int32_t xpos)
{
	bitmap_16_draw(firstpix, iwidth, src, xpos, 6, 1);
}

static void bitmap_16_7(int32_t firstpix, int32_t iwidth, data32_t *src, int32_t xpos)
{
	bitmap_16_draw(firstpix, iwidth, src, xpos, 7, -1);
}

static void (*bitmap16[8])(int32_t, int32_t, data32_t *, int32_t) =
{
	bitmap_16_0,
	bitmap_16_1,
	bitmap_16_2,
	bitmap_16_3,
	bitmap_16_4,
	bitmap_16_5,
	bitmap_16_6,
	bitmap_16_7
};



static INLINE uint8_t lookup_pixel(const data32_t *src, int i, int pitch, int depth)
{
	int ppl			= 32 / depth;
	data32_t data	= src[((i & ppl) / ppl) + ((i / (ppl*2)) * 2 * pitch)];
	uint8_t pix		= (data >> ((~i & (ppl-1)) * depth)) & ((1 << depth) - 1);
	return pix;
}



/*************************************
 *
 *	Standard bitmap processor
 *
 *************************************/

static data32_t *process_bitmap(data32_t *objdata, int vc, int logit)
{
	/* extract minimal data */
	uint32_t upper = objdata[0];
	uint32_t lower = objdata[1];
	uint32_t ypos = (lower >> 3) & 0x7ff;
	uint32_t height = (lower >> 14) & 0x3ff;
	uint32_t link = (lower >> 24) | ((upper & 0x7ff) << 8);
	uint32_t data = (upper >> 11);
	data32_t *src = (data32_t *)get_jaguar_memory(data << 3);

	if (logit)
	{
		/* second phrase */
		uint32_t upper2 = objdata[2];
		uint32_t lower2 = objdata[3];

		/* extract data */
		int32_t xpos = (int32_t)(lower2 << 20) >> 20;
		uint8_t depth = 1 << ((lower2 >> 12) & 7);
		uint8_t pitch = (lower2 >> 15) & 7;
		uint32_t dwidth = (lower2 >> 18) & 0x3ff;
		int32_t iwidth = (lower2 >> 28) | ((upper2 & 0x3f) << 4);
		uint8_t _index = (upper2 >> 6) & 0x3f;
		uint8_t flags = (upper2 >> 13) & 0x0f;
		uint8_t firstpix = (upper2 >> 17) & 0x3f;

		logerror("        ypos=%X height=%X link=%06X data=%06X\n", ypos, height, link << 3, data << 3);
		logerror("        xpos=%X depth=%X pitch=%X dwidth=%X iwidth=%X index=%X flags=%X firstpix=%X\n", xpos, depth, pitch, dwidth, iwidth, _index, flags, firstpix);
	}

	/* only render if valid */
	if (vc >= ypos && height > 0 && src)
	{
		/* second phrase */
		uint32_t upper2 = objdata[2];
		uint32_t lower2 = objdata[3];

		/* extract data */
		int32_t xpos = (int32_t)(lower2 << 20) >> 20;
		uint8_t depthlog = (lower2 >> 12) & 7;
		uint8_t pitch = (lower2 >> 15) & 7;
		uint32_t dwidth = (lower2 >> 18) & 0x3ff;
		uint32_t iwidth = ((lower2 >> 28) | ((upper2 & 0x3f) << 4)) << (6 - depthlog);
		uint8_t _index = (upper2 >> 5) & 0xfe;
		uint8_t flags = (upper2 >> 13) & 0x0f;
		uint8_t firstpix = ((upper2 >> 17) & 0x1f) >> depthlog;
		int i, dxpos = (flags & 1) ? -1 : 1;

		/* preadjust for firstpix */
		xpos += firstpix * dxpos;

		/* switch off the depth */
		switch (depthlog)
		{
			/* 1bpp case */
			case 0:
			{
				uint16_t *clut = (uint16_t *)jaguar_gpu_clut;
				clut += _index & 0xfe;

				/* non-blending */
				if (!(flags & 2))
				{
					for (i = firstpix; i < iwidth; i++)
					{
						uint8_t pix = lookup_pixel(src, i, pitch, 1);

						if (xpos >= 0 && xpos < 360 && (pix || !(flags & 4)))
							scanline[xpos] = clut[BYTE_XOR_BE(pix)];
						xpos += dxpos;
					}
				}

				/* blending */
				else
				{
					for (i = firstpix; i < iwidth; i++)
					{
						uint8_t pix = lookup_pixel(src, i, pitch, 1);

						if (xpos >= 0 && xpos < 360 && (pix || !(flags & 4)))
							BLEND(scanline[xpos], clut[BYTE_XOR_BE(pix)]);
						xpos += dxpos;
					}
				}
				break;
			}

			/* 2bpp case */
			case 1:
			{
				uint16_t *clut = (uint16_t *)jaguar_gpu_clut;
				clut += _index & 0xfc;

				/* non-blending */
				if (!(flags & 2))
				{
					for (i = firstpix; i < iwidth; i++)
					{
						uint8_t pix = lookup_pixel(src, i, pitch, 2);

						if (xpos >= 0 && xpos < 360 && (pix || !(flags & 4)))
							scanline[xpos] = clut[BYTE_XOR_BE(pix)];
						xpos += dxpos;
					}
				}

				/* blending */
				else
				{
					for (i = firstpix; i < iwidth; i++)
					{
						uint8_t pix = lookup_pixel(src, i, pitch, 2);

						if (xpos >= 0 && xpos < 360 && (pix || !(flags & 4)))
							BLEND(scanline[xpos], clut[BYTE_XOR_BE(pix)]);
						xpos += dxpos;
					}
				}
				break;
			}

			/* 4bpp case */
			case 2:
				/* only handle pitch=1 for now */
				if (pitch != 1)
					logerror("Unhandled pitch = %d\n", pitch);

				clutbase = (uint16_t *)jaguar_gpu_clut + (_index & 0xf0);
				(*bitmap4[flags & 7])(firstpix, iwidth, src, xpos);
				break;

			/* 8bpp case */
			case 3:
				/* only handle pitch=1 for now */
				if (pitch != 1)
					logerror("Unhandled pitch = %d\n", pitch);

				clutbase = (uint16_t *)jaguar_gpu_clut;
				(*bitmap8[flags & 7])(firstpix, iwidth, src, xpos);
				break;

			/* 16bpp case */
			case 4:
				/* only handle pitch=1 for now */
				if (pitch != 1)
					logerror("Unhandled pitch = %d\n", pitch);

				(*bitmap16[flags & 7])(firstpix, iwidth, src, xpos);
				break;

			default:
				fprintf(stderr, "Unhandled bitmap source depth = %d\n", depthlog);
				break;
		}

		/* decrement the height and add to the source data offset */
		objdata[0] = upper + (dwidth << 11);
		objdata[1] = lower - (1 << 14);
	}

	return (data32_t *)get_jaguar_memory(link << 3);
}



/*************************************
 *
 *	Scaled bitmap object processor
 *
 *************************************/

static data32_t *process_scaled_bitmap(data32_t *objdata, int vc, int logit)
{
	/* extract data */
	uint32_t upper = objdata[0];
	uint32_t lower = objdata[1];
	uint32_t ypos = (lower >> 3) & 0x7ff;
	uint32_t height = (lower >> 14) & 0x3ff;
	uint32_t link = (lower >> 24) | ((upper & 0x7ff) << 8);
	uint32_t data = (upper >> 11);
	data32_t *src = (data32_t *)get_jaguar_memory(data << 3);

	/* third phrase */
	uint32_t lower3 = objdata[5];
	int32_t remainder = (lower3 >> 16) & 0xff;

	if (logit)
	{
		/* second phrase */
		uint32_t upper2 = objdata[2];
		uint32_t lower2 = objdata[3];

		/* extract data */
		int32_t xpos = (int32_t)(lower2 << 20) >> 20;
		uint8_t depth = 1 << ((lower2 >> 12) & 7);
		uint8_t pitch = (lower2 >> 15) & 7;
		uint32_t dwidth = (lower2 >> 18) & 0x3ff;
		int32_t iwidth = (lower2 >> 28) | ((upper2 & 0x3f) << 4);
		uint8_t _index = (upper2 >> 6) & 0x3f;
		uint8_t flags = (upper2 >> 13) & 0x0f;
		uint8_t firstpix = (upper2 >> 17) & 0x3f;

		int32_t hscale = lower3 & 0xff;
		int32_t vscale = (lower3 >> 8) & 0xff;

		logerror("        ypos=%X height=%X link=%06X data=%06X\n", ypos, height, link << 3, data << 3);
		logerror("        xpos=%X depth=%X pitch=%X dwidth=%X iwidth=%X index=%X flags=%X firstpix=%X\n", xpos, depth, pitch, dwidth, iwidth, _index, flags, firstpix);
		logerror("        hscale=%X vscale=%X remainder=%X\n", hscale, vscale, remainder);
	}

	/* only render if valid */
	if (vc >= ypos && (height > 0 || remainder > 0) && src)
	{
		/* second phrase */
		uint32_t upper2 = objdata[2];
		uint32_t lower2 = objdata[3];

		/* extract data */
		int32_t xpos = (int32_t)(lower2 << 20) >> 20;
		uint8_t depthlog = (lower2 >> 12) & 7;
		uint8_t pitch = (lower2 >> 15) & 7;
		uint32_t dwidth = (lower2 >> 18) & 0x3ff;
		int32_t iwidth = ((lower2 >> 28) | ((upper2 & 0x3f) << 4)) << (6 - depthlog);
		uint8_t _index = (upper2 >> 5) & 0xfe;
		uint8_t flags = (upper2 >> 13) & 0x0f;
		uint8_t firstpix = ((upper2 >> 17) & 0x1f) >> depthlog;

		int32_t hscale = lower3 & 0xff;
		int32_t vscale = (lower3 >> 8) & 0xff;
		int32_t xleft = hscale;
		int dxpos = (flags & 1) ? -1 : 1;
		int xpix = firstpix, yinc;

		/* only handle pitch=0 for now */
		if (pitch != 1)
			logerror("Unhandled pitch = %d\n", pitch);
		if (flags & 2)
		{
			printf("Unhandled blend mode in scaled bitmap case\n");
			logerror("Unhandled blend mode in scaled bitmap case\n");
		}

		/* preadjust for firstpix */
		xpos += firstpix * dxpos;

		/* ignore hscale = 0 */
		if (hscale != 0)
		{
			/* switch off the depth */
			switch (depthlog)
			{
				case 2:
				{
					uint16_t *clut = (uint16_t *)jaguar_gpu_clut;
					clut += _index & 0xf0;

					/* render in phrases */
					while (xpix < iwidth)
					{
						uint16_t pix = (src[xpix / 8] >> ((~xpix & 7) * 4)) & 0x0f;

						while (xleft > 0)
						{
							if (xpos >= 0 && xpos < 360 && (pix || !(flags & 4)))
								scanline[xpos] = clut[BYTE_XOR_BE(pix)];
							xpos += dxpos;
							xleft -= 0x20;
						}
						while (xleft <= 0)
							xleft += hscale, xpix++;
					}
					break;
				}

				case 3:
				{
					uint16_t *clut = (uint16_t *)jaguar_gpu_clut;

					/* render in phrases */
					while (xpix < iwidth)
					{
						uint16_t pix = (src[xpix / 4] >> ((~xpix & 3) * 8)) & 0xff;

						while (xleft > 0)
						{
							if (xpos >= 0 && xpos < 360 && (pix || !(flags & 4)))
								scanline[xpos] = clut[BYTE_XOR_BE(pix)];
							xpos += dxpos;
							xleft -= 0x20;
						}
						while (xleft <= 0)
							xleft += hscale, xpix++;
					}
					break;
				}

				case 4:
					while (xpix < iwidth)
					{
						uint16_t pix = src[xpix / 2] >> ((~xpix & 1) * 16);

						while (xleft > 0)
						{
							if (xpos >= 0 && xpos < 360 && (pix || !(flags & 4)))
								scanline[xpos] = pix;
							xpos += dxpos;
							xleft -= 0x20;
						}
						while (xleft <= 0)
							xleft += hscale, xpix++;
					}
					break;

				default:
					fprintf(stderr, "Unhandled scaled bitmap source depth = %d\n", depthlog);
					break;
			}
		}

		/* handle Y scale */
		remainder -= 0x20;
		yinc = 0;
		while (remainder <= 0 && vscale != 0)
			remainder += vscale, yinc++;
		if (yinc > height)
			yinc = height, remainder = 0;

		/* decrement the height and add to the source data offset */
		objdata[0] = upper + yinc * (dwidth << 11);
		objdata[1] = lower - yinc * (1 << 14);
		objdata[5] = (lower3 & ~0xff0000) | ((remainder & 0xff) << 16);
	}

	return (data32_t *)get_jaguar_memory(link << 3);
}



/*************************************
 *
 *	Brach object processor
 *
 *************************************/

static data32_t *process_branch(data32_t *objdata, int vc, int logit)
{
	uint32_t upper = objdata[0];
	uint32_t lower = objdata[1];
	uint32_t ypos = (lower >> 3) & 0x7ff;
	uint32_t cc = (lower >> 14) & 7;
	uint32_t link = (lower >> 24) | ((upper & 0x7ff) << 8);
	int taken = 0;

#ifndef MESS
	if ((ypos & 1) && ypos != 0x7ff)
		fprintf(stderr, "        branch cc=%d ypos=%X link=%06X - \n", cc, ypos, link << 3);
#endif

	switch (cc)
	{
		/* 0: branch if ypos == vc or ypos == 0x7ff */
		case 0:
			if (logit) logerror("        branch if %X == vc or %X == 0x7ff to %06X\n", ypos, ypos, link << 3);
			taken = (ypos == vc) || (ypos == 0x7ff);
			break;

		/* 1: branch if ypos > vc */
		case 1:
			if (logit) logerror("        branch if %X > vc to %06X\n", ypos, link << 3);
			taken = (ypos > vc);
			break;

		/* 2: branch if ypos < vc */
		case 2:
			if (logit) logerror("        branch if %X < vc to %06X\n", ypos, link << 3);
			taken = (ypos < vc);
			break;

		/* 3: branch if object processor flag is set */
		case 3:
			if (logit) logerror("        branch if object flag set to %06X\n", link << 3);
			fprintf(stderr, "Unhandled branch!\n");
			link = 0, taken = 1;
			break;

		/* 4: branch on second half of display line */
		case 4:
			if (logit) logerror("        branch if second half of line to %06X\n", link << 3);
			taken = (vc & 1);
			break;

		default:
			fprintf(stderr, "Invalid branch!\n");
			link = 0, taken = 1;
			break;
	}

	/* handle the branch */
	return taken ? (data32_t *)get_jaguar_memory(link << 3) : (objdata + 2);
}



/*************************************
 *
 *	Process object list
 *
 *************************************/

static void process_object_list(struct mame_bitmap *bitmap, const struct rectangle *cliprect)
{
	data32_t *objdata;
	int y, x, pass;
	int logit;

	/* loop over all scanlines */
	for (y = cliprect->min_y; y <= cliprect->max_y; y++)
	{
		/* erase the scanline first */
		for (x = 0; x < 336; x++)
			scanline[x] = gpu_regs[BG];

#if LOG_OBJECTS
		logit = (y == cliprect->min_y);
#else
		logit = 0;
#endif

		/* two passes per scanline */
		for (pass = 0; pass < 1; pass++)
		{
			int vc = y * 2 + pass + gpu_regs[VBE];
			int done = 0, count = 0;

			/* fetch the object pointer */
			objdata = (data32_t *)get_jaguar_memory((gpu_regs[OLP_H] << 16) | gpu_regs[OLP_L]);
			while (!done && objdata && count++ < 100)
			{
				/* the low 3 bits determine the command */
				switch (objdata[1] & 7)
				{
					/* bitmap object */
					case 0:
						if (logit)
							logerror("bitmap = %08X-%08X %08X-%08X\n", objdata[0], objdata[1], objdata[2], objdata[3]);
						objdata = process_bitmap(objdata, vc, logit);
						break;

					/* scaled bitmap object */
					case 1:
						if (logit)
							logerror("scaled = %08X-%08X %08X-%08X %08X-%08X\n", objdata[0], objdata[1], objdata[2], objdata[3], objdata[4], objdata[5]);
						objdata = process_scaled_bitmap(objdata, vc, logit);
						break;

					/* branch */
					case 3:
						if (logit)
							logerror("branch = %08X-%08X\n", objdata[0], objdata[1]);
						objdata = process_branch(objdata, vc, logit);
						break;

					/* stop */
					case 4:
					{
						int interrupt = (objdata[1] >> 3) & 1;
						done = 1;

						if (logit)
							logerror("stop   = %08X-%08X\n", objdata[0], objdata[1]);
						if (interrupt)
						{
#ifndef MESS
							fprintf(stderr, "stop int=%d\n", interrupt);
#endif
							cpu_irq_state |= 4;
							update_cpu_irq();
						}
						break;
					}

					default:
						fprintf(stderr, "%08X %08X\n", objdata[0], objdata[1]);
						done = 1;
						break;
				}
			}
		}

		/* render this scanline */
		draw_scanline16(bitmap, 0, y, 336, scanline, pen_table, -1);
	}
}
