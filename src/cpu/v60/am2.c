
// AM2 Functions (for ReadAMAddress)
// *********************************

uint32_t am2Register(void)
{
	amFlag = 1;
	amOut = modVal&0x1F;
	return 1;
}

uint32_t am2RegisterIndirect(void)
{
	amFlag = 0;
	amOut = v60.reg[modVal&0x1F];
	return 1;
}

uint32_t bam2RegisterIndirect(void)
{
	amFlag = 0;
	amOut = v60.reg[modVal&0x1F];
	bamOffset = 0;
	return 1;
}

uint32_t am2RegisterIndirectIndexed(void)
{
	amFlag = 0;

	switch (modDim)
	{
	case 0:
		amOut = v60.reg[modVal2&0x1F] + v60.reg[modVal&0x1F];
		break;
	case 1:
		amOut = v60.reg[modVal2&0x1F] + v60.reg[modVal&0x1F] * 2;
		break;
	case 2:
		amOut = v60.reg[modVal2&0x1F] + v60.reg[modVal&0x1F] * 4;
		break;
	case 3:
		amOut = v60.reg[modVal2&0x1F] + v60.reg[modVal&0x1F] * 8;
		break;
	}

	return 2;
}

uint32_t bam2RegisterIndirectIndexed(void)
{
	amFlag = 0;
	amOut = v60.reg[modVal2&0x1F];
	bamOffset = v60.reg[modVal&0x1F];
	return 2;
}

uint32_t am2Autoincrement(void)
{
	amFlag = 0;
	amOut = v60.reg[modVal&0x1F];

	switch (modDim)
	{
	case 0:
		v60.reg[modVal&0x1F] += 1;
		break;
	case 1:
		v60.reg[modVal&0x1F] += 2;
		break;
	case 2:
		v60.reg[modVal&0x1F] += 4;
		break;
	case 3:
		v60.reg[modVal&0x1F] += 8;
		break;
	}

	return 1;
}

uint32_t bam2Autoincrement(void)
{
	amFlag = 0;
	amOut = v60.reg[modVal&0x1F];
	bamOffset = 0;

	switch (modDim)
	{
	case 10:
		v60.reg[modVal&0x1F]+=1;
		break;
	case 11:
		v60.reg[modVal&0x1F]+=4;
		break;
	default:
		logerror("CPU - AM2 - 7 (t0 PC=%x)\n", PC);
		abort();
		break;
	}

	return 1;
}

uint32_t am2Autodecrement(void)
{
	amFlag = 0;

	switch (modDim)
	{
	case 0:
		v60.reg[modVal&0x1F] -= 1;
		break;
	case 1:
		v60.reg[modVal&0x1F] -= 2;
		break;
	case 2:
		v60.reg[modVal&0x1F] -= 4;
		break;
	case 3:
		v60.reg[modVal&0x1F] -= 8;
		break;
	}

	amOut = v60.reg[modVal&0x1F];
	return 1;
}

uint32_t bam2Autodecrement(void)
{
	amFlag = 0;
	bamOffset = 0;

	switch (modDim)
	{
	case 10:
		v60.reg[modVal&0x1F]-=1;
		break;
	case 11:
		v60.reg[modVal&0x1F]-=4;
		break;
	default:
		logerror("CPU - BAM2 - 7 (PC=%06x)\n", PC);
		abort();
		break;
	}

	amOut = v60.reg[modVal&0x1F];
	return 1;
}


uint32_t am2Displacement8(void)
{
	amFlag = 0;
	amOut = v60.reg[modVal&0x1F] + (int8_t)OpRead8(modAdd+1);

	return 2;
}

uint32_t bam2Displacement8(void)
{
	amFlag = 0;
	amOut = v60.reg[modVal&0x1F];
	bamOffset = (int8_t)OpRead8(modAdd+1);

	return 2;
}

uint32_t am2Displacement16(void)
{
	amFlag = 0;
	amOut = v60.reg[modVal&0x1F] + (int16_t)OpRead16(modAdd+1);

	return 3;
}

uint32_t bam2Displacement16(void)
{
	amFlag = 0;
	amOut = v60.reg[modVal&0x1F];
	bamOffset = (int16_t)OpRead16(modAdd+1);

	return 3;
}

uint32_t am2Displacement32(void)
{
	amFlag = 0;
	amOut = v60.reg[modVal&0x1F] + OpRead32(modAdd+1);

	return 5;
}

uint32_t bam2Displacement32(void)
{
	amFlag = 0;
	amOut = v60.reg[modVal&0x1F];
	bamOffset = OpRead32(modAdd+1);

	return 5;
}

uint32_t am2DisplacementIndexed8(void)
{
	amFlag = 0;

	switch (modDim)
	{
	case 0:
		amOut = v60.reg[modVal2&0x1F] + (int8_t)OpRead8(modAdd+2) + v60.reg[modVal&0x1F];
		break;
	case 1:
		amOut = v60.reg[modVal2&0x1F] + (int8_t)OpRead8(modAdd+2) + v60.reg[modVal&0x1F] * 2;
		break;
	case 2:
		amOut = v60.reg[modVal2&0x1F] + (int8_t)OpRead8(modAdd+2) + v60.reg[modVal&0x1F] * 4;
		break;
	case 3:
		amOut = v60.reg[modVal2&0x1F] + (int8_t)OpRead8(modAdd+2) + v60.reg[modVal&0x1F] * 8;
		break;
	}

	return 3;
}

uint32_t bam2DisplacementIndexed8(void)
{
	amFlag = 0;
	amOut = v60.reg[modVal2&0x1F] + (int8_t)OpRead8(modAdd+2);
	bamOffset = v60.reg[modVal&0x1F];

	return 3;
}

uint32_t am2DisplacementIndexed16(void)
{
	amFlag = 0;

	switch (modDim)
	{
	case 0:
		amOut = v60.reg[modVal2&0x1F] + (int16_t)OpRead16(modAdd+2) + v60.reg[modVal&0x1F];
		break;
	case 1:
		amOut = v60.reg[modVal2&0x1F] + (int16_t)OpRead16(modAdd+2) + v60.reg[modVal&0x1F] * 2;
		break;
	case 2:
		amOut = v60.reg[modVal2&0x1F] + (int16_t)OpRead16(modAdd+2) + v60.reg[modVal&0x1F] * 4;
		break;
	case 3:
		amOut = v60.reg[modVal2&0x1F] + (int16_t)OpRead16(modAdd+2) + v60.reg[modVal&0x1F] * 8;
		break;
	}

	return 4;
}

uint32_t bam2DisplacementIndexed16(void)
{
	amFlag = 0;
	amOut = v60.reg[modVal2&0x1F] + (int16_t)OpRead16(modAdd+2);
	bamOffset = v60.reg[modVal&0x1F];

	return 4;
}

uint32_t am2DisplacementIndexed32(void)
{
	amFlag = 0;

	switch (modDim)
	{
	case 0:
		amOut = v60.reg[modVal2&0x1F] + OpRead32(modAdd+2) + v60.reg[modVal&0x1F];
		break;
	case 1:
		amOut = v60.reg[modVal2&0x1F] + OpRead32(modAdd+2) + v60.reg[modVal&0x1F] * 2;
		break;
	case 2:
		amOut = v60.reg[modVal2&0x1F] + OpRead32(modAdd+2) + v60.reg[modVal&0x1F] * 4;
		break;
	case 3:
		amOut = v60.reg[modVal2&0x1F] + OpRead32(modAdd+2) + v60.reg[modVal&0x1F] * 8;
		break;
	}

	return 6;
}

uint32_t bam2DisplacementIndexed32(void)
{
	amFlag = 0;
	amOut = v60.reg[modVal2&0x1F] + OpRead32(modAdd+2);
	bamOffset = v60.reg[modVal&0x1F];

	return 6;
}

uint32_t am2PCDisplacement8(void)
{
	amFlag = 0;
	amOut = PC + (int8_t)OpRead8(modAdd+1);

	return 2;
}

uint32_t bam2PCDisplacement8(void)
{
	amFlag = 0;
	amOut = PC;
	bamOffset = (int8_t)OpRead8(modAdd+1);

	return 2;
}

uint32_t am2PCDisplacement16(void)
{
	amFlag = 0;
	amOut = PC + (int16_t)OpRead16(modAdd+1);

	return 3;
}

uint32_t bam2PCDisplacement16(void)
{
	amFlag = 0;
	amOut = PC;
	bamOffset = (int16_t)OpRead16(modAdd+1);

	return 3;
}

uint32_t am2PCDisplacement32(void)
{
	amFlag = 0;
	amOut = PC + OpRead32(modAdd+1);

	return 5;
}

uint32_t bam2PCDisplacement32(void)
{
	amFlag = 0;
	amOut = PC;
	bamOffset = OpRead32(modAdd+1);

	return 5;
}


uint32_t am2PCDisplacementIndexed8(void)
{
	amFlag = 0;

	switch (modDim)
	{
	case 0:
		amOut = PC + (int8_t)OpRead8(modAdd+2) + v60.reg[modVal&0x1F];
		break;
	case 1:
		amOut = PC + (int8_t)OpRead8(modAdd+2) + v60.reg[modVal&0x1F] * 2;
		break;
	case 2:
		amOut = PC + (int8_t)OpRead8(modAdd+2) + v60.reg[modVal&0x1F] * 4;
		break;
	case 3:
		amOut = PC + (int8_t)OpRead8(modAdd+2) + v60.reg[modVal&0x1F] * 8;
		break;
	}

	return 3;
}

uint32_t bam2PCDisplacementIndexed8(void)
{
	amFlag = 0;
	amOut = PC + (int8_t)OpRead8(modAdd+2);
	bamOffset = v60.reg[modVal&0x1F];

	return 3;
}

uint32_t am2PCDisplacementIndexed16(void)
{
	amFlag = 0;

	switch (modDim)
	{
	case 0:
		amOut = PC + (int16_t)OpRead16(modAdd+2) + v60.reg[modVal&0x1F];
		break;
	case 1:
		amOut = PC + (int16_t)OpRead16(modAdd+2) + v60.reg[modVal&0x1F] * 2;
		break;
	case 2:
		amOut = PC + (int16_t)OpRead16(modAdd+2) + v60.reg[modVal&0x1F] * 4;
		break;
	case 3:
		amOut = PC + (int16_t)OpRead16(modAdd+2) + v60.reg[modVal&0x1F] * 8;
		break;
	}

	return 4;
}

uint32_t bam2PCDisplacementIndexed16(void)
{
	amFlag = 0;
	amOut = PC + (int16_t)OpRead16(modAdd+2);
	bamOffset = v60.reg[modVal&0x1F];

	return 4;
}

uint32_t am2PCDisplacementIndexed32(void)
{
	amFlag = 0;

	switch (modDim)
	{
	case 0:
		amOut = PC + OpRead32(modAdd+2) + v60.reg[modVal&0x1F];
		break;
	case 1:
		amOut = PC + OpRead32(modAdd+2) + v60.reg[modVal&0x1F] * 2;
		break;
	case 2:
		amOut = PC + OpRead32(modAdd+2) + v60.reg[modVal&0x1F] * 4;
		break;
	case 3:
		amOut = PC + OpRead32(modAdd+2) + v60.reg[modVal&0x1F] * 8;
		break;
	}

	return 6;
}

uint32_t bam2PCDisplacementIndexed32(void)
{
	amFlag = 0;
	amOut = PC + OpRead32(modAdd+2);
	bamOffset = v60.reg[modVal&0x1F];

	return 6;
}

uint32_t am2DisplacementIndirect8(void)
{
	amFlag = 0;
	amOut = MemRead32(v60.reg[modVal&0x1F] + (int8_t)OpRead8(modAdd+1));

	return 2;
}

uint32_t bam2DisplacementIndirect8(void)
{
	amFlag = 0;
	amOut = MemRead32(v60.reg[modVal&0x1F] + (int8_t)OpRead8(modAdd+1));
	bamOffset = 0;
	return 2;
}

uint32_t am2DisplacementIndirect16(void)
{
	amFlag = 0;
	amOut = MemRead32(v60.reg[modVal&0x1F] + (int16_t)OpRead16(modAdd+1));

	return 3;
}

uint32_t bam2DisplacementIndirect16(void)
{
	amFlag = 0;
	amOut = MemRead32(v60.reg[modVal&0x1F] + (int16_t)OpRead16(modAdd+1));
	bamOffset = 0;
	return 3;
}

uint32_t am2DisplacementIndirect32(void)
{
	amFlag = 0;
	amOut = MemRead32(v60.reg[modVal&0x1F] + OpRead32(modAdd+1));

	return 5;
}

uint32_t bam2DisplacementIndirect32(void)
{
	amFlag = 0;
	amOut = MemRead32(v60.reg[modVal&0x1F] + OpRead32(modAdd+1));
	bamOffset = 0;

	return 5;
}

uint32_t am2DisplacementIndirectIndexed8(void)
{
	amFlag = 0;

	switch (modDim)
	{
	case 0:
		amOut = MemRead32(v60.reg[modVal2&0x1F] + (int8_t)OpRead8(modAdd+2)) + v60.reg[modVal&0x1F];
		break;
	case 1:
		amOut = MemRead32(v60.reg[modVal2&0x1F] + (int8_t)OpRead8(modAdd+2)) + v60.reg[modVal&0x1F] * 2;
		break;
	case 2:
		amOut = MemRead32(v60.reg[modVal2&0x1F] + (int8_t)OpRead8(modAdd+2)) + v60.reg[modVal&0x1F] * 4;
		break;
	case 3:
		amOut = MemRead32(v60.reg[modVal2&0x1F] + (int8_t)OpRead8(modAdd+2)) + v60.reg[modVal&0x1F] * 8;
		break;
	}

	return 3;
}

uint32_t bam2DisplacementIndirectIndexed8(void)
{
	amFlag = 0;
	amOut = MemRead32(v60.reg[modVal2&0x1F] + (int8_t)OpRead8(modAdd+2));
	bamOffset = v60.reg[modVal&0x1F];

	return 3;
}

uint32_t am2DisplacementIndirectIndexed16(void)
{
	amFlag = 0;

	switch (modDim)
	{
	case 0:
		amOut = MemRead32(v60.reg[modVal2&0x1F] + (int16_t)OpRead16(modAdd+2)) + v60.reg[modVal&0x1F];
		break;
	case 1:
		amOut = MemRead32(v60.reg[modVal2&0x1F] + (int16_t)OpRead16(modAdd+2)) + v60.reg[modVal&0x1F] * 2;
		break;
	case 2:
		amOut = MemRead32(v60.reg[modVal2&0x1F] + (int16_t)OpRead16(modAdd+2)) + v60.reg[modVal&0x1F] * 4;
		break;
	case 3:
		amOut = MemRead32(v60.reg[modVal2&0x1F] + (int16_t)OpRead16(modAdd+2)) + v60.reg[modVal&0x1F] * 8;
		break;
	}

	return 4;
}

uint32_t bam2DisplacementIndirectIndexed16(void)
{
	amFlag = 0;
	amOut = MemRead32(v60.reg[modVal2&0x1F] + (int16_t)OpRead16(modAdd+2));
	bamOffset = v60.reg[modVal&0x1F];

	return 4;
}

uint32_t am2DisplacementIndirectIndexed32(void)
{
	amFlag = 0;

	switch (modDim)
	{
	case 0:
		amOut = MemRead32(v60.reg[modVal2&0x1F] + OpRead32(modAdd+2)) + v60.reg[modVal&0x1F];
		break;
	case 1:
		amOut = MemRead32(v60.reg[modVal2&0x1F] + OpRead32(modAdd+2)) + v60.reg[modVal&0x1F] * 2;
		break;
	case 2:
		amOut = MemRead32(v60.reg[modVal2&0x1F] + OpRead32(modAdd+2)) + v60.reg[modVal&0x1F] * 4;
		break;
	case 3:
		amOut = MemRead32(v60.reg[modVal2&0x1F] + OpRead32(modAdd+2)) + v60.reg[modVal&0x1F] * 8;
		break;
	}

	return 6;
}

uint32_t bam2DisplacementIndirectIndexed32(void)
{
	amFlag = 0;
	amOut = MemRead32(v60.reg[modVal2&0x1F] + OpRead32(modAdd+2));
	bamOffset = v60.reg[modVal&0x1F];

	return 6;
}

uint32_t am2PCDisplacementIndirect8(void)
{
	amFlag = 0;
	amOut = MemRead32(PC + (int8_t)OpRead8(modAdd+1));

	return 2;
}

uint32_t bam2PCDisplacementIndirect8(void)
{
	amFlag = 0;
	amOut = MemRead32(PC + (int8_t)OpRead8(modAdd+1));
	bamOffset = 0;

	return 2;
}

uint32_t am2PCDisplacementIndirect16(void)
{
	amFlag = 0;
	amOut = MemRead32(PC + (int16_t)OpRead16(modAdd+1));

	return 3;
}

uint32_t bam2PCDisplacementIndirect16(void)
{
	amFlag = 0;
	amOut = MemRead32(PC + (int16_t)OpRead16(modAdd+1));
	bamOffset = 0;

	return 3;
}

uint32_t am2PCDisplacementIndirect32(void)
{
	amFlag = 0;
	amOut = MemRead32(PC + OpRead32(modAdd+1));

	return 5;
}

uint32_t bam2PCDisplacementIndirect32(void)
{
	amFlag = 0;
	amOut = MemRead32(PC + OpRead32(modAdd+1));
	bamOffset = 0;

	return 5;
}

uint32_t am2PCDisplacementIndirectIndexed8(void)
{
	amFlag = 0;

	switch (modDim)
	{
	case 0:
		amOut = MemRead32(PC + (int8_t)OpRead8(modAdd+2)) + v60.reg[modVal&0x1F];
		break;
	case 1:
		amOut = MemRead32(PC + (int8_t)OpRead8(modAdd+2)) + v60.reg[modVal&0x1F] * 2;
		break;
	case 2:
		amOut = MemRead32(PC + (int8_t)OpRead8(modAdd+2)) + v60.reg[modVal&0x1F] * 4;
		break;
	case 3:
		amOut = MemRead32(PC + (int8_t)OpRead8(modAdd+2)) + v60.reg[modVal&0x1F] * 8;
		break;
	}

	return 3;
}

uint32_t bam2PCDisplacementIndirectIndexed8(void)
{
	amFlag = 0;
	amOut = MemRead32(PC + (int8_t)OpRead8(modAdd+2));
	bamOffset = v60.reg[modVal&0x1F];

	return 3;
}

uint32_t am2PCDisplacementIndirectIndexed16(void)
{
	amFlag = 0;

	switch (modDim)
	{
	case 0:
		amOut = MemRead32(PC + (int16_t)OpRead16(modAdd+2)) + v60.reg[modVal&0x1F];
		break;
	case 1:
		amOut = MemRead32(PC + (int16_t)OpRead16(modAdd+2)) + v60.reg[modVal&0x1F] * 2;
		break;
	case 2:
		amOut = MemRead32(PC + (int16_t)OpRead16(modAdd+2)) + v60.reg[modVal&0x1F] * 4;
		break;
	case 3:
		amOut = MemRead32(PC + (int16_t)OpRead16(modAdd+2)) + v60.reg[modVal&0x1F] * 8;
		break;
	}

	return 4;
}


uint32_t bam2PCDisplacementIndirectIndexed16(void)
{
	amFlag = 0;
	amOut = MemRead32(PC + (int16_t)OpRead16(modAdd+2));
	bamOffset = v60.reg[modVal&0x1F];

	return 4;
}


uint32_t am2PCDisplacementIndirectIndexed32(void)
{
	amFlag = 0;

	switch (modDim)
	{
	case 0:
		amOut = MemRead32(PC + OpRead32(modAdd+2)) + v60.reg[modVal&0x1F];
		break;
	case 1:
		amOut = MemRead32(PC + OpRead32(modAdd+2)) + v60.reg[modVal&0x1F] * 2;
		break;
	case 2:
		amOut = MemRead32(PC + OpRead32(modAdd+2)) + v60.reg[modVal&0x1F] * 4;
		break;
	case 3:
		amOut = MemRead32(PC + OpRead32(modAdd+2)) + v60.reg[modVal&0x1F] * 8;
		break;
	}

	return 6;
}

uint32_t bam2PCDisplacementIndirectIndexed32(void)
{
	amFlag = 0;
	amOut = MemRead32(PC + OpRead32(modAdd+2));
	bamOffset = v60.reg[modVal&0x1F];

	return 6;
}

uint32_t am2DoubleDisplacement8(void)
{
	amFlag = 0;
	amOut = MemRead32(v60.reg[modVal&0x1F] + (int8_t)OpRead8(modAdd+1)) + (int8_t)OpRead8(modAdd+2);

	return 3;
}

uint32_t bam2DoubleDisplacement8(void)
{
	amFlag = 0;
	amOut = MemRead32(v60.reg[modVal&0x1F] + (int8_t)OpRead8(modAdd+1));
	bamOffset = (int8_t)OpRead8(modAdd+2);

	return 3;
}

uint32_t am2DoubleDisplacement16(void)
{
	amFlag = 0;
	amOut = MemRead32(v60.reg[modVal&0x1F] + (int16_t)OpRead16(modAdd+1)) + (int16_t)OpRead16(modAdd+3);

	return 5;
}

uint32_t bam2DoubleDisplacement16(void)
{
	amFlag = 0;
	amOut = MemRead32(v60.reg[modVal&0x1F] + (int16_t)OpRead16(modAdd+1));
	bamOffset = (int8_t)OpRead8(modAdd+3);

	return 5;
}

uint32_t am2DoubleDisplacement32(void)
{
	amFlag = 0;
	amOut = MemRead32(v60.reg[modVal&0x1F] + OpRead32(modAdd+1)) + OpRead32(modAdd+5);

	return 9;
}

uint32_t bam2DoubleDisplacement32(void)
{
	amFlag = 0;
	amOut = MemRead32(v60.reg[modVal&0x1F] + OpRead32(modAdd+1));
	bamOffset = OpRead32(modAdd+5);

	return 9;
}


uint32_t am2PCDoubleDisplacement8(void)
{
	amFlag = 0;
	amOut = MemRead32(PC + (int8_t)OpRead8(modAdd+1)) + (int8_t)OpRead8(modAdd+2);

	return 3;
}

uint32_t bam2PCDoubleDisplacement8(void)
{
	amFlag = 0;
	amOut = MemRead32(PC + (int8_t)OpRead8(modAdd+1));
	bamOffset = (int8_t)OpRead8(modAdd+2);

	return 3;
}

uint32_t am2PCDoubleDisplacement16(void)
{
	amFlag = 0;
	amOut = MemRead32(PC + (int16_t)OpRead16(modAdd+1)) + (int16_t)OpRead16(modAdd+3);

	return 5;
}

uint32_t bam2PCDoubleDisplacement16(void)
{
	amFlag = 0;
	amOut = MemRead32(PC + (int16_t)OpRead16(modAdd+1));
	bamOffset = (int8_t)OpRead8(modAdd+3);

	return 5;
}

uint32_t am2PCDoubleDisplacement32(void)
{
	amFlag = 0;
	amOut = MemRead32(PC + OpRead32(modAdd+1)) + OpRead32(modAdd+5);

	return 9;
}

uint32_t bam2PCDoubleDisplacement32(void)
{
	amFlag = 0;
	amOut = MemRead32(PC + OpRead32(modAdd+1));
	bamOffset = OpRead32(modAdd+5);

	return 9;
}

uint32_t am2DirectAddress(void)
{
	amFlag = 0;
	amOut = OpRead32(modAdd+1);

	return 5;
}

uint32_t bam2DirectAddress(void)
{
	amFlag = 0;
	amOut = OpRead32(modAdd+1);
	bamOffset = 0;

	return 5;
}

uint32_t am2DirectAddressIndexed(void)
{
	amFlag = 0;

	switch (modDim)
	{
	case 0:
		amOut = OpRead32(modAdd+2) + v60.reg[modVal&0x1F];
		break;
	case 1:
		amOut = OpRead32(modAdd+2) + v60.reg[modVal&0x1F] * 2;
		break;
	case 2:
		amOut = OpRead32(modAdd+2) + v60.reg[modVal&0x1F] * 4;
		break;
	case 3:
		amOut = OpRead32(modAdd+2) + v60.reg[modVal&0x1F] * 8;
		break;
	}

	return 6;
}

uint32_t bam2DirectAddressIndexed(void)
{
	amFlag = 0;
	amOut = OpRead32(modAdd+2);
	bamOffset = v60.reg[modVal&0x1F];

	return 6;
}

uint32_t am2DirectAddressDeferred(void)
{
	amFlag = 0;
	amOut = MemRead32(OpRead32(modAdd+1));

	return 5;
}

uint32_t bam2DirectAddressDeferred(void)
{
	amFlag = 0;
	amOut = MemRead32(OpRead32(modAdd+1));
	bamOffset = 0;

	return 5;
}

uint32_t am2DirectAddressDeferredIndexed(void)
{
	amFlag = 0;

	switch (modDim)
	{
	case 0:
		amOut = MemRead32(OpRead32(modAdd+2)) + v60.reg[modVal&0x1F];
		break;
	case 1:
		amOut = MemRead32(OpRead32(modAdd+2)) + v60.reg[modVal&0x1F] * 2;
		break;
	case 2:
		amOut = MemRead32(OpRead32(modAdd+2)) + v60.reg[modVal&0x1F] * 4;
		break;
	case 3:
		amOut = MemRead32(OpRead32(modAdd+2)) + v60.reg[modVal&0x1F] * 8;
		break;
	}

	return 6;
}

uint32_t bam2DirectAddressDeferredIndexed(void)
{
	amFlag = 0;
	amOut = MemRead32(OpRead32(modAdd+2));
	bamOffset = v60.reg[modVal&0x1F];

	return 6;
}

uint32_t am2Immediate(void)
{
	// Fuck off LDPR
	return am1Immediate();
}

uint32_t am2ImmediateQuick(void)
{
	// fuck off LDPR
	return am1ImmediateQuick();
}


// AM2 Tables (for ReadAMAddress)
// ******************************

uint32_t am2Error1(void)
{
	logerror("CPU - AM2 - 1 (PC=%06x)\n", PC);
	abort();
}

uint32_t am2Error2(void)
{
	logerror("CPU - AM2 - 2 (PC=%06x)\n", PC);
	abort();
}

uint32_t am2Error3(void)
{
	logerror("CPU - AM2 - 3 (PC=%06x)\n", PC);
	abort();
}

uint32_t am2Error4(void)
{
	logerror("CPU - AM2 - 4 (PC=%06x)\n", PC);
	abort();
}

uint32_t am2Error5(void)
{
	logerror("CPU - AM2 - 5 (PC=%06x)\n", PC);
	abort();
}

uint32_t bam2Error1(void)
{
	logerror("CPU - BAM2 - 1 (PC=%06x)\n", PC);
	abort();
}

uint32_t bam2Error2(void)
{
	logerror("CPU - BAM2 - 2 (PC=%06x)\n", PC);
	abort();
}

uint32_t bam2Error3(void)
{
	logerror("CPU - BAM2 - 3 (PC=%06x)\n", PC);
	abort();
}

uint32_t bam2Error4(void)
{
	logerror("CPU - BAM2 - 4 (PC=%06x)\n", PC);
	abort();
}

uint32_t bam2Error5(void)
{
	logerror("CPU - BAM2 - 5 (PC=%06x)\n", PC);
	abort();
}

uint32_t bam2Error6(void)
{
	logerror("CPU - BAM2 - 6 (PC=%06x)\n", PC);
	abort();
}


uint32_t (*AMTable2_G7a[16])(void) =
{
	am2PCDisplacementIndexed8,
	am2PCDisplacementIndexed16,
	am2PCDisplacementIndexed32,
	am2DirectAddressIndexed,
	am2Error5,
	am2Error5,
	am2Error5,
	am2Error5,
	am2PCDisplacementIndirectIndexed8,
	am2PCDisplacementIndirectIndexed16,
	am2PCDisplacementIndirectIndexed32,
	am2DirectAddressDeferredIndexed,
	am2Error5,
	am2Error5,
	am2Error5,
	am2Error5
};

uint32_t (*BAMTable2_G7a[16])(void) =
{
	bam2PCDisplacementIndexed8,
	bam2PCDisplacementIndexed16,
	bam2PCDisplacementIndexed32,
	bam2DirectAddressIndexed,
	bam2Error5,
	bam2Error5,
	bam2Error5,
	bam2Error5,
	bam2PCDisplacementIndirectIndexed8,
	bam2PCDisplacementIndirectIndexed16,
	bam2PCDisplacementIndirectIndexed32,
	bam2DirectAddressDeferredIndexed,
	bam2Error5,
	bam2Error5,
	bam2Error5,
	bam2Error5
};

uint32_t am2Group7a(void)
{
	if (!(modVal2&0x10))
		return am2Error4();

	return AMTable2_G7a[modVal2&0xF]();
}

uint32_t bam2Group7a(void)
{
	if (!(modVal2&0x10))
		return bam2Error4();

	return BAMTable2_G7a[modVal2&0xF]();
}

uint32_t (*AMTable2_G7[32])(void) =
{
	am2ImmediateQuick,
	am2ImmediateQuick,
	am2ImmediateQuick,
	am2ImmediateQuick,
	am2ImmediateQuick,
	am2ImmediateQuick,
	am2ImmediateQuick,
	am2ImmediateQuick,
	am2ImmediateQuick,
	am2ImmediateQuick,
	am2ImmediateQuick,
	am2ImmediateQuick,
	am2ImmediateQuick,
	am2ImmediateQuick,
	am2ImmediateQuick,
	am2ImmediateQuick,
	am2PCDisplacement8,
	am2PCDisplacement16,
	am2PCDisplacement32,
	am2DirectAddress,
	am2Immediate,
	am2Error2,
	am2Error2,
	am2Error2,
	am2PCDisplacementIndirect8,
	am2PCDisplacementIndirect16,
	am2PCDisplacementIndirect32,
	am2DirectAddressDeferred,
	am2PCDoubleDisplacement8,
	am2PCDoubleDisplacement16,
	am2PCDoubleDisplacement32,
	am2Error2
};

uint32_t (*BAMTable2_G7[32])(void) =
{
	bam2Error6,
	bam2Error6,
	bam2Error6,
	bam2Error6,
	bam2Error6,
	bam2Error6,
	bam2Error6,
	bam2Error6,
	bam2Error6,
	bam2Error6,
	bam2Error6,
	bam2Error6,
	bam2Error6,
	bam2Error6,
	bam2Error6,
	bam2Error6,
	bam2PCDisplacement8,
	bam2PCDisplacement16,
	bam2PCDisplacement32,
	bam2DirectAddress,
	bam2Error6,
	bam2Error2,
	bam2Error2,
	bam2Error2,
	bam2PCDisplacementIndirect8,
	bam2PCDisplacementIndirect16,
	bam2PCDisplacementIndirect32,
	bam2DirectAddressDeferred,
	bam2PCDoubleDisplacement8,
	bam2PCDoubleDisplacement16,
	bam2PCDoubleDisplacement32,
	bam2Error2
};

uint32_t (*AMTable2_G6[8])(void) =
{
	am2DisplacementIndexed8,
	am2DisplacementIndexed16,
	am2DisplacementIndexed32,
	am2RegisterIndirectIndexed,
	am2DisplacementIndirectIndexed8,
	am2DisplacementIndirectIndexed16,
	am2DisplacementIndirectIndexed32,
	am2Group7a
};

uint32_t (*BAMTable2_G6[8])(void) =
{
	bam2DisplacementIndexed8,
	bam2DisplacementIndexed16,
	bam2DisplacementIndexed32,
	bam2RegisterIndirectIndexed,
	bam2DisplacementIndirectIndexed8,
	bam2DisplacementIndirectIndexed16,
	bam2DisplacementIndirectIndexed32,
	bam2Group7a
};




uint32_t am2Group6(void)
{
	modVal2=OpRead8(modAdd+1);
	return AMTable2_G6[modVal2>>5]();
}
uint32_t bam2Group6(void)
{
	modVal2=OpRead8(modAdd+1);
	return BAMTable2_G6[modVal2>>5]();
}

uint32_t am2Group7(void)
{
	return AMTable2_G7[modVal&0x1F]();
}
uint32_t bam2Group7(void)
{
	return BAMTable2_G7[modVal&0x1F]();
}


uint32_t (*AMTable2[2][8])(void) =
{
	{
		am2Displacement8,
		am2Displacement16,
		am2Displacement32,
		am2RegisterIndirect,
		am2DisplacementIndirect8,
		am2DisplacementIndirect16,
		am2DisplacementIndirect32,
		am2Group7
	},

	{
		am2DoubleDisplacement8,
		am2DoubleDisplacement16,
		am2DoubleDisplacement32,
		am2Register,
		am2Autoincrement,
		am2Autodecrement,
		am2Group6,
		am2Error1
	}
};

uint32_t (*BAMTable2[2][8])(void) =
{
	{
		bam2Displacement8,
		bam2Displacement16,
		bam2Displacement32,
		bam2RegisterIndirect,
		bam2DisplacementIndirect8,
		bam2DisplacementIndirect16,
		bam2DisplacementIndirect32,
		bam2Group7
	},

	{
		bam2DoubleDisplacement8,
		bam2DoubleDisplacement16,
		bam2DoubleDisplacement32,
		bam2Error6,
		bam2Autoincrement,
		bam2Autodecrement,
		bam2Group6,
		bam2Error1
	}
};




