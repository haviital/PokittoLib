#pragma once

class CMenu
{
public:
    enum MenuMode {
        enumNoMenu = 0,
        enumStartupEmptyMenu = 1,
        enumMainMenu = 2,
        enumContinueMenu = 3,
        enumTimeTrialFinishedMenu = 4,
        enumRaceFinishedMenu = 5,
        enumPilotPictureMenu = 6,
    };

public:
    CMenu();
    void HandleMenus(bool isRace_, uint32_t bestLap_ms, MenuMode requestedMenuMode );
    bool HandleGenericMenu( uint32_t bestLap_ms, int32_t& /*in out */ cursorPos, char* item1, char* item2, char* item3, char* item4);
    bool HandlePilotPictureMenu();

public:
    bool m_isOpen;
    MenuMode m_mode = enumMainMenu;
    int32_t m_cursorPos;
    bool m_pressedAkeyDownOutsideMenu;
    int32_t m_pilotPicturePage;
};
