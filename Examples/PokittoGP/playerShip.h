#pragma once

#include "main.h"
#include "ship.h"
#include "menu.h"

class CPlayerShip : public CShip, public CAnimValueCallback
{
public:
    CPlayerShip();
    virtual void Reset();
    virtual void Update();
    void HandleGameKeys();
    void HandleRaceStartingAndEnding( bool isOnStartingGrid );
    void FindActiveWaypoint();
    void CalculateRank();

    // Handle keys
    virtual void SetImpulse( fix16_t fxImpulseAngle )
    {
        m_bitmap = billboard_object_bitmaps[2];  // red car
        m_fxImpulseAcc = fix16_one<<2;
        m_fxImpulseAngle = fxImpulseAngle; // -(fix16_pi>>2);
    }

public: // From CAnimValueCallback

    // When this is called the CAnimValue is finished and will be used for other tasks. The caller should either
    // set a pointer to the CAnimValue object to NULL, or start another animation on the object.
    // The parameter can be used e.g. for chained animations to indicate the state.
    void Finished( int32_t par );

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
    //bool m_waypointsVisited[ 50 ];
    int8_t m_activeWaypointIndex;
    int8_t m_lastCheckedWPIndex;
    bool m_doRecalcRank;
    uint32_t m_activeWaypointFoundTimeInMs;
    CAnimValue* m_jumpAnimValue;
    CAnimValue* m_boosterAnimValue;
};
