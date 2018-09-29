REM *** splits the 16x16 bmp to four 8x8 ppms and makes textures of them

echo on
 

C:\bin\NetPBM-10.68\bin\bmptoppm %1.bmp > tmp\%1.ppm

rem *** A: top left 8x8
rem Get each mipmap as a separate image
C:\bin\NetPBM-10.68\bin\pamcut -top=0 -left=0 -width=8 -height=8 tmp\%1.ppm > tmp\%1_a_mm1.ppm
C:\bin\NetPBM-10.68\bin\pamcut -top=16 -left=0 -width=4 -height=4 tmp\%1.ppm > tmp\%1_a_mm2.ppm
C:\bin\NetPBM-10.68\bin\pamcut -top=16 -left=8 -width=2 -height=2 tmp\%1.ppm > tmp\%1_a_mm3.ppm

rem Create 8x12 image of the 8x8 image
C:\bin\NetPBM-10.68\bin\pamcut -top=0 -left=0 -width=8 -height=12 -pad tmp\%1_a_mm1.ppm > tmp\%1_a_mm1_tmp.ppm

rem Paste the mipmaps on the 8x12 image
C:\bin\NetPBM-10.68\bin\pnmpaste tmp\%1_a_mm2.ppm 0 8 tmp\%1_a_mm1_tmp.ppm > tmp\%1_a_mm2_tmp.ppm
C:\bin\NetPBM-10.68\bin\pnmpaste tmp\%1_a_mm3.ppm 4 8 tmp\%1_a_mm2_tmp.ppm > tmp\%1_a.ppm

rem Apply the palette and create the header file
call ./genhdr_ppm.cmd %1_a

rem *** B: top right 8x8
rem Get each mipmap as a separate image
C:\bin\NetPBM-10.68\bin\pamcut -top=0 -left=8 -width=8 -height=8 tmp\%1.ppm > tmp\%1_b_mm1.ppm
C:\bin\NetPBM-10.68\bin\pamcut -top=16 -left=4 -width=4 -height=4 tmp\%1.ppm > tmp\%1_b_mm2.ppm
C:\bin\NetPBM-10.68\bin\pamcut -top=16 -left=10 -width=2 -height=2 tmp\%1.ppm > tmp\%1_b_mm3.ppm

rem Create 8x12 image of the 8x8 image
C:\bin\NetPBM-10.68\bin\pamcut -top=0 -left=0 -width=8 -height=12 -pad tmp\%1_b_mm1.ppm > tmp\%1_b_mm1_tmp.ppm

rem Paste the mipmaps on the 8x12 image
C:\bin\NetPBM-10.68\bin\pnmpaste tmp\%1_b_mm2.ppm 0 8 tmp\%1_b_mm1_tmp.ppm > tmp\%1_b_mm2_tmp.ppm
C:\bin\NetPBM-10.68\bin\pnmpaste tmp\%1_b_mm3.ppm 4 8 tmp\%1_b_mm2_tmp.ppm > tmp\%1_b.ppm

rem Apply the palette and create the header file
call ./genhdr_ppm.cmd %1_b

rem *** C: bottom left 8x8
rem Get each mipmap as a separate image
C:\bin\NetPBM-10.68\bin\pamcut -top=8 -left=0 -width=8 -height=8 tmp\%1.ppm > tmp\%1_c_mm1.ppm
C:\bin\NetPBM-10.68\bin\pamcut -top=20 -left=0 -width=4 -height=4 tmp\%1.ppm > tmp\%1_c_mm2.ppm
C:\bin\NetPBM-10.68\bin\pamcut -top=18 -left=8 -width=2 -height=2 tmp\%1.ppm > tmp\%1_c_mm3.ppm

rem Create 8x12 image of the 8x8 image
C:\bin\NetPBM-10.68\bin\pamcut -top=0 -left=0 -width=8 -height=12 -pad tmp\%1_c_mm1.ppm > tmp\%1_c_mm1_tmp.ppm

rem Paste the mipmaps on the 8x12 image
C:\bin\NetPBM-10.68\bin\pnmpaste tmp\%1_c_mm2.ppm 0 8 tmp\%1_c_mm1_tmp.ppm > tmp\%1_c_mm2_tmp.ppm
C:\bin\NetPBM-10.68\bin\pnmpaste tmp\%1_c_mm3.ppm 4 8 tmp\%1_c_mm2_tmp.ppm > tmp\%1_c.ppm

rem Apply the palette and create the header file
call ./genhdr_ppm.cmd %1_c

rem *** D: bottom right 8x8
rem Get each mipmap as a separate image
C:\bin\NetPBM-10.68\bin\pamcut -top=8 -left=8 -width=8 -height=8 tmp\%1.ppm > tmp\%1_d_mm1.ppm
C:\bin\NetPBM-10.68\bin\pamcut -top=20 -left=4 -width=4 -height=4 tmp\%1.ppm > tmp\%1_d_mm2.ppm
C:\bin\NetPBM-10.68\bin\pamcut -top=18 -left=10 -width=2 -height=2 tmp\%1.ppm > tmp\%1_d_mm3.ppm

rem Create 8x12 image of the 8x8 image
C:\bin\NetPBM-10.68\bin\pamcut -top=0 -left=0 -width=8 -height=12 -pad tmp\%1_d_mm1.ppm > tmp\%1_d_mm1_tmp.ppm

rem Paste the mipmaps on the 8x12 image
C:\bin\NetPBM-10.68\bin\pnmpaste tmp\%1_d_mm2.ppm 0 8 tmp\%1_d_mm1_tmp.ppm > tmp\%1_d_mm2_tmp.ppm
C:\bin\NetPBM-10.68\bin\pnmpaste tmp\%1_d_mm3.ppm 4 8 tmp\%1_d_mm2_tmp.ppm > tmp\%1_d.ppm

rem Apply the palette and create the header file
call ./genhdr_ppm.cmd %1_d

