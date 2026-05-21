
/*
 * CMPC: What happens to _S flag if the strings are identical?
 *   I suppose that it will be cleared. And is it set or cleared
 *   when the first one is a substring of the second? I suppose
 *   cleared (since _S should be (src > dst))
 * MOVC: Why MOVCS does not exist in downward version?
 * SHCHDB/SHCHDH: R27 is filled with the offset from the start or from the end?
 *
 * Strange stuff:
 *   SCHC opcodes does *not* modify _Z flag as stated in V60 manual:
 *   they do the opposite (set if not found, reset if found)
 */

uint32_t f7aOp1, f7aOp2;
uint8_t f7aFlag1, f7aFlag2;
uint32_t f7aLenOp1, f7aLenOp2;
uint8_t subOp;

uint32_t f7bBamOffset1, f7bBamOffset2;

#define f7bOp1 f7aOp1
#define f7bFlag1 f7aFlag1
#define f7bOp2 f7aOp2
#define f7bFlag2 f7aFlag2
#define f7bLen  f7aLenOp1

#define f7cOp1 f7aOp1
#define f7cOp2 f7aOp2
#define f7cLen f7aLenOp1
#define f7cFlag1 f7aFlag1
#define f7cFlag2 f7aFlag2

#define F7AEND()	\
	return amLength1 + amLength2 + 4;

#define F7BEND()	\
	return amLength1 + amLength2 + 3;

#define F7CEND()	\
	return amLength1 + amLength2 + 3;

#define F7BCREATEBITMASK(x)	\
	x=((1<<(x))-1)

#define F7CCREATEBITMASK(x)	\
	x=((1<<(x))-1)

void F7aDecodeOperands(uint32_t (*DecodeOp1)(void), uint8_t dim1, uint32_t (*DecodeOp2)(void), uint8_t dim2)
{
	uint8_t appb;
	// Decode first operand
	modDim=dim1;
	modM=subOp&0x40;
	modAdd=PC+2;
	amLength1=DecodeOp1();
	f7aFlag1=amFlag;
	f7aOp1=amOut;

	// Decode length
	appb=OpRead8(PC+2+amLength1);
	if (appb&0x80)
		f7aLenOp1=v60.reg[appb&0x1F];
	else
		f7aLenOp1=appb;

	// Decode second operand
	modDim=dim2;
	modM=subOp&0x20;
	modAdd=PC+3+amLength1;
	amLength2=DecodeOp2();
	f7aFlag2=amFlag;
	f7aOp2=amOut;

	// Decode length
	appb=OpRead8(PC+3+amLength1+amLength2);
	if (appb&0x80)
		f7aLenOp2=v60.reg[appb&0x1F];
	else
		f7aLenOp2=appb;
}

void F7bDecodeFirstOperand(uint32_t (*DecodeOp1)(void), uint8_t dim1)
{
	uint8_t appb;
	// Decode first operand
	modDim=dim1;
	modM=subOp&0x40;
	modAdd=PC+2;
	amLength1=DecodeOp1();
	f7bFlag1=amFlag;
	f7bOp1=amOut;

	// Decode ext
	appb=OpRead8(PC+2+amLength1);
	if (appb&0x80)
		f7bLen=v60.reg[appb&0x1F];
	else
		f7bLen=appb;
}


void F7bWriteSecondOperand(uint8_t dim2)
{
	modDim=dim2;
	modM=subOp&0x20;
	modAdd=PC+3+amLength1;
	amLength2=WriteAM();
}


void F7bDecodeOperands(uint32_t (*DecodeOp1)(void), uint8_t dim1, uint32_t (*DecodeOp2)(void), uint8_t dim2)
{
	// Decode first operand
	F7bDecodeFirstOperand(DecodeOp1,dim1);
	f7bBamOffset1 = bamOffset;

	// Decode second operand
	modDim=dim2;
	modM=subOp&0x20;
	modAdd=PC+3+amLength1;
	amLength2=DecodeOp2();
	f7bFlag2=amFlag;
	f7bOp2=amOut;
	f7bBamOffset2 = bamOffset;
}

void F7cDecodeOperands(uint32_t (*DecodeOp1)(void), uint8_t dim1, uint32_t (*DecodeOp2)(void), uint8_t dim2)
{
	uint8_t appb;
	// Decode first operand
	modDim=dim1;
	modM=subOp&0x40;
	modAdd=PC+2;
	amLength1=DecodeOp1();
	f7cFlag1=amFlag;
	f7cOp1=amOut;

	// Decode second operand
	modDim=dim2;
	modM=subOp&0x20;
	modAdd=PC+2+amLength1;
	amLength2=DecodeOp2();
	f7cFlag2=amFlag;
	f7cOp2=amOut;

	// Decode ext
	appb=OpRead8(PC+2+amLength1+amLength2);
	if (appb&0x80)
		f7cLen=v60.reg[appb&0x1F];
	else
		f7cLen=appb;
}

#define F7CLOADOP1BYTE(appb) \
	if (f7cFlag1) \
		appb = (uint8_t)(v60.reg[f7cOp1]&0xFF); \
	else \
		appb = MemRead8(f7cOp1);

#define F7CLOADOP2BYTE(appb) \
	if (f7cFlag2) \
		appb = (uint8_t)(v60.reg[f7cOp2]&0xFF); \
	else \
		appb = MemRead8(f7cOp2);


#define F7CSTOREOP2BYTE() \
	if (f7cFlag2) \
		SETREG8(v60.reg[f7cOp2], appb); \
	else \
		MemWrite8(f7cOp2, appb);

#define F7CSTOREOP2HALF() \
	if (f7cFlag2) \
		SETREG16(v60.reg[f7cOp2], apph); \
	else \
		MemWrite16(f7cOp2, apph);

uint32_t opCMPSTRB(uint8_t bFill, uint8_t bStop)
{
	uint32_t i,dest;
	uint8_t c1,c2;

	F7aDecodeOperands(ReadAMAddress,0,ReadAMAddress,0);

	// Filling
	if (bFill)
	{
		if (f7aLenOp1 < f7aLenOp2)
		{
			for (i=f7aLenOp1;i<f7aLenOp2;i++)
				MemWrite8(f7aOp1+i,(uint8_t)R26);
		}
		else if (f7aLenOp2 < f7aLenOp1)
		{
			for (i=f7aLenOp2;i<f7aLenOp1;i++)
				MemWrite8(f7aOp2+i,(uint8_t)R26);
		}
	}

	dest=(f7aLenOp1 < f7aLenOp2 ? f7aLenOp1 : f7aLenOp2);

	_Z = 0;
	_S = 0;
	if (bStop) _CY = 1;

	for (i=0;i<dest;i++)
	{
		c1=MemRead8(f7aOp1+i);
		c2=MemRead8(f7aOp2+i);

		if (c1>c2)
		{
			_S=1;	break;
		}
		else if (c2>c1)
		{
			_S=0;	break;
		}

		if (bStop)
			if (c1==(uint8_t)R26 || c2==(uint8_t)R26)
			{
				_CY=0;
				break;
			}
	}

	R28=f7aLenOp1+i;
	R27=f7aLenOp2+i;

	if (i==dest)
	{
		if (f7aLenOp1 > f7aLenOp2)
			_S=1;
		else if (f7aLenOp2 > f7aLenOp1)
			_S=0;
		else
			_Z=1;
	}

	F7AEND();
}

uint32_t opCMPSTRH(uint8_t bFill, uint8_t bStop)
{
	uint32_t i,dest;
	uint16_t c1,c2;

	F7aDecodeOperands(ReadAMAddress,0,ReadAMAddress,0);

	// Filling
	if (bFill)
	{
		if (f7aLenOp1 < f7aLenOp2)
		{
			for (i=f7aLenOp1;i<f7aLenOp2;i++)
				MemWrite16(f7aOp1+i*2,(uint16_t)R26);
		}
		else if (f7aLenOp2 < f7aLenOp1)
		{
			for (i=f7aLenOp2;i<f7aLenOp1;i++)
				MemWrite16(f7aOp2+i*2,(uint16_t)R26);
		}
	}

	dest=(f7aLenOp1 < f7aLenOp2 ? f7aLenOp1 : f7aLenOp2);

	_Z = 0;
	_S = 0;
	if (bStop) _CY = 1;

	for (i=0;i<dest;i++)
	{
		c1=MemRead16(f7aOp1+i*2);
		c2=MemRead16(f7aOp2+i*2);

		if (c1>c2)
		{
			_S=1;	break;
		}
		else if (c2>c1)
		{
			_S=0;	break;
		}

		if (bStop)
			if (c1==(uint16_t)R26 || c2==(uint16_t)R26)
			{
				_CY=0;
				break;
			}
	}

	R28=f7aLenOp1+i*2;
	R27=f7aLenOp2+i*2;

	if (i==dest)
	{
		if (f7aLenOp1 > f7aLenOp2)
			_S=1;
		else if (f7aLenOp2 > f7aLenOp1)
			_S=0;
		else
			_Z=1;
	}

	F7AEND();
}



uint32_t opMOVSTRUB(uint8_t bFill, uint8_t bStop) /* TRUSTED (0,0) (1,0) */
{
	uint32_t i,dest;
	uint8_t c1;

//	if (bStop)
//	{
//		int a=1;
//	}

	F7aDecodeOperands(ReadAMAddress,0,ReadAMAddress,0);

	dest=(f7aLenOp1 < f7aLenOp2 ? f7aLenOp1 : f7aLenOp2);

	for (i=0;i<dest;i++)
	{
		MemWrite8(f7aOp2+i,(c1=MemRead8(f7aOp1+i)));

		if (bStop && c1==(uint8_t)R26)
			break;
	}

	R28=f7aOp1+i;
	R27=f7aOp2+i;

	if (bFill && f7aLenOp1 < f7aLenOp2)
	{
		for (;i<f7aLenOp2;i++)
			MemWrite8(f7aOp2+i,(uint8_t)R26);

		R27=f7aOp2+i;
	}


	F7AEND();
}

uint32_t opMOVSTRDB(uint8_t bFill, uint8_t bStop)
{
	uint32_t i,dest;
	uint8_t c1;

	F7aDecodeOperands(ReadAMAddress,0,ReadAMAddress,0);

	dest=(f7aLenOp1 < f7aLenOp2 ? f7aLenOp1 : f7aLenOp2);

	for (i=0;i<dest;i++)
	{
		MemWrite8(f7aOp2+(dest-i-1),(c1=MemRead8(f7aOp1+(dest-i-1))));

		if (bStop && c1==(uint8_t)R26)
			break;
	}

	R28=f7aOp1+(f7aLenOp1-i-1);
	R27=f7aOp2+(f7aLenOp2-i-1);

	if (bFill && f7aLenOp1 < f7aLenOp2)
	{
		for (;i<f7aLenOp2;i++)
			MemWrite8(f7aOp2+dest+(f7aLenOp2-i-1),(uint8_t)R26);

		R27=f7aOp2+(f7aLenOp2-i-1);
	}


	F7AEND();
}


uint32_t opMOVSTRUH(uint8_t bFill, uint8_t bStop) /* TRUSTED (0,0) (1,0) */
{
	uint32_t i,dest;
	uint16_t c1;

//	if (bStop)
//	{	int a=1; }

	F7aDecodeOperands(ReadAMAddress,1,ReadAMAddress,1);

	dest=(f7aLenOp1 < f7aLenOp2 ? f7aLenOp1 : f7aLenOp2);

	for (i=0;i<dest;i++)
	{
		MemWrite16(f7aOp2+i*2,(c1=MemRead16(f7aOp1+i*2)));

		if (bStop && c1==(uint16_t)R26)
			break;
	}

	R28=f7aOp1+i*2;
	R27=f7aOp2+i*2;

	if (bFill && f7aLenOp1 < f7aLenOp2)
	{
		for (;i<f7aLenOp2;i++)
			MemWrite16(f7aOp2+i*2,(uint16_t)R26);

		R27=f7aOp2+i*2;
	}

	F7AEND();
}

uint32_t opMOVSTRDH(uint8_t bFill, uint8_t bStop)
{
	uint32_t i,dest;
	uint16_t c1;

//	if (bFill | bStop)
//	{ int a=1; }

	F7aDecodeOperands(ReadAMAddress,1,ReadAMAddress,1);

//	if (f7aLenOp1 != f7aLenOp2)
//	{ int a=1; }

	dest=(f7aLenOp1 < f7aLenOp2 ? f7aLenOp1 : f7aLenOp2);

	for (i=0;i<dest;i++)
	{
		MemWrite16(f7aOp2+(dest-i-1)*2,(c1=MemRead16(f7aOp1+(dest-i-1)*2)));

		if (bStop && c1==(uint16_t)R26)
			break;
	}

	R28=f7aOp1+(f7aLenOp1-i-1)*2;
	R27=f7aOp2+(f7aLenOp2-i-1)*2;

	if (bFill && f7aLenOp1 < f7aLenOp2)
	{
		for (;i<f7aLenOp2;i++)
			MemWrite16(f7aOp2+(f7aLenOp2-i-1)*2,(uint16_t)R26);

		R27=f7aOp2+(f7aLenOp2-i-1)*2;
	}

	F7AEND();
}

uint32_t opSEARCHUB(uint8_t bSearch)
{
	uint8_t appb;
	uint32_t i;

	F7bDecodeOperands(ReadAMAddress,0,ReadAM,0);

	for (i=0;i<f7bLen;i++)
	{
		appb = (MemRead8(f7bOp1+i)==(uint8_t)f7bOp2);
		if ((bSearch && appb) || (!bSearch && !appb))
			break;
	}

	R28=f7bOp1+i;
	R27=i;

	// This is the opposite as stated in V60 manual...
	if (i!=f7bLen)
		_Z=0;
	else
		_Z=1;

	F7BEND();
}

uint32_t opSEARCHUH(uint8_t bSearch)
{
	uint8_t appb;
	uint32_t i;

	F7bDecodeOperands(ReadAMAddress,1,ReadAM,1);

	for (i=0;i<f7bLen;i++)
	{
		appb = (MemRead16(f7bOp1+i*2)==(uint16_t)f7bOp2);
		if ((bSearch && appb) || (!bSearch && !appb))
			break;
	}

	R28=f7bOp1+i*2;
	R27=i;

	if (i!=f7bLen)
		_Z=0;
	else
		_Z=1;

	F7BEND();
}

uint32_t opSEARCHDB(uint8_t bSearch)
{
	uint8_t appb;
	int32_t i;

	F7bDecodeOperands(ReadAMAddress,0,ReadAM,0);

	for (i=f7bLen;i>=0;i--)
	{
		appb = (MemRead8(f7bOp1+i)==(uint8_t)f7bOp2);
		if ((bSearch && appb) || (!bSearch && !appb))
			break;
	}

	R28=f7bOp1+i;
	R27=i;

	// This is the opposite as stated in V60 manual...
	if ((uint32_t)i!=f7bLen)
		_Z=0;
	else
		_Z=1;

	F7BEND();
}

uint32_t opSEARCHDH(uint8_t bSearch)
{
	uint8_t appb;
	int32_t i;

	F7bDecodeOperands(ReadAMAddress,1,ReadAM,1);

	for (i=f7bLen-1;i>=0;i--)
	{
		appb = (MemRead16(f7bOp1+i*2)==(uint16_t)f7bOp2);
		if ((bSearch && appb) || (!bSearch && !appb))
			break;
	}

	R28=f7bOp1+i*2;
	R27=i;

	if ((uint32_t)i!=f7bLen)
		_Z=0;
	else
		_Z=1;

	F7BEND();
}


uint32_t opSCHCUB(void) { return opSEARCHUB(1); }
uint32_t opSCHCUH(void) { return opSEARCHUH(1); }
uint32_t opSCHCDB(void) { return opSEARCHDB(1); }
uint32_t opSCHCDH(void) { return opSEARCHDH(1); }
uint32_t opSKPCUB(void) { return opSEARCHUB(0); }
uint32_t opSKPCUH(void) { return opSEARCHUH(0); }
uint32_t opSKPCDB(void) { return opSEARCHDB(0); }
uint32_t opSKPCDH(void) { return opSEARCHDH(0); }

uint32_t opCMPCB(void) { return opCMPSTRB(0,0); }
uint32_t opCMPCH(void) { return opCMPSTRH(0,0); }
uint32_t opCMPCFB(void) { return opCMPSTRB(1,0); }
uint32_t opCMPCFH(void) { return opCMPSTRH(1,0); }
uint32_t opCMPCSB(void) { return opCMPSTRB(0,1); }
uint32_t opCMPCSH(void) { return opCMPSTRH(0,1); }

uint32_t opMOVCUB(void) { return opMOVSTRUB(0,0); }
uint32_t opMOVCUH(void) { return opMOVSTRUH(0,0); }
uint32_t opMOVCFUB(void) { return opMOVSTRUB(1,0); }
uint32_t opMOVCFUH(void) { return opMOVSTRUH(1,0); }
uint32_t opMOVCSUB(void) { return opMOVSTRUB(0,1); }
uint32_t opMOVCSUH(void) { return opMOVSTRUH(0,1); }

uint32_t opMOVCDB(void) { return opMOVSTRDB(0,0); }
uint32_t opMOVCDH(void) { return opMOVSTRDH(0,0); }
uint32_t opMOVCFDB(void) { return opMOVSTRDB(1,0); }
uint32_t opMOVCFDH(void) { return opMOVSTRDH(1,0); }

uint32_t opEXTBFZ(void) /* TRUSTED */
{
	F7bDecodeFirstOperand(BitReadAM, 11);

	F7BCREATEBITMASK(f7bLen);

	modWriteValW=(f7bOp1 >> bamOffset) & f7bLen;

	F7bWriteSecondOperand(2);

	F7BEND();
}

uint32_t opEXTBFS(void) /* TRUSTED */
{
 	F7bDecodeFirstOperand(BitReadAM, 11);

	F7BCREATEBITMASK(f7bLen);

	modWriteValW=(f7bOp1 >> bamOffset) & f7bLen;
	if (modWriteValW & ((f7bLen+1)>>1))
		modWriteValW |= ~f7bLen;

	F7bWriteSecondOperand(2);

	F7BEND();
}

uint32_t opEXTBFL(void)
{
	uint32_t appw;

	F7bDecodeFirstOperand(BitReadAM, 11);

	appw=f7bLen;
	F7BCREATEBITMASK(f7bLen);

	modWriteValW=(f7bOp1 >> bamOffset) & f7bLen;
	modWriteValW<<=32-appw;

	F7bWriteSecondOperand(2);

	F7BEND();
}

uint32_t opSCHBS(uint32_t bSearch1)
{
	uint32_t i,data;
	uint32_t offset;

	F7bDecodeFirstOperand(BitReadAMAddress,10);

	// Read first uint8_t
	f7bOp1 += bamOffset/8;
	data = MemRead8(f7bOp1);
	offset = bamOffset&7;

	// Scan bitstring
	for (i=0;i<f7bLen;i++)
	{
		// Update the work register
		R28 = f7bOp1;

		// There is a 0/1 at current offset?
		if ((bSearch1 && (data&(1<<offset))) ||
			(!bSearch1 && !(data&(1<<offset))))
			break;

		// Next bit please
		offset++;
		if (offset==8)
		{
			// Next uint8_t please
			offset=0;
			f7bOp1++;
			data = MemRead8(f7bOp1);
		}
	}

	// Set zero if bit not found
	_Z = (i == f7bLen);

	// Write to destination the final offset
	modWriteValW = i;
	F7bWriteSecondOperand(2);

	F7BEND();
}

uint32_t opSCH0BSU(void) { return opSCHBS(0); }
uint32_t opSCH1BSU(void) { return opSCHBS(1); }

uint32_t opINSBFR(void)
{
	uint32_t appw;
	F7cDecodeOperands(ReadAM,2,BitReadAMAddress,11);

	F7CCREATEBITMASK(f7cLen);

	f7cOp2 += bamOffset/8;
	appw = MemRead32(f7cOp2);
	bamOffset &= 7;

	appw &= ~(f7cLen << bamOffset);
	appw |=  (f7cLen & f7cOp1) << bamOffset;

	MemWrite32(f7cOp2, appw);

	F7CEND();
}

uint32_t opINSBFL(void)
{
	uint32_t appw;
	F7cDecodeOperands(ReadAM,2,BitReadAMAddress,11);

	f7cOp1 >>= (32-f7cLen);

	F7CCREATEBITMASK(f7cLen);

	f7cOp2 += bamOffset/8;
	appw = MemRead32(f7cOp2);
	bamOffset &= 7;

	appw &= ~(f7cLen << bamOffset);
	appw |=  (f7cLen & f7cOp1) << bamOffset;

	MemWrite32(f7cOp2, appw);

	F7CEND();
}

uint32_t opMOVBSD(void)
{
	uint32_t i;
	uint8_t srcdata, dstdata;

	F7bDecodeOperands(BitReadAMAddress,10,BitReadAMAddress,10);

//	if (f7bLen!=1)
//	{ int a=1; }

	f7bBamOffset1 += f7bLen-1;
	f7bBamOffset2 += f7bLen-1;

	f7bOp1 += f7bBamOffset1/8;
	f7bOp2 += f7bBamOffset2/8;

	f7bBamOffset1 &= 7;
	f7bBamOffset2 &= 7;

	srcdata = MemRead8(f7bOp1);
	dstdata = MemRead8(f7bOp2);

	for (i=0;i<f7bLen;i++)
	{
		// Update work registers
		R28 = f7bOp1;
		R27 = f7bOp2;

		dstdata &= ~(1 << f7bBamOffset2);
		dstdata |= ((srcdata >> f7bBamOffset1) & 1) << f7bBamOffset2;

		if (f7bBamOffset1 == 0)
		{
			f7bBamOffset1 = 8;
			f7bOp1--;
			srcdata = MemRead8(f7bOp1);
		}
		if (f7bBamOffset2 == 0)
		{
			MemWrite8(f7bOp2, dstdata);
			f7bBamOffset2 = 8;
			f7bOp2--;
			dstdata = MemRead8(f7bOp2);
		}

		f7bBamOffset1--;
		f7bBamOffset2--;
	}

	// Flush of the final data
	if (f7bBamOffset2 != 7)
		MemWrite8(f7bOp2, dstdata);

	F7BEND();
}

uint32_t opMOVBSU(void)
{
	uint32_t i;
	uint8_t srcdata, dstdata;

	F7bDecodeOperands(BitReadAMAddress,10,BitReadAMAddress,10);

	f7bOp1 += f7bBamOffset1/8;
	f7bOp2 += f7bBamOffset2/8;

	f7bBamOffset1 &= 7;
	f7bBamOffset2 &= 7;

	srcdata = MemRead8(f7bOp1);
	dstdata = MemRead8(f7bOp2);

	for (i=0;i<f7bLen;i++)
	{
		// Update work registers
		R28 = f7bOp1;
		R27 = f7bOp2;

		dstdata &= ~(1 << f7bBamOffset2);
		dstdata |= ((srcdata >> f7bBamOffset1) & 1) << f7bBamOffset2;

		f7bBamOffset1++;
		f7bBamOffset2++;
		if (f7bBamOffset1 == 8)
		{
			f7bBamOffset1 = 0;
			f7bOp1++;
			srcdata = MemRead8(f7bOp1);
		}
		if (f7bBamOffset2 == 8)
		{
			MemWrite8(f7bOp2, dstdata);
			f7bBamOffset2 = 0;
			f7bOp2++;
			dstdata = MemRead8(f7bOp2);
		}
	}

	// Flush of the final data
	if (f7bBamOffset2 != 0)
		MemWrite8(f7bOp2, dstdata);

	F7BEND();
}

// RADM 0x20f4b8 holds the time left

uint32_t opADDDC(void)
{
	uint8_t appb;
	uint8_t src, dst;

	F7cDecodeOperands(ReadAM, 0, ReadAMAddress, 0);

	if (f7cLen != 0)
	{
		logerror("ADDDC %x (pat: %x)\n", f7cOp1, f7cLen);
	}

	F7CLOADOP2BYTE(appb);

	src = (appb >> 4) * 10 + (appb & 0xF);
	dst = (uint8_t)(f7cOp1 >> 4) * 10 + (uint8_t)(f7cOp1 & 0xF);

	appb = src + dst + (_CY?1:0);

	if (appb >= 100)
	{
		appb -= 100;
		_CY = 1;
	}
	else
		_CY = 0;

	// compute z flag:
	// cleared if result non-zero or carry generated
	// unchanged otherwise
	if (appb != 0 || _CY)
		_Z = 0;

	appb = ((appb/10)<<4) | (appb % 10);

	F7CSTOREOP2BYTE();
	F7CEND();
}

uint32_t opSUBDC(void)
{
	int8_t appb;
	uint32_t src, dst;

	F7cDecodeOperands(ReadAM, 0, ReadAMAddress, 0);

	if (f7cLen != 0)
	{
		logerror("SUBDC %x (pat: %x)\n", f7cOp1, f7cLen);
	}

	F7CLOADOP2BYTE(appb);

	src = ((appb & 0xF0) >> 4) * 10 + (appb & 0xF);
	dst = (uint32_t)(f7cOp1 >> 4) * 10 + (uint32_t)(f7cOp1 & 0xF);

	// Note that this APPB must be SIGNED!
	appb = (int32_t)src - (int32_t)dst - (_CY?1:0);

	if (appb < 0)
	{
		appb += 100;
		_CY = 1;
	}
	else
		_CY = 0;

	// compute z flag:
	// cleared if result non-zero or carry generated
	// unchanged otherwise
	if (appb != 0 || _CY)
		_Z = 0;

	appb = ((appb/10)<<4) | (appb % 10);

	F7CSTOREOP2BYTE();
	F7CEND();
}

uint32_t opSUBRDC(void)
{
	F7cDecodeOperands(ReadAM, 0, ReadAMAddress, 0);

	logerror("SUBRDC %x (pat: %x)\n", f7cOp1, f7cLen);

	F7CEND();
}

uint32_t opCVTDPZ(void)
{
	uint16_t apph;

	F7cDecodeOperands(ReadAM, 0, ReadAMAddress, 1);

	apph = (uint16_t)(((f7cOp1 >> 4) & 0xF) | ((f7cOp1 & 0xF) << 8));
	apph |= (f7cLen);
	apph |= (f7cLen<<8);

	// Z flag is unchanged if src is zero, cleared otherwise
	if (f7cOp1 != 0) _Z = 0;

	F7CSTOREOP2HALF();
	F7CEND();
}

uint32_t opCVTDZP(void)
{
	uint8_t appb;
	F7cDecodeOperands(ReadAM, 1, ReadAMAddress, 0);

	if ((f7cOp1 & 0xF0) != (f7cLen & 0xF0) || ((f7cOp1 >> 8) & 0xF0) != (f7cLen & 0xF0))
	{
		// Decimal exception
		logerror("CVTD.ZP Decimal exception #1!\n");
	}

	if ((f7cOp1 & 0xF) > 9 || ((f7cOp1 >> 8) & 0xF) > 9)
	{
		// Decimal exception
		logerror("CVTD.ZP Decimal exception #2!\n");
	}

	appb = (uint8_t)(((f7cOp1 >> 8) & 0xF) | ((f7cOp1 & 0xF) << 4));
	if (appb != 0) _Z = 0;

	F7CSTOREOP2BYTE();
	F7CEND();
}

uint32_t op58UNHANDLED(void)
{
	logerror("Unhandled 58 opcode at PC: /%06x\n", PC);
	abort();
}

uint32_t op5AUNHANDLED(void)
{
	logerror("Unhandled 5A opcode at PC: /%06x\n", PC);
	abort();
}

uint32_t op5BUNHANDLED(void)
{
	logerror("Unhandled 5B opcode at PC: /%06x\n", PC);
	abort();
}

uint32_t op5DUNHANDLED(void)
{
	logerror("Unhandled 5D opcode at PC: /%06x\n", PC);
	abort();
}

uint32_t op59UNHANDLED(void)
{
	logerror("Unhandled 59 opcode at PC: /%06x\n", PC);
	abort();
}

uint32_t (*Op59Table[32])(void) =
{
	opADDDC,
	opSUBDC,
	op59UNHANDLED,
	op59UNHANDLED,
	op59UNHANDLED,
	op59UNHANDLED,
	op59UNHANDLED,
	op59UNHANDLED,
	op59UNHANDLED,
	op59UNHANDLED,
	op59UNHANDLED,
	op59UNHANDLED,
	op59UNHANDLED,
	op59UNHANDLED,
	op59UNHANDLED,
	op59UNHANDLED,
	opCVTDPZ,
	op59UNHANDLED,
	op59UNHANDLED,
	op59UNHANDLED,
	op59UNHANDLED,
	op59UNHANDLED,
	op59UNHANDLED,
	op59UNHANDLED,
	opCVTDZP,
	op59UNHANDLED,
	op59UNHANDLED,
	op59UNHANDLED,
	op59UNHANDLED,
	op59UNHANDLED,
	op59UNHANDLED,
	op59UNHANDLED
};


uint32_t (*Op5BTable[32])(void) =
{
	opSCH0BSU,		op5BUNHANDLED,
	opSCH1BSU,    	op5BUNHANDLED,
	op5BUNHANDLED,op5BUNHANDLED,
	op5BUNHANDLED,op5BUNHANDLED,
	opMOVBSU,		opMOVBSD,
	op5BUNHANDLED,op5BUNHANDLED,
	op5BUNHANDLED,op5BUNHANDLED,
	op5BUNHANDLED,op5BUNHANDLED,
	op5BUNHANDLED,op5BUNHANDLED,
	op5BUNHANDLED,op5BUNHANDLED,
	op5BUNHANDLED,op5BUNHANDLED,
	op5BUNHANDLED,op5BUNHANDLED,
	op5BUNHANDLED,op5BUNHANDLED,
	op5BUNHANDLED,op5BUNHANDLED,
	op5BUNHANDLED,op5BUNHANDLED,
	op5BUNHANDLED,op5BUNHANDLED
};


uint32_t (*Op5DTable[32])(void) =
{
	op5DUNHANDLED,
	op5DUNHANDLED,
	op5DUNHANDLED,
	op5DUNHANDLED,
	op5DUNHANDLED,
	op5DUNHANDLED,
	op5DUNHANDLED,
	op5DUNHANDLED,
	opEXTBFS,
	opEXTBFZ,
	opEXTBFL,
	op5DUNHANDLED,
	op5DUNHANDLED,
	op5DUNHANDLED,
	op5DUNHANDLED,
	op5DUNHANDLED,
	op5DUNHANDLED,
	op5DUNHANDLED,
	op5DUNHANDLED,
	op5DUNHANDLED,
	op5DUNHANDLED,
	op5DUNHANDLED,
	op5DUNHANDLED,
	op5DUNHANDLED,
	opINSBFR,
	opINSBFL,
	op5DUNHANDLED,
	op5DUNHANDLED,
	op5DUNHANDLED,
	op5DUNHANDLED,
	op5DUNHANDLED,
	op5DUNHANDLED
};

uint32_t (*Op585ATable[32][2])(void) =
{
	{	opCMPCB      ,		opCMPCH      	},
	{	opCMPCFB     ,		opCMPCFH     	},
	{	opCMPCSB     ,		opCMPCSH      },
	{	op58UNHANDLED,		op5AUNHANDLED	},
	{	op58UNHANDLED,		op5AUNHANDLED	},
	{	op58UNHANDLED,		op5AUNHANDLED	},
	{	op58UNHANDLED,		op5AUNHANDLED	},
	{	op58UNHANDLED,		op5AUNHANDLED	},
	{	opMOVCUB     ,		opMOVCUH     	},
	{	opMOVCDB     ,		opMOVCDH		},
	{	opMOVCFUB    ,		opMOVCFUH    	},
	{	opMOVCFDB    ,		opMOVCFDH    	},
	{	opMOVCSUB    ,		opMOVCSUH	    },
	{	op58UNHANDLED,		op5AUNHANDLED	},
	{	op58UNHANDLED,		op5AUNHANDLED	},
	{	op58UNHANDLED,		op5AUNHANDLED	},
	{	op58UNHANDLED,		op5AUNHANDLED	},
	{	op58UNHANDLED,		op5AUNHANDLED	},
	{	op58UNHANDLED,		op5AUNHANDLED	},
	{	op58UNHANDLED,		op5AUNHANDLED	},
	{	op58UNHANDLED,		op5AUNHANDLED	},
	{	op58UNHANDLED,		op5AUNHANDLED	},
	{	op58UNHANDLED,		op5AUNHANDLED	},
	{	op58UNHANDLED,		op5AUNHANDLED	},
	{	opSCHCUB     ,		opSCHCUH     	},
	{	opSCHCDB     ,		opSCHCDH     	},
	{	opSKPCUB     ,		opSKPCUH     	},
	{	opSKPCDB     ,		opSKPCDH		},
	{	op58UNHANDLED,		op5AUNHANDLED	},
	{	op58UNHANDLED,		op5AUNHANDLED	},
	{	op58UNHANDLED,		op5AUNHANDLED	},
	{	op58UNHANDLED,		op5AUNHANDLED	}
};

uint32_t op58(void)
{
	subOp = OpRead8(PC + 1);

	return Op585ATable[subOp&0x1F][0]();
}

uint32_t op5A(void)
{
	subOp = OpRead8(PC + 1);

	return Op585ATable[subOp&0x1F][1]();
}

uint32_t op5B(void)
{
	subOp = OpRead8(PC + 1);

	return Op5BTable[subOp&0x1F]();
}

uint32_t op5D(void)
{
	subOp = OpRead8(PC + 1);

	return Op5DTable[subOp&0x1F]();
}

uint32_t op59(void)
{
	subOp = OpRead8(PC + 1);

	return Op59Table[subOp&0x1F]();
}
