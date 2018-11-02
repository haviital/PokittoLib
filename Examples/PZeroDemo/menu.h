#pragma once

class CMenu
{
public:
    enum MenuMode {
        enumMainMenu = 0,
        enumContinueMenu = 1,
        enumTimeTrialFinishedMenu = 2,
    };

public:
    CMenu();
    void HandleMenus(bool isRace_, uint32_t bestLap_ms );
    bool HandleGenericMenu( uint32_t bestLap_ms, int32_t& /*in out */ cursorPos, char* item1, char* item2, char* item3, char* item4);

public:
    bool m_isOpen;
    MenuMode m_mode = enumMainMenu;
    int32_t m_cursorPos;
};
