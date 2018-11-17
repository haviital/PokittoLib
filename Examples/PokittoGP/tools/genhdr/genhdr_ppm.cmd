
echo on

C:\bin\NetPBM-10.68\bin\ppmtobmp tmp\%1.ppm -mapfile=tmp\palette256.ppm > output\%1.bmp
cd output
..\bmp2pok %1.bmp -8 -n
copy %1.c ..\..\..\gfx_hdr\%1.h
cd ..

