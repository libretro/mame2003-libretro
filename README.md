# mame2003-libretro
[![pipeline status](https://git.libretro.com/libretro/mame2003-libretro/badges/master/pipeline.svg)](https://git.libretro.com/libretro/mame2003-libretro/-/commits/master)

MAME as it was in 2003, using the libretro API. Suitable for lower-end devices that would struggle to run current versions of MAME (later versions of MAME are increasingly accurate, thus can perform worse).

## ROMset compatibility with MAME 2003
View our [live compatibility table](https://buildbot.libretro.com/compatibility_lists/cores/mame2003/mame2003.html). See an incorrect entry? Create a new issue to let us know!

**The MAME 2003 core accepts MAME 0.78 ROMsets.** Each version of an arcade emulator must be used with ROMs that have the same exact version number. For example, MAME 0.37b5 ROMsets are required by the MAME 2000 emulator, but MAME 0.37b5 sets will not work correctly with the MAME 2003 or MAME 2010 emulator cores. Those cores require MAME 0.78 and MAME 0.139 ROM sets, respectively.

File-not-found errors are the result of a ROMset that is wrong or incomplete, including if you're trying to run a "Split" clone .zip without the parent .zip present. "Non-Merged ROM" romset collections do not require parent .zips to be present. "Full Non-Merged" romset collections do not require the parent .zip or the BIOS .zip; Full Non-Merged romsets are fully standalone.

## Additional configuration information:
 * Official documentation: [https://docs.libretro.com/library/mame_2003/](https://docs.libretro.com/library/mame_2003/)
 * RetroPie's documentation: [https://retropie.org.uk/docs/lr-mame2003/](https://retropie.org.uk/docs/lr-mame2003/)

### Development reference links:
 * [MAME: Benchmarks, Useful Code, Bug Fixes, Known Issues](http://www.anthrofox.org/code/mame/index.html) at anthrofox.org
 * [diff file which records efforts taken to address the unaligned memory issue](https://code.oregonstate.edu/svn/dsp_bd/uclinux-dist/trunk/user/games/xmame/xmame-0.106/src/unix/contrib/patches/word-align-patch)
 * [Directory of xmame diffs](http://web.archive.org/web/20090718202532/http://www.filewatcher.com/b/ftp/ftp.zenez.com/pub/mame/xmame.0.0.html) - Offline as of March 2017
