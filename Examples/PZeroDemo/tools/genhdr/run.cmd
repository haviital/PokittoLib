
echo on

rem *** palette

genpalette image_shadow.bmp image_ship.bmp image_cactus.bmp image_rock1.bmp image_ball1.bmp image_ball2.bmp image_ball3.bmp image_ball4.bmp image_ball5.bmp image_offroad1_a.bmp image_terrain_1.bmp image_terrain_2.bmp image_terrain_3.bmp image_terrain_4.bmp image_terrain_5.bmp image_terrain_6.bmp image_terrain_7.bmp image_terrain_8.bmp image_start.bmp image_road1.bmp image_road2.bmp image_sky.bmp image_numbers.bmp image_light.bmp
C:\bin\NetPBM-10.68\bin\pngtopnm palette.png > tmp\palette256.ppm
rem C:\bin\NetPBM-10.68\bin\ppmtobmp tmp\palette256.ppm > palette.bmp
C:\bin\NetPBM-10.68\bin\ppmtobmp tmp\palette256.ppm -mapfile=tmp\palette256.ppm > output\palette.bmp
cd output
..\bmp2pok palette.bmp -8 -p -n
copy palette.c ..\..\..\gfx_hdr\palette.h
cd ..

rem *** images

call ./genhdr.cmd image_numbers

rem *** textures

call ./genhdr.cmd image_shadow

call ./genhdr_split25_ship.cmd image_ship

rem call ./genhdr.cmd image_pokitto
call ./genhdr.cmd image_cactus
call ./genhdr.cmd image_rock1

rem textures

call ./genhdr_split4_mipmap.cmd image_ball1
call ./genhdr_split4.cmd image_ball2
call ./genhdr_split4.cmd image_ball3
call ./genhdr_split4.cmd image_ball4
call ./genhdr_split4.cmd image_ball5

rem call ./genhdr_split4_mipmap.cmd image_terrain_1
rem call ./genhdr_split4_mipmap.cmd image_terrain_2
rem call ./genhdr_split4_mipmap.cmd image_terrain_3
rem call ./genhdr_split4_mipmap.cmd image_terrain_4
rem call ./genhdr_split4_mipmap.cmd image_terrain_5
call ./genhdr_split4_mipmap.cmd image_terrain_6
call ./genhdr_split4_mipmap.cmd image_terrain_7
call ./genhdr_split4_mipmap.cmd image_terrain_8

call ./genhdr_split4_mipmap.cmd image_start

call ./genhdr.cmd image_road1
call ./genhdr.cmd image_road2
call ./genhdr.cmd image_light

call ./genhdr.cmd image_sky
call ./genhdr.cmd image_sky_long








