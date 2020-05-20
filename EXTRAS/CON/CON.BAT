rem build con

\tools\cp68 con.c con.i
\tools\c068 con.i con.1 con.2 con.3 -f 
\tools\c168 con.1 con.2 con.s 
era  con.1
era con.2
era con.i
\tools\as68 -l -s \tools\ -u con.s 
era  con.s
