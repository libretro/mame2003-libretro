# MAME 2003 CHANGELOG

# 0.78

## General Source Changes


Various System 24 Improvements, Fixing Priorities, Clipping issues etc.  [Olivier Galibert]
	src/drivers/system24.c
	src/vidhrdw/system24.c
	src/vidhrdw/segaic24.c
	src/vidhrdw/segaic24.h

Improved Lightgun Code / Reload Handling [Bryan McPhail]
	'-lightgun' now enables the lightgun only - no button remapping is performed.
	'-reload' (formally '-offscreen_reload') is now needed for 2nd button presses
           to map to offscreen shots.
	'-dual' (formally '-dual_lightgun') adds support for the Actlabs Dual Lightgun
           setup, it may be used with and without '-reload' and it requires '-lightgun'. 
           This mode attempts to automatically remap the player 1 & 2 buttons to suit the
           lightgun setup.  If it doesn't work you probably need to delete the .cfg file
           for that game.
	
	src/input.h
	src/windows/input.c
	src/windows/input.h
	src/windows/window.c

Removed '(hack?) from descriptions of newly added raizing games, stickers on the boards
seem to indicate they are genuine Korean release versions.

Improved ms32 sound a bit [Tomasz Slanina]
	src/drivers/ms32.c

Removed 8.3 ROM Filename Limitation [Olivier Galibert]
	src/mame.c

Removed neomame / cpmame

Various Seattle + Related Updates [Aaron Giles]
	MIPS3 core:
	* fixed handling of condition codes so they are stored in CCR31 now
	* fixed handling of CCR31 so that rounding modes are set correctly (in the
	DRC only)

	Seattle:
	* added support for Biofreaks
	* fixed remaining issues in California Speed
	* verified California Speed HDD dump (works in self test, not in boot ROM
	test)
	* fixed timer change that broke Blitz 2000
	* added proper PIC IDs for Wayne Gretzky and Mace

	IDE controller:
	* fixed final sector handling for Gauntlet: Dark Legacy

	Midway IC:
	* added sound auto acknowledgement option
	* added new PIC mapping for Gauntlet: Dark Legacy
	* fixed initial sound IRQ state

	DCS:
	* fixed reporting of input full/output empty states

	Voodoo:
	* added initial Voodoo 2 support
	* removed hack to toss what I thought were bogus triangles
	* improved W buffer handling
	* improved accuracy of depth buffer handling

PSX core Improvements [smf]
	* Fixed MVMVA disassembly
	* SWC bit in SR is ignored
	* Fixed MVMVA with LM bit set
	* Added SQR & CC opcodes
	* Added preliminary looped DMA detection
	* Improved root counter emulation
	* Improved SIO emulation
	* Preliminary SPU emulation
	* Preliminary screen flipping / blanking support
	* Added GPU packet 41, 52 & 77
	* Supports uploading 1024 pixel wide textures
	* Resetting GPU sets default texture window
	src\cpu\mips\mipsdasm.c
	src\cpu\mips\psx.c
	src\machine\psx.c
	src\includes\psx.h
	src\sound\psx.c
	src\sound\psx.h
	src\vidhrdw\psx.c

GP-13 board layout [Brian A. Troha]
	src\drivers\namcos11.c

NCR 53CF96-2 SCSI controller emulation [R.Belmont]
	src\machine\am53cf96.c
	src\machine\am53cf96.h

Added ADPCM and fixed jerky music in Pass [Tomasz Slanina]
	src/drivers/pass.c

Fixed the colors in Hana Oriduru [Luca Elia]

ROM patch to fix sound cpu communication in Gundam Psycho Salamander [Tomasz Slanina]
	still need a real fix ..
	src/drivers/dcon.c

Improvements to Speed Attack Colours [Pierpaolo Prazzoli]

Misc Fixes / Tilemap conversions [Curt Coder]
	(prehisle, psychic5, tunhunt + more)

More MS32 Sound Improvements [Tomasz Slanina]

-------------------------------------------------------------
## New Games supported or promoted from GAME_NOT_WORKING status:


Bonanza Bros [Olivier Galibert]
Quiz Rouka Ni Tattenasai [Olivier Galibert, David Haywood]
Dynamic Country Club [Olivier Galibert]
	src/drivers/system24.c

Biofreaks (Prototype) [Aaron Giles]
California Speed [Aaron Giles]
	src/drivers/seattle.c

Crypt Killer [R.Belmont, smf]
	src/drivers/konamigq.c

Tekken 3 [smf]
Ehrgeiz
Fighting Layer
Mr Driller
Aquarush
	src/drivers/namcos12.c

Super Tank [Norbert Kehrer]
	src/drivers/supertnk.c

Ashita no Joe [David Haywood, Pierpaolo Prazzoli]
	sound problems
	src/drivers/ashnojoe.c
	src/vidhrdw/ashnojoe.c

Mahjong Campus Hunting [Luca Elia]
7jigen no Youseitachi [Luca Elia]
Mahjong Yarunara [Luca Elia]
Mahjong Angels [Luca Elia]
Quiz TV Gassyuukoku [Luca Elia]
	src/drivers/dynax.c
	src/vidhrdw/dynax.c
	src/includes/dynax.h

Borderline (bootleg) [David Haywood]
	no sound
	src/drivers/vicdual.c

Cross Pang [Pierpaolo Prazzoli]
	no sound, some sprite glitches, will try to clean up for u1
	src/drivers/crospang.c

Funny Bubble [David Haywood]
	sound banking problem, some sprite glitches, will try to clean up for u1
	src/drivers/funybubl.c

New Clones supported or promoted from GAME_NOT_WORKING status:
--------------------------------------------------------------

Lode Runner III - The Golden Labyrinth [Pierpaolo Prazzoli]
Fighting Hawk (World) [Fabrice Arzeno]
Chinese Hero (older) [Stefan Jokisch]
Delta Race (omega Race bootleg) 
Vs. Atari R.B.I. Baseball (set 2) [Pierpaolo Prazzoli]
Premier Soccer (EAB) [Taucher]
Pairs V1.2 [Taucher]


----------------------------------------
## New Non-Working games / clones supported

Scramble Spirits [Olivier Galibert]
Super Masters Golf [Olivier Galibert]
Quiz Syukudai wo Wasuremashita [Olivier Galibert]
Gain Ground [Olivier Galibert]
Crackdown [Olivier Galibert]
	src/drivers/system24.c
	all encrypted

Mr Driller 2 [smf]
	src/drivers/namcos10.c

Star Gladiator [smf]
Battle Arena Toshinden 2 (USA)
Strider 2 (USA)
Strider 2 (ASIA)
Gallop Racer 2 (USA)
Brave Blade
Beastorizer
Primal Rage 2
NBA Jam Extreme
Judge Dredd (Rev C)
Judge Dredd (Rev B)
	src/drivers/zn.c

Ace Driver [Guru]
	src/drivers/namcos22.c

Champion Baseball Japan set 2 [Stefan Jokisch]

Target Balls [David Haywood]
	not looked at it much yet, seems to be paradise.c

New and/or Recommended Input Options and Settings
--------------------------------------------------------------

Added 'X-Way Joystick' Option to Analog Controls Menu only for IPT_DIAL
and IPT_DIAL_V devices.  This option is for use with x-way rotary joysticks
(where the joystick can rotate X discrete steps, 8 or 12 appears to be most
common).  The joystick can still move up, down, left, right and diagonally.
The most famous game to use this control scheme is Ikari Warriors.
Notes on this option:
    * When on, prevents the character from rotating two discrete steps
    sometimes when the rotary joystick is just rotated one step.
    * When on, the character will only rotate up to half the frame rate
    instead of the frame rate, so up to 30 times per second instead of 60
    times a second if the game is running at 60Hz.

Recommended analog control settings for known games that use an x-way rotary
joystick to give the intended rotary control of the character. (Note: Settings
will work best if the game is running at its native 60Hz, and the time
the rotary joystick "holds" down the virtual IPT_DIAL or IPT_DIAL_V button
is longer than 1/60th of a second, but less than 2/60ths of a second.):
    * Bermuda Triangle
        - Key/Joy Speed: 16
        - Sensitivity: 100%
        - X-Way Joystick: On
    * Caliber 50
        - Key/Joy Speed: 4
        - Sensitivity: 100%
        - X-Way Joystick: On
    * Downtown
        - Key/Joy Speed: 22
        - Sensitivity: 100%
        - X-Way Joystick: On
    * Guerrilla War
        - Key/Joy Speed: 16
        - Sensitivity: 100%
        - X-Way Joystick: On
    * Heavy Barrel
        - Key/Joy Speed: 21
        - Sensitivity: 100%
        - X-Way Joystick: On
    * Ikari Warriors
        - Key/Joy Speed: 16
        - Sensitivity: 100%
        - X-Way Joystick: On
    * Victory Road (kind of Ikari Warriors II)
        - Key/Joy Speed: 16
        - Sensitivity: 100%
        - X-Way Joystick: On
    * Ikari III - The Rescue
        - Key/Joy Speed: 21
        - Sensitivity: 100%
        - X-Way Joystick: On
    * Midnight Resistance
        - Key/Joy Speed: 21
        - Sensitivity: 100%
        - X-Way Joystick: On
    * SAR - Search and Rescue
        - Key/Joy Speed: 21
        - Sensitivity: 100%
        - X-Way Joystick: On
    * Time Soldiers
        - Key/Joy Speed: 21
        - Sensitivity: 100%
        - X-Way Joystick: On
    * Touchdown Fever
        - Key/Joy Speed: 2
        - Sensitivity: 100%
        - X-Way Joystick: On
    * Touchdown Fever 2
        - Key/Joy Speed: 2
        - Sensitivity: 100%
        - X-Way Joystick: On

# 0.77u3
------

## General Source Changes


Added SHA1 Information to remaining drivers

Seattle (and related) Improvements [Aaron Giles]
	* added ADSP2104 variant to the ADSP2100 core
	* fixed ROM loading code in the ADSP2100 core
	* fixed several minor bugs that affected Biofreaks and California Speed
	* included several DIP switch fixes from Brian Troha
	* fixed input ports for California Speed and SF Rush
	* increased default visible area to handle Biofreaks
	* added speedups to Blitz and Blitz 2000
	* made sure the IDE controller waits a minimum amount of time before
	generating an interrupt
	* the IDE controller features buffer is now filled in completely
	* fixed code that reads the bus master status register from a word offset
	* tweaked the Midway PIC handling so that Biofreaks can read its serial
	number
	* changed the mechanism for DCS to report buffer full/empty states
	* added IOASIC support for buffer full/buffer empty interrupts from DCS
	* added preliminary DCS support for ADSP-2104-based sound
	* added code to clip the Voodoo vertexes to 1/16th of a pixel increments
	* fixed writes to the palette RAM on the Voodoo
	* fixed small errors in the Voodoo rendering when bilinear filtering
	* fixed handling of local alpha blending on the Voodoo

STV Improvements [Angelo Salese. Mariusz Wojcieszek]

Attempted to improve quizpani sound banking [Pierpaolo Prazzoli]

Snes emulation improvements from MESS (used for NSS) [Nathan Woods]
	src/drivers/nss.c
	src/includes/snes.h
	src/machine/snes.c
	src/sndhrdw/snes.c
	src/vidhrdw/snes.c
	    - Misc changes and improvements from the SNES driver in MESS

Fixed Uncio sound frequency with values taken from real pcb [El Condor]

YMF271 sound core plus very preliminary hook up in ms32.c (only Desert War makes
any sound at all and it doesn't really seem to be anytihng like correct, this is
probably due to a combination of z80 communication and banking issues) [R.Belmont]

Made sure each bitmap is aligned on a 16-bit boundary [Brad Oliver]
	src/common.c

Updated Vball driver to use graphics extracted from origianl PCB [Jarek Burczynski]

Fixed viofight to only have one OKI [Jarek Burczynski]
	- fixed adpcm sound in viofight: now there's is only one OKI M6295 chip
	- corrected Z80 clock in viofight
	- corrected viofight rom names
	- added some info about PALs: viofight, nastar


Misc Fixes for MESS [Nathan Woods]
	src/common.c:
	src/common.h:
	- Added auto_strdup(), just an auto_malloc'd version of strdup()

	src/ui_text.c:
	src/ui_text.h:
	src/mame.c:
	- Cleanups in UI string support, primarily to more easily
	support MESS specific strings without modifying MAME files.  Also
	removed uistring_shutdown(), which is made unnecessary by auto_strdup()

	src/osdepend.h
	src/cpu/z80/z80.c
	src/windows/config.c
	- Adds osd_die() function; a friendlier way to die in some
	unexpected fatal way than exit(-1) or [shudder] raise(SIGABRT)

	src/harddisk.h
	src/chd.h
	- Adds #ifndef <headername>_H protection

	src/windows/config.c
	- Adds minor code within #ifdef MESS


TMS9900 inline asm update [Brad Oliver]
	The syntax for the inline ppc assembly in the tms9900 core has changed 
	slightly

Hooked up Backgrounds in Miss Bubble 2 [Angelo Salese]

SCSP updates [Elsemi, R.Belmont]

Path for Solaris [Lawrence Gold]
	Attached is a patch for Solaris that replaces uses of sqrtf and floorf,
	which aren't available on that platform, with their double-precision
	counterparts.  Please let me know if this is a problem, in which case I
	can add some special-case #defines for Solaris

Changed Liberator to use Proms, proper PROMS are not dumped however [Stefan Jokisch]

Improved Namco Sound core {BUT]

-------------------------------------------------------------
## New Games supported or promoted from GAME_NOT_WORKING status:


Gals Hustler [Pierpaolo Prazzoli, David Haywood]
	src/drivers/galpanic.c

Hot Rod [GreyRogue, Olivier Galibert]
	** you must init the default settings or it won't work properly **

Monkey Mole Panic [Luca Elia]


New Clones supported or promoted from GAME_NOT_WORKING status:
--------------------------------------------------------------

Street Fighter the Movie (1.11) [Brian Troha]

Sorcer Striker (hack?) [Brian Troha]
Kingdom Grand Prix (hack?) [Brian Troha]
	difficult to verify if these are genuine

Power Instinct (USA alt bootleg) [Pierpaolo Prazzoli]
	sound not yet working

Roller Aces (set 2) [Pierpaolo Prazzoli]



----------------------------------------
## New Non-Working games / clones supported

Malzak [Barry Rodewald]
	collisions don't work
	src/drivers/malzak.c
	src/vidhrdw/malzak.c

Ashita no Joe [David Haywood]
	incomplete set

# 0.77u2
------

make sure you do a CLEAN build or this won't work

## General Source Changes


CHDman updates, with added checks to make sure source chd's aren't corrupt
when doing updates etc. [Aaron Giles]
	src/chdman.c

Fixed Grand Striker 2 dipswitches [Pierpaolo Prazzoli]
	src/drivers/metro.c

Found the way the OTIR/INIR instructions affect the PF flag and removed
the look-up tables [Ramsoft, Sean Young]
	src/cpu/z80/z80.c

Fixed a few v60 floating point opcodes [Olivier Galibert]

Merged taito_x.c and seta.c video hardware, this is correct but due to some
differences in the drivers a few glitches have been introduced in twinhawk
that will need investigating [David Haywood]

Mess Related Updates [Nathan Woods]
	src/windows/fileio.c:
	- Merged in some #ifdef MESS'd code

Improved bg gfxdecode in missb2, but the backgrounds still haven't been
hooked up yet [David Haywood]
	src/drivers/missb2.c

Improved / Fixed Ym2610 save states [Leon van Rooij]
	src/sound/fm.c

SCSP fixes (improves music in some games a little) [ElSemi / R.Belmont]
	src/sound/scsp.c

Improved Mysterious Stone driver based on schematics [Curt Coder]
	src/drivers/mystston.c
	src/vidhrdw/mystston.c

Improved Raiden drum volume Balance [R.Belmont]
	src/drivers/raiden.c
	src/sndhrdw/seibu.h

-------------------------------------------------------------
## New Games supported or promoted from GAME_NOT_WORKING status:


Gundam Ex Revue [Pierpaolo Prazzoli, David Haywood]
	src/drivers/seta2.c
	src/vidhrdw/seta2.c

Runaway (prototype) [Stefan Jokisch]
	src/drivers/centiped.c
	src/vidhrdw/runaway.c
	src/drivers/runaway.c
	src/vidhrdw/runaway.c

NFL Blitz [Aaron Giles]
NFL Blitz 2000 [Aaron Giles]
	src/drivers/seattle.c

Kyuukyoku no Striker / Last Striker [Luca Elia]
	src/drivers/taito_x.c

MegaTech : Tournament Golf [David Haywood]
	src/drivers/segac2.c

Nettoh Quiz Champion [Luca Elia]
	src/drivers/ddenlovr.c

New Clones supported or promoted from GAME_NOT_WORKING status:
--------------------------------------------------------------

Capitol (clone of pleiads) [Pierpaolo Prazzoli]
	src/drivers/phoenix.c

Thunder ceptor [BUT]
	src/drivers/tceptor.c
	src/vidhrdw/tceptor.c


----------------------------------------
## New Non-Working games / clones supported

Rabbit
Tokimeki Mahjong Paradise - Dear My Love
	these look tricky, blitter based gfx / sound chip, maybe i'll come back
	to them at a later date
	src/drivers/rabbit.c

BioFreaks [Aaron Giles]
	missing hd image
	src/drivers/seattle.c

Jackie Chan [David Haywood]
	protection? gfx decode not worked out either, does nothing, may look at
	it again one day but probably not, no work has been done on it for about
	a year.
	src/drivers/jchan.c


# 0.77u1 (private)

note, credit for the Z80 DAA emulation improvement in the previous release should
have been given to 'Ramsoft' ( http://www.ramsoft.bbk.org/ )

## General Source Changes


New CHD format and management tools [Aaron Giles]
	hdcomp is now chdman
	old chd files are _not_ compatible, they will need to be updated
	using chdman -update oldchd.chd newchd.chd
	CHD now stands for 'Compressed Hunks of Data' not 'Compressed
	Hard Drive' as the format is more flexible.

Misc Changes [Nathan Woods]
	src/drivers/sengokmj.c:
	src/drivers/stv.c:
	- Minor cleanups (specifically changed some variables to static)

	src/machine/6522via.c:
	src/vidhrdw/tms9928a.c:
	- Fixes and updates from MESS

Updated a couple of drivers with new Hard Drive SHA1s [Roman Scherzer]
	src/drivers/cojag.c
	src/drivers/djmain.c
	src/drivers/seattle.c

Fixed sound in tpgolf [Leon van Rooij]
	src/drivers/neogeo.c

Various other changes (dip fixes, tilemap conversions etc.) [various people]


-------------------------------------------------------------
## New Games supported or promoted from GAME_NOT_WORKING status:


Angel Kiss [David Haywood]
	no sound
	src/drivers/ms32.c

Grand Striker 2 [David Haywood]
	priority glitch inbetween round
	src/drivers/metro.c
	src/vidhrdw/metro.c

Hatch Catch [David Haywood]
	src/drivers/tumblep.c
	src/vidhrdw/tumblep.c

Cookie & Bibi 2 [David Haywood]
	src/drivers/snowbros.c
	src/vidhrdw/snowbros.c

Quiz Panicuru Fantasy [Pierpaolo Prazzoli]
	incorrect sample banking
	src/drivers/quizpani.c
	src/vidhrdw/quizpani.c

Video Pinball [Stefan Jokisch]
	driver was rewritten, it works now
	src/drivers/videopin.c
	src/vidhrdw/videopin.c


----------------------------------------
## New Non-Working games / clones supported

BC Story (SemiCom)
	protection (missing 0x200 bytes of program code)
	
0.77

## General Source Changes


Fix to allow a driver to have MSM6295s and plain ADPCM voices coexist
(required for gcpinbal).  [R.Belmont]
	src/sound/adpcm.c

Full Sound / Music in Grand Cross [R.Belmont]
	src/drivers/gcpinbal.c


Improved Bishi Bashi colours [Angelo Salese]
	src/drivers/bishi.c
	src/palette.c
	src/palette.h

Fixed ckongs sprites [Pierpaolo Prazzoli]

Updated Double Dragon Driver [Bryan McPhail]
	- Correct video & interrupt timing derived from Xain schematics
	and confirmed on real DD board.

	- Corrected interrupt handling, epecially to MCU (but one
	semi-hack remains).

	- TStrike now boots but sprites don't appear (I had them working
	at one point, can't remember what broke them again).

	- Dangerous Dungeons fixed.

	- World version of Double Dragon added (actually same roms as
	the bootleg, but confirmed from real board)

	- Removed stereo audio flag (still on Toffy - does it have it?)

	src/drivers/ddragon.c

Various ST-V improvements [Angelo Salese]
	src/drivers/stv.c

Twin16/TMNT fixes [Curt Coder]
	drivers/twin16.c

	- Fixed cuebrick NVRAM handler
	- Added watchdog reset

	drivers/tmnt.c

	- Fixed mia/mia2 inputs/dips
	- Fixed visible area in cuebrckj/mia/mia2
	- Changed cuebrckj to "Cue Brick (World version D)", since text
	  is in English)

TMS32031 Fixes [Aaron Giles]
	many fixes to the TMS32031 core; Offroad Challenge is almost playable as a
	result, minus the fact that the clipping plane seems to like clipping out
	the cars during the race. But the terrain problems are fixed at least.

Memory Annoyance Fix [Aaron Giles]
	Unmapped memory accesses always reported the raw byte address, not the
	CPU-level address. This is annoying for chips like the TMS34010 or the
	TMS32031, so now the effecitve offset is logged instead, taking into account
	the address shift.

Some Input Port Corrections [Robin Merrill, fixed by Bryan McPhail]

OSD Fix [Aaron Giles]
	This is a minor fix to enable the OSD for non-debug builds regardless of
	the -debug flag

Misc Fixes [Nathan Woods]
	src/usrintrf.c:
	- Adds a handful of comments to uifontdata[]
	- Changes character 14 to a solid circle; previously this
	character was blank and unused
	- Miscellaneous changes within #ifdef MESS

Various Atari Improvements
	* CAGE works
	* motion object checksums work for growth games
	* sprite/playfield priorities work for growth games
	* no more 6502 speedups


	drivers/atarig1.c:
	* added support for the MO command register -- MO checksums now work
	* added missing SHA1 values and marked some ROMs as NO_DUMP
	* removed 6502 speedups

	drivers/atarig42.c:
	vidhrdw/atarig42.c:
	* added support for the MO command register -- MO checksums now work
	* added SLOOP emulation for Guardians of the Hood, which is fully playable
	* added missing SHA1 values
	* removed 6502 speedups
	* documented color MUXing
	* removed old orientation code
	* added MO/playfield priorities

	drivers/atarigt.c:
	vidhrdw/atarigt.c:
	* added support for the MO command register -- MO checksums now work
	* hooked up CAGE audio
	* added missing SHA1 values
	* documented the overly complicated color MUXing
	* added MO/playfield priorities

	drivers/atarigx2.c:
	* added support for the MO command register -- MO checksums now work
	* added missing SHA1 values
	* added another Road Riot's Revenge variant
	* some input port tweaks
	* removed 6502 speedups

	machine/asic65.c:
	* improved logic for Road Riot 4WD -- almost playable
	* added support for Guardians of the Hood

	machine/atarigen.c:
	* fixed bad handling of EEPROM data in 32-bit mode

	sndhrdw/cage.c:
	* fixed frequency computation -- it now works ok!
	* added speedup mechanism
	* fixed positioning of the 4 voices

	vidhrdw/atarirle.c:
	* implemented the MO command register and checksumming

Improved Z80 DAA emulation allowing removal of look-up table [Sean Young / Stefano]


-------------------------------------------------------------
## New Games supported or promoted from GAME_NOT_WORKING status:


Seattle Hardware [Aaron Giles]
	Mace - The Dark Age
	Blitz '99
	CarnEvil
	Wayne Gretzky's 3D Hockey

	src/drivers/seattle.c
	src/vidhrdw/voodoo.c
	+many others

Dangerous Dungeons [Bryan McPhail]
	src/drivers/ddragon.c

Goal '92 (bootleg of Seibu Cup Soccer) [Pierpaolo Prazzoli]
	src/drivers/goal92.c
	src/vidhrdw/goal92.c

T-Mek (prototype) [Aaron Giles]
	bad sound

Guardians of the Hood [Aaron Giles]

New Clones supported or promoted from GAME_NOT_WORKING status:
--------------------------------------------------------------

Double Dragon (World) [Bryan McPhail]

The Simpsons (2 player, alt) [R.Belmont]

Martial Champion [EAA] [R.Belmont]


----------------------------------------
## New Non-Working games / clones supported

Namco System 12 [smf]
	(sets include tekken3, soulclbr, ehrgeiz, mdhorse, fgtlayer,
         pacapp, sws99, tekkentt, mrdrillr, aquarush, golgo13)
	src/drivers/namcos12.c

Godzilla [Pierpaolo Prazzoli]
Bonk's Adventure [Pierpaolo Prazzoli]
Seibu Cup Soccer [Pierpaolo Prazzoli]
	protection issues
	src/drivers/leigonna.c

SF Rush [Aaron Giles]
California Speed [Aaron Giles]
	src/drivers/seattle.c

Sports Fishing 2 (stv) [ANY]
	incomplete dump / no way to support CD and MPEG decoder
	src/drivers/stv.c
	src/machine/stvcd.c

# 0.76u2

## General Source Changes


Rewrote rf5c68 sound core [Olivier Galibert]
	src/sound/rf5c68.c
	src/sound/rf5c68.h

Misc Changes [Nathan Woods]
	src/fileio.c:
	- Integrates some MESS specific fileio functionality (within
	#ifdef MESS)

	src/sndintrf.c:
	- Removing the update procedure in a MESS specific sound core

Some Hyperstone Fixes / Improvements [Mooglyguy, Pierpaolo Prazzoli]
	src/cpu/e132xs/32xsdasm.c
	src/cpu/e132xs/e132xs.c
	src/cpu/e132xs/e132xs.h
	src/drivers/vamphalf.c

Fixed SCSP crash with -nosound [R.Belmont]
	src/sound/scsp.c

Attempted to fix some hangs in the Soul Edge clones [smf]
	src/machine/psx.c

Added new function: activecpu_eat_cycles(cycles), needed for idle skipping in
certain situations [Aaron Giles]
	src/cpuexec.c
	src/cpuexec.h

Midway V-Unit Update [Aaron Giles]
	* WarGods sound is 100% now
	* added some pre-initialization of the WarGods NVRAM
	* added speedup handlers for all games (it makes a little difference)
	* revamped the DCS2 handling to support stereo output
	* fixed a number of synchronization issues in the DCS2 handling
	* implemented a few missing features in the Midway I/O ASIC
	src/sound/dcs.c
	src/sound/dcs.h
	src/drivers/midvunit.c
	src/machine/midwayic.c
	src/machine/midwayic.h

Cpu Fixes / Tweaks [Aaron Giles]
	ADSP2100:
	* fixed loading of 0 counters so they don't execute infinitely
	* fixed initial state of the SSTAT register

	MIPS3 (R4000/R5000):
	* fixed timer handling so we don't set a timer too far in advance
	* changed timer handling so that the default Compare value doesn't trigger
	timers
	* fixed cycle chomping to make sure we don't go over an interrupt boundary
	* added correct cycle times for MULT/DIV instructions
	* fixed display of the Count reigster in the debugger

	MIPS3DRC:
	* same fixes as above, plus:
	* fixed FP rounding glitches
	* now actually respecting the global FP rounding mode

	R3000:
	* fixed instruction timings for MULT/DIV

	X86DRC:
	* added better support for tweaking the FP rounding modes

	src/cpu/adsp2100/2100ops.c
	src/cpu/adsp2100/adsp2100.c
	src/cpu/mips/mips3.c
	src/cpu/mips/mips3drc.c
	src/cpu/mips/r3000.c
	src/x86drc.c
	src/x86drc.h

MagMax sound improvements (Emulated some op.amps and RC filter circuits giving
more exact results) [Jarek Burczynski]
	src/drivers/magmax.c

Fixes to the ymdeltat module. Generally - even more flags work correctly now. These changes are
necessary for the msx sound emulation (Y8950) and were tested in nlmsx emulator by Frits
Hilderink. [Jarek Burczynski]
	Look at the top of each *.c file for a detailed list of changes.
	src/sound/fm.c
	src/sound/fm.h
	src/sound/fmopl.c
	src/sound/fmopl.h
	src/sound/ymdeltat.c
	src/sound/ymdeltat.h

Improved Sound in Cabal / Dead Angle by adding ADPCM sounds [R.Belmont, Jarek Burczynski]
	src/drivers/cabal.c
	src/drivers/deadang.c
	src/sndhrdw/seibu.c
	src/sndhrdw/seibu.h

Added OKI6295 sounds in Grand Cross [R.Belmont]
	src/drivers/gcpinbal.c

Asterock Dipswitches [Pierpaolo Prazzoli]
	src/drivers/asteroid.c

Fixed Aspect Ratio calculationbug on some rotated games
	(galspnbl for example) [S�bastien Volpe]
	src/common.c
	src/windows/wind3d.c
	src/windows/winddraw.c
	src/windows/window.c

Added Sample SUpport rto Armor Attack and Sundance [Tim Cottrill]
	src/drivers/cinemat.c
	src/sndhrdw/sinemat.c
	src/includes/cinemat.h

Decoded BG graphics in Thunder Ceptor II but BG's still aren't hooked up yet,
improved existing graphics a bit
	[David Haywood, BUT]
	src/drivers/tceptor.c
	src/vidhrdw/tceptor.c

Fixed Dragon Unit Dipswitches [I�igo Luja]
	src/drivers/seta.c

Some twin16.c / tmnt.c cleanups, fg layer converted to tilemaps [Curt Coder]

Some input fixes [HowardC]
	atetris.c
	neogeo.c
	+others

Fixed Xain'd Sleena hang? [David Haywood]
	src/drivers/xain.c

-------------------------------------------------------------
## New Games supported or promoted from GAME_NOT_WORKING status:


Several ST-V games are probably now playable, however be warned many are very very
slow, this is not a bug, sound is still poor or non-existant in most titles, these
haven't been tested for long so could easily fail at later levels.  Games which
were marked as working in previous updates are also improved (shienryu etc.)
	[David Haywood, Angelo Salese, Olivier Galibert]

	Puyo Puyo Sun - some priority glitches, otherwise looks good
	Mausuke Ojama the World - mostly ok
	Ejihon Tantei Jimusyo - missing alpha effect on lens
	Virtua Fighter Kids - bad sprite colours, missing ground effects
	Columns 97 - miscoloured sprites, background not correctly scaled
	Cotton 2 - missing ground in places
	Cotton Boomerang - missing ground in places
	Die Hard Arcade / Dynamite Deka - missing floors / ceilings

	there might be others too, a few can certainly be watched.  this driver still
	needs a lot of work, I don't expect it to be completed soon due to the
	incredible complexity of this hardware.

Sega Dual Space Attack / Head On Dual board [Stefan Jokisch]
	src/drivers/vicdual.c



----------------------------------------
## New Non-Working games / clones supported
	
Quiz Panicuru Fantasy [David Haywood]
	haven't found any time for this in the last 6 months so included it anyway
	so it doesn't get lost, if anybody wants to finish it they're welcome
	src/drivers/quizpani.c

# 0.76u1

## General Source Changes


Ported ElSemi's SCSP Core, this gives preliminary sound in the ST-V games, however at
the moment most of them play static, Shienryu is mostly correct [R.Belmont]
	src/sound/scsp.c
	src/sound/scsp.h
	src/sound/scsplfo.c
	src/drivers/stv.c

More ST-V Improvements, fixing various graphical issues (Shienryu Sprite Colours) and
general other bits and pieces.  More games are 'watchable' now, for example Cotton 2,
Cotton Boomerang, Puyo Puyo Sun, Columns 97 however they lack controls [David Haywood]
	src/drivers/stv.c
	src/vidhrdw/stvvdp1.c
	src/vidhrdw/stvvdp2.c

Some Atari improvements with the new sets (old speed hacks removed that aren't really
needed anymore, both screens being displayed in Cyberball) [Aaron Giles]
	src/ -various, too many to list

Misc Changes [Nathan Woods]
	src/sndintrf.c:
	- Removed wave_sh_stop() call from the MESS specific WAVE sound
	interface (it is extraneous now)

	src/x86drc.c:
	src/x86drc.h:
	- Added a few macros for other miscellaneous X86 opcodes
	- Added a drc_dasm() call, a debug call that uses MAME's I386
	disassembler to disassemble DRC'd code
	(didn't compile, commented out until fixed)

	src/mame.h:
	src/windows/config.c:
	- Added 'GAMENOUN' and 'GAMESNOUN' macros; defined to be the
	human readable text for the words 'game' and 'games' respectively (this
	is so MESS can use the nouns 'system' and 'systems')

v60 Improvements (LDTASK, STTASK, ABSF and some missing \n.) [Olivier Galibert]
	src/cpu/v60/*

Added Sound and Improved Background emulation in HeliFire [Jarek Burczynski]
	src/drivers/8080bw.c
	src/sndhrdw/8080bw.c
	src/vidhrdw/8080bw.c

Misc Windows OSD updates and D3D fixes [Leon van Rooij]
	makefile:
	windows\windows.mak:
	 Added CFLAGSOSDEPEND variable. It is used when compiling osd code only
	(e.g. DirectX needs -Wno-strict-aliasing).
	 COMPILESYSTEM_CYGWIN determines if nasm or nasmw is used (the cygwin
	version of nasm is called nasm)

	windows\fileio.c:
	 Quick fix for some #defines in that don't work with win32api 2.4

	windows\snprintf.c:
	 Changed a variable name that conflicts with a gcc built-in

	windows\blit.c:
	windows\wind3d.c:
	 Fixed a bug that affected stv games when flipping/rotating the image

	windows\wind3d.c:
	windows\window.c:
	windows\window.h:
 	window size/prescale/rgb-auto effect can now change when the image size is
	changed by the game driver

SegaPCM fixes [Olivier Galibert]
	Fixes the pitch (the 15800Hz value was wrong, it should have been the
	main 4Mhz clock divived by 256) and also makes the code ultra-paranoid
	when it comes to out-of-range accesses in the sample roms.  Power
	Drift's sound code sometimes puts semirandom values for the first
	handful of keyons on song transitions.
	src/sound/segapcm.c

Added Proper Inits to term2la1 [Brian Troha]

Merged Zaxxon and Congo drivesr [CUrt Coder]

Reversed harddisk.c back to 0.73 status

-------------------------------------------------------------
## New Games supported or promoted from GAME_NOT_WORKING status:


Baku Baku Animal [David Haywood]
	still a few gfx glitches and no sound
	src/drivers/stv.c
	src/vidhrdw/stvvdp1.c
	src/vidhrdw/stvvdp2.c

Tekken [smf]
Tekken 2 [smf]
Soul Edge [smf]
	(Soul Edge Ver II. doesn't work)
Xevious 3D/G (XV31/VER.A) [smf]
Prime Goal EX (PG1/VER.A) [smf]
	none of the above 5 have sound
	src/drivers/namcos11.c
	+ various PSX support files

Super Speed Race [Stefan Jokisch]
	no sound
	src/drivers/sspeedr.c
	src/vidhrdw/sspeedr.c

Cosmo [Jarek Burczynski]
	no sound
	src/drivers/8080bw.c
	src/vidhrdw/8080bw.c

Thunder Ceptor II [BUT]
	missing backgrounds
	src/drivers/tceptor.c
	src/vidhrdw/tceptor.c

New Clones supported or promoted from GAME_NOT_WORKING status:
--------------------------------------------------------------

100 Atari clones [Aaron Giles]
	* 2 new Road Runner variants
	* 8 new Road Blasters variants
	* 2 new Paperboy variants
	* 5 new 720' variants
	* 6 new Super Sprint variants
	* 7 new Championship Sprint variants
	* 7 new APB variants
	* 2 new Blasteroids sets
	* 1 new Cyberball set
	* 3 new 2-player Cyberball 2072 sets
	* 1 new Tournament Cyberball 2072 set
	* 3 new Skull & Crossbones sets
	* 3 new Toobin' sets
	* 6 new Vindicators sets
	* 4 new Xybots sets
	* 10 new Hard Drivin' versions
	* 3 new Hard Drivin' Compact versions
	* 9 new STUN Runner versions
	* 9 new Race Drivin' versions
	* 7 new Race Drivin' Compact versions
	* 2 new Steel Talons versions

Superior Soliders (US) (parent of psoldier) [Chris Hardy]
	src/drivers/m92.c

Asterock (Sidam bootleg of asteroid) [Pierpaolo Prazzoli]
	src/drivers/asteroid.c

Sky Raiders (bootleg of uniwars) [Pierpaolo Prazzoli]
	src/drivers/galaxian.c


# 0.76

It appears no new timer system related bugs were found with 0.75u1 so *hopefully* this
release will be better than the last, 0.75 ended up not being not too great due to the
problems with the YM2151 sound etc.

## General Source Changes


Various ST-V and SH-2 improvements, shienryu is improved as are many other games but
no more are really playable yet (although I have changed Shienryu to IMPERFECT_GFX
instead of NOT_WORKING as it seems ok, just the sprites are black)
	[David Haywood, Olivier Galibert, sthief, Angelo Salese]
	src/drivers/stv.c
	src/vidhrdw/stvvdp1.c
	src/vidhrdw/stvvdp2.c
	src/cpu/sh2/sh2.c

Added Insector sound rom loading [Fabrice Frances]
	src/drivers/gottlieb.c

Changed Jumping Cross default dipswitches
Fixed invalid dipswitch in Big Event Golf
Readded a bit of code that was removed from blockage driver when it was converted to
use tilemap (blasto needed it)

-------------------------------------------------------------
## New Games supported or promoted from GAME_NOT_WORKING status:


VS. Super Xevious [Pierpaolo Prazzoli]
VS. Freedom Force [Pierpaolo Prazzoli]
	src/drivers/vsnes.c
	src/machine/vsnes.c

Bishi Bashi Championship Mini Game Senshuken [R. Belmont]
	some gfx problems
	src/drivers/bishi.c
	src/vidhrdw/bishi.c

New Clones supported or promoted from GAME_NOT_WORKING status:
--------------------------------------------------------------

Street Fighter II CE bootlegs (sf2yyc, sf2koryu, sf2m4 - sf2m7) [Barry Harris]
	src/drivers/cps1.c
	src/vidhrdw/cps1.c

A couple of Gauntlet, Gauntlet 2 and Vindicators Pt 2 clones [Aaron Giles]
	src/drivers/gauntlet.c

Q*Bert (early test) [Peter Rittwage]
Mad Planets (UK Taitel license) [Tom McClintock]
	src/drivers/gottlieb.c

# 0.75u1

Please TEST this release, I had hoped 0.75 would be the end of the timer related bugs
for good but it wasn't so if this tests out as ok then I'll probably release 0.76
within a fairly short period of time, if sound has vanished on any games, or anything
that booted in .75 no longer boots let us know, the sooner the better.


## General Source Changes


Attempted to fix remaining timer system related bugs [Aaron Giles]
	src/timer.c

Cleaned up SH-2 core a bit and added a possible kludge for Sol Divide's
Music [Olivier Galibert]
	src/cpu/sh2/sh2.c

Sound in Mogura Desse [Jarek Burczynski]
	src/drivers/mogura.c

Fixed default.cfg saving [BUT]

Dips in dbz and lethalj drivers [El Condor]

Fixed Speedup for r3000 version of Area 51 / Max. Force Duo [Aaron Giles]
	src/drivers/cojag.c

-------------------------------------------------------------
## New Games supported or promoted from GAME_NOT_WORKING status:


Gorkans [Tomasz Slanina]
	correct gfx roms are now loaded
	src/drivers/pacman.c



----------------------------------------
## New Non-Working games / clones supported

TryOut [Pierpaolo Prazzoli]
	src/drivers/tryout.c


# 0.75

## General Source Changes


TESTDRIVERS still enabled (see note for 0.74u2)

Fixed bug in the time accounting for the V60 which was causing Outrunners to run
with flickering sprites and slow music [Aaron Giles]
	src/cpu/v60/v60.c

Fixed another V60 bug introduced in 0.74u2 which broke the v70 games (and further
broke Outrunners) [Kobat, Olivier Galibert]
	src/cpu/v60/v60.c

Fixed some bugs introduced with the timer system changes a while back [Aaron Giles]
	Hopefully this is the last of them.... Fixes Exterminator and the Atari
	polygon games. Hopefully doesn't break anything in the process.

	src/cpuexec.c
	* changed scanline timing to use floor() to get consistent results
	* fixed bug in time computation if requesting a scanline in the next frame

	src/cpu/tms34010/tms34010.c
	* removed 4x executes per loop which screwed up timing
	* fixed internal interrupt generation to get consistent timing results

	src/timer.c
	* now uses the active callback as a base time when adjusting timers outside
	of a CPU context

	src/drivers/harddriv.c
	src/machine/harddriv.c
	* fixed all the games again, but they are slower now due to more necessary
	interleaving

Fixed Dorachan Orientation [Tomasz Slanina]
	src/drivers/dorachan.c
	src/vidhrdw/dorachan.c

Various Fixes [Nathan Woods]
	src/artwork.c:
	src/artwork.h:
	src/common.c:
	- Fixes a screenshot handling bug that affected Space Invaders

	src/cheat.c:
	- Moves the MESS specific code into a separate module, reducing
	#ifdef clutter

	src/harddisk.c:
	- Moved some variable declarations out of for loops so that it
	can compile in non-GCC compilers

	src/cpu/i86/instr86.c:
	- Fixes REP CMPSW so that it uses words and not bytes

	src/cpuintrf.c:
	src/cpuintrf.h:
	- Adds a call cpuintrf_set_dasm_override() that gives drivers
	the ability to customize disassemblies.  This probably isn't very useful
	for MAME, but it is useful for MESS for things like software interrupts
	used for BIOS calls.

	src/windows/config.c:
	- Increases the size of the buffer used when logerror() output
	goes to OutputDebugString()

	src/windows/config.c:
	src/windows/config.h:
	src/windows/winmain.c:
	- Moves config.c externs to a header file
	- Adds another call, cli_rc_create(), which creates MAME's rc
	struct for use elsewhere.  (I'm actually creating a debug time
	consistency check feature for MAME32, to verify at runtime that the
	arguments that MAME32 has are consistent with MAME's).

	src/rc.c:
	- When rc_destroy() is now called, freed pointers to strings and
	files will now be NULLed out.


Overhauled the Canyon Bomber driver with a number of minor fixes like correct
RAM size and VBLANK duration. [Stefan Jokisch]
	- hiscore reset button added
	- sprite positions should be fixed according to the schems
	- language ROM added (i.e. the language dip works now)
	- prototype set renamed from canbprot to canyonp

Improved Super Chase sample loading [sedn]
	src/drivers/superchs.c

Replaced instances of "inline", "static inline", and "static __inline" with
"INLINE".  This fixes some build problems with  xmame and makes things more
consistent. [Lawrence Gold]
	src/cpu/mips/mips.c
	src/vidhrdw/psx.c
	src/machine/psx.c
	src/drivers/namcos2.c
	src/machine/psx.c
	src/vidhrdw/psx.c

Corrected CPU speeds etc. in snowbros.c [The Guru]
	src/drivers/snowbros.c

Reversed inptport.c changes to 0.74u1 as the version in 0.74u2 wasn't correcly
merged, these will need resubmitting

Some namcos22 Improvements [Phil Stroffolino]
	The major things improved are:
	- fixes to display list processing (affects Prop Cycle ending)
	- gouraud shading
	- misc improvements for the not-working games
	src/drivers/namcos22.c
	src/vidhrdw/namcos22.c
	src/vidhrdw/namcos3d.h
	src/includes/namcos22.h
	src/includes/namcos3d.h



-------------------------------------------------------------
## New Games supported or promoted from GAME_NOT_WORKING status:


Lady Frog [Tomasz Slanina]
	old 'ladyfrog' set (not working) renamed to 'roldfrog' (Return Of Lady Frog)
	src/drivers/ladyfrog.c
	src/vidhrdw/ladyfrog.c

Sundance [Stefan Jokisch]
	controls now emulated
	src/drivers/cinemat.c

Dodgem [Mike Coates, Pierpaolo Prazzoli]
	no sound, the video hardware emulation was also rewritten for this driver, its
	quite slow now but more accurate.
	src/drivers/zac2650.c
	src/vidhrdw/zac2650.c

Main Event (SNK) [Tomasz Slanina]
	controls probably need work
	src/drivers/mainsnk.c
	src/vidhrdw/mainsnk.c


New Clones supported or promoted from GAME_NOT_WORKING status:
--------------------------------------------------------------

Lunar Battle (prototype, later) (clone of gravitar) [Aaron Giles]
	src/drivers/bwidow.c
Cloak & Dagger (International Versions, Spanish, French, German) [Aaron Giles]
	src/drivers/cloak.c
Dig Dug (newer Atari rev) [Aaron Giles]
	src/drivers/digdug.c
Tempest (rev ?) [Aaron Giles]
	src/drivers/tempest.c
Sub Hunter (clone of depthch) [Stefan Jokisch]
	src/drivers/vicdual.c
Route X (bootleg)
	src/drivers/route16.c


----------------------------------------
## New Non-Working games / clones supported

Task Force Harrier (Japan)
	src/drivers/nmk16.c
Various Sf2 bootlegs
	Please note, I have no intention of fixing these, however if you want
	to try and think you can do it in a clean way then feel free.
	src/drivers/cps1.c
	src/vidhrdw/cps1.c
Gorkans [Tomasz Slanina]
	gameplay works but there are no good gfx roms
	src/drivers/pacman.c
Whizz [Tomasz Slanina]
	main program rom appears to be bad, maybe a gfx rom too
	src/drivers/sidearms.c
	src/vidhrdw/sidearms.c
Marine Date [insideoutboy]
	collisions not emulated + various other issues
	src/drivers/marinedt.c


# 0.74u2

## General Source Changes


The TESTDRIVER system has been removed, all drivers are now enabled working or not,
this decision was made by several members of the development team.  The drivers
that were previously TESTDRIVERS are still marked as GAME_NOT_WORKING so can be
filtered out easily with a front-end.  Please note, the enabling of these drivers
does not indicate they're any closer to working than when they were first added
and in many cases it could still be several years before they work depending on
when they're worked on and how difficult they prove to be.  We're always looking
for talented programmers to work on and improve Mame, some of these drivers could
be a good place to start (others may be near impossible to fix)

If any GAME_NOT_WORKING flags have been missed let us know at Mametesters


Added MCU emulation to Tiger Heli [Tomasz Slanina]

Various SegaPCM fixes [Olivier Galibert]

Fixed some V60 Floating Point Operations and made core exit on unhandled opcodes,
note UltraX doesn't work again [Olivier Galibert]

Fixing missing continue graphics on actfancr [Bryan McPhail]

Fixed recently introduced tp84 video bug [David Haywood]

Fixed Config Saving Bug [BUT]

Various Dipswitch Fixes [I�igo Luja]


New games supported:
--------------------

Touchdown Fever II [Smitdogg]

Dorachan [Tomasz Slanina]
	no sound	

Jumping Cross [Tomasz Slanina]
	attract mode seems a bit wrong

Demoneye-X [Pierpaolo Prazzoli]
	gfx problems, no sound

Bakuretsu Breaker [David Haywood]
	sound problems

Suzuka 8 Hours [Dang Spot]

Moero Pro Yakyuu Homerun [Tomasz Slanina]
	missing voices

Galaxian / Space Invaders Test Roms [Mike O'Malley]
	worth supporting?


### New Clones supported:

Phantomas (clone of spectar) [Pierpaolo Prazzoli]
Donkey Kong 3 (bootleg on DKJr hw) [Pierpaolo Prazzoli]
Driving Force (Galaxian hw) [Pierpaolo Prazzoli]
Food Fight (rev 2 and cocktail) [Aaron Giles]
Depthcharge (rev 1) [MASH]
Mighty Monkey (original) [Pierpaolo Prazzoli]
Dynablaster (world) [Brian Troha]
Champion Golf (original?) [Brian Troha]


New Non-Working games supported
-------------------------------

(All drivers that were previously TESTDRIVERS)

Dealer [Pierpaolo Prazzoli]
	bad rom or bad decryption

# 0.74u1

## General Source Changes


Mame now recognises off-screen shots with the Act-Labs gun in a way which works
better with certain drivers (Area 51 etc,) [Bryan McPhail)

Fixed some problems with defaults in inptport.c [Robin Merrill]
	src/inptport.c

Various SH2 fixes improving Master/Slave support (this improves shienryu on st-v
but its not yet playable due to incomplete st-v emulation) [Olivier Galibert]
	src/cpu/sh2/sh2.c
	src/cpu/sh2/sh2.h

Fixed SegaPCM bug [Olivier Galibert]
	src/sound/segapcm.c

Moved Some Config Code around [Nathan Woods]
	src/config.c
	src/config.h
	src/inptport.c

Hopefully fixed crash some people were having with Nostradamus [David Haywood]
	src/vidhrdw/mcatadv.c

Added Paddle support to pop 'n' bounce (use -mouse, enable it in test mode, the
default is joystick not paddle) [David Haywood]

Improved the type 1 GX games a bit but none are working well enough to enable
yet [R.Belmont]
	src/drivers/konamigx.c
	src/vidhrdw/konamigx.c

Fixed Crash with the original mustang set [R.Belmont]
	src/drivers/nmk16.c

Changed frogs to use a 3 way joystick instead of a 3 way joystick and 2 buttons
which was incorrect [Robin Merrill]

Remapped some of the mid*unit.c buttons to match the real board inputs

Further ST-V improvements [Angelo Salese, David Haywood]
	src/drivers/stv.c
	src/vidhrdw/stvvdp1.c
	src/vidhrdw/stvvdp2.c


New games supported:
--------------------

Porky [Pierpaolo Prazzoli, Peter Storey] - no sound
Driving Force [Pierpaolo Prazzoli]
Big Bucks [Pierpaolo Prazzoli]
	src/drivers/pacman.c
	src/vidhrdw/pengo.c
	src/includes/pengo.h

Embargo [Stefan Jokisch, MASH]
	src/drivers/embargo.c

### New Clones supported:

8 Ball ACtion (Pacman Kit) [Pierpaolo Prazzoli, David Widel] - bad colours
	no sound
	src/drivers/pacman.c
	src/vidhrdw/pengo.c
	src/includes/pengo.h

PuzzLoop (USA) [Brian Troha]
	src/drivers/suprnova.c


### New TESTDRIVERS:

Competition Golf [Angelo Salese]
	incomplete video emulation
	src/drivers/compgolf.c

(Medal) Yumefuda [Angelo Salese]
	preliminary, doesn't boot yet
	src/drivers/yumefuda.c

# 0.74

Basically a cleaned up version of .73 now my machine is working again.

## General Source Changes


Misc source changes mainly to Improve compatibility with MESS [Nathan Woods]
	src/driver.h:
	- Minor changes within #ifdef MESS

	src/inptport.c:
	- Minor change to recognize MESS IPT_CONFIG input ports

	src/legacy.h:
	- Changed some of the prototypes to use 'mame_file *' instead of
	'void *'

	src/mame.h:
	- Added APPNAME and APPLONGNAME macros to define the name of the
	program (MAME/M.A.M.E. vs MESS/M.E.S.S.)

	src/vidhrdw/jaguar.c:
	- Adds some MESS specific templated blitters within #ifdef MESS

	src/vidhrdw/ppu2c03b.c:
	src/vidhrdw/ppu2c03b.h:
	src/vidhrdw/playch10.c:
	src/vidhrdw/vsnes.c:
	- Small speedups (eliminated some usage of plot_pixel())
	- Added support for scanline counts other than 262
	- Interrupt changes by Brad Oliver
	- Requisite changes to playch10 and vsnes to support Brad's
	interrupt changes

	src/windows/input.c:
	src/windows/video.c:
	src/windows/window.c:
	src/windows/window.h:
	src/windows/winmain.c:
	- Misc changes to support MESS's menu bar (should not affect
	MAME)
	- Misc changes to use APPNAME and APPLONGNAME

Improved Loading of .72 cfg Files (prevents Mixer settings corruption) [Stefan Jokisch]
	src/sound/mixer.c

Fixed User-Interface bug which was causing problems setting controls for some of the
players [Stefan Jokisch]
	src/usrintrf.c

Support for 8 Controllers [Robin Merrill]
	src/input.h
	src/windows/input.c
	src/inptport.c

Workaround for a v60 crash causing ultra-x not to run on some systems [Tomasz Slanina]
	src/cpu/v60/op12.c

Support upd7759 Silence Compression [Olivier Galibert]
	src/sound/upd7759.c

SegaPCM cleanups / fixes [Olivier Galibert]
	src/sound/segapcm.c
	src/sound/segapcm.h
	src/sndintrf.c

Changed Afterburner Inputs to use z-axis for throttle [Howard Casto]
	src/drivers/aburner.c

Some Improvements to Chequered Flag protection + Sound [Angelo Salese]
	src/drivers/chqflag.c
	src/vidhrdw/konamiic.c

More ST-V Improvements [Angelo Salese, David Haywood]
	src/drivers/stv.c
	src/vidhrdw/stvvdp1.c
	src/vidhrdw/stvvdp2.c

New games supported:
--------------------

Miyasu Nonki no Quiz 18-Kin (Japan) [Takahiro Nogi]
	src/drivers/welltris.c
	src/vidhrdw/welltris.c

Ojanko Club (Japan) [Takahiro Nogi, Uki]
	src/drivers/ojankohs.c

Quiz & Variety Sukusuku Inufuku (Japan) [Takahiro Nogi]
	src/drivers/inufuku.c
	src/vidhrdw/inufuku.c

Dancing Eyes (DC1/VER.A) [smf]
Kosodate Quiz My Angel 3 (KQT1/VER.A) [smf]
	src/drivers/namcos11.c
	+ most psx files

Super-X [David Haywood]
	src/drivers/dooyong.c

A Few new Megatech sets (mostly SMS based, not yet working)
	src/drivers/segac2.c

### New Clones supported:

Natsuiro Mahjong (Mahjong Summer Story) (Japan) (clone of mjnatsu) [Takahiro Nogi]
	src/drivers/fromance.c

Bang (Japan) [Manuel Abadia]
	src/drivers/gaelco2.c

Hangly Man (set 3) [Manuel Abadia]
	src/drivers/pacman.c
Apocaljpse Now (bootleg of Rescue) [Pierpaolo Prazzoli]
	src/drivers/scobra.c
Explorer (bootleg of Scramble) [Pierpaolo Prazzoli]
	src/drivers/scramble.c
	todo : check these 3 bootlegs are worth supporting, if not remove them


# 0.73

Sorry, this list of changes isn't complete due to hardware problems I'm having.

## General Source Changes


Many Improvements to the ST-V emulation, resulting in Hanagumi Columns being
playable and other games on the driver starting to show some
graphics [Angelo Salese, David Haywood]
	src/drivers/stv.c
	src/vidhrdw/stvvdp1.c
	src/vidhrdw/stvvdp2.c

Continued MegaPlay Improvements (still none fulyl working yet but more of the
bios menu tests pass now) [Barry Rodewald]
	src/drivers/segac2.c

Some System GX Improvements, mainly to the not yet working games [R.Belmont]
	src/drivers/konamigx.c
	src/vidhrdw/konamigx.c

Attempted to Improve Super Dodge Ball MCU Simulation [Acho A. Tang]
	src/drivers/sdodgeb.c
	src/vidhrdw/sdodgeb.c

Various Changes to keep the code inline with MESS [Nathan Woods]

Added 8 Player Support to the Core [Stefan Jokisch]

New games supported:
--------------------

Hanagumi Taisen Columns - Sakura Wars  [Angelo Salese, David Haywood]
	This the first st-v game to be supported by MAME, please note that due
	to the complexity of the system emulation is still preliminary and there
	are still various issues that will need ironing out, there is currently
	no sound.
	src/drivers/stv.c
	src/vidhrdw/stvvdp1.c
	src/vidhrdw/stvvdp2.c

Ultra X Weapons [Tomasz Slanina]
	some gfx offset problems / alignment (sprite handle can move?)
	src/drivers/ssv.c

Twin Eagle 2 [Tomasz Slanina]
	src/drivers/ssv.c
	Twin Eagle 2 DOES NOT WORK due to protection involving the directions
	in which objects travel, and as a side-effect collisions don't work
	correctly either, if somebody fixes this then itmight be fine apart
	from a few offset problems.

Super Trivia Master [Pierpaolo Prazzoli]
	wrong colours
	src/drivers/strvmstr.c

Sprint 8 [Stefan Jokisch]
	src/drivers/sprint8.c
	src/vidhrdw/sprint8.c

New Clones supported:
--------------------

Lady Bug (bootleg on Galaxian Hardware) [Mike Coates]
	input ports incomplete
	src/drivers/galaxian.c
	src/includes/galaxian.h
	src/machine/scramble.c

Video Pool (Moon Cresta hardware) (bootleg of Video Hustler) [Mike Coates]
	dipswitches incomplete
	src/drivers/galaxian.c

# 0.72u2

## General Source Changes


Improved Megaplay a bit, the bios screens now show however the games still don't
work [Barry Rodewald]
	src/drivers/segac2.c
	src/includes/segac2.h
	src/vidhrdw/segac2.c

Various ST-V improvements, Hanagumi Columns shows a partial title screen (still
disabled so you'll have to enable it yourself if you want to see)
	[Angelo Salese]
	src/drivers/stv.c

Removed DCS Speed-Ups
	src/sndhrdw/dcs.c

Fixed a bug in the C 68000 core with the SBCD Opcode [Acho A/ Tang]
	States of the sign(N) and overflow(V) flags after SBCD,
	ABCD and NBCD operations are "undefined" according to Motorola.
	The C core randomizes the V flag and sets the N flag like signed
	operations.

	Viofight expects determinant results of the two flags from SBCD
	or it gets stuck in a math loop. The V flag now returns zero and
	the N flag mirrors carry exactly like what the ASM core does.
	src/cpu/m68000/m68k_in.c

Fixed Mustache Boy Continue Screen and attempted to improve speed [Tomasz Slanina]
	src/drivers/mustache.c

Misc Konami Fixes [Acho A. Tang]
	tmnt.c:
	- qgakumon resetting and screen darkening
	- tmnt2 load-save states(not good enough yet)
	- ssriders diagonal jittering(needs testing)

	konamiic.c:
	- boundary conditions and kludge removal in K007420

	rockrage.c:
	- rogue sprites

	kncljoe.c:
	- bad scrolling, clipping, rogue sprites

Couple of Improvements to the Fuuki games (Priority, BG Colours) [Paul Priest]
	src/drivers/fuukifg2.c
	src/vidhrdw/fuukifg2.c
	src/drivers/fuukifg3.c
	src/vidhrdw/fuukifg3.c

Guard against OP_ROM/OP_RAM overflows [Nathan Woods]
	This patch adds an optional facility to guard against OP_ROM/OP_RAM
	overflows.  This is implemented by adding new variables OP_MEM_MIN and
	OP_MEM_MAX that track the minimum and maximum address ranges for which
	using OP_ROM/OP_RAM is valid.

	Support for this feature is enabled via conditional compilation in
	memory.h.  There are three possible modes:

	CPUREADOP_SAFETY_NONE   - The status quo; no range checking at
	all
	CPUREADOP_SAFETY_PARTIAL  - Range checking only against the
	maximum address
	CPUREADOP_SAFETY_FULL     - Range checking against both the
	minimum and the maximum

	This patch has MAME using CPUREADOP_SAFETY_NONE, unless MAME_DEBUG is
	defined in which case CPUREADOP_SAFETY_FULL is set.  Thus as is, this
	patch should have zero effect on release versions of MAME.
	Additionally, being able to safely disassemble code near bank boundaries
	when using the MAME debugger is extremely useful.

	There is a slight performance hit when this is enabled.

Dipswitch Fixes:
	1st dipswitch of 3rd bank in 1941 [Fabrizio Pedrazzini]
	src/drivers/cps1.c

	Tetris Plus 2 (MS32 version) [Sonikos]
	src/drivers/ms32.c

	and a few others ...

Changed nkdodgeb to use proms from another Japan bootleg set, maybe this fixes
some problems with the colours.
	src/drivers/nkdodgeb.c

Included 8080 core updates from MESS
	src/cpu/i8085/*

Preliminary Hyperstone Core, not yet working well enough to run anything [Pierpaolo Prazzoli]

Now using correct Dragon Blaze sound ROM
	src/drivers/psikyosh.c

New Clones supported:
--------------------

Hard Dunk (World)
	src/drivers/multi32.c

Super Bike (DK Conversion) [Pierpaolo Prazzoli]
	src/drivers/dkong.c

Some CPS clones [Razoola]
	src/drivers/cps2.c

New games supported:
--------------------

Mogura Desse (Cabinet Test Board) [David Haywood]
	no sound (discrete)
	src/drivers/mogura.c

Speed Attack [Angelo Salese, Pierpaolo Prazzoli]
	wrong colours
	src/drivers/speedatk.c
	src/vidhrdw/speedatk.c

Some New MegaTech Sets (mostly not working yet)
	src/drivers/segac2.c

# 0.72u1

## General Source Changes


Changed Fuuki 16-bit driver to use Partial Updates for Raster Effects and enabled
them [David Haywood, Paul Priest]
	src/drivers/fuukifg2.c
	src/vidhrdw/fuukifg2.c

Temporary Fixes to a few of the System 16 driver problems (sound banking for
example)  Note, the driver is still being rewritten these are only minor 'fixes'
to improve things a bit until the rewrite is complete [Acho A. Tang]
	src/drivers/sysetm16.c
	src/vidhrdw/system16.c
	src/vidhrdw/sys16spr.c

Change so DIP menu isn't displayed if no Dipswitches are defined in the driver
instead of flashing a blank mneu when you try and enter it [Paul Priest]
	src/usrintrf.c

Various Changes [Nathan Woods]
	src/cpu/jaguar/jaguar.c:
	- Fixed a bug in the NORMI opcode

	src/cpu/mips/mips.c:
	- Removed use of the GCC specific varargs on macros feature

	src/sndhrdw/jaguar.c:
	- Changed around one of the #include paths (due to
	ProjectBuilder on Mac issues)

	src/sound/tms5220.c:
	src/sound/tms5220.h:
	- Added support for the TMS0285 variant (an old variant of the
	TMS5220)

	src/vidhrdw/jagblit.c:
	src/vidhrdw/jaguar.c:
	- Added support for the SRCSHADE and DSTA2 command bits to the
	Jaguar blitter.
	- Added some miscellaneous logging code
	- Changed around one of the #include paths (due to
	ProjectBuilder on Mac issues)

	src/vidhrdw/jagobj.c:
	- Added support for objects with pitch != 1 for objects with 1-2
	bits per pixels

	src/harddisk.c:
	src/harddisk.h:
	- Added a function to retrieve the interface

	src/ui_text.c:
	src/ui_text.h:
	- Added a MESS specific UI string (within #ifdef MESS of course)

	src/usrintrf.c:
	- Parameterized the setdipswitches() call; so that a menu that
	looks like the DIP switch menu can easily be made with different IPT_*
	values (MESS has a configuration menu that functions a lot like the DIP
	switch menu)

	src/artwork.c:
	- Small fix to allow the UI to be drawn if the artwork system is
	active but no artwork is present (not an issue for MAME)

	src/mamedbg.c:
	- Cleanup and fix of set_ea_info() so that negative offsets of 8
	or 16 bits do not have extraneous FF's in front of them.
	- More proper fix to the scanline display bug that prevented
	registers from being edited when the scanline display was on.

	src/cpu/hd6309/6309dasm.c:
	- Better message for when invalid opcodes are disassembled

	src/artwork.c
	src/artwork.h
	- Fixed crashing bug with clones

	src/inptport.c:
	- Adds some MESS specific IPT defaults (within #ifdef MESS)
	- Modified input_port_allocate() so that PORT_BITX declarations
	that specify JOYCODE_a_BUTTONb for their default codes will also get
	JOYCODE_MOUSE_a_BUTTONb or'd in.

	src/input.c:
	src/input.h:
	- Added seq_set_4() and seq_set_5()

	src/windows/video.c:
	- Changed render_frame() so that throttle_speed() will also be
	called when game_is_paused is set.  This prevents MAME from busy looping
	when paused.

Fixed Remaining Protection problems in TMNT2 and Sunset Riders, Fixed Combat School
Trackball sets from Crashing, Fixed Escape Kids Spries, Fixed Sticky Sprites in
Detana! Twinbee, Fixed Sprite Lag in some games [Acho A. Tang]
	src/drivers/combatsc.c
	src/vidhrdw/konamiic.c
	src/vidhrdw/konamiic.h
	src/palette.c
	src/palette.h
	src/drivers/tmnt.c
	src/vidhrdw/tmnt.c

Misc Improvements / Cleanups / Fixes [Acho A. Tang]
	src/drivers/aeroboto.c
	src/vidhrdw/aeroboto.c
	src/drivers/bwing.c
	src/vidhrdw/bwing.c
	src/drivers/equites.c
	src/vidhrdw/equites.c
	src/machine/equites.c
	src/drivers/halleys.c
	src/drivers/sgladiat.c
	src/drivers/sidearms.c
	src/vidhrdw/sidearms.c
	src/vidhrdw/spdodgeb.c
	src/drivers/wecleman.c
	src/vidhrdw/wecleman.c
	src/drivers/wiz.c

Fixed Background Graphics in Trackball version of WC90 [Angelo Salese]
	src/drivers/wc90.c
	src/vidhrdw/wc90.c
	
Fixed sound in Super Doubles Tennis [Pierpaolo Prazzoli]
	src/drivers/btime.c

New games supported:
--------------------

Asura Blade [Paul Priest]
	missing sound, some priority errors
	src/drivers/fuukifg3.c
	src/vidhrdw/fuukifg3.c

Gaia Crusaders [Paul Priest]
	src/drivers/cave.c

Street Heat [Dave Widel]
	src/drivers/dkong.c

New Clones supported:
--------------------

8 Ball Action (DkJr Kit) [Pierpaolo Prazzoli]
	src/drivers/dkong.c


# 0.72u2

## General Source Changes


Improved Megaplay a bit, the bios screens now show however the games still don't
work [Barry Rodewald]
	src/drivers/segac2.c
	src/includes/segac2.h
	src/vidhrdw/segac2.c

Various ST-V improvements, Hanagumi Columns shows a partial title screen (still
disabled so you'll have to enable it yourself if you want to see)
	[Angelo Salese]
	src/drivers/stv.c

Removed DCS Speed-Ups
	src/sndhrdw/dcs.c

Fixed a bug in the C 68000 core with the SBCD Opcode [Acho A/ Tang]
	States of the sign(N) and overflow(V) flags after SBCD,
	ABCD and NBCD operations are "undefined" according to Motorola.
	The C core randomizes the V flag and sets the N flag like signed
	operations.

	Viofight expects determinant results of the two flags from SBCD
	or it gets stuck in a math loop. The V flag now returns zero and
	the N flag mirrors carry exactly like what the ASM core does.
	src/cpu/m68000/m68k_in.c

Fixed Mustache Boy Continue Screen and attempted to improve speed [Tomasz Slanina]
	src/drivers/mustache.c

Misc Konami Fixes [Acho A. Tang]
	tmnt.c:
	- qgakumon resetting and screen darkening
	- tmnt2 load-save states(not good enough yet)
	- ssriders diagonal jittering(needs testing)

	konamiic.c:
	- boundary conditions and kludge removal in K007420

	rockrage.c:
	- rogue sprites

	kncljoe.c:
	- bad scrolling, clipping, rogue sprites

Couple of Improvements to the Fuuki games (Priority, BG Colours) [Paul Priest]
	src/drivers/fuukifg2.c
	src/vidhrdw/fuukifg2.c
	src/drivers/fuukifg3.c
	src/vidhrdw/fuukifg3.c

Guard against OP_ROM/OP_RAM overflows [Nathan Woods]
	This patch adds an optional facility to guard against OP_ROM/OP_RAM
	overflows.  This is implemented by adding new variables OP_MEM_MIN and
	OP_MEM_MAX that track the minimum and maximum address ranges for which
	using OP_ROM/OP_RAM is valid.

	Support for this feature is enabled via conditional compilation in
	memory.h.  There are three possible modes:

	CPUREADOP_SAFETY_NONE   - The status quo; no range checking at
	all
	CPUREADOP_SAFETY_PARTIAL  - Range checking only against the
	maximum address
	CPUREADOP_SAFETY_FULL     - Range checking against both the
	minimum and the maximum

	This patch has MAME using CPUREADOP_SAFETY_NONE, unless MAME_DEBUG is
	defined in which case CPUREADOP_SAFETY_FULL is set.  Thus as is, this
	patch should have zero effect on release versions of MAME.
	Additionally, being able to safely disassemble code near bank boundaries
	when using the MAME debugger is extremely useful.

	There is a slight performance hit when this is enabled.

Dipswitch Fixes:
	1st dipswitch of 3rd bank in 1941 [Fabrizio Pedrazzini]
	src/drivers/cps1.c

	Tetris Plus 2 (MS32 version) [Sonikos]
	src/drivers/ms32.c

	and a few others ...

Changed nkdodgeb to use proms from another Japan bootleg set, maybe this fixes
some problems with the colours.
	src/drivers/nkdodgeb.c

Included 8080 core updates from MESS
	src/cpu/i8085/*

Preliminary Hyperstone Core, not yet working well enough to run anything [Pierpaolo Prazzoli]

Now using correct Dragon Blaze sound ROM
	src/drivers/psikyosh.c

New Clones supported:
--------------------

Hard Dunk (World)
	src/drivers/multi32.c

Super Bike (DK Conversion) [Pierpaolo Prazzoli]
	src/drivers/dkong.c

Some CPS clones [Razoola]
	src/drivers/cps2.c

New games supported:
--------------------

Mogura Desse (Cabinet Test Board) [David Haywood]
	no sound (discrete)
	src/drivers/mogura.c

Speed Attack [Angelo Salese, Pierpaolo Prazzoli]
	wrong colours
	src/drivers/speedatk.c
	src/vidhrdw/speedatk.c

Some New MegaTech Sets (mostly not working yet)
	src/drivers/segac2.c

# 0.72u1

## General Source Changes


Changed Fuuki 16-bit driver to use Partial Updates for Raster Effects and enabled
them [David Haywood, Paul Priest]
	src/drivers/fuukifg2.c
	src/vidhrdw/fuukifg2.c

Temporary Fixes to a few of the System 16 driver problems (sound banking for
example)  Note, the driver is still being rewritten these are only minor 'fixes'
to improve things a bit until the rewrite is complete [Acho A. Tang]
	src/drivers/sysetm16.c
	src/vidhrdw/system16.c
	src/vidhrdw/sys16spr.c

Change so DIP menu isn't displayed if no Dipswitches are defined in the driver
instead of flashing a blank mneu when you try and enter it [Paul Priest]
	src/usrintrf.c

Various Changes [Nathan Woods]
	src/cpu/jaguar/jaguar.c:
	- Fixed a bug in the NORMI opcode

	src/cpu/mips/mips.c:
	- Removed use of the GCC specific varargs on macros feature

	src/sndhrdw/jaguar.c:
	- Changed around one of the #include paths (due to
	ProjectBuilder on Mac issues)

	src/sound/tms5220.c:
	src/sound/tms5220.h:
	- Added support for the TMS0285 variant (an old variant of the
	TMS5220)

	src/vidhrdw/jagblit.c:
	src/vidhrdw/jaguar.c:
	- Added support for the SRCSHADE and DSTA2 command bits to the
	Jaguar blitter.
	- Added some miscellaneous logging code
	- Changed around one of the #include paths (due to
	ProjectBuilder on Mac issues)

	src/vidhrdw/jagobj.c:
	- Added support for objects with pitch != 1 for objects with 1-2
	bits per pixels

	src/harddisk.c:
	src/harddisk.h:
	- Added a function to retrieve the interface

	src/ui_text.c:
	src/ui_text.h:
	- Added a MESS specific UI string (within #ifdef MESS of course)

	src/usrintrf.c:
	- Parameterized the setdipswitches() call; so that a menu that
	looks like the DIP switch menu can easily be made with different IPT_*
	values (MESS has a configuration menu that functions a lot like the DIP
	switch menu)

	src/artwork.c:
	- Small fix to allow the UI to be drawn if the artwork system is
	active but no artwork is present (not an issue for MAME)

	src/mamedbg.c:
	- Cleanup and fix of set_ea_info() so that negative offsets of 8
	or 16 bits do not have extraneous FF's in front of them.
	- More proper fix to the scanline display bug that prevented
	registers from being edited when the scanline display was on.

	src/cpu/hd6309/6309dasm.c:
	- Better message for when invalid opcodes are disassembled

	src/artwork.c
	src/artwork.h
	- Fixed crashing bug with clones

	src/inptport.c:
	- Adds some MESS specific IPT defaults (within #ifdef MESS)
	- Modified input_port_allocate() so that PORT_BITX declarations
	that specify JOYCODE_a_BUTTONb for their default codes will also get
	JOYCODE_MOUSE_a_BUTTONb or'd in.

	src/input.c:
	src/input.h:
	- Added seq_set_4() and seq_set_5()

	src/windows/video.c:
	- Changed render_frame() so that throttle_speed() will also be
	called when game_is_paused is set.  This prevents MAME from busy looping
	when paused.

Fixed Remaining Protection problems in TMNT2 and Sunset Riders, Fixed Combat School
Trackball sets from Crashing, Fixed Escape Kids Spries, Fixed Sticky Sprites in
Detana! Twinbee, Fixed Sprite Lag in some games [Acho A. Tang]
	src/drivers/combatsc.c
	src/vidhrdw/konamiic.c
	src/vidhrdw/konamiic.h
	src/palette.c
	src/palette.h
	src/drivers/tmnt.c
	src/vidhrdw/tmnt.c

Misc Improvements / Cleanups / Fixes [Acho A. Tang]
	src/drivers/aeroboto.c
	src/vidhrdw/aeroboto.c
	src/drivers/bwing.c
	src/vidhrdw/bwing.c
	src/drivers/equites.c
	src/vidhrdw/equites.c
	src/machine/equites.c
	src/drivers/halleys.c
	src/drivers/sgladiat.c
	src/drivers/sidearms.c
	src/vidhrdw/sidearms.c
	src/vidhrdw/spdodgeb.c
	src/drivers/wecleman.c
	src/vidhrdw/wecleman.c
	src/drivers/wiz.c

Fixed Background Graphics in Trackball version of WC90 [Angelo Salese]
	src/drivers/wc90.c
	src/vidhrdw/wc90.c
	
Fixed sound in Super Doubles Tennis [Pierpaolo Prazzoli]
	src/drivers/btime.c

New games supported:
--------------------

Asura Blade [Paul Priest]
	missing sound, some priority errors
	src/drivers/fuukifg3.c
	src/vidhrdw/fuukifg3.c

Gaia Crusaders [Paul Priest]
	src/drivers/cave.c

Street Heat [Dave Widel]
	src/drivers/dkong.c

New Clones supported:
--------------------

8 Ball Action (DkJr Kit) [Pierpaolo Prazzoli]
	src/drivers/dkong.c

# 0.72

FAQ about new drivers added since 0.71
--------------------------------------

for Run and Gun you must hold down F2 and press F3 to init the Eeprom otherwise
the game won't boot, the rungunu set doesn't work correctly.

Megatech / Megaplay / NSS are preliminary, since the menu system doesn't yet work
perfectly on Megatech I've marked all the games as non-working, Megaplay still
doesn't boot so those are marked as Non-working, and the NSS menu isn't emulated
at all yet so I also consider that to be Non-working, I've enabled these anyway
as it may encourage others to help with the development, Mame is NOT turning into
an emulator for Consoles, these are Arcade systems much like PlayChoice-10.

Outrunners is currently still broken due to problems with the V60 and recent timer
updates, sorry.

The games using the sp0250 sound for speech still need samples for the sound
effects, however samples for the speech are no longer needed with this update.

## General Source Changes


Work Around for Genix Protection + tilemap scroll making it playable [Paul Priest]
	src/drivers/pirates.c
	src/vidhrdw/pirates.c

Various Conversions to Tilemaps [Curt Coder]
	some were left out as they need checking, or devs didn't like them

Various Dipswitch Fixes [I�igo Luja]

Cleanups to Darius Video [Acho A. Tang]
	src/vidhrdw/warriorb.c
	src/vidhrdw/taitoic.c
	src/vidhrdw/taitoic.h
	src/vidhrdw/ninjaw.c
	src/vidhrdw/darius.c

6821 PIA fixes [Martin Adrian]
	src/machine/6821pia.c
	src/machine/6821pia.h

Kludge for Combat School Music [Acho A. Tang]
	might be an fm.c bug, being investigated by Jarek

Some Cleanups in itech32.c, dogfgt.c [Brian Troha]

New Clones supported:
--------------------

Zero Point (alt) [Brian Troha]
	src/drivers/unico.c


# 0.71u3p


## General Source Changes


Cleaned up tmnt.c cuebrick addition, added SSriders bootleg (not working) [R.Belmont]
	src/drivers/tmnt.c
	src/vidhrdw/tmnt.c

Added some Slapstics used in Europe / Japan [Aaron Giles]
	src/machine/slapstic.c

This patch adds some minor callbacks to the artwork system in order to
enable some MESS specific functionality without polluting artwork.c with
a bunch of ugly #ifdef MESS's. [Nathan Woords]
	src/drivers/artwork.c
	src/drivers/artwork.h
	src/mame.c

Fixed Tile flip in mermaid driver [Pierpaolo Prazzoli]
	src/vidhrdw/mermaid.c

NSS Fixes [Pierpaolo Prazzoli]
	src/cpu/g65816/g65816.c
	src/drivers/nss.c
	src/machine/snes.c

Vector Cleanups / Improvements [Frank Palazzolo]
	- Removed VEC_SHIFT and "magic numbers" stuff from all drivers, 
	vector.c: all fixed point stuff is now always 16.16.
	- Removed "bad trench vectors hack" from Star Wars which I created in 
	1997.  Appears not to be necessary any more :)
	- Moved flipping and swapping API stuff from vector.c to avgdvg.c, 
	this is specific to tempest and quantum only.
	- Added clipping to Sega games per the schematics
	- Using floating point in vector.c to do scaling.
	- Removed dead code using Machine->orientation from vector.c
	- Added capability to register an external vector renderer.
	src/drivers/quantum.c
	src/drivers/tempest.c
	src/machine/starwars.c
	src/vidhrdw/avgdvg.c
	src/vidhrdw/avgdvg.h
	src/vidhrdw/aztarac.c
	src/vidhrdw/cchasm.c
	src/vidhrdw/cinemat.c
	src/vidhrdw/sega.c
	src/vidhrdw/vector.c
	src/vidhrdw/vector.h

Fixed compile problems [Brad Oliver]
	src/vidhrdw/bigevglf.c
	src/vidhrdw/segaic24.c

re-written and merged the drivers for Sprint 1/2 and Dominos [Stefan Jokisch]
	src/drivers/sprint2.c
	src/vidhrdw/sprint2.c

Fixed some problems with Vector Patch [Frank Palazzolo]
	src/drivers/quantum.c
	src/drivers/tempest.c
	src/vidhrdw/sega.c
	src/vidhrdw/avgdvg.c

Various Fixes [Nathan Woods]
	src/fileio.c:
	- Fixes a bug in mame_fgetc() when reading characters between
	0x80 and 0xFF

	src/hash.c:
	src/hash.h:
	- Adds a new function, hash_verify_string(), that takes a string
	and checks to see if it is a valid hash string

	src/mame.c:
	- Added a consistency check that uses hash_verify_string() to
	make sure all of the hash strings are valid

Tiger Heli MCU simulation improvements [Angelo Salese]
	src/drivers/slapfght.c
	src/machine/slapfght.c

Coverted Text Layer of Act Fancer to Tilemaps [Angelo Salese]
	src/vidhrdw/actfancr.c

Fix drawgfx 1:1 optimization [Olivier Galibert]
	src/drawgfx.c

Fixed i803x jumps [Olivier Galibert]
	src/cpu/i8039/8039dasm.c
	src/cpu/i8039/i8039.c

sp0250 sound / g80 speech board emulation [Olivier Galibert]
	src/drivers/sega.c
	src/drivers/segar.c
	src/includes/sega.h
	src/mame.mak
	src/rules.mak
	src/sndhrdw/sega.c
	src/sndhrdw/segar.c
	src/sndintrf.c
	src/sndintrf.h
	src/sndhrdw/segasnd.c
	src/sndhrdw/segasnd.h
	src/sound/sp0250.c
	src/sound/sp0250.h

Fixed Power Instinct Dips [Brian Troha]
	src/drivers/powerins.c

Improved Risky Challenge decryption [Angelo Salese]
	src/machine/irem_cpu.c

Misc Patches [Nathan Woods]
	src/inptport.c:
	src/inptport.h:
	- Raises the maximum number of ports to thirty
	- Adds a few MESS specific IPT_* types (within #ifdef MESS)

	src/cpu/i86/i86.c:
	src/cpu/i86/i286.c:
	- Adds state save support

	src/includes/snes.h:
	src/machine/snes.c:
	- Update from Anthony Kruize (MESS SNES driver author)

Starsweep GFX improvements [smf]
	src/cpu/mips/mips.c
	src/cpu/mips/mipsdasm.c
	src/driver.c
	src/drivers/namcos11.c
	src/drivers/zn.c
	src/includes/psx.h
	src/vidhrdw/psx.c

Fixed register editing when Scanline display is enabled in debugger [Nathan Woods]
	src/mamedbg.c

Fixed the BCD conversion when using the Decimal Flag in ADC and SBC opcodes
in the G65816 CPU core.This fixes at least the Act Raiser timer/score bugs. [Angelo Salese]
	src/cpu/g65816/g65816.c
	src/cpu/g65816/g65816cm.h
	src/cpu/g65816/g65816op.h

Added correct roms to Genix but it still doesn't work due to protection [David Haywood]
	src/drivers/pirates.c

Improvements to Megatech emulation, although its still far from being correct and still
under development, will be split from segac2.c once finished [Barry Rodewald, David Haywood]
	src/drivers/segac2.c
	src/vidhrdw/segac2.c

New games supported:
--------------------

Power Instinct 2 [Olivier Galibert, ShiRiRu]
	todo: figure out why we need a rom patch to avoid an ingame problem
	src/drivers/cave.c
	src/vidhrdw/cave.c

Sprint 4 [Stefan Jokisch]
	src/drivers/sprint4.c
	src/vidhrdw/sprint4.c

Pit 'n' Run [Tomasz Slanina, Pierpaolo Prazzoli]
	missing analog sounds
	src/drivers/pitnrun.c
	src/machine/pitnrun.c
	src/vidhrdw/pitnrun.c

Special Forces [Pierpaolo Prazzoli]
	no sound, missing rom?
Eight Ball Action [Pierpaolo Prazzoli]
	bad colours, missing proms
	src/drivers/dkong.c
	src/vidhrdw/dkong.c

Imago [Pierpaolo Prazzoli]
	gfx problems, bad colours due to missing proms
	src/drivers/fastfred.c
	src/vidhrdw/fastfred.c
	src/includes/fastfred.h

Sengoku Mahjong [Angelo Salese]
	src/drivers/sengokmj.c
	src/vidhrdw/sengokmj.c

New Clones supported:
--------------------

Shuffle Shot 1.37 [Brian Troha]
WC Bowling 1.61 [Brian Troha]
	src/drivers/itech32.c

Sprint 2 (set 2) [Stefan Jokisch]
	src/drivers/sprint2.c

Parodius (World) [Brian Troha]
	src/drivers/parodius.c

Acrobatic Dog Fight [Brian Troha]
	src/drivers/dogfgt.c

Sunset Riders (bootleg) [R.Belmont]
	doesn't work
	src/drivers/tmnt.c

Pit Fighter (Japan) [Aaron Giles]
	src/drivers/atarig1.c

Various CPS2 clones [Razoola]
	src/drivers/cps2.c
	src/vidhrdw/cps1.c
	
0.71u2

## General Source Changes


Some progress on MegaTech emulation [David Haywood]
	menu is now displayed, but carts not recognised, display currently controllled
	by a dipswitch, will be changed later, enabled the other games on the driver
	while work is done, these might be disabled for 0.72
	src/drivers/segasyse.c
	src/vidhrdw/segasyse.c
	src/drivers/segac2.c
	src/vidhrdw/segac2.c
	src/includes/segac2.h

Fixes to some NeoGeo sets [Razoola]
	98e -> 98, -98a 99 <-> 99a 
	src/drivers/neogeo.c

Fixed Sound Chips in lsasquad [Jarek Burczynski]
	src/drivers/lsasquad.c

Fixed Loading of Landmaker (prototype) [David Haywood]
	src/drivers/taito_f3.c

Converted Some Drivers to Tilemaps [Curt Coder]
	src/drivers/bogeyman.c
	src/vidhrdw/bogeyman.c
	src/drivers/funkybee.c
	src/vidhrdw/funkybee.c
	+ more

Cleaned Up some Konami GX rom loading [R.Belmont]
	src/drivers/konamigx.c

Various Changes [Nathan Woods]
	src/cpu/jaguar/jaguar.c
	src/includes/jaguar.h
	src/vidhrdw/jaguar.c
	src/vidhrdw/jagobj.c
	- Adds save state support to Cojag
	- Disables certain error logging and speedup hacks under MESS
	- get_jaguar_memory() now based on memory_get_read_ptr()
	- Jaguar blitter won't crash if it is instructed to blit to a
	  weird address

	src/windows/winmain.c
	- Moved helpfile name into a constant

	src/mame.c
	- Minor tweaks in #ifdef MESS, and #ifndef MESS'ed a consistency
	  check inapprpriate for MESS

	src/inptport.c
	src/inptport.h
	- Added very small hooks for MESS

	src/input.c
	- Changed seq_name() so that instead of saying "<KEY> or n/a",
	  it will say "<KEY>"

Cpu Interrupt Improvements fixing the Exidy 440 games (Chiller etc.), Ultimate
Tennnis, and the Seibu Sound System [Aaron Giles]
	src/cpuint.c
	src/sndhrdw/seibu.c

Fixed Reset in robokid when time runs out (robokid37b15red) [Tomasz Slanina]
	src/drivers/omegaf.c

Stopped Nova2001 from resetting when a coin is inserted [Tomasz Slanina]
	if you insert coins too quickly the game still resets, its probably
	just the way it was, the code for this part was changed in the clone
	src/drivers/nova2001.c

Some Dipswitch Fixes [I�igo Luja]
	src/drivers/rohga.c
	src/drivers/deadang.c

Changed way Snapshot code works [Nathan Woods]

Huge Improvements in Prop Cycle (Namco System 22) [Phil Stroffolino]
	Changes include:
	- pulled out matrix3d to its own module
	- near plane clipping
	- perspective correct textures
	- preliminary lighting (all or nothing)
	- gamma correction
	- preliminary fader support
	- per polygon and per object depth bias
	- sprite-polygon priority
	- two-sided polygon flag
	- tile flip attributes
	- tilemap color bank
	- improved steering responsiveness
	- camera zoom and pan feature
	- fixes to display list parsing
	- corrected rotation mappings for System22/21
	src/drivers/namcos22.c
	src/vidhrdw/namcos22.c
	src/includes/namcos22.h
	src/vidhrdw/namcos21.c
	src/vidhrdw/namcos3d.c
	src/includes/namcos3d.h
	src/vidhrdw/matrix3d.c
	src/vidhrdw/matrix3d.h

Stopped Sprites being hidden by track in fourtrax [Phil Stroffolino]
	src/vidhrdw/namcos2.c

Fixed CPS2 raster effect (xmvsf069agre) [ShiRiRu]
	src/drivers/cps2.c
	src/vidhrdw/cps1.c

Added Missing Resources
	src/info.c
	
	

New games supported:
--------------------

Xor World (prototype) [Manuel Abadia]
	src/drivers/xorworld.c
	src/vidhrdw/xorworld.c
	src/sound/saa1099.c
	src/sound/saa1099.h

Big Event Golf [Tomasz Slanina, Jarek Burczynski]
	src/drivers/bigevglf.c
	src/vidhrdw/bigevglf.c
	src/machine/bigevglf.c

Strike Bowling [Tomasz Slanina, Jarek Burczynski]
	lacks analog sounds, in progress
	src/drivers/sbowling.c

Super Speed Race Junior [Tomasz Slanina]
	bad colours due to missing proms
	src/drivers/ssrj.c
	src/vidhrdw/ssrj.c

Great Guns [Jarek Burczynski]
	graphics leave trails
	src/drivers/mazerbla.c

Taisen Hot Gimmick 3 Digital Surfing [David Haywood]
	src/drivers/psikyo4.c

Actraiser (Nintendo Super System) [MESS, Pierpaolo Prazzoli, David Haywood]
	just to show the current status, has no menu system emulated, still being
	worked on, no sound, everything else on the system has been left as 
	testdrivers
	src/drivers/nss.c
	src/vidhrdw/snes.c
	src/machine/snes.c
	src/sndhrdw/snes.c
	src/includes/snes.h

Genix [David Haywood]
	missing gfx / sound roms, might work with them
	src/drivers/pirates.c

New clones supported
--------------------

Cowbows of Moo Mesa (bootleg) [R.Belmont]
	no sprites yet
	src/drivers/moo.c

Cue Brick (Japan Ver. D) [R.Belmont]
	I suspect its not Japan
	src/drivers/tmnt.c
	src/vidhrdw/tmnt.c

Pest Place (Mario Bros bootleg) [Pierpaolo Prazzoli]
	bad colours, some bad gfx etc.
	src/drivers/dkong.c
	src/vidhrdw/dkong.c

Area 51 / Maximum Force Duo (R3000) [Brian A Troha]
	speed hack needs doing
	src/drivers/cojag.c

New TESTDRIVERS in source
-------------------------

Little Casino [Pierpaolo Prazzoli, David Haywood]
	inputs, video etc. needs finishing
	src/drivers/ltcasino.c

Super Draw Poker [Pierpaolo Prazzoli]
	needs inputs finishing, need to work out if its a gambling game, if so it will
	be removed
	src/drivers/supdrapo.c
	

# 0.71u1

## General Source Changes


Cleaned Up and Fixed filenames in Namco System 11 driver [smf]
	src/drivers/namcos11.c

Added Cocktail Mode to Mole Attack [Pierpaolo Prazzoli]
	src/drivers/mole.c

Fixed Mame not looking in Parent directory for Hard Drive Images [Stefan Jokisch]
	src/mame.c
	src/audit.c

Converted Drivers to use Tilemaps [Curt Coder]
	src/drivers/rocnrope.c
	src/vidhrdw/rocnrope.c
	src/drivers/nova2001.c
	src/vidhrdw/nova2001.c
	src/drivers/hanaawas.c
	src/vidhrdw/hanaawas.c

Improved Genesis Emulation a bit in preparation for MegaTech / MegaPlay,
sound code based on MESS [David Haywood]
	src/drivers/segac2.c
	src/vidhrdw/segac2.c

Improvements to GI Joe [Acho A. Tang]
	tilemap.h,tilemap.c
	 - added tilemap_get_transparency_data() for transparency cache
	   manipulation

	vidhrdw\konamiic.c
	 - added preliminary K056832 tilemap<->linemap switching and tileline
	   code

	drivers\gijoe.c
	 - updated video settings, memory map and irq handler
	 - added object blitter

	vidhrdw\gijoe.c
	 - completed K054157 to K056832 migration
	 - added ground scroll emulation
	 - fixed sprite and BG priority
	 - improved shadows and layer alignment

Improvements to Run and Gun making it playable (single screen game only) [Acho A. Tang]
	drivers\rungun.c (this file)
	 - mem maps, device settings, component communications, I/O's, sound...etc.

	vidhrdw\rungun.c
	 - general clean-up, clipping, alignment

	vidhrdw\konamiic.c
	 - missing sprites and priority

Added missing rowselect effect to toaplan2 driver (bbakraid, batrider) [David Haywood]
	src/vidhrdw/toaplan2.c

Compiler Fixes [Bryan McPhail]
	src/vidhrdw/raiga.c

Compiler Fixes [Brad Oliver]
	src/drivers/system24.c
	src/vidhrdw/system24.c

New games supported:
--------------------

Sonic (MegaTech, MegaPlay) [David Haywood]
	just to show the current status, megatech has no menu system emulated, megaplay doesn't boot,
        everything else on the systems has been left as testdrivers
	src/drivers/segac2.c
	src/vidhrdw/segac2.c


New clones supported
--------------------

Landmaker (World prototype) [Bryan Mcphail]
	src/drivers/taito_f3.c


# 0.71

Everything from 0.70u1 - u5 (see details below) as well as

## General Source Changes


Fixed MultiPCM banking for Stadium Cross [R.Belmont]
	src/drivers/multi32.c
	src/sound/multipcm.c
	src/sound/multipcm.h

Improved Ay8910 Noise Generator [Jarek Burczynski]
	src/sound/ay8910.c

Corrected some filenames for Ajax [Brian A. Troha]
	src/drivers/ajax.c

Improved Graphics in Strato Fighter and Super Pinball Action [Leon van Rooij]
	src/drivers/raiga.c
	src/vidhrdw/raiga.c
	src/drivers/spbactn.c
	src/vidhrdw/spbactn.c
	src/drawgfx.c

Converted a bunch of drivers to tilemaps, fixed dipswitches etc. [Curt Coder]

New games supported:
--------------------

Boxer (prototype) [Stefan Jokisch]
	src/drivers/boxer.c
	src/vidhrdw/boxer.c

Star Sweep [smf, MooglyGuy]
	src/drivers/namcos11.c
	src/drivers/psx.c
	src/vidhrdw/psx.c
	+ various other source files

Magical Spot [Pierpaolo Prazzoli]
	src/drivers/cosmic.c

New clones supported
--------------------

Sengekis Striker (Asia) [Brian A. Troha, David Haywood]
	src/drivers/suprnova.c
	src/vidhrdw/suprnova.c

Maximum Force (v1.05) [Brian A. Troha]
	note: there seems to be a problem with the way mame handles chd filenames for clones
	src/drivers/cojag.c

Space Invasion (Europe Original?) [Brian A. Troha]
	src/drivers/commando.c

Underfire (US and Japan) [Brian A. Troha]
	src/drivers/undrfire.c

Puzzle Bobble (NeoGeo) (set 2) [Razoola]
	src/drivers/neogeo.c


New TESTDRIVERS in source
-------------------------

Various ZN based systems [smf, MooglyGuy]

Rougien [..]
	video hardware emulation needs finishing
	src/drivers/mermaid.c

# 0.70u5

Please, if you submit dipswitch changes to me make sure they've been tested with a DEBUG compile, fixing
'fixed' dipswitches is time consuming, there are extra warnings when you attempt to run Mame in a DEBUG
compile.

## General Source Changes


Some Dipswitch Fixes [I�igo Luja]
	src/drivers/gameplan.c
	src/drivers/cps1.c (kodj064gra)
	src/drivers/ms32.c
	src/drivers/freekick.c
	src/drivers/wwfwfest.c (wwfwfest055gra partial?)
	src/drivers/exidy.c (venture37b8gra)
	src/drivers/suprnova.c
	src/drivers/seta.c
	src/drivers/system16.c

Improvements and Fixes to Strato Fighter / Raiga [David Haywood, TheGuru, Leon van Rooij]
	there are still some graphics problems (priority, incomplete alpha blending effects, bad tiles)
	src/drivers/gaiden.c
	src/vidhrdw/gaiden.c

Fixed some Misplaced Sprites in Tricky Doc [Pierpaolo Prazzoli]
	src/vidhrdw/sauro.c

Changed drivers to use mame_rand for random numbers [Barry Rodewald]
	src/drivers/artmagic.c
	src/drivers/centiped.c
	src/drivers/namcona1.c
	src/drivers/namconb1.c
	src/machine/namcos1.c

A Few Namco Fixes for things broken in u4 [Phil Stroffolino]
	src/vidhrdw/namcos2.c
	src/machine/namcos2.c

Changed Steel Gunner to use Correct Rom [Bryan McPhail]
	src/drivers/namcos2.c

Improved ncv1 filenames [TheGuru]
	src/drivers/namcond1.c

Fixed Sprites in simpsons [Acho A Tang]
	src/drivers/simpsons.c
	src/machine/simpsons.c
	src/vidhrdw/simpsons.c
	src/vidhrdw/konamiic.c
	src/vidhrdw/konamiic.h

Various Changes [Nathan Woods]
	src/fileio.c:
	- Introduced a "debug cookie"; when in debug mode, puts a cookie
	value in the mame_file structure to prevent files from being closed
	twice without an assertion

	src/windows/window.c:
	- Fixes a bug in visible area changing in windowed, non-stretch
	mode.  Previously, when the visible area changed, the code would simply
	change the visible area, and look at the resulting window size and tweak
	it accordingly with win_adjust_window().  The problem with this is that
	if you have a sequence where the visible area is made larger, then
	smaller again, what would happen is you wouldn't get the original window
	size; you would simply get the enlarged window with a bunch of buffer
	area.  Now instead of calling win_adjust_window(), I added code that
	preserves the "scale" factors, and the space between the emulation
	rectangle and the window client area.

	src/windows/winddraw.h:
	src/windows/winddraw.c:
	src/windows/wind3d.c:
	- Added a call win_ddraw_fullscreen_margins() that given the
	dwWidth and dwHeight parameters of a DirectDraw surface, comes up with a
	RECT that represents the margins.  For MAME, this is a very simple
	operation (a rectangle from (0,0)-(dwWidth,dwHeight)) (and the
	implementation that I've submitted is corrspondingly simple), but in
	MESS the window can have a menu bar so this operation works differently.
	I've also modified winddraw.c and wind3d.c to use this call.

	src/timer.c:
	src/timer.h:
	- Changes timers so that they are now 'mame_timer *' instead of
	'void *'.  mame_timer is an opaque structure, similar to mame_file in
	fileio.h.  I didn't update any code using timers yet; it doesn't seem to
	me that there isn't a rush and since they used 'void *' before,
	everything compiles just fine.  I hope noone objects to this one.

	src/vidhrdw/jaguar.c:
	- Fixes a potential crash in jaguar_tom_regs_w() (while this
	crash doesn't occur under normal execution of the cojag games though, it
	can occur if modified ROMs are being used).

	src/sound/fmopl.c:
	Added #if/#endif pair

Changed message for Double Clicking on EXE slightly [Leon van Rooij]
	src/windows/winmain.c


New games supported:
--------------------

Stadium Cross [Jason Lo]
	there seem to be some sound problems
	src/drivers/multi32.c
	src/drivers/system32.c
	src/vidhrdw/system32.c

R2D Tank [Pierpaolo Prazzoli]
	src/drivers/r2dtank.c

New TESTDRIVERS in source
-------------------------

Counter Run (bootleg)
	encrypted?
	src/drivers/freekick.c

New clones supported
--------------------

Some namco ones that were forgotten last time


# 0.70u4

## General Source Changes


Tilemap viewer now displays a more logical ordering for "tilemap#1", "tilemap#2", etc. [Phil Stroffolino]
Broadened support for palette_offset, and made it work in tilemap viewer [Phil Stroffolino]
	src/tilemap.c

Various Namco Improvements [Phil Stroffolino]
	- Improved Namco road emulation
	- Speedup when ROZ layer rapidly switches palettes (Metal Hawk 2nd stage)
	- Window registers in namconb1 sprite drawing (fixes the "Tank" stage of Point Blank
	- lots of consolidation/cleanup in vidhdrw/namcos2.c
	
	src/drivers/namcoic.c
	src/includes/namcoic.h
	src/drivers/namconb1.c
	src/vidhrdw/namconb1.c
	src/includes/namconb1.h
	src/drivers/namcos2.c
	src/vidhrdw/namcos2.c
	src/machine/namcos2.c
	src/includes/namcos2.h
	src/drivers/namcos21.c
	src/vidhrdw/namcos21.c
	src/drivers/namcos22.c
	src/vidhrdw/namcos22.c
	src/includes/namcos22.h
	src/vidhrdw/namcos3d.c
	src/includes/namcos3d.h

Silent Dragon Dipswitch Fixes [Brian A. Troha]
	src/drivers/taito_b.c

A Few Data East Rom Name Fixes / Board Info [Brian A. Troha]
	src/drivers/deco32.c
	src/vidhrdw/deco16ic.c


New games supported:
--------------------

Final Lap [Phil Stroffolino]
Suzuka 8 Hours 2 [Phil Stroffolino]
Four Trax [Phil Stroffolino]
	some gfx / sound problems but mostly working
	src/drivers/namcos2.c
	+ the other namco files listed above

Final Lap 2 [Phil Stroffolino]
Suzuka 8 Hours [Phil Stroffolino]
	not working due to bad / missing roms and / or incomplete emulation but enabled anyway
	src/drivers/namcos2.c
	+ the other namco files listed above


Final Lap 3 [Phil Stroffolino]
	not working due to incomplete emulation but enabled anyway
	src/drivers/namcos2.c
	+ the other namco files listed above

New TESTDRIVERS in source
-------------------------

Cyber Commando [Phil Stroffolino]
Air Combat 22 [Phil Stroffolino]
Cyber Cycles [Phil Stroffolino]
Time Crisis [Phil Stroffolino]
	very preliminary
	src/drivers/namcos22.c
	+ the other namco files listed above

R2D Tank [Pierpaolo Prazzoli]
	still being worked on
	src/drivers/r2dtank.c



# 0.70u3

## General Source Changes


Allowed Region to be selected via Dipswitch on Banpresto games in cave.c [David Haywood]
	src/drivers/cave.c

Message Box prompting the user to read windows.txt if MAME.EXE is double clicked on instead of being run
from the command line [Leon van Rooij]
	note, this still needs testing on Win9x, it works as expected on 2000/XP, if it creates new
	problems it might not be included.  Seems to work on 98
	src/windows/winmain.c

Fixed Colours in Mustache Boy with new Proms [Tomasz Slanina]
	src/drivers/mustache.c
	src/vidhrdw/mustache.c

Fixed Colours in Tricky Doc with new Proms [David Haywood]
	src/drivers/sauro.c

Added Colour Map to Enigma 2, swapped sets around, used a more sensible cpu freq [Tomasz Slanina]
	src/drivers/enigma2.c

Fixed Trackball controls in Country Club [Tomasz Slanina]
	src/drivers/snk.c

New games supported:
--------------------

Battle Cry [Tomasz Slanina]
	some tiny gfx glitches
	src/drivers/homedata.c
	src/vidhrdw/homedata.c
	src/includes/homedata.h

New clones supported
--------------------

Dock Man (parent of portman) [TheGuru]
	src/drivers/thepit.c


# 0.70u2

## General Source Changes


Fixed a few minor problems in the Wolf Pack Driver [Stefan Jokisch]
	src/drivers/wolfpack.c
	src/vidhrdw/wolfpack.c

Fixed ports in some TESTDRIVERS so there are no complaints with the Sanity Checks [smf]
	src/drivers/coolpool.c
	src/drivers/taitoair.c
	src/cpu/yms32025/tms32025.h
	
Various Sound Improvements to YM2610, YM2608, Y8950 [Jarek Burczynski]
	Inspired by Frits Hilderink (NLMSX emulator author) I attempted to:
	- improve support of the ymdeltat module for the status registers (YM2610,YM2608,Y8950),
	- implement x1bit/x8bit DRAM usage for the ADPCM samples,
	- implement memory read/write via deltat (Y8950 and YM2608)
	- ADPCM synthesis in 'slave' mode (CPU controlled, instead of memory based).

	I made some tests but the changes are quite big so please test them before including. I'm sure the
	Y8950 will be tested by NLMSX authors too.

	src/sound/ymdelta.c
	src/sound/ymdelta.h
	src/sound/fmopl.c
	src/sound/fmopl.h
	src/sound/fm.c
	src/sound/3812intf.c
	src/sound/3812intf.h
	
Some Dipswitch / Input Fixes [Curt Coder]
	src/drivers/twin16.c
	src/drivers/sega.c
	src/drivers/safarir.c
	src/drivers/playch10.c
	src/drivers/nmk16.c
	
Some Dipswitch / Input Fixes [El Condor]
	src/drivers/crimfght.c
	src/drivers/cps1.c
	src/drivers/contra.c
	src/drivers/astrocde.c
	src/drivers/8080bw.c
	
Some Dipswitch Fixes [I�igo Luja]
	src/drivers/toaplan2.c
	src/drivers/taito_b.c
	src/drivers/deniam.c
	src/drivers/sg1000a.c
	
Fixed i86 DAA opcodes + more [Nathan Woods]
	src/usrintrf.c:
	src/usrintrf.h:
	- Renames display_fps() to ui_display_fps() and makes it
	non-static
	
	src/windows/fileio.c:
	- In osd_fopen(), if a file is opened with mode "r",
	FILE_SHARE_READ will be passed to the dwShareMode parameter of
	CreateFile().  This allows other programs to read any files that MAME
	opens for read only.

	src/cpu/i86/instr86.c
	src/cpu/nec/nec.h
	- i86 DAA fixes

Corrected Green Beret Rom Names [G�rald]
	src/drivers/gberet.c

Preliminary MCU simulation for Paddle 2 [Angelo Salese]
	src/drivers/arkanoid.c
	
Fixed Audit with Optional Roms [Nathan Woods]
	src\audit.c
	src\audit.h
	
New games supported:
--------------------

Enigma 2 [Pierpaolo Prazzoli, Tomasz Slanina]
	No Sound (bad rom?) could be merged with 8080bw?
	src/drivers/enigma2.c


# 0.70u1

## General Source Changes


Fixed Night Striker dips [Brian A. Troha]
	src/drivers\taito_z.c
	
This fixes the data size parameter of the DUMP command in the debugger [smf]
This fixes the bug where SAVE only worked on 8bit cpus in the debugger [smf]
Made dbg_show_scanline the default [Bryan McPhail]
	src/mamedbg.c

Changed a few MAME core files for MESS (shouldn't change Mame behavior) [Nathan Woods]
	src/usrintrf.h:
	src/usrintrf.c:
	- Adds four new calls to control showing the fps count and the
	profiler:
	void ui_show_fps_set(int show);
	int ui_show_fps_get(void);
	void ui_show_profiler_set(int show);
	int ui_show_profiler_get(void);
	
	src/common.h:
	- Adds ROM_LOAD32_DWORD
	
	src/machine/idectrl.c:
	src/machine/idectrl.h:
	src/harddisk.c:
	src/harddisk.h:
	src/hdcomp.c:
	- Enhances the hard disk code to support sector sizes other than
	512 bytes (a non-IDE harddisk in MESS for one of the TI systems uses
	this).  src/machine/idectrl.c has been modified to take this into
	account; it requires that HD images be 512 bytes.  The new code is fully
	backwards compatible with old CHDs that do not have a sector size within
	them.  

Tiny changes to enable easy compiling under VC6/7 & older non-c99 compilers [Bryan McPhail]
	src/drivers/konamigx.c
	src/drivers/mystwarr.c
	src/hdcomp.c
	src/windows/fileio.c
	src/windows/wind3d.c
	
Konami Updates and Fixes (including fix for dbz / moo / simpsons crashes in 0.70) [Acho A. Tang]
	src/drivers/moo.c
	src/vidhrdw/moo.c
	src/drivers/xexex.c
	src/vidhrdw/xexex.c
	src/vidhrdw/konamiic.c
	src/vidhrdw/konamiic.h
	
Fixed some problems with the new alt. bios code [Chris Kirmse, Paul Priest]
	src/common.c
	src/info.c
	
Rewrote Wolf Pack driver based on schematics [Stefan Jokisch]
	src/drivers/wolfpack.c
	src/vidhrdw/wolfpack.c

D3D Code fixes / Improvements [Leon van Rooij]
	src/windows/wind3d.c

New games supported:
--------------------

Grand Tour [Tomasz Slanina]
	src/drivers/iqblock.c


# 0.70

This contains all the updates listed which were part of the source only diffs listed below
(0.69a, 0.69b, 0.69u3) plus a few extra bits and pieces.  A couple of drivers still appear to
be broken with the new timer system (deadang, artmagic for example) but hopefully most of the
problems were ironed out.

## General Source Changes


Konami GX Sound Improvements [R.Belmont]

General Konami Updates [Acho A. Tang, R. Belmont]

Various Dipswitch fixes / conversion of drivers to tilemaps [Curt Coder]

Removed hacks from Oriental Legend [Olivier Galibert]
	src\drivers\pgm.c

Some D3D updates / fixes [Leon van Rooij]

Sanity checks for Ports [Pierpaolo Prazzoli]
	src\mame.c
	
Documentation updates [Santeri Saarimaa, Randolph, Leon van Rooij]

Improved bios selection code, now -bios japan can be used with the NeoGeo driver for
example [Paul Priest]

New clones supported
--------------------

Korosuke Roller (clone of Crush Roller) [BUT, nono]
	src\drivers\pacman.c
	
New TESTDRIVERS in source
-------------------------

DJ Boy [Phil Stroffolino]
	not working due to protection
	src\drivers\djboy.c
	src\vidhrdw\djboy.c

# 0.69u3 (0.69 Update 3)

source diff, changed numbering system to ux instead of a,b,c because people were getting
confused and thinking mame069b.zip (mame 0.69 _b_inary) was the updated version when it
wasn't.

misc fixes and improvements this time, no changes related to the timer system so as far
as testing that goes this isn't an essential upgrade.

if you use this code in your own build of Mame please make sure you use the correct
version number to reflect the inclusion of this updated code.

## General Source Changes


Direct 3D update to change the way cleanstretch is handled [Leon van Rooij]
	-cleanstretch works like this:
	-cleanstretch auto: let blitter decide, the same as -nocs in 0.69 (default).
	-cleanstretch full: same as -cs in 0.69
	-cleanstretch none: disable (won't look good with scanlines)
	-cleanstretch horizontal: horz only
	-cleanstretch vertical: vert only
	
Added Ozon 1 Prom [Pierpaolo Prazzoli]
	src\drivers\galaxian.c
	
Added correct roms to Mighty Monkey (bootleg on Super Cobra Hardware) [Pierpaolo Prazzoli]
	src\drivers\scobra.c

Fixed some problems in the makefile for the xml2info program [Andrea Mazzoleni]
	makefile
	src\core.mak
	
Fixed crash in usclssic []
	src\drivers\seta.c

Fixed problem where controls stopped working after the first level in Tugboat [Tomasz Slanina]
	src\drivers\tugboat.c

Changed MULU timing in asm 68k core so that it matches the C ones [Razoola]
	src\cpu\m68000\make68k.c

Bug fix to loop addressing on YMF278B which helps stablise the tuning on looped samples
(noticable in parts of s1945 at least). [OpenMSX]
	src\sound\ymf278b.c
	
Fixed hedpanic sound banking []
	src\drivers\esd16.c
	
Corrected various clocks in the vball driver [TheGuru]
	src\drivers\vball.c

New clones supported
--------------------

Field Day (now the parent of The Undoukai) [Tomasz Slanina]
	src\drivers\40love.c

King of Fighters '98 (alt M1) [Razoola]
King of the Monsters (set 2) [Razoola]
	src\drivers\neogeo.c
	
Various CPS2 clones [Razoola]
	src\drivers\cps2.c
	
New TESTDRIVERS in source
-------------------------

Sparkman [Luca Elia]
	not working, protection
	src\drivers\suna8.c



# 0.69b (0.69 Update 2)

Another source diff update, this fixes some of the problems Mametesters discovered
with the new timer code, please test this well too, if anything else is broken which
worked in .69 let us know, it could be important.

## General Source Changes


Fixed Dipswitches and Inputs in Atlantic City Action / Broadwalk Casion [Peter Storey]
	src\drivers\pacman.c
	
Fixed a few Misc Typos [Santeri Saarimaa]
	src\drivers\metro.c
	src\drivers\oneshot.c
	src\drivers\lethalj.c
	
Added 'Right' in Moon Shuttle [David Haywood]
	src\drivers\galaxian.c

Various fixes to the PIC16C5x core. [Quench]
	src\cpu\pic16c5x\pic16c5x.c
	src\cpu\pic16c5x\pic16c5x.h
	src\cpuintrf.c
	
Sound working in BIGTWIN [Quench]
	src\drivers\playmark.c
	
The use of the PIC HEX dumps for DRGNMST and BIGTWIN [Quench]
	src\drivers\playmark.c
	src\drivers\drgnmst.c

Fixed a bug in the V60 CPU core causing it to break with the new timer system [Aaron Giles]
	src\cpu\v60\v60.c
	
Fixed a problem in snprintf.c printing out floats with fractional partswith leading
zeroes [Chris Kirmse]
	src\windows\snprintf.c

Fixed bug in K054539 causing bad vocals in Dadandarn [R.Belmont]
	src\sound\k054539.c
	
Added PGM Calender [Pierpaolo Prazzoli]
	src\drivers\pgm.c
	
Fixed a problem which caused sound to break in games using a YM2610 using the new timer
system [Aaron Giles]
	src\cpuint.c
	
Changed NeoGeo to use boost_interleave for tight CPU communications instead of throwing
cyces away [Aaron Giles]
	src\drivers\neogeo.c

New games supported:
--------------------

Space Cruiser [Tomasz Slanina]
	src\drivers\taitosj.c

Mustache Boy [Tomasz Slanina, Nicola Salmoria]
	previously a TESTDRIVER, decryption completed by Nicola
	still bad colours and no sound due to missing roms / mcu?
	src\drivers\mustache.c
	src\vidhrdw\mustache.c
	
Evil Stone [Tomasz Slanina]
	src\drivers\taito_l.c
	
Glass [Manuel Abadia]
	this isn't playable due to protection but I've enabled it anyway
	src\drivers\glass.c
	src\vidhrdw\glass.c
	
Tri-Pool [David Haywood]
	src\drivers\jack.c

New clones supported
--------------------

Final Fight (Japan Set 2) [Razoola]
	src\drivers\cps1.c
	src\vidhrdw\cps1.c

# 0.69a (0.69 Update 1)

This version is provided as a source diff only, more releases like this may follow
depending on how this one goes down, the intention is to allow people to have the
latest code without the hassle and bandwidth requirements of a new binary.

If you can't compile this then don't worry, normal releases will still happen, this
doesn't change anything as normal release plans are concerned, please don't hassle
whoever maintains your favourite port of Mame to add the updates here, not everybody
is going to have time to make new binaries on a regular basis.

## General Source Changes


Fixed eeprom in Head Panic [Paul Priest]
	src\drivers\esd16.c

Few tweaks/minor optimizations to MIPS3 Dynacrec [Aaron Giles]
	src\x86drc.c
	src\x86drc.h
	src\cpu\mips\mips3drc.c

Improved Save-State support for NeoGeo games [Mame32Plus]
Changed NeoGeo Resolution to be 320 wide, this is correct, the glitches it causes
are verified against a real MVS [David Haywood]
Verified and corrected clock speeds etc. for NeoGeo [TheGuru]
	src\drivers\neogeo.c
	src\machine\neogeo.c
	src\vidhrdw\neogeo.c
	src\includes\neogeo.h
	
Removed analog input definition and encoded it directly from
Joystick input in Metal Soldier Isaac II [Jarek Burczynski]
	src\drivers\msisaac.c

IDE Update adding support for a few things needed in the future [Aaron Giles]
	src\machine\idectrl.c
	src\machine\idectrl.h

TMS32010 BIO fix [Aaron Giles]
	src\cpu\tms32010\tms32010.h
	
C 68k core is now default, this fixes collisions in Assault []

Fixed bug causing Timings to be broken when using D3D [Leon van Rooij]
	src\windows\wind3d.c

Optional XML output and xml to info converter (see xmlinfo.txt) [Andrea Mazzoleni]
	use -listxml, converter isn't fully set up yet.
	src\info.c
	src\info.h
	src\windows\fronthlp.c
	src\xml2info\*

A number of Changes to the Timer System [Aaron Giles]

	All the CPU scheduling has moved into cpuexec.c and been taken out of
	timer.c. A number of bugs relating to time tracking for suspended CPUs have
	been fixed.

	A side effect of all this is a change in the behavior of the CPU scheduling.
	For the most part, games are fine with this. However, there were a few
	exceptions:

	* taitosj.c: timing of the MCU communication broke
	* arkanoid.c: timing of the MCU communication broke
	* harddriv.c: stunrun timing glitches, racedriv/steeltal/hdrivair broken
	
	(There may be more, but I haven't found them yet.)

	The first two problems have been fixed, as has racedriv, but steeltal,
	hdrivair, and stunrun still have issues.
	
	Oh, one other useful function I added is this:

	   cpu_boost_interleave(double timeslice_duration, double boost_duration);

	You call this if you need to temporarily crank up the interleave for a short
	period of time in order to get CPU synchronization working. I used this in
	arkanoid and taitosj to get the MCU communications working. For example, you
	could say:

	   cpu_boost_interleave(TIME_IN_USEC(1), TIME_IN_USEC(100));

	which would interleave the CPUs on 1 usec boundaries for the next 100 usecs.
	Even more useful is this:

	   cpu_boost_interleave(0, TIME_IN_USEC(10));

	A very small value (0) for the first parameter means "use perfect
	synchronization", which should guarantee cycle-accurate synchronization for
	the specified duration. Note that this is *really* slow, so you don't want
	to be doing it too much!

	Let us know ASAP if anything else is broken as a result of this update
	
	src\cpuintrf.c
	src\cpuintrf.h
	src\cpuexec.c
	src\cpuexec.h
	src\timer.c
	src\timer.h
	src\drivers\taitosj.c
	src\machine\taitosj.c
	src\machine\arkanoid.c
	src\drivers\harddriv.c
	src\machine\harddriv.c
	src\sndhrdw\harddriv.c
	
New games supported:
--------------------

Guts'n [Paul Priest]
	src\drivers\suprnova.c
	src\vidhrdw\suprnova.c
	
Dragonball Z [R.Belmont]
	src\drivers\dbz.c
	src\vidhrdw\dbz.c
	src\vidhrdw\konamiic.c
	src\vidhrdw\konamiic.h

Ozon I [Pierpaolo Prazzoli]
	bad colours due to missing prom
	src\drivers\galaxian.c

New clones supported
--------------------

King of Fighters '95 (set 2) [Jarek Burczynski]
	note, we use the new v1,c1,c2,c3,c4 from this set in the parent too
	src\drivers\neogeo.c
Kyros No Yakata (Japan) [David Haywood]
	src\drivers\alpha68k.c
Ambush (Tecfri) [David Haywood]
	src\drivers\ambush.c
Frisky Tom (set 2) [David Haywood]
	src\drivers\seicross.c

New TESTDRIVERS in source
-------------------------

Photo Y2K [ElSemi]
	unemulated protection
Stadium Cross [David Haywood]
	graphic problems, no controls
	src\drivers\multi32.c


# 0.69

Mainly a bugfix release for problems discovered with new features in 0.68,
it also further clears the backlog.  Put together by David Haywood, 

If you submit changes please make sure they're up to date with the latest
source code, if not it means more work for us in sorting them out.

Several people have asked about the SHA1Merger program for updating their
own drivers, if you need this it should be on www.mame.net within a few
hours of this release.

Note, we are aware of some problems with the Direct 3D core which cause
some games to function incorrectly (Rolling Thunder title screen, Hard
Drivin' Dashboard, Puyo Puyo title screen raster etc.) if you have problems
with games when using -d3d try them without before reporting a bug. (although
please do report games broken with -d3d if you find them but make sure to
specify the problem is with -d3d)

The cause of this problem seems to lie somewhere in

result = IDirect3D7_CreateDevice(d3d7, &IID_IDirect3DHALDevice, back_surface, &d3d_device7);

however we don't know how to fix this nor why it happens, could the real problem
be elsewhere?

## General Source Changes


Fixed a problem with missing 'returns' in the PIC CPU core [Bryan McPhail]
	src\cpu\pic16c5x\pic16c5x.c

Fixed a few MCU simulation problems in bking3 [Angelo Salese]
	src\drivers\bking2.c

Various changes [Nathan Woods]

	src/cheat.c:
	- Minor change to the MESS specific code

	src/datafile.c:
	- DriverDataCompareFunc() is now declared with CLIB_DECL

	src/hash.c:
	- Removed strupr() in favor a manual implementation, because it
	is non-ANSI

	src/usrintrf.c:
	- Minor tweaks within #ifdef MESS code
	- UI code for discrete sound engine now encapsulated within '#if
	HAS_DISCRETE'

	src/ui_text.c:
	src/ui_text.h:
	- Changed some #ifdef MESS specific strings

	src/windows/rc.c:
	src/windows/rc.h:
	- Fixed a potential crash that could occur if the last line in
	an INI file was not terminated with a CRLF
	- Added osd_rc_write(); which does the reverse as osd_rc_read()
	(this is only used in the MESS Win32 port)

	src/fileio.c:
	src/fileio.h:
	- New mame_fputs(), mame_vfprintf() and mame_fprintf() functions
	(used in osd_rc_write()) above
	
	src/windows/fileio.c:
	- Removed some no longer relevant MESS globals
	- Unicode enabled (i.e. - compiles under #define UNICODE)

	src/windows/winalloc.c:
	- Added corresponding _msize implementation (this fixes a benign
	but very subtle bug when debugging with VC)

	src/idectrl.c:
	src/idectrl.h:
	- Added ide_controller_init_custom() (used by MESS)

	src/fileio.c:
	src/hash.c:
	src/windows/fronthlp.c:
	- Removed private prototypes of crc32(), in favor of #include <zlib.h>

	src/windows/fronthlp.c:
	- Minor tweaks within #ifdef MESS

	src/sha1.h:
	- Put #ifndef _STDINT_H around typedefs for uint32_t and uint8_t
	(apparently these are already defined on the Mac)

	src/windows/winalloc.c:
	src/windows/winmain.c:
	- Put CLIB_DECL in front of a few declarations


Various Changes [Andrea Mazzoleni]

	src/info.c
	- Substitutes in the -listinfo output the strings "nodump" and "baddump"
	  with "flags nodump" and "flags baddump".

	src/sound/disc_mth.c
	- Substitutes the _HUGE macro with the standard DBL_MAX value
	
	src/cpu/tms9900/9900stat.h
	src/cpu/tms9900/99xxstat.h
	- Adds a test for the GNU C compiler to skip some POWERPC inline assembler
	  not compatible with the GNU assembler format.

	src/x86drc.h
	- Fixes some macros with arguments not used. Otherwise it doesn't compile
	  with gcc 2.95.3.

Auditing Fixes for cases where no good dump is known but a file is found [BUT]
	src\audit.c
	
Improvements and Bug Fixes to the Direct 3D Blitting [Leon van Rooij]

	-effect same as 0.68 and earlier, no effect on d3d module
	-zoom or -z (default is 2) select zoom level, works for the ddraw code as well
	find the smallest display mode that will fit an n times zoomed game image
	(only used when no display mode is explicitly forced,
	the d3d code takes the aspect ratio into account, ddraw behaviour is 
	unchanged)
	-cleanstretch or -cs only strecth to integer ratios, works for the ddraw 
	code as well

	-d3dfilter or -d3dflt select filtering algorithm
	0 is none, 1 bi-linear, 2 flat bi-cubic, 3 gaussian bicubic, 4 anisotropic
	-d3deffect behaves exactly the same as -effect, but only affects the d3d module
	You can use all -effect parameters, so none, scan25, scan50, scan75,
	rgb16, rgb6, rgb4, rgb3, and sharp work, they are identical at 2x zoom
	scan75v and rgb4v also work, and are identical to scan75 adn rgb4
	(as the d3d effects are automatically rotated already)
	Additionally, all effects presets and auto work
	none affects only the presets now.
	The presets are renamed, in addition to the -effect presets, they are now:
	rgbminmask,
	dotmedmask,
	rgbmedmask,
	rgbmicro
	rgbtiny
	aperturegrille,
	dotmedbright,
	rgbmaxbright

	-d3dscan scanline intensity, 100 is off
	-d3dfeedback feedback intensity, 0 is off
	-d3dsaturate oversaturation intensity, 0 is off
	-d3dprescale AKA sharp.
	-d3dcustom custom preset, as before
	(anyone who wants to customise presets can, everybody else should ignore it)
	-d3dexpert Normal users should not use this one, basically if you can't 
	read the
	source to figure out what it does, you have no business using it <g>
	Behaves as -effect did in the previous updates, plus some new stuff;
	disable disables all effects,
	prescale lets you control prescale explicitly,
	pixelcounter changes the scanline texture
	(use mame whatever -z 4 -d3d -cs -flt 0 -d3dexpert pixelcounter),
	norotate disables rotating effects
	
	It has it's own ini section and it handles priorities itself to ensure 
	correct behaviour. For custom presets, patterns are no longer loaded from 
	the effects directory, the name should just include the path.
	
	There are warning messages when you do e.g. -nod3d -d3dscan50, or -d3d 
	-effect sharp (although the latter will work as it has up until 0.68). It 
	tests all hardware capabilities it should (as far as I'm aware, at least). 
	It won't chop textures in 256x256 blocks for old Voodoo cards. 
	-texture_management is now enabled by default as leaving it off seems to 
	have worse consequences for more people than having it on. I haven't added 
	a workaround for cards that can't render to textures yet, other than that, 
	any possible typos, etc. or any suggestions people may have, it should be 
	good for 0.69.
	
	Apparently some videocards don't like the DX7 pageflip in combination with 
	a triple buffer. (behaviour when triple buffering is unchanged from 0.67, 
	that goes for the d3d code as well as the ddraw code).	
	
	src\windows\video.c
	src\windows\video.h
	src\windows\wind3d.c
	src\windows\wind3d.h
	src\windows\win3dfx.c
	src\windows\win3dfx.h
	src\windows\windraw.c
	src\windows\windraw.h
	src\windows\window.c
	src\windows\window.h
	src\windows\d3d_extra.h
	src\windows\windows.mak
	
Added USA 2-Slot bios to NeoGeo Driver (recompile to use) also readded line removed
by mistake, fixed coinage bug when using USA Bios []
	src\drivers\neogeo.c
	src\machine\neogeo.c
	
Added Asia Bios to NeoGeo Driver []
	src\drivers\neogeo.c
	
Renamed Proms in Eggor [David Haywood]
	src\drivers\pacman.c

Fixed Demo Sound Dipswitch in sotsugyo [Fujix]
	src\drivers\funkyjet.c

Fixed Infinite Lives Dipswitch in qtono2 [BUT]
	src\drivers\cps1.c
	
Various Fixes and Improvements to Chack'n Pop [BUT]
	src\drivers\chaknpop.c
	src\machine\chaknpop.c
	src\vidhrdw\chaknpop.c
	
Fixed the Sound Loading in some Taito F3 Games [Paul Priest, Ryan Holtz]
	src\drivers\taito_f3.c

Corrected Dragon Master Dipswitches and Frequencies [David Silva, Guru]
	src\drivers\drgnmst.c

Added NeoGeo Mahjong control panel support [David Haywood]
	src\drivers\neogeo.c
	src\machine\neogeo.c

Corrected sprite priority in brkthru and darwin and converted the driver to use
tilemaps [Satoshi Suzuki, David Haywood]
	src\drivers\brkthru.c
	src\vidhrdw\brkthru.c

Fixed a bug with -crconly which could cause options.debug_width to become
corrupt [Paul Priest]
	src\mame.h

Fixed a problem where a NODUMP rom was reported as found even if it didn't
exist in the rompaths [HobbesAtPlay]
	src\fileio.c

Corrected Colours in Progres & added 3rd Button for Aiming [Uki]
	src\drivers\timelimt.c
	
Fixed Length of Proms in Main Event (SNK) TESTDRIVER [David Haywood]
	src\drivers\mainsnk.c

Fixed Merging of Sets in Shougi TESTDRIVER [Toby Broyad]
	src\drivers\shougi.c

Fixed Name of Mach Breakers (was Match Breakers) [MameTesters]
	src\drivers\namconb1.c
	
Fixed Interrupt being triggered on Non-Existant CPU for TaitoSJ games with no
MCU [Aaron Giles]
	src\machine\taitosj.c
	
Added -bios option [Paul Priest]

Adds the system option "bios". An integer that is valid where: 0 <= n < 15

	Rom loading is now modified so that if any of the 4 bits in
	ROM_BIOSFLAGSMASK of the rom flags are set, it will only be loaded if
	the value+1 == options.bios

	NeoGeo has been modified to take advantage of this as follows:
	0 - "sp-s2.sp1"    - Europe, 1 Slot (also been seen on a 4 slot) (default)
	1 - "sp-s.sp1"     - Europe, 4 Slot
	2 - "usa_2slt.bin" - US, 2 Slot
	3 - "sp-e.sp1"     - US, 6 Slot (V5?)
	4 - "asia-s3.sp1"  - Asia S3 Ver 6
	5 - "vs-bios.rom"  - Japan, Ver 6 VS Bios
	6 - "sp-j2.rom"    - Japan, Older
	
	Note that using a value >6 and <15 will result in NO bios rom being
	loaded (And a red screen in NeoGeo's case).
	
	src\common.c
	src\common.h
	src\drivers\neogeo.c
	src\mame.h
	src\windows\config.c

Corrected Filenames for Steel Gunner 2 [Brian Troha]
	src\drivers\namcos2.c
	
Fixed Scrolling bug in One Shot One Kill [Angelo Salese]
	src\vidhrdw\oneshot.c

Fixed Mcu Reset for Tiger Heli [Angelo Salese]
	src\machine\slapfght.c

Removed a graphic rom loading kludge in Yamato,which caused a bad gray square
when launching missiles(yamato37b9gre);also fixed crashes in clone
yamato2. [Angelo Salese]
	src\drivers\yamato.c

Removed NeoGeo Memory Card Manager RESET / CALL option from UI, this feature is
only available on the AES console and should be in Mess (and probably won't be
needed at all with a real AES bios) []
	src\ui_text.c
	src\ui_text.h
	src\usrintrf.c

Changed NeoGeo Framerate to 59fps based on Mametesters report about Kof98 Intro Sync
It would be nice if somebody could give a real accurate measurement of this []
	src\drivers\neogeo.c

Fixed FM bug causing bad sounds in maglord and other games which was introduced in
the last release [Jarek Burczynski]

Discrete Sound update for several games [Derrick Renaud]

	- Atari Baseball - added video invert (triples/home runs).
	- Avalanche - changed to discrete sounds adding missing top rocks sound
	(Aud0 trigger). (and yes the game is supposed to sound horrible)
	- Dominos - added 60Hz ac signal (but it does not seem to affect anything).
	- Drag Race - added discrete sounds.
	- Night Driver - added discrete sounds and video invert during crash.
	Motor/skid sounds are not 100% correct.
	- Sprint 2 - switched player controls so they lined up under the proper
	score color.
	- Orbit - added discrete sounds.

	- [but] fixed the 'no stereo' bug in src\sound\discrete.c
	- I also fixed a minor bug in src\sound\discrete.h causing
	DISCRETE_LOGIC_INVERT not to work.
	- And did minor code cleanup to Basketball.
	- Removed GAME_IMPERFECT_SOUND flag from Circus.  If correct samples are
	used then it is ok, like all sample games.  (New accurate samples have been
	posted at twisty's)
	
	src\machine\atarifb.c
	src\drivers\avalnche.c
	src\machine\avalnche.c
	src\drivers\bsktball.c
	src\drivers\circus.c
	src\sound\discrete.c
	src\sound\discrete.h
	src\drivers\dominos.c
	src\machine\dominos.c
	src\includes\dominos.h
	src\drivers\dragrace.c
	src\drivers\nitedrvr.c
	src\machine\nitedrvr.c
	src\includes\nitedrvr.c
	src\drivers\orbit.c
	src\drivers\sprint2.c
	
Preliminary WIP Discrete Sound in Subs [Derrick Renaud]
	src\drivers\subs.c
	src\machine\subs.c
	src\vidhrdw\subs.c

Correct Colours in Clash Road [Luca Elia]
	src\drivers\clshroad.c
	src\vidhrdw\clshroad.c


New games supported:
--------------------

Wolf Pack (prototype) [Stefan Jokisch]
	graphic emulation might not be perfect, no sound
	src\drivers\wolfpack.c
	src\vidhrdw\wolfpack.c
Naughty Mouse [Dave Widel]
	src\drivers\pacman.c
Broadwalk Casino / Atlantic City Action [Dave Widel]
	this doesn't appear to be a gambling game
	src\drivers\pacman.c
	src\includes\pacman.h
	src\machine\acitya.c
Paradise [Luca Elia]
	there seems to be a problem with the hi-score system
	src\drivers\paradise.c
	src\vidhrdw\paradise.c
	src\includes\paradise.h

New clones supported
--------------------

Metamorphic Force (Japan JAA) [GMB]
	src\drivers\mystwarr.c
GI Joe (Japan) [GMB]
	src\drivers\gijoe.c
Falcon (Phoenix bootleg) [Smitdogg]
	src\drivers\phoenix.c
Mr. Do's Castle (older) [Lee Taylor]
	src\drivers\docastle.c
Police Trainer 1.3B (Sharpshooter Hardware) [Brian Troha]
	fails rom check, does so on original hardware too
	src\drivers\policetr.c
Street Fighter 2 (USA 911101) [David Haywood]
	src\drivers\cps1.c
	src\vidhrdw\cps1.c
Make Trax (set 2) [Dave Widel]
	src\drivers\pacman.c
NewPuc2 (hack) [Dave Widel]
	this hack was common in the arcades, it has intermissions between each
	maze and a slot machine every 3 mazes.
	src\drivers\pacman.c
World Class Bowling (v1.66, v1.65) [Brian Troha]
	v1.66 is now the parent
	src\drivers\itech32.c
Targ (cocktail?) []
Rallys (bootleg? of Spectar) []
	src\drivers\exidy.c
Route 16 (set 2) []
	src\drivers\route16.c
Labyrinth Runner (World Ver. K) []
	src\drivers\labyrunr.c
The Main Event (2 Players ver. X) []
	src\drivers\mainevt.c
Quartet 2 (Japan) []
Flash Point (Japan, bootleg) []
	no, these have nothing to do with the system16 rewrite
	src\drivers\system16.c
Chopper [US set 2 & set 3] []
	src\drivers\snk.c
	src\vidhrdw\snk.c
Tunnel Hunt (Atari Original) [David Haywood]
	this is now the parent, old set is tunhuntc
	src\drivers\tunhunt.c
Arkanoid (US, older) [Stephane Humbert]
	src\drivers\arkanoid.c
	
New TESTDRIVERS in source
-------------------------

Polygonet Commanders [R.Belmont]
	only text layer is emulated, hangs waiting for network, DSP used for 3D
	graphics isn't emulated
	src\drivers\plygonet.c
	src\vidhrdw\plygonet.c
Mahjong Daireikai [David Haywood]
Mahjong Channel Zoom In [David Haywood]
Mahjong Kakumei [David Haywood]
Mahjong Kakumei 2 - Princess League [David Haywood]
	these just load the roms and decode the gfx for now, they might be
	similar to nmk16.c just incase somebody wants to finish them.
	src\drivers\jalmah.c
Slipstream [David Haywood]
	doesn't work due to unemulated v60 opcodes
	src\drivers\system32.c
Atlus Print Club (v1, v2, v4, v5)
	mainly for reference, it might not be possible to emulate them as they 
	need a camera + printer
	src\drivers\segac2.c
Portraits [Steve Ellenoff & Peo]
	preliminary driver, still being worked on
	src\drivers\portrait.c
	src\vidhrdw\portrait.c
Mustache Boy [Tomasz Slanina]
	preliminary driver, gfx not fully decrypted, missing proms, no sound
	src\drivers\mustache.c
	src\vidhrdw\mustache.c
	src\sndhrdw\seibu.c

# 0.68

Note, this Release was put together by David Haywood, some of it is
untested but most of it seems to work.

## General Source Changes


SHA1 hash support, including various new command line options [Farfetch'd]
	src\too many to list ;-)

Mame Blitter Generation Code [Phil Stroffolino]
	src\mameblit.c
	src\mame.c
	src\mame.h
	src\drawgfx.c
	src\blitgen.c

Basic Direct3D blitting support [Leon van Rooij]

You need DirectX7 headers and libs to compile it, I'm using the set from
http:\\caesar.logiqx.com\html\tools\compilers\mingw.shtml which has the
original MS headers (the same set is used for Allegro and FB Alpha) and
I've made a minor update to the ddraw code so that it compiles with this
set of headers (VC++ should still be fine as well).

The code manually imports the DirectDrawCreateEx() function so MAME
compiled with d3d support will (or rather should, I've not actually tested
that yet) still run on PC's with older versions of DirectX. It should work
with pretty much any 3D hardware (except perhaps older Voodoo-based cards),
and the speed should be similar to the DirectDraw blit (a bit faster for
some older cards).

-direct3d or -d3d: use Direct3D (default is off, overrides -dd);
-filter or -flt: use bi-linear filtering (default is on);
-texture_management: use DirectX texture management (default is off, but
 some 3D cards need this (however, there's a small-ish speed-penalty)).
	src\windows\windows.mak
	src\windows\window.h
	src\windows\window.c
	src\windows\windraw.h
	src\windows\windraw.c
	src\windows\wind3d.h
	src\windows\wind3d.c
	src\windows\video.h
	src\windows\video.c
	
Various Konami Related Fixes and Improvements [Acho A. Tang, R. Belmont]
	src\tilemap.c
	src\palette.c
	src\drivers\mystwarr.c
	src\vidhrdw\mystwarr.c
	src\vidhrdw\konamiic.c
	src\vidhrdw\konamiic.h
	src\machine\konamigx.h
	src\drivers\konamigx.c
	src\machine\konamigx.c
	src\vidhrdw\konamigx.c
	src\drivers\xexex.c
	src\vidhrdw\xexex.c
	src\sound\ko54539.c
	src\sound\ko54539.h
	src\drawgfx.c
	src\blitgen.c
	
Various System 32 Updates (Priority, SVF Pitch etc.) [Acho A. Tang, Jason Lo, David Haywood]
	Note, theres a chance some things may be broken
	src\drivers\multi32.c
	src\drivers\system32.c
	src\vidhrdw\system32.c
	
Sound in Macross Plus & Quiz Bisyoujo Senshi Sailor Moon [Bryan McPhail]
	src\drivers\macrossp.c
	src\sound\es5506.c

Fixes for Crashes introduced in last version
	src\drivers\namcos1.c
	src\drivers\namcos2.c
	src\drivers\shangha3.c

+ plenty of other things I just don't have time to list.
	
New games supported:
--------------------

Chack'n Pop [BUT]
	src\drivers\chaknpop.c
	src\machine\chaknpop.c
	src\vidhrdw\chaknpop.c

Salamander 2 [Acho A. Tang, R. Belmont]
	Some protection issues remain (ships in attract mode)
Tokimeki Memorial Taisen Puzzle-dama [Acho A. Tang, R. Belmont]
Dragoon Might [Acho A. Tang, R. Belmont]
	src\ see list in 'Various Konami Updates' above
Koi Koi Shimasyo 2 - Super Real Hanafuda [Luca Elia]
Vasara []
Vasara 2 []
	src\drivers\ssv.c
	src\vidhrdw\ssv.c
	src\includes\seta.h
Billiard Academy Real Break [Luca Elia]
	src\drivers\realbrk.c
	src\vidhrdw\realbrk.c
	src\includes\realbrk.h
	src\drivers\seta2.c
	src\machine\tmp68301.c
	src\machine\tmp68301.h
	src\sound\ymz280b.c
	src\sound\ymz280b.h
Cannonball [Jarek Burczynski]
	some gfx problems (Bad ROMs?)
	src\drivers\cclimber.c
Kick Start Wheelie King [Tomasz Slanina]
	previously a testdriver, emulation completed
	src\drivers\taitosj.c
	src\vidhrdw\taitosj.c
Truco-Tron [Ernesto Corvi]
	src\drivers\truco.c
	src\vidhrdw\truco.c
(Super) Ground Effects [David Graves, Bryan McPhail]
	src\drivers\groundfx.c
	src\vidhrdw\groundfx.c
	src\vidhrdw\taitoic.c
	src\drivers\undrfire.c
Horizon [smf, David Haywood]
	src\drivers\m62.c
	src\vidhrdw\m62.c
Fire Battle [Martin Pugh]
	previously a testdriver, patch added
	src\drivers\clshroad.c
Rapid Hero [David Haywood]
	src\drivers\nmk16.c
	src\vidhrdw\nmk16.c
Zero Point 2 [Luca Elia]
	src\drivers\unico.c
	src\vidhrdw\unico.c
	src\includes\unico.h
Idol Janshi Su-Chi-Pie 2 [David Haywood]
	src\drivers\ms32.c
	src\vidhrdw\ms32.c
Quiz Ghost Hunter [Olivier Galibert]
Tokoro San no MahMahjan [Olivier Galibert]
Tokoro San no MahMahjan 2 [Olivier Galibert]
Quiz Mekiromeki Story [Olivier Galibert]
	src\drivers\system24.c
	src\vidhrdw\system24.c
	src\machine\system24.c
	src\includes\system24.h
	src\vidhrdw\segaic24.c
	src\vidhrdw\segaic24.h
Straight Flush [Tomasz Slanina]
	2 player mode isn't working
	src\drivers\8080bw.c
	src\vidhrdw\8080bw.c
	src\includes\8080bw.h
SD Gundam Neo Battling [Luca Elia]
	src\drivers\seta.c
Fantasy Land [Luca Elia]
Galaxy Gunners [Luca Elia]
	src\drivers\fantland.c
	src\vidhrdw\fantland.c
Metal Soldier Isaac II [Angelo Salese]
	previously a testdriver, mcu simulation added
	src\drivers\msisaac.c
Progress [David Haywood]
	Bad Colours due to missing Proms
	src\drivers\timelimt.c
Birdie King 3 [Angelo Salese]
	src\drivers\bking2.c
Dark Tower [Bryan McPhail, David Haywood]
	dips need mapping correclty, default coinage is bad
	src\drivers\ddragon.c
Tricky Doc [Reip, David Haywood]
	Bad Colours due to missing Proms
	src\drivers\sauro.c
	src\vidhrdw\sauro.c
Tournament Table [Stefan Jokisch]
	src\drivers\tourtabl.c
	src\machine\6532riot.c
	src\machine\6532riot.h
	src\sound\tiaintf.c
	src\sound\tiaintf.h
	src\sound\tiasound.c
	src\sound\tiasound.h
	src\vidhrdw\tia.c
Eggor [Tomasz Slanina]
	src\drivers\pacman.c
Rock Climber [Tomasz Slanina]
	src\drivers\galaxian.c
	src\vidhrdw\galaxian.c
	src\includes\galaxian.h
Kung-Fu Taikun [Tomasz Slanina]
	src\drivers\wiz.c
Metal Clash [Luca Elia]
	src\drivers\metlclsh.c
	src\vidhrdw\metlclsh.c
Head Panic
	eeprom needs hooking up
	src\drivers\esd16.c
	src\vidhrdw\esd16.c
Puckman Pockimon [Luca Elia]
	src\drivers\segac2.c
	src\vidhrdw\segac2.c
Operation Wolf 3 [David Graves, Bryan McPhail]
	src\drivers\slapshot.c
Sky Army [Ryan Holtz]
	src\drivers\skyarmy.c
Super Doubles Tennis [Bryan McPhail]
	src\drivers\btime.c
Egg Venture [Aaron Giles]
Lethal Justice [Aaron Giles]
	src\drivers\lethalj.c
	src\vidhrdw\lethalj.c
	src\includes\lethalj.h
Area 51 \ Maximum Force Duo [Aaron Giles]
	src\drivers\cojag.c
Prebillian [Tomasz Slanina]
	src\drivers\pbillian.c
	src\vidhrdw\pbillian.c
	src\sndhrdw\pbillian.c
Bwings [Acho A. Tang]
Zaviga [Acho A. Tang]
	src\drivers\bwing.c
	src\vidhrdw\bwing.c
Wall Crash [Jarek Burczynski]
	src\drivers\wallc.c
Mighty Pang [Razoola]
	src\drivers\cps2.c
	src\vidhrdw\cps1.c
J-League Soccer V-Shoot [Phil Stroffolino]
	src\drivers\namconb1.c
Rotary Fighter [Barry Rodewald]
	src\drivers\rotaryf.c
The Deep \ Run Deep [Luca Elia]
	src\drivers\thedeep.c
	src\vidhrdw\thedeep.c


New clones supported
--------------------

Lethal Enforcers 2 (USA) [Brian Troha]
	src\drivers\konamigx.c
Violent Storm (Europe) [David Haywood]
	src\drivers\mystwarr.c
Mello Yello Q*Bert [David Haywood]
	src\drivers\gottlieb.c
Big Bang (clone of Thunder Dragon 2) [David Haywood]
Vandyke (Jaleco) [David Haywood]
	src\drivers\nmk16.c
BlockBuster (clone of Mr Jong) [David Haywood]
	src\drivers\mrjong.c
Catsbee (clone of Galaga) [David Haywood]
	src\drivers\galaga.c
	src\vidhrdw\galaga.c
Croquis (clone of Logic Pro) [David Haywood]
	src\drivers\deniam.c
Super Pinball Action (US) [David Haywood]
	src\drivers\spbactn.c
Pollux (set 2) [David Haywood]
	src\drivers\dooyong.c
Vautour (clone of Phoenix) [Chris Hardy]
	src\drivers\phoenix.c
Moon Alien (clone of Galaxian) [David Haywood]
	src\drivers\galaxian.c
Country Club (clone of Fighting Golf) [Tomasz Slanina]
	src\drivers\snk.c
DoDonPachi (International Version) [Brian Troha]
	src\drivers\cave.c
Batrider (Korea) [Brian Troha]
	src\drivers\toaplan2.c
Vs Block Breaker (clone of sarukani) [David Haywood]
	src\drivers\suprnova.c
	src\vidhrdw\suprnova.c
Ring Fighter (clone of Vs. Gong Fight) [David Haywood]
	src\drivers\tsamurai.c
Space Panic (various alt sets) [Zsolt]
	src\drivers\cosmic.c
Piranha (GL sets) [Dave Widel]
	src\drivers\pacman.c
Hydra (prototype set 2) [Aaron Giles]
	src\drivers\atarig1.c
Millpac (clone of centipede) [Chris Hardy]
	src\drivers\centiped.c
Some Cps1 Clones [Razoola]
	src\drivers\cps1.c
Bang Bead (Release Version) [Various]
	src\drivers\neogeo.c
Terminator 2 (LA1) [Brian Troha]
	src\drivers\midyunit.c
Various NeoGeo Clones [Razoola]
	src\drivers\neogeo.c
	src\machine\neogeo.c

	
New TESTDRIVERS in source
-------------------------

Shougi [Jarek Burczynski, Tomasz Slanina]
Shougi 2 [Jarek Burczynski, Tomasz Slanina]
	Not Working due to Protection MCU
	src\drivers\shougi.c
Konami's Open Golf Championship [Acho A. Tang, R. Belmont]
	Missing ROZ layer, always thinks the ball is in the water
	src\drivers\konamigx.c
F1 Grand Prix Star 2 [Luca Elia]
	Missing \ Bad ROMs
	src\drivers\cischeat.c
	src\vidhrdw\cischeat.c
Ultra Weapon X [Luca Elia]
	problems with v60 interrupts
Joryuu Syougi Kyoushitsu [Luca Elia]
	unemulated CPU
	src\drivers\ssv.c
	src\vidhrdw\ssv.c
Hot Rod [Olivier Galibert]
Bonanza Bros [Olivier Galibert]
Quiz Rouka Ni Tattenasai [Olivier Galibert]
	various problems with the System 24 emulation
	src\drivers\system24.c
	src\vidhrdw\system24.c
	src\machine\system24.c
	src\includes\system24.h
	src\vidhrdw\segaic24.c
	src\vidhrdw\segaic24.h
Thunder Strike
	src\drivers\ddragon.c
Hot Smash [Tomasz Slanina]
	MCU Protection problems
	src\drivers\pbillian.c
	src\vidhrdw\pbillian.c
	src\sndhrdw\pbillian.c
Raiden 2 [Bryan McPhail]
	Protected and Encrypted Sprites
	src\drivers\raiden2.c
Grand Tour [David Haywood]
	Protection?
	src\drivers\iqblock.c
Great Wall
	Needs work on vidhrdw (none done)
	src\drivers\grtwall.c
China Dragon
	Needs work on vidhrdw (none done)
	src\drivers\chindrag.c
Space Bugger [David Haywood]
	bad rom?
	src\drivers\sbugger.c
	src\vidhrdw\sbugger.c
Main Event (SNK) [David Haywood]
	vidhrdw not finished
	src\drivers\mainsnk.c


# 0.67


MAME Testers bugs fixed (there are probably more)
-----------------------

rocnrope37b7ora [Nicola Salmoria]
m62sound066yel
bladstle065gra   [Jake Stookey]
poundfor37b14gra (only the second part) [Jake Stookey]


These drivers have improved SOUND:
----------------------------------

- Fixed sample playback in the M72 games. [Nicola Salmoria]

- Music in Battle Rangers. [Charles MacDonald]


Other drivers changes:
----------------------

- Dozens of corrections to game description/flags [Toby Broyad]

- Deveral improvements to Championship VBall. [Steve Ellenoff]


Changes to the main program:
----------------------------

- New options -autoror/-autorol. They rotate the screen only if the game is
  vertical; useful with "pivot" LCD monitors. [Paul Priest]


Source:
-------

- MIPS 3/4 x86 dynamic recompiling core. [Aaron Giles]

- PIC16C5x CPU core. [Quench]

- Sega 315-5560 "MultiPCM" sound chip emulation. [R. Belmont]

- HuC6280 sound emulation. [Charles MacDonald]


New games supported:
--------------------

Mystic Warriors [R. Belmont, Phil Stroffolino]
Gaiapolis [R. Belmont, Phil Stroffolino]
Metamorphic Force [R. Belmont, Phil Stroffolino]
Kyukyoku Sentai Dadandarn [R. Belmont, Phil Stroffolino, Acho A. Tang]
Violent Storm [R. Belmont, Phil Stroffolino, Acho A. Tang]
Twin Bee Yahhoo! [R. Belmont, Phil Stroffolino, Acho A. Tang]
Martial Champion [R. Belmont, Phil Stroffolino, Acho A. Tang]
Lethal Enforcers 2 [R. Belmont, Phil Stroffolino, Olivier Galibert]
Taisen Puzzle-Dama [R. Belmont, Phil Stroffolino, Olivier Galibert]
Gokujou Parodius [R. Belmont, Phil Stroffolino, Olivier Galibert]
Sexy Parodius [R. Belmont, Phil Stroffolino, Olivier Galibert]
Daisu-Kiss [R. Belmont, Phil Stroffolino, Olivier Galibert]
Taisen Tokkae-Dama [R. Belmont, Phil Stroffolino, Olivier Galibert, David Haywood]
Guardians / Denjin Makai II [Luca Elia]
Dragongun [Bryan McPhail]
Tattoo Assassins [Bryan McPhail]
Locked 'n Loaded [Bryan McPhail]
Dead Angle [Bryan McPhail, David Haywood]
Dragon Master [David Haywood, Quench]
Las Vegas Girl (Girl '94) [David Haywood]
Hard Dunk [Jason Lo, R. Belmont, David Haywood]
Outrunners [Jason Lo, R. Belmont, David Haywood]
Tecmo Bowl [David Haywood, Tomasz Slanina]
Complex X [Brian Crowe]


==========================================================================

# 0.66

Codename "FR-34"


MAME Testers bugs fixed (there are probably more)
-----------------------

galpanib062gre   [Paul Priest]
usclssic065red   [Kale]
pang3065gre
housemn2065gra   [Nomax]
myangel065gre    [Nicola Salmoria]
88games062yel    [Nicola Salmoria]
glfgreat061gre   (was fixed some time ago)
robocop2064gre   [Bryan McPhail]
dassault061gre2  [Bryan McPhail]
jdredd060gra     [Smitdogg]
ringdest062gra   [Smitdogg]
spbactn061gra    [Smitdogg]
term2064gra      [Smitdogg]
xmen062ora       [Smitdogg]
sotsugyo061red	 [Bryan McPhail]
tmnt065gre		 [Acho A. Tang]
astdelux059gre   [Derrick Renaud]
gunforc2058gre_2 [Angelo Salese]
gunforc2058gre_3 [Bryan McPhail]


MAME Testers bugs to check
--------------------------

mrheli37b2gre   - I believe this was fixed some time ago


These drivers have improved SOUND:
----------------------------------

- Fixed samples in Pachinko Sexy Reaction [Suppi-Chan]

- Major improvements to the Namco NA sound emulation. [cync]

- Sound in IQ Block. [David Haywood]

- Fixed drums in Exterminator [Steve Ellenoff, Jim Hernandez]


Other drivers changes:
----------------------

- Misc improvements to the Namco NA driver. [Phil Stroffolino]

- Several improvements and games additions to the Nintendo VS and Playchoice
  drivers. [Pierpaolo Prazzoli]

- Several fixes to Super Real Darwin [Acho A. Tang]


Changes to the main program:
----------------------------

- New option -high_priority to increase the thread priority so MAME runs better
  while other programs are running. It is off by default because enabling it
  might make MAME use too much CPU time.

- Major update to the discrete sound system. This includes adjustable parameters
  controls in the UI, fixes to existing games, and new sound support in canyon,
  polaris, sprint and ultratnk. [Derrick Renaud & Keith Wilkins]

- Fixed some bugs in the I86/NEC CPU emulation. This includes raster effect
  problems in Geostorm and the 99 credits bug in Lethal Thunder. [Bryan McPhail]


Source:
-------

- Now compiled with GCC 3.2.2. Removed the patch that was needed to work around
  a GCC 3.2 bug.

- First version of YMF262 emulator. [Jarek Burczynski]


New games supported:
--------------------

Fantasy '95 [Brian A. Troha]
Hana no Mai [Nicola Salmoria]
Mahjong Friday [Nicola Salmoria]
Mahjong Dial Q2 [Nicola Salmoria]
Don Den Mahjong [Nicola Salmoria]
Watashiha Suzumechan [Nicola Salmoria]
Mahjong Studio 101 [Nicola Salmoria]
Mahjong Derringer [Nicola Salmoria]
Hana Yayoi [Nicola Salmoria]
Untouchable [Nicola Salmoria]
Rong Rong [Nicola Salmoria]
Don Den Lover Vol. 1 [Nicola Salmoria]
Quiz Channel Question [Nicola Salmoria]
Mad Donna [David Haywood]
Dorodon [Frank Palazzolo]
Ben Bero Beh [Acho A. Tang]
Halley's Comet [Acho A. Tang]
Triv Quiz [MooglyGuy]
Gigas [Tomasz Slanina]
Gigas Mark II [Tomasz Slanina]
Mad Shark [Luca Elia]
Ultra Toukon Densetsu [Luca Elia]


### New Clones supported:

Gratia (set 1)
Mysterious Stones (set 1)
Cosmic Cop (= Gallop)


==========================================================================

# 0.65


MAME Testers bugs fixed (there are probably more)
-----------------------

arbalest060gre    [Nicola Salmoria]
arbalest37b1gre   [Nicola Salmoria] (please verify)
calibr50062gre    [Nicola Salmoria]
downtown062gre    [Nicola Salmoria]
oisipuzl061gre    [Nicola Salmoria]
galpanic064gre    [Nicola Salmoria]
mitchellc063gre   [Smitdogg]
avengers061gre    [Acho A. Tang]
avengers37b16gre  [Acho A. Tang]
lwingsc37b7gre    [Acho A. Tang]
pow37b5yel        [Acho A. Tang] (only the first part)
powj36rc2gre      [Acho A. Tang]
sidearms060red    [Acho A. Tang]
sidearms055gre    [Acho A. Tang]
turtship37b5yel   [Acho A. Tang]
roadblst35b10yel  [Aaron Giles]
kinst063red       [Aaron Giles]


MAME Testers bugs to check
--------------------------

krzybowl058gre  - does this still apply? please check
seta2c060yel    - does this still apply? please check
myangel056gre   - this was probably fixed a couple of releases ago, please check
avengers061gre2 - can this be verified on a real US board? Might be a bug in the original.


These drivers have improved GRAPHICS:
-------------------------------------

- Fixes to CPS2 raster effects. [Shiriru]

- Preliminary starfield emulation in Sidearms. [Acho A. Tang]

- Cleaned up some Nichibutsu mahjong games, added LCD support to House Mannequin and
  Bijokko Yume Monogatari. [Nicola Salmoria]

- Major improvements to the Jaleco Mega System 32 games. [Nicola Salmoria]


These drivers have improved SOUND:
----------------------------------

- Fixed sound in Fire Truck, Super Bug, Monte Carlo. [Derrick Renaud]

- Fixed some issues with uPD7759 (TMNT, 88 Games, P.O.W.) [Acho A. Tang]


Changes to the main program:
----------------------------

- Optimized V60/V70 memory access. [Aaron Giles]

- Added stereo support to the X1-010 emulator and made it a proper sound core.
  [Nicola Salmoria]


New games supported:
--------------------

Orange Club - Maruhi Kagai Jugyou [Nicola Salmoria]
Mahjong-zukino Korinai Menmen [Nicola Salmoria]
Idol no Himitsu [Nicola Salmoria]
Kanatsuen no Onna [Nicola Salmoria]
Pairs [MooglyGuy]
Tao Taido [David Haywood, Stephane Humbert]
Perestroika Girls [David Haywood]
Ryuusei Janshi Kirara Star [Malice]
Hayaoshi Quiz Ouza Ketteisen [Malice]
Mouser [Frank Palazzolo]
Ribbit! [Aaron Giles]


==========================================================================

# 0.64


MAME Testers bugs fixed (there are probably more)
-----------------------

spaceint061red [Stefan Jokish]
robocop2056gre [Bryan McPhail]
stoneage060gre [Bryan McPhail]


These drivers have improved GRAPHICS:
-------------------------------------

- Fixed priorities and shadows in Skull & Crossbones. [Aaron Giles]


These drivers have improved SOUND:
----------------------------------

- Sound in Reikai Doushi [Nicola Salmoria]


Other drivers changes:
----------------------

- Simulated the 8751 protection in Fire Trap. [Bryan McPhail]


New games supported:
--------------------

Tickee Tickats [Aaron Giles]
Crowns Golf [Aaron Giles]
Ultimate Tennis [Aaron Giles, Nicola Salmoria]
Stone Ball [Aaron Giles, Nicola Salmoria]
Cheese Chase [Aaron Giles, Nicola Salmoria]
Mahjong Kojinkyouju (Private Teacher) [Nicola Salmoria]
Mahjong Vitamin C [Nicola Salmoria]
Mahjong-yougo no Kisotairyoku [Nicola Salmoria]
Mahjong Kinjirareta Asobi [Nicola Salmoria]
Mahjong Lemon Angel [Nicola Salmoria]
Mahjong Jogakuen [Nicola Salmoria]
Mahjong Ikagadesuka [Nicola Salmoria]
Raiga - Strato Fighter
Edward Randy [Bryan McPhail]
Mutant Fighter / Death Brade [Bryan McPhail]
Wizard Fire / Dark Seal 2 [Bryan McPhail]
Zombie Raid [David Haywood]
Enforce
Shot Rider [Tomasz Slanina]


### New Clones supported:

Shadow Warriors (set 2)
Carrier Air Wing (US)
Chi-Toitsu (= Mahjong Gakuen)
Mahjong Nanpa Story (Ura)


==========================================================================


# 0.63

An interim release with many things submitted in the past two months not
included yet.


These drivers have improved GRAPHICS:
-------------------------------------

- Subroc-3D, Turbo and Buck Rogers use the artwork to show the LED counters.
  [Stefan Jokish]

- Fixed colors in Dynamic Ski. [Stefan Jokish]


These drivers have improved SOUND:
----------------------------------

- Samples support in Subroc-3D [Dave France]

- Fixed noie frequency in the Exidy games. [Jim Hernandez]

- Preliminary Namco NA sound support. [Phil Stroffolino]


Other drivers changes:
----------------------

- Partially simulated the protection in Birdie Try [Angelo Salese]

- Improved decryption in Quiz F1 1-2 Finish, it's still incomplete though.
  [Angelo Salese]

- Fixed ball position recognition in Golfing Greats. [Olivier Galibert]


Changes to the main program:
----------------------------

- New options -nodisclaimer and -nogameinfo to disable startup screens.
  Warnings about incorrect emulation are not disabled. [Aaron Giles]

- Replaced RDTSC timing with QueryPerformanceCounter. This should fix problems
  on SpeedStep notebooks. It seems, however, that it causes even worse problems
  on other machines. Therefore, the old behaviour can be forced with the new
  -rdtsc option. [smf]


Source:
-------

- Now compiled with MinGW 2.0/GCC 3.2. There might be bugs caused by the new
  compiler, let us know.

- All new, much better, YM2413 emulation. [Jarek Burczynski]

- Drum support in YM2608 emulation (check e.g. Tail 2 Nose). [Jarek Burczynski]

- Moved most of the file I/O out of the OS depndant code and into the core.
  Added two new types, mame_file and osd_file, which enforce better type
  checking throughout the system (and prevents confusing the two).
  Instead of calling osd_fopen, osd_fread, osd_fwrite, etc., you now call
  mame_fopen, mame_fread, mame_fwrite, etc. The osd_* calls are reserved
  to the core.
  All the ZIP handling is done in the mame_file functions, so it will be
  inherited by all ports.
  [Aaron Giles]

- [Win32] The diff, snap, cfg, nvram, etc. directories are created
  automatically if they don't exist the first time MAME tries to create a file
  there. [Aaron Giles]

- MIPS III/IV CPU cores. [Aaron Giles]

- ADSP-2115 CPU support. [Aaron Giles]

- TMS32025 CPU core. [Quench]

- Fixed YMF278B emulation. [R.Belmont]


MAME Testers bugs fixed (there are probably more)
-----------------------

galaga37b8gre [Satoshi Suzuki]
galaga37b16gre [Satoshi Suzuki]
cutieq37b2gre [Stephane Humbert]
crusn062gre [Aaron Giles]
totcarn062gre [Aaron Giles]
commsega062gra [Stephane Humbert]
wiping062red [Stefan Jokish]
heartatk062red [Stefan Jokish]
polyplay060red [Stefan Jokish]


New games supported:
--------------------

Off Road Challenge [Aaron Giles]
Shrike Avenger [Aaron Giles]
War Gods [Aaron Giles]
Grudge Match [Aaron Giles]
Sharpshooter [Aaron Giles]
Killer Instinct [Aaron Giles, Bryan McPhail]
Killer Instinct 2 [Aaron Giles, Bryan McPhail]
Dr. Micro [Uki]
Run and Gun [R.Belmont]
Holosseum [David Haywood, Olivier Galibert, R. Belmont, Farfetch'd]
Super Visual Football [David Haywood, Olivier Galibert, R. Belmont, Farfetch'd]
Burning Rivals [David Haywood, Olivier Galibert, R. Belmont, Farfetch'd]
Rad Mobile [David Haywood, Olivier Galibert, R. Belmont, Farfetch'd]
Rad Rally [David Haywood, Olivier Galibert, R. Belmont, Farfetch'd]
F1 Exhaust Note [David Haywood, Olivier Galibert, R. Belmont, Farfetch'd]
Alien 3 [David Haywood, Olivier Galibert, R. Belmont, Farfetch'd]
Sonic [David Haywood, Olivier Galibert, R. Belmont, Farfetch'd]
Golden Axe 2 [David Haywood, Olivier Galibert, R. Belmont, Farfetch'd]
Spiderman [David Haywood, Olivier Galibert, R. Belmont, Farfetch'd]
Arabian Fight [David Haywood, Olivier Galibert, R. Belmont, Farfetch'd]
Gumbo [David Haywood]
Golly Ghost [Phil Stroffolino]
Moon Shuttle [Zsolt Vasvari]
SF-X [Zsolt Vasvari]
Mighty Monkey [Zsolt Vasvari]
Triv Two [David Haywood]
Pirates [David Haywood, Nicola Salmoria, Paul Priest]
Free Kick [Tomasz Slanina]
Perfect Billiard [Nicola Salmoria]
Wakakusamonogatari Mahjong Yonshimai [Nicola Salmoria]
Got-cha [Nicola Salmoria]
Formula 1 Grand Prix Part II [Nicola Salmoria]
Real Mahjong Haihai [Nicola Salmoria]
Real Mahjong Haihai Jinji Idou Hen [Nicola Salmoria]
Real Mahjong Haihai Seichouhen [Nicola Salmoria]
Mahjong Kyou Jidai [Nicola Salmoria]
The Mah-jong [Uki]
Mahjong Hourouki Part 1 - Seisyun Hen [Nicola Salmoria]
Mahjong Hourouki Gaiden [Nicola Salmoria]
Mahjong Hourouki Okite [Nicola Salmoria]
Mahjong Clinic [Nicola Salmoria]
Mahjong Rokumeikan [Nicola Salmoria]
Tugboat [MooglyGuy, Nicola Salmoria]
Super Triv 2 [MooglyGuy]
Penguin Bros
Target Hits [Manuel Abadia]
TH Strikes Back [Manuel Abadia]
Alligator Hunt [Manuel Abadia]
World Rally 2: Twin Racing [Manuel Abadia]
Maniac Squares [Manuel Abadia]
Snow Board Championship [Manuel Abadia]
Bang! [Manuel Abadia]
Boomer Rang'r / Genesis [Bryan McPhail]
Kamikaze Cabbie [Bryan McPhail]
Liberation [Bryan McPhail]
P-47 Aces


### New Clones supported:

Nostradamus (Korea)
Cruis'n World (rev L2.0)
Terminator 2 (LA2)
Super High Impact (prototype)
Mr Kougar (set 2)
Donkey Kong Jr (bootleg on Galaxians hardware)
Police Trainer (older)
Hokuha Syourin Hiryu no Ken (=Shanghai Kid)


==========================================================================


# 0.62

LICENSE CHANGE WARNING:
We are considering changing distribution license, switching to GPL for the
whole project and LGPL for some CPU and sound cores.
If you contributed code to MAME and are against this change, now is the
right time to let us know.


These drivers have improved GRAPHICS:
-------------------------------------

- Improved raster effects in CPS2 games. [Barry Rodewald]

- Fixed priorities in several Atari games. [Aaron Giles]

- Sprite alpha blending in the Taito F3 games. [Shiriru]


These drivers have improved SOUND:
----------------------------------

- Improved sound in Asteroids. [Ken Reneris]

- Sound in Bal Cube and Bang Bang Ball, and in the Psikyo games.
  [Olivier Galibert]

- Rewritten sound support for the Seta games. [Manbow-J]

- Sound in Blazing Tornado. [R.Belmont]

- Fixed sound in Battle Bakraid. [R.Belmont]

- Fixed cracking and missing sounds in some neogeo games especially kof99 "how
  to play" loop and shocktro end of attract missing sound. [ElSemi]

- Sound in Hard Drivin'. [Aaron Giles]


Other drivers changes:
----------------------

- CusKey simulation for Namco games. [John Wil]


Changes to the main program:
----------------------------

- Fixed keyboard LEDs being messed up on program exit. [Paul Priest]

- TESTDRIVER's are now part of the database used by -romident and -isknown.
  [Nicola Salmoria]


Source:
-------

- Screen rotation is now entirely handled at blit time by the OS layer. The
  core no longer prerotates the bitmap. [Aaron Giles]

- Preliminary YMF278B emulation (missing FM support). [R.Belmont]

- Implementation of the SH7604 internal timers, giving sound to Sol Divide and
  the Super Kaneko Nova System games. [R.Belmont]

- V70 CPU support. [Olivier Galibert]

- DSP32 CPU emulation. [Aaron Giles]

- TMS32013 CPU emulation. [Aaron Giles]


MAME Testers bugs fixed (there are probably more)
-----------------------

bjourney061gre
doubledr36rc1gre [ElSemi]
crosshairs061yel [Robin Merrill]
tnzs36b16yel MIGHT be fixed - needs thorough testing [Kale]
deco8061red [Paul Priest]
blstroidc061gra
bking236b14yel [Stefan Jokish]
bking237b14gre [Stefan Jokish]
fround37b16gre [Bryan McPhail]
vulcan37b13gre [Bryan McPhail]
vulcan237b1gre [Bryan McPhail]
vulcan137b1gre [Bryan McPhail]


New games supported:
--------------------

Fantasia II [Nicola Salmoria]
Golden Fire II [Nicola Salmoria]
Bouncing Balls [ElSemi]
Dragon World II [David Haywood, ElSemi]
Tengai / Sengoku Blade [Paul Priest]
Magical Cat Adventure [Paul Priest, David Haywood]
Nostradamus [Paul Priest, David Haywood]
Strikers 1945 [Olivier Galibert, R.Belmont]
IPM Invader
Green Beret
Escape Kids [Ohsaki Masayuki]
Solvalou [Phil Stroffolino]
Star Blade [Phil Stroffolino]
Prop Cycle [Phil Stroffolino]
Lucky & Wild [Phil Stroffolino]
Super World Stadium '95 [Phil Stroffolino]
Metal Hawk [Phil Stroffolino]
Steel Gunner 2 [Phil Stroffolino]
Amazon [Phil Stroffolino]
Kid no Hore Hore Daisakusen [Phil Stroffolino, Stephane Humbert]
Legion [David Haywood, Phil Stroffolino]
Dragonball Z 2 Super Battle [David Haywood]
Daioh [David Haywood]
Cyvern [Sylvain Glaize, David Haywood]
Sen-Know [Sylvain Glaize, David Haywood]
Gals Panic 4 [Sylvain Glaize, David Haywood]
Gals Panic S - Extra Edition [Sylvain Glaize, David Haywood]
Gals Panic S2 [Sylvain Glaize, David Haywood]
Panic Street [Sylvain Glaize, David Haywood]
PuzzLoop [Sylvain Glaize, David Haywood]
Jan Jan Paradise [Sylvain Glaize, David Haywood]
Jan Jan Paradise 2 [Sylvain Glaize, David Haywood]
Otome Ryouran [Sylvain Glaize, David Haywood]
Tel Jan [Sylvain Glaize, David Haywood]
Sengeki Striker [Sylvain Glaize, David Haywood]
Speed Ball [Aaron Giles]
Race Drivin' [Aaron Giles]
Steel Talons [Aaron Giles]
Asylum [Aaron Giles]
Cruis'n USA [Aaron Giles]
Cruis'n World [Aaron Giles]
Area 51 [Aaron Giles]
Maximum Force [Aaron Giles]
Vicious Circle [Aaron Giles]
Sky Raider [Stefan Jokish]
Starship 1 [Frank Palazzolo, Stefan Jokish]
Desert War [David Haywood, Paul Priest, Stephane Humbert]
Gratia - Second Earth [David Haywood, Paul Priest, Stephane Humbert]
The Game Paradise - Master of Shooting! [David Haywood, Paul Priest, Stephane Humbert]
One Shot One Kill [David Haywood, Paul Priest, Stephane Humbert]
Tetris Plus [David Haywood, Paul Priest, Stephane Humbert]
Best Bout Boxing [David Haywood, Paul Priest, Stephane Humbert]
Super Slam [David Haywood]
Aquarium [David Haywood]
S.S. Mission [David Haywood]
Fit of Fighting [David Haywood]
The History of Martial Arts [David Haywood]
Indoor Soccer [David Haywood]
Diver Boy [David Haywood]
Speed Spin [David Haywood]
Masked Riders Club Battle Race [David Haywood, Stephane Humbert]
Hanaroku [David Haywood, Stephane Humbert]
Fancy World - Earth of Crisis [David Haywood, Stephane Humbert]
Flower [InsideOutBoy, David Haywood, Stephane Humbert]
Ace [Jarek Burczynski]
N.Y. Captor [Tomasz Slanina]
Beam Invader [Zsolt Vasvari]
Zarya Vostoka [Zsolt Vasvari]
Megadon [Mike Haaland]
Catapult [Mike Haaland]
4 En Raya [Tomasz Slanina]
Sky Skipper
Gomoku Narabe Renju [Takahiro Nogi]


### New Clones supported:

Sanrin San Chan (= Spatter)
New Rally X (vertical bootleg)
Lode Runner - The Dig Fight (ver. A)
Samurai Aces (= Sengoku Ace)
Big Striker (bootleg)
X-Men (US 2 Players)
MatchIt (= Sichuan 2)
Super Basketball (version G)
Thunder Cross II (Asia)
Alpha Fighter / Head On
Donkey Kong (US set 2)
Triple Fun (= Oishii Puzzle)
Blandia
Centipede (1 player, timed)
Raiden [Taiwan]
Goindol (World and Japan)
Super Volleyball (US)
Strikers 1945 (Japan, unprotected)


==========================================================================


# 0.61


These drivers have improved GRAPHICS:
-------------------------------------

- Fixed tilemaps in Sotsugyo Shousho. [Nicola Salmoria]

- Fixed graphics in Pinbo. [Zsolt Vasvari]

- Improved raster effects handling in NeoGeo games. [Razoola]

- Improved gfx in HAL21. [Acho A. Tang]

- Preliminary support for raster effects in CPS2 games. [Barry Rodewald]

- Added alpha blending to Psikyo games. [Paul Priest]


These drivers have improved SOUND:
----------------------------------

- Fixed Y8950 confincting with YM3812/YM3526 in some SNK games.
  [Jarek Burczynski]

- Fixed sound in Spinal Breakers. [Nicola Salmoria]

- Sound in HAL21. [Acho A. Tang]


Other drivers changes:
----------------------

- Fixed controls & gfx in Outrun and Space Harrier. [Bryan McPhail]

- Simulated protection in S.P.Y. [Acho A. Tang]

- Fixed Rip Cord. [Acho A. Tang]

- Fixed collision detection in Labyrinth Runner and Fast Lane. [Acho A. Tang]

- Fixed protection in Funky Jet [Bryan mcPhail, Stephane Humbert]

- Several fixes to the F3 games. [Shiriru, Bryan McPhail]


Changes to the main program:
----------------------------

- Light gun support. [Bryan McPhail]


Source:
-------

- Important for porters: major redesign of the OS interface for display update.
  - VIDEO_SUPPORTS_DIRTY flag removed
  - osd_mark_dirty() removed
  - osd_create_display() now takes an extra parameter rgb_components
  - osd_set_visible_area() removed
  - osd_allocate_colors() removed
  - osd_modify_pen() removed
  - osd_update_video_and_audio() now takes a mame_display pointer
  - osd_debugger_focus() removed
  - osd_set/get_gamma() removed
  - osd_set/get_brightness() removed
  [Aaron Giles]

- Completely redesigned the artwork handling. Any game can now use artwork,
  with no specific support in the driver. Configuration is done with external
  .art files. [Aaron Giles]
  -[no]artcrop -- crops artwork to the game screen area only
  -[no]backdrop -- enables/disables backdrop art
  -[no]overlay -- enables/disables overlay art
  -[no]bezel -- enables/disables bezel art

- The Windows version now rotates the screen at blit time instead of requiring
  the core to do so. Since the new code is usually faster than the old one, and
  it allows for great simplifications in the core, rotation at blit time will
  probably be a requirement in next release. Porters are advised to begin
  implementing it now. [Aaron Giles]

- New YM-2413 emulator. [Fabio R. Schmidlin, Charles Mac Donald]


MAME Testers bugs fixed (there are probably more)
-----------------------

mk3060gre
ozmawars055gra
alpine057gra     [Stephane Humbert]
elevator057gra   [Stephane Humbert]
tinstar36b4gra   [Stephane Humbert]
kof94057gre      [RYO]
tokib059red      [David Graves]
dogyuun058yel    [Quench]
battleg37B10gre  [Quench]
batrider058gre   [Quench]
cchasm1059red    [Paul Priest]
dspirit37b6gre   [Acho A. Tang]
rompers36rc2gre  [Acho A. Tang]
rompers37b4gre   [Acho A. Tang]
splatter336b6gre [Acho A. Tang]
splatter336b9yel [Acho A. Tang]
splatter137b5gre [Acho A. Tang]
splatter237b5gre [Acho A. Tang]
splatter057gre   [Acho A. Tang]
dangseed060yel (needs checking)
bjourney37b16gre [Acho A. Tang]
ncombat36b3yel   [Acho A. Tang]
ncommand37b2gre  [Acho A. Tang]
overtop37b13red
sdodgeb37b13yel (needs checking)
iganinju37b14yel [Kale]
iganinju37b1yel  [Kale]
iganinju055yel   [Kale]
scudhamm058gre   [Mike Herrin]
adstick37b16gre  [Robin Merrill]
pedal055yel      [Robin Merrill]
toobin058yel     [Robin Merrill]


New games supported:
--------------------

Super Slams [David Haywood]
Macross Plus [David Haywood]
Quiz Bisyoujo Senshi Sailor Moon - Chiryoku Tairyoku Toki no Un [David Haywood]
Jump Kids [David Haywood]
Toffy [David Haywood]
Super Toffy [David Haywood]
Battle Cross [David Haywood]
Gulf War II [David Haywood]
Bubble 2000 [David Haywood]
4 Fun in 1 [David Haywood]
Kick Goal [David Haywood, Nicola Salmoria]
U.S. Games collections [David Haywood, Nicola Salmoria]
Wily Tower [Nicola Salmoria]
Lethal Crash Race [Nicola Salmoria]
F-1 Grand Prix [Nicola Salmoria]
Dog-Fight [Nicola Salmoria]
Find Out [Nicola Salmoria]
Golfing Greats [Nicola Salmoria]
Sankokushi [Nicola Salmoria]
Super Shanghai Dragon's Eye [Bryan McPhail]
Mechanized Attack [Bryan McPhail]
Beast Busters [Bryan McPhail]
Thunder Zone / Desert Assault [Bryan McPhail]
The Next Space [Bryan McPhail, Acho A. Tang]
Bakutotsu Kijuutei [Acho A. Tang]
Equites [Acho A. Tang]
Bull Fighter [Acho A. Tang]
The Koukouyakyuh [Acho A. Tang]
Splendor Blast [Acho A. Tang]
High Voltage [Acho A. Tang]
Red Robin [Zsolt Vasvari]
Net Wars [Zsolt Vasvari]
Clay Shoot [Zsolt Vasvari]
Mr. Kougar [Zsolt Vasvari]
Car Polo [Zsolt Vasvari]
Dingo [M*A*S*H, Zsolt Vasvari]
Birdie King [M*A*S*H]
Inferno [Juergen Buchmueller, Aaron Giles]
Dribbling [Aaron Giles]
Triple Hunt [Stefan Jokish]
Orbit [Stefan Jokish]
Monte Carlo [Stefan Jokish]
Tube Panic [Jarek Burczynski]
Roller Jammer [Jarek Burczynski]
Kikiippatsu Mayumi-chan [Uki]
Knights of Valour / Sango [ElSemi]
Battle Bakraider [Quench]


### New Clones supported:

ESP Ra.De (International Ver 1998 4/22)
ESP Ra. De. (Japan Ver 1998 4/21)
The Outfoxies (Japan)
Tactician (set 2)
Battles (=Xevious)
Power Spikes (World)
Condor (= Phoenix)
Grind Stormer (set 2)
Battle Garegga (set 2)
Hell Fire (1 player)
Same! Same! Same! (2 players)
Gravitar (prototype)
Lunar Battle (= Gravitar)
Super Cobra (Sega)
S.P.Y. (World)
Air Buster (English)
Baraduke (set 2)
Metro-Cross (set 2)
Red Hawk (= Stagger I)
Bagman (bootleg on Galaxian hardware)


==========================================================================


# 0.60


These drivers have improved GRAPHICS:
-------------------------------------

- Fixed disappearing enemies in later levels of Batsugun. [Kale]

- Fixed colors in Wec Le Mans and wrong graphics in Hot Chase. [Acho A. Tang]


These drivers have improved SOUND:
----------------------------------

- Improved Irem GA20 emulation, used by M92 games. [Acho A. Tang]


Other drivers changes:
----------------------

- Fixed several problems in Kiki KaiKai. [Acho A. Tang]

- Several fixes to Psikyo games. [Paul Priest, David Haywood]

- Several fixes to nemesis.c. [Hau]


Changes to the main program:
----------------------------

- Support for external configuration files for special controllers; this
  replaces the -hotrod and -hotrodse options. See ctrl.txt for the detailed
  explanation. [Ron Fries]

- Complete rewrite of the cheat engine. [Ian Patterson]


Source:
-------

- New function memory_set_unmap_value(), to specify the value returned when an
  unmapped memory address is read. [Aaron Giles]

- Fixes to Y8950 sample playback. [Acho A. Tang]

- Complete rewrite of YM3812/YM3526/Y8950 emulation, verified on the real chip.
  [Jarek Burczynski]

- osd_opl_control() and osd_opl_write() removed; direct access to the
  SoundBlaster OPL chip is no longer useful.


MAME Testers bugs fixed (there are probably more)
-----------------------

blmbycar059red
uopoko059red
gcpinbal059red
jitsupro059red
metroc059red
phelios059red
paclandc059red
ncv1058red
av2mj059red
fromancec058red
neogeoc37b15gre
fround056red
bioatack37b6gre
crush057gre
driverc056ora
batsugun37b6gre


New games supported:
--------------------

Judge Dredd [Aaron Giles]
Primal Rage [Aaron Giles]
Road Riot's Revenge [Aaron Giles]
World Class Bowling [Aaron Giles]
Tactician [Nicola Salmoria, Stephane Humbert]
The Masters of Kin [Nicola Salmoria]
IQ Block [Nicola Salmoria, Ernesto Corvi]
Taxi Driver [Nicola Salmoria]
Block Gal [Angelo Salese]
Drag Race [Stefan Jokisch]
Pool Shark [Stefan Jokisch]
The Outfoxies [Phil Stroffolino]
Oriental Legend [ElSemi, David Haywood]
News [David Haywood]
Shadow Force [David Haywood]
The Legend of Silk Road [David Haywood, Stephane Humbert, R.Belmont]
Miss Bubble 2 [David Haywood]
Black Heart [from Raine]
Mustang [from Raine]
Many Bloc [David Haywood, Stephan Humbert]
Xyonix [David Haywood, Stephan Humbert, Nicola Salmoria]
Hyper Pacman [David Haywood, Stephan Humbert]
Mille Miglia 2: Great 1000 Miles Rally [David Haywood, Stephan Humbert]
Formation Z [Acho A. Tang]
Tank Busters [Jarek Burczynski]
Hyper Duel [E. Watanabe]


### New Clones supported:

Pengo (bootleg)
Bone Crusher (= Knuckle Joe)
Fighter & Attacker (= F/A)
Knuckle Heads (World)
Hero in the Castle of Doom (Donkey Kong conversion)
Blood Storm (v1.10)
Fighting Golf (set 2)
Clowns (rev 1)
Magic Worm (= Centipede)
Hot Shots Tennis (V1.0)
Strata Bowling (V1)
Pirate Pete (= Jungle King)
Street Fighter (prototype)
Nebulas Ray (World)
Gun Bird (World)
Bubble Symphony (Europe)
Space Invaders '95 (US)
C.O.W. Boys of Moo Mesa (World)
R-Type Leo (World)
Dragon Punch (= Sports Match)


==========================================================================


# 0.59


Other drivers changes:
----------------------

- Loads of fixes to input ports etc. in several drivers. [Stephane Humbert]

- Fixed Rainbow Islands and Rainbow Islands Extra c-chip emulation
  [Robert Gallagher, Tormod Tjaberg]

- Fixed Super Stingray and Kyros, and most problems in Gold Medalist.
  [Acho A. Tang]


Changes to the main program:
----------------------------

- [Windows] Some changes to use less CPU time when it's not needed.
  Use -nosleep to disable this behaviour during gameplay. [Aaron Giles]

- [Windows] MMX implementation of the tilemap drawing code, providing a 5-10%
  speed increase with some games. [Andrea Mazzoleni]


Source:
-------

- Rewritten the UPD7759 emulator (used by TMNT etc). [Olivier Galibert]

- Support for compressed hard disk images. [Aaron Giles]

- New function tilemap_set_palette_offset() to set a global palette offset
  without having to use tilemap_mark_all_tiles_dirty(). [Aaron Giles]


MAME Testers bugs fixed (there are probably more)
-----------------------

eto37b9gre
captaven058gre_2
captaven058gre_5
thndrx2058red
punchoutc37b15gre
skyadvnt136b8gre
gangwars37b15gre


New games supported:
--------------------

Police Trainer [Aaron Giles]
Pass [David Haywood, Stephan Humbert]
Super Pinball Action [David Haywood, Stephan Humbert]
Sol Divide [David Haywood]
Strikers 1945 II [David Haywood]
Space Bomber [David Haywood]
Daraku Tenshi - The Fallen Angels [David Haywood]
Gunbird 2 [David Haywood]
Mahjong Sisters [Uki]
Quiz Gakuen Paradise [Uki]
Quiz DNA no Hanran [Uki]
Quiz Gekiretsu Scramble [Uki]
Wild West C.O.W.boys of Moo Mesa [R.Belmont]
Bucky O'Hare [R.Belmont]
Super Bug [Stefan Jokisch]
Paddle Mania [Acho A. Tang]
Scorpion [Stephane Humbert]


### New Clones supported:

Dynasty Wars (World)
Captain America (Japan)
Gradius 3 (World?)
Magical Crystals (World)
Crystal Castles (version 4)
Moon Cresta (Nichibutsu set 2)
Galaxian (Midway, old rev)
Pisces (original)
Haunted Castle (Japan version N)
Beraboh Man (Japan version B)


==========================================================================


# 0.58


Other drivers changes:
----------------------

- Fixed (?) collision detection in Thunder Cross. [Eddie Edwards]

- Fixed the VLM5030 emulation (Punch Out, Track & Field, Yie-Ar Kung Fu etc.).
  [Tatsuyuki Satoj]


Changes to the main program:
----------------------------

- [windows] New blitting effect "-effect sharp". This gives a sharper image
  than the default. [John IV]


Source:
-------

- New macros have been added to declare common callback functions and enforce
  naming conventions. All drivers have been converted to the new macros:

      void init_drivername()            -> DRIVER_INIT( name )
      int generate_int()                -> INTERRUPT_GEN( func )
      void name_init_machine()          -> MACHINE_INIT( name )
      void nvram_handler()              -> NVRAM_HANDLER( name )
      void name_vh_convert_color_prom() -> PALETTE_INIT( name )
      int name_vh_start()               -> VIDEO_START( name )
      void name_vh_stop()               -> VIDEO_STOP( name )
      void name_vh_eof()                -> VIDEO_EOF( name )
      void name_vh_screenrefresh()      -> VIDEO_UPDATE( name )

- Major change to the way machine drivers are set up. A series of macros is
  now available (defined in driver.h) to "build up" a machine driver. The
  ordering of items in the machine driver is arbitrary, apart from CPU-specific
  data, which is tied to the most-recently referenced CPU. With this new format,
  machine drivers can now be built by modifying another machine driver,
  providing a primitive inheritance mechanism. See any driver for an example
  of the new format. See mcr.c for a more complex example of inheritance.

- New functions auto_bitmap_alloc() and auto_bitmap_alloc_depth() are now
  available to create automatically managed bitmaps. Combined with auto_malloc(),
  there is no need for a video_stop function in most cases. All existing
  drivers have been modified to take advantage of these features.

- The old interrupt system has now been made officially obsolete. This means
  that the function cpu_cause_interrupt() is no longer available; you must use
  cpu_set_irq_line() or cpu_set_irq_line_and_vector() instead. It also means
  that a number of old/conflicting constants have been removed. In addition,
  interrupt generation callbacks for CPUs no longer return an IRQ line or
  vector. Instead, the interrupt callback must actively cause the interrupt
  via cpu_set_irq_line(). All existing drivers have been updated to these
  changes.

- New interrupt callback functions: nmi_line_pulse(), nmi_line_assert(),
  irqn_line_hold(), irqn_line_pulse(), irqn_line_assert(). These functions
  replace the old interrupt() and nmi_interrupt() callbacks and can be used
  directly as interrupt callbacks.

- The function tilemap_set_clip() has been removed. In its place, there is a
  new 'cliprect' parameter to tilemap_draw(). All existing drivers have been
  updated to support this change.

- In preparation for partial updating support, the parameters to video_update
  have changed. A new parameter 'cliprect' has been added, which should be
  honored. In the future, the parameter will contain the subset of the screen
  to render; for now, it just contains Machine->visible_area. In addition,
  the 'full_refresh' parameter is gone; update functions must always redraw
  the requested portion of the bitmap. Some drivers have been updated to fully
  support the cliprect; others still need to be updated.

- Two new functions have been added to the timer system. timer_create() creates
  a new "permanent" timer, and timer_adjust() adjusts the scheduling of that
  timer. This is now the only way to dynamically manipulate a timer. The old
  functions timer_set() and timer_pulse() are still around, but they no longer
  return a handle to the timer they create. Timers created with timer_create()
  should be created at initialization time, not dynamically. Timers also now
  participate in the automatic resource tracking, so there is no need to
  explicitly dispose of them. All existing drivers have been updated with the
  necessary changes.

- Basic generic NVRAM handlers have been added. For a number of games, it is
  sufficient to point the global variables 'generic_nvram' and
  'generic_nvram_size' to the location and size of the NVRAM area. In the
  machine driver, you can now specify 'generic_0fill' or 'generic_1fill' to
  request a generic NVRAM handler. The 0fill and 1fill refer to how the memory
  is initialized in the absence of an .nv file.

- The following old functions and macros have now been fully deprecated:

     cpu_get_pc()           -> activecpu_get_pc()
     cpu_get_sp()           -> activecpu_get_sp()
     cpu_getpreviouspc()    -> activecpu_get_previouspc()
     cpu_get_reg()          -> activecpu_get_reg()
     cpu_set_reg()          -> activecpu_set_reg()
     cpu_set_op_base()      -> activecpu_set_op_base()
     cpu_get_pc_byte()      -> activecpu_get_pc_byte()
     READ_WORD              -> (no equivalent)
     WRITE_WORD             -> (no equivalent)

- New function force_partial_update(), call this to force a partial update to
  occur up to and including the specified scanline. [Aaron Giles]

- ARM CPU emulation [Bryan McPhail, Phil Stroffolino]


MAME Testers bugs fixed (there are probably more)
-----------------------

rampart057yel
artwork057gre
mrgoemon36b16gre
pigout37b4gre


New games supported:
--------------------

Gun Force 2 [Chris Hardy]
Angel Kids [David Haywood]
Noboranka [Stephane Humbert]
Steel Force [David Haywood, Stephan Humbert]
Mug Smashers [David Haywood, Stephan Humbert]
Captain America [Bryan McPhail]
Lemmings [Bryan McPhail]
Flyball [Stefan Jokisch]
Grand Cross [David Graves]


### New Clones supported:

Mayday (set 3)
Alpha One (prototype of Major Havoc)
Blaster (kit)
Buck Rogers (encrypted)
