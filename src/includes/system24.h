
/* system24temp_ functions / variables are from shared rewrite files,
   once the rest of the rewrite is complete they can be removed, I
   just made a copy & renamed them for now to avoid any conflicts
*/

/* New Code */

extern uint16_t *system24temp_sys16_shared_ram;
READ16_HANDLER( system24temp_sys16_shared_ram_r );
WRITE16_HANDLER( system24temp_sys16_shared_ram_w );

void system24temp_sys16_io_set_callbacks(uint8_t (*io_r)(int port),
							void  (*io_w)(int port, uint8_t data),
							void  (*cnt_w)(uint8_t data),
							READ16_HANDLER ((*iod_r)),
							WRITE16_HANDLER((*iod_w)));
READ16_HANDLER ( system24temp_sys16_io_r );
WRITE16_HANDLER( system24temp_sys16_io_w );
READ32_HANDLER ( system24temp_sys16_io_dword_r );
WRITE32_HANDLER( system24temp_sys16_io_dword_w );

void system24temp_sys16_io_2_set_callbacks(uint8_t (*io_r)(int port),
							  void  (*io_w)(int port, uint8_t data),
							  void  (*cnt_w)(uint8_t data),
							  READ16_HANDLER ((*iod_r)),
							  WRITE16_HANDLER((*iod_w)));
READ16_HANDLER ( system24temp_sys16_io_2_r );
WRITE16_HANDLER( system24temp_sys16_io_2_w );
READ16_HANDLER ( system24temp_sys16_io_2_dword_r );
WRITE16_HANDLER( system24temp_sys16_io_2_dword_w );

/* End New Code */
