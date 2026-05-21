/*************************************************************************

	Raster Elite Tickee Tickats hardware

**************************************************************************/

/*----------- defined in driver/tickee.c -----------*/

extern uint16_t *tickee_control;


/*----------- defined in vidhrdw/tickee.c -----------*/

extern uint16_t *tickee_vram;

VIDEO_START( tickee );

VIDEO_UPDATE( tickee );
