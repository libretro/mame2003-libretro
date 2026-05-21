/***************************************************************************

	Killer Instinct hardware

	driver by Aaron Giles and Bryan McPhail

***************************************************************************/

#include "driver.h"
#include "kinst.h"


/* Pointer to the VRAM page currently selected for scanout. The hardware is
   double-buffered: the game renders into the back page, then flips the page
   select (control register $80, bit 2) to display it. We read the selected
   page directly, so no intermediate buffer or copy is needed. */
data32_t *kinst_video_base;



/*************************************
 *
 *	Palette setup
 *
 *************************************/

PALETTE_INIT( kinst )
{
	int i;

	/* standard 5-5-5 RGB palette */
	for (i = 0; i < 32768; i++)
	{
		int r = i & 31;
		int g = (i >> 5) & 31;
		int b = (i >> 10) & 31;
		palette_set_color(i, (r << 3) | (r >> 2), (g << 3) | (g >> 2), (b << 3) | (b >> 2));
	}
}



/*************************************
 *
 *	Video system start
 *
 *************************************/

VIDEO_START( kinst )
{
	return 0;
}



/*************************************
 *
 *	Main refresh
 *
 *************************************/

VIDEO_UPDATE( kinst )
{
	int y;
	unsigned fb_pitch;
	/* Fast path: paint RGB565 straight into the frontend framebuffer, skipping
	   the game bitmap and the core's conversion pass. The framebuffer stores
	   0BGR1555 (red in the low 5 bits, blue in the high 5), matching the
	   palette: red/blue land in their RGB565 slots and green expands 5->6
	   bits exactly as the palette path does. */
	UINT16 *fb = (UINT16 *)mame2003_direct_rgb565_begin(&fb_pitch);

	if (fb)
	{
		for (y = cliprect->min_y; y <= cliprect->max_y; y++)
		{
			data32_t *src  = &kinst_video_base[640/4 * y];
			UINT16   *dest = (UINT16 *)((UINT8 *)fb + y * fb_pitch);
			int i;

			for (i = 0; i < 320; i += 2)
			{
				UINT16 v0 = (UINT16)(*src & 0x7fff);
				UINT16 v1 = (UINT16)((*src++ >> 16) & 0x7fff);
				UINT16 g0 = (UINT16)((v0 >> 5) & 0x1F);
				UINT16 g1 = (UINT16)((v1 >> 5) & 0x1F);
				*dest++ = (UINT16)(((v0 & 0x1F) << 11) | (((g0 << 1) | (g0 >> 4)) << 5) | ((v0 >> 10) & 0x1F));
				*dest++ = (UINT16)(((v1 & 0x1F) << 11) | (((g1 << 1) | (g1 >> 4)) << 5) | ((v1 >> 10) & 0x1F));
			}
		}
		return;
	}

	/* Fallback: write 15-bit palette indices into the game bitmap; the core
	   converts to the frontend format (and composites any UI/artwork). */
	for (y = cliprect->min_y; y <= cliprect->max_y; y++)
	{
		data32_t *src = &kinst_video_base[640/4 * y];
		UINT16 *dest = (UINT16 *)bitmap->line[y];
		int i;

		for (i = 0; i < 320; i += 2)
		{
			*dest++ = *src & 0x7fff;
			*dest++ = (*src++ >> 16) & 0x7fff;
		}
	}
}
