rem build trapdump

\tools\cp68 trapdump.c trapdump.i
\tools\c068 trapdump.i trapdump.1 trapdump.2 trapdump.3 -f 
\tools\c168 trapdump.1 trapdump.2 trapdump.s 
era trapdump.i
era trapdump.1
era trapdump.2
\tools\as68 -l -s \tools\ -u trapdump.s 
era  trapdump.s

\tools\link68 trapdump.rel = trapdump,mini
\tools\relmod trapdump.rel trapdump.prg

era trapdump.rel