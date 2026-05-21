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

	/* Convert the currently selected VRAM page straight into the bitmap.
	   Two 15-bit pixels are packed per 32-bit word; the palette is a 1:1
	   5-5-5 map, so the masked value is used directly as the pixel index. */
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
