/*************************************************************************

	Atari I, Robot hardware

*************************************************************************/

/*----------- defined in machine/irobot.c -----------*/

extern uint8_t irvg_clear;
extern uint8_t irobot_bufsel;
extern uint8_t irobot_alphamap;
extern uint8_t *irobot_combase;

DRIVER_INIT( irobot );
MACHINE_INIT( irobot );

READ_HANDLER( irobot_status_r );
WRITE_HANDLER( irobot_statwr_w );
WRITE_HANDLER( irobot_out0_w );
WRITE_HANDLER( irobot_rom_banksel_w );
READ_HANDLER( irobot_control_r );
WRITE_HANDLER( irobot_control_w );
READ_HANDLER( irobot_sharedmem_r );
WRITE_HANDLER( irobot_sharedmem_w );


/*----------- defined in vidhrdw/irobot.c -----------*/

PALETTE_INIT( irobot );
VIDEO_START( irobot );
VIDEO_UPDATE( irobot );

WRITE_HANDLER( irobot_paletteram_w );

void irobot_poly_clear(void);
void irobot_run_video(void);
