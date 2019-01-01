
#pragma once

#include "fix16.h"
#include "PokittoCookie.h"

class CShip;
class CPlayerShip;

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
//const fix16_t fxDefaultOtherShipSpeed = (fix16_one*10)>>2;
//const fix16_t fxDefaultOtherShipSpeedInCorner =  (fxMaxSpeedCollided * 12)>>2;
//const fix16_t fxDefaultOtherShipSpeedInSlowCorner =  (fxMaxSpeedCollided * 7)>>2;

const fix16_t fxCameraBehindPlayerY = fix16_from_int(70);
const uint8_t trackTraceLineCount = 30;
//const fix16_t fxDefaultOtherShipSpeed = fix16_one;

//const fix16_t fxMaxSpeedCollided = fix16_one>>4;

const uint32_t g_BillboardObjectArrayMaxCount = 3*8;
const int32_t g_animValueArrayCount = 10;

extern uint32_t g_currentFrameTimeInMs;
class CAnimValue;
extern CAnimValue g_animValueArray[g_animValueArrayCount];

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

class CAnimValueCallback
{
public:

    // When this is called the CAnimValue is finished and will be used for other tasks. The caller should either
    // set a pointer to the CAnimValue object to NULL, or start another animation on the object.
    // The parameter can be used e.g. for chained animations to indicate the state.
    virtual void Finished( int32_t par ) = 0;
};

class CAnimValue
{
public:
    CAnimValue() { Reset(); }

    void Reset()
    {
        m_isActive = false;
        m_startTimeInMs = 0;
        m_endTimeInMs = 0;
        m_fxStartValue = 0;
        m_fxEndValue = 0;
        m_fxValue = 0;
        m_pfinishedCB = NULL;
        m_finishedCBParam = 0;
    }

    void Start( uint32_t durationInMs,  fix16_t fxStartValue, fix16_t fxEndValue, CAnimValueCallback* cb = NULL, int32_t callbackParam = 0)
    {
        m_isActive = true;
        m_startTimeInMs = g_currentFrameTimeInMs;
        m_endTimeInMs = m_startTimeInMs + durationInMs;
        m_fxStartValue = fxStartValue;
        m_fxEndValue = fxEndValue;
        m_fxValue = fxStartValue;
        m_pfinishedCB = cb;
        m_finishedCBParam  = callbackParam;
    }

    void Run()
    {
        if( g_currentFrameTimeInMs > m_endTimeInMs )
        {
            m_isActive = false;
            if( m_pfinishedCB )
                m_pfinishedCB->Finished( m_finishedCBParam );

            // Note: the m_isActive can be set to true in the Finished() callback if a new animation is started.
            if( ! m_isActive )
                Reset();
        }
        else
        {
            uint32_t deltaTimeInMs = m_endTimeInMs - m_startTimeInMs;
            uint32_t currentDeltaTimeInMs = g_currentFrameTimeInMs - m_startTimeInMs;
            fix16_t fxFactor = 0;
            if( deltaTimeInMs != 0 )
                fxFactor = fix16_div( currentDeltaTimeInMs, deltaTimeInMs );
            fix16_t fxDeltaValue = m_fxEndValue - m_fxStartValue;
            m_fxValue = m_fxStartValue + fix16_mul( fxFactor, fxDeltaValue );
        }
    }

    static CAnimValue* GetFreeElement()
    {
        for( int32_t i=0; i<g_animValueArrayCount; i++)
            if( g_animValueArray[i].m_isActive == false )
                return &( g_animValueArray[ i ] );
        return NULL;
    }

    static void RunAll()
    {
        for( int32_t i=0; i<g_animValueArrayCount; i++)
            if( g_animValueArray[i].m_isActive == true )
                g_animValueArray[ i ].Run();
    }

public:
    bool m_isActive;
    uint32_t m_startTimeInMs;
    uint32_t m_endTimeInMs;
    fix16_t m_fxStartValue;
    fix16_t m_fxEndValue;
    fix16_t m_fxValue; // Current value
    CAnimValueCallback* m_pfinishedCB;
    int32_t m_finishedCBParam;
};

// extern
extern Pokitto::Core mygame;
extern Pokitto::Sound snd;
extern fix16_t PerspectiveScaleY[];
extern fix16_t PerspectiveScaleX[];
extern uint8_t* blockMapRAM;
extern const uint8_t blockMapROM[];
extern uint8_t* blockMap;
extern const uint8_t blockData[][8*8];
extern const uint8_t trackTraceLine[trackTraceLineCount];
extern const uint8_t* current_texture_bitmaps[];
extern const uint8_t* current_texture_bitmaps_mm1[];
extern const uint8_t* current_texture_bitmaps_mm2[];
extern uint8_t* g_spriteBitmaps[];
extern const CWaypoint waypoints[];
extern const uint32_t waypointCount;
extern const CObject3d g_timeTrialBilboardObjects[];
extern CPlayerShip g_playerShip;
extern const uint8_t image_numbers[];
extern uint16_t g_gamePalette[256];
extern const uint16_t palette_pal[256];
extern int32_t g_billboardObjectInRamCount;
extern CObject3d g_BillboardObjectArray[g_BillboardObjectArrayMaxCount];

extern uint32_t g_frameNum;
extern bool g_isRace;
extern uint32_t g_shipCount;
extern CShip* g_ships[];

//
uint8_t GetTileIndexCommon(int32_t posX, int32_t posY);
void ResetGame(bool isRace_);
void SetupMusic(int32_t songNumber);

//
void DrawMode7(int32_t tile2PosX, int32_t tile2PosY, fix16_t fxAngle, fix16_t fxRotateCenterX, fix16_t fxRotateCenterY, fix16_t* perspectiveScaleXArr, fix16_t* perspectiveScaleYArr, uint16_t sceneryH2);
void DrawLapTime(int32_t milliseconds, uint32_t x, uint32_t y, fix16_t fxScaleFactor);
void DrawBitmapOpaque8bit(int32_t posX, int32_t posY, const uint8_t* bitmapPtr, uint32_t bitmapW, uint32_t bitmapH );
void DrawBitmap8bit(int32_t posX, int32_t posY, const uint8_t* bitmapPtr, uint32_t bitmapW, uint32_t bitmapH );
void DrawScaledBitmap8bit(int32_t posX, int32_t posY, const uint8_t* bitmapPtr, uint32_t bitmapW, uint32_t bitmapH, uint32_t scaledW, uint32_t scaledH );
void DrawRankNumber(int32_t x, int32_t y);
void SaveHighScore(uint32_t final_lap_time_ms);
void RestoreRomTextures();
