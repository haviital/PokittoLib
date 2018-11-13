#pragma once

#include "main.h"
#include "ship.h"
#include "menu.h"

class CPlayerShip : public CShip
{
public:
    CPlayerShip();
    virtual void Reset();
    virtual void Update();
    void HandleGameKeys();
    void CalculateRank();

public:
    uint32_t m_final_lap_time_ms;
    uint32_t m_start_ms;
    bool m_isCollidedToPlayerShip;
    bool m_isCollided;
    int m_tonefreq;
    bool m_isTurningLeft;
    bool m_isTurningRight;
    fix16_t m_fxCameraBehindPlayerTarget;
    fix16_t m_fxCameraBehindPlayerCurrent;
    int32_t m_currentRank;
    uint32_t m_current_lap_time_ms;
    CMenu::MenuMode m_requestedMenuMode;
};
