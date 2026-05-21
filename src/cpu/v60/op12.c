/*
 * MUL* and MULU* do not set OV correctly
 * DIVX: the second operand should be treated as dword instead of word
 * GETATE, GETPTE and GETRA should not be used
 * UPDPSW: _CY and _OV must be cleared or unchanged? I suppose
 *   cleared, like TEST being done on the mask operand.
 * MOVT: I cannot understand exactly what happens to the result
 *   when an overflow occurs
 *
 * Unimplemented opcodes:
 * ROTC, UPDATE, UPDPTE
 */

uint32_t f12Op1, f12Op2;
uint8_t f12Flag1, f12Flag2;


/*
 *  Macro to access data in operands decoded with ReadAMAddress()
 */

#define F12LOADOPBYTE(num)			  \
	if (f12Flag##num)								\
		appb = (uint8_t)v60.reg[f12Op##num];  \
	else														\
		appb = MemRead8(f12Op##num);

#define F12LOADOPHALF(num)			  \
	if (f12Flag##num)								\
		apph = (uint16_t)v60.reg[f12Op##num];  \
	else														\
		apph = MemRead16(f12Op##num);

#define F12LOADOPWORD(num)			  \
	if (f12Flag##num)								\
		appw = v60.reg[f12Op##num];  \
	else														\
		appw = MemRead32(f12Op##num);

#define F12STOREOPBYTE(num)				\
	if (f12Flag##num)								\
		SETREG8(v60.reg[f12Op##num], appb);	\
	else														\
		MemWrite8(f12Op##num,appb);

#define F12STOREOPHALF(num)				\
	if (f12Flag##num)								\
		SETREG16(v60.reg[f12Op##num], apph);	\
	else														\
		MemWrite16(f12Op##num,apph);

#define F12STOREOPWORD(num)				\
	if (f12Flag##num)								\
		v60.reg[f12Op##num] = appw;	\
	else														\
		MemWrite32(f12Op##num,appw);

#define F12LOADOP1BYTE()  F12LOADOPBYTE(1)
#define F12LOADOP1HALF()  F12LOADOPHALF(1)
#define F12LOADOP1WORD()  F12LOADOPWORD(1)

#define F12LOADOP2BYTE()  F12LOADOPBYTE(2)
#define F12LOADOP2HALF()  F12LOADOPHALF(2)
#define F12LOADOP2WORD()  F12LOADOPWORD(2)

#define F12STOREOP1BYTE()  F12STOREOPBYTE(1)
#define F12STOREOP1HALF()  F12STOREOPHALF(1)
#define F12STOREOP1WORD()  F12STOREOPWORD(1)

#define F12STOREOP2BYTE()  F12STOREOPBYTE(2)
#define F12STOREOP2HALF()  F12STOREOPHALF(2)
#define F12STOREOP2WORD()  F12STOREOPWORD(2)



#define F12END()									\
	return amLength1 + amLength2 + 2;

uint8_t if12;

// Decode the first operand of the instruction and prepare
// writing to the second operand.
void F12DecodeFirstOperand(uint32_t (*DecodeOp1)(void), uint8_t dim1)
{
	if12 = OpRead8(PC + 1);

	// Check if F1 or F2
	if (if12 & 0x80)
	{
		modDim = dim1;
		modM = if12 & 0x40;
		modAdd = PC + 2;
		amLength1 = DecodeOp1();
		f12Op1 = amOut;
		f12Flag1 = amFlag;
	}
	else
	{
		// Check D flag
		if (if12 & 0x20)
		{
			modDim = dim1;
			modM = if12 & 0x40;
			modAdd = PC + 2;
			amLength1 = DecodeOp1();
			f12Op1 = amOut;
			f12Flag1 = amFlag;
		}
		else
		{
			if (DecodeOp1==ReadAM)
			{
				switch (dim1)
				{
				case 0:
					f12Op1 = (uint8_t)v60.reg[if12 & 0x1F];
					break;
				case 1:
					f12Op1 = (uint16_t)v60.reg[if12 & 0x1F];
					break;
				case 2:
					f12Op1 = v60.reg[if12 & 0x1F];
					break;
				}

				f12Flag1 = 0;
			}
			else
			{
				f12Flag1 = 1;
				f12Op1 = if12 & 0x1F;
			}

			amLength1 = 0;
		}
	}
}

void F12WriteSecondOperand(uint8_t dim2)
{
	modDim = dim2;

	// Check if F1 or F2
	if (if12 & 0x80)
	{
		modM = if12 & 0x20;
		modAdd = PC + 2 + amLength1;
		modDim = dim2;
		amLength2 = WriteAM();
	}
	else
	{
		// Check D flag
		if (if12 & 0x20)
		{
			switch (dim2)
			{
			case 0:
				SETREG8(v60.reg[if12 & 0x1F], modWriteValB);
				break;
			case 1:
				SETREG16(v60.reg[if12 & 0x1F], modWriteValH);
				break;
			case 2:
				v60.reg[if12 & 0x1F] = modWriteValW;
				break;
			}

			amLength2 = 0;
		}
		else
		{
			modM = if12 & 0x40;
			modAdd = PC + 2;
			modDim = dim2;
			amLength2 = WriteAM();
		}
	}
}



// Decode both format 1/2 operands
void F12DecodeOperands(uint32_t (*DecodeOp1)(void), uint8_t dim1, uint32_t (*DecodeOp2)(void), uint8_t dim2)
{
	uint8_t _if12 = OpRead8(PC + 1);

	// Check if F1 or F2
	if (_if12 & 0x80)
	{
		modDim = dim1;
		modM = _if12 & 0x40;
		modAdd = PC + 2;
		amLength1 = DecodeOp1();
		f12Op1 = amOut;
		f12Flag1 = amFlag;

		modDim = dim2;
		modM = _if12 & 0x20;
		modAdd = PC + 2 + amLength1;
		amLength2 = DecodeOp2();
		f12Op2 = amOut;
		f12Flag2 = amFlag;
	}
	else
	{
		// Check D flag
		if (_if12 & 0x20)
		{
			if (DecodeOp2==ReadAMAddress)
			{
				f12Op2 = _if12 & 0x1F;
				f12Flag2 = 1;
			}
			else
			{
				switch (dim2)
				{
				case 0:
					f12Op2 = (uint8_t)v60.reg[_if12 & 0x1F];
					break;
				case 1:
					f12Op2 = (uint16_t)v60.reg[_if12 & 0x1F];
					break;
				case 2:
					f12Op2 = v60.reg[_if12 & 0x1F];
					break;
				}
			}

			amLength2 = 0;

			modDim = dim1;
			modM = _if12 & 0x40;
			modAdd = PC + 2;
			amLength1 = DecodeOp1();
			f12Op1 = amOut;
			f12Flag1 = amFlag;
		}
		else
		{
			if (DecodeOp1==ReadAMAddress)
			{
				f12Op1 = _if12 & 0x1F;
				f12Flag1 = 1;
			}
			else
			{
				switch (dim1)
				{
				case 0:
					f12Op1 = (uint8_t)v60.reg[_if12 & 0x1F];
					break;
				case 1:
					f12Op1 = (uint16_t)v60.reg[_if12 & 0x1F];
					break;
				case 2:
					f12Op1 = v60.reg[_if12 & 0x1F];
					break;
				}
			}
			amLength1 = 0;

			modDim = dim2;
			modM = _if12 & 0x40;
			modAdd = PC + 2 + amLength1;
			amLength2 = DecodeOp2();
			f12Op2 = amOut;
			f12Flag2 = amFlag;
		}
	}
}

uint32_t opADDB(void) /* TRUSTED (C too!)*/
{
	uint8_t appb;
	F12DecodeOperands(ReadAM,0,ReadAMAddress,0);

	F12LOADOP2BYTE();

	ADDB(appb, (uint8_t)f12Op1);

	F12STOREOP2BYTE();
	F12END();
}

uint32_t opADDH(void) /* TRUSTED (C too!)*/
{
	uint16_t apph;
	F12DecodeOperands(ReadAM,1,ReadAMAddress,1);

	F12LOADOP2HALF();

	ADDW(apph, (uint16_t)f12Op1);

	F12STOREOP2HALF();
	F12END();
}

uint32_t opADDW(void) /* TRUSTED (C too!) */
{
	uint32_t appw;
	F12DecodeOperands(ReadAM,2,ReadAMAddress,2);

	F12LOADOP2WORD();

	ADDL(appw, (uint32_t)f12Op1);

	F12STOREOP2WORD();
	F12END();
}

uint32_t opADDCB(void)
{
	uint8_t appb, temp;

	F12DecodeOperands(ReadAM,0,ReadAMAddress,0);

	F12LOADOP2BYTE();

	temp = ((uint8_t)f12Op1 + (_CY?1:0));
	ADDB(appb, temp);

	F12STOREOP2BYTE();
	F12END();
}

uint32_t opADDCH(void)
{
	uint16_t apph, temp;

	F12DecodeOperands(ReadAM,1,ReadAMAddress,1);

	F12LOADOP2HALF();

	temp = ((uint16_t)f12Op1 + (_CY?1:0));
	ADDW(apph, temp);

	F12STOREOP2HALF();
	F12END();
}

uint32_t opADDCW(void)
{
	uint32_t appw, temp;

	F12DecodeOperands(ReadAM,2,ReadAMAddress,2);

	F12LOADOP2WORD();

	temp = f12Op1 + (_CY?1:0);
	ADDL(appw, temp);

	F12STOREOP2WORD();
	F12END();
}

uint32_t opANDB(void) /* TRUSTED */
{
	uint8_t appb;
	F12DecodeOperands(ReadAM,0,ReadAMAddress,0);

	F12LOADOP2BYTE();

	appb &= f12Op1;
	_OV = 0;
	_S = ((appb&0x80)!=0);
	_Z = (appb==0);

	F12STOREOP2BYTE();
	F12END();
}

uint32_t opANDH(void) /* TRUSTED */
{
	uint16_t apph;
	F12DecodeOperands(ReadAM,1,ReadAMAddress,1);

	F12LOADOP2HALF();

	apph &= f12Op1;
	_OV = 0;
	_S = ((apph&0x8000)!=0);
	_Z = (apph==0);

	F12STOREOP2HALF();
	F12END();
}

uint32_t opANDW(void) /* TRUSTED */
{
	uint32_t appw;
	F12DecodeOperands(ReadAM,2,ReadAMAddress,2);

	F12LOADOP2WORD();

	appw &= f12Op1;
	_OV = 0;
	_S = ((appw&0x80000000)!=0);
	_Z = (appw==0);

	F12STOREOP2WORD();
	F12END();
}

uint32_t opCALL(void) /* TRUSTED */
{
	F12DecodeOperands(ReadAMAddress,0,ReadAMAddress,2);

	SP -= 4;
	MemWrite32(SP, AP);
	AP = f12Op2;

	SP -= 4;
	MemWrite32(SP, PC + amLength1 + amLength2 + 2);
	PC = f12Op1;
	ChangePC(PC);

	return 0;
}

uint32_t opCHKAR(void)
{
	F12DecodeOperands(ReadAM,0,ReadAM,0);

	// No MMU and memory permissions yet @@@
	_Z = 1;
	_CY = 0;
	_S = 0;

	F12END();
}

uint32_t opCHKAW(void)
{
	F12DecodeOperands(ReadAM,0,ReadAM,0);

	// No MMU and memory permissions yet @@@
	_Z = 1;
	_CY = 0;
	_S = 0;

	F12END();
}

uint32_t opCHKAE(void)
{
	F12DecodeOperands(ReadAM,0,ReadAM,0);

	// No MMU and memory permissions yet @@@
	_Z = 1;
	_CY = 0;
	_S = 0;

	F12END();
}

uint32_t opCHLVL(void)
{
	F12DecodeOperands(ReadAM,0,ReadAM,0);

	if (f12Op1>3)
	{
		logerror("Illegal data field on opCHLVL, PC=%x\n", PC);
		abort();
	}

	UPDATEPSW();

	SP -= 4;
	MemWrite32(SP,f12Op2);

	SP -= 4;
	MemWrite32(SP,0x1800 + f12Op1*0x100);

	SP -= 4;
	MemWrite32(SP,PSW);

	SP -= 4;
	MemWrite32(SP,PC + amLength1 + amLength2 + 2);

	PC = GETINTVECT(24+f12Op1);
	ChangePC(PC);

	return 0;
}

uint32_t opCLR1(void) /* TRUSTED */
{
	uint32_t appw;
	F12DecodeOperands(ReadAM,2,ReadAMAddress,2);

	F12LOADOP2WORD();

	_CY = ((appw & (1<<f12Op1))!=0);
	_Z = !(_CY);

	appw &= ~(1<<f12Op1);

	F12STOREOP2WORD();
	F12END();
}

uint32_t opCMPB(void) /* TRUSTED (C too!) */
{
	uint8_t appb;
	F12DecodeOperands(ReadAM,0,ReadAM,0);

	appb = (uint8_t)f12Op2;
	SUBB(appb, (uint8_t)f12Op1);

	F12END();
}

uint32_t opCMPH(void) /* TRUSTED (C too!) */
{
	uint16_t apph;
	F12DecodeOperands(ReadAM,1,ReadAM,1);

	apph = (uint16_t)f12Op2;
	SUBW(apph, (uint16_t)f12Op1);

	F12END();
}


uint32_t opCMPW(void) /* TRUSTED (C too!)*/
{
	F12DecodeOperands(ReadAM,2,ReadAM,2);

	SUBL(f12Op2, (uint32_t)f12Op1);

	F12END();
}

uint32_t opDIVB(void) /* TRUSTED */
{
	uint8_t appb;
	F12DecodeOperands(ReadAM,0,ReadAMAddress,0);

	F12LOADOP2BYTE();

	_OV = ((appb == 0x80) && (f12Op1==0xFF));
	if (f12Op1 && !_OV)
		appb= (int8_t)appb / (int8_t)f12Op1;
	_Z = (appb == 0);
	_S = ((appb & 0x80)!=0);

	F12STOREOP2BYTE();
	F12END();
}

uint32_t opDIVH(void) /* TRUSTED */
{
	uint16_t apph;
	F12DecodeOperands(ReadAM,1,ReadAMAddress,1);

	F12LOADOP2HALF();

	_OV = ((apph == 0x8000) && (f12Op1==0xFFFF));
	if (f12Op1 && !_OV)
		apph = (int16_t)apph / (int16_t)f12Op1;
	_Z = (apph == 0);
	_S = ((apph & 0x8000)!=0);

	F12STOREOP2HALF();
	F12END();
}

uint32_t opDIVW(void) /* TRUSTED */
{
	uint32_t appw;
	F12DecodeOperands(ReadAM,2,ReadAMAddress,2);

	F12LOADOP2WORD();

	_OV = ((appw == 0x80000000) && (f12Op1==0xFFFFFFFF));
	if (f12Op1 && !_OV)
		appw = (int32_t)appw / (int32_t)f12Op1;
	_Z = (appw == 0);
	_S = ((appw & 0x80000000)!=0);

	F12STOREOP2WORD();
	F12END();
}

uint32_t opDIVX(void)
{
	uint32_t a,b;
	int64_t dv;

	F12DecodeOperands(ReadAM,2,ReadAMAddress,3);

	if (f12Flag2)
	{
		a=v60.reg[f12Op2&0x1F];
		b=v60.reg[(f12Op2&0x1F)+1];
	}
	else
	{
		a=MemRead32(f12Op2);
		b=MemRead32(f12Op2+4);
	}

	dv = ((uint64_t)b<<32) | ((uint64_t)a);

	a = dv / (int64_t)((int32_t)f12Op1);
	b = dv % (int64_t)((int32_t)f12Op1);

	_S = ((a & 0x80000000)!=0);
	_Z = (a == 0);

	if (f12Flag2)
	{
		v60.reg[f12Op2&0x1F]=a;
		v60.reg[(f12Op2&0x1F)+1]=b;
	}
	else
	{
		MemWrite32(f12Op2,a);
		MemWrite32(f12Op2+4,b);
	}

	F12END();
}

uint32_t opDIVUX(void)
{
	uint32_t a,b;
	uint64_t dv;

	F12DecodeOperands(ReadAM,2,ReadAMAddress,3);

	if (f12Flag2)
	{
		a=v60.reg[f12Op2&0x1F];
		b=v60.reg[(f12Op2&0x1F)+1];
	}
	else
	{
		a=MemRead32(f12Op2);
		b=MemRead32(f12Op2+4);
	}

	dv = (uint64_t)(((uint64_t)b<<32) | (uint64_t)a);
	a = (uint32_t)(dv / (uint64_t)f12Op1);
	b = (uint32_t)(dv % (uint64_t)f12Op1);

	_S = ((a & 0x80000000) != 0);
	_Z = (a == 0);

	if (f12Flag2)
	{
		v60.reg[f12Op2&0x1F]=a;
		v60.reg[(f12Op2&0x1F)+1]=b;
	}
	else
	{
		MemWrite32(f12Op2,a);
		MemWrite32(f12Op2+4,b);
	}

	F12END();
}


uint32_t opDIVUB(void) /* TRUSTED */
{
	uint8_t appb;
	F12DecodeOperands(ReadAM,0,ReadAMAddress,0);

	F12LOADOP2BYTE();

	_OV = 0;
	if (f12Op1)	appb /= (uint8_t)f12Op1;
	_Z = (appb == 0);
	_S = ((appb & 0x80)!=0);

	F12STOREOP2BYTE();
	F12END();
}

uint32_t opDIVUH(void) /* TRUSTED */
{
	uint16_t apph;
	F12DecodeOperands(ReadAM,1,ReadAMAddress,1);

	F12LOADOP2HALF();

	_OV = 0;
	if (f12Op1)	apph /= (uint16_t)f12Op1;
	_Z = (apph == 0);
	_S = ((apph & 0x8000)!=0);

	F12STOREOP2HALF();
	F12END();
}

uint32_t opDIVUW(void) /* TRUSTED */
{
	uint32_t appw;
	F12DecodeOperands(ReadAM,2,ReadAMAddress,2);

	F12LOADOP2WORD();

	_OV = 0;
	if (f12Op1)	appw /= f12Op1;
	_Z = (appw == 0);
	_S = ((appw & 0x80000000)!=0);

	F12STOREOP2WORD();
	F12END();
}

uint32_t opINB(void)
{
	F12DecodeFirstOperand(ReadAMAddress,0);
	modWriteValB=PortRead8(f12Op1);
	F12WriteSecondOperand(0);
	F12END();
}

uint32_t opINH(void)
{
	F12DecodeFirstOperand(ReadAMAddress,1);
	modWriteValH=PortRead16(f12Op1);
	F12WriteSecondOperand(1);
	F12END();
}

uint32_t opINW(void)
{
	F12DecodeFirstOperand(ReadAMAddress,2);
	modWriteValW=PortRead32(f12Op1);
	F12WriteSecondOperand(2);
	F12END();
}

uint32_t opLDPR(void)
{
	F12DecodeOperands(ReadAMAddress,2,ReadAM,2);
	if (f12Op2 >= 0 && f12Op2 <= 28)
	{
	  if (f12Flag1 &&(!(OpRead8(PC + 1)&0x80 && OpRead8(PC + 2)==0xf4 ) ))
			v60.reg[f12Op2 + 36] = v60.reg[f12Op1];
		else
			v60.reg[f12Op2 + 36] = f12Op1;
	}
	else
	{
		logerror("Invalid operand on LDPR PC=%x\n", PC);
		abort();
	}
	F12END();
}

uint32_t opLDTASK(void)
{
	int i;
	F12DecodeOperands(ReadAMAddress,2,ReadAM,2);

	TCB = f12Op2;

	UPDATEPSW();
	v60WritePSW(PSW & 0xefffffff);

	TKCW = MemRead32(f12Op2);
	f12Op2 += 4;
	if(SYCW & 0x100) {
		L0SP = MemRead32(f12Op2);
		f12Op2 += 4;
	}
	if(SYCW & 0x200) {
		L1SP = MemRead32(f12Op2);
		f12Op2 += 4;
	}
	if(SYCW & 0x400) {
		L2SP = MemRead32(f12Op2);
		f12Op2 += 4;
	}
	if(SYCW & 0x800) {
		L3SP = MemRead32(f12Op2);
		f12Op2 += 4;
	}

	v60ReloadStack();

	// 31 registers supported, _not_ 32
	for(i=0; i<31; i++)
		if(f12Op1 & (1<<i)) {
			v60.reg[i] = MemRead32(f12Op2);
			f12Op2 += 4;
		}

	// #### Ignore the virtual addressing crap.

	F12END();
}

uint32_t opMOVD(void) /* TRUSTED */
{
	uint32_t a,b;

	F12DecodeOperands(ReadAMAddress,3,ReadAMAddress,3);

	if (f12Flag1)
	{
		a=v60.reg[f12Op1&0x1F];
		b=v60.reg[(f12Op1&0x1F)+1];
	}
	else
	{
		a=MemRead32(f12Op1);
		b=MemRead32(f12Op1+4);
	}

	if (f12Flag2)
	{
		v60.reg[f12Op2&0x1F]=a;
		v60.reg[(f12Op2&0x1F)+1]=b;
	}
	else
	{
		MemWrite32(f12Op2,a);
		MemWrite32(f12Op2+4,b);
	}

	F12END();
}

uint32_t opMOVB(void) /* TRUSTED */
{
	F12DecodeFirstOperand(ReadAM,0);
	modWriteValB = (uint8_t)f12Op1;
	F12WriteSecondOperand(0);
	F12END();
}

uint32_t opMOVH(void) /* TRUSTED */
{
	F12DecodeFirstOperand(ReadAM,1);
	modWriteValH = (uint16_t)f12Op1;
	F12WriteSecondOperand(1);
	F12END();
}

uint32_t opMOVW(void) /* TRUSTED */
{
	F12DecodeFirstOperand(ReadAM,2);
	modWriteValW = f12Op1;
	F12WriteSecondOperand(2);
	F12END();
}

uint32_t opMOVEAB(void) /* TRUSTED */
{
	F12DecodeFirstOperand(ReadAMAddress,0);
	modWriteValW = f12Op1;
	F12WriteSecondOperand(2);
	F12END();
}

uint32_t opMOVEAH(void) /* TRUSTED */
{
	F12DecodeFirstOperand(ReadAMAddress,1);
	modWriteValW = f12Op1;
	F12WriteSecondOperand(2);
	F12END();
}

uint32_t opMOVEAW(void) /* TRUSTED */
{
	F12DecodeFirstOperand(ReadAMAddress,2);
	modWriteValW = f12Op1;
	F12WriteSecondOperand(2);
	F12END();
}

uint32_t opMOVSBH(void) /* TRUSTED */
{
	F12DecodeFirstOperand(ReadAM,0);
	modWriteValH = (int8_t)(f12Op1&0xFF);
	F12WriteSecondOperand(1);
	F12END();
}

uint32_t opMOVSBW(void) /* TRUSTED */
{
	F12DecodeFirstOperand(ReadAM,0);
	modWriteValW = (int8_t)(f12Op1&0xFF);
	F12WriteSecondOperand(2);
	F12END();
}

uint32_t opMOVSHW(void) /* TRUSTED */
{
	F12DecodeFirstOperand(ReadAM,1);
	modWriteValW = (int16_t)(f12Op1&0xFFFF);
	F12WriteSecondOperand(2);
	F12END();
}

uint32_t opMOVTHB(void)
{
	F12DecodeFirstOperand(ReadAM,1);
	modWriteValB = (uint8_t)(f12Op1&0xFF);

	// Check for overflow: the truncated bits must match the sign
	//  of the result, otherwise overflow
	if (((modWriteValB&0x80)==0x80 && ((f12Op1&0xFF00)==0xFF00)) ||
		  ((modWriteValB&0x80)==0 && ((f12Op1&0xFF00)==0x0000)))
		_OV = 0;
	else
		_OV = 1;

	F12WriteSecondOperand(0);
	F12END();
}

uint32_t opMOVTWB(void)
{
	F12DecodeFirstOperand(ReadAM,2);
	modWriteValB = (uint8_t)(f12Op1&0xFF);

	// Check for overflow: the truncated bits must match the sign
	//  of the result, otherwise overflow
	if (((modWriteValB&0x80)==0x80 && ((f12Op1&0xFFFFFF00)==0xFFFFFF00)) ||
		  ((modWriteValB&0x80)==0 && ((f12Op1&0xFFFFFF00)==0x00000000)))
		_OV = 0;
	else
		_OV = 1;

	F12WriteSecondOperand(0);
	F12END();
}

uint32_t opMOVTWH(void)
{
	F12DecodeFirstOperand(ReadAM,2);
	modWriteValH = (uint16_t)(f12Op1&0xFFFF);

	// Check for overflow: the truncated bits must match the sign
	//  of the result, otherwise overflow
	if (((modWriteValH&0x8000)==0x8000 && ((f12Op1&0xFFFF0000)==0xFFFF0000)) ||
		  ((modWriteValH&0x8000)==0 && ((f12Op1&0xFFFF0000)==0x00000000)))
		_OV = 0;
	else
		_OV = 1;

	F12WriteSecondOperand(1);
	F12END();
}


uint32_t opMOVZBH(void) /* TRUSTED */
{
	F12DecodeFirstOperand(ReadAM,0);
	modWriteValH = (uint16_t)f12Op1;
	F12WriteSecondOperand(1);
	F12END();
}

uint32_t opMOVZBW(void) /* TRUSTED */
{
	F12DecodeFirstOperand(ReadAM,0);
	modWriteValW = f12Op1;
	F12WriteSecondOperand(2);
	F12END();
}

uint32_t opMOVZHW(void) /* TRUSTED */
{
	F12DecodeFirstOperand(ReadAM,1);
	modWriteValW = f12Op1;
	F12WriteSecondOperand(2);
	F12END();
}

uint32_t opMULB(void)
{
	uint8_t appb;
	uint32_t tmp;
	F12DecodeOperands(ReadAM,0,ReadAMAddress,0);

	F12LOADOP2BYTE();

	// @@@ OV not set!!
	tmp=(int8_t)appb * (int32_t)(int8_t)f12Op1;
	appb = tmp;
	_Z = (appb == 0);
	_S = ((appb & 0x80)!=0);
	_OV = ((tmp >> 8)!=0);

	F12STOREOP2BYTE();
	F12END();
}

uint32_t opMULH(void)
{
	uint16_t apph;
	uint32_t tmp;
	F12DecodeOperands(ReadAM,1,ReadAMAddress,1);

	F12LOADOP2HALF();

	// @@@ OV not set!!
	tmp=(int16_t)apph * (int32_t)(int16_t)f12Op1;
	apph = tmp;
	_Z = (apph == 0);
	_S = ((apph & 0x8000)!=0);
	_OV = ((tmp >> 16)!=0);

	F12STOREOP2HALF();
	F12END();
}

uint32_t opMULW(void)
{
	uint32_t appw;
	uint64_t tmp;
	F12DecodeOperands(ReadAM,2,ReadAMAddress,2);

	F12LOADOP2WORD();

	// @@@ OV not set!!
	tmp=(int32_t)appw * (int64_t)(int32_t)f12Op1;
	appw = tmp;
	_Z = (appw == 0);
	_S = ((appw & 0x80000000)!=0);
	_OV = ((tmp >> 32) != 0);

	F12STOREOP2WORD();
	F12END();
}

uint32_t opMULUB(void)
{
	uint8_t appb;
	uint32_t tmp;
	F12DecodeOperands(ReadAM,0,ReadAMAddress,0);

	F12LOADOP2BYTE();

	// @@@ OV not set!!
	tmp = appb * (uint8_t)f12Op1;
	appb = tmp;
	_Z = (appb == 0);
	_S = ((appb & 0x80)!=0);
	_OV = ((tmp >> 8)!=0);

	F12STOREOP2BYTE();
	F12END();
}

uint32_t opMULUH(void)
{
	uint16_t apph;
	uint32_t tmp;
	F12DecodeOperands(ReadAM,1,ReadAMAddress,1);

	F12LOADOP2HALF();

	// @@@ OV not set!!
	tmp=apph * (uint16_t)f12Op1;
	apph = tmp;
	_Z = (apph == 0);
	_S = ((apph & 0x8000)!=0);
	_OV = ((tmp >> 16)!=0);

	F12STOREOP2HALF();
	F12END();
}

uint32_t opMULUW(void)
{
	uint32_t appw;
	uint64_t tmp;
	F12DecodeOperands(ReadAM,2,ReadAMAddress,2);

	F12LOADOP2WORD();

	// @@@ OV not set!!
	tmp=(uint64_t)appw * (uint64_t)f12Op1;
	appw = tmp;
	_Z = (appw == 0);
	_S = ((appw & 0x80000000)!=0);
	_OV = ((tmp >> 32)!=0);

	F12STOREOP2WORD();
	F12END();
}

uint32_t opNEGB(void) /* TRUSTED  (C too!)*/
{
	F12DecodeFirstOperand(ReadAM,0);

	modWriteValB = 0;
	SUBB(modWriteValB, (int8_t)f12Op1);

	F12WriteSecondOperand(0);
	F12END();
}

uint32_t opNEGH(void) /* TRUSTED  (C too!)*/
{
	F12DecodeFirstOperand(ReadAM,1);

	modWriteValH = 0;
	SUBW(modWriteValH, (int16_t)f12Op1);

	F12WriteSecondOperand(1);
	F12END();
}

uint32_t opNEGW(void) /* TRUSTED  (C too!)*/
{
	F12DecodeFirstOperand(ReadAM,2);

	modWriteValW = 0;
	SUBL(modWriteValW, (int32_t)f12Op1);

	F12WriteSecondOperand(2);
	F12END();
}

uint32_t opNOTB(void) /* TRUSTED */
{
	F12DecodeFirstOperand(ReadAM,0);
	modWriteValB=~f12Op1;

	_OV=0;
	_S=((modWriteValB&0x80)!=0);
	_Z=(modWriteValB==0);

	F12WriteSecondOperand(0);
	F12END();
}

uint32_t opNOTH(void) /* TRUSTED */
{
	F12DecodeFirstOperand(ReadAM,1);
	modWriteValH=~f12Op1;

	_OV=0;
	_S=((modWriteValH&0x8000)!=0);
	_Z=(modWriteValH==0);

	F12WriteSecondOperand(1);
	F12END();
}

uint32_t opNOTW(void) /* TRUSTED */
{
	F12DecodeFirstOperand(ReadAM,2);
	modWriteValW=~f12Op1;

	_OV=0;
	_S=((modWriteValW&0x80000000)!=0);
	_Z=(modWriteValW==0);

	F12WriteSecondOperand(2);
	F12END();
}

uint32_t opNOT1(void) /* TRUSTED */
{
	uint32_t appw;
	F12DecodeOperands(ReadAM,2,ReadAMAddress,2);

	F12LOADOP2WORD();

	_CY = ((appw & (1<<f12Op1))!=0);
	_Z = !(_CY);

	if (_CY)
		appw &= ~(1<<f12Op1);
	else
		appw |= (1<<f12Op1);

	F12STOREOP2WORD();
	F12END();
}

uint32_t opORB(void) /* TRUSTED  (C too!)*/
{
	uint8_t appb;
	F12DecodeOperands(ReadAM,0,ReadAMAddress,0);

	F12LOADOP2BYTE();

	ORB(appb, (uint8_t)f12Op1);

	F12STOREOP2BYTE();
	F12END();
}

uint32_t opORH(void) /* TRUSTED (C too!)*/
{
	uint16_t apph;
	F12DecodeOperands(ReadAM,1,ReadAMAddress,1);

	F12LOADOP2HALF();

	ORW(apph, (uint16_t)f12Op1);

	F12STOREOP2HALF();
	F12END();
}

uint32_t opORW(void) /* TRUSTED (C too!) */
{
	uint32_t appw;
	F12DecodeOperands(ReadAM,2,ReadAMAddress,2);

	F12LOADOP2WORD();

	ORL(appw, (uint32_t)f12Op1);

	F12STOREOP2WORD();
	F12END();
}

uint32_t opOUTB(void)
{
	F12DecodeOperands(ReadAM,0,ReadAMAddress,2);
	PortWrite8(f12Op2,(uint8_t)f12Op1);
	F12END();
}

uint32_t opOUTH(void)
{
	F12DecodeOperands(ReadAM,1,ReadAMAddress,2);
	PortWrite16(f12Op2,(uint16_t)f12Op1);
	F12END();
}

uint32_t opOUTW(void)
{
	F12DecodeOperands(ReadAM,2,ReadAMAddress,2);
	PortWrite32(f12Op2,f12Op1);
	F12END();
}

uint32_t opREMB(void)
{
	uint8_t appb;
	F12DecodeOperands(ReadAM,0,ReadAMAddress,0);

	F12LOADOP2BYTE();

	_OV = 0;
	if (f12Op1)
		appb= (int8_t)appb % (int8_t)f12Op1;
	_Z = (appb == 0);
	_S = ((appb & 0x80)!=0);

	F12STOREOP2BYTE();
	F12END();
}

uint32_t opREMH(void)
{
	uint16_t apph;
	F12DecodeOperands(ReadAM,1,ReadAMAddress,1);

	F12LOADOP2HALF();

	_OV = 0;
	if (f12Op1)
		apph=(int16_t)apph % (int16_t)f12Op1;
	_Z = (apph == 0);
	_S = ((apph & 0x8000)!=0);

	F12STOREOP2HALF();
	F12END();
}

uint32_t opREMW(void)
{
	uint32_t appw;
	F12DecodeOperands(ReadAM,2,ReadAMAddress,2);

	F12LOADOP2WORD();

	_OV = 0;
	if (f12Op1)
		appw=(int32_t)appw % (int32_t)f12Op1;
	_Z = (appw == 0);
	_S = ((appw & 0x80000000)!=0);

	F12STOREOP2WORD();
	F12END();
}

uint32_t opREMUB(void)
{
	uint8_t appb;
	F12DecodeOperands(ReadAM,0,ReadAMAddress,0);

	F12LOADOP2BYTE();

	_OV = 0;
	if (f12Op1)
		appb %= (uint8_t)f12Op1;
	_Z = (appb == 0);
	_S = ((appb & 0x80)!=0);

	F12STOREOP2BYTE();
	F12END();
}

uint32_t opREMUH(void)
{
	uint16_t apph;
	F12DecodeOperands(ReadAM,1,ReadAMAddress,1);

	F12LOADOP2HALF();

	_OV = 0;
	if (f12Op1)
		apph %= (uint16_t)f12Op1;
	_Z = (apph == 0);
	_S = ((apph & 0x8000)!=0);

	F12STOREOP2HALF();
	F12END();
}

uint32_t opREMUW(void)
{
	uint32_t appw;
	F12DecodeOperands(ReadAM,2,ReadAMAddress,2);

	F12LOADOP2WORD();

	_OV = 0;
	if (f12Op1)
		appw %= f12Op1;
	_Z = (appw == 0);
	_S = ((appw & 0x80000000)!=0);

	F12STOREOP2WORD();
	F12END();
}

uint32_t opROTB(void) /* TRUSTED */
{
	uint8_t appb;
	int8_t i,count;

	F12DecodeOperands(ReadAM,0,ReadAMAddress,0);

	F12LOADOP2BYTE();

	count=(int8_t)(f12Op1&0xFF);
	if (count>0)
	{
		for (i=0;i<count;i++)
			appb = (appb<<1) | ((appb&0x80) >> 7);

		_CY=(appb&0x1)!=0;
	}
	else if (count<0)
	{
		count=-count;
		for (i=0;i<count;i++)
			appb = (appb>>1) | ((appb&0x1) << 7);

		_CY=(appb&0x80)!=0;
	}
	else
		_CY=0;

	_OV=0;
	_S=(appb&0x80)!=0;
	_Z=(appb==0);

	F12STOREOP2BYTE();
	F12END();
}

uint32_t opROTH(void) /* TRUSTED */
{
	uint16_t apph;
	int8_t i,count;

	F12DecodeOperands(ReadAM,0,ReadAMAddress,1);

	F12LOADOP2HALF();

	count=(int8_t)(f12Op1&0xFF);
	if (count>0)
	{
		for (i=0;i<count;i++)
			apph = (apph<<1) | ((apph&0x8000) >> 15);

		_CY=(apph&0x1)!=0;
	}
	else if (count<0)
	{
		count=-count;
		for (i=0;i<count;i++)
			apph = (apph>>1) | ((apph&0x1) << 15);

		_CY=(apph&0x8000)!=0;
	}
	else
		_CY=0;

	_OV=0;
	_S=(apph&0x8000)!=0;
	_Z=(apph==0);

	F12STOREOP2HALF();
	F12END();
}

uint32_t opROTW(void) /* TRUSTED */
{
	uint32_t appw;
	int8_t i,count;

	F12DecodeOperands(ReadAM,0,ReadAMAddress,2);

	F12LOADOP2WORD();

	count=(int8_t)(f12Op1&0xFF);
	if (count>0)
	{
		for (i=0;i<count;i++)
			appw = (appw<<1) | ((appw&0x80000000) >> 31);

		_CY=(appw&0x1)!=0;
	}
	else if (count<0)
	{
		count=-count;
		for (i=0;i<count;i++)
			appw = (appw>>1) | ((appw&0x1) << 31);

		_CY=(appw&0x80000000)!=0;
	}
	else
		_CY=0;

	_OV=0;
	_S=(appw&0x80000000)!=0;
	_Z=(appw==0);

	F12STOREOP2WORD();
	F12END();
}

uint32_t opROTCB(void) /* TRUSTED */
{
	uint8_t appb;
	int8_t i,cy,count;

	F12DecodeOperands(ReadAM,0,ReadAMAddress,0);

	F12LOADOP2BYTE();
	NORMALIZEFLAGS();

	count=(int8_t)(f12Op1&0xFF);
	if (count>0)
	{
		for (i=0;i<count;i++)
		{
			cy = _CY;
			_CY = (uint8_t)((appb & 0x80) >> 7);
			appb = (appb<<1) | cy;
		}
	}
	else if (count<0)
	{
		count=-count;
		for (i=0;i<count;i++)
		{
			cy = _CY;
			_CY = (appb & 1);
			appb = (appb>>1) | (cy << 7);
		}
	}
	else
		_CY = 0;

	_OV=0;
	_S=(appb&0x80)!=0;
	_Z=(appb==0);

	F12STOREOP2BYTE();
	F12END();
}

uint32_t opROTCH(void) /* TRUSTED */
{
	uint16_t apph;
	int8_t i,cy,count;

	F12DecodeOperands(ReadAM,0,ReadAMAddress,1);

	F12LOADOP2HALF();
	NORMALIZEFLAGS();

	count=(int8_t)(f12Op1&0xFF);
	if (count>0)
	{
		for (i=0;i<count;i++)
		{
			cy = _CY;
			_CY = (uint8_t)((apph & 0x8000) >> 15);
			apph = (apph<<1) | cy;
		}
	}
	else if (count<0)
	{
		count=-count;
		for (i=0;i<count;i++)
		{
			cy = _CY;
			_CY = (uint8_t)(apph & 1);
			apph = (apph>>1) | ((uint16_t)cy << 15);
		}
	}
	else
		_CY = 0;

	_OV=0;
	_S=(apph&0x8000)!=0;
	_Z=(apph==0);

	F12STOREOP2HALF();
	F12END();
}

uint32_t opROTCW(void) /* TRUSTED */
{
	uint32_t appw;
	int8_t i,cy,count;

	F12DecodeOperands(ReadAM,0,ReadAMAddress,2);

	F12LOADOP2WORD();
	NORMALIZEFLAGS();

	count=(int8_t)(f12Op1&0xFF);
	if (count>0)
	{
		for (i=0;i<count;i++)
		{
			cy = _CY;
			_CY = (uint8_t)((appw & 0x80000000) >> 31);
			appw = (appw<<1) | cy;
		}
	}
	else if (count<0)
	{
		count=-count;
		for (i=0;i<count;i++)
		{
			cy = _CY;
			_CY = (uint8_t)(appw & 1);
			appw = (appw>>1) | ((uint32_t)cy << 31);
		}
	}
	else
		_CY=0;

	_OV=0;
	_S=(appw&0x80000000)!=0;
	_Z=(appw==0);

	F12STOREOP2WORD();
	F12END();
}

uint32_t opRVBIT(void)
{
	F12DecodeFirstOperand(ReadAM,0);

	modWriteValB =(uint8_t)
								(((f12Op1 & (1<<0)) << 7) |
								 ((f12Op1 & (1<<1)) << 5) |
								 ((f12Op1 & (1<<2)) << 3) |
								 ((f12Op1 & (1<<3)) << 1) |
								 ((f12Op1 & (1<<4)) >> 1) |
								 ((f12Op1 & (1<<5)) >> 3) |
								 ((f12Op1 & (1<<6)) >> 5) |
								 ((f12Op1 & (1<<7)) >> 7));

	F12WriteSecondOperand(0);
	F12END();
}

uint32_t opRVBYT(void) /* TRUSTED */
{
	F12DecodeFirstOperand(ReadAM,2);

	modWriteValW = ((f12Op1 & 0x000000FF) << 24) |
								 ((f12Op1 & 0x0000FF00) << 8)  |
								 ((f12Op1 & 0x00FF0000) >> 8)  |
								 ((f12Op1 & 0xFF000000) >> 24);

	F12WriteSecondOperand(2);
	F12END();
}

uint32_t opSET1(void) /* TRUSTED */
{
	uint32_t appw;
	F12DecodeOperands(ReadAM,2,ReadAMAddress,2);

	F12LOADOP2WORD();

	_CY = ((appw & (1<<f12Op1))!=0);
	_Z = !(_CY);

	appw |= (1<<f12Op1);

	F12STOREOP2WORD();
	F12END();
}


uint32_t opSETF(void)
{
	F12DecodeFirstOperand(ReadAM,0);

	// Normalize the flags
	NORMALIZEFLAGS();

	switch (f12Op1 & 0xF)
	{
	case 0:
		if (!_OV) modWriteValB=0;
		else modWriteValB=1;
		break;
	case 1:
		if (_OV) modWriteValB=0;
		else modWriteValB=1;
		break;
	case 2:
		if (!_CY) modWriteValB=0;
		else modWriteValB=1;
		break;
	case 3:
		if (_CY) modWriteValB=0;
		else modWriteValB=1;
		break;
	case 4:
		if (!_Z) modWriteValB=0;
		else modWriteValB=1;
		break;
	case 5:
		if (_Z) modWriteValB=0;
		else modWriteValB=1;
		break;
	case 6:
		if (!(_CY | _Z)) modWriteValB=0;
		else modWriteValB=1;
		break;
	case 7:
		if ((_CY | _Z)) modWriteValB=0;
		else modWriteValB=1;
		break;
	case 8:
		if (!_S) modWriteValB=0;
		else modWriteValB=1;
		break;
	case 9:
		if (_S) modWriteValB=0;
		else modWriteValB=1;
		break;
	case 10:
		modWriteValB=1;
		break;
	case 11:
		modWriteValB=0;
		break;
	case 12:
		if (!(_S^_OV)) modWriteValB=0;
		else modWriteValB=1;
		break;
	case 13:
		if ((_S^_OV)) modWriteValB=0;
		else modWriteValB=1;
		break;
	case 14:
		if (!((_S^_OV)|_Z)) modWriteValB=0;
		else modWriteValB=1;
		break;
	case 15:
		if (((_S^_OV)|_Z)) modWriteValB=0;
		else modWriteValB=1;
		break;
	}

	F12WriteSecondOperand(0);

	F12END();
}

/*
#define SHIFTLEFT_OY(val, count, bitsize) \
{\
	uint32_t tmp = ((val) >> (bitsize-1)) & 1; \
	tmp <<= count; \
	tmp -= 1; \
	tmp <<= (bitsize - (count)); \
	_OV = (((val) & tmp) != tmp); \
	_CY = (((val) & (1 << (count-1))) != 0); \
}
*/

// During the shift, the overflow is set if the sign bit changes at any point during the shift
#define SHIFTLEFT_OV(val, count, bitsize) \
{\
	uint32_t tmp; \
	if (count == 32) \
		tmp = 0xFFFFFFFF; \
	else \
		tmp = ((1 << (count)) - 1); \
	tmp <<= (bitsize - (count)); \
	if (((val) >> (bitsize-1)) & 1) \
		_OV = (((val) & tmp) != tmp); \
	else \
		_OV = (((val) & tmp) != 0); \
}

#define SHIFTLEFT_CY(val, count, bitsize) \
	_CY = (uint8_t)(((val) >> (bitsize - count)) & 1);



#define SHIFTARITHMETICRIGHT_OV(val, count, bitsize) \
	_OV = 0;

#define SHIFTARITHMETICRIGHT_CY(val, count, bitsize) \
	_CY = (uint8_t)(((val) >> (count-1)) & 1);



uint32_t opSHAB(void)
{
	uint8_t appb;
	int8_t count;

	F12DecodeOperands(ReadAM,0,ReadAMAddress,0);

	F12LOADOP2BYTE();

	count=(int8_t)(f12Op1&0xFF);

	// Special case: destination unchanged, flags set
	if (count == 0)
	{
		_CY = _OV = 0;
		SetSZPF_Byte(appb);
	}
	else if (count>0)
	{
		SHIFTLEFT_OV(appb, count, 8);

		// @@@ Undefined what happens to CY when count >= bitsize
		SHIFTLEFT_CY(appb, count, 8);

		// do the actual shift...
		if (count >= 8)
			appb = 0;
		else
			appb <<= count;

		// and set zero and sign
		SetSZPF_Byte(appb);
	}
	else
	{
		count = -count;

		SHIFTARITHMETICRIGHT_OV(appb, count, 8);
		SHIFTARITHMETICRIGHT_CY(appb, count, 8);

		if (count >= 8)
			appb = (appb & 0x80) ? 0xFF : 0;
		else
			appb = ((int8_t)appb) >> count;

		SetSZPF_Byte(appb);
	}

//	printf("SHAB: %x _CY: %d _Z: %d _OV: %d _S: %d\n", appb, _CY, _Z, _OV, _S);

	F12STOREOP2BYTE();
	F12END();
}

uint32_t opSHAH(void)
{
	uint16_t apph;
	int8_t count;

	F12DecodeOperands(ReadAM,0,ReadAMAddress,1);

	F12LOADOP2HALF();

	count=(int8_t)(f12Op1&0xFF);

	// Special case: destination unchanged, flags set
	if (count == 0)
	{
		_CY = _OV = 0;
		SetSZPF_Word(apph);
	}
	else if (count>0)
	{
		SHIFTLEFT_OV(apph, count, 16);

		// @@@ Undefined what happens to CY when count >= bitsize
		SHIFTLEFT_CY(apph, count, 16);

		// do the actual shift...
		if (count >= 16)
			apph = 0;
		else
			apph <<= count;

		// and set zero and sign
		SetSZPF_Word(apph);
	}
	else
	{
		count = -count;

		SHIFTARITHMETICRIGHT_OV(apph, count, 16);
		SHIFTARITHMETICRIGHT_CY(apph, count, 16);

		if (count >= 16)
			apph = (apph & 0x8000) ? 0xFFFF : 0;
		else
			apph = ((int16_t)apph) >> count;

		SetSZPF_Word(apph);
	}

//	printf("SHAH: %x >> %d = %x _CY: %d _Z: %d _OV: %d _S: %d\n", oldval, count, apph, _CY, _Z, _OV, _S);

	F12STOREOP2HALF();
	F12END();
}

uint32_t opSHAW(void)
{
	uint32_t appw;
	int8_t count;

	F12DecodeOperands(ReadAM,0,ReadAMAddress,2);

	F12LOADOP2WORD();

	count=(int8_t)(f12Op1&0xFF);

	// Special case: destination unchanged, flags set
	if (count == 0)
	{
		_CY = _OV = 0;
		SetSZPF_Long(appw);
	}
	else if (count>0)
	{
		SHIFTLEFT_OV(appw, count, 32);

		// @@@ Undefined what happens to CY when count >= bitsize
		SHIFTLEFT_CY(appw, count, 32);

		// do the actual shift...
		if (count >= 32)
			appw = 0;
		else
			appw <<= count;

		// and set zero and sign
		SetSZPF_Long(appw);
	}
	else
	{
		count = -count;

		SHIFTARITHMETICRIGHT_OV(appw, count, 32);
		SHIFTARITHMETICRIGHT_CY(appw, count, 32);

		if (count >= 32)
			appw = (appw & 0x80000000) ? 0xFFFFFFFF : 0;
		else
			appw = ((int32_t)appw) >> count;

		SetSZPF_Long(appw);
	}

//	printf("SHAW: %x >> %d = %x _CY: %d _Z: %d _OV: %d _S: %d\n", oldval, count, appw, _CY, _Z, _OV, _S);

	F12STOREOP2WORD();
	F12END();
}


uint32_t opSHLB(void) /* TRUSTED */
{
	uint8_t appb;
	int8_t count;
	uint32_t tmp;

	F12DecodeOperands(ReadAM,0,ReadAMAddress,0);

	F12LOADOP2BYTE();

	count=(int8_t)(f12Op1&0xFF);
	if (count>0)
	{
		// left shift flags:
		// carry gets the last bit shifted out,
		// overflow is always CLEARed

		_OV = 0;	// default to no overflow

		// now handle carry
		tmp = appb & 0xff;
		tmp <<= count;
		SetCFB(tmp);	// set carry properly

		// do the actual shift...
		appb <<= count;

		// and set zero and sign
		SetSZPF_Byte(appb);
	}
	else
	{
		if (count == 0)
		{
			// special case: clear carry and overflow, do nothing else
			_CY = _OV = 0;
			SetSZPF_Byte(appb);	// doc. is unclear if this is true...
		}
		else
		{
			// right shift flags:
			// carry = last bit shifted out
			// overflow always cleared
			tmp = appb & 0xff;
			tmp >>= ((-count)-1);
			_CY = (uint8_t)(tmp & 0x1);
			_OV = 0;

			appb >>= -count;
			SetSZPF_Byte(appb);
		}
	}

//	printf("SHLB: %x _CY: %d _Z: %d _OV: %d _S: %d\n", appb, _CY, _Z, _OV, _S);

	F12STOREOP2BYTE();
	F12END();
}

uint32_t opSHLH(void) /* TRUSTED */
{
	uint16_t apph;
	int8_t count;
	uint32_t tmp;

	F12DecodeOperands(ReadAM,0,ReadAMAddress,1);

	F12LOADOP2HALF();

	count=(int8_t)(f12Op1&0xFF);
//	printf("apph: %x count: %d  ", apph, count);
	if (count>0)
	{
		// left shift flags:
		// carry gets the last bit shifted out,
		// overflow is always CLEARed

		_OV = 0;

		// now handle carry
		tmp = apph & 0xffff;
		tmp <<= count;
		SetCFW(tmp);	// set carry properly

		// do the actual shift...
		apph <<= count;

		// and set zero and sign
		SetSZPF_Word(apph);
	}
	else
	{
		if (count == 0)
		{
			// special case: clear carry and overflow, do nothing else
			_CY = _OV = 0;
			SetSZPF_Word(apph);	// doc. is unclear if this is true...
		}
		else
		{
			// right shift flags:
			// carry = last bit shifted out
			// overflow always cleared
			tmp = apph & 0xffff;
			tmp >>= ((-count)-1);
			_CY = (uint8_t)(tmp & 0x1);
			_OV = 0;

			apph >>= -count;
			SetSZPF_Word(apph);
		}
	}

//	printf("SHLH: %x _CY: %d _Z: %d _OV: %d _S: %d\n", apph, _CY, _Z, _OV, _S);

	F12STOREOP2HALF();
	F12END();
}

uint32_t opSHLW(void) /* TRUSTED */
{
	uint32_t appw;
	int8_t count;
	uint64_t tmp;

	F12DecodeOperands(ReadAM,0,ReadAMAddress,2);

	F12LOADOP2WORD();

	count=(int8_t)(f12Op1&0xFF);
	if (count>0)
	{
		// left shift flags:
		// carry gets the last bit shifted out,
		// overflow is always CLEARed

		_OV = 0;

		// now handle carry
		tmp = appw & 0xffffffff;
		tmp <<= count;
		SetCFL(tmp);	// set carry properly

		// do the actual shift...
		appw <<= count;

		// and set zero and sign
		SetSZPF_Long(appw);
	}
	else
	{
		if (count == 0)
		{
			// special case: clear carry and overflow, do nothing else
			_CY = _OV = 0;
			SetSZPF_Long(appw);	// doc. is unclear if this is true...
		}
		else
		{
			// right shift flags:
			// carry = last bit shifted out
			// overflow always cleared
			tmp = (uint64_t)(appw & 0xffffffff);
			tmp >>= ((-count)-1);
			_CY = (uint8_t)(tmp & 0x1);
			_OV = 0;

			appw >>= -count;
			SetSZPF_Long(appw);
		}
	}

//	printf("SHLW: %x _CY: %d _Z: %d _OV: %d _S: %d\n", appw, _CY, _Z, _OV, _S);

	F12STOREOP2WORD();
	F12END();
}

uint32_t opSTPR(void)
{
	F12DecodeFirstOperand(ReadAM,2);
	if (f12Op1 >= 0 && f12Op1 <= 28)
		modWriteValW = v60.reg[f12Op1 + 36];
	else
	{
		logerror("Invalid operand on STPR PC=%x\n", PC);
		abort();
	}
	F12WriteSecondOperand(2);
	F12END();
}


uint32_t opSUBB(void) /* TRUSTED (C too!) */
{
	uint8_t appb;
	F12DecodeOperands(ReadAM,0,ReadAMAddress,0);

	F12LOADOP2BYTE();

	SUBB(appb, (uint8_t)f12Op1);

	F12STOREOP2BYTE();
	F12END();
}

uint32_t opSUBH(void) /* TRUSTED (C too!) */
{
	uint16_t apph;
	F12DecodeOperands(ReadAM,1,ReadAMAddress,1);

	F12LOADOP2HALF();

	SUBW(apph, (uint16_t)f12Op1);

	F12STOREOP2HALF();
	F12END();
}

uint32_t opSUBW(void) /* TRUSTED (C too!) */
{
	uint32_t appw;
	F12DecodeOperands(ReadAM,2,ReadAMAddress,2);

	F12LOADOP2WORD();

	SUBL(appw, (uint32_t)f12Op1);

	F12STOREOP2WORD();
	F12END();
}


uint32_t opSUBCB(void)
{
	uint8_t appb;
	uint8_t src;
	F12DecodeOperands(ReadAM,0,ReadAMAddress,0);

	F12LOADOP2BYTE();

	src = (uint8_t)f12Op1 + (_CY?1:0);
	SUBB(appb, src);

	F12STOREOP2BYTE();
	F12END();
}

uint32_t opSUBCH(void)
{
	uint16_t apph;
	uint16_t src;

	F12DecodeOperands(ReadAM,1,ReadAMAddress,1);

	F12LOADOP2HALF();

	src = (uint16_t)f12Op1 + (_CY?1:0);
	SUBW(apph, src);

	F12STOREOP2HALF();
	F12END();
}

uint32_t opSUBCW(void)
{
	uint32_t appw;
	uint32_t src;

	F12DecodeOperands(ReadAM,2,ReadAMAddress,2);

	F12LOADOP2WORD();

	src = (uint32_t)f12Op1 + (_CY?1:0);
	SUBL(appw, src);

	F12STOREOP2WORD();
	F12END();
}

uint32_t opTEST1(void)
{
	F12DecodeOperands(ReadAM,2,ReadAM,2);

	_CY = ((f12Op2 & (1<<f12Op1))!=0);
	_Z = !(_CY);

	F12END();
}

uint32_t opUPDPSWW(void)
{
	F12DecodeOperands(ReadAM,2,ReadAM,2);

	v60WritePSW((v60ReadPSW() & (~f12Op2)) | (f12Op1 & f12Op2));

	F12END();
}

uint32_t opUPDPSWH(void)
{
	F12DecodeOperands(ReadAM,2,ReadAM,2);

	f12Op2 &= 0xFFFF;
	f12Op1 &= 0xFFFF;
	v60WritePSW((v60ReadPSW() & (~f12Op2)) | (f12Op1 & f12Op2));

	F12END();
}

uint32_t opXCHB(void) /* TRUSTED */
{
	uint8_t appb, temp;

	F12DecodeOperands(ReadAMAddress,0,ReadAMAddress,0);

	F12LOADOP1BYTE();
	temp=appb;
	F12LOADOP2BYTE();
	F12STOREOP1BYTE();
	appb=temp;
	F12STOREOP2BYTE();

	F12END()
}

uint32_t opXCHH(void) /* TRUSTED */
{
	uint16_t apph, temp;

	F12DecodeOperands(ReadAMAddress,1,ReadAMAddress,1);

	F12LOADOP1HALF();
	temp=apph;
	F12LOADOP2HALF();
	F12STOREOP1HALF();
	apph=temp;
	F12STOREOP2HALF();

	F12END()
}

uint32_t opXCHW(void) /* TRUSTED */
{
	uint32_t appw, temp;

	F12DecodeOperands(ReadAMAddress,2,ReadAMAddress,2);

	F12LOADOP1WORD();
	temp=appw;
	F12LOADOP2WORD();
	F12STOREOP1WORD();
	appw=temp;
	F12STOREOP2WORD();

	F12END()
}

uint32_t opXORB(void) /* TRUSTED (C too!) */
{
	uint8_t appb;
	F12DecodeOperands(ReadAM,0,ReadAMAddress,0);

	F12LOADOP2BYTE();

	XORB(appb, (uint8_t)f12Op1);

	F12STOREOP2BYTE();
	F12END();
}

uint32_t opXORH(void) /* TRUSTED (C too!) */
{
	uint16_t apph;
	F12DecodeOperands(ReadAM,1,ReadAMAddress,1);

	F12LOADOP2HALF();

	XORW(apph, (uint16_t)f12Op1);

	F12STOREOP2HALF();
	F12END();
}

uint32_t opXORW(void) /* TRUSTED (C too!) */
{
	uint32_t appw;
	F12DecodeOperands(ReadAM,2,ReadAMAddress,2);

	F12LOADOP2WORD();

	XORL(appw, (uint32_t)f12Op1);

	F12STOREOP2WORD();
	F12END();
}

uint32_t opMULX(void)
{
	int32_t a,b;
	int64_t res;

	F12DecodeOperands(ReadAM,2,ReadAMAddress,3);

	if (f12Flag2)
	{
		a=v60.reg[f12Op2&0x1F];
	}
	else
	{
		a=MemRead32(f12Op2);
	}

	res = (int64_t)a * (int64_t)(int32_t)f12Op1;

	b = (int32_t)((res >> 32)&0xffffffff);
	a = (int32_t)(res&0xffffffff);

	_S = ((b & 0x80000000) != 0);
	_Z = (a == 0 && b == 0);

	if (f12Flag2)
	{
		v60.reg[f12Op2&0x1F]=a;
		v60.reg[(f12Op2&0x1F)+1]=b;
	}
	else
	{
		MemWrite32(f12Op2,a);
		MemWrite32(f12Op2+4,b);
	}

	F12END();
}

uint32_t opMULUX(void)
{
	int32_t a,b;
	uint64_t res;

	F12DecodeOperands(ReadAM,2,ReadAMAddress,3);

	if (f12Flag2)
	{
		a=v60.reg[f12Op2&0x1F];
	}
	else
	{
		a=MemRead32(f12Op2);
	}

	res = (uint64_t)a * (uint64_t)f12Op1;
	b = (int32_t)((res >> 32)&0xffffffff);
	a = (int32_t)(res&0xffffffff);

	_S = ((b & 0x80000000) != 0);
	_Z = (a == 0 && b == 0);

	if (f12Flag2)
	{
		v60.reg[f12Op2&0x1F]=a;
		v60.reg[(f12Op2&0x1F)+1]=b;
	}
	else
	{
		MemWrite32(f12Op2,a);
		MemWrite32(f12Op2+4,b);
	}

	F12END();
}
