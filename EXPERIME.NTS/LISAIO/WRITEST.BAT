rem build writest

\tools\cp68 writest.c writest.i
\tools\c068 writest.i writest.1 writest.2 writest.3 -f 
\tools\c168 writest.1 writest.2 writest.s 
era  writest.1
era writest.2

\tools\as68 -l -s \tools\ -u writest.s 
\tools\as68 -l -s \tools\ -u profilio.s 
\tools\as68 -l -s \tools\ -u profiglu.s 

era  writest.s

\tools\link68 writest.rel = writest,profiglu,profilio,mini
\tools\relmod writest.rel writest.prg

