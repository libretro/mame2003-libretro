
// NOTE for bit string/field addressing
// ************************************
// modDim must be passed as 10 for bit string instructions,
// and as 11 for bit field instructions



// Output variables for ReadAMAddress()
uint8_t amFlag;
uint32_t amOut;
uint32_t bamOffset;

// Appo temp var
uint32_t amLength1,amLength2,amLength3,amLength4;


// Global vars used by AM functions
uint32_t modAdd;
uint8_t modM;
uint8_t modVal;
uint8_t modVal2;
uint8_t modWriteValB;
uint16_t modWriteValH;
uint32_t modWriteValW;
uint8_t modDim;

// Addressing mode functions and tables
#include "am1.c" // ReadAM
#include "am2.c" // ReadAMAddress
#include "am3.c" // WriteAM

/*
  Input:
  modAdd
	modDim

  Output:
	amOut
	amLength
*/

uint32_t ReadAM(void)
{
	modM=modM?1:0;
	modVal=OpRead8(modAdd);
	return AMTable1[modM][modVal>>5]();
}

uint32_t BitReadAM(void)
{
	modM=modM?1:0;
	modVal=OpRead8(modAdd);
	return BAMTable1[modM][modVal>>5]();
}



/*
  Input:
  modAdd
	modDim

  Output:
	amOut
	amFlag
	amLength
*/

uint32_t ReadAMAddress(void)
{
	modM=modM?1:0;
	modVal=OpRead8(modAdd);
	return AMTable2[modM][modVal>>5]();
}

uint32_t BitReadAMAddress(void)
{
	modM=modM?1:0;
	modVal=OpRead8(modAdd);
	return BAMTable2[modM][modVal>>5]();
}

/*
  Input:
  modAdd
	modDim
	modWriteValB/H/W

  Output:
	amOut
	amLength
*/

uint32_t WriteAM(void)
{
	modM=modM?1:0;
	modVal=OpRead8(modAdd);
	return AMTable3[modM][modVal>>5]();
}


