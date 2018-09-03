/**************************************************************************/
/*!
    @file     GameData.h
    @author   Hannu Viitala

*/
/**************************************************************************/

#include "Pokitto.h"
#include "fix16.h"

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
#include "image_ship6.h"
#include "image_ball1_a.h"
#include "image_ball1_b.h"
#include "image_ball1_c.h"
#include "image_ball1_d.h"
#include "image_ball2_a.h"
#include "image_ball2_b.h"
#include "image_ball2_c.h"
#include "image_ball2_d.h"
#include "image_ball3_a.h"
#include "image_ball3_b.h"
#include "image_ball3_c.h"
#include "image_ball3_d.h"
#include "image_ball4_a.h"
#include "image_ball4_b.h"
#include "image_ball4_c.h"
#include "image_ball4_d.h"
#include "image_ball5_a.h"
#include "image_ball5_b.h"
#include "image_ball5_c.h"
#include "image_ball5_d.h"
#include "image_terrain_1_a.h"
#include "image_terrain_1_b.h"
#include "image_terrain_1_c.h"
#include "image_terrain_1_d.h"
#include "image_terrain_2_a.h"
#include "image_terrain_2_b.h"
#include "image_terrain_2_c.h"
#include "image_terrain_2_d.h"
#include "image_terrain_3_a.h"
#include "image_terrain_3_b.h"
#include "image_terrain_3_c.h"
#include "image_terrain_3_d.h"
#include "image_terrain_4_a.h"
#include "image_terrain_4_b.h"
#include "image_terrain_4_c.h"
#include "image_terrain_4_d.h"
#include "image_terrain_5_a.h"
#include "image_terrain_5_b.h"
#include "image_terrain_5_c.h"
#include "image_terrain_5_d.h"
#include "image_terrain_6_a.h"
#include "image_terrain_6_b.h"
#include "image_terrain_6_c.h"
#include "image_terrain_6_d.h"
#include "image_terrain_7_a.h"
#include "image_terrain_7_b.h"
#include "image_terrain_7_c.h"
#include "image_terrain_7_d.h"
#include "image_terrain_8_a.h"
#include "image_terrain_8_b.h"
#include "image_terrain_8_c.h"
#include "image_terrain_8_d.h"
#include "image_road1.h"
#include "image_road2.h"
#include "image_sky.h"


 // Defines blocks of 64 (8x8) tiles
 const uint8_t blockData[][8*8] = {

    // block: 0
    {
        // 8x8 tile indexes.Left edge of the road
        7, 1, 2, 5, 5, 5, 5, 5,
        9, 3, 4, 5, 5, 5, 5, 5,
        7, 1, 2, 6, 6, 6, 6, 6,
        9, 3, 4, 6, 6, 6, 6, 6,
        7, 1, 2, 5, 5, 5, 5, 5,
        9, 3, 4, 5, 5, 5, 5, 5,
        7, 1, 2, 6, 6, 6, 6, 6,
        9, 3, 4, 6, 6, 6, 6, 6,
     },
    // block: 1
    {
        // 8x8 tile indexes. Right edge of the road
        5, 5, 5, 5, 5, 1, 2, 8,
        5, 5, 5, 5, 5, 3, 4,10,
        6, 6, 6, 6, 6, 1, 2, 8,
        6, 6, 6, 6, 6, 3, 4,10,
        5, 5, 5, 5, 5, 1, 2, 8,
        5, 5, 5, 5, 5, 3, 4,10,
        6, 6, 6, 6, 6, 1, 2, 8,
        6, 6, 6, 6, 6, 3, 4,10,
    },
    // block: 2. Surface
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
    // block: 3. Road
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
        1, 2, 1, 2, 1, 2, 1, 2,
        3, 4, 3, 4, 3, 4, 3, 4,
        5, 5, 6, 6, 5, 5, 6, 6,
        5, 5, 6, 6, 5, 5, 6, 6,
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
        5, 5, 6, 6, 5, 5, 6, 6,
        5, 5, 6, 6, 5, 5, 6, 6,
        1, 2, 1, 2, 1, 2, 1, 2,
        3, 4, 3, 4, 3, 4, 3, 4,
        9,10, 9,10, 9,10, 9,10,
    },
    // block: 6. TL outer corner.
    {
        // 8x8 tile indexes.
        7, 8, 7, 8, 7, 8, 7, 8,
        9,10, 9,10, 1, 2, 1, 2,
        7, 8, 1, 2, 3, 4, 3, 4,
        9,10, 3, 4, 6, 6, 6, 6,
        7, 1, 2, 6, 6, 6, 6, 6,
        9, 3, 4, 6, 6, 6, 6, 6,
        7, 1, 2, 6, 6, 6, 6, 6,
        9, 3, 4, 6, 6, 6, 6, 6,
     },
    // block: 7. TL inner corner.
    {
        // 8x8 tile indexes.
        6, 6, 6, 6, 6, 6, 6, 6,
        6, 6, 6, 6, 6, 6, 6, 6,
        6, 6, 6, 6, 6, 6, 6, 6,
        6, 6, 6, 6, 6, 6, 6, 6,
        6, 6, 6, 6, 6, 6, 6, 6,
        6, 6, 6, 6, 6, 6, 6, 6,
        6, 6, 6, 6, 6, 6, 1, 2,
        6, 6, 6, 6, 6, 6, 3, 4,
     },
    // block: 8. BL outer corner.
    {
        // 8x8 tile indexes.
        7, 1, 2, 6, 6, 6, 6, 6,
        9, 3, 4, 6, 6, 6, 6, 6,
        7, 1, 2, 6, 6, 6, 6, 6,
        9, 3, 4, 6, 6, 6, 6, 6,
        7, 8, 1, 2, 6, 6, 6, 6,
        9,10, 3, 4, 1, 2, 1, 2,
        7, 8, 7, 8, 3, 4, 3, 4,
        9,10, 9,10, 9,10, 9,10,
     },
    // block: 9. BL inner corner.
    {
        // 8x8 tile indexes.
        6, 6, 6, 6, 6, 6, 1, 2,
        6, 6, 6, 6, 6, 6, 3, 4,
        6, 6, 6, 6, 6, 6, 6, 6,
        6, 6, 6, 6, 6, 6, 6, 6,
        6, 6, 6, 6, 6, 6, 6, 6,
        6, 6, 6, 6, 6, 6, 6, 6,
        6, 6, 6, 6, 6, 6, 6, 6,
        6, 6, 6, 6, 6, 6, 6, 6,
     },
    // block: 10. TR outer corner.
    {
        // 8x8 tile indexes.
        7, 8, 7, 8, 7, 8, 7, 8,
        1, 2, 1, 2, 9,10, 9,10,
        3, 4, 3, 4, 1, 2, 7, 8,
        6, 6, 6, 6, 3, 4, 9,10,
        6, 6, 6, 6, 6, 1, 2, 8,
        6, 6, 6, 6, 6, 3, 4,10,
        6, 6, 6, 6, 6, 1, 2, 8,
        6, 6, 6, 6, 6, 3, 4,10,
     },
    // block: 11. TR inner corner.
    {
        // 8x8 tile indexes.
        6, 6, 6, 6, 6, 6, 6, 6,
        6, 6, 6, 6, 6, 6, 6, 6,
        6, 6, 6, 6, 6, 6, 6, 6,
        6, 6, 6, 6, 6, 6, 6, 6,
        6, 6, 6, 6, 6, 6, 6, 6,
        6, 6, 6, 6, 6, 6, 6, 6,
        1, 2, 6, 6, 6, 6, 6, 6,
        3, 4, 6, 6, 6, 6, 6, 6,
     },
    // block: 12. TR outer corner.
    {
        // 8x8 tile indexes.
        6, 6, 6, 6, 6, 3, 4, 8,
        6, 6, 6, 6, 6, 1, 2,10,
        6, 6, 6, 6, 6, 3, 4, 8,
        6, 6, 6, 6, 6, 1, 2,10,
        6, 6, 6, 6, 3, 4, 7, 8,
        3, 4, 3, 4, 1, 2, 9,10,
        1, 2, 1, 2, 7, 8, 7, 8,
        9,10, 9,10, 9,10, 9,10,
     },
    // block: 13. TR inner corner.
    {
        // 8x8 tile indexes.
        1, 2, 6, 6, 6, 6, 6, 6,
        3, 4, 6, 6, 6, 6, 6, 6,
        6, 6, 6, 6, 6, 6, 6, 6,
        6, 6, 6, 6, 6, 6, 6, 6,
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
};

// Map of blocks. Defines the whole game field!
const uint8_t blockMap[mapWidth*mapHeight] = {
   14,14,14,14,14,14, 6, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,10,14,
   14,14,14,14,14,14, 0, 7, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,11, 1,14,
   14,14,14,14,14,14, 0, 1,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14, 0, 1,14,
   14,14,14,14,14,14, 0, 1,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14, 0, 1,14,
   14,14,14,14,14,14, 0, 1,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14, 0, 1,14,
   14,14,14,14,14,14, 0, 1,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14, 0, 1,14,
   14,14,14,14,14,14, 0, 9, 4, 4, 4,10,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14, 0, 1,14,
   14,14,14,14,14,14, 8, 5, 5, 5,11, 1,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14, 0, 1,14,
    6, 4, 4, 4, 4, 4, 4, 4, 4, 4,13, 1,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14, 0, 1,14,
    0, 7, 5, 5, 5, 5, 5, 5, 5, 5, 5,12,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14, 0, 1,14,
    0, 1,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14, 0, 1,14,
    0, 1,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14, 0, 1,14,
    0, 1,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14, 0, 1,14,
    0, 1,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14, 0, 1,14,
    0, 1,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14, 0, 1,14,
    0, 1,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14, 0, 1,14,
    0, 1,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14, 0, 9,10,
    0, 1,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14, 8,11, 1,
    0, 1,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14, 0, 1,
    0, 1,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14, 0, 1,
    0, 1,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14, 0, 1,
    0, 9, 4, 4, 4, 4, 4, 4, 4, 4,10,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14, 0, 1,
    8, 5, 5, 5, 5, 5, 5, 5, 5,11, 1,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14, 0, 1,
   14,14,14,14,14,14,14,14,14, 0, 1,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14, 0, 1,
   14,14,14,14,14,14,14,14,14, 0, 1,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14, 0, 1,
   14,14,14,14,14,14,14,14,14, 0, 1,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14, 0, 1,
   14,14,14,14,14,14,14,14,14, 0, 1,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14, 0, 1,
   14,14,14,14,14,14,14,14,14, 0, 1,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14, 0, 1,
   14,14,14,14,14,14,14,14,14, 0, 1,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14, 0, 1,
   14,14,14,14,14,14,14,14,14, 0, 1,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14, 0, 1,
   14,14,14,14,14,14,14,14,14, 0, 9, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,13, 1,
   14,14,14,14,14,14,14,14,14, 8, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,12,
};

const uint8_t ship_bitmaps_count = 26;
const uint8_t* ship_bitmaps [ship_bitmaps_count] = {
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
    image_ship6+2,
};

const uint8_t terrain_bitmaps_count = 8*4;
const uint8_t* terrain_bitmaps [terrain_bitmaps_count] = {
    image_terrain_1_a+2,
    image_terrain_1_b+2,
    image_terrain_1_c+2,
    image_terrain_1_d+2,
    image_terrain_4_a+2,
    image_terrain_4_b+2,
    image_terrain_4_c+2,
    image_terrain_4_d+2,
    image_terrain_2_a+2,
    image_terrain_2_b+2,
    image_terrain_2_c+2,
    image_terrain_2_d+2,
    image_terrain_5_a+2,
    image_terrain_5_b+2,
    image_terrain_5_c+2,
    image_terrain_5_d+2,
    image_terrain_3_a+2,
    image_terrain_3_b+2,
    image_terrain_3_c+2,
    image_terrain_3_d+2,
    image_terrain_6_a+2,
    image_terrain_6_b+2,
    image_terrain_6_c+2,
    image_terrain_6_d+2,
    image_terrain_7_a+2,
    image_terrain_7_b+2,
    image_terrain_7_c+2,
    image_terrain_7_d+2,
    image_terrain_8_a+2,
    image_terrain_8_b+2,
    image_terrain_8_c+2,
    image_terrain_8_d+2,
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

const uint8_t all_texture_bitmaps_count = 11;
const uint8_t* all_texture_bitmaps[all_texture_bitmaps_count] = {
    image_terrain_1_a+2,
    image_ball1_a+2,
    image_ball1_b+2,
    image_ball1_c+2,
    image_ball1_d+2,
    image_road1+2,
    image_road2+2,
    image_terrain_6_a+2,
    image_terrain_6_b+2,
    image_terrain_6_c+2,
    image_terrain_6_d+2,
};


