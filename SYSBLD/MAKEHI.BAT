rem "Making GEMDOSHI.SYS for the 68000"
del disk.h
copy hd.h disk.h
c @cfls
asm @sfls
lc lisahi
link68 command.rel = coma,command,lmul
relmod command.rel command.prg
