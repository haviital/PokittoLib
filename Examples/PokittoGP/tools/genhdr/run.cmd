
echo on

rem *** palette

genpalette image_numbers.bmp image_shadow.bmp image_ship.bmp image_sky.bmp image_cactus.bmp image_rock1.bmp image_spot.bmp image_ball1.bmp image_terrain_6.bmp image_start.bmp image_road1.bmp image_road2.bmp image_light.bmp
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
call ./genhdr.cmd image_spot

rem textures

call ./genhdr_split4_mipmap.cmd image_ball1

call ./genhdr_split4_mipmap.cmd image_terrain_6

call ./genhdr_split4_mipmap.cmd image_start

call ./genhdr.cmd image_road1
call ./genhdr.cmd image_road2
call ./genhdr.cmd image_light

call ./genhdr.cmd image_sky
call ./genhdr.cmd image_sky_long

rem *** special pictures

rem the pilots
copy image_pilots1.bmp output\image_pilots1.bmp
cd output
..\bmp2pok image_pilots1.bmp -8 -p -n
copy image_pilots1.c ..\..\..\gfx_hdr\image_pilots1.h
cd ..
copy image_pilots2.bmp output\image_pilots2.bmp
cd output
..\bmp2pok image_pilots2.bmp -8 -p -n
copy image_pilots2.c ..\..\..\gfx_hdr\image_pilots2.h
cd ..

rem the title screen
copy image_titlescreen.bmp output\image_titlescreen.bmp
cd output
..\bmp2pok image_titlescreen.bmp -8 -p -n
copy image_titlescreen.c ..\..\..\gfx_hdr\image_titlescreen.h
cd ..








