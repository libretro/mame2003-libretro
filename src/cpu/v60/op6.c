
/*
	FULLY TRUSTED
*/

uint32_t opTB(int reg) /* TRUSTED */
{
	if (v60.reg[reg] == 0)
	{
		PC += (int16_t)OpRead16(PC + 2);
		return 0;
	}

	return 4;
}

uint32_t opDBGT(int reg) /* TRUSTED */
{
	v60.reg[reg]--;

	NORMALIZEFLAGS();
	if ((v60.reg[reg] != 0) && !((_S ^ _OV) | _Z))
	{
		PC += (int16_t)OpRead16(PC + 2);
		return 0;
	}

	return 4;
}

uint32_t opDBLE(int reg) /* TRUSTED */
{
	v60.reg[reg]--;

	NORMALIZEFLAGS();
	if ((v60.reg[reg] != 0) && ((_S ^ _OV) | _Z))
	{
		PC += (int16_t)OpRead16(PC + 2);
		return 0;
	}

	return 4;
}


uint32_t opDBGE(int reg) /* TRUSTED */
{
	v60.reg[reg]--;

	NORMALIZEFLAGS();
	if ((v60.reg[reg] != 0) && !(_S ^ _OV))
	{
		PC += (int16_t)OpRead16(PC + 2);
		return 0;
	}

	return 4;
}

uint32_t opDBLT(int reg) /* TRUSTED */
{
	v60.reg[reg]--;

	NORMALIZEFLAGS();
	if ((v60.reg[reg] != 0) && (_S ^ _OV))
	{
		PC += (int16_t)OpRead16(PC + 2);
		return 0;
	}

	return 4;
}

uint32_t opDBH(int reg) /* TRUSTED */
{
	v60.reg[reg]--;

	if ((v60.reg[reg] != 0) && !(_CY | _Z))
	{
		PC += (int16_t)OpRead16(PC + 2);
		return 0;
	}

	return 4;
}

uint32_t opDBNH(int reg) /* TRUSTED */
{
	v60.reg[reg]--;

	if ((v60.reg[reg] != 0) && (_CY | _Z))
	{
		PC += (int16_t)OpRead16(PC + 2);
		return 0;
	}

	return 4;
}


uint32_t opDBL(int reg) /* TRUSTED */
{
	v60.reg[reg]--;

	if ((v60.reg[reg] != 0) && (_CY))
	{
		PC += (int16_t)OpRead16(PC + 2);
		return 0;
	}

	return 4;
}

uint32_t opDBNL(int reg) /* TRUSTED */
{
	v60.reg[reg]--;

	if ((v60.reg[reg] != 0) && !(_CY))
	{
		PC += (int16_t)OpRead16(PC + 2);
		return 0;
	}

	return 4;
}

uint32_t opDBE(int reg) /* TRUSTED */
{
	v60.reg[reg]--;

	if ((v60.reg[reg] != 0) && (_Z))
	{
		PC += (int16_t)OpRead16(PC + 2);
		return 0;
	}

	return 4;
}

uint32_t opDBNE(int reg) /* TRUSTED */
{
	v60.reg[reg]--;

	if ((v60.reg[reg] != 0) && !(_Z))
	{
		PC += (int16_t)OpRead16(PC + 2);
		return 0;
	}

	return 4;
}

uint32_t opDBV(int reg) /* TRUSTED */
{
	v60.reg[reg]--;

	if ((v60.reg[reg] != 0) && (_OV))
	{
		PC += (int16_t)OpRead16(PC + 2);
		return 0;
	}

	return 4;
}

uint32_t opDBNV(int reg) /* TRUSTED */
{
	v60.reg[reg]--;

	if ((v60.reg[reg] != 0) && !(_OV))
	{
		PC += (int16_t)OpRead16(PC + 2);
		return 0;
	}

	return 4;
}

uint32_t opDBN(int reg) /* TRUSTED */
{
	v60.reg[reg]--;

	if ((v60.reg[reg] != 0) && (_S))
	{
		PC += (int16_t)OpRead16(PC + 2);
		return 0;
	}

	return 4;
}

uint32_t opDBP(int reg) /* TRUSTED */
{
	v60.reg[reg]--;

	if ((v60.reg[reg] != 0) && !(_S))
	{
		PC += (int16_t)OpRead16(PC + 2);
		return 0;
	}

	return 4;
}

uint32_t opDBR(int reg) /* TRUSTED */
{
	v60.reg[reg]--;

	if (v60.reg[reg] != 0)
	{
		PC += (int16_t)OpRead16(PC + 2);
		return 0;
	}

	return 4;
}

uint32_t (*OpC6Table[8])(int reg) = /* TRUSTED */
{
	opDBV,
	opDBL,
	opDBE,
	opDBNH,
	opDBN,
	opDBR,
	opDBLT,
	opDBLE
};

uint32_t (*OpC7Table[8])(int reg) = /* TRUSTED */
{
	opDBNV,
	opDBNL,
	opDBNE,
	opDBH,
	opDBP,
	opTB,
	opDBGE,
	opDBGT
};


uint32_t opC6(void) /* TRUSTED */
{
	uint8_t appb=OpRead8(PC + 1);
	return OpC6Table[appb>>5](appb&0x1f);
}

uint32_t opC7(void) /* TRUSTED */
{
	uint8_t appb=OpRead8(PC + 1);
	return OpC7Table[appb>>5](appb&0x1f);
}

