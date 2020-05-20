rem "Making GEMDOSFI.SYS for the 68000"
del disk.h
copy floppy.h disk.h
c @cfls
asm @sfls
lc lisa
link68 command.rel = coma,command,lmul
relmod command.rel command.prg
