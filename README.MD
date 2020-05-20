# GEMDOS for the Apple Lisa

This repository contains nearly all you need to build Digital Research's GEMDOS for the Apple Lisa.  The only other thing you need is an Atari ST emulator.  I recommend Hatari, simply because that is the one I have tested it on.

This is a bit of a "lost OS" for the Lisa, which mostly seems to exist as an example in the "GEM Porting Kit" for how to port GEMDOS to a 68k-based system.  Unfortunately the porting kit is slightly incomplete and seems to be aimed at building on a Lisa.

If that doesn't sound interesting to you, this probably isn't the code for you.

The rest of GEM also exists for the Lisa.  I aim to get to that in due course.


## Building instructions

1. Check out the repository
2. Download the Hatari emulator
3. Set its "GEMDOS Drive" (Hatari's name for its host directory filing system) to the checked out repository.
4. Load Hatari.  Double-click on the C drive and then COMMAND.PRG
5. Run the following commands:
   `CD SYSBLD`
   `SETUP` - this copies the relevant files into the SYSBLD directory
   `MAKEFI` - this builds GEMDOSFI.SYS, which is a floppy disk bootable GEMDOS.
   
You can also use the `MAKEHI` batch file to make a hard disc bootable image.  But since hard disc support seems to be broken right now, this is probably less helpful.


## Making a boot disc

See: http://www.alternative-system.com/en/revive-gemdos-for-lisa/ (not my link!)


## Code provenance and licensing

The code in this repository is nearly all from the Caldera dump of the GEM source code under the GPL.  The original forms of these files are all available at http://www.deltasoft.com/downloads-gemworld.htm as ZIP files.  The only exception is the reconstructed CON.C which is unique to this repository but is a composite of a GPLed GEMDOS CONSOLE.C and the corresponding file from CP/M 1.2 (don't ask).  I believe, given the very broad wording of the license for the CP/M code, that this is within the license terms for that code.