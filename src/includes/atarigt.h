/*************************************************************************

	Atari GT hardware

*************************************************************************/

/*----------- defined in vidhrdw/atarigt.c -----------*/

extern uint8_t atarigt_is_primrage;


/*----------- defined in vidhrdw/atarigt.c -----------*/

extern uint16_t *atarigt_colorram;

void atarigt_colorram_w(uint32_t address, uint16_t data, uint16_t mem_mask);
uint16_t atarigt_colorram_r(uint32_t address);

VIDEO_START( atarigt );
VIDEO_UPDATE( atarigt );

void atarigt_scanline_update(int scanline);
