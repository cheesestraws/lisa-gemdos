rem build freeze

\tools\cp68 freeze.c freeze.i
\tools\c068 freeze.i freeze.1 freeze.2 freeze.3 -f 
\tools\c168 freeze.1 freeze.2 freeze.s 
era freeze.i
era freeze.1
era freeze.2
\tools\as68 -l -s \tools\ -u freeze.s 
era  freeze.s

\tools\link68 freeze.rel = freeze
\tools\relmod freeze.rel freeze.prg

era freeze.rel