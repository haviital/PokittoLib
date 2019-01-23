/**************************************************************************/
/*!
    @file     GameData.h
    @author   Hannu Viitala

*/
/**************************************************************************/

#include "Pokitto.h"
#include "fix16.h"
#include "main.h"

#include "palette.h"
#include "image_shadow.h"
#include "image_ship_1_a.h"
#include "image_ship_1_b.h"
#include "image_ship_1_c.h"
#include "image_ship_1_d.h"
#include "image_ship_1_e.h"
#include "image_ship_2_a.h"
#include "image_ship_2_b.h"
#include "image_ship_2_c.h"
#include "image_ship_2_d.h"
#include "image_ship_2_e.h"
#include "image_ship_3_a.h"
#include "image_ship_3_b.h"
#include "image_ship_3_c.h"
#include "image_ship_3_d.h"
#include "image_ship_3_e.h"
#include "image_ship_4_a.h"
#include "image_ship_4_b.h"
#include "image_ship_4_c.h"
#include "image_ship_4_d.h"
#include "image_ship_4_e.h"
#include "image_ship_5_a.h"
#include "image_ship_5_b.h"
#include "image_ship_5_c.h"
#include "image_ship_5_d.h"
#include "image_ship_5_e.h"
#include "image_cactus.h"
#include "image_rock1.h"
#include "image_ball1_a.h"
#include "image_ball1_b.h"
#include "image_ball1_c.h"
#include "image_ball1_d.h"
#include "image_terrain_6_a.h"
#include "image_terrain_6_b.h"
#include "image_terrain_6_c.h"
#include "image_terrain_6_d.h"
#include "image_start_a.h"
#include "image_start_b.h"
#include "image_start_c.h"
#include "image_start_d.h"
#include "image_road1.h"
#include "image_road2.h"
#include "image_sky.h"
#include "image_sky_long.h"
#include "image_light.h"
#include "image_spot.h"

// Palette
// UI colors ==> 0-7 colors
// image_numbers.bmp ==> 0-10 colors
// image_numbers.bmp image_shadow.bmp image_ship.bmp ==> 0-31 colors
// image_numbers.bmp image_shadow.bmp image_ship.bmp image_sky.bmp ==> 0-40 colors
// all: 0-83 colors

 // Defines blocks of 64 (8x8) tiles
 const uint8_t blockData[][8*8] = {

    // block: 0
    {
        // 8x8 tile indexes.Left edge of the road
        7, 8, 7, 1, 2, 5, 5, 5,
        9,10, 9, 3, 4, 5, 5, 5,
        7, 8, 7, 1, 2, 6, 6, 6,
        9,10, 9, 3, 4, 6, 6, 6,
        7, 8, 7, 1, 2, 5, 5, 5,
        9,10, 9, 3, 4, 5, 5, 5,
        7, 8, 7, 1, 2, 6, 6, 6,
        9,10, 9, 3, 4, 6, 6, 6,
     },
    // block: 1
    {
        // 8x8 tile indexes. Right edge of the road
        5, 5, 5, 1, 2, 8, 7, 8,
        5, 5, 5, 3, 4, 9,10,10,
        6, 6, 6, 1, 2, 8, 7, 8,
        6, 6, 6, 3, 4, 9,10,10,
        5, 5, 5, 1, 2, 8, 7, 8,
        5, 5, 5, 3, 4, 9,10,10,
        6, 6, 6, 1, 2, 8, 7, 8,
        6, 6, 6, 3, 4, 9,10,10,
    },
    // block: 2. Terrain
    {
        // 8x8 tile indexes
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
    },
    // block: 3. Road middle block, vertical
    {
        // 8x8 tile indexes
        5, 5, 5, 5, 5, 5, 5, 5,
        5, 5, 5, 5, 5, 5, 5, 5,
        6, 6, 6, 6, 6, 6, 6, 6,
        6, 6, 6, 6, 6, 6, 6, 6,
        5, 5, 5, 5, 5, 5, 5, 5,
        5, 5, 5, 5, 5, 5, 5, 5,
        6, 6, 6, 6, 6, 6, 6, 6,
        6, 6, 6, 6, 6, 6, 6, 6,
    },
     // block: 4
    {
        // 8x8 tile indexes.Top edge of the road
        7, 8, 7, 8, 7, 8, 7, 8,
        9,10, 9,10, 9,10, 9,10,
        7, 8, 7, 8, 7, 8, 7, 8,
        1, 2, 1, 2, 1, 2, 1, 2,
        3, 4, 3, 4, 3, 4, 3, 4,
        5, 5, 6, 6, 5, 5, 6, 6,
        5, 5, 6, 6, 5, 5, 6, 6,
        5, 5, 6, 6, 5, 5, 6, 6,
     },
     // block: 5
    {
        // 8x8 tile indexes. Bottom edge of the road
        5, 5, 6, 6, 5, 5, 6, 6,
        5, 5, 6, 6, 5, 5, 6, 6,
        5, 5, 6, 6, 5, 5, 6, 6,
        1, 2, 1, 2, 1, 2, 1, 2,
        3, 4, 3, 4, 3, 4, 3, 4,
        9,10, 9,10, 9,10, 9,10,
        7, 8, 7, 8, 7, 8, 7, 8,
        9,10, 9,10, 9,10, 9,10,
    },
    // block: 6. TL outer corner.
    {
        // 8x8 tile indexes.
        7, 8, 7, 8, 7, 8, 7, 8,
        9,10, 9,10, 9,10, 9,10,
        7, 8, 7, 8, 7, 8, 7, 8,
        9,10, 9,10, 9,10, 9,10,
        7, 8, 7, 8, 7, 8, 1, 2,
        9,10, 9,10, 9,10, 3, 4,
        7, 8, 7, 8, 1, 2, 6, 6,
        9,10, 9,10, 3, 4, 6, 6,
     },
    // block: 7. TL inner corner.
    {
        // 8x8 tile indexes.
        6, 6, 6, 6, 6, 6, 6, 6,
        6, 6, 6, 6, 6, 6, 6, 6,
        6, 6, 6, 6, 6, 6, 6, 6,
        6, 6, 6, 6, 6, 6, 6, 6,
        6, 6, 6, 6, 6, 6, 1, 2,
        6, 6, 6, 6, 6, 6, 3, 4,
        6, 6, 6, 6, 1, 2, 7, 8,
        6, 6, 6, 6, 3, 4, 9,10,
     },
    // block: 8. BL outer corner.
    {
        // 8x8 tile indexes.
        7, 8, 7, 8, 1, 2, 6, 6,
        9,10, 9,10, 3, 4, 6, 6,
        7, 8, 7, 8, 7, 8, 1, 2,
        9,10, 9,10, 9,10, 3, 4,
        7, 8, 7, 8, 7, 8, 7, 8,
        9,10, 9,10, 9,10, 9,10,
        7, 8, 7, 8, 7, 8, 7, 8,
        9,10, 9,10, 9,10, 9,10,
     },
    // block: 9. BL inner corner.
    {
        // 8x8 tile indexes.
        6, 6, 6, 6, 1, 2, 7, 8,
        6, 6, 6, 6, 3, 4, 9,10,
        6, 6, 6, 6, 6, 6, 1, 2,
        6, 6, 6, 6, 6, 6, 3, 4,
        6, 6, 6, 6, 6, 6, 6, 6,
        6, 6, 6, 6, 6, 6, 6, 6,
        6, 6, 6, 6, 6, 6, 6, 6,
        6, 6, 6, 6, 6, 6, 6, 6,
     },
    // block: 10. TR outer corner.
    {
        // 8x8 tile indexes.
        7, 8, 7, 8, 7, 8, 7, 8,
        9,10, 9,10, 9,10, 9,10,
        7, 8, 7, 8, 7, 8, 7, 8,
        9,10, 9,10, 9,10, 9,10,
        1, 2, 7, 8, 7, 8, 7, 8,
        3, 4, 9,10, 9,10, 9,10,
        6, 6, 1, 2, 7, 8, 7, 8,
        6, 6, 3, 4, 9,10, 9,10,
     },
    // block: 11. TR inner corner.
    {
        // 8x8 tile indexes.
        6, 6, 6, 6, 6, 6, 6, 6,
        6, 6, 6, 6, 6, 6, 6, 6,
        6, 6, 6, 6, 6, 6, 6, 6,
        6, 6, 6, 6, 6, 6, 6, 6,
        1, 2, 6, 6, 6, 6, 6, 6,
        3, 4, 6, 6, 6, 6, 6, 6,
        7, 8, 1, 2, 6, 6, 6, 6,
        9,10, 3, 4, 6, 6, 6, 6,
     },
    // block: 12. BR outer corner.
    {
        // 8x8 tile indexes.
        6, 6, 1, 2, 7, 8, 7, 8,
        6, 6, 3, 4, 9,10, 9,10,
        1, 2, 7, 8, 7, 8, 7, 8,
        3, 4, 9,10, 9,10, 9,10,
        7, 8, 7, 8, 7, 8, 7, 8,
        9,10, 9,10, 9,10, 9,10,
        7, 8, 7, 8, 7, 8, 7, 8,
        9,10, 9,10, 9,10, 9,10,
     },
    // block: 13. BR inner corner.
    {
        // 8x8 tile indexes.
        7, 8, 1, 2, 6, 6, 6, 6,
        9,10, 3, 4, 6, 6, 6, 6,
        1, 2, 6, 6, 6, 6, 6, 6,
        3, 4, 6, 6, 6, 6, 6, 6,
        6, 6, 6, 6, 6, 6, 6, 6,
        6, 6, 6, 6, 6, 6, 6, 6,
        6, 6, 6, 6, 6, 6, 6, 6,
        6, 6, 6, 6, 6, 6, 6, 6,
     },
    // block: 14. Terrain.
    {
        // 8x8 tile indexes.
        7, 8, 7, 8, 7, 8, 7, 8,
        9,10, 9,10, 9,10, 9,10,
        7, 8, 7, 8, 7, 8, 7, 8,
        9,10, 9,10, 9,10, 9,10,
        7, 8, 7, 8, 7, 8, 7, 8,
        9,10, 9,10, 9,10, 9,10,
        7, 8, 7, 8, 7, 8, 7, 8,
        9,10, 9,10, 9,10, 9,10,
     },

    // block: 15
    {
        // 8x8 tile indexes.Left edge of the road. Start grid.
         8, 7, 8, 1, 2,11,12,11,
        10, 9,10, 3, 4,13,14,13,
         8, 7, 8, 1, 2,11,12,11,
        10, 9,10, 3, 4,13,14,13,
         8, 7, 8, 1, 2,11,12,11,
        10, 9,10, 3, 4,13,14,13,
         8, 7, 8, 1, 2,11,12,11,
        10, 9,10, 3, 4,13,14,13,
     },

    // block: 16
    {
        // 8x8 tile indexes. Right edge of the road. Start grid.
       12,11,12, 1, 2, 8, 7, 8,
       14,13,14, 3, 4,10, 9,10,
       12,11,12, 1, 2, 8, 7, 8,
       14,13,14, 3, 4,10, 9,10,
       12,11,12, 1, 2, 8, 7, 8,
       14,13,14, 3, 4,10, 9,10,
       12,11,12, 1, 2, 8, 7, 8,
       14,13,14, 3, 4,10, 9,10,
    },
    // block: 17
    {
        // 8x8 tile indexes.Left edge of the road. Light strip.
         8, 1, 2,15,15,15,15,15,
        10, 3, 4,15,15,15,15,15,
         8, 1, 2,15,15,15,15,15,
        10, 3, 4,15,15,15,15,15,
         8, 1, 2,15,15,15,15,15,
        10, 3, 4,15,15,15,15,15,
         8, 1, 2,15,15,15,15,15,
        10, 3, 4,15,15,15,15,15,
     },

    // block: 18
    {
        // 8x8 tile indexes. Right edge of the road. Light strip.
        15,15,15,15,15, 1, 2, 8,
        15,15,15,15,15, 3, 4,10,
        15,15,15,15,15, 1, 2, 8,
        15,15,15,15,15, 3, 4,10,
        15,15,15,15,15, 1, 2, 8,
        15,15,15,15,15, 3, 4,10,
        15,15,15,15,15, 1, 2, 8,
        15,15,15,15,15, 3, 4,10,
    },

    // block: 19. Road middle block, horizontal
    {
        // 8x8 tile indexes
        5, 5, 6, 6, 5, 5, 6, 6,
        5, 5, 6, 6, 5, 5, 6, 6,
        5, 5, 6, 6, 5, 5, 6, 6,
        5, 5, 6, 6, 5, 5, 6, 6,
        5, 5, 6, 6, 5, 5, 6, 6,
        5, 5, 6, 6, 5, 5, 6, 6,
        5, 5, 6, 6, 5, 5, 6, 6,
        5, 5, 6, 6, 5, 5, 6, 6,
    },
    // block: 20. Road middle block, empty
    {
        // 8x8 tile indexes
        6, 6, 6, 6, 6, 6, 6, 6,
        6, 6, 6, 6, 6, 6, 6, 6,
        6, 6, 6, 6, 6, 6, 6, 6,
        6, 6, 6, 6, 6, 6, 6, 6,
        6, 6, 6, 6, 6, 6, 6, 6,
        6, 6, 6, 6, 6, 6, 6, 6,
        6, 6, 6, 6, 6, 6, 6, 6,
        6, 6, 6, 6, 6, 6, 6, 6,
    },
};

// Map of blocks. Defines the whole game field!
const uint8_t blockMapROM[mapWidth*mapHeight] = {

// I      0     I     1     I     2     I     3     I     4     I     5     I     6     I     7    I
   14,14,14,14,14,14, 6, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,10,14, // =
   14,14,14,14,14,14, 0, 7, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,11, 1,14,
   14,14,14,14,14,14, 0, 1,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14, 0, 1,14, // 0
   14,14,14,14,14,14, 0, 1,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14, 0, 1,14,
   14,14,14,14,14,14, 0, 1,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14, 0, 1,14, // =
   14,14,14,14,14,14, 0, 1,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14, 0, 1,14,
   14,14,14,14,14,14, 0, 9, 4, 4, 4,10,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14, 0, 1,14, // 1
   14,14,14,14,14,14, 8, 5, 5, 5,11, 1,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14, 0, 1,14,
    6, 4, 4, 4, 4, 4, 4, 4, 4, 4,13, 1,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14, 0, 1,14, // =
    0, 7, 5, 5, 5, 5, 5, 5, 5, 5, 5,12,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,17,18,14,
    0, 1,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14, 0, 1,14, // 2
    0, 1,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14, 0, 1,14,
    0, 1,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14, 0, 1,14, // =
    0, 1,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14, 0, 1,14,
   15,16,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14, 0, 1,14, // 3
    0, 1,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14, 0, 1,14,
    0, 1,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14, 0, 9,10, // =
    0, 1,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14, 8,11, 1,
    0, 1,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14, 0, 1, // 4
    0, 1,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14, 0, 1,
    0, 1,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14, 0, 1, // =
    0, 9, 4, 4, 4, 4, 4, 4, 4, 4,10,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14, 0, 1,
    8, 5, 5, 5, 5, 5, 5, 5, 5,11, 1,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14, 0, 1, // 5
   14,14,14,14,14,14,14,14,14, 0, 1,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14, 0, 1,
   14,14,14,14,14,14,14,14,14, 0, 1,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14, 0, 1, // =
   14,14,14,14,14,14,14,14,14, 0, 1,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14, 0, 1,
   14,14,14,14,14,14,14,14,14, 0, 1,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14, 0, 1, // 6
   14,14,14,14,14,14,14,14,14, 0, 1,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14, 0, 1,
   14,14,14,14,14,14,14,14,14, 0, 1,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14, 0, 1, // =
   14,14,14,14,14,14,14,14,14, 0, 1,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14, 0, 1,
   14,14,14,14,14,14,14,14,14, 0, 9, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,13, 1, // 7
   14,14,14,14,14,14,14,14,14, 8, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,12,
// I      0     I     1     I     2     I     3     I     4     I     5     I     6     I     7    I
};

// The whole track in sequence of big blocks (4x4 blockmap blocks)
const uint8_t trackTraceLine[trackTraceLineCount] =
{
    4*8 + 0, 5*8 + 0, 5*8 + 1, 5*8 + 2, 6*8 + 2, 7*8 + 2, 7*8 + 3, 7*8 + 4, 7*8 + 5, 7*8 + 6,
    7*8 + 7, 6*8 + 7, 5*8 + 7, 4*8 + 7, 3*8 + 7, 2*8 + 7, 1*8 + 7, 0*8 + 7, 0*8 + 6, 0*8 + 5,
    0*8 + 4, 0*8 + 3, 0*8 + 2, 0*8 + 1, 1*8 + 1, 1*8 + 2, 2*8 + 2, 2*8 + 1, 2*8 + 0, 3*8 + 0,
};

const uint8_t billboard_object_bitmaps_count = 25 + 3;

const uint8_t* billboard_object_bitmaps [billboard_object_bitmaps_count] = {
    image_ship_1_a+2,
    image_ship_1_b+2,
    image_ship_1_c+2,
    image_ship_1_d+2,
    image_ship_1_e+2,
    image_ship_2_a+2,
    image_ship_2_b+2,
    image_ship_2_c+2,
    image_ship_2_d+2,
    image_ship_2_e+2,
    image_ship_3_a+2,
    image_ship_3_b+2,
    image_ship_3_c+2,
    image_ship_3_d+2,
    image_ship_3_e+2,
    image_ship_4_a+2,
    image_ship_4_b+2,
    image_ship_4_c+2,
    image_ship_4_d+2,
    image_ship_4_e+2,
    image_ship_5_a+2,
    image_ship_5_b+2,
    image_ship_5_c+2,
    image_ship_5_d+2,
    image_ship_5_e+2,
    //image_ship6+2,
    //image_pokitto+2,
    image_cactus+2,
    image_rock1+2,
    image_spot+2,
};

const uint8_t terrain_bitmaps_count = 1*4;
const uint8_t* terrain_bitmaps [terrain_bitmaps_count] = {
//    image_terrain_1_a+2,
//    image_terrain_1_b+2,
//    image_terrain_1_c+2,
//    image_terrain_1_d+2,
//    image_terrain_4_a+2,
//    image_terrain_4_b+2,
//    image_terrain_4_c+2,
//    image_terrain_4_d+2,
//    image_terrain_2_a+2,
//    image_terrain_2_b+2,
//    image_terrain_2_c+2,
//    image_terrain_2_d+2,
//    image_terrain_5_a+2,
//    image_terrain_5_b+2,
//    image_terrain_5_c+2,
//    image_terrain_5_d+2,
//    image_terrain_3_a+2,
//    image_terrain_3_b+2,
//    image_terrain_3_c+2,
//    image_terrain_3_d+2,
    image_terrain_6_a+2,
    image_terrain_6_b+2,
    image_terrain_6_c+2,
    image_terrain_6_d+2,
//    image_terrain_7_a+2,
//    image_terrain_7_b+2,
//    image_terrain_7_c+2,
//    image_terrain_7_d+2,
//    image_terrain_8_a+2,
//    image_terrain_8_b+2,
//    image_terrain_8_c+2,
//    image_terrain_8_d+2,
};

//const uint8_t terrain_bitmaps_count = 8*4;
//const uint8_t* terrain_bitmaps [terrain_bitmaps_count] = {
//    image_terrain_1_a+2,
//    image_terrain_1_b+2,
//    image_terrain_1_c+2,
//    image_terrain_1_d+2,
//    image_terrain_2_a+2,
//    image_terrain_2_b+2,
//    image_terrain_2_c+2,
//    image_terrain_2_d+2,
//    image_terrain_3_a+2,
//    image_terrain_3_b+2,
//    image_terrain_3_c+2,
//    image_terrain_3_d+2,
//    image_terrain_4_a+2,
//    image_terrain_4_b+2,
//    image_terrain_4_c+2,
//    image_terrain_4_d+2,
//    image_terrain_5_a+2,
//    image_terrain_5_b+2,
//    image_terrain_5_c+2,
//    image_terrain_5_d+2,
//    image_terrain_6_a+2,
//    image_terrain_6_b+2,
//    image_terrain_6_c+2,
//    image_terrain_6_d+2,
//    image_terrain_7_a+2,
//    image_terrain_7_b+2,
//    image_terrain_7_c+2,
//    image_terrain_7_d+2,
//    image_terrain_8_a+2,
//    image_terrain_8_b+2,
//    image_terrain_8_c+2,
//    image_terrain_8_d+2,
//};

const uint8_t edge_bitmaps_count = 1*4;
const uint8_t* edge_bitmaps [edge_bitmaps_count] = {
    image_ball1_a+2,
    image_ball1_b+2,
    image_ball1_c+2,
    image_ball1_d+2,
//    image_ball2_a+2,
//    image_ball2_b+2,
//    image_ball2_c+2,
//    image_ball2_d+2,
//    image_ball3_a+2,
//    image_ball3_b+2,
//    image_ball3_c+2,
//    image_ball3_d+2,
//    image_ball4_a+2,
//    image_ball4_b+2,
//    image_ball4_c+2,
//    image_ball4_d+2,
//    image_ball5_a+2,
//    image_ball5_b+2,
//    image_ball5_c+2,
//    image_ball5_d+2,
};

const uint8_t current_texture_bitmaps_count = 16;
const uint8_t* current_texture_bitmaps[current_texture_bitmaps_count] = {
    NULL,
    image_ball1_a+2,        /*index: 1*/
    image_ball1_b+2,        /*index: 2*/
    image_ball1_c+2,        /*index: 3*/
    image_ball1_d+2,        /*index: 4*/
    image_road1+2,          /*index: 5*/
    image_road2+2,          /*index: 6*/
    image_terrain_6_a+2,    /*index: 7*/
    image_terrain_6_b+2,    /*index: 8*/
    image_terrain_6_c+2,    /*index: 9*/
    image_terrain_6_d+2,    /*index: 10*/
    image_start_a+2,        /*index: 11*/
    image_start_b+2,        /*index: 12*/
    image_start_c+2,        /*index: 13*/
    image_start_d+2,        /*index: 14*/
    image_light+2,          /*index: 15*/
};

const uint8_t* current_texture_bitmaps_mm1[current_texture_bitmaps_count] = {0};
const uint8_t* current_texture_bitmaps_mm2[current_texture_bitmaps_count] = {0};

// Billboard objects on track
// voiko nämä poistaa?
const fix16_t fxShipScaledSizeFactor = fix16_from_float(0.65);
const fix16_t fxCactusScaledSizeFactor = fix16_from_float(0.8);
const uint8_t* cactus_bm = billboard_object_bitmaps[25];
const int16_t cactusBmW  = *(cactus_bm - 2);
const int16_t cactusBmH  = *(cactus_bm - 1);
const fix16_t fxCactusScaledW  = cactusBmW * fxCactusScaledSizeFactor;
const fix16_t fxCactusScaledH  = cactusBmH * fxCactusScaledSizeFactor;
const uint8_t* stone_bm = billboard_object_bitmaps[26];
const int16_t stoneBmW  = *(stone_bm - 2);
const int16_t stoneBmH  = *(stone_bm - 1);
const fix16_t fxStoneScaledW  = fix16_from_int(stoneBmW);
const fix16_t fxStoneScaledH  = fix16_from_int(stoneBmH);
const uint8_t* ship_bm = billboard_object_bitmaps[1];
const int16_t shipBmW  = *(ship_bm - 2);
const int16_t shipBmH  = *(ship_bm - 1);
const fix16_t fxShipScaledW  = shipBmW * fxShipScaledSizeFactor;
const fix16_t fxShipScaledH  = shipBmH * fxShipScaledSizeFactor;
