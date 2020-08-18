rem
rem
rem *************************
rem 
rem MAKING THE BDOS
rem 
rem *************************
rem
rem
cd \BDOSSRC
make

rem
rem
rem *************************
rem 
rem MAKING THE BIOS    
rem                        
rem *************************
rem
rem
cd \SYSBLD
setup
make
copy gemdos.sys \output