#include "Pokitto.h"
#include "PokittoCookie.h"
#include "fix16.h"
#include "playerShip.h"

const fix16_t fxInitialRotVel = fix16_pi / 1000;
const uint8_t amplitude = 255;//127;
const int8_t wavetype = 1;
const int8_t wavetypeCrash = 4;
const int8_t arpmode=1;
const fix16_t fxRotAccFactor = fix16_from_float(1.2);

CPlayerShip::CPlayerShip()
{
    m_lapTimingState = enumReadyToStart;
    m_final_lap_time_ms = 0;
    m_start_ms = 0;
    m_isCollidedToPlayerShip = false;
    m_isCollidedToSurface = false;
    m_isTurningLeft = false;
    m_isTurningRight = false;

    // *** Setup sound
    m_tonefreq=0;
    snd.ampEnable(1);
    snd.playTone(1,m_tonefreq,amplitude,wavetype,arpmode);
    m_fxCameraBehindPlayerTarget = fxCameraBehindPlayerY;
    m_fxCameraBehindPlayerCurrent = fxCameraBehindPlayerY;
}

void CPlayerShip::Update()
{
    fix16_t fxVelOld = m_fxVel;
    bool prevCollided = m_isCollidedToSurface;

    // Calcucalate the current waypoint and rank.
    CalculateRank();

    // *** Check collision to road edges
    m_isCollidedToSurface = false;
    uint8_t wavetype = 1;
    //uint8_t tileIndex = 5;
    uint8_t tileIndex = GetTileIndexCommon(fix16_to_int(m_fxX), fix16_to_int(m_fxY));
    if(
        tileIndex != 5 && tileIndex != 6 &&
        (tileIndex < 11 || tileIndex > 15)
    )
    {
        m_isCollidedToSurface = true;
        wavetype = 5;

    }

    // get the current lap time
    if( m_lapTimingState == enumReadyToStart )
        m_current_lap_time_ms = 0;
    else if( m_lapTimingState == enumFinished )
        m_current_lap_time_ms = m_final_lap_time_ms;
    else
        m_current_lap_time_ms = mygame.getTime() - m_start_ms;

    // Handle lap starting and ending detection.
    bool isOnStartingGrid = ( tileIndex >= 11 && tileIndex <= 14);
    bool isOnHalfWayPoint = (tileIndex == 15);
    switch(m_lapTimingState)
    {
    case enumReadyToStart:
        if( isOnStartingGrid )
        {
            m_lapTimingState = enumStarted;
            m_start_ms = mygame.getTime();  // started
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
            m_final_lap_time_ms = mygame.getTime() - m_start_ms;
            m_lapTimingState = enumFinished;
            //menu.m_mode = CMenu::enumTimeTrialFinishedMenu;
            //menu.m_isOpen = true;

            // Save cookie if this is the best time
            if(highscore.bestLap_ms == 0 || m_final_lap_time_ms < highscore.bestLap_ms)
            {
                highscore.bestLap_ms = m_final_lap_time_ms;
                highscore.saveCookie();
            }
        }
        break;
    case enumFinished:
        break;
    }

    // Read keys.
    HandleGameKeys();

    // If colliding, slow down
    if( m_isCollidedToSurface ) {


        // Break or stop
        if( m_isCollidedToPlayerShip )
        {
            m_fxVel = fix16_one;
        }
        else if(m_fxVel > fxMaxSpeedCollided)
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

    // Limit turning speed
    if(m_fxRotVel>fxInitialRotVel*10)
        m_fxRotVel = fxInitialRotVel*10;

    // Limit speed
    if(m_fxVel>fxMaxSpeed)
        m_fxVel = fxMaxSpeed;
    else if(m_fxVel<-fxMaxSpeed)
        m_fxVel = -fxMaxSpeed;

    fix16_t fxCos = fix16_cos(m_fxAngle);
    fix16_t fxSin = fix16_sin(m_fxAngle);

    m_fxX += fix16_mul(m_fxVel, fxCos);
    m_fxY += fix16_mul(m_fxVel, fxSin);

    // Change sound effect if needed.
    if(fxVelOld != m_fxVel || prevCollided != m_isCollidedToSurface )
    {
        m_tonefreq = fix16_to_int(abs(m_fxVel*5));
        if(m_tonefreq>50) m_tonefreq = 50;
        snd.playTone(1,m_tonefreq,amplitude,wavetype,arpmode);
    }

    // Update camera pos

    //fix16_t fxDiff =  m_fxCameraBehindPlayerTarget - m_fxCameraBehindPlayerCurrent;
    //fxDiff >>= 1;
    //m_fxCameraBehindPlayerCurrent += fxDiff;

    if(m_fxCameraBehindPlayerTarget > m_fxCameraBehindPlayerCurrent)
    {
        m_fxCameraBehindPlayerCurrent += fix16_one>>2;
    }
    else if(m_fxCameraBehindPlayerTarget < m_fxCameraBehindPlayerCurrent)
    {
        m_fxCameraBehindPlayerCurrent -= fix16_one>>2;
    }
}

void CPlayerShip::CalculateRank()
{
    // Calc distance to the 4 next waypoints and choose the nearest.
    int32_t oldWaypointIndex = m_activeWaypointIndex;
    int32_t ii = m_activeWaypointIndex;
    fix16_t fxPlayerMinDist = 0;
    for(int32_t i=0; i<4; i++, ii++)
    {
        if(ii >= waypointCount)
            ii = 0;

       // Direction vector to the current waypoint.
        fix16_t fxDirX = fix16_from_int(waypoints[ii].x) - m_fxX;
        fix16_t fxDirY = fix16_from_int(waypoints[ii].y) - m_fxY;

        // Calculate distance.
        // Scale down so that it will not overflow
        fix16_t fxX3 = fxDirX;
        fix16_t fxY3 = fxDirY;
        fxX3 >>= 4;
        fxY3 >>= 4;
        fix16_t fxDistanceToWaypoint = fix16_mul(fxX3, fxX3) + fix16_mul(fxY3,fxY3);
        if(fxDistanceToWaypoint< fxPlayerMinDist)
        {
            fxPlayerMinDist = fxDistanceToWaypoint;
            m_activeWaypointIndex = ii;
        }
    }

    // Should we increase the lap?
    if(oldWaypointIndex > m_activeWaypointIndex)
        m_activeLapNum++;

    // Calc the current rank.
    if( g_isRace && (g_frameNum % 100) == 0)
    {
        m_currentRank = 1;
        for(int32_t i=1; i < g_shipCount; i++)
        {
            if( g_ships[i]->m_activeLapNum > m_activeLapNum ||
                g_ships[i]->m_activeWaypointIndex > m_activeWaypointIndex )
            {
                m_currentRank++;
            }
            else if( g_ships[i]->m_activeLapNum == m_activeLapNum ||
                g_ships[i]->m_activeWaypointIndex == m_activeWaypointIndex )
            {
                // The same waypoint as the player. Compare the distances.

               // Direction vector to the current waypoint.
                int32_t index = g_ships[i]->m_activeWaypointIndex;
                fix16_t fxDirX = fix16_from_int(waypoints[index].x) - m_fxX;
                fix16_t fxDirY = fix16_from_int(waypoints[index].y) - m_fxY;

                // Calculate distance.
                // Scale down so that it will not overflow
                fix16_t fxX3 = fxDirX;
                fix16_t fxY3 = fxDirY;
                fxX3 >>= 4;
                fxY3 >>= 4;
                fix16_t fxOtherShipDistance = fix16_mul(fxX3, fxX3) + fix16_mul(fxY3,fxY3);

                // Check which is nearer to the waypoint
                if(fxOtherShipDistance < fxPlayerMinDist)
                    m_currentRank++;
            }
        }
    }
}

void CPlayerShip::Reset()
{
    CShip::Reset();

     // Reset game
    m_lapTimingState = enumReadyToStart;
    m_fxX = fix16_from_int(35);
    m_fxY = fix16_from_int(550);
    m_fxVel = 0;
    m_fxAngle = fix16_pi>>1;
    m_fxRotVel = fxInitialRotVel;
    m_fxCameraBehindPlayerTarget = fxCameraBehindPlayerY;
    m_fxCameraBehindPlayerCurrent = fxCameraBehindPlayerY;
    m_currentRank = 0;
    m_current_lap_time_ms = 0;
}

// Handle keys
void CPlayerShip::HandleGameKeys()
{

#if 1
        // Playing

        // Turn left
    if(mygame.buttons.leftBtn()) {
        if( ! m_isTurningLeft )
            m_fxRotVel = fxInitialRotVel; // Reset to initial velocity when started turning
        m_fxAngle += m_fxRotVel;
        m_isTurningLeft = true;
        m_fxRotVel = fix16_mul(m_fxRotVel, fxRotAccFactor);
    }
    else {
        if( m_isTurningLeft )
            m_fxRotVel = fxInitialRotVel;
        m_isTurningLeft = false;
    }

    // Turn right
    if(mygame.buttons.rightBtn()) {
        if( ! m_isTurningRight )
            m_fxRotVel = fxInitialRotVel; // Reset to initial velocity when started turning
        m_fxAngle -= m_fxRotVel;
        m_isTurningRight = true;
        m_fxRotVel = fix16_mul(m_fxRotVel, fxRotAccFactor);
    }
    else {
        if( m_isTurningRight )
            m_fxRotVel = fxInitialRotVel;
        m_isTurningRight = false;
    }


    // Thrust
    if(mygame.buttons.aBtn()) {

        if(!m_isCollidedToSurface || m_fxVel<=fxMaxSpeedCollided)
        {
            m_fxVel = m_fxVel + (fix16_one>>4);
            m_fxCameraBehindPlayerTarget = fxCameraBehindPlayerY + fix16_from_int(3);
        }
    }

    // Reverse
    else if(mygame.buttons.bBtn()) {

        if(!m_isCollidedToSurface || m_fxVel>=fxMaxSpeedCollided)
        {
            m_fxVel = m_fxVel - (fix16_one>>4);
            m_fxCameraBehindPlayerTarget = fxCameraBehindPlayerY - fix16_from_int(5);
        }
    }
    // Break a little if no A button is pressed
    else  {

        if(!m_isCollidedToSurface || m_fxVel>=fxMaxSpeedCollided)
        {
            m_fxVel = m_fxVel - (fix16_one>>5);
            m_fxCameraBehindPlayerTarget = fxCameraBehindPlayerY;
        }

        if(m_fxVel < 0)
            m_fxVel = 0;
    }
#else
    if(mygame.buttons.leftBtn()) {
        m_fxX += fix16_one;
    }
    else if(mygame.buttons.rightBtn()) {
        m_fxX -= fix16_one;
    }
    else if(mygame.buttons.upBtn()) {
        m_fxY += fix16_one;
    }
    else if(mygame.buttons.downBtn()) {
        m_fxY -= fix16_one;
    }
    else if(mygame.buttons.aBtn()) {
        m_fxAngle += fxRotVel;
    }
    else if(mygame.buttons.bBtn()) {
        m_fxAngle -= fxRotVel;
    }

#endif
}


