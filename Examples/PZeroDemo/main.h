

// Constants

// Constants
const int32_t screenW = 110;
const int32_t screenH = 88;
const uint8_t mapWidth = 32;
const uint8_t mapHeight = 32;

const uint8_t texW = 8;
const uint8_t texH = 12;
const uint8_t tileW = 8;
const uint8_t tileH = 8;

// extern
extern Pokitto::Core mygame;
extern Pokitto::Sound snd;
extern fix16_t PerspectiveScaleY[];
extern fix16_t PerspectiveScaleX[];
extern const uint8_t blockMap[];
extern const uint8_t blockData[][8*8];
extern const uint8_t* current_texture_bitmaps[];
extern const uint8_t* current_texture_bitmaps_mm1[];
extern const uint8_t* current_texture_bitmaps_mm2[];
