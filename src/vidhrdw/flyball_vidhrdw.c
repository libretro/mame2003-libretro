/***************************************************************************

Atari Flyball video emulation

***************************************************************************/

#include "driver.h"

static struct tilemap* flyball_tilemap;

uint8_t flyball_pitcher_vert;
uint8_t flyball_pitcher_horz;
uint8_t flyball_pitcher_pic;
uint8_t flyball_ball_vert;
uint8_t flyball_ball_horz;

uint8_t* flyball_playfield_ram;


static uint32_t flyball_get_memory_offset(uint32_t col, uint32_t row, uint32_t num_cols, uint32_t num_rows)
{
	if (col == 0)
	{
		col = num_cols;
	}

	return num_cols * (num_rows - row) - col;
}


static void flyball_get_tile_info(int tile_index)
{
	uint8_t data = flyball_playfield_ram[tile_index];

	int flags =
		((data & 0x40) ? TILE_FLIPX : 0) |
		((data & 0x80) ? TILE_FLIPY : 0);

	int code = data & 63;

	if ((flags & TILE_FLIPX) && (flags & TILE_FLIPY))
	{
		code += 64;
	}

	SET_TILE_INFO(0, code, 0, flags)
}


VIDEO_START( flyball )
{
	flyball_tilemap = tilemap_create(flyball_get_tile_info,
		flyball_get_memory_offset, TILEMAP_OPAQUE, 8, 16, 32, 16);

	return 0;
}


VIDEO_UPDATE( flyball )
{
	int pitcherx = flyball_pitcher_horz;
	int pitchery = flyball_pitcher_vert - 31;

	int ballx = flyball_ball_horz - 1;
	int bally = flyball_ball_vert - 17;

	int x;
	int y;

	tilemap_mark_all_tiles_dirty(flyball_tilemap);

	/* draw playfield */

	tilemap_draw(bitmap, cliprect, flyball_tilemap, 0, 0);

	/* draw pitcher */

	drawgfx(bitmap, Machine->gfx[1], flyball_pitcher_pic ^ 0xf,
		0, 1, 0, pitcherx, pitchery, &Machine->visible_area, TRANSPARENCY_PEN, 1);

	/* draw ball */

	for (y = bally; y < bally + 2; y++)
	{
		for (x = ballx; x < ballx + 2; x++)
		{
			if (x >= Machine->visible_area.min_x &&
			    x <= Machine->visible_area.max_x &&
			    y >= Machine->visible_area.min_y &&
			    y <= Machine->visible_area.max_y)
			{
				plot_pixel(bitmap, x, y, Machine->pens[1]);
			}
		}
	}
}
