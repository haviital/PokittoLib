#pragma once

#include "fix16.h"

class CAnimBitmap : public CAnimValueCallback
{
public:

    CAnimBitmap() :
        m_bitmap( NULL ),
        m_animValue( NULL ),
        m_x1( 0 ),
        m_y1( 0 ),
        m_x2( 0 ),
        m_y2( 0 ),
        m_pfinishedCB( NULL )
    {}

    void Start( uint32_t durationInMs, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint8_t* bitmap, CAnimValueCallback* cb = NULL, int32_t callbackParam = 0 )
    {

        m_pfinishedCB = cb;

        if(m_animValue)
            m_animValue->Reset();
        else
            m_animValue = CAnimValue::GetFreeElement();

        if( m_animValue )
            m_animValue->Start( durationInMs,  fix16_from_int( 0 ), fix16_from_int( 1 ), this, callbackParam );
        m_bitmap = bitmap;
        m_x1 = x1;
        m_y1 = y1;
        m_x2 = x2;
        m_y2 = y2;
    }

    void Draw()
    {
        if( m_animValue )
        {
            fix16_t fxFactor = m_animValue->m_fxValue;
            int32_t relX = fix16_to_int( fxFactor * (m_x2 - m_x1) );
            int32_t currX = m_x1 + relX;

            int32_t relY = fix16_to_int( fxFactor * (m_y2 - m_y1) );
            int32_t currY = m_y1 + relY;

            // Draw
            DrawBitmap8bit( currX, currY, &(m_bitmap[2]), m_bitmap[0], m_bitmap[1] );
        }
    }

    // When this is called the CAnimValue is finished and will be used for other tasks. The caller should either
    // set a pointer to the CAnimValue object to NULL, or start another animation on the object.
    // The parameter can be used e.g. for chained animations to indicate the state.
    void Finished( int32_t par )
    {
        if( m_pfinishedCB )
            m_pfinishedCB->Finished( par );

        if( !m_animValue->m_isActive )
            m_animValue = NULL; // Must be done!
    }

public:
    uint8_t* m_bitmap;
    CAnimValue* m_animValue;
    int32_t m_x1;
    int32_t m_y1;
    int32_t m_x2;
    int32_t m_y2;
    CAnimValueCallback* m_pfinishedCB;
};


class CMenu : public CAnimValueCallback
{
public:
    enum MenuMode
    {
        enumNoMenu = 0,
        enumStartupEmptyMenu = 1,
        enumMainMenu = 2,
        enumContinueMenu = 3,
        enumTimeTrialFinishedMenu = 4,
        enumRaceFinishedMenu = 5,
        enumPilotPictureMenu = 6,
        enumSelectTrackMenu = 7,
    };

    enum class ButtonAnimState
    {
        enumNone = 0,
        enumShow = 1,
        enumDisappear = 2,
    };

public:
    CMenu();
    void HandleMenus(bool isRace_, uint32_t bestLap_ms, MenuMode requestedMenuMode );
    bool HandleGenericMenu( uint32_t bestLap_ms, int32_t& /*in out */ cursorPos, char* item1, char* item2, char* item3, char* item4);
    bool HandlePilotPictureMenu();
    bool HandleSelectTrackMenu();
    bool ReadAndValidateTrack(
        char* trackPath, char* trackFileName,
        /*OUT*/char* myTrack2, /*OUT*/char* trackName, /*OUT*/char* authorName );
    bool ReadAndValidateTextures(char* trackDirPath, char* trackDirName);

    // When this is called the CAnimValue is finished and will be used for other tasks. The caller should either
    // set a pointer to the CAnimValue object to NULL, or start another animation on the object.
    // The parameter can be used e.g. for chained animations to indicate the state.
    void Finished( int32_t par );

public:
    static const int32_t dirPathAndNameArrMaxLen = 10;
    static const int32_t dirPathAndNameMaxLen = 30;

public:
    bool m_isOpen;
    MenuMode m_mode = enumMainMenu;
    int32_t m_cursorPos;
    bool m_pressedAkeyDownOutsideMenu;
    int32_t m_pilotPicturePage;
    bool m_hasTrackBeenLoaded;
    fix16_t m_perspectiveScaleX[screenH];
    fix16_t m_perspectiveScaleY[screenH];
    fix16_t m_fxCamAngle;
    fix16_t m_fxScaleFactor;
    bool m_isFullScreenView;
    int32_t m_previewX;
    int32_t m_previewY;
    int32_t m_previewPhase;
    uint16_t m_sceneryH;
    int32_t m_trackNum;
    int32_t m_trackCount;
    bool m_isTrackOk;
    char m_dirNameArr[dirPathAndNameArrMaxLen][dirPathAndNameMaxLen];
    int32_t m_test;
    CAnimBitmap m_animUpButtonHint;
    CAnimBitmap m_animDownButtonHint;
};
