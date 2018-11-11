#pragma once

#include "main.h"

class CShip : public CObject3d
{
public:
    CShip();
    virtual void Update();
    virtual void Reset();
    virtual void UpdateTrackPos();

public:
    fix16_t m_fxVel;
    fix16_t m_fxAcc;
    fix16_t m_fxDeacc;
    fix16_t m_fxAngle;
    fix16_t m_fxRotVel;
    fix16_t m_fxMaxSpeed;
    fix16_t m_fxCornerSpeed1;
    fix16_t m_fxCornerSpeed2;
    int32_t m_activeWaypointIndex;
    fix16_t m_fxWaypointTargetSpeed;
    //fix16_t m_fxLastDistanceToWaypoint;
    int32_t m_activeLapNum;
    bool m_isPlayer;
    int32_t m_trackIndex;
    LapTimingState m_lapTimingState;
};
