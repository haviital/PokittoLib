

/*
 * BMP image as 4bpp (16 colour index) data
 */

#include <stdint.h>
#include "Pixonia.h"


const uint16_t ter_hut_pal[] = {
22530,12922,10764,9284,13670,20137,65312,41728,33280,65535,46486,38066,29614,0,63488,63519
};

const uint8_t ter_hut[] = {
18,19,
255,255,255,255,255,255,255,255,255,255,255,255,188,191,255,255,255,255,
255,255,255,45,199,136,15,255,255,255,255,120,34,136,0,135,255,255,
255,136,136,136,0,120,120,143,255,136,136,135,0,120,135,7,136,255,
240,0,208,120,135,136,112,120,127,248,0,136,7,135,112,119,7,143,
240,128,0,8,120,135,7,125,255,247,8,128,0,112,120,125,215,255,
248,0,8,135,119,141,102,128,255,240,13,208,0,141,214,109,141,255,
247,13,208,0,8,141,216,215,255,248,13,216,128,112,0,215,112,255,
48,13,208,8,141,215,112,3,255,116,221,208,0,7,112,13,71,255,
135,131,72,128,112,13,55,120,255,120,136,131,8,141,55,119,119,255,
120,136,136,131,55,119,136,119,255,
};
