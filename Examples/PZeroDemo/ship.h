#pragma once

#include "main.h"

class CShip : public CObject3d
{
public:
    void Update();

public:
    fix16_t m_fxVel;
    fix16_t m_fxAcc;
    fix16_t m_fxAngle;
    fix16_t m_fxRotVel;
    fix16_t m_fxMaxSpeed;
    int32_t m_activeWaypointIndex;
    //fix16_t m_fxLastDistanceToWaypoint;
};
