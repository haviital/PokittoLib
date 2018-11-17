REM *** splits the 16x16 bmp to four 8x8 ppms and makes textures of them

echo on
 

C:\bin\NetPBM-10.68\bin\bmptoppm %1.bmp > tmp\%1.ppm
C:\bin\NetPBM-10.68\bin\pamcut -top=0 -left=0 -width=8 -height=8 tmp\%1.ppm > tmp\%1_a.ppm
call ./genhdr_ppm.cmd %1_a
C:\bin\NetPBM-10.68\bin\pamcut -top=0 -left=8 -width=8 -height=8 tmp\%1.ppm > tmp\%1_b.ppm
call ./genhdr_ppm.cmd %1_b
C:\bin\NetPBM-10.68\bin\pamcut -top=8 -left=0 -width=8 -height=8 tmp\%1.ppm > tmp\%1_c.ppm
call ./genhdr_ppm.cmd %1_c
C:\bin\NetPBM-10.68\bin\pamcut -top=8 -left=8 -width=8 -height=8 tmp\%1.ppm > tmp\%1_d.ppm
call ./genhdr_ppm.cmd %1_d

