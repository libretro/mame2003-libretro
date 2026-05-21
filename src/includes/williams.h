/*************************************************************************

	Driver for early Williams games

**************************************************************************/


/*----------- defined in machine/wmsyunit.c -----------*/

/* Generic old-Williams PIA interfaces */
extern struct pia6821_interface williams_pia_0_intf;
extern struct pia6821_interface williams_muxed_pia_0_intf;
extern struct pia6821_interface williams_dual_muxed_pia_0_intf;
extern struct pia6821_interface williams_49way_pia_0_intf;
extern struct pia6821_interface williams_49way_muxed_pia_0_intf;
extern struct pia6821_interface williams_pia_1_intf;
extern struct pia6821_interface williams_snd_pia_intf;

/* Game-specific old-Williams PIA interfaces */
extern struct pia6821_interface defender_pia_0_intf;
extern struct pia6821_interface stargate_pia_0_intf;
extern struct pia6821_interface lottofun_pia_0_intf;
extern struct pia6821_interface sinistar_snd_pia_intf;
extern struct pia6821_interface playball_pia_1_intf;
extern struct pia6821_interface spdball_pia_3_intf;

/* Generic later-Williams PIA interfaces */
extern struct pia6821_interface williams2_muxed_pia_0_intf;
extern struct pia6821_interface williams2_pia_1_intf;
extern struct pia6821_interface williams2_snd_pia_intf;

/* Game-specific later-Williams PIA interfaces */
extern struct pia6821_interface mysticm_pia_0_intf;
extern struct pia6821_interface tshoot_pia_0_intf;
extern struct pia6821_interface tshoot_snd_pia_intf;
extern struct pia6821_interface joust2_pia_1_intf;

/* banking variables */
extern uint8_t *williams_bank_base;
extern uint8_t *defender_bank_base;
extern const uint32_t *defender_bank_list;
extern uint8_t williams2_bank;

/* switches controlled by $c900 */
extern uint16_t sinistar_clip;
extern uint8_t williams_cocktail;

/* initialization */
MACHINE_INIT( defender );
MACHINE_INIT( williams );
MACHINE_INIT( williams2 );
MACHINE_INIT( joust2 );

/* banking */
WRITE_HANDLER( williams_vram_select_w );
WRITE_HANDLER( defender_bank_select_w );
WRITE_HANDLER( blaster_bank_select_w );
WRITE_HANDLER( blaster_vram_select_w );
WRITE_HANDLER( williams2_bank_select_w );

/* misc */
WRITE_HANDLER( williams2_7segment_w );

/* Mayday protection */
extern uint8_t *mayday_protection;
READ_HANDLER( mayday_protection_r );


/*----------- defined in vidhrdw/wmsyunit.c -----------*/

extern uint8_t *williams_videoram;
extern uint8_t *williams2_paletteram;

/* blitter variables */
extern uint8_t *williams_blitterram;
extern uint8_t williams_blitter_xor;
extern uint8_t williams_blitter_remap;
extern uint8_t williams_blitter_clip;

/* tilemap variables */
extern uint8_t williams2_tilemap_mask;
extern const uint8_t *williams2_row_to_palette;
extern uint8_t williams2_M7_flip;
extern int8_t  williams2_videoshift;
extern uint8_t williams2_special_bg_color;

/* later-Williams video control variables */
extern uint8_t *williams2_blit_inhibit;
extern uint8_t *williams2_xscroll_low;
extern uint8_t *williams2_xscroll_high;

/* Blaster extra variables */
extern uint8_t *blaster_color_zero_flags;
extern uint8_t *blaster_color_zero_table;
extern uint8_t *blaster_video_bits;


WRITE_HANDLER( defender_videoram_w );
WRITE_HANDLER( williams_videoram_w );
WRITE_HANDLER( williams2_videoram_w );
WRITE_HANDLER( williams_blitter_w );
WRITE_HANDLER( blaster_remap_select_w );
WRITE_HANDLER( blaster_palette_0_w );
READ_HANDLER( williams_video_counter_r );

VIDEO_START( williams );
VIDEO_UPDATE( williams );
VIDEO_UPDATE( williams2 );

VIDEO_START( blaster );
VIDEO_START( williams2 );

WRITE_HANDLER( williams2_fg_select_w );
WRITE_HANDLER( williams2_bg_select_w );
