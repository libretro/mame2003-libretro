/*************************************************************************

	Atari Batman hardware

*************************************************************************/

/*----------- defined in vidhrdw/batman.c -----------*/

extern uint8_t batman_alpha_tile_bank;

VIDEO_START( batman );
VIDEO_UPDATE( batman );

void batman_scanline_update(int scanline);
