/***************************************************************************

	Sun Electronics Kangaroo hardware

	driver by Ville Laitinen

***************************************************************************/

#include "driver.h"


/*----------- defined in vidhrdw/kangaroo.c -----------*/

extern uint8_t *kangaroo_video_control;
extern uint8_t *kangaroo_bank_select;
extern uint8_t *kangaroo_blitter;
extern uint8_t *kangaroo_scroll;

PALETTE_INIT( kangaroo );
VIDEO_START( kangaroo );
VIDEO_UPDATE( kangaroo );

WRITE_HANDLER( kangaroo_blitter_w );
WRITE_HANDLER( kangaroo_videoram_w );
WRITE_HANDLER( kangaroo_video_control_w );
WRITE_HANDLER( kangaroo_bank_select_w );
WRITE_HANDLER( kangaroo_color_mask_w );
