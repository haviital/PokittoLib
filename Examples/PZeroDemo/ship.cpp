#include "Pokitto.h"
#include "PokittoCookie.h"
#include "fix16.h"
#include "ship.h"

CShip::CShip() :
    m_isPlayer(false),
    m_activeLapNum(0),
    m_trackIndex(0),
    m_lapTimingState(enumReadyToStart)
{
}

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

    // Handle lap starting and ending detection.
    bool isOnStartingGrid = ( tileIndex >= 11 && tileIndex <= 14);
    bool isOnHalfWayPoint = (tileIndex == 15);
    switch(m_lapTimingState)
    {
    case enumReadyToStart:
        if( isOnStartingGrid )
        {
            m_lapTimingState = enumStarted;
        }
        break;
    case enumStarted:
        if( ! isOnStartingGrid )
        {
            m_lapTimingState = enumOnTimedTrack;
            //m_lapTimingState = enumOverHalfWayPoint;
        }
        break;
    case enumOnTimedTrack:
        if( isOnHalfWayPoint )
        {
            m_lapTimingState = enumOverHalfWayPoint;
         }
        break;
    case enumOverHalfWayPoint:
        if( isOnStartingGrid )
        {
            // Finished!
            m_lapTimingState = enumStarted;
            m_activeLapNum++;
        }
        break;
    case enumFinished:
        break;
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
    if( fxDistanceToWaypoint < fxLimit>>7 )
    {
        // Ship is inside the waypoint radious. Stop following it and go towards the next waypoint.

        // Next waypoint.
        int32_t i = m_activeWaypointIndex;
        if(++i >= waypointCount)
        {
            i = 0;
        }
        m_activeWaypointIndex = i;
        //fxLastDistanceToWaypoint = fxDistanceToWaypoint;

        // Direction vector to the current waypoint.
        fxDirX = fix16_from_int(waypoints[m_activeWaypointIndex].x) - m_fxX;
        fxDirY = fix16_from_int(waypoints[m_activeWaypointIndex].y) - m_fxY;

        fix16_t m_fxWaypointTargetSpeed = 0;
        switch(waypoints[m_activeWaypointIndex].fxTargetSpeed)
        {
        case fxDefaultOtherShipSpeed:
            m_fxWaypointTargetSpeed = m_fxMaxSpeed;
            break;
        case fxDefaultOtherShipSpeedInCorner:
            m_fxWaypointTargetSpeed = m_fxCornerSpeed1;
            break;
        case fxDefaultOtherShipSpeedInSlowCorner:
            m_fxWaypointTargetSpeed = m_fxCornerSpeed2;
            break;
        }

    }

    // Calculate angle to the current waypoint

    fix16_t fxAngleToWayPoint = fix16_atan2(fxDirY, fxDirX);
   	fix16_t shipAngle = m_fxAngle % (fix16_pi << 1);

    fix16_t fxAngleDiff = fxAngleToWayPoint - shipAngle;
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
        // Not colided

        // Get target speed
         // If my direction is over 90 degrees wrrong, slow down
         fix16_t fxTargetSpeed = m_fxWaypointTargetSpeed;
        if( (fxAngleDiff > (fix16_pi>>2) ) || (fxAngleDiff < -(fix16_pi>>2) ) )
            fxTargetSpeed = m_fxCornerSpeed2;

        // Calculate speed to the current waypoint
        if( m_fxVel > fxTargetSpeed )
            m_fxVel -= m_fxDeacc;  // Break
        else
            m_fxVel += m_fxAcc;
    }

    // Limit speed
    if(m_fxVel > m_fxMaxSpeed)
        m_fxVel = m_fxMaxSpeed;
    else if(m_fxVel < fxMaxSpeedCollided)
        m_fxVel = fxMaxSpeedCollided;

    fix16_t fxCos = fix16_cos(m_fxAngle);
    fix16_t fxSin = fix16_sin(m_fxAngle);

    m_fxX += fix16_mul(m_fxVel, fxCos);
    m_fxY += fix16_mul(m_fxVel, fxSin);

    // Update the ship position on track.
    UpdateTrackPos();
}

void CShip::Reset()
{
     // Reset game
    m_activeLapNum = 1;
    m_lapTimingState = enumReadyToStart;
}

// Update the ship position on track.
void CShip::UpdateTrackPos()
{
     // Get the position.
     //int32_t pos = fix16_to_int((m_fxX>>8) + ((m_fxY>>8) * 8));
     int32_t pos = (fix16_to_int(m_fxX)>>8) + ((fix16_to_int(m_fxY)>>8) * 8);

    // Find the current track index
    int32_t i = m_trackIndex;
    for(int32_t ii=0; ii<10; ii++, i++ )
    {
        if(i>=trackTraceLineCount)
            i = 0;

        if(trackTraceLine[i] == pos)
        {
            // Found the ship position on track!
            m_trackIndex = i;
            break;
        }
    }
}

