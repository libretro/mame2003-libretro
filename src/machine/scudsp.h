/*SCU DSP stuff*/

extern void dsp_prg_ctrl(uint32_t data);
extern void dsp_prg_data(uint32_t data);
extern void dsp_ram_addr_ctrl(uint32_t data);
extern void dsp_ram_addr_w(uint32_t data);
extern uint32_t dsp_ram_addr_r(void);
extern void dsp_execute_program(void);

