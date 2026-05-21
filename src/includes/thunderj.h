/*************************************************************************

	Atari ThunderJaws hardware

*************************************************************************/

/*----------- defined in vidhrdw/thunderj.c -----------*/

extern uint8_t thunderj_alpha_tile_bank;

VIDEO_START( thunderj );
VIDEO_UPDATE( thunderj );

void thunderj_mark_high_palette(struct mame_bitmap *bitmap, uint16_t *pf, uint16_t *mo, int x, int y);
void thunderj_scanline_update(int scanline);
