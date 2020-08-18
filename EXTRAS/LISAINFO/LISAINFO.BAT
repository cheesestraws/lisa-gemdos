rem build lisainfo

\tools\cp68 lisainfo.c lisainfo.i
\tools\c068 lisainfo.i lisainfo.1 lisainfo.2 lisainfo.3 -f 
\tools\c168 lisainfo.1 lisainfo.2 lisainfo.s 
era lisainfo.i
era lisainfo.1
era lisainfo.2
\tools\as68 -l -s \tools\ -u lisainfo.s 
era  lisainfo.s

\tools\link68 lisainfo.rel = lisainfo,mini
\tools\relmod lisainfo.rel lisainfo.prg

era lisainfo.rel