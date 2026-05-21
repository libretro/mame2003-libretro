/***************************************************************************

	Kyugo hardware games

***************************************************************************/

/* defined in machine/kyugo.c */
extern uint8_t *kyugo_sharedram;

MACHINE_INIT( kyugo );

READ_HANDLER( kyugo_sharedram_r );

WRITE_HANDLER( kyugo_sharedram_w );
WRITE_HANDLER( kyugo_sub_cpu_control_w );

/* defined in vidhrdw/kyugo.c */
extern uint8_t *kyugo_fgvideoram;
extern uint8_t *kyugo_bgvideoram;
extern uint8_t *kyugo_bgattribram;
extern uint8_t *kyugo_spriteram_1;
extern uint8_t *kyugo_spriteram_2;

READ_HANDLER( kyugo_spriteram_2_r );

WRITE_HANDLER( kyugo_fgvideoram_w );
WRITE_HANDLER( kyugo_bgvideoram_w );
WRITE_HANDLER( kyugo_bgattribram_w );
WRITE_HANDLER( kyugo_scroll_x_lo_w );
WRITE_HANDLER( kyugo_gfxctrl_w );
WRITE_HANDLER( kyugo_scroll_y_w );
WRITE_HANDLER( kyugo_flipscreen_w );

VIDEO_START( kyugo );

VIDEO_UPDATE( kyugo );
