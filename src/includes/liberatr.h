/*************************************************************************

	Atari Liberator hardware

*************************************************************************/

/*----------- defined in vidhrdw/liberatr.c -----------*/

extern uint8_t *liberatr_base_ram;
extern uint8_t *liberatr_planet_frame;
extern uint8_t *liberatr_planet_select;
extern uint8_t *liberatr_x;
extern uint8_t *liberatr_y;
extern uint8_t *liberatr_bitmapram;

VIDEO_START( liberatr );
VIDEO_UPDATE( liberatr );

WRITE_HANDLER( liberatr_colorram_w ) ;
WRITE_HANDLER( liberatr_bitmap_w );
READ_HANDLER( liberatr_bitmap_xy_r );
WRITE_HANDLER( liberatr_bitmap_xy_w );
