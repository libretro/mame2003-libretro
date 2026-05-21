/*********************************************************/
/*    SEGA 16ch 8bit PCM                                 */
/*********************************************************/

#include "driver.h"

struct segapcm
{
	uint8_t  *ram;
	uint16_t low[16];
	const uint8_t *rom, *rom_end;
	uint32_t *step;
	int rate;
	int bankshift;
	int bankmask;
} spcm;

static void SEGAPCM_update(int num, int16_t **buffer, int length)
{
	int ch;
	memset(buffer[0], 0, length*2);
	memset(buffer[1], 0, length*2);

	for(ch=0; ch<16; ch++)
		if(!(spcm.ram[0x86+8*ch] & 1)) {
			uint8_t *base = spcm.ram+8*ch;
			uint32_t addr = (base[5] << 24) | (base[4] << 16) | spcm.low[ch];
			uint16_t loop = (base[0x85] << 8)|base[0x84];
			uint8_t end = base[6]+1;
			uint8_t delta = base[7];
			uint32_t step = spcm.step[delta];
			uint8_t voll = base[2];
			uint8_t volr = base[3];
			uint8_t flags = base[0x86];
			const uint8_t *rom = spcm.rom + ((flags & spcm.bankmask) << spcm.bankshift);
			int i;

			for(i=0; i<length; i++) {
				int8_t v;
				const uint8_t *ptr;
				if((addr >> 24) == end) {
					if(!(flags & 2))
						addr = loop << 16;
					else {
						flags |= 1;
						break;
					}
				}
				ptr = rom + (addr >> 16);
				if(ptr < spcm.rom_end)
					v = rom[addr>>16] - 0x80;
				else
					v = 0;
				buffer[0][i] += (v*voll);
				buffer[1][i] += (v*volr);
				addr += step;
			}
			base[0x86] = flags;
			base[4] = addr >> 16;
			base[5] = addr >> 24;
			spcm.low[ch] = flags & 1 ? 0 : addr;
		}
}

int SEGAPCM_sh_start( const struct MachineSound *msound )
{
	struct SEGAPCMinterface *intf = msound->sound_interface;
	const char *name[2];
	int vol[2];
	int mask, rom_mask;
	int i;

	spcm.rate = intf->mode == SEGAPCM_SAMPLE15K ? 4000000/256 : 4000000/128;

	spcm.rom = (const uint8_t *)memory_region(intf->region);
	spcm.rom_end = spcm.rom + memory_region_length(intf->region);
	spcm.ram = auto_malloc(0x800);
	spcm.step = auto_malloc(sizeof(uint32_t)*256);

	if(!spcm.ram || !spcm.step)
		return 1;

	for(i=0; i<256; i++)
		spcm.step[i] = i*spcm.rate*(double)(65536/128) / Machine->sample_rate;

	memset(spcm.ram, 0xff, 0x800);

	spcm.bankshift = (uint8_t)(intf->bank);
	mask = intf->bank >> 16;
	if(!mask)
		mask = BANK_MASK7>>16;

	for(rom_mask = 1; rom_mask < memory_region_length(intf->region); rom_mask *= 2);
	rom_mask--;

	spcm.bankmask = mask & (rom_mask >> spcm.bankshift);

	name[0] = "SEGAPCM L";
	name[1] = "SEGAPCM R";
	vol[0] = (MIXER_PAN_LEFT<<8)  | (intf->volume & 0xff);
	vol[1] = (MIXER_PAN_RIGHT<<8) | (intf->volume & 0xff);
	stream_init_multi(2, name, vol, Machine->sample_rate, 0, SEGAPCM_update );

	return 0;
}

void SEGAPCM_sh_stop( void )
{
}


WRITE_HANDLER( SegaPCM_w )
{
	spcm.ram[offset & 0x07ff] = data;
}

READ_HANDLER( SegaPCM_r )
{
	return spcm.ram[offset & 0x07ff];
}
