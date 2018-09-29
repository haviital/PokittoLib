REM *** splits the big ship bmp to 25 ship ppms and makes textures of them

echo on
 

C:\bin\NetPBM-10.68\bin\bmptoppm %1.bmp > tmp\%1.ppm

C:\bin\NetPBM-10.68\bin\pamcut -top=1 -left=1 -width=22 -height=12 tmp\%1.ppm > tmp\%1_1_a.ppm
call ./genhdr_ppm.cmd %1_1_a
C:\bin\NetPBM-10.68\bin\pamcut -top=1 -left=25 -width=22 -height=12 tmp\%1.ppm > tmp\%1_1_b.ppm
call ./genhdr_ppm.cmd %1_1_b
C:\bin\NetPBM-10.68\bin\pamcut -top=1 -left=49 -width=22 -height=12 tmp\%1.ppm > tmp\%1_1_c.ppm
call ./genhdr_ppm.cmd %1_1_c
C:\bin\NetPBM-10.68\bin\pamcut -top=1 -left=73 -width=22 -height=12 tmp\%1.ppm > tmp\%1_1_d.ppm
call ./genhdr_ppm.cmd %1_1_d
C:\bin\NetPBM-10.68\bin\pamcut -top=1 -left=97 -width=22 -height=12 tmp\%1.ppm > tmp\%1_1_e.ppm
call ./genhdr_ppm.cmd %1_1_e

C:\bin\NetPBM-10.68\bin\pamcut -top=15 -left=1 -width=22 -height=12 tmp\%1.ppm > tmp\%1_2_a.ppm
call ./genhdr_ppm.cmd %1_2_a
C:\bin\NetPBM-10.68\bin\pamcut -top=15 -left=25 -width=22 -height=12 tmp\%1.ppm > tmp\%1_2_b.ppm
call ./genhdr_ppm.cmd %1_2_b
C:\bin\NetPBM-10.68\bin\pamcut -top=15 -left=49 -width=22 -height=12 tmp\%1.ppm > tmp\%1_2_c.ppm
call ./genhdr_ppm.cmd %1_2_c
C:\bin\NetPBM-10.68\bin\pamcut -top=15 -left=73 -width=22 -height=12 tmp\%1.ppm > tmp\%1_2_d.ppm
call ./genhdr_ppm.cmd %1_2_d
C:\bin\NetPBM-10.68\bin\pamcut -top=15 -left=97 -width=22 -height=12 tmp\%1.ppm > tmp\%1_2_e.ppm
call ./genhdr_ppm.cmd %1_2_e

C:\bin\NetPBM-10.68\bin\pamcut -top=29 -left=1 -width=22 -height=12 tmp\%1.ppm > tmp\%1_3_a.ppm
call ./genhdr_ppm.cmd %1_3_a
C:\bin\NetPBM-10.68\bin\pamcut -top=29 -left=25 -width=22 -height=12 tmp\%1.ppm > tmp\%1_3_b.ppm
call ./genhdr_ppm.cmd %1_3_b
C:\bin\NetPBM-10.68\bin\pamcut -top=29 -left=49 -width=22 -height=12 tmp\%1.ppm > tmp\%1_3_c.ppm
call ./genhdr_ppm.cmd %1_3_c
C:\bin\NetPBM-10.68\bin\pamcut -top=29 -left=73 -width=22 -height=12 tmp\%1.ppm > tmp\%1_3_d.ppm
call ./genhdr_ppm.cmd %1_3_d
C:\bin\NetPBM-10.68\bin\pamcut -top=29 -left=97 -width=22 -height=12 tmp\%1.ppm > tmp\%1_3_e.ppm
call ./genhdr_ppm.cmd %1_3_e

C:\bin\NetPBM-10.68\bin\pamcut -top=43 -left=1 -width=22 -height=12 tmp\%1.ppm > tmp\%1_4_a.ppm
call ./genhdr_ppm.cmd %1_4_a
C:\bin\NetPBM-10.68\bin\pamcut -top=43 -left=25 -width=22 -height=12 tmp\%1.ppm > tmp\%1_4_b.ppm
call ./genhdr_ppm.cmd %1_4_b
C:\bin\NetPBM-10.68\bin\pamcut -top=43 -left=49 -width=22 -height=12 tmp\%1.ppm > tmp\%1_4_c.ppm
call ./genhdr_ppm.cmd %1_4_c
C:\bin\NetPBM-10.68\bin\pamcut -top=43 -left=73 -width=22 -height=12 tmp\%1.ppm > tmp\%1_4_d.ppm
call ./genhdr_ppm.cmd %1_4_d
C:\bin\NetPBM-10.68\bin\pamcut -top=43 -left=97 -width=22 -height=12 tmp\%1.ppm > tmp\%1_4_e.ppm
call ./genhdr_ppm.cmd %1_4_e

C:\bin\NetPBM-10.68\bin\pamcut -top=57 -left=1 -width=22 -height=12 tmp\%1.ppm > tmp\%1_5_a.ppm
call ./genhdr_ppm.cmd %1_5_a
C:\bin\NetPBM-10.68\bin\pamcut -top=57 -left=25 -width=22 -height=12 tmp\%1.ppm > tmp\%1_5_b.ppm
call ./genhdr_ppm.cmd %1_5_b
C:\bin\NetPBM-10.68\bin\pamcut -top=57 -left=49 -width=22 -height=12 tmp\%1.ppm > tmp\%1_5_c.ppm
call ./genhdr_ppm.cmd %1_5_c
C:\bin\NetPBM-10.68\bin\pamcut -top=57 -left=73 -width=22 -height=12 tmp\%1.ppm > tmp\%1_5_d.ppm
call ./genhdr_ppm.cmd %1_5_d
C:\bin\NetPBM-10.68\bin\pamcut -top=57 -left=97 -width=22 -height=12 tmp\%1.ppm > tmp\%1_5_e.ppm
call ./genhdr_ppm.cmd %1_5_e


