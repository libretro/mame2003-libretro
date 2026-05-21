/*************************************************************************

	Atari G1 hardware

*************************************************************************/

/*----------- defined in vidhrdw/atarig1.c -----------*/

WRITE16_HANDLER( atarig1_mo_control_w );

VIDEO_START( atarig1 );
VIDEO_UPDATE( atarig1 );

void atarig1_scanline_update(int param);

extern uint8_t atarig1_pitfight;
