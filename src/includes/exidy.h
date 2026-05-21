/*************************************************************************

	Exidy 6502 hardware

*************************************************************************/

/*----------- defined in sndhrdw/exidy.c -----------*/

int exidy_sh_start(const struct MachineSound *msound);

WRITE_HANDLER( exidy_shriot_w );
WRITE_HANDLER( exidy_sfxctrl_w );
WRITE_HANDLER( exidy_sh8253_w );
WRITE_HANDLER( exidy_sh6840_w );
READ_HANDLER( exidy_shriot_r );
READ_HANDLER( exidy_sh8253_r );
READ_HANDLER( exidy_sh6840_r );

WRITE_HANDLER( mtrap_voiceio_w );
READ_HANDLER( mtrap_voiceio_r );


/*----------- defined in sndhrdw/targ.c -----------*/

extern uint8_t targ_spec_flag;

int targ_sh_start(const struct MachineSound *msound);
void targ_sh_stop(void);

WRITE_HANDLER( targ_sh_w );


/*----------- defined in vidhrdw/exidy.c -----------*/

#define PALETTE_LEN 8
#define COLORTABLE_LEN 20

extern uint8_t *exidy_characterram;
extern uint8_t *exidy_sprite_no;
extern uint8_t *exidy_sprite_enable;
extern uint8_t *exidy_sprite1_xpos;
extern uint8_t *exidy_sprite1_ypos;
extern uint8_t *exidy_sprite2_xpos;
extern uint8_t *exidy_sprite2_ypos;
extern uint8_t *exidy_color_latch;
extern uint8_t *exidy_palette;
extern uint16_t *exidy_colortable;

extern uint8_t sidetrac_palette[];
extern uint8_t targ_palette[];
extern uint8_t spectar_palette[];
extern uint16_t exidy_1bpp_colortable[];
extern uint16_t exidy_2bpp_colortable[];

extern uint8_t exidy_collision_mask;
extern uint8_t exidy_collision_invert;

PALETTE_INIT( exidy );
VIDEO_START( exidy );
VIDEO_EOF( exidy );
VIDEO_UPDATE( exidy );

INTERRUPT_GEN( exidy_vblank_interrupt );

WRITE_HANDLER( exidy_characterram_w );
WRITE_HANDLER( exidy_color_w );

READ_HANDLER( exidy_interrupt_r );
