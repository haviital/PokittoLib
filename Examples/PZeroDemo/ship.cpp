#include "Pokitto.h"
#include "PokittoCookie.h"
#include "fix16.h"
#include "ship.h"

void CShip::Update()
{
    // Direction vector to the current waypoint.
    fix16_t fxDirX = fix16_from_int(waypoints[m_activeWaypointIndex][0]) - m_fxX;
    fix16_t fxDirY = fix16_from_int(waypoints[m_activeWaypointIndex][1]) - m_fxY;

    // Calculate distance.
    // Scale down so that it will not overflow
    fix16_t fxX3 = fxDirX;
    fix16_t fxY3 = fxDirY;
    fxX3 >>= 4;
    fxY3 >>= 4;
    fix16_t fxDistanceToWaypoint = fix16_mul(fxX3, fxX3) + fix16_mul(fxY3,fxY3);
    const fix16_t fxLimit = fix16_from_int(20*20);
    if( fxDistanceToWaypoint < fxLimit>>8 )
    {
        // Ship is inside the waypoint radious. Stop following it and go towards the next waypoint.

        // Next waypoint.
        int32_t i = m_activeWaypointIndex;
        if(i++ >= waypointCount)
            i = 0;
        m_activeWaypointIndex = i;
        //fxLastDistanceToWaypoint = fxDistanceToWaypoint;

        // Direction vector to the current waypoint.
        fxDirX = fix16_from_int(waypoints[m_activeWaypointIndex][0]) - m_fxX;
        fxDirY = fix16_from_int(waypoints[m_activeWaypointIndex][1]) - m_fxY;
    }

    // Calculate angle to the current waypoint
    m_fxAngle = fix16_atan2(-fxDirX, fxDirY);

    // Limit speed
    if(m_fxVel > m_fxMaxSpeed)
        m_fxVel = m_fxMaxSpeed;
    else if(m_fxVel < -m_fxMaxSpeed)
        m_fxVel = -m_fxMaxSpeed;

    fix16_t fxCos = fix16_cos(-m_fxAngle);
    fix16_t fxSin = fix16_sin(-m_fxAngle);

    m_fxY += fix16_mul(m_fxVel, fxCos);
    m_fxX += fix16_mul(m_fxVel, fxSin);
}
