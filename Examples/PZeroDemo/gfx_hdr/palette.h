

/*
 * BMP image as 8bpp (up to 256 color index) data
 */

#include <stdint.h>

const uint16_t palette_pal[] = {
63519,0,23275,15186,10731,1984,65344,64479,40159,65503,1728,56896,56155,31707,57051,1472,48448,47831,23255,48599,23243,59391,1216,40000,39507,14803,40147,13040,12975,35881,61120,56896,52672,42322,44567,46336,44224,44224,35953,13926,8650,9476,52480,63072,33840,35857,25932,15274,23500,65535,19658,12874,11494,19653,50307,52579,16007,11557,54560,65152,21162,38066,57083,46518,59164,19049,57051,21130,46486,23243,7619,54819,13542,44326,11750,16008,15720,19945,23916,23819,58551,56407,36183,38295,54263,31991,52087,27831,49911,23639,49815,19479,47575,13239,48607,44383,38079,31775,25471,19071,42266,44410,46522,50682,52826,54938,52831,57050,58519,56439,56375,54295,32023,34071,34103,36183,54295,54231,31959,56439,56375,54231,54167,27831,29911,32023,34103,52119,29911,36183,52087,49943,23639,25719,29879,31991,40191,52794,57018,56343,49815,19447,21559,25719,29911,48671,48639,57018,59194,44415,38111,31743,25439,44378,46458,48602,50714,44447,40223,33887,29631,46458,46522,50682,52794,46527,42335,38111,35967,48602,50714,54938,46527,44415,42303,52858,54938,57050,59130,48635,46523,46491,44443,44407,44375,42299,40155,40183,38075,31771,29627,40179,4190,35921,48601,40185,27501,29584,12632,25392,31695,21168,27501,29616,48631,31736,40179,38066,36313,63390,40539,61277,44765,48991,38034,35921,33808,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

const uint8_t palette[] = {
217,1,
0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F,0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F,0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5A,0x5B,0x5C,0x5D,0x5E,0x5F,0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7A,0x7B,0x7C,0x7D,0x7E,0x7F,0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F,0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF,0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,
};