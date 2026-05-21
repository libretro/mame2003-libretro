#ifndef _UPD7810_H_
#define _UPD7810_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef RUNTIME_LOADER
	extern void upd7810_runtime_loader_init(void);
#endif

/*
  all types have internal ram at 0xff00-0xffff
  7810
  7811 (4kbyte),7812(8),7814(16) have internal rom at 0x0000
*/

// unfortunatly memory configuration differs with internal rom size
typedef enum {
	TYPE_7810,
	TYPE_7810_GAMEMASTER, // a few modifications until internal rom dumped
	TYPE_7807
//	TYPE_78C10, // stop instruction added
//	TYPE_78IV,
//	TYPE_78K0,
//	TYPE_78K0S
//	millions of subtypes
} UPD7810_TYPE;

/* Supply an instance of this function in your driver code:
 * It will be called whenever an output signal changes or a new
 * input line state is to be sampled.
 */
typedef int (*upd7810_io_callback)(int ioline, int state);

// use it as reset parameter in the Machine struct
typedef struct {
    UPD7810_TYPE type;
    upd7810_io_callback io_callback;
} UPD7810_CONFIG;

enum {
	UPD7810_PC=1, UPD7810_SP, UPD7810_PSW,
	UPD7810_EA, UPD7810_V, UPD7810_A, UPD7810_VA,
	UPD7810_BC, UPD7810_B, UPD7810_C, UPD7810_DE, UPD7810_D, UPD7810_E, UPD7810_HL, UPD7810_H, UPD7810_L,
	UPD7810_EA2, UPD7810_V2, UPD7810_A2, UPD7810_VA2,
	UPD7810_BC2, UPD7810_B2, UPD7810_C2, UPD7810_DE2, UPD7810_D2, UPD7810_E2, UPD7810_HL2, UPD7810_H2, UPD7810_L2,
	UPD7810_CNT0, UPD7810_CNT1, UPD7810_TM0, UPD7810_TM1, UPD7810_ECNT, UPD7810_ECPT, UPD7810_ETM0, UPD7810_ETM1,
	UPD7810_MA, UPD7810_MB, UPD7810_MCC, UPD7810_MC, UPD7810_MM, UPD7810_MF,
	UPD7810_TMM, UPD7810_ETMM, UPD7810_EOM, UPD7810_SML, UPD7810_SMH,
	UPD7810_ANM, UPD7810_MKL, UPD7810_MKH, UPD7810_ZCM,
	UPD7810_TXB, UPD7810_RXB, UPD7810_CR0, UPD7810_CR1, UPD7810_CR2, UPD7810_CR3,
	UPD7810_TXD, UPD7810_RXD, UPD7810_SCK, UPD7810_TI, UPD7810_TO, UPD7810_CI, UPD7810_CO0, UPD7810_CO1
};

/* port numbers for PA,PB,PC,PD and PF */
enum {
	UPD7810_PORTA, UPD7810_PORTB, UPD7810_PORTC, UPD7810_PORTD, UPD7810_PORTF
};

enum {
	UPD7807_PORTA, UPD7807_PORTB, UPD7807_PORTC, UPD7807_PORTD, UPD7807_PORTF,
	UPD7807_PORTT
};

/* IRQ lines */
#define UPD7810_INTF1		0
#define UPD7810_INTF2		1
#define UPD7810_INTFE1      4

extern int upd7810_icount;						/* cycle count */

void upd7810_init (void);				/* Initialize save states */
void upd7810_reset (void *param);		/* Reset registers to the initial values */
void upd7810_exit  (void);				/* Shut down CPU core */
int	upd7810_execute(int cycles);		/* Execute cycles - returns number of cycles actually run */
unsigned upd7810_get_context (void *dst);/* Get registers, return context size */
void upd7810_set_context (void *src);	/* Set registers */
unsigned upd7810_get_reg (int regnum);
void upd7810_set_reg (int regnum, unsigned val);
void upd7810_set_irq_line(int irqline, int state);
void upd7810_set_irq_callback(int (*callback)(int irqline));
const char *upd7810_info(void *context, int regnum);
unsigned upd7810_dasm(char *buffer, unsigned pc);

#define upd7807_init upd7810_init
void upd7807_reset (void *param);
#define upd7807_exit upd7810_exit
#define upd7807_execute upd7810_execute
#define upd7807_get_context upd7810_get_context
#define upd7807_set_context upd7810_set_context
#define upd7807_get_reg upd7810_get_reg
#define upd7807_set_reg upd7810_set_reg
#define upd7807_set_irq_line upd7810_set_irq_line
#define upd7807_set_irq_callback upd7810_set_irq_callback
const char *upd7807_info(void *context, int regnum);
unsigned upd7807_dasm(char *buffer, unsigned pc);

typedef struct {
	PAIR	ppc;	/* previous program counter */
	PAIR	pc; 	/* program counter */
	PAIR	sp; 	/* stack pointer */
	uint8_t	op; 	/* opcode */
	uint8_t	op2;	/* opcode part 2 */
	uint8_t	iff;	/* interrupt enable flip flop */
	uint8_t	psw;	/* processor status word */
	PAIR	ea; 	/* extended accumulator */
	PAIR	va; 	/* accumulator + vector register */
	PAIR	bc; 	/* 8bit B and C registers / 16bit BC register */
	PAIR	de; 	/* 8bit D and E registers / 16bit DE register */
	PAIR	hl; 	/* 8bit H and L registers / 16bit HL register */
	PAIR	ea2;	/* alternate register set */
	PAIR	va2;
	PAIR	bc2;
	PAIR	de2;
	PAIR	hl2;
	PAIR	cnt;	/* 8 bit timer counter */
	PAIR	tm; 	/* 8 bit timer 0/1 comparator inputs */
	PAIR	ecnt;	/* timer counter register / capture register */
	PAIR	etm;	/* timer 0/1 comparator inputs */
	uint8_t	ma; 	/* port A input or output mask */
	uint8_t	mb; 	/* port B input or output mask */
	uint8_t	mcc;	/* port C control/port select */
	uint8_t	mc; 	/* port C input or output mask */
	uint8_t	mm; 	/* memory mapping */
	uint8_t	mf; 	/* port F input or output mask */
	uint8_t	tmm;	/* timer 0 and timer 1 operating parameters */
	uint8_t	etmm;	/* 16-bit multifunction timer/event counter */
	uint8_t	eom;	/* 16-bit timer/event counter output control */
	uint8_t	sml;	/* serial interface parameters low */
	uint8_t	smh;	/* -"- high */
	uint8_t	anm;	/* analog to digital converter operating parameters */
	uint8_t	mkl;	/* interrupt mask low */
	uint8_t	mkh;	/* -"- high */
	uint8_t	zcm;	/* bias circuitry for ac zero-cross detection */
	uint8_t	pa_in;	/* port A,B,C,D,F inputs */
	uint8_t	pb_in;
	uint8_t	pc_in;
	uint8_t	pd_in;
	uint8_t	pf_in;
	uint8_t	pa_out; /* port A,B,C,D,F outputs */
	uint8_t	pb_out;
	uint8_t	pc_out;
	uint8_t	pd_out;
	uint8_t	pf_out;
	uint8_t	cr0;	/* analog digital conversion register 0 */
	uint8_t	cr1;	/* analog digital conversion register 1 */
	uint8_t	cr2;	/* analog digital conversion register 2 */
	uint8_t	cr3;	/* analog digital conversion register 3 */
	uint8_t	txb;	/* transmitter buffer */
	uint8_t	rxb;	/* receiver buffer */
	uint8_t	txd;	/* port C control line states */
	uint8_t	rxd;
	uint8_t	sck;
	uint8_t	ti;
	uint8_t	to;
	uint8_t	ci;
	uint8_t	co0;
	uint8_t	co1;
	uint16_t	irr;	/* interrupt request register */
	uint16_t	itf;	/* interrupt test flag register */

/* internal helper variables */
	uint16_t	txs;	/* transmitter shift register */
	uint16_t	rxs;	/* receiver shift register */
	uint8_t	txcnt;	/* transmitter shift register bit count */
	uint8_t	rxcnt;	/* receiver shift register bit count */
	uint8_t	txbuf;	/* transmitter buffer was written */
	int32_t	ovc0;	/* overflow counter for timer 0 (for clock div 12/384) */
	int32_t	ovc1;	/* overflow counter for timer 0 (for clock div 12/384) */
	int32_t	ovce;	/* overflow counter for ecnt */
	int32_t	ovcf;	/* overflow counter for fixed clock div 3 mode */
	int32_t	ovcs;	/* overflow counter for serial I/O */
	uint8_t	edges;	/* rising/falling edge flag for serial I/O */
	const struct opcode_s *opXX;	/* opcode table */
	const struct opcode_s *op48;
	const struct opcode_s *op4C;
	UPD7810_CONFIG config;
	int (*irq_callback)(int irqline);
}	UPD7810;

extern UPD7810 upd7810;

#ifdef MAME_DEBUG
extern unsigned Dasm7810( char *dst, unsigned pc );
extern unsigned Dasm7807( char *dst, unsigned pc );
#endif

#ifdef __cplusplus
}
#endif

#endif

