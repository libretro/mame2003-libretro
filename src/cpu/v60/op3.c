uint32_t opINCB(void) /* TRUSTED */
{
	uint8_t appb;
	modAdd=PC+1;
	modDim=0;

	amLength1=ReadAMAddress();

	if (amFlag)
		appb=(uint8_t)v60.reg[amOut];
	else
		appb=MemRead8(amOut);

	ADDB(appb, 1);

	if (amFlag)
		SETREG8(v60.reg[amOut], appb);
	else
		MemWrite8(amOut, appb);

	return amLength1+1;
}

uint32_t opINCH(void) /* TRUSTED */
{
	uint16_t apph;
	modAdd=PC+1;
	modDim=1;

	amLength1=ReadAMAddress();

	if (amFlag)
		apph=(uint16_t)v60.reg[amOut];
	else
		apph=MemRead16(amOut);

	ADDW(apph, 1);

	if (amFlag)
		SETREG16(v60.reg[amOut], apph);
	else
		MemWrite16(amOut, apph);

	return amLength1+1;
}

uint32_t opINCW(void) /* TRUSTED */
{
	uint32_t appw;
	modAdd=PC+1;
	modDim=2;

	amLength1=ReadAMAddress();

	if (amFlag)
		appw=v60.reg[amOut];
	else
		appw=MemRead32(amOut);

	ADDL(appw, 1);

	if (amFlag)
		v60.reg[amOut]=appw;
	else
		MemWrite32(amOut,appw);

	return amLength1+1;
}

uint32_t opDECB(void) /* TRUSTED */
{
	uint8_t appb;
	modAdd=PC+1;
	modDim=0;

	amLength1=ReadAMAddress();

	if (amFlag)
		appb=(uint8_t)v60.reg[amOut];
	else
		appb=MemRead8(amOut);

	SUBB(appb, 1);

	if (amFlag)
		SETREG8(v60.reg[amOut], appb);
	else
		MemWrite8(amOut, appb);

	return amLength1+1;
}

uint32_t opDECH(void) /* TRUSTED */
{
	uint16_t apph;
	modAdd=PC+1;
	modDim=1;

	amLength1=ReadAMAddress();

	if (amFlag)
		apph=(uint16_t)v60.reg[amOut];
	else
		apph=MemRead16(amOut);

	SUBW(apph, 1);

	if (amFlag)
		SETREG16(v60.reg[amOut], apph);
	else
		MemWrite16(amOut, apph);

	return amLength1+1;
}

uint32_t opDECW(void) /* TRUSTED */
{
	uint32_t appw;
	modAdd=PC+1;
	modDim=2;

	amLength1=ReadAMAddress();

	if (amFlag)
		appw=v60.reg[amOut];
	else
		appw=MemRead32(amOut);

	SUBL(appw, 1);

	if (amFlag)
		v60.reg[amOut]=appw;
	else
		MemWrite32(amOut,appw);

	return amLength1+1;
}

uint32_t opJMP(void) /* TRUSTED */
{
	modAdd=PC+1;
	modDim=0;

	// Read the address of the operand
	ReadAMAddress();

	// It cannot be a register!!
	assert(amFlag==0);

	// Jump there
	PC=amOut;
	ChangePC(PC);

	return 0;
}

uint32_t opJSR(void) /* TRUSTED */
{
	modAdd=PC + 1;
	modDim=0;

	// Read the address of the operand
	amLength1=ReadAMAddress();

	// It cannot be a register!!
	assert(amFlag==0);

	// Save NextPC into the stack
	SP -= 4;
	MemWrite32(SP, PC + amLength1 + 1);

	// Jump there
	PC=amOut;
	ChangePC(PC);

	return 0;
}

uint32_t opPREPARE(void)	/* somewhat TRUSTED */
{
	modAdd=PC+1;
	modDim=2;

	// Read the operand
	amLength1=ReadAM();

	// step 1: save frame pointer on the stack
	SP -= 4;
	MemWrite32(SP, FP);

	// step 2: FP = new SP
	FP = SP;

	// step 3: SP -= operand
	SP -= amOut;

	return amLength1 + 1;
}

uint32_t opRET(void) /* TRUSTED */
{
	modAdd=PC + 1;
	modDim=2;

	// Read the operand
	ReadAM();

	// Read return address from stack
	PC=MemRead32(SP);
	SP+=4;
	ChangePC(PC);

	// Restore AP from stack
	AP=MemRead32(SP);
	SP+=4;

	// Skip stack frame
	SP += amOut;

	return 0;
}

uint32_t opTRAP(void)
{
	modAdd=PC + 1;
	modDim=0;

	// Read the operand
	amLength1=ReadAM();

	// Normalize the flags
	NORMALIZEFLAGS();

	switch ((amOut >> 4) & 0xF)
	{
	case 0:
		if (!_OV) return amLength1+1;
		else break;
	case 1:
		if (_OV) return amLength1+1;
		else break;
	case 2:
		if (!_CY) return amLength1+1;
		else break;
	case 3:
		if (_CY) return amLength1+1;
		else break;
	case 4:
		if (!_Z) return amLength1+1;
		else break;
	case 5:
		if (_Z) return amLength1+1;
		else break;
	case 6:
		if (!(_CY | _Z)) return amLength1+1;
		else break;
	case 7:
		if ((_CY | _Z)) return amLength1+1;
		else break;
	case 8:
		if (!_S) return amLength1+1;
		else break;
	case 9:
		if (_S) return amLength1+1;
		else break;
	case 10:
		break;
	case 11:
		return amLength1+1;
	case 12:
		if (!(_S^_OV)) return amLength1+1;
		else break;
	case 13:
		if ((_S^_OV)) return amLength1+1;
		else break;
	case 14:
		if (!((_S^_OV)|_Z)) return amLength1+1;
		else break;
	case 15:
		if (((_S^_OV)|_Z)) return amLength1+1;
		else break;
	}

	UPDATEPSW();

	// Issue the software trap with interrupts
	SP -= 4;
	MemWrite32(SP, 0x3000 + 0x100 * (amOut&0xF));

	SP -= 4;
	MemWrite32(SP, PSW);

	SP -= 4;
	MemWrite32(SP, PC + amLength1 + 1);

	PC = GETINTVECT(48 + (amOut&0xF));
	ChangePC(PC);

	return 0;
}

uint32_t opRETIU(void) /* TRUSTED */
{
	uint32_t tempPSW;
	modAdd=PC + 1;
	modDim=1;

	// Read the operand
	ReadAM();

	// Restore PC and PSW from stack
	PC = MemRead32(SP);
	SP += 4;
	ChangePC(PC);

	tempPSW=MemRead32(SP);
	SP += 4;

	// Destroy stack frame
	SP += amOut;
	
	v60WritePSW(tempPSW);

	// Update all the flags from PSW
	UPDATECPUFLAGS();
	UPDATEFPUFLAGS();

	return 0;
}

uint32_t opRETIS(void)
{
	uint32_t appw;

	modAdd=PC + 1;
	modDim=1;

	// Read the operand
	ReadAM();

	// Restore PC and PSW from stack
	PC = MemRead32(SP);
	SP += 4;
	ChangePC(PC);

	appw = MemRead32(SP);
	SP += 4;

	v60WritePSW(appw);

	// Destroy stack frame
	SP += amOut;

	// Update only CPU flags from PSW @@@
//	UPDATECPUFLAGS();

	return 0;
}

uint32_t opSTTASK(void)
{
	int i;
	uint32_t adr;

	modAdd=PC + 1;
	modDim=2;

	amLength1 = ReadAM();

	adr = TCB;

	UPDATEPSW();
	v60WritePSW(PSW | 0x10000000);

	MemWrite32(adr, TKCW);
	adr += 4;
	if(SYCW & 0x100) {
		MemWrite32(adr, L0SP);
		adr += 4;
	}
	if(SYCW & 0x200) {
		MemWrite32(adr, L1SP);
		adr += 4;
	}
	if(SYCW & 0x400) {
		MemWrite32(adr, L2SP);
		adr += 4;
	}
	if(SYCW & 0x800) {
		MemWrite32(adr, L3SP);
		adr += 4;
	}

	// 31 registers supported, _not_ 32
	for(i=0; i<31; i++)
		if(amOut & (1<<i)) {
			MemWrite32(adr, v60.reg[i]);
			adr += 4;
		}

	// #### Ignore the virtual addressing crap.

	return amLength1 + 1;
}

uint32_t opGETPSW(void)
{
	UPDATEPSW();

	modAdd=PC + 1;
	modDim=2;
	modWriteValW=PSW;

	// Write PSW to the operand
	amLength1=WriteAM();

	return amLength1 + 1;
}

uint32_t opTASI(void)
{
	uint8_t appb;
	modAdd=PC + 1;
	modDim=0;

	// Load the address of the operand
	amLength1=ReadAMAddress();

	// Load uint8_t from the address
	if (amFlag)
		appb=(uint8_t)v60.reg[amOut&0x1F];
	else
		appb=MemRead8(amOut);

	// Set the flags for SUB appb,FF
	SUBB(appb, 0xff);

	// Write FF in the operand
	if (amFlag)
		SETREG8(v60.reg[amOut&0x1F], 0xFF);
	else
		MemWrite8(amOut,0xFF);

	return amLength1 + 1;
}

uint32_t opCLRTLB(void)
{
	modAdd=PC+1;
	modDim=2;

	// Read the operand
	amLength1=ReadAM();

	// @@@ TLB not yet emulated

	return amLength1 + 1;
}

uint32_t opPOPM(void)
{
	int i;

	modAdd=PC+1;
	modDim=2;

	// Read the bit register list
	amLength1=ReadAM();

	for (i=0;i<31;i++)
		if (amOut & (1<<i))
		{
			v60.reg[i] = MemRead32(SP);
			SP += 4;
		}

	if (amOut & (1<<31))
	{
		PSW = (PSW & 0xFFFF0000) | MemRead16(SP);
		SP += 4;
		UPDATECPUFLAGS();
	}

	return amLength1 + 1;
}

uint32_t opPUSHM(void)
{
	int i;

	modAdd=PC+1;
	modDim=2;

	// Read the bit register list
	amLength1=ReadAM();

	if (amOut & (1<<31))
	{
		UPDATEPSW();
		SP -= 4;
		MemWrite32(SP,PSW);
	}

	for (i=0;i<31;i++)
		if (amOut & (1<<(30-i)))
		{
			SP -= 4;
			MemWrite32(SP,v60.reg[(30-i)]);
		}


	return amLength1 + 1;
}

uint32_t opTESTB(void) /* TRUSTED */
{
	modAdd=PC+1;
	modDim=0;

	// Read the operand
	amLength1=ReadAM();

	_Z = (amOut == 0);
	_S = ((amOut & 0x80) != 0);
	_CY = 0;
	_OV = 0;

	return amLength1 + 1;
}

uint32_t opTESTH(void) /* TRUSTED */
{
	modAdd=PC+1;
	modDim=1;

	// Read the operand
	amLength1=ReadAM();

	_Z = (amOut == 0);
	_S = ((amOut & 0x8000) != 0);
	_CY = 0;
	_OV = 0;

	return amLength1 + 1;
}

uint32_t opTESTW(void) /* TRUSTED */
{
	modAdd=PC+1;
	modDim=2;

	// Read the operand
	amLength1=ReadAM();

	_Z = (amOut == 0);
	_S = ((amOut & 0x80000000) != 0);
	_CY = 0;
	_OV = 0;

	return amLength1 + 1;
}

uint32_t opPUSH(void)
{
	modAdd=PC+1;
	modDim=2;

	amLength1=ReadAM();

	SP-=4;
	MemWrite32(SP,amOut);

	return amLength1 + 1;
}

uint32_t opPOP(void)
{
	modAdd=PC+1;
	modDim=2;
	modWriteValW=MemRead32(SP);
	SP+=4;
	amLength1=WriteAM();

	return amLength1 + 1;
}


uint32_t opINCB_0(void) { modM=0; return opINCB(); }
uint32_t opINCB_1(void) { modM=1; return opINCB(); }
uint32_t opINCH_0(void) { modM=0; return opINCH(); }
uint32_t opINCH_1(void) { modM=1; return opINCH(); }
uint32_t opINCW_0(void) { modM=0; return opINCW(); }
uint32_t opINCW_1(void) { modM=1; return opINCW(); }

uint32_t opDECB_0(void) { modM=0; return opDECB(); }
uint32_t opDECB_1(void) { modM=1; return opDECB(); }
uint32_t opDECH_0(void) { modM=0; return opDECH(); }
uint32_t opDECH_1(void) { modM=1; return opDECH(); }
uint32_t opDECW_0(void) { modM=0; return opDECW(); }
uint32_t opDECW_1(void) { modM=1; return opDECW(); }

uint32_t opJMP_0(void) { modM=0; return opJMP(); }
uint32_t opJMP_1(void) { modM=1; return opJMP(); }

uint32_t opJSR_0(void) { modM=0; return opJSR(); }
uint32_t opJSR_1(void) { modM=1; return opJSR(); }

uint32_t opPREPARE_0(void) { modM=0; return opPREPARE(); }
uint32_t opPREPARE_1(void) { modM=1; return opPREPARE(); }

uint32_t opRET_0(void) { modM=0; return opRET(); }
uint32_t opRET_1(void) { modM=1; return opRET(); }

uint32_t opTRAP_0(void) { modM=0; return opTRAP(); }
uint32_t opTRAP_1(void) { modM=1; return opTRAP(); }

uint32_t opRETIU_0(void) { modM=0; return opRETIU(); }
uint32_t opRETIU_1(void) { modM=1; return opRETIU(); }

uint32_t opRETIS_0(void) { modM=0; return opRETIS(); }
uint32_t opRETIS_1(void) { modM=1; return opRETIS(); }

uint32_t opGETPSW_0(void) { modM=0; return opGETPSW(); }
uint32_t opGETPSW_1(void) { modM=1; return opGETPSW(); }

uint32_t opTASI_0(void) { modM=0; return opTASI(); }
uint32_t opTASI_1(void) { modM=1; return opTASI(); }

uint32_t opCLRTLB_0(void) { modM=0; return opCLRTLB(); }
uint32_t opCLRTLB_1(void) { modM=1; return opCLRTLB(); }

uint32_t opPOPM_0(void) { modM=0; return opPOPM(); }
uint32_t opPOPM_1(void) { modM=1; return opPOPM(); }

uint32_t opPUSHM_0(void) { modM=0; return opPUSHM(); }
uint32_t opPUSHM_1(void) { modM=1; return opPUSHM(); }

uint32_t opTESTB_0(void) { modM=0; return opTESTB(); }
uint32_t opTESTB_1(void) { modM=1; return opTESTB(); }

uint32_t opTESTH_0(void) { modM=0; return opTESTH(); }
uint32_t opTESTH_1(void) { modM=1; return opTESTH(); }

uint32_t opTESTW_0(void) { modM=0; return opTESTW(); }
uint32_t opTESTW_1(void) { modM=1; return opTESTW(); }

uint32_t opPUSH_0(void) { modM=0; return opPUSH(); }
uint32_t opPUSH_1(void) { modM=1; return opPUSH(); }

uint32_t opPOP_0(void) { modM=0; return opPOP(); }
uint32_t opPOP_1(void) { modM=1; return opPOP(); }

uint32_t opSTTASK_0(void) { modM=0; return opSTTASK(); }
uint32_t opSTTASK_1(void) { modM=1; return opSTTASK(); }
