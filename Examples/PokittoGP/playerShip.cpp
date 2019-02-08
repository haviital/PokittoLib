#include "Pokitto.h"
#include "PokittoCookie.h"
#include "fix16.h"
#include "playerShip.h"

const fix16_t fxInitialRotVel = fix16_pi / 1000;
const uint8_t amplitude = 170;
const int8_t wavetype = 1;
const int8_t wavetypeCrash = 4;
const int8_t arpmode=0;
const fix16_t fxRotAccFactor = fix16_from_float(1.2);

CPlayerShip::CPlayerShip()
{
    m_final_lap_time_ms = 0;
    m_start_ms = 0;
    m_isCollidedToPlayerShip = false;
    m_isCollided = false;
    m_isTurningLeft = false;
    m_isTurningRight = false;
    m_jumpAnimValue = NULL;
    m_boosterAnimValue = NULL;

    // *** Setup sound
    m_tonefreq=0;
   m_fxCameraBehindPlayerTarget = fxCameraBehindPlayerY;
    m_fxCameraBehindPlayerCurrent = fxCameraBehindPlayerY;
}

void CPlayerShip::Update()
{
    fix16_t fxVelOld = m_fxVel;
    bool prevCollided = m_isCollided;

    m_bitmap = billboard_object_bitmaps[0];  // org car

    // Calcucalate the current waypoint and rank.
    if( m_doRecalcRank )
        CalculateRank();
    m_doRecalcRank = false;

    // *** Check collision to road edges
    m_isCollided = false;
    uint8_t wavetype = 1;
    //uint8_t tileIndex = 5;
    uint8_t tileIndex = GetTileIndexCommon(fix16_to_int(m_fxX), fix16_to_int(m_fxY));
    if( (tileIndex == 16 ) )
    {
        // Jump from the ramp.
        if( ! m_jumpAnimValue )
        {
            m_jumpAnimValue = CAnimValue::GetFreeElement();
            m_jumpAnimValue->Start( 1500, 0, fix16_pi, this, 0 );
        }
    }
    else if( tileIndex == 15 )
    {
        // Booster.
        if( ! m_boosterAnimValue )
        {
            m_boosterAnimValue = CAnimValue::GetFreeElement();
            m_boosterAnimValue->Start( 1000, fix16_one, fix16_one, this, 1 );
        }
    }
    else if( ! m_jumpAnimValue && tileIndex != 5 && tileIndex != 6 &&
        (tileIndex < 11 || tileIndex > 15) )
    {
        // Collided to the terrain.
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

    // Handle race/time trial starting and ending detection.
    bool isOnStartingGrid = ( tileIndex >= 11 && tileIndex <= 14);
    HandleRaceStartingAndEnding( isOnStartingGrid );

    // Find active waypoint
    FindActiveWaypoint();

    // Read keys.
    HandleGameKeys();

    // Boost
    if( m_boosterAnimValue )
        m_fxVel = g_fxBoostVel;

    // If colliding, slow down
    if( m_isCollided ) {

        // Break or stop
        if( /*m_isCollidedToPlayerShip*/ false )
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
    if(m_fxVel>fxMaxSpeed && m_boosterAnimValue==NULL )
        m_fxVel = fxMaxSpeed;
    else if(m_fxVel<-fxMaxSpeed)
        m_fxVel = -fxMaxSpeed;

    fix16_t fxCos = fix16_cos(m_fxAngle);
    fix16_t fxSin = fix16_sin(m_fxAngle);

    fix16_t fxVelX = fix16_mul(m_fxVel, fxCos);
    fix16_t fxVelY = fix16_mul(m_fxVel, fxSin);

    if( m_fxImpulseAcc != 0 )
    {
        // Move ship along the impulse.
        fix16_t fxImpulseCos = fix16_cos( m_fxImpulseAngle );
        fix16_t fxImpulseSin = fix16_sin( m_fxImpulseAngle );
        fix16_t fxImpulseVelX = fix16_mul( m_fxImpulseAcc, fxImpulseCos );
        fix16_t fxImpulseVelY = fix16_mul( m_fxImpulseAcc, fxImpulseSin );
        fxVelX += fxImpulseVelX;
        fxVelY += fxImpulseVelY;
#if 0
        // Project impulse on ship acc vector
        // Get the angle between impulse and ship
        fix16_t fxAngleBetween = m_fxAngle - fxShipAngle;
        fix16_t fxAngleBetweenCos = fix16_cos( fxAngleBetween );
        fix16_t fxImpulseVecOnShipVec = fix16_mul( m_fxVel, fxAngleBetweenCos );
        m_fxVel += fxImpulseVecOnShipVec;
        if( m_fxVel < 0 )
            m_fxVel = 0;
#endif
        // Reset impulse
        m_fxImpulseAngle = 0;
        m_fxImpulseAcc = 0;
    }

    m_fxX += fxVelX;
    m_fxY += fxVelY;

    // Change sound effect if needed.
    if(fxVelOld != m_fxVel || prevCollided != m_isCollided )
    {
        if(m_boosterAnimValue)
        {
            // Booster sound
            setOSC(&osc1,1,/*wavetype*/4,1,0,0,/* m_tonefreq */ 40,amplitude,0,0,0,0,0,0,arpmode,1,0);
        }
        else
        {
            // Sound by the speed
            m_tonefreq = fix16_to_int(abs(m_fxVel*5));
            if(m_tonefreq>50) m_tonefreq = 50;
                //snd.playTone(1,m_tonefreq,amplitude,wavetype,arpmode);
            setOSC(&osc1,1,wavetype,1,0,0,m_tonefreq,amplitude,0,0,0,0,0,0,arpmode,1,0);
        }
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

void CPlayerShip::HandleRaceStartingAndEnding( bool isOnStartingGrid )
{
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
        }
        break;
    case enumOnTimedTrack:
        if( isOnStartingGrid && m_activeWaypointIndex > waypointCount - 5 )  // At least the one of the 5 last waypoints visited
        {
            // Startup song.
            SetupMusic(2);

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
            SaveHighScore(m_final_lap_time_ms);

            m_activeLapNum++;
        }
        break;

    case enumFinished:
        break;
    }
}

void CPlayerShip::FindActiveWaypoint()
{

    // *** Update visited waypoints

    // Direction vector to the current waypoint. Check the 5 next waypoints, one at a frame.
    m_lastCheckedWPIndex++;
    if( ++m_lastCheckedWPIndex >= waypointCount + 5)
        m_lastCheckedWPIndex -= waypointCount;  // new round
    if( m_lastCheckedWPIndex > m_activeWaypointIndex + 5 || m_lastCheckedWPIndex < m_activeWaypointIndex )
       m_lastCheckedWPIndex = m_activeWaypointIndex + 1;

    // Calc real index.  m_lastCheckedWPIndex can be bigger than waypointCount.
    int8_t lastCheckedWPIndex = m_lastCheckedWPIndex;
    if(lastCheckedWPIndex >= waypointCount )
        lastCheckedWPIndex -= waypointCount;


    fix16_t fxDirX = fix16_from_int(waypoints[ lastCheckedWPIndex].x ) - m_fxX;
    fix16_t fxDirY = fix16_from_int(waypoints[ lastCheckedWPIndex].y ) - m_fxY;

    // Calculate distance.
    // Scale down so that it will not overflow
    fix16_t fxX3 = fxDirX;
    fix16_t fxY3 = fxDirY;
    fxX3 >>= 4;
    fxY3 >>= 4;
    fix16_t fxDistanceToWaypoint = fix16_mul(fxX3, fxX3) + fix16_mul(fxY3,fxY3);
    const fix16_t fxLimit = fix16_from_int(20*20);
    if( fxDistanceToWaypoint < fxLimit>>4 )
    {
        m_activeWaypointIndex = lastCheckedWPIndex;
        m_lastCheckedWPIndex = -1; // None checked
        m_activeWaypointFoundTimeInMs = mygame.getTime();
        m_doRecalcRank = true;
        //m_waypointsVisited[ m_lastVisitedWPIndex ] = true;
    }
}

void CPlayerShip::CalculateRank()
{
    int32_t numOfCarsWithBetterRank = 0;
    int8_t activeWaypointIndex = m_activeWaypointIndex + 1;  // active waypoint is 1 less for player than for other ships
    for(int32_t i=1; i < g_shipCount; i++)
    {
        if( g_ships[i]->m_activeLapNum > m_activeLapNum ||
            ( g_ships[i]->m_activeLapNum == m_activeLapNum && g_ships[i]->m_activeWaypointIndex >= activeWaypointIndex ))
        {
            numOfCarsWithBetterRank++;
        }
    }

    m_currentRank = numOfCarsWithBetterRank+1;
}

void CPlayerShip::Reset()
{
    CShip::Reset();

     // Reset game
    m_fxX = fix16_from_int(45);
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
    //snd.playTone(1,m_tonefreq,amplitude,wavetype,arpmode);
    setOSC(&osc1,1,wavetype,1,0,0,m_tonefreq,amplitude,0,0,0,0,0,0,arpmode,1,0);
    //for( int32_t i = 0; i < 50 && i < waypointCount; i++ ) m_waypointsVisited[ i ] = false;
    m_activeWaypointIndex = -1;
    m_lastCheckedWPIndex = -1;
    m_doRecalcRank = false;
    m_activeWaypointFoundTimeInMs = 0;
    if( m_jumpAnimValue )
        m_jumpAnimValue->Reset();
    m_jumpAnimValue = NULL;
    if( m_boosterAnimValue )
        m_boosterAnimValue->Reset();
    m_boosterAnimValue = NULL;
 }

// Handle keys
void CPlayerShip::HandleGameKeys()
{

#if 1
    // Playing

    // Debug: calculate available RAM is all button , A, B and C, are pressed!
    if( mygame.buttons.aBtn() && mygame.buttons.bBtn() && mygame.buttons.cBtn() )
        CalcFreeRamAndHang();

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
    // Break a little (friction) if no A button is pressed
    else  {

        if(!m_isCollided || m_fxVel>=fxMaxSpeedCollided)
        {
            m_fxVel = m_fxVel - (fix16_one>>5); // Friction
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

// Animation finished.
void CPlayerShip::Finished( int32_t par )
{
    if(par==0)
        m_jumpAnimValue = NULL;
    else if(par==1)
        m_boosterAnimValue = NULL;
}

