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
    m_final_lap_time_ms = 0;
    m_start_ms = 0;
    m_isCollidedToPlayerShip = false;
    m_isCollided = false;
    m_isTurningLeft = false;
    m_isTurningRight = false;

    // *** Setup sound
    m_tonefreq=0;
   m_fxCameraBehindPlayerTarget = fxCameraBehindPlayerY;
    m_fxCameraBehindPlayerCurrent = fxCameraBehindPlayerY;
}

void CPlayerShip::Update()
{
    fix16_t fxVelOld = m_fxVel;
    bool prevCollided = m_isCollided;

    // Calcucalate the current waypoint and rank.
    CalculateRank();

    // *** Check collision to road edges
    m_isCollided = false;
    uint8_t wavetype = 1;
    //uint8_t tileIndex = 5;
    uint8_t tileIndex = GetTileIndexCommon(fix16_to_int(m_fxX), fix16_to_int(m_fxY));
    if( m_isCollidedToPlayerShip ||
        (
            tileIndex != 5 && tileIndex != 6 &&
            (tileIndex < 11 || tileIndex > 15)
        )
    )
    {
        m_isCollided = true;
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

            // Open the menu after the race.
            if( g_isRace)
            //if( g_isRace )
            {
                if( m_activeLapNum>2)
                {
                    // Race finished
                    m_requestedMenuMode = CMenu::enumRaceFinishedMenu;
                    m_lapTimingState = enumFinished;
                }
                else
                {
                    m_lapTimingState = enumStarted;
                }
            }
            else
            {
                // Open the menu after the time trial.
                m_requestedMenuMode = CMenu::enumTimeTrialFinishedMenu;
            }


            // Save cookie if this is the best time
            if( !g_isRace && (highscore.bestLap_ms == 0 || m_final_lap_time_ms < highscore.bestLap_ms))
            {
                highscore.bestLap_ms = m_final_lap_time_ms;
                highscore.saveCookie();
            }

            m_activeLapNum++;
        }
        break;

    case enumFinished:
        break;
    }

    // Read keys.
    HandleGameKeys();

    // If colliding, slow down
    if( m_isCollided ) {


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
    if(fxVelOld != m_fxVel || prevCollided != m_isCollided )
    {
        m_tonefreq = fix16_to_int(abs(m_fxVel*5));
        //!!!HV  if(m_tonefreq>50) m_tonefreq = 50;
            //!!!HV snd.playTone(1,m_tonefreq,amplitude,wavetype,arpmode);
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

    //
    m_isCollidedToPlayerShip = false;
}

void CPlayerShip::CalculateRank()
{
    // Update the ship position on track.
    int32_t oldTrackIndex = m_trackIndex;
    UpdateTrackPos();

    //
    if(oldTrackIndex != m_trackIndex)
    {
        int32_t numOfCarsWithBetterRank = 0;
        for(int32_t i=1; i < g_shipCount; i++)
        {
            if( g_ships[i]->m_activeLapNum > m_activeLapNum ||
                ( g_ships[i]->m_activeLapNum == m_activeLapNum && g_ships[i]->m_trackIndex >= m_trackIndex ))
            {
                numOfCarsWithBetterRank++;
            }
        }

        m_currentRank = numOfCarsWithBetterRank+1;
    }
}

void CPlayerShip::Reset()
{
    CShip::Reset();

     // Reset game
    m_fxX = fix16_from_int(35);
    m_fxY = fix16_from_int(550);
    m_fxVel = 0;
    m_fxAngle = fix16_pi>>1;
    m_fxRotVel = fxInitialRotVel;
    m_fxCameraBehindPlayerTarget = fxCameraBehindPlayerY;
    m_fxCameraBehindPlayerCurrent = fxCameraBehindPlayerY;
    m_currentRank = 0;
    m_current_lap_time_ms = 0;
    m_requestedMenuMode = CMenu::enumNoMenu;
    //!!!HV snd.ampEnable(1);
    //!!!HV snd.playTone(1,m_tonefreq,amplitude,wavetype,arpmode);
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

        if(!m_isCollided || m_fxVel<=fxMaxSpeedCollided)
        {
            m_fxVel = m_fxVel + (fix16_one>>4);
            m_fxCameraBehindPlayerTarget = fxCameraBehindPlayerY + fix16_from_int(3);
        }
    }

    // Reverse
    else if(mygame.buttons.bBtn()) {

        if(!m_isCollided || m_fxVel>=fxMaxSpeedCollided)
        {
            m_fxVel = m_fxVel - (fix16_one>>4);
            m_fxCameraBehindPlayerTarget = fxCameraBehindPlayerY - fix16_from_int(5);
        }
    }
    // Break a little if no A button is pressed
    else  {

        if(!m_isCollided || m_fxVel>=fxMaxSpeedCollided)
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

