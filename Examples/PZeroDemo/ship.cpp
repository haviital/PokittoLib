#include "Pokitto.h"
#include "PokittoCookie.h"
#include "fix16.h"
#include "ship.h"

void CShip::Update()
{
    // *** Check collision to road edges
    bool collided = false;
    uint8_t tileIndex = GetTileIndexCommon(fix16_to_int(m_fxX), fix16_to_int(m_fxY));
    if(
        tileIndex != 5 && tileIndex != 6 &&
        (tileIndex < 11 || tileIndex > 15)
    )
    {
        collided = true;
    }

    // *** Follow the waypoint

    // Direction vector to the current waypoint.
    fix16_t fxDirX = fix16_from_int(waypoints[m_activeWaypointIndex].x) - m_fxX;
    fix16_t fxDirY = fix16_from_int(waypoints[m_activeWaypointIndex].y) - m_fxY;

    // Calculate distance.
    // Scale down so that it will not overflow
    fix16_t fxX3 = fxDirX;
    fix16_t fxY3 = fxDirY;
    fxX3 >>= 4;
    fxY3 >>= 4;
    fix16_t fxDistanceToWaypoint = fix16_mul(fxX3, fxX3) + fix16_mul(fxY3,fxY3);
    const fix16_t fxLimit = fix16_from_int(20*20);
    if( fxDistanceToWaypoint < fxLimit>>5 )
    {
        // Ship is inside the waypoint radious. Stop following it and go towards the next waypoint.

        // Next waypoint.
        int32_t i = m_activeWaypointIndex;
        if(++i >= waypointCount)
            i = 0;
        m_activeWaypointIndex = i;
        //fxLastDistanceToWaypoint = fxDistanceToWaypoint;

        // Direction vector to the current waypoint.
        fxDirX = fix16_from_int(waypoints[m_activeWaypointIndex].x) - m_fxX;
        fxDirY = fix16_from_int(waypoints[m_activeWaypointIndex].y) - m_fxY;
    }

    // Calculate angle to the current waypoint

    fix16_t fxAngleToWayPoint = fix16_atan2(-fxDirX, fxDirY);
    fix16_t fxAngleDiff = fxAngleToWayPoint - m_fxAngle;
    if(fxAngleDiff > fix16_pi || fxAngleDiff < -fix16_pi)
        fxAngleDiff -= (2*fix16_pi);
    if( fxAngleDiff < 0 )
    {
        // Rotate certain amount
        if(fxAngleDiff < -m_fxRotVel )
            m_fxAngle -= m_fxRotVel;
        else
            m_fxAngle += fxAngleDiff;
    }
    else
    {
        // Rotate certain amount
        if(fxAngleDiff > m_fxRotVel )
            m_fxAngle += m_fxRotVel;
        else
            m_fxAngle += fxAngleDiff;
    }


    // *** Move

    // If colliding, slow down
    if( collided ) {

        // Break or stop
        if(m_fxVel > fxMaxSpeedCollided)
        {
            m_fxVel -= (fix16_one>>4);
            if(m_fxVel < 0)
                m_fxVel = 0;
        }
        else if(m_fxVel < -fxMaxSpeedCollided)
        {
            m_fxVel += (fix16_one>>4);
            if(m_fxVel > 0)
                m_fxVel = 0;
        }
    }
    else
    {
        // Get target speed
        fix16_t fxTargetSpeed = waypoints[m_activeWaypointIndex].fxTargetSpeed;
        if( (fxAngleDiff > (fix16_pi>>2) ) || (fxAngleDiff < -(fix16_pi>>2) ) )
            fxTargetSpeed = fxDefaultOtherShipSpeedInCorner;

        // Calculate speed to the current waypoint
        if( m_fxVel > fxTargetSpeed )
            m_fxVel -= m_fxAcc<<4;  // Break faster than accelerate
        else
            m_fxVel += m_fxAcc;
    }




    // Limit speed
    if(m_fxVel > m_fxMaxSpeed)
        m_fxVel = m_fxMaxSpeed;
    else if(m_fxVel < fxDefaultOtherShipSpeedInCorner)
        m_fxVel = fxDefaultOtherShipSpeedInCorner;

    fix16_t fxCos = fix16_cos(-m_fxAngle);
    fix16_t fxSin = fix16_sin(-m_fxAngle);

    m_fxY += fix16_mul(m_fxVel, fxCos);
    m_fxX += fix16_mul(m_fxVel, fxSin);
}
