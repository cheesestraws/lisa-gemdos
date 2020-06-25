rem MAKING THE BDOS

path c:\tools

a10 rwa
bdosc @allc

del \bdosobj\*.o
copy *.o \bdosobj