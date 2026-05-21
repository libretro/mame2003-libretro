/***************************************************************************

Atari Orbit video emulation

***************************************************************************/

#include "driver.h"

uint8_t* orbit_playfield_ram;
uint8_t* orbit_sprite_ram;

static struct tilemap* tilemap;

static int orbit_flip_screen;


WRITE_HANDLER( orbit_playfield_w )
{
	tilemap_mark_tile_dirty(tilemap, offset);

	orbit_playfield_ram[offset] = data;
}


WRITE_HANDLER( orbit_sprite_w )
{
	orbit_sprite_ram[offset] = data;
}


static uint32_t get_memory_offset(uint32_t col, uint32_t row, uint32_t num_cols, uint32_t num_rows)
{
	return num_cols * row + col;
}


static void get_tile_info(int tile_index)
{
	uint8_t code = orbit_playfield_ram[tile_index];

	int flags = 0;

	if (code & 0x40)
	{
		flags |= TILE_FLIPX;
	}
	if (orbit_flip_screen)
	{
		flags |= TILE_FLIPY;
	}

	SET_TILE_INFO(3, code & 0x3f, 0, flags)
}


VIDEO_START( orbit )
{
	tilemap = tilemap_create(get_tile_info, get_memory_offset, 0, 16, 16, 32, 30);

	if (tilemap == NULL)
		return 1;

	return 0;
}


static void orbit_draw_sprites(struct mame_bitmap* bitmap, const struct rectangle* cliprect)
{
	const uint8_t* p = orbit_sprite_ram;

	int i;

	for (i = 0; i < 16; i++)
	{
		int code = *p++;
		int vpos = *p++;
		int hpos = *p++;
		int flag = *p++;

		int layout =
			((flag & 0xc0) == 0x80) ? 1 :
			((flag & 0xc0) == 0xc0) ? 2 : 0;

		int flip_x = code & 0x40;
		int flip_y = code & 0x80;

		int zoom_x = 0x10000;
		int zoom_y = 0x10000;

		code &= 0x3f;

		if (flag & 1)
		{
			code |= 0x40;
		}
		if (flag & 2)
		{
			zoom_x *= 2;
		}

		vpos = 240 - vpos;

		hpos <<= 1;
		vpos <<= 1;

		drawgfxzoom(bitmap, Machine->gfx[layout], code, 0, flip_x, flip_y,
			hpos, vpos, cliprect, TRANSPARENCY_PEN, 0, zoom_x, zoom_y);
	}
}


VIDEO_UPDATE( orbit )
{
	orbit_flip_screen = readinputport(3) & 8;

	tilemap_draw(bitmap, cliprect, tilemap, 0, 0);

	orbit_draw_sprites(bitmap, cliprect);
}
