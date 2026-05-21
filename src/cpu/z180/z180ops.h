/***************************************************************
 * Enter HALT state; write 1 to fake port on first execution
 ***************************************************************/
#define ENTER_HALT {											\
	_PC--;														\
	_HALT = 1;													\
	if( !after_EI ) 											\
		z180_burn( z180_icount );								\
}

/***************************************************************
 * Leave HALT state; write 0 to fake port
 ***************************************************************/
#define LEAVE_HALT {											\
	if( _HALT ) 												\
	{															\
		_HALT = 0;												\
		_PC++;													\
	}															\
}

/***************************************************************
 * Input a byte from given I/O port
 ***************************************************************/
#define IN(port)												\
	(((port ^ IO_IOCR) & 0xffc0) == 0) ?						\
		z180_readcontrol(port) : cpu_readport16(port)

/***************************************************************
 * Output a byte to given I/O port
 ***************************************************************/
#define OUT(port,value) 										\
	if (((port ^ IO_IOCR) & 0xffc0) == 0)						\
		z180_writecontrol(port,value);							\
	else cpu_writeport16(port,value)

/***************************************************************
 * MMU calculate the memory managemant lookup table
 * bb and cb specify a 4K page
 * If the 4 most significant bits of an 16 bit address are
 * greater or equal to the bank base, the bank base register
 * specifies the 4K offset into the 20 bit address space.
 * If the 4 bits are also greater or equal to the common base,
 * the common base register is used to specify the offset.
 ***************************************************************/
static INLINE void z180_mmu( void )
{
	uint32_t addr, page, bb, cb;
	bb = IO_CBAR & 15;
	cb = IO_CBAR >> 4;
	for( page = 0; page < 16; page++ )
	{
		addr = page << 12;
		if (page >= bb)
		{
			if (page >= cb)
				addr += IO_CBR << 12;
			else
				addr += IO_BBR << 12;
		}
		Z180.mmu[page] = addr;
	}
}


#define MMU_REMAP_ADDR(addr) (Z180.mmu[((addr)>>12)&15]|((addr)&4095))

/***************************************************************
 * Read a byte from given memory location
 ***************************************************************/
#define RM(addr)	cpu_readmem20(MMU_REMAP_ADDR(addr))
uint8_t cpu_readmemz180(uint32_t offset)
{
	return RM(offset);
}

/***************************************************************
 * Write a byte to given memory location
 ***************************************************************/
#define WM(addr,value) cpu_writemem20(MMU_REMAP_ADDR(addr),value)
void cpu_writememz180(uint32_t offset, uint8_t data)
{
	WM(offset, data);
}

/***************************************************************
 * Read a word from given memory location
 ***************************************************************/
static INLINE void RM16( uint32_t addr, PAIR *r )
{
	r->b.l = RM(addr);
	r->b.h = RM(addr+1);
}

/***************************************************************
 * Write a word to given memory location
 ***************************************************************/
static INLINE void WM16( uint32_t addr, PAIR *r )
{
	WM(addr,r->b.l);
	WM(addr+1,r->b.h);
}

/***************************************************************
 * ROP() is identical to RM() except it is used for
 * reading opcodes. In case of system with memory mapped I/O,
 * this function can be used to greatly speed up emulation
 ***************************************************************/
static INLINE uint8_t ROP(void)
{
	uint32_t addr = _PCD;
	_PC++;
	return cpu_readop(MMU_REMAP_ADDR(addr));
}

/****************************************************************
 * ARG() is identical to ROP() except it is used
 * for reading opcode arguments. This difference can be used to
 * support systems that use different encoding mechanisms for
 * opcodes and opcode arguments
 ***************************************************************/
static INLINE uint8_t ARG(void)
{
	uint32_t addr = _PCD;
	_PC++;
	return cpu_readop_arg(MMU_REMAP_ADDR(addr));
}

static INLINE uint32_t ARG16(void)
{
	uint32_t addr = _PCD;
	_PC += 2;
	return cpu_readop_arg(MMU_REMAP_ADDR(addr)) | (cpu_readop_arg(MMU_REMAP_ADDR(addr+1)) << 8);
}

/****************************************************************************
 * Change program counter - MMU lookup
 ****************************************************************************/
#define z180_change_pc(addr) change_pc20(MMU_REMAP_ADDR(addr))
void cpu_setOPbasez180(int pc)
{
	z180_change_pc(pc);
}

/***************************************************************
 * Calculate the effective addess EA of an opcode using
 * IX+offset resp. IY+offset addressing.
 ***************************************************************/
#define EAX EA = (uint32_t)(uint16_t)(_IX+(int8_t)ARG())
#define EAY EA = (uint32_t)(uint16_t)(_IY+(int8_t)ARG())

/***************************************************************
 * POP
 ***************************************************************/
#define POP(DR) { RM16( _SPD, &Z180.DR ); _SP += 2; }

/***************************************************************
 * PUSH
 ***************************************************************/
#define PUSH(SR) { _SP -= 2; WM16( _SPD, &Z180.SR ); }

/***************************************************************
 * JP
 ***************************************************************/
#if BUSY_LOOP_HACKS
#define JP {													\
	unsigned oldpc = _PCD-1;									\
	_PCD = ARG16(); 											\
	z180_change_pc(_PCD);										\
	/* speed up busy loop */									\
	if( _PCD == oldpc ) 										\
	{															\
		if( !after_EI ) 										\
			BURNODD( z180_icount, 1, cc[Z180_TABLE_op][0xc3] ); \
	}															\
	else														\
	{															\
		uint8_t op = cpu_readop(_PCD);							\
		if( _PCD == oldpc-1 )									\
		{														\
			/* NOP - JP $-1 or EI - JP $-1 */					\
			if ( op == 0x00 || op == 0xfb ) 					\
			{													\
				if( !after_EI ) 								\
					BURNODD( z180_icount-cc[Z180_TABLE_op][0x00],\
						2, cc[Z180_TABLE_op][0x00]+cc[Z180_TABLE_op][0xc3]); \
			}													\
		}														\
		else													\
		/* LD SP,#xxxx - JP $-3 (Galaga) */ 					\
		if( _PCD == oldpc-3 && op == 0x31 ) 					\
		{														\
			if( !after_EI ) 									\
				BURNODD( z180_icount-cc[Z180_TABLE_op][0x31],	\
					2, cc[Z180_TABLE_op][0x31]+cc[Z180_TABLE_op][0xc3]); \
		}														\
	}															\
}
#else
#define JP {													\
	_PCD = ARG16(); 											\
	z180_change_pc(_PCD);										\
}
#endif

/***************************************************************
 * JP_COND
 ***************************************************************/

#define JP_COND(cond)											\
	if( cond )													\
	{															\
		_PCD = ARG16(); 										\
		z180_change_pc(_PCD);									\
	}															\
	else														\
	{															\
		_PC += 2;												\
	}

/***************************************************************
 * JR
 ***************************************************************/
#define JR()													\
{																\
	unsigned oldpc = _PCD-1;									\
	int8_t arg = (int8_t)ARG(); /* ARG() also increments _PC */ 	\
	_PC += arg; 			/* so don't do _PC += ARG() */      \
	z180_change_pc(_PCD);										\
	/* speed up busy loop */									\
	if( _PCD == oldpc ) 										\
	{															\
		if( !after_EI ) 										\
			BURNODD( z180_icount, 1, cc[Z180_TABLE_op][0x18] ); \
	}															\
	else														\
	{															\
		uint8_t op = cpu_readop(_PCD);							\
		if( _PCD == oldpc-1 )									\
		{														\
			/* NOP - JR $-1 or EI - JR $-1 */					\
			if ( op == 0x00 || op == 0xfb ) 					\
			{													\
				if( !after_EI ) 								\
				   BURNODD( z180_icount-cc[Z180_TABLE_op][0x00],\
					   2, cc[Z180_TABLE_op][0x00]+cc[Z180_TABLE_op][0x18]); \
			}													\
		}														\
		else													\
		/* LD SP,#xxxx - JR $-3 */								\
		if( _PCD == oldpc-3 && op == 0x31 ) 					\
		{														\
			if( !after_EI ) 									\
			   BURNODD( z180_icount-cc[Z180_TABLE_op][0x31],	\
				   2, cc[Z180_TABLE_op][0x31]+cc[Z180_TABLE_op][0x18]); \
		}														\
	}															\
}

/***************************************************************
 * JR_COND
 ***************************************************************/
#define JR_COND(cond,opcode)									\
	if( cond )													\
	{															\
		int8_t arg = (int8_t)ARG(); /* ARG() also increments _PC */ \
		_PC += arg; 			/* so don't do _PC += ARG() */  \
		CC(ex,opcode);											\
		z180_change_pc(_PCD);									\
	}															\
	else _PC++; 												\

/***************************************************************
 * CALL
 ***************************************************************/
#define CALL()													\
	EA = ARG16();												\
	PUSH( PC ); 												\
	_PCD = EA;													\
	z180_change_pc(_PCD)

/***************************************************************
 * CALL_COND
 ***************************************************************/
#define CALL_COND(cond,opcode)									\
	if( cond )													\
	{															\
		EA = ARG16();											\
		PUSH( PC ); 											\
		_PCD = EA;												\
		CC(ex,opcode);											\
		z180_change_pc(_PCD);									\
	}															\
	else														\
	{															\
		_PC+=2; 												\
	}

/***************************************************************
 * RET_COND
 ***************************************************************/
#define RET_COND(cond,opcode)									\
	if( cond )													\
	{															\
		POP(PC);												\
		z180_change_pc(_PCD);									\
		CC(ex,opcode);											\
	}

/***************************************************************
 * RETN
 ***************************************************************/
#define RETN	{												\
	LOG(("Z180 #%d RETN IFF1:%d IFF2:%d\n", cpu_getactivecpu(), _IFF1, _IFF2)); \
	POP(PC);													\
	z180_change_pc(_PCD);										\
	if( _IFF1 == 0 && _IFF2 == 1 )								\
	{															\
		_IFF1 = 1;												\
		if( Z180.irq_state[0] != CLEAR_LINE ||					\
			Z180.request_irq >= 0 ) 							\
		{														\
			LOG(("Z180 #%d RETN takes INT0\n",                  \
				cpu_getactivecpu()));							\
			take_interrupt(Z180_INT0);							\
		}														\
		else if( Z180.irq_state[1] != CLEAR_LINE )				\
		{														\
			LOG(("Z180 #%d RETN takes INT1\n",                  \
				cpu_getactivecpu()));							\
			take_interrupt(Z180_INT1);							\
		}														\
		else if( Z180.irq_state[2] != CLEAR_LINE )				\
		{														\
			LOG(("Z180 #%d RETN takes INT2\n",                  \
				cpu_getactivecpu()));							\
			take_interrupt(Z180_INT2);							\
		}														\
	}															\
	else _IFF1 = _IFF2; 										\
}

/***************************************************************
 * RETI
 ***************************************************************/
#define RETI	{												\
	int device = Z180.service_irq;								\
	POP(PC);													\
	z180_change_pc(_PCD);										\
/* according to http://www.msxnet.org/tech/Z80/z80undoc.txt */	\
/*	_IFF1 = _IFF2;	*/											\
	if( device >= 0 )											\
	{															\
		LOG(("Z180 #%d RETI device %d: $%02x\n",                \
			cpu_getactivecpu(), device, Z180.irq[device].irq_param)); \
		Z180.irq[device].interrupt_reti(Z180.irq[device].irq_param); \
	}															\
}

/***************************************************************
 * LD	R,A
 ***************************************************************/
#define LD_R_A {												\
	_R = _A;													\
	_R2 = _A & 0x80;				/* keep bit 7 of R */		\
}

/***************************************************************
 * LD	A,R
 ***************************************************************/
#define LD_A_R {												\
	_A = (_R & 0x7f) | _R2; 									\
	_F = (_F & CF) | SZ[_A] | ( _IFF2 << 2 );					\
}

/***************************************************************
 * LD	I,A
 ***************************************************************/
#define LD_I_A {												\
	_I = _A;													\
}

/***************************************************************
 * LD	A,I
 ***************************************************************/
#define LD_A_I {												\
	_A = _I;													\
	_F = (_F & CF) | SZ[_A] | ( _IFF2 << 2 );					\
}

/***************************************************************
 * RST
 ***************************************************************/
#define RST(addr)												\
	PUSH( PC ); 												\
	_PCD = addr;												\
	z180_change_pc(_PCD)

/***************************************************************
 * INC	r8
 ***************************************************************/
static INLINE uint8_t INC(uint8_t value)
{
	uint8_t res = value + 1;
	_F = (_F & CF) | SZHV_inc[res];
	return (uint8_t)res;
}

/***************************************************************
 * DEC	r8
 ***************************************************************/
static INLINE uint8_t DEC(uint8_t value)
{
	uint8_t res = value - 1;
	_F = (_F & CF) | SZHV_dec[res];
	return res;
}

/***************************************************************
 * RLCA
 ***************************************************************/
#if Z180_EXACT
#define RLCA													\
	_A = (_A << 1) | (_A >> 7); 								\
	_F = (_F & (SF | ZF | PF)) | (_A & (YF | XF | CF))
#else
#define RLCA													\
	_A = (_A << 1) | (_A >> 7); 								\
	_F = (_F & (SF | ZF | YF | XF | PF)) | (_A & CF)
#endif

/***************************************************************
 * RRCA
 ***************************************************************/
#if Z180_EXACT
#define RRCA													\
	_F = (_F & (SF | ZF | PF)) | (_A & (YF | XF | CF)); 		\
	_A = (_A >> 1) | (_A << 7)
#else
#define RRCA													\
	_F = (_F & (SF | ZF | YF | XF | PF)) | (_A & CF);			\
	_A = (_A >> 1) | (_A << 7)
#endif

/***************************************************************
 * RLA
 ***************************************************************/
#if Z180_EXACT
#define RLA {													\
	uint8_t res = (_A << 1) | (_F & CF);							\
	uint8_t c = (_A & 0x80) ? CF : 0; 							\
	_F = (_F & (SF | ZF | PF)) | c | (res & (YF | XF)); 		\
	_A = res;													\
}
#else
#define RLA {													\
	uint8_t res = (_A << 1) | (_F & CF);							\
	uint8_t c = (_A & 0x80) ? CF : 0; 							\
	_F = (_F & (SF | ZF | YF | XF | PF)) | c;					\
	_A = res;													\
}
#endif

/***************************************************************
 * RRA
 ***************************************************************/
#if Z180_EXACT
#define RRA {													\
	uint8_t res = (_A >> 1) | (_F << 7);							\
	uint8_t c = (_A & 0x01) ? CF : 0; 							\
	_F = (_F & (SF | ZF | PF)) | c | (res & (YF | XF)); 		\
	_A = res;													\
}
#else
#define RRA {													\
	uint8_t res = (_A >> 1) | (_F << 7);							\
	uint8_t c = (_A & 0x01) ? CF : 0; 							\
	_F = (_F & (SF | ZF | YF | XF | PF)) | c;					\
	_A = res;													\
}
#endif

/***************************************************************
 * RRD
 ***************************************************************/
#define RRD {													\
	uint8_t n = RM(_HL);											\
	WM( _HL, (n >> 4) | (_A << 4) );							\
	_A = (_A & 0xf0) | (n & 0x0f);								\
	_F = (_F & CF) | SZP[_A];									\
}

/***************************************************************
 * RLD
 ***************************************************************/
#define RLD {													\
	uint8_t n = RM(_HL);											\
	WM( _HL, (n << 4) | (_A & 0x0f) );							\
	_A = (_A & 0xf0) | (n >> 4);								\
	_F = (_F & CF) | SZP[_A];									\
}

/***************************************************************
 * ADD	A,n
 ***************************************************************/
#if BIG_FLAGS_ARRAY
#define ADD(value)												\
{																\
	uint32_t ah = _AFD & 0xff00;									\
	uint32_t res = (uint8_t)((ah >> 8) + value);					\
	_F = SZHVC_add[ah | res];									\
	_A = res;													\
}
#else
#define ADD(value)												\
{																\
	unsigned val = value;										\
	unsigned res = _A + val;									\
	_F = SZ[(uint8_t)res] | ((res >> 8) & CF) |					\
		((_A ^ res ^ val) & HF) |								\
		(((val ^ _A ^ 0x80) & (val ^ res) & 0x80) >> 5);		\
	_A = (uint8_t)res;											\
}
#endif

/***************************************************************
 * ADC	A,n
 ***************************************************************/
#if BIG_FLAGS_ARRAY
#define ADC(value)												\
{																\
	uint32_t ah = _AFD & 0xff00, c = _AFD & 1;					\
	uint32_t res = (uint8_t)((ah >> 8) + value + c);				\
	_F = SZHVC_add[(c << 16) | ah | res];						\
	_A = res;													\
}
#else
#define ADC(value)												\
{																\
	unsigned val = value;										\
	unsigned res = _A + val + (_F & CF);						\
	_F = SZ[res & 0xff] | ((res >> 8) & CF) |					\
		((_A ^ res ^ val) & HF) |								\
		(((val ^ _A ^ 0x80) & (val ^ res) & 0x80) >> 5);		\
	_A = res;													\
}
#endif

/***************************************************************
 * SUB	n
 ***************************************************************/
#if BIG_FLAGS_ARRAY
#define SUB(value)												\
{																\
	uint32_t ah = _AFD & 0xff00;									\
	uint32_t res = (uint8_t)((ah >> 8) - value);					\
	_F = SZHVC_sub[ah | res];									\
	_A = res;													\
}
#else
#define SUB(value)												\
{																\
	unsigned val = value;										\
	unsigned res = _A - val;									\
	_F = SZ[res & 0xff] | ((res >> 8) & CF) | NF |				\
		((_A ^ res ^ val) & HF) |								\
		(((val ^ _A) & (_A ^ res) & 0x80) >> 5);				\
	_A = res;													\
}
#endif

/***************************************************************
 * SBC	A,n
 ***************************************************************/
#if BIG_FLAGS_ARRAY
#define SBC(value)												\
{																\
	uint32_t ah = _AFD & 0xff00, c = _AFD & 1;					\
	uint32_t res = (uint8_t)((ah >> 8) - value - c);				\
	_F = SZHVC_sub[(c<<16) | ah | res]; 						\
	_A = res;													\
}
#else
#define SBC(value)												\
{																\
	unsigned val = value;										\
	unsigned res = _A - val - (_F & CF);						\
	_F = SZ[res & 0xff] | ((res >> 8) & CF) | NF |				\
		((_A ^ res ^ val) & HF) |								\
		(((val ^ _A) & (_A ^ res) & 0x80) >> 5);				\
	_A = res;													\
}
#endif

/***************************************************************
 * NEG
 ***************************************************************/
#define NEG {													\
	uint8_t value = _A;											\
	_A = 0; 													\
	SUB(value); 												\
}

/***************************************************************
 * DAA
 ***************************************************************/
#define DAA {													\
	int idx = _A;												\
	if( _F & CF ) idx |= 0x100; 								\
	if( _F & HF ) idx |= 0x200; 								\
	if( _F & NF ) idx |= 0x400; 								\
	_AF = DAATable[idx];										\
}

/***************************************************************
 * AND	n
 ***************************************************************/
#define AND(value)												\
	_A &= value;												\
	_F = SZP[_A] | HF

/***************************************************************
 * OR	n
 ***************************************************************/
#define OR(value)												\
	_A |= value;												\
	_F = SZP[_A]

/***************************************************************
 * XOR	n
 ***************************************************************/
#define XOR(value)												\
	_A ^= value;												\
	_F = SZP[_A]

/***************************************************************
 * CP	n
 ***************************************************************/
#if BIG_FLAGS_ARRAY
#define CP(value)												\
{																\
	uint32_t ah = _AFD & 0xff00;									\
	uint32_t res = (uint8_t)((ah >> 8) - value);					\
	_F = SZHVC_sub[ah | res];									\
}
#else
#define CP(value)												\
{																\
	unsigned val = value;										\
	unsigned res = _A - val;									\
	_F = SZ[res & 0xff] | ((res >> 8) & CF) | NF |				\
		((_A ^ res ^ val) & HF) |								\
		((((val ^ _A) & (_A ^ res)) >> 5) & VF);				\
}
#endif

/***************************************************************
 * EX	AF,AF'
 ***************************************************************/
#define EX_AF { 												\
	PAIR tmp;													\
	tmp = Z180.AF; Z180.AF = Z180.AF2; Z180.AF2 = tmp;			\
}

/***************************************************************
 * EX	DE,HL
 ***************************************************************/
#define EX_DE_HL {												\
	PAIR tmp;													\
	tmp = Z180.DE; Z180.DE = Z180.HL; Z180.HL = tmp;			\
}

/***************************************************************
 * EXX
 ***************************************************************/
#define EXX {													\
	PAIR tmp;													\
	tmp = Z180.BC; Z180.BC = Z180.BC2; Z180.BC2 = tmp;			\
	tmp = Z180.DE; Z180.DE = Z180.DE2; Z180.DE2 = tmp;			\
	tmp = Z180.HL; Z180.HL = Z180.HL2; Z180.HL2 = tmp;			\
}

/***************************************************************
 * EX	(SP),r16
 ***************************************************************/
#define EXSP(DR)												\
{																\
	PAIR tmp = { { 0, 0, 0, 0 } };								\
	RM16( _SPD, &tmp ); 										\
	WM16( _SPD, &Z180.DR ); 									\
	Z180.DR = tmp;												\
}


/***************************************************************
 * ADD16
 ***************************************************************/
#define ADD16(DR,SR)											\
{																\
	uint32_t res = Z180.DR.d + Z180.SR.d; 						\
	_F = (_F & (SF | ZF | VF)) |								\
		(((Z180.DR.d ^ res ^ Z180.SR.d) >> 8) & HF) |			\
		((res >> 16) & CF); 									\
	Z180.DR.w.l = (uint16_t)res;									\
}

/***************************************************************
 * ADC	r16,r16
 ***************************************************************/
#define ADC16(DR)												\
{																\
	uint32_t res = _HLD + Z180.DR.d + (_F & CF);					\
	_F = (((_HLD ^ res ^ Z180.DR.d) >> 8) & HF) |				\
		((res >> 16) & CF) |									\
		((res >> 8) & SF) | 									\
		((res & 0xffff) ? 0 : ZF) | 							\
		(((Z180.DR.d ^ _HLD ^ 0x8000) & (Z180.DR.d ^ res) & 0x8000) >> 13); \
	_HL = (uint16_t)res;											\
}

/***************************************************************
 * SBC	r16,r16
 ***************************************************************/
#define SBC16(DR)												\
{																\
	uint32_t res = _HLD - Z180.DR.d - (_F & CF);					\
	_F = (((_HLD ^ res ^ Z180.DR.d) >> 8) & HF) | NF |			\
		((res >> 16) & CF) |									\
		((res >> 8) & SF) | 									\
		((res & 0xffff) ? 0 : ZF) | 							\
		(((Z180.DR.d ^ _HLD) & (_HLD ^ res) &0x8000) >> 13);	\
	_HL = (uint16_t)res;											\
}

/***************************************************************
 * RLC	r8
 ***************************************************************/
static INLINE uint8_t RLC(uint8_t value)
{
	unsigned res = value;
	unsigned c = (res & 0x80) ? CF : 0;
	res = ((res << 1) | (res >> 7)) & 0xff;
	_F = SZP[res] | c;
	return res;
}

/***************************************************************
 * RRC	r8
 ***************************************************************/
static INLINE uint8_t RRC(uint8_t value)
{
	unsigned res = value;
	unsigned c = (res & 0x01) ? CF : 0;
	res = ((res >> 1) | (res << 7)) & 0xff;
	_F = SZP[res] | c;
	return res;
}

/***************************************************************
 * RL	r8
 ***************************************************************/
static INLINE uint8_t RL(uint8_t value)
{
	unsigned res = value;
	unsigned c = (res & 0x80) ? CF : 0;
	res = ((res << 1) | (_F & CF)) & 0xff;
	_F = SZP[res] | c;
	return res;
}

/***************************************************************
 * RR	r8
 ***************************************************************/
static INLINE uint8_t RR(uint8_t value)
{
	unsigned res = value;
	unsigned c = (res & 0x01) ? CF : 0;
	res = ((res >> 1) | (_F << 7)) & 0xff;
	_F = SZP[res] | c;
	return res;
}

/***************************************************************
 * SLA	r8
 ***************************************************************/
static INLINE uint8_t SLA(uint8_t value)
{
	unsigned res = value;
	unsigned c = (res & 0x80) ? CF : 0;
	res = (res << 1) & 0xff;
	_F = SZP[res] | c;
	return res;
}

/***************************************************************
 * SRA	r8
 ***************************************************************/
static INLINE uint8_t SRA(uint8_t value)
{
	unsigned res = value;
	unsigned c = (res & 0x01) ? CF : 0;
	res = ((res >> 1) | (res & 0x80)) & 0xff;
	_F = SZP[res] | c;
	return res;
}

/***************************************************************
 * SLL	r8
 ***************************************************************/
static INLINE uint8_t SLL(uint8_t value)
{
	unsigned res = value;
	unsigned c = (res & 0x80) ? CF : 0;
	res = ((res << 1) | 0x01) & 0xff;
	_F = SZP[res] | c;
	return res;
}

/***************************************************************
 * SRL	r8
 ***************************************************************/
static INLINE uint8_t SRL(uint8_t value)
{
	unsigned res = value;
	unsigned c = (res & 0x01) ? CF : 0;
	res = (res >> 1) & 0xff;
	_F = SZP[res] | c;
	return res;
}

/***************************************************************
 * BIT	bit,r8
 ***************************************************************/
#undef BIT
#define BIT(bit,reg)											\
	_F = (_F & CF) | HF | SZ_BIT[reg & (1<<bit)]

/***************************************************************
 * BIT	bit,(IX/Y+o)
 ***************************************************************/
#if Z180_EXACT
#define BIT_XY(bit,reg) 										\
	_F = (_F & CF) | HF | (SZ_BIT[reg & (1<<bit)] & ~(YF|XF)) | ((EA>>8) & (YF|XF))
#else
#define BIT_XY	BIT
#endif

/***************************************************************
 * RES	bit,r8
 ***************************************************************/
static INLINE uint8_t RES(uint8_t bit, uint8_t value)
{
	return value & ~(1<<bit);
}

/***************************************************************
 * SET	bit,r8
 ***************************************************************/
static INLINE uint8_t SET(uint8_t bit, uint8_t value)
{
	return value | (1<<bit);
}

/***************************************************************
 * LDI
 ***************************************************************/
#if Z180_EXACT
#define LDI {													\
	uint8_t io = RM(_HL); 										\
	WM( _DE, io );												\
	_F &= SF | ZF | CF; 										\
	if( (_A + io) & 0x02 ) _F |= YF; /* bit 1 -> flag 5 */		\
	if( (_A + io) & 0x08 ) _F |= XF; /* bit 3 -> flag 3 */		\
	_HL++; _DE++; _BC--;										\
	if( _BC ) _F |= VF; 										\
}
#else
#define LDI {													\
	WM( _DE, RM(_HL) ); 										\
	_F &= SF | ZF | YF | XF | CF;								\
	_HL++; _DE++; _BC--;										\
	if( _BC ) _F |= VF; 										\
}
#endif

/***************************************************************
 * CPI
 ***************************************************************/
#if Z180_EXACT
#define CPI {													\
	uint8_t val = RM(_HL);										\
	uint8_t res = _A - val;										\
	_HL++; _BC--;												\
	_F = (_F & CF) | (SZ[res] & ~(YF|XF)) | ((_A ^ val ^ res) & HF) | NF;  \
	if( _F & HF ) res -= 1; 									\
	if( res & 0x02 ) _F |= YF; /* bit 1 -> flag 5 */			\
	if( res & 0x08 ) _F |= XF; /* bit 3 -> flag 3 */			\
	if( _BC ) _F |= VF; 										\
}
#else
#define CPI {													\
	uint8_t val = RM(_HL);										\
	uint8_t res = _A - val;										\
	_HL++; _BC--;												\
	_F = (_F & CF) | SZ[res] | ((_A ^ val ^ res) & HF) | NF;	\
	if( _BC ) _F |= VF; 										\
}
#endif

/***************************************************************
 * INI
 ***************************************************************/
#if Z180_EXACT
#define INI {													\
	uint8_t io = IN(_BC); 										\
	_B--;														\
	WM( _HL, io );												\
	_HL++;														\
	_F = SZ[_B];												\
	if( io & SF ) _F |= NF; 									\
	if( (_C + io + 1) & 0x100 ) _F |= HF | CF;					\
	if( (irep_tmp1[_C & 3][io & 3] ^							\
		 breg_tmp2[_B] ^										\
		 (_C >> 2) ^											\
		 (io >> 2)) & 1 )										\
		_F |= PF;												\
}
#else
#define INI {													\
	_B--;														\
	WM( _HL, IN(_BC) ); 										\
	_HL++;														\
	_F = (_B) ? NF : NF | ZF;									\
}
#endif

/***************************************************************
 * OUTI
 ***************************************************************/
#if Z180_EXACT
#define OUTI {													\
	uint8_t io = RM(_HL); 										\
	_B--;														\
	OUT( _BC, io ); 											\
	_HL++;														\
	_F = SZ[_B];												\
	if( io & SF ) _F |= NF; 									\
	if( (_C + io + 1) & 0x100 ) _F |= HF | CF;					\
	if( (irep_tmp1[_C & 3][io & 3] ^							\
		 breg_tmp2[_B] ^										\
		 (_C >> 2) ^											\
		 (io >> 2)) & 1 )										\
		_F |= PF;												\
}
#else
#define OUTI {													\
	_B--;														\
	OUT( _BC, RM(_HL) );										\
	_HL++;														\
	_F = (_B) ? NF : NF | ZF;									\
}
#endif

/***************************************************************
 * LDD
 ***************************************************************/
#if Z180_EXACT
#define LDD {													\
	uint8_t io = RM(_HL); 										\
	WM( _DE, io );												\
	_F &= SF | ZF | CF; 										\
	if( (_A + io) & 0x02 ) _F |= YF; /* bit 1 -> flag 5 */		\
	if( (_A + io) & 0x08 ) _F |= XF; /* bit 3 -> flag 3 */		\
	_HL--; _DE--; _BC--;										\
	if( _BC ) _F |= VF; 										\
}
#else
#define LDD {													\
	WM( _DE, RM(_HL) ); 										\
	_F &= SF | ZF | YF | XF | CF;								\
	_HL--; _DE--; _BC--;										\
	if( _BC ) _F |= VF; 										\
}
#endif

/***************************************************************
 * CPD
 ***************************************************************/
#if Z180_EXACT
#define CPD {													\
	uint8_t val = RM(_HL);										\
	uint8_t res = _A - val;										\
	_HL--; _BC--;												\
	_F = (_F & CF) | (SZ[res] & ~(YF|XF)) | ((_A ^ val ^ res) & HF) | NF;  \
	if( _F & HF ) res -= 1; 									\
	if( res & 0x02 ) _F |= YF; /* bit 1 -> flag 5 */			\
	if( res & 0x08 ) _F |= XF; /* bit 3 -> flag 3 */			\
	if( _BC ) _F |= VF; 										\
}
#else
#define CPD {													\
	uint8_t val = RM(_HL);										\
	uint8_t res = _A - val;										\
	_HL--; _BC--;												\
	_F = (_F & CF) | SZ[res] | ((_A ^ val ^ res) & HF) | NF;	\
	if( _BC ) _F |= VF; 										\
}
#endif

/***************************************************************
 * IND
 ***************************************************************/
#if Z180_EXACT
#define IND {													\
	uint8_t io = IN(_BC); 										\
	_B--;														\
	WM( _HL, io );												\
	_HL--;														\
	_F = SZ[_B];												\
	if( io & SF ) _F |= NF; 									\
	if( (_C + io - 1) & 0x100 ) _F |= HF | CF;					\
	if( (drep_tmp1[_C & 3][io & 3] ^							\
		 breg_tmp2[_B] ^										\
		 (_C >> 2) ^											\
		 (io >> 2)) & 1 )										\
		_F |= PF;												\
}
#else
#define IND {													\
	_B--;														\
	WM( _HL, IN(_BC) ); 										\
	_HL--;														\
	_F = (_B) ? NF : NF | ZF;									\
}
#endif

/***************************************************************
 * OUTD
 ***************************************************************/
#if Z180_EXACT
#define OUTD {													\
	uint8_t io = RM(_HL); 										\
	_B--;														\
	OUT( _BC, io ); 											\
	_HL--;														\
	_F = SZ[_B];												\
	if( io & SF ) _F |= NF; 									\
	if( (_C + io - 1) & 0x100 ) _F |= HF | CF;					\
	if( (drep_tmp1[_C & 3][io & 3] ^							\
		 breg_tmp2[_B] ^										\
		 (_C >> 2) ^											\
		 (io >> 2)) & 1 )										\
		_F |= PF;												\
}
#else
#define OUTD {													\
	_B--;														\
	OUT( _BC, RM(_HL) );										\
	_HL--;														\
	_F = (_B) ? NF : NF | ZF;									\
}
#endif

/***************************************************************
 * LDIR
 ***************************************************************/
#define LDIR													\
	LDI;														\
	if( _BC )													\
	{															\
		_PC -= 2;												\
		CC(ex,0xb0);											\
	}

/***************************************************************
 * CPIR
 ***************************************************************/
#define CPIR													\
	CPI;														\
	if( _BC && !(_F & ZF) ) 									\
	{															\
		_PC -= 2;												\
		CC(ex,0xb1);											\
	}

/***************************************************************
 * INIR
 ***************************************************************/
#define INIR													\
	INI;														\
	if( _B )													\
	{															\
		_PC -= 2;												\
		CC(ex,0xb2);											\
	}

/***************************************************************
 * OTIR
 ***************************************************************/
#define OTIR													\
	OUTI;														\
	if( _B )													\
	{															\
		_PC -= 2;												\
		CC(ex,0xb3);											\
	}

/***************************************************************
 * LDDR
 ***************************************************************/
#define LDDR													\
	LDD;														\
	if( _BC )													\
	{															\
		_PC -= 2;												\
		CC(ex,0xb8);											\
	}

/***************************************************************
 * CPDR
 ***************************************************************/
#define CPDR													\
	CPD;														\
	if( _BC && !(_F & ZF) ) 									\
	{															\
		_PC -= 2;												\
		CC(ex,0xb9);											\
	}

/***************************************************************
 * INDR
 ***************************************************************/
#define INDR													\
	IND;														\
	if( _B )													\
	{															\
		_PC -= 2;												\
		CC(ex,0xba);											\
	}

/***************************************************************
 * OTDR
 ***************************************************************/
#define OTDR													\
	OUTD;														\
	if( _B )													\
	{															\
		_PC -= 2;												\
		CC(ex,0xbb);											\
	}

/***************************************************************
 * EI
 ***************************************************************/
#define EI {													\
	/* If interrupts were disabled, execute one more			\
	 * instruction and check the IRQ line.						\
	 * If not, simply set interrupt flip-flop 2 				\
	 */ 														\
	if( _IFF1 == 0 )											\
	{															\
		_IFF1 = _IFF2 = 1;										\
		_PPC = _PCD;											\
		CALL_MAME_DEBUG;										\
		_R++;													\
		while( cpu_readop(_PCD) == 0xfb ) /* more EIs? */		\
		{														\
			LOG(("Z180 #%d multiple EI opcodes at %04X\n",      \
				cpu_getactivecpu(), _PC));						\
			CC(op,0xfb);										\
			_PPC =_PCD; 										\
			CALL_MAME_DEBUG;									\
			_PC++;												\
			_R++;												\
		}														\
		if( Z180.irq_state[0] != CLEAR_LINE ||					\
			Z180.request_irq >= 0 ) 							\
		{														\
			after_EI = 1;	/* avoid cycle skip hacks */		\
			EXEC(op,ROP()); 									\
			after_EI = 0;										\
			LOG(("Z180 #%d EI takes INT0\n", cpu_getactivecpu())); \
			take_interrupt(Z180_INT0);							\
		}														\
		else if( Z180.irq_state[1] != CLEAR_LINE )				\
		{														\
			after_EI = 1;	/* avoid cycle skip hacks */		\
			EXEC(op,ROP()); 									\
			after_EI = 0;										\
			LOG(("Z180 #%d EI takes INT1\n", cpu_getactivecpu())); \
			take_interrupt(Z180_INT1);							\
		}														\
		else if( Z180.irq_state[2] != CLEAR_LINE )				\
		{														\
			after_EI = 1;	/* avoid cycle skip hacks */		\
			EXEC(op,ROP()); 									\
			after_EI = 0;										\
			LOG(("Z180 #%d EI takes INT2\n", cpu_getactivecpu())); \
			take_interrupt(Z180_INT2);							\
		} else EXEC(op,ROP());									\
	} else _IFF2 = 1;											\
}

/***************************************************************
 * TST	n
 ***************************************************************/
#define TST(value)												\
	_F = SZP[_A & value] | HF

/***************************************************************
 * MLT	rr
 ***************************************************************/
#define MLT(DR) {												\
	Z180.DR.w.l = Z180.DR.b.l * Z180.DR.b.h;					\
}

/***************************************************************
 * OTIM
 ***************************************************************/
#define OTIM {													\
	_B--;														\
	OUT( _C, RM(_HL) ); 										\
	_HL++;														\
	_C++;														\
	_F = (_B) ? NF : NF | ZF;									\
}

/***************************************************************
 * OTDM
 ***************************************************************/
#define OTDM {													\
	_B--;														\
	OUT( _C, RM(_HL) ); 										\
	_HL--;														\
	_C--;														\
	_F = (_B) ? NF : NF | ZF;									\
}

/***************************************************************
 * OTIMR
 ***************************************************************/
#define OTIMR													\
	OTIM;														\
	if( _B )													\
	{															\
		_PC -= 2;												\
		CC(ex,0xb3);											\
	}

/***************************************************************
 * OTDMR
 ***************************************************************/
#define OTDMR													\
	OTDM;														\
	if( _B )													\
	{															\
		_PC -= 2;												\
		CC(ex,0xb3);											\
	}

/***************************************************************
 * OTDMR
 ***************************************************************/
#define SLP {													\
	z180_icount = 0;											\
	_HALT = 2;													\
}

