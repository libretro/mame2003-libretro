/*************************************************************************

	Kitco Crowns Golf hardware

**************************************************************************/

/*----------- defined in vidhrdw/crgolf.c -----------*/

extern uint8_t *crgolf_color_select;
extern uint8_t *crgolf_screen_flip;
extern uint8_t *crgolf_screen_select;
extern uint8_t *crgolf_screenb_enable;
extern uint8_t *crgolf_screena_enable;

WRITE_HANDLER( crgolf_videoram_bit0_w );
WRITE_HANDLER( crgolf_videoram_bit1_w );
WRITE_HANDLER( crgolf_videoram_bit2_w );

READ_HANDLER( crgolf_videoram_bit0_r );
READ_HANDLER( crgolf_videoram_bit1_r );
READ_HANDLER( crgolf_videoram_bit2_r );

PALETTE_INIT( crgolf );
VIDEO_START( crgolf );
VIDEO_UPDATE( crgolf );
