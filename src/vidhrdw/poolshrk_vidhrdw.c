/***************************************************************************

Atari Poolshark video emulation

***************************************************************************/

#include "driver.h"

uint8_t* poolshrk_playfield_ram;
uint8_t* poolshrk_hpos_ram;
uint8_t* poolshrk_vpos_ram;

static struct tilemap* tilemap;


static uint32_t get_memory_offset(uint32_t col, uint32_t row, uint32_t num_cols, uint32_t num_rows)
{
	return num_cols * row + col;
}


static void get_tile_info(int tile_index)
{
	SET_TILE_INFO(1, poolshrk_playfield_ram[tile_index] & 0x3f, 0, 0)
}


VIDEO_START( poolshrk )
{
	tilemap = tilemap_create(get_tile_info, get_memory_offset,
		TILEMAP_TRANSPARENT, 8, 8, 32, 32);

	if (tilemap == NULL)
		return 1;

	tilemap_set_transparent_pen(tilemap, 0);

	return 0;
}


VIDEO_UPDATE( poolshrk )
{
	int i;

	tilemap_mark_all_tiles_dirty(tilemap);

	fillbitmap(bitmap, Machine->pens[0], cliprect);

	/* draw sprites */

	for (i = 0; i < 16; i++)
	{
		int hpos = poolshrk_hpos_ram[i];
		int vpos = poolshrk_vpos_ram[i];

		drawgfx(bitmap, Machine->gfx[0], i, (i == 0) ? 0 : 1, 0, 0,
			248 - hpos, vpos - 15, cliprect, TRANSPARENCY_PEN, 0);
	}

	/* draw playfield */

	tilemap_draw(bitmap, cliprect, tilemap, 0, 0);
}
