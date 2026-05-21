/*************************************************************************

	Meadows S2650 hardware

*************************************************************************/

/*----------- defined in sndhrdw/meadows.c -----------*/

int meadows_sh_start(const struct MachineSound *msound);
void meadows_sh_stop(void);
void meadows_sh_dac_w(int data);
void meadows_sh_update(void);
extern uint8_t meadows_0c00;
extern uint8_t meadows_0c01;
extern uint8_t meadows_0c02;
extern uint8_t meadows_0c03;


/*----------- defined in vidhrdw/meadows.c -----------*/

VIDEO_START( meadows );
VIDEO_UPDATE( meadows );
WRITE_HANDLER( meadows_videoram_w );
WRITE_HANDLER( meadows_spriteram_w );

