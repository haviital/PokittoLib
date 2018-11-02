
#pragma once

#include "fix16.h"
#include "PokittoCookie.h"

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

const fix16_t fxMaxSpeedCollided = fix16_one>>1;
const fix16_t fxMaxSpeed = fix16_one*6;  // player max speed
const fix16_t fxDefaultOtherShipSpeed = fix16_one*10;
const fix16_t fxDefaultOtherShipSpeedInCorner =  fxMaxSpeedCollided * 12;
const fix16_t fxDefaultOtherShipSpeedInSlowCorner =  fxMaxSpeedCollided * 7;
//const fix16_t fxDefaultOtherShipSpeed = fix16_one;

//const fix16_t fxMaxSpeedCollided = fix16_one>>4;

enum LapTimingState {
    enumReadyToStart = 0,
    enumStarted = 1,
    enumOnTimedTrack = 2,
    enumOverHalfWayPoint = 3,
    enumFinished = 4,

};

//
class CObject3d
{
public:
    fix16_t m_fxX;
    fix16_t m_fxY;
    fix16_t m_fxScaledWidth;
    fix16_t m_fxScaledHeight;
    const uint8_t* m_bitmap;
    int16_t m_bitmapW;
    int16_t m_bitmapH;

    fix16_t m_fxXInView;
    fix16_t m_fxYInView;
    fix16_t m_fxDistancePot;
};

class CWaypoint
{
 public:
    int32_t x;
    int32_t y;
    fix16_t fxTargetSpeed;
};

class mycookie : public Pokitto::Cookie
{
public:
    uint32_t bestLap_ms = 0;
    uint32_t version = 1;
};

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
extern const CWaypoint waypoints[];
extern const uint32_t waypointCount;
extern const CObject3d g_timeTrialBilboardObjects[];

extern mycookie highscore;

//
uint8_t GetTileIndexCommon(int32_t posX, int32_t posY);
void ResetGame(bool isRace_);

//
void DrawLapTime(int32_t milliseconds, uint32_t x, uint32_t y, fix16_t fxScaleFactor);
void DrawBitmapOpaque8bit(int32_t posX, int32_t posY, const uint8_t* bitmapPtr, uint32_t bitmapW, uint32_t bitmapH );
void DrawBitmap8bit(int32_t posX, int32_t posY, const uint8_t* bitmapPtr, uint32_t bitmapW, uint32_t bitmapH );
void DrawScaledBitmap8bit(int32_t posX, int32_t posY, const uint8_t* bitmapPtr, uint32_t bitmapW, uint32_t bitmapH, uint32_t scaledW, uint32_t scaledH );


