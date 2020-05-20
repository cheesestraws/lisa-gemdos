path c:\tools;c:\
cp \BIOSSRC\ @cfls .c
cp \BIOSSRC\ @sfls .s
copy @hfls
copy @ofls
ren bios.h fs.h
