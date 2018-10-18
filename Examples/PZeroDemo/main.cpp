#include "Pokitto.h"
#include "PokittoCookie.h"
#include "fix16.h"
#include "main.h"
#include "gfx_hdr/GameData.h"
#include "ship.h"


Pokitto::Core mygame;
Pokitto::Sound snd;

// Prototypes
void InitGameObjects();
void CreateObject( uint16_t index, fix16_t fxX, fix16_t fxY, const uint8_t* bitmap, fix16_t fxScaledWidth, fix16_t fxScaledHeight);
void CreateShip( uint16_t objIndex, uint16_t shipIndex, fix16_t fxX, fix16_t fxY, const uint8_t* bitmap, fix16_t fxScaledWidth, fix16_t fxScaledHeight, fix16_t fxVel);
void HandleGameKeys();
void HandleSetupMenu(int32_t& lastListPos);
void DrawMode7(int32_t tile2PosX, int32_t tile2PosY, fix16_t fxAngle);
uint8_t GetTileIndex(int32_t tile2PosX, int32_t tile2PosY, fix16_t fxAngle, int32_t getX, int32_t getY);
bool Draw3dObects(fix16_t fxPosX, fix16_t fxPosY, fix16_t fxAngle);
bool HandleStartGameMenu( int32_t lastLap_ms );
bool HandleGenericMenu( bool showBestTime, int32_t& /*in out */ cursorPos, char* item1, char* item2, char* item3, char* item4);

const int32_t KRotCenterX = 0;
const int32_t KRotCenterY = -44;
const fix16_t fxMaxSpeedCollided = fix16_one>>1;
const fix16_t fxInitialRotVel = fix16_pi / 1000;
const fix16_t fxRotAccFactor = fix16_from_float(1.2);

// 3d lookup tables
// z = (zs * h) / y
fix16_t PerspectiveScaleY[screenH];
fix16_t PerspectiveScaleX[screenH];

// Ship state
bool isTurningLeft = false;
bool isTurningRight = false;
bool collided = false;

// Camera pos
//fix16_t fxX = fix16_from_int(55);
fix16_t fxCamX = fix16_from_int(42);
fix16_t fxCamY = fix16_from_int(490);

// Ship velocity
fix16_t fxRotVel = fxInitialRotVel;
fix16_t fxVel = 0;
fix16_t fxAngle = 0;

// Ship bitmap
const uint8_t* activeShipBitmapData = billboard_object_bitmaps[0];
uint32_t shipBitmapW = *(activeShipBitmapData - 2);
uint32_t shipBitmapH = *(activeShipBitmapData - 1);

//
int32_t textureMode = 1;


/*
struct DrawListItem
{
    Object3d* obj;
    fix16_t fxDistancePot;
};
*/

// Drawing order list
const uint32_t g_drawListMaxCount = 100;
CObject3d* g_drawList[g_drawListMaxCount] = {0};

// Objects
const uint32_t g_objects3dMaxCount = 32;
uint32_t g_objects3dCount = 0;
CObject3d* g_objects3d[ g_objects3dMaxCount ] = {0};

// Ships
const uint32_t g_shipsMaxCount = 10;
uint32_t g_shipCount = 0;
CShip* g_ships[ g_objects3dMaxCount ] = {0};

enum LapTimingState {
    enumReadyToStart = 0,
    enumStarted = 1,
    enumOnTimedTrack = 2,
    enumOverHalfWayPoint = 3,
    enumFinished = 4,

};

enum MenuMode {
    enumMainMenu = 0,
    enumContinueMenu = 1,
    enumTimeTrialFinishedMenu = 2,
};

class mycookie : public Pokitto::Cookie
{
public:
    uint32_t bestLap_ms = 0;
    uint32_t version = 1;
};

// Create an instance of cookie.
mycookie highscore;

// Main
int main () {

    // Initialize variables.
    fix16_t fxVelOld = -1;
    fix16_t fxCos = fix16_one;
    fix16_t fxSin = 0;
    bool prevCollided = false;
    bool isManualRotation = true;
    //bool isSetupMenuActive = false;
    int32_t lastSetupListPos = 1;
    uint32_t start_ms = 0;
    uint32_t final_lap_time_ms = 0;
    LapTimingState lapTimingState = enumReadyToStart;
    bool isMenuOpen = true;
    MenuMode menuMode = enumMainMenu;
    int32_t menuCursorPos = 0;

    // Load cookie
    // NOTE: This must be before Pokitto::core::begin(). Otherwise the audio interrupt do not work!
    highscore.begin("PZERO2", highscore); //initialize cookie
    highscore.loadCookie();
    highscore.version = 1;

    // *** Setup PokittoLib

    mygame.begin();
    mygame.display.load565Palette(palette_pal);
    mygame.display.persistence = 1;
    #ifdef POK_SIM
    mygame.setFrameRate(60);
    #endif // PROJ_SIM

    // Init game object.
    InitGameObjects();

    // *** Setup sound

    int tonefreq=0;
    uint8_t amplitude = 255;//127;
    uint8_t wavetype = 1, wavetypeCrash = 4, arpmode=1;
    snd.ampEnable(1);
    snd.playTone(1,tonefreq,amplitude,wavetype,arpmode);
    //snd.playTone(1,100,255,0);

    //uint8_t const discrete_vol_levels[8]      {0,32,64,96,128,160,192,224};
    //Pokitto::Sound::globalVolume = discrete_vol_levels[7];
    //snd.setVolume(Pokitto::Sound::globalVolume);


    // *** Calculate lookup tables.

    // z = (zs * h) / y
    fix16_t fxPerspectiveFactor = fix16_from_int(90*screenH);
    for( int32_t y = 0; y<screenH; y++) {

        #if 1 // 3d
         // s = k/(y+15) ==> y+15 = k/s ==> y = k/s -15;
         // y = zk*yk /z -15
         PerspectiveScaleY[y] = fix16_div(fxPerspectiveFactor, fix16_from_float((float)((y+screenShiftY)*2)));
         PerspectiveScaleX[y] = PerspectiveScaleY[y];
        #else // 2d
         PerspectiveScaleY[y] = fix16_from_float(y*2.0);
         PerspectiveScaleX[y] = fix16_from_float(100*2.0);
        #endif
    }

    // Get mipmap pointers.
    for( uint32_t ii=0; ii<current_texture_bitmaps_count; ii++)
    {
        current_texture_bitmaps_mm1[ii] =  current_texture_bitmaps[ii] + (texW * tileH);
        current_texture_bitmaps_mm2[ii] =  current_texture_bitmaps[ii] + (texW * tileH) + (tileW>>1);
    }

    //
    start_ms = mygame.getTime();

    // *** The game loop
    while (mygame.isRunning()) {

        if (mygame.update()) {

            // Draw sky
            fix16_t fxAnglePositive =  ((-fxAngle) % (fix16_pi<<1)) +  (fix16_pi<<1);
            int16_t skyX = ((fxAnglePositive>>9) % 22);
            uint16_t skyW = image_sky_long[0];
            uint16_t skyH = image_sky_long[1];
            const uint8_t* skyBitmapPtr = &(image_sky_long[2]);
            DrawBitmapOpaque8bit( 0 - skyX, 0, skyBitmapPtr, skyW, skyH );

            // ** Draw the road and edges and terrain.

            DrawMode7(fix16_to_int(fxCamX), fix16_to_int(fxCamY), fxAngle);

            // Draw 3d objects
            bool isCollidedToPlayerShip = Draw3dObects(fxCamX, fxCamY, fxAngle);


            // ** Draw the ship shadow

            uint16_t shadowW = image_shadow[0];
            uint16_t shadowH = image_shadow[1];
            const uint8_t* shadowBitmapPtr = &(image_shadow[2]);
            DrawScaledBitmap8bit( 55-(shadowW>>1), 56+7,
                                  shadowBitmapPtr,
                                  shadowW, shadowH, shadowW, shadowH);

            // *** Draw ship bitmap

            // Move ship up and down when collided
            int32_t shipY = 56;
            if(collided) {
                fix16_t fxBumbAngle = fxCamX + fxCamY;
                fix16_t fxBumpHeight = fix16_sin(fxBumbAngle) * 4;
                shipY -= abs(fix16_to_int(fxBumpHeight));
            }

            // Draw the bitmpa (currently not scaled)
            DrawScaledBitmap8bit( 55-(shipBitmapW>>1), shipY,
                                  activeShipBitmapData,
                                  shipBitmapW, shipBitmapH, shipBitmapW, shipBitmapH);

            // *** Draw the lap counter

            // get curren lap time
            uint32_t current_lap_time_ms = 0;
            if( lapTimingState == enumReadyToStart )
                current_lap_time_ms = 0;
            else if( lapTimingState == enumFinished )
                current_lap_time_ms = final_lap_time_ms;
            else
                current_lap_time_ms = mygame.getTime() - start_ms;

            // Draw lap time
            int32_t lapStartX = 110-5;  // 5 pixel margin
            lapStartX -= 5*6; // 5 chars
            DrawLapTime(current_lap_time_ms, lapStartX, 1, fix16_one );

            // *** Check collision
            prevCollided = collided;
            uint8_t tileIndex = GetTileIndex(fix16_to_int(fxCamX), fix16_to_int(fxCamY), fxAngle, 55, 56);
            if( isCollidedToPlayerShip ||
                (
                    tileIndex != 5 && tileIndex != 6 &&
                    (tileIndex < 11 || tileIndex > 15)
                )
            )
            {
                collided = true;
                wavetype = 5;
            }
            else {
                collided = false;
                wavetype = 1;
            }

            // *** starting line
            bool isOnStartingGrid = ( tileIndex >= 11 && tileIndex <= 14);
            bool isOnHalfWayPoint = (tileIndex == 15);

            // Hit the starting line
            switch(lapTimingState)
            {
            case enumReadyToStart:
                if( isOnStartingGrid )
                {
                    lapTimingState = enumStarted;
                    start_ms = mygame.getTime();  // started
                }
                break;
            case enumStarted:
                if( ! isOnStartingGrid )
                {
                    //lapTimingState = enumOnTimedTrack;
                    lapTimingState = enumOverHalfWayPoint;
                }
                break;
            case enumOnTimedTrack:
                if( isOnHalfWayPoint )
                {
                    lapTimingState = enumOverHalfWayPoint;
                 }
                break;
            case enumOverHalfWayPoint:
                if( isOnStartingGrid )
                {
                    // Finished!
                    final_lap_time_ms = mygame.getTime() - start_ms;
                    lapTimingState = enumFinished;
                    menuMode = enumTimeTrialFinishedMenu;
                    isMenuOpen = true;

                    // Save cookie if this is the best time
                    if(highscore.bestLap_ms == 0 || final_lap_time_ms < highscore.bestLap_ms)
                    {
                        highscore.bestLap_ms = final_lap_time_ms;
                        highscore.saveCookie();
                    }
                }
                break;
            case enumFinished:
                break;
            }

            // Print coordinates on screen
            #if 0
            char text[128];
            mygame.display.print(0,0, itoa(fix16_to_int(fxCamX),text,10));
            mygame.display.print(", ");
            mygame.display.print(itoa(fix16_to_int(fxCamY)+65,text,10));
            mygame.display.print("     ");
            #endif

            // Open/close the setup menu
            if(isMenuOpen || mygame.buttons.pressed(BTN_C))
            {
                isMenuOpen = true;
            }

            if(isMenuOpen)
            {
                switch( menuMode )
                {
                case enumMainMenu:
                    {
                        isMenuOpen =  HandleGenericMenu( true, menuCursorPos, "Time trial", "Race", NULL, NULL);
                        if( ! isMenuOpen )
                        {
                            // if time trial
                            if(menuCursorPos == 0)
                                menuMode = enumContinueMenu;

                             // Menu closed
                            menuCursorPos = 0;
                       }
                   }
                   break;

                case enumContinueMenu:
                    {
                       isMenuOpen =  HandleGenericMenu( true, menuCursorPos, "Restart", "Continue", "Exit race", NULL);
                        if( ! isMenuOpen )
                        {
                             // if "Restart" selected, go to main menu
                            if(menuCursorPos == 0)
                            {
                                lapTimingState = enumReadyToStart;

                                 // Reset game
                                fxCamX = fix16_from_int(42);
                                fxCamY = fix16_from_int(490);
                                fxVel = 0;
                                fxAngle = 0;
                                fxRotVel = fxInitialRotVel;
                           }

                           // if "Continue", go to main menu
                            else if(menuCursorPos == 1)
                            {
                                // Continue
                            }

                           // if "Exit race" selected, go to main menu
                            else if(menuCursorPos == 2)
                            {
                                menuMode = enumMainMenu;
                                isMenuOpen = true;
                            }

                             // Menu closed
                            menuCursorPos = 0;

                       }
                    }
                    break;

                case enumTimeTrialFinishedMenu:
                    {
                       isMenuOpen =  HandleGenericMenu( true, menuCursorPos, "Restart", "Exit race", NULL, NULL);
                        if( ! isMenuOpen )
                        {
                            // if "Restart" selected, go to main menu
                            if(menuCursorPos == 0)
                            {
                                lapTimingState = enumReadyToStart;

                                 // Reset game
                                fxCamX = fix16_from_int(42);
                                fxCamY = fix16_from_int(490);
                                fxVel = 0;
                                fxAngle = 0;
                                fxRotVel = fxInitialRotVel;
                            }

                            // if "Exit race" selected, go to main menu
                            else if(menuCursorPos == 1)
                            {
                                menuMode = enumMainMenu;
                                isMenuOpen = true;
                            }

                            // Menu closed
                            menuCursorPos = 0;

                        }
                    }
                    break;

                default:
                    break;
                }

            }

            if(!isMenuOpen)
            {
                // No menu opean

                // Check buttons
                HandleGameKeys();

                // Limit turning speed
                if(fxRotVel>fxInitialRotVel*10)
                    fxRotVel = fxInitialRotVel*10;

                // Limit speed
                if(fxVel>fxMaxSpeed)
                    fxVel = fxMaxSpeed;
                else if(fxVel<-fxMaxSpeed)
                    fxVel = -fxMaxSpeed;

                // If colliding, slow down
                if( collided ) {

                    // Break or stop
                    if( isCollidedToPlayerShip )
                    {
                        fxVel = fix16_one;
                    }
                    else if(fxVel>fxMaxSpeedCollided)
                    {
                        fxVel = fxVel - (fix16_one>>4);
                        if(fxVel<0)
                            fxVel = 0;
                    }
                    else if(fxVel<-fxMaxSpeedCollided)
                    {
                        fxVel = fxVel + (fix16_one>>4);
                        if(fxVel>0)
                            fxVel = 0;
                    }
                }

                // Change sound effect if needed.
                if(fxVelOld != fxVel || prevCollided != collided ) {
                    tonefreq = fix16_to_int(abs(fxVel*5));
                    if(tonefreq>50) tonefreq = 50;
                    snd.playTone(1,tonefreq,amplitude,wavetype,arpmode);
                }

                // Move the  texture plane

                fxCos = fix16_cos(-fxAngle);
                fxSin = fix16_sin(-fxAngle);

                fxCamY = fxCamY + fix16_mul(fxVel, fxCos);
                fxCamX = fxCamX + fix16_mul(fxVel, fxSin);
                fxVelOld = fxVel;

                // Move other ships
                for(int32_t i=0; i < g_shipCount; i++)
                {
                    g_ships[i]->Update();
                }
            }
        }
    }
}

// Init game objects
void InitGameObjects()
{
    // Setup game objects
    fix16_t fxCarOffsetX = fix16_from_int(0);
    fix16_t fxCarOffsetY = fix16_from_int(650);
    fix16_t fxCarStepY = fix16_from_int(80);
    fix16_t fxRoadWidth = fix16_from_int(97+30);
    fix16_t fxScaledSizeFactor = fix16_from_float(0.65);
    fix16_t fxCactusScaledSizeFactor = fix16_from_float(0.8);
    uint8_t i = 0;

    // Set cactuses
    const uint8_t* cactus_bm = billboard_object_bitmaps[25];
    CreateObject(i++, fix16_from_int(3), fix16_from_int(632), cactus_bm,
              *(cactus_bm - 2) * fxCactusScaledSizeFactor,
              *(cactus_bm - 1) * fxCactusScaledSizeFactor );
    CreateObject(i++, fix16_from_int(243), fix16_from_int(1465), cactus_bm,
              *(cactus_bm - 2) * fxCactusScaledSizeFactor,
              *(cactus_bm - 1) * fxCactusScaledSizeFactor );
    CreateObject(i++, fix16_from_int(706), fix16_from_int(1425), cactus_bm,
              *(cactus_bm - 2) * fxCactusScaledSizeFactor,
              *(cactus_bm - 1) * fxCactusScaledSizeFactor );
    CreateObject(i++, fix16_from_int(667), fix16_from_int(2024), cactus_bm,
              *(cactus_bm - 2) * fxCactusScaledSizeFactor,
              *(cactus_bm - 1) * fxCactusScaledSizeFactor );
    CreateObject(i++, fix16_from_int(1174), fix16_from_int(1844), cactus_bm,
              *(cactus_bm - 2) * fxCactusScaledSizeFactor,
              *(cactus_bm - 1) * fxCactusScaledSizeFactor );
    CreateObject(i++, fix16_from_int(2050), fix16_from_int(1851), cactus_bm,
              *(cactus_bm - 2) * fxCactusScaledSizeFactor,
              *(cactus_bm - 1) * fxCactusScaledSizeFactor );
    CreateObject(i++, fix16_from_int(1922), fix16_from_int(1130), cactus_bm,
              *(cactus_bm - 2) * fxCactusScaledSizeFactor,
              *(cactus_bm - 1) * fxCactusScaledSizeFactor );
    CreateObject(i++, fix16_from_int(1986), fix16_from_int(35), cactus_bm,
              *(cactus_bm - 2) * fxCactusScaledSizeFactor,
              *(cactus_bm - 1) * fxCactusScaledSizeFactor );

    // Set stones
    const uint8_t* stone_bm = billboard_object_bitmaps[26];
    CreateObject(i++, fix16_from_int(131), fix16_from_int(1056), stone_bm,
              fix16_from_int(*(stone_bm - 2)), fix16_from_int(*(stone_bm - 1)) );
    CreateObject(i++, fix16_from_int(582), fix16_from_int(1469), stone_bm,
              fix16_from_int(*(stone_bm - 2)), fix16_from_int(*(stone_bm - 1)) );
    CreateObject(i++, fix16_from_int(953), fix16_from_int(2042), stone_bm,
              fix16_from_int(*(stone_bm - 2)), fix16_from_int(*(stone_bm - 1)) );
    CreateObject(i++, fix16_from_int(1839), fix16_from_int(1920), stone_bm,
              fix16_from_int(*(stone_bm - 2)), fix16_from_int(*(stone_bm - 1)) );
    CreateObject(i++, fix16_from_int(521), fix16_from_int(127), stone_bm,
              fix16_from_int(*(stone_bm - 2)), fix16_from_int(*(stone_bm - 1)) );
    CreateObject(i++, fix16_from_int(581), fix16_from_int(505), stone_bm,
              fix16_from_int(*(stone_bm - 2)), fix16_from_int(*(stone_bm - 1)) );
    CreateObject(i++, fix16_from_int(333), fix16_from_int(634), stone_bm,
              fix16_from_int(*(stone_bm - 2)), fix16_from_int(*(stone_bm - 1)) );
    CreateObject(i++, fix16_from_int(6), fix16_from_int(1220), stone_bm,
              fix16_from_int(*(stone_bm - 2)), fix16_from_int(*(stone_bm - 1)) );

    // Create ships
    uint32_t shipIndex = 0;
    fix16_t shipStepX = 50;
    fix16_t shipStepY = 50;
    g_shipCount = 10;
    for(int32_t ii=0; ii<g_shipCount; ii++)
    {
        int32_t shipBitmapIndex = (rand()%25) + 1;
        const uint8_t* bmData = billboard_object_bitmaps[shipBitmapIndex];
        fix16_t fxBmWidth = *(bmData - 2) * fxScaledSizeFactor;
        fix16_t fxBmHeight = *(bmData - 1) * fxScaledSizeFactor;
        CreateShip(i++, ii, fix16_from_int(30 + ((ii&1) ? shipStepX : 0)), fix16_from_int(600 + (shipStepY*ii)),
                   bmData, fxBmWidth, fxBmHeight, fix16_one );
    }

    // Set the object count.
    g_objects3dCount = i;

    static_assert( g_objects3dMaxCount <= g_drawListMaxCount, "error");
    for( int32_t i = 0; i < g_objects3dCount; i++)
    {
        g_drawList[i] = g_objects3d[i];
    }

}

// Handle keys
void HandleGameKeys()
{

#if 1
        // Playing

        // Turn left
    if(mygame.buttons.leftBtn()) {
        if( ! isTurningLeft )
            fxRotVel = fxInitialRotVel; // Reset to initial velocity when started turning
        fxAngle += fxRotVel;
        isTurningLeft = true;
        fxRotVel = fix16_mul(fxRotVel, fxRotAccFactor);
    }
    else {
        if( isTurningLeft )
            fxRotVel = fxInitialRotVel;
        isTurningLeft = false;
    }

    // Turn right
    if(mygame.buttons.rightBtn()) {
        if( ! isTurningRight )
            fxRotVel = fxInitialRotVel; // Reset to initial velocity when started turning
        fxAngle -= fxRotVel;
        isTurningRight = true;
        fxRotVel = fix16_mul(fxRotVel, fxRotAccFactor);
    }
    else {
        if( isTurningRight )
            fxRotVel = fxInitialRotVel;
        isTurningRight = false;
    }


    // Thrust
    if(mygame.buttons.aBtn()) {

        if(!collided || fxVel<=fxMaxSpeedCollided)
            fxVel = fxVel + (fix16_one>>4);
    }

    // Reverse
    else if(mygame.buttons.bBtn()) {

        if(!collided || fxVel>=fxMaxSpeedCollided)
            fxVel = fxVel - (fix16_one>>4);
    }
    // Break a little if no A button is pressed
    else  {

        if(!collided || fxVel>=fxMaxSpeedCollided)
            fxVel = fxVel - (fix16_one>>5);
        if(fxVel < 0)
            fxVel = 0;
    }
#else
    if(mygame.buttons.leftBtn()) {
        fxCamX += fix16_one;
    }
    else if(mygame.buttons.rightBtn()) {
        fxCamX -= fix16_one;
    }
    else if(mygame.buttons.upBtn()) {
        fxCamY += fix16_one;
    }
    else if(mygame.buttons.downBtn()) {
        fxCamY -= fix16_one;
    }
    else if(mygame.buttons.aBtn()) {
        fxAngle += fxRotVel;
    }
    else if(mygame.buttons.bBtn()) {
        fxAngle -= fxRotVel;
    }

#endif
}

//
bool HandleGenericMenu( bool showBestTime, int32_t& /*in out */ cursorPos, char* item1, char* item2, char* item3, char* item4)
{
    // Set window
    int32_t winX = 0;
    int32_t winY = 16;
    int32_t winW = 110;
    int32_t winH = 0;

    // Set window height
    if(showBestTime) winH += 15;
    int32_t H = 0;
    if(item1) winH += 10;
    if(item2) winH += 10;
    if(item3) winH += 10;
    if(item4) winH += 10;
    winH += 4;  // margin

    int32_t currY = winY;

    // Draw menu window background
    mygame.display.setColor(1,1);
    mygame.display.fillRect(winX, winY, winW, winH);

	currY += 4;  // margin
    mygame.display.setInvisibleColor(1);

    // Print Best time
    if( showBestTime )
    {
        mygame.display.setColor(2,1);
        //currY += 4;
        mygame.display.print(winX+4,currY,"Best: ");
        //currY -= 1;
        DrawLapTime(highscore.bestLap_ms, winX+55, currY, fix16_from_float(1.5) );
        currY += 15;
    }

	// Print menu
	int32_t numItems = 0;
    if(item1)
    {
        if(cursorPos == 0)
            mygame.display.setColor(3,1);
        else
            mygame.display.setColor(2,1);
        mygame.display.print(winX+4, currY, item1);
        currY += 10;
        numItems++;
    }
    if(item2)
    {
        if(cursorPos == 1)
            mygame.display.setColor(3,1);
        else
            mygame.display.setColor(2,1);
        mygame.display.print(winX+4, currY, item2);
        currY += 10;
        numItems++;
    }
    if(item3)
    {
        if(cursorPos == 2)
            mygame.display.setColor(3,1);
        else
            mygame.display.setColor(2,1);
        mygame.display.print(winX+4, currY, item3);
        currY += 10;
        numItems++;
    }
    if(item4)
    {
        if(cursorPos == 3)
            mygame.display.setColor(3,1);
        else
            mygame.display.setColor(2,1);
        mygame.display.print(winX+4, currY, item4);
        currY += 10;
        numItems++;
    }

    // Print cursor
    //mygame.display.print(winX, winY + 4 + (cursorPos*10), ">");

    // Read keys
    if(mygame.buttons.pressed(BTN_UP))
       if(--cursorPos < 0 )
            cursorPos = 0;

    if(mygame.buttons.pressed(BTN_DOWN))
       if(++cursorPos >= numItems )
            cursorPos = numItems-1;

    if(mygame.buttons.pressed(BTN_B))
    {
        return false;
    }
    else
    {
        return true;
    }
}

//
bool HandleStartGameMenu( int32_t lastLap_ms )
{
    //
    int32_t winX = 0;
    int32_t winY = 16;
    int32_t winW = 110;
    int32_t winH = 31;
    if( lastLap_ms > 0 )
		winH += 20;
    int32_t currY = winY;

    // Draw menu window background
    mygame.display.setColor(1,1);
    mygame.display.fillRect(winX, winY, winW, winH);

    // Best time
    mygame.display.setColor(2,1);
    mygame.display.setInvisibleColor(1);
	currY += 4;
    mygame.display.print(winX+5,currY,"BEST: ");
    currY -= 1;
    DrawLapTime(highscore.bestLap_ms, winX+55, currY, fix16_from_float(1.5) );
    currY += 15;

    // Last time
    if( lastLap_ms > 0 )
    {
        DrawLapTime(lastLap_ms, winX+30, currY, fix16_one<<1 );
        currY += 20;
    }

//
    mygame.display.setColor(2,1);
    mygame.display.setInvisibleColor(1);
    mygame.display.print(winX+5,currY,"C TO START");

    // Read keys
    if(mygame.buttons.pressed(BTN_C))
    {
        // Reset game
        fxCamX = fix16_from_int(42);
        fxCamY = fix16_from_int(490);
        fxVel = 0;
        fxAngle = 0;
        fxRotVel = fxInitialRotVel;

        return false;
    }
    else
    {
        return true;
    }
}



// Draw the setup menu and handle keys.
void HandleSetupMenu(int32_t& lastListPos)
{
    const int32_t itemsCount = 4;
    char menuStr[itemsCount][10] =
    {
        "SHIP","EDGE","TERRAIN","T.MODE=0"
    };
    menuStr[3][7] = (char)(textureMode+'0');

    // Draw the list of texture types
    int32_t y = 0;
    mygame.display.setColor(1,2);
    mygame.display.setInvisibleColor(2);
    for(int32_t im = lastListPos-1; im<min(itemsCount, lastListPos+2); im++)
    {
        if(im<0)
        {
            // skip
        }
        else if(im==lastListPos)
        {
             mygame.display.print(12,y,"<");
             mygame.display.print(menuStr[im]);
             mygame.display.print(">");
        }
        else
            mygame.display.print(12,y,menuStr[im]);
        y += 9;
    }

    // Read keys
    int32_t changeDir = 0;
    if(mygame.buttons.pressed(BTN_RIGHT))
        changeDir = 1;
    else if(mygame.buttons.pressed(BTN_LEFT))
        changeDir = -1;
    else if(mygame.buttons.pressed(BTN_UP))
    {
        if(--lastListPos<0)
           lastListPos = 0;
    }
    else if(mygame.buttons.pressed(BTN_DOWN))
    {
        if(++lastListPos>=itemsCount)
           lastListPos = itemsCount-1;
    }

    // Change the texture on the game
    if(changeDir!=0)
    {
        if(lastListPos==0) // ship
        {
             // Switch active bitmap data
            int32_t i=0;
            for(; i< billboard_object_bitmaps_count; i++)
                if(activeShipBitmapData==billboard_object_bitmaps[i])
                    break;
            i+= changeDir;
            if(i>= billboard_object_bitmaps_count)
                i=0;
            if(i < 0)
                i=billboard_object_bitmaps_count-1;
            activeShipBitmapData=billboard_object_bitmaps[i];
            shipBitmapW = *(activeShipBitmapData - 2);
            shipBitmapH = *(activeShipBitmapData - 1);
        }
        else if(lastListPos==1) // edge
        {
            // Switch active tile bitmap data
            int32_t i=0;
            for(; i< edge_bitmaps_count; i+=4)
                if(current_texture_bitmaps[1]==edge_bitmaps[i])
                    break;

            i+=4*changeDir; // next 4 textures

            if(i >= edge_bitmaps_count)
                i=0;
            if(i < 0)
                i=edge_bitmaps_count-4;
            current_texture_bitmaps[1] = edge_bitmaps[i];
            current_texture_bitmaps[2] = edge_bitmaps[i+1];
            current_texture_bitmaps[3] = edge_bitmaps[i+2];
            current_texture_bitmaps[4] = edge_bitmaps[i+3];

            // Get mipmap pointers.
            for( uint32_t i=0; i<current_texture_bitmaps_count; i++)
            {
                current_texture_bitmaps_mm1[i] =  current_texture_bitmaps[i] + (texW * tileH);
                current_texture_bitmaps_mm2[i] =  current_texture_bitmaps[i] + (texW * tileH) + (tileW>>1);
            }
        }
        else if(lastListPos==2) // terrain
        {
            // Switch active tile bitmap data
            int32_t i=0;
            for(; i< terrain_bitmaps_count; i+=4)
                if(current_texture_bitmaps[7]==terrain_bitmaps[i])
                    break;

            i+=4*changeDir; // next 4 textures

            if(i >= terrain_bitmaps_count)
                i=0;
            if(i < 0)
                i=terrain_bitmaps_count-4;
            current_texture_bitmaps[7] = terrain_bitmaps[i];
            current_texture_bitmaps[8] = terrain_bitmaps[i+1];
            current_texture_bitmaps[9] = terrain_bitmaps[i+2];
            current_texture_bitmaps[10] = terrain_bitmaps[i+3];

            // Get mipmap pointers.
            for( uint32_t i=0; i<current_texture_bitmaps_count; i++)
            {
                current_texture_bitmaps_mm1[i] =  current_texture_bitmaps[i] + (texW * tileH);
                current_texture_bitmaps_mm2[i] =  current_texture_bitmaps[i] + (texW * tileH) + (tileW>>1);
            }
        }
        else if(lastListPos==3) // texture mode
        {
            textureMode+=changeDir; // next 4 textures
            if(textureMode<0)
                textureMode = 1;
            if(textureMode>1)
                textureMode = 0;
        }
    }
}

uint8_t GetTileIndex(int32_t tile2PosX, int32_t tile2PosY, fix16_t fxAngle, int32_t getX, int32_t getY)
{
    const uint16_t sceneryH = 16;
    uint8_t* scrptr = mygame.display.getBuffer() + (sceneryH*mygame.display.width); // 8-bit screen buffer
    fix16_t fxStepX = fix16_one;
    const fix16_t fxCos = fix16_cos(fxAngle);
    const fix16_t fxSin = fix16_sin(fxAngle);

    // Move caused by rotation.
    const int32_t reqRotateCenterX = tile2PosX + g_rotatingCenterX;
    const int32_t reqRotateCenterY = tile2PosY + g_rotatingCenterY;
    const fix16_t fxRotatedRotateCenterX = (reqRotateCenterX * fxCos) - (reqRotateCenterY * fxSin);
    const fix16_t fxRotatedRotateCenterY = (reqRotateCenterX * fxSin) + (reqRotateCenterY * fxCos);
    const fix16_t fxRotatedCenterDiffX = fxRotatedRotateCenterX - fix16_from_int(reqRotateCenterX);
    const fix16_t fxRotatedCenterDiffY = fxRotatedRotateCenterY - fix16_from_int(reqRotateCenterY);

    fix16_t fxZ = PerspectiveScaleY[getY];
    fix16_t fxstepXFromY = PerspectiveScaleX[getY];
    fix16_t fxFinalY =  fxZ + fix16_from_int(tile2PosY);

    // *** Step for scaling
    fxStepX = fxstepXFromY >> 7;
    fix16_t fxStepXInU = fix16_mul(fxStepX, fxCos);
    fix16_t fxStepXInV = fix16_mul(fxStepX, fxSin);

     // *** Shear the scanline to move horizontal origo to the middle
    fix16_t fxFinalX = -(fxstepXFromY>>1) + fix16_from_int(tile2PosX);
    fix16_t fxU2 = fix16_mul(fxFinalX, fxCos) - fix16_mul(fxFinalY, fxSin) - fxRotatedCenterDiffX;
    fix16_t fxV2 = fix16_mul(fxFinalX, fxSin) + fix16_mul(fxFinalY, fxCos) - fxRotatedCenterDiffY;

    fxU2 += fxStepXInU * getX;
    fxV2 += fxStepXInV * getX;

    uint32_t finalU = fix16_to_int( fxU2 );
    uint32_t finalV = fix16_to_int( fxV2 );

    // *** Get the tile number from the "map"

    // Raad the game map.
    uint32_t blockDataX = (finalU >> 3);
    uint32_t blockDataY = (finalV >> 3);
    const uint8_t blockMapX = (blockDataX >> 3);
    const uint8_t blockMapY = (blockDataY >> 3);
    uint8_t* tileBitmapPtr;
    uint8_t tileIndex = 0;
    if(blockMapX < mapWidth && blockMapY < mapHeight) {
        const uint8_t blockDataIndex = blockMap[blockMapX+ (blockMapY*mapWidth)];
        // Get the tile.
        tileIndex = blockData[blockDataIndex][(blockDataX & 0x7) + ((blockDataY & 0x7)*8)];
    }
    else
        tileIndex = 0; // Background tile

    //
    return tileIndex;
}

void CreateObject( uint16_t index, fix16_t fxX, fix16_t fxY, const uint8_t* bitmap, fix16_t fxScaledWidth, fix16_t fxScaledHeight)
{
    g_objects3d[index] = new CObject3d();

    g_objects3d[index]->m_fxX = fxX;
    g_objects3d[index]->m_fxY = fxY;
    g_objects3d[index]->m_bitmap = bitmap;
    g_objects3d[index]->m_bitmapW =*(bitmap - 2);
    g_objects3d[index]->m_bitmapH =*(bitmap - 1);
    g_objects3d[index]->m_fxScaledWidth = fxScaledWidth;
    g_objects3d[index]->m_fxScaledHeight = fxScaledHeight;
}

void CreateShip( uint16_t objIndex, uint16_t shipIndex, fix16_t fxX, fix16_t fxY, const uint8_t* bitmap, fix16_t fxScaledWidth, fix16_t fxScaledHeight, fix16_t fxVel)
{
    // Create and initialize ship
    CShip* oppShip = new CShip();
    g_objects3d[objIndex] = oppShip;
    g_ships[shipIndex] = oppShip;
    oppShip->m_fxX = fxX;
    oppShip->m_fxY = fxY;
    oppShip->m_bitmap = bitmap;
    oppShip->m_bitmapW =*(bitmap - 2);
    oppShip->m_bitmapH =*(bitmap - 1);
    oppShip->m_fxScaledWidth = fxScaledWidth;
    oppShip->m_fxScaledHeight = fxScaledHeight;
    oppShip->m_fxVel = fxVel;
    oppShip->m_fxAngle = 0;
    oppShip->m_fxMaxSpeed = fxMaxSpeed;
    oppShip->m_activeWaypointIndex = 0;
}



bool Draw3dObects(fix16_t fxCamPosX, fix16_t fxCamPosY, fix16_t fxAngle)
{
    const fix16_t fxCos = fix16_cos(-fxAngle);
    const fix16_t fxSin = fix16_sin(-fxAngle);
    const fix16_t fxRotCenterX = fix16_from_int(g_rotatingCenterX);
    const fix16_t fxRotCenterY = fix16_from_int(g_rotatingCenterY-6);
    const int32_t horizonY = 0 + sceneryH;
    bool isCollidedToPlayerShip = false;

    for( int32_t i = 0; i < g_drawListMaxCount; i++)
    {
        CObject3d* obj = g_drawList[i];
        if( obj != NULL )
        {
            fix16_t fxX = obj->m_fxX;
            fix16_t fxY = obj->m_fxY;

            // Translate the object to view (camera) domain.
            fxX -= fxCamPosX;
            fxY -= fxCamPosY;

            // Check for fast exit, if the object is too far.
            const int32_t viewFrustumLimit = 500;
            const fix16_t fxViewFrustumLimit = fix16_from_int(viewFrustumLimit);
            if( fxX > fxViewFrustumLimit || fxX < -fxViewFrustumLimit ||
                fxY > fxViewFrustumLimit || fxY < -fxViewFrustumLimit )
            {
                // Object is too far. Do not draw.
                obj->m_fxDistancePot = fix16_max;
            }
            else
            {
                // Rotate around the rotating center.
                fxX -= fxRotCenterX;
                fxY -= fxRotCenterY;
                fix16_t fxRotatedX = fix16_mul(fxX, fxCos) - fix16_mul(fxY, fxSin);
                fix16_t fxRotatedY = fix16_mul(fxX, fxSin) + fix16_mul(fxY, fxCos);
                fxX = fxRotatedX + fxRotCenterX;
                fxY = fxRotatedY + fxRotCenterY;

                obj->m_fxXInView = fxX;
                obj->m_fxYInView = fxY;

                // Calculate distance.
                // Scale down so that it will not overflow
                fix16_t fxX2 = fxX >> 4;
                fix16_t fxY2 = fxY >> 4;
                #ifdef POK_SIM
                //if(abs(fxX2)>fix16_max)
                #endif // POK_SIM
                fix16_t fxDistancePot = fix16_mul(fxX2, fxX2) + fix16_mul(fxY2,fxY2);
                const fix16_t fxViewFrustumDistancePotLimit =
                    fix16_from_int((viewFrustumLimit>>3)*(viewFrustumLimit>>4));
                if( fxDistancePot > fxViewFrustumDistancePotLimit )
                {
                    obj->m_fxDistancePot = fix16_max;  // Too far, do not draw
                }
                else
                {
                    obj->m_fxDistancePot = fxDistancePot;

                    // *** Check collision
                    if( ! isCollidedToPlayerShip )
                    {

                        // Position relative to player ship.
                        fix16_t fxX3 = fxX - fxRotCenterX;
                        fix16_t fxY3 = fxY - fxRotCenterY;
                        // Calculate distance.
                        // Scale down so that it will not overflow
                        fxX3 >>= 4;
                        fxY3 >>= 4;
                        fix16_t fxDistanceFromShipPot = fix16_mul(fxX3, fxX3) + fix16_mul(fxY3,fxY3);
                        const int32_t ShipCollisionLimit = 10;
                        const fix16_t fxShipCollisionDistancePotLimit = fix16_from_int(ShipCollisionLimit*ShipCollisionLimit);
                        if( fxDistanceFromShipPot < fxShipCollisionDistancePotLimit>>8 )
                            isCollidedToPlayerShip = true;
                    }
                }

            }  // end if

        }  // end if

        // Project and draw previous object, if not the first item in the list and the previous object is not too far (invisible).
        if( i > 0 && g_drawList[i-1]->m_fxDistancePot != fix16_max )
        {
            // If this object is farther than the previous one. Swap (bubble sort) before drawing.
            CObject3d* drawablePrevObj = g_drawList[i-1];
            if( obj != NULL && obj->m_fxDistancePot > drawablePrevObj->m_fxDistancePot )
            {
                // Swap
                g_drawList[i-1] = obj;
                g_drawList[i] = drawablePrevObj;
            }

            // * Project 3D to 2D

            // Get the object bitmap size
            const uint8_t* bitmapData = drawablePrevObj->m_bitmap;
            uint32_t bitmapW = drawablePrevObj->m_bitmapW;
            uint32_t bitmapH = drawablePrevObj->m_bitmapH;
            fix16_t fxScaledWidth = drawablePrevObj->m_fxScaledWidth;
            fix16_t fxScaledHeight = drawablePrevObj->m_fxScaledHeight;

            // Bottom left corner
            fix16_t fx3dX = drawablePrevObj->m_fxXInView;
            fix16_t fx3dZ = drawablePrevObj->m_fxYInView;
            fix16_t fx3dY = fix16_from_int(-34.0);
            const int32_t perspectiveScaleFactor = 115.0;
            fix16_t  fxZFactor = fix16_div( fix16_from_int(perspectiveScaleFactor), fx3dZ );
            fix16_t  fxScreenBlX = fix16_mul(fx3dX, fxZFactor );
            fix16_t  fxScreenBlY = fix16_mul( fx3dY, fxZFactor );

            // Top right corner
            fx3dX += fxScaledWidth;
            fix16_t  fxScreenTrX = fix16_mul(fx3dX, fxZFactor );
            fix16_t  fxScreenTrY = fix16_mul( fx3dY + fxScaledHeight, fxZFactor );

            // Draw scaled bitmap
            int32_t scaledWidth = fix16_to_int((fxScreenTrX - fxScreenBlX));
            int32_t scaledHeight = fix16_to_int((fxScreenTrY - fxScreenBlY));
            DrawScaledBitmap8bit(
                fix16_to_int(fxScreenBlX) + 63 - (scaledWidth>>1), horizonY -screenShiftY- fix16_to_int(fxScreenBlY) - scaledHeight,
                bitmapData,
                bitmapW, bitmapH, scaledWidth, scaledHeight );

        }  // end if

        // end of list?
         if( obj == NULL ) break;

    }  // end for

    return isCollidedToPlayerShip;
}


