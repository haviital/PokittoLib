#pragma once

#include "fix16.h"


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
