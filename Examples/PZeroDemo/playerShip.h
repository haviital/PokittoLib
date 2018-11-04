#pragma once

#include "main.h"
#include "ship.h"

class CPlayerShip : public CShip
{
public:
    CPlayerShip();
    void Reset();
    virtual void Update();
    void HandleGameKeys();

public:
    LapTimingState m_lapTimingState;
    uint32_t m_final_lap_time_ms;
    uint32_t m_start_ms;
    bool m_isCollidedToPlayerShip;
    bool m_isCollidedToSurface;
    int m_tonefreq;
    bool m_isTurningLeft;
    bool m_isTurningRight;
    fix16_t m_fxCameraBehindPlayerTarget;
    fix16_t m_fxCameraBehindPlayerCurrent;
};
