

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

const uint16_t sceneryH = 16;
const uint16_t screenShiftY = 5;

const int32_t g_rotatingCenterX = -6;
const int32_t g_rotatingCenterY = 72;

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

//
void DrawLapTime(int32_t milliseconds, uint32_t x, uint32_t y, fix16_t fxScaleFactor);
void DrawBitmapOpaque8bit(int32_t posX, int32_t posY, const uint8_t* bitmapPtr, uint32_t bitmapW, uint32_t bitmapH );
void DrawBitmap8bit(int32_t posX, int32_t posY, const uint8_t* bitmapPtr, uint32_t bitmapW, uint32_t bitmapH );
void DrawScaledBitmap8bit(int32_t posX, int32_t posY, const uint8_t* bitmapPtr, uint32_t bitmapW, uint32_t bitmapH, uint32_t scaledW, uint32_t scaledH );


