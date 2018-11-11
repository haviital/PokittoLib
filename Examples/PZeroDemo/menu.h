#pragma once

class CMenu
{
public:
    enum MenuMode {
        enumNoMenu = 0,
        enumMainMenu = 1,
        enumContinueMenu = 2,
        enumTimeTrialFinishedMenu = 3,
        enumRaceFinishedMenu = 4,
        enumPilotPictureMenu = 5,
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
};
