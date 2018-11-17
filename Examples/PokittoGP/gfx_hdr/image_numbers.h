

/*
 * BMP image as 8bpp (up to 256 color index) data
 */

#include <stdint.h>

const uint8_t image_numbers[] = {
6,77,
0x00,0x14,0x14,0x14,0x00,0x00,
0x14,0x1D,0xD1,0xD1,0x14,0x00,
0x14,0x1D,0x14,0xD1,0x14,0x00,
0x14,0x1D,0x14,0xD1,0x14,0x00,
0x14,0x1D,0x14,0xD1,0x14,0x00,
0x14,0x1D,0xD1,0xD1,0x14,0x00,
0x00,0x14,0x14,0x14,0x00,0x00,
0x00,0x00,0x14,0x00,0x00,0x00,
0x00,0x14,0xD1,0x14,0x00,0x00,
0x14,0x1D,0xD1,0x14,0x00,0x00,
0x00,0x14,0xD1,0x14,0x00,0x00,
0x00,0x14,0xD1,0x14,0x00,0x00,
0x14,0x1D,0xD1,0xD1,0x14,0x00,
0x00,0x14,0x14,0x14,0x00,0x00,
0x00,0x14,0x14,0x00,0x00,0x00,
0x14,0x1D,0xD1,0x14,0x00,0x00,
0x00,0x14,0x14,0xD1,0x14,0x00,
0x00,0x14,0xD1,0x14,0x00,0x00,
0x14,0x1D,0x14,0x14,0x00,0x00,
0x14,0x1D,0xD1,0xD1,0x14,0x00,
0x00,0x14,0x14,0x14,0x00,0x00,
0x00,0x14,0x14,0x00,0x00,0x00,
0x14,0x1D,0xD1,0x14,0x00,0x00,
0x00,0x14,0x14,0xD1,0x14,0x00,
0x00,0x14,0xD1,0x14,0x00,0x00,
0x00,0x14,0x14,0xD1,0x14,0x00,
0x14,0x1D,0xD1,0x14,0x00,0x00,
0x00,0x14,0x14,0x00,0x00,0x00,
0x00,0x00,0x00,0x14,0x00,0x00,
0x00,0x14,0x14,0xD1,0x14,0x00,
0x14,0x1D,0x14,0xD1,0x14,0x00,
0x14,0x1D,0xD1,0xD1,0x14,0x00,
0x00,0x14,0x14,0xD1,0x14,0x00,
0x00,0x00,0x14,0xD1,0x14,0x00,
0x00,0x00,0x00,0x14,0x00,0x00,
0x00,0x14,0x14,0x14,0x00,0x00,
0x14,0x1D,0xD1,0xD1,0x14,0x00,
0x14,0x1D,0x14,0x14,0x00,0x00,
0x00,0x14,0xD1,0xD1,0x14,0x00,
0x00,0x14,0x14,0xD1,0x14,0x00,
0x14,0x1D,0xD1,0xD1,0x14,0x00,
0x00,0x14,0x14,0x14,0x00,0x00,
0x00,0x00,0x14,0x14,0x00,0x00,
0x00,0x14,0xD1,0xD1,0x14,0x00,
0x14,0x1D,0x14,0x14,0x00,0x00,
0x14,0x1D,0xD1,0xD1,0x14,0x00,
0x14,0x1D,0x14,0xD1,0x14,0x00,
0x14,0x1D,0xD1,0xD1,0x14,0x00,
0x00,0x14,0x14,0x14,0x00,0x00,
0x00,0x14,0x14,0x14,0x00,0x00,
0x14,0x1D,0x1D,0xD1,0x14,0x00,
0x00,0x14,0x14,0xD1,0x14,0x00,
0x00,0x14,0xD1,0x14,0x00,0x00,
0x00,0x14,0xD1,0x14,0x00,0x00,
0x00,0x14,0xD1,0x14,0x00,0x00,
0x00,0x00,0x14,0x00,0x00,0x00,
0x00,0x14,0x14,0x14,0x00,0x00,
0x14,0x1D,0xD1,0xD1,0x14,0x00,
0x14,0x1D,0x14,0xD1,0x14,0x00,
0x14,0x1D,0xD1,0xD1,0x14,0x00,
0x14,0x1D,0x14,0xD1,0x14,0x00,
0x14,0x1D,0xD1,0xD1,0x14,0x00,
0x00,0x14,0x14,0x14,0x00,0x00,
0x00,0x14,0x14,0x14,0x00,0x00,
0x14,0x1D,0xD1,0xD1,0x14,0x00,
0x14,0x1D,0x14,0xD1,0x14,0x00,
0x14,0x1D,0xD1,0xD1,0x14,0x00,
0x00,0x14,0x14,0xD1,0x14,0x00,
0x00,0x00,0x14,0xD1,0x14,0x00,
0x00,0x00,0x00,0x14,0x00,0x00,
0x00,0x14,0x00,0x14,0x00,0x00,
0x14,0x1D,0x14,0xD1,0x14,0x00,
0x14,0x1D,0x14,0xD1,0x14,0x00,
0x00,0x14,0x00,0x14,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,
};