
// AM3 Functions (for ReadAM)
// **************************

uint32_t am3Register(void)
{
	switch (modDim)
	{
	case 0:
		SETREG8(v60.reg[modVal&0x1F], modWriteValB);
		break;
	case 1:
		SETREG16(v60.reg[modVal&0x1F], modWriteValH);
		break;
	case 2:
		v60.reg[modVal&0x1F] = modWriteValW;
		break;
	}

	return 1;
}

uint32_t am3RegisterIndirect(void)
{
	switch (modDim)
	{
	case 0:
		MemWrite8(v60.reg[modVal&0x1F], modWriteValB);
		break;
	case 1:
		MemWrite16(v60.reg[modVal&0x1F], modWriteValH);
		break;
	case 2:
		MemWrite32(v60.reg[modVal&0x1F], modWriteValW);
		break;
	}

	return 1;
}

uint32_t am3RegisterIndirectIndexed(void)
{
	switch (modDim)
	{
	case 0:
		MemWrite8(v60.reg[modVal2&0x1F] + v60.reg[modVal&0x1F], modWriteValB);
		break;
	case 1:
		MemWrite16(v60.reg[modVal2&0x1F] + v60.reg[modVal&0x1F] * 2, modWriteValH);
		break;
	case 2:
		MemWrite32(v60.reg[modVal2&0x1F] + v60.reg[modVal&0x1F] * 4, modWriteValW);
		break;
	}

	return 2;
}

uint32_t am3Autoincrement(void)
{
	switch (modDim)
	{
	case 0:
		MemWrite8(v60.reg[modVal&0x1F], modWriteValB);
		v60.reg[modVal&0x1F] += 1;
		break;
	case 1:
		MemWrite16(v60.reg[modVal&0x1F], modWriteValH);
		v60.reg[modVal&0x1F] += 2;
		break;
	case 2:
		MemWrite32(v60.reg[modVal&0x1F], modWriteValW);
		v60.reg[modVal&0x1F] += 4;
		break;
	}

	return 1;
}

uint32_t am3Autodecrement(void)
{
	switch (modDim)
	{
	case 0:
		v60.reg[modVal&0x1F] -= 1;
		MemWrite8(v60.reg[modVal&0x1F], modWriteValB);
		break;
	case 1:
		v60.reg[modVal&0x1F] -= 2;
		MemWrite16(v60.reg[modVal&0x1F], modWriteValH);
		break;
	case 2:
		v60.reg[modVal&0x1F] -= 4;
		MemWrite32(v60.reg[modVal&0x1F], modWriteValW);
		break;
	}

	return 1;
}

uint32_t am3Displacement8(void)
{
	switch (modDim)
	{
	case 0:
		MemWrite8(v60.reg[modVal&0x1F] + (int8_t)OpRead8(modAdd+1), modWriteValB);
		break;
	case 1:
		MemWrite16(v60.reg[modVal&0x1F] + (int8_t)OpRead8(modAdd+1), modWriteValH);
		break;
	case 2:
		MemWrite32(v60.reg[modVal&0x1F] + (int8_t)OpRead8(modAdd+1), modWriteValW);
		break;
	}

	return 2;
}

uint32_t am3Displacement16(void)
{
	switch (modDim)
	{
	case 0:
		MemWrite8(v60.reg[modVal&0x1F] + (int16_t)OpRead16(modAdd+1), modWriteValB);
		break;
	case 1:
		MemWrite16(v60.reg[modVal&0x1F] + (int16_t)OpRead16(modAdd+1), modWriteValH);
		break;
	case 2:
		MemWrite32(v60.reg[modVal&0x1F] + (int16_t)OpRead16(modAdd+1), modWriteValW);
		break;
	}

	return 3;
}

uint32_t am3Displacement32(void)
{
	switch (modDim)
	{
	case 0:
		MemWrite8(v60.reg[modVal&0x1F] + OpRead32(modAdd+1), modWriteValB);
		break;
	case 1:
		MemWrite16(v60.reg[modVal&0x1F] + OpRead32(modAdd+1), modWriteValH);
		break;
	case 2:
		MemWrite32(v60.reg[modVal&0x1F] + OpRead32(modAdd+1), modWriteValW);
		break;
	}

	return 5;
}


uint32_t am3DisplacementIndexed8(void)
{
	switch (modDim)
	{
	case 0:
		MemWrite8(v60.reg[modVal2&0x1F] + (int8_t)OpRead8(modAdd+2) + v60.reg[modVal&0x1F], modWriteValB);
		break;
	case 1:
		MemWrite16(v60.reg[modVal2&0x1F] + (int8_t)OpRead8(modAdd+2) + v60.reg[modVal&0x1F] * 2, modWriteValH);
		break;
	case 2:
		MemWrite32(v60.reg[modVal2&0x1F] + (int8_t)OpRead8(modAdd+2) + v60.reg[modVal&0x1F] * 4, modWriteValW);
		break;
	}

	return 3;
}

uint32_t am3DisplacementIndexed16(void)
{
	switch (modDim)
	{
	case 0:
		MemWrite8(v60.reg[modVal2&0x1F] + (int16_t)OpRead16(modAdd+2) + v60.reg[modVal&0x1F], modWriteValB);
		break;
	case 1:
		MemWrite16(v60.reg[modVal2&0x1F] + (int16_t)OpRead16(modAdd+2) + v60.reg[modVal&0x1F] * 2, modWriteValH);
		break;
	case 2:
		MemWrite32(v60.reg[modVal2&0x1F] + (int16_t)OpRead16(modAdd+2) + v60.reg[modVal&0x1F] * 4, modWriteValW);
		break;
	}

	return 4;
}

uint32_t am3DisplacementIndexed32(void)
{
	switch (modDim)
	{
	case 0:
		MemWrite8(v60.reg[modVal2&0x1F] + OpRead32(modAdd+2) + v60.reg[modVal&0x1F], modWriteValB);
		break;
	case 1:
		MemWrite16(v60.reg[modVal2&0x1F] + OpRead32(modAdd+2) + v60.reg[modVal&0x1F] * 2, modWriteValH);
		break;
	case 2:
		MemWrite32(v60.reg[modVal2&0x1F] + OpRead32(modAdd+2) + v60.reg[modVal&0x1F] * 4, modWriteValW);
		break;
	}

	return 6;
}


uint32_t am3PCDisplacement8(void)
{
	switch (modDim)
	{
	case 0:
		MemWrite8(PC + (int8_t)OpRead8(modAdd+1), modWriteValB);
		break;
	case 1:
		MemWrite16(PC + (int8_t)OpRead8(modAdd+1), modWriteValH);
		break;
	case 2:
		MemWrite32(PC + (int8_t)OpRead8(modAdd+1), modWriteValW);
		break;
	}

	return 2;
}

uint32_t am3PCDisplacement16(void)
{
	switch (modDim)
	{
	case 0:
		MemWrite8(PC + (int16_t)OpRead16(modAdd+1), modWriteValB);
		break;
	case 1:
		MemWrite16(PC + (int16_t)OpRead16(modAdd+1), modWriteValH);
		break;
	case 2:
		MemWrite32(PC + (int16_t)OpRead16(modAdd+1), modWriteValW);
		break;
	}

	return 3;
}

uint32_t am3PCDisplacement32(void)
{
	switch (modDim)
	{
	case 0:
		MemWrite8(PC + OpRead32(modAdd+1), modWriteValB);
		break;
	case 1:
		MemWrite16(PC + OpRead32(modAdd+1), modWriteValH);
		break;
	case 2:
		MemWrite32(PC + OpRead32(modAdd+1), modWriteValW);
		break;
	}

	return 5;
}

uint32_t am3PCDisplacementIndexed8(void)
{
	switch (modDim)
	{
	case 0:
		MemWrite8(PC + (int8_t)OpRead8(modAdd+2) + v60.reg[modVal&0x1F], modWriteValB);
		break;
	case 1:
		MemWrite16(PC + (int8_t)OpRead8(modAdd+2) + v60.reg[modVal&0x1F] * 2, modWriteValH);
		break;
	case 2:
		MemWrite32(PC + (int8_t)OpRead8(modAdd+2) + v60.reg[modVal&0x1F] * 4, modWriteValW);
		break;
	}

	return 3;
}

uint32_t am3PCDisplacementIndexed16(void)
{
	switch (modDim)
	{
	case 0:
		MemWrite8(PC + (int16_t)OpRead16(modAdd+2) + v60.reg[modVal&0x1F], modWriteValB);
		break;
	case 1:
		MemWrite16(PC + (int16_t)OpRead16(modAdd+2) + v60.reg[modVal&0x1F] * 2, modWriteValH);
		break;
	case 2:
		MemWrite32(PC + (int16_t)OpRead16(modAdd+2) + v60.reg[modVal&0x1F] * 4, modWriteValW);
		break;
	}

	return 4;
}

uint32_t am3PCDisplacementIndexed32(void)
{
	switch (modDim)
	{
	case 0:
		MemWrite8(PC + OpRead32(modAdd+2) + v60.reg[modVal&0x1F], modWriteValB);
		break;
	case 1:
		MemWrite16(PC + OpRead32(modAdd+2) + v60.reg[modVal&0x1F] * 2, modWriteValH);
		break;
	case 2:
		MemWrite32(PC + OpRead32(modAdd+2) + v60.reg[modVal&0x1F] * 4, modWriteValW);
		break;
	}

	return 6;
}

uint32_t am3DisplacementIndirect8(void)
{
	switch (modDim)
	{
	case 0:
		MemWrite8(MemRead32(v60.reg[modVal&0x1F] + (int8_t)OpRead8(modAdd+1)), modWriteValB);
		break;
	case 1:
		MemWrite16(MemRead32(v60.reg[modVal&0x1F] + (int8_t)OpRead8(modAdd+1)), modWriteValH);
		break;
	case 2:
		MemWrite32(MemRead32(v60.reg[modVal&0x1F] + (int8_t)OpRead8(modAdd+1)), modWriteValW);
		break;
	}

	return 2;
}

uint32_t am3DisplacementIndirect16(void)
{
	switch (modDim)
	{
	case 0:
		MemWrite8(MemRead32(v60.reg[modVal&0x1F] + (int16_t)OpRead16(modAdd+1)), modWriteValB);
		break;
	case 1:
		MemWrite16(MemRead32(v60.reg[modVal&0x1F] + (int16_t)OpRead16(modAdd+1)), modWriteValH);
		break;
	case 2:
		MemWrite32(MemRead32(v60.reg[modVal&0x1F] + (int16_t)OpRead16(modAdd+1)), modWriteValW);
		break;
	}

	return 3;
}

uint32_t am3DisplacementIndirect32(void)
{
	switch (modDim)
	{
	case 0:
		MemWrite8(MemRead32(v60.reg[modVal&0x1F] + OpRead32(modAdd+1)), modWriteValB);
		break;
	case 1:
		MemWrite16(MemRead32(v60.reg[modVal&0x1F] + OpRead32(modAdd+1)), modWriteValH);
		break;
	case 2:
		MemWrite32(MemRead32(v60.reg[modVal&0x1F] + OpRead32(modAdd+1)), modWriteValW);
		break;
	}

	return 5;
}


uint32_t am3DisplacementIndirectIndexed8(void)
{
	switch (modDim)
	{
	case 0:
		MemWrite8(MemRead32(v60.reg[modVal2&0x1F] + (int8_t)OpRead8(modAdd+2)) + v60.reg[modVal&0x1F], modWriteValB);
		break;
	case 1:
		MemWrite16(MemRead32(v60.reg[modVal2&0x1F] + (int8_t)OpRead8(modAdd+2)) + v60.reg[modVal&0x1F] * 2, modWriteValH);
		break;
	case 2:
		MemWrite32(MemRead32(v60.reg[modVal2&0x1F] + (int8_t)OpRead8(modAdd+2)) + v60.reg[modVal&0x1F] * 4, modWriteValW);
		break;
	}

	return 3;
}

uint32_t am3DisplacementIndirectIndexed16(void)
{
	switch (modDim)
	{
	case 0:
		MemWrite8(MemRead32(v60.reg[modVal2&0x1F] + (int16_t)OpRead16(modAdd+2)) + v60.reg[modVal&0x1F], modWriteValB);
		break;
	case 1:
		MemWrite16(MemRead32(v60.reg[modVal2&0x1F] + (int16_t)OpRead16(modAdd+2)) + v60.reg[modVal&0x1F] * 2, modWriteValH);
		break;
	case 2:
		MemWrite32(MemRead32(v60.reg[modVal2&0x1F] + (int16_t)OpRead16(modAdd+2)) + v60.reg[modVal&0x1F] * 4, modWriteValW);
		break;
	}

	return 4;
}

uint32_t am3DisplacementIndirectIndexed32(void)
{
	switch (modDim)
	{
	case 0:
		MemWrite8(MemRead32(v60.reg[modVal2&0x1F] + OpRead32(modAdd+2)) + v60.reg[modVal&0x1F], modWriteValB);
		break;
	case 1:
		MemWrite16(MemRead32(v60.reg[modVal2&0x1F] + OpRead32(modAdd+2)) + v60.reg[modVal&0x1F] * 2, modWriteValH);
		break;
	case 2:
		MemWrite32(MemRead32(v60.reg[modVal2&0x1F] + OpRead32(modAdd+2)) + v60.reg[modVal&0x1F] * 4, modWriteValW);
		break;
	}

	return 6;
}

uint32_t am3PCDisplacementIndirect8(void)
{
	switch (modDim)
	{
	case 0:
		MemWrite8(MemRead32(PC + (int8_t)OpRead8(modAdd+1)), modWriteValB);
		break;
	case 1:
		MemWrite16(MemRead32(PC + (int8_t)OpRead8(modAdd+1)), modWriteValH);
		break;
	case 2:
		MemWrite32(MemRead32(PC + (int8_t)OpRead8(modAdd+1)), modWriteValW);
		break;
	}

	return 2;
}

uint32_t am3PCDisplacementIndirect16(void)
{
	switch (modDim)
	{
	case 0:
		MemWrite8(MemRead32(PC + (int16_t)OpRead16(modAdd+1)), modWriteValB);
		break;
	case 1:
		MemWrite16(MemRead32(PC + (int16_t)OpRead16(modAdd+1)), modWriteValH);
		break;
	case 2:
		MemWrite32(MemRead32(PC + (int16_t)OpRead16(modAdd+1)), modWriteValW);
		break;
	}

	return 3;
}

uint32_t am3PCDisplacementIndirect32(void)
{
	switch (modDim)
	{
	case 0:
		MemWrite8(MemRead32(PC + OpRead32(modAdd+1)), modWriteValB);
		break;
	case 1:
		MemWrite16(MemRead32(PC + OpRead32(modAdd+1)), modWriteValH);
		break;
	case 2:
		MemWrite32(MemRead32(PC + OpRead32(modAdd+1)), modWriteValW);
		break;
	}

	return 5;
}


uint32_t am3PCDisplacementIndirectIndexed8(void)
{
	switch (modDim)
	{
	case 0:
		MemWrite8(MemRead32(PC + (int8_t)OpRead8(modAdd+2)) + v60.reg[modVal&0x1F], modWriteValB);
		break;
	case 1:
		MemWrite16(MemRead32(PC + (int8_t)OpRead8(modAdd+2)) + v60.reg[modVal&0x1F] * 2, modWriteValH);
		break;
	case 2:
		MemWrite32(MemRead32(PC + (int8_t)OpRead8(modAdd+2)) + v60.reg[modVal&0x1F] * 4, modWriteValW);
		break;
	}

	return 3;
}

uint32_t am3PCDisplacementIndirectIndexed16(void)
{
	switch (modDim)
	{
	case 0:
		MemWrite8(MemRead32(PC + (int16_t)OpRead16(modAdd+2)) + v60.reg[modVal&0x1F], modWriteValB);
		break;
	case 1:
		MemWrite16(MemRead32(PC + (int16_t)OpRead16(modAdd+2)) + v60.reg[modVal&0x1F] * 2, modWriteValH);
		break;
	case 2:
		MemWrite32(MemRead32(PC + (int16_t)OpRead16(modAdd+2)) + v60.reg[modVal&0x1F] * 4, modWriteValW);
		break;
	}

	return 4;
}

uint32_t am3PCDisplacementIndirectIndexed32(void)
{
	switch (modDim)
	{
	case 0:
		MemWrite8(MemRead32(PC + OpRead32(modAdd+2)) + v60.reg[modVal&0x1F], modWriteValB);
		break;
	case 1:
		MemWrite16(MemRead32(PC + OpRead32(modAdd+2)) + v60.reg[modVal&0x1F] * 2, modWriteValH);
		break;
	case 2:
		MemWrite32(MemRead32(PC + OpRead32(modAdd+2)) + v60.reg[modVal&0x1F] * 4, modWriteValW);
		break;
	}

	return 6;
}


uint32_t am3DoubleDisplacement8(void)
{
	switch (modDim)
	{
	case 0:
		MemWrite8(MemRead32(v60.reg[modVal&0x1F] + (int8_t)OpRead8(modAdd+1)) + (int8_t)OpRead8(modAdd+2), modWriteValB);
		break;
	case 1:
		MemWrite16(MemRead32(v60.reg[modVal&0x1F] + (int8_t)OpRead8(modAdd+1)) + (int8_t)OpRead8(modAdd+2), modWriteValH);
		break;
	case 2:
		MemWrite32(MemRead32(v60.reg[modVal&0x1F] + (int8_t)OpRead8(modAdd+1)) + (int8_t)OpRead8(modAdd+2), modWriteValW);
		break;
	}

	return 3;
}

uint32_t am3DoubleDisplacement16(void)
{
	switch (modDim)
	{
	case 0:
		MemWrite8(MemRead32(v60.reg[modVal&0x1F] + (int16_t)OpRead16(modAdd+1)) + (int16_t)OpRead16(modAdd+3), modWriteValB);
		break;
	case 1:
		MemWrite16(MemRead32(v60.reg[modVal&0x1F] + (int16_t)OpRead16(modAdd+1)) + (int16_t)OpRead16(modAdd+3), modWriteValH);
		break;
	case 2:
		MemWrite32(MemRead32(v60.reg[modVal&0x1F] + (int16_t)OpRead16(modAdd+1)) + (int16_t)OpRead16(modAdd+3), modWriteValW);
		break;
	}

	return 5;
}

uint32_t am3DoubleDisplacement32(void)
{
	switch (modDim)
	{
	case 0:
		MemWrite8(MemRead32(v60.reg[modVal&0x1F] + OpRead32(modAdd+1)) + OpRead32(modAdd+5), modWriteValB);
		break;
	case 1:
		MemWrite16(MemRead32(v60.reg[modVal&0x1F] + OpRead32(modAdd+1)) + OpRead32(modAdd+5), modWriteValH);
		break;
	case 2:
		MemWrite32(MemRead32(v60.reg[modVal&0x1F] + OpRead32(modAdd+1)) + OpRead32(modAdd+5), modWriteValW);
		break;
	}

	return 9;
}


uint32_t am3PCDoubleDisplacement8(void)
{
	switch (modDim)
	{
	case 0:
		MemWrite8(MemRead32(PC + (int8_t)OpRead8(modAdd+1)) + (int8_t)OpRead8(modAdd+2), modWriteValB);
		break;
	case 1:
		MemWrite16(MemRead32(PC + (int8_t)OpRead8(modAdd+1)) + (int8_t)OpRead8(modAdd+2), modWriteValH);
		break;
	case 2:
		MemWrite32(MemRead32(PC + (int8_t)OpRead8(modAdd+1)) + (int8_t)OpRead8(modAdd+2), modWriteValW);
		break;
	}

	return 3;
}

uint32_t am3PCDoubleDisplacement16(void)
{
	switch (modDim)
	{
	case 0:
		MemWrite8(MemRead32(PC + (int16_t)OpRead16(modAdd+1)) + (int16_t)OpRead16(modAdd+3), modWriteValB);
		break;
	case 1:
		MemWrite16(MemRead32(PC + (int16_t)OpRead16(modAdd+1)) + (int16_t)OpRead16(modAdd+3), modWriteValH);
		break;
	case 2:
		MemWrite32(MemRead32(PC + (int16_t)OpRead16(modAdd+1)) + (int16_t)OpRead16(modAdd+3), modWriteValW);
		break;
	}

	return 5;
}

uint32_t am3PCDoubleDisplacement32(void)
{
	switch (modDim)
	{
	case 0:
		MemWrite8(MemRead32(PC + OpRead32(modAdd+1)) + OpRead32(modAdd+5), modWriteValB);
		break;
	case 1:
		MemWrite16(MemRead32(PC + OpRead32(modAdd+1)) + OpRead32(modAdd+5), modWriteValH);
		break;
	case 2:
		MemWrite32(MemRead32(PC + OpRead32(modAdd+1)) + OpRead32(modAdd+5), modWriteValW);
		break;
	}

	return 9;
}

uint32_t am3DirectAddress(void)
{
	switch (modDim)
	{
	case 0:
		MemWrite8(OpRead32(modAdd+1), modWriteValB);
		break;
	case 1:
		MemWrite16(OpRead32(modAdd+1), modWriteValH);
		break;
	case 2:
		MemWrite32(OpRead32(modAdd+1), modWriteValW);
		break;
	}

	return 5;
}

uint32_t am3DirectAddressIndexed(void)
{
	switch (modDim)
	{
	case 0:
		MemWrite8(OpRead32(modAdd+2) + v60.reg[modVal&0x1F], modWriteValB);
		break;
	case 1:
		MemWrite16(OpRead32(modAdd+2) + v60.reg[modVal&0x1F] * 2, modWriteValH);
		break;
	case 2:
		MemWrite32(OpRead32(modAdd+2) + v60.reg[modVal&0x1F] * 4, modWriteValW);
		break;
	}

	return 6;
}

uint32_t am3DirectAddressDeferred(void)
{
	switch (modDim)
	{
	case 0:
		MemWrite8(MemRead32(OpRead32(modAdd+1)), modWriteValB);
		break;
	case 1:
		MemWrite16(MemRead32(OpRead32(modAdd+1)), modWriteValH);
		break;
	case 2:
		MemWrite32(MemRead32(OpRead32(modAdd+1)), modWriteValW);
		break;
	}

	return 5;
}

uint32_t am3DirectAddressDeferredIndexed(void)
{
	switch (modDim)
	{
	case 0:
		MemWrite8(MemRead32(OpRead32(modAdd+2)) + v60.reg[modVal&0x1F], modWriteValB);
		break;
	case 1:
		MemWrite16(MemRead32(OpRead32(modAdd+2)) + v60.reg[modVal&0x1F], modWriteValH);
		break;
	case 2:
		MemWrite32(MemRead32(OpRead32(modAdd+2)) + v60.reg[modVal&0x1F], modWriteValW);
		break;
	}

	return 6;
}

uint32_t am3Immediate(void)
{
	logerror("CPU - AM3 - IMM (PC=%06x)\n", PC);
	abort();
}

uint32_t am3ImmediateQuick(void)
{
	logerror("CPU - AM3 - IMMQ (PC=%06x)\n", PC);
	abort();
}



// AM3 Tables (for ReadAMAddress)
// ******************************

uint32_t am3Error1(void)
{
	logerror("CPU - AM3 - 1 (PC=%06x)\n", PC);
	abort();
}

uint32_t am3Error2(void)
{
	logerror("CPU - AM3 - 2 (PC=%06x)\n", PC);
	abort();
}

uint32_t am3Error3(void)
{
	logerror("CPU - AM3 - 3 (PC=%06x)\n", PC);
	abort();
}

uint32_t am3Error4(void)
{
	logerror("CPU - AM3 - 4 (PC=%06x)\n", PC);
	abort();
}

uint32_t am3Error5(void)
{
	logerror("CPU - AM3 - 5 (PC=%06x)\n", PC);
	abort();
}

uint32_t (*AMTable3_G7a[16])(void) =
{
	am3PCDisplacementIndexed8,
	am3PCDisplacementIndexed16,
	am3PCDisplacementIndexed32,
	am3DirectAddressIndexed,
	am3Error5,
	am3Error5,
	am3Error5,
	am3Error5,
	am3PCDisplacementIndirectIndexed8,
	am3PCDisplacementIndirectIndexed16,
	am3PCDisplacementIndirectIndexed32,
	am3DirectAddressDeferredIndexed,
	am3Error5,
	am3Error5,
	am3Error5,
	am3Error5
};

uint32_t am3Group7a(void)
{
	if (!(modVal2&0x10))
		return am3Error4();

	return AMTable3_G7a[modVal2&0xF]();
}

uint32_t (*AMTable3_G7[32])(void) =
{
	am3ImmediateQuick,
	am3ImmediateQuick,
	am3ImmediateQuick,
	am3ImmediateQuick,
	am3ImmediateQuick,
	am3ImmediateQuick,
	am3ImmediateQuick,
	am3ImmediateQuick,
	am3ImmediateQuick,
	am3ImmediateQuick,
	am3ImmediateQuick,
	am3ImmediateQuick,
	am3ImmediateQuick,
	am3ImmediateQuick,
	am3ImmediateQuick,
	am3ImmediateQuick,
	am3PCDisplacement8,
	am3PCDisplacement16,
	am3PCDisplacement32,
  am3DirectAddress,
	am3Immediate,
	am3Error2,
	am3Error2,
	am3Error2,
  am3PCDisplacementIndirect8,
  am3PCDisplacementIndirect16,
  am3PCDisplacementIndirect32,
	am3DirectAddressDeferred,
	am3PCDoubleDisplacement8,
	am3PCDoubleDisplacement16,
	am3PCDoubleDisplacement32,
  am3Error2
};

uint32_t (*AMTable3_G6[8])(void) =
{
	am3DisplacementIndexed8,
	am3DisplacementIndexed16,
	am3DisplacementIndexed32,
	am3RegisterIndirectIndexed,
	am3DisplacementIndirectIndexed8,
	am3DisplacementIndirectIndexed16,
	am3DisplacementIndirectIndexed32,
	am3Group7a
};




uint32_t am3Group6(void)
{
	modVal2=OpRead8(modAdd+1);
	return AMTable3_G6[modVal2>>5]();
}


uint32_t am3Group7(void)
{
	return AMTable3_G7[modVal&0x1F]();
}



uint32_t (*AMTable3[2][8])(void) =
{
	{
		am3Displacement8,
		am3Displacement16,
		am3Displacement32,
		am3RegisterIndirect,
		am3DisplacementIndirect8,
		am3DisplacementIndirect16,
		am3DisplacementIndirect32,
		am3Group7
	},

	{
		am3DoubleDisplacement8,
		am3DoubleDisplacement16,
		am3DoubleDisplacement32,
		am3Register,
		am3Autoincrement,
		am3Autodecrement,
		am3Group6,
		am3Error1
	}
};



