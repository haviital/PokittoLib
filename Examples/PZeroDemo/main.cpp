#include "Pokitto.h"
#include "fix16.h"
#include "main.h"
#include "gfx_hdr/GameData.h"
#include "ship.h"
#include "playerShip.h"
#include "menu.h"


Pokitto::Core mygame;
Pokitto::Sound snd;

// Prototypes
void InitGameObjectsForTrack1(bool isRace);
void HandleSetupMenu(int32_t& lastListPos);
void DrawMode7(int32_t tile2PosX, int32_t tile2PosY, fix16_t fxAngle);
uint8_t GetTileIndex(int32_t tile2PosX, int32_t tile2PosY, fix16_t fxAngle, int32_t getX, int32_t getY);
bool Draw3dObects(fix16_t fxPosX, fix16_t fxPosY, fix16_t fxAngle);
bool HandleStartGameMenu( int32_t lastLap_ms );
bool HandleGenericMenu( bool showBestTime, int32_t& /*in out */ cursorPos, char* item1, char* item2, char* item3, char* item4);

const int32_t KRotCenterX = 0;
const int32_t KRotCenterY = -44;

// 3d lookup tables
// z = (zs * h) / y
fix16_t PerspectiveScaleY[screenH];
fix16_t PerspectiveScaleX[screenH];

// Camera pos
//fix16_t fxX = fix16_from_int(55);
fix16_t fxCamX = fix16_from_int(42);
fix16_t fxCamY = fix16_from_int(490);

// Ship bitmap
const uint8_t* activeShipBitmapData = billboard_object_bitmaps[0];
uint32_t shipBitmapW = *(activeShipBitmapData - 2);
uint32_t shipBitmapH = *(activeShipBitmapData - 1);

uint32_t g_frameNum = 0;
bool g_isRace = true;

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

// Reserve space for objects in RAM.
CObject3d g_BillboardObjectArray[3*8];
CShip g_ShipObjectArray[1*8];
CPlayerShip g_playerShip;

// Ships
const uint32_t g_shipsMaxCount = 10;
uint32_t g_shipCount = 0;
CShip* g_ships[ g_objects3dMaxCount ] = {0};

// Create an instance of cookie.
mycookie highscore;

// Main
int main () {

    // Initialize variables.
    bool prevCollided = false;
    bool isManualRotation = true;
    bool isStartup = true;
    //bool isSetupMenuActive = false;
    int32_t lastSetupListPos = 1;
    CMenu menu;

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
    InitGameObjectsForTrack1(g_isRace);

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

    //!!HV
    uint32_t tempo = 60;
    samplespertick = (float)((60.0f/(float)tempo)*POK_AUD_FREQ)/16;
    emptySong();
    emptyBlocks();
    initStreams(0);
    setOSC(&osc1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
    setOSC(&osc2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
    setOSC(&osc3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
    track1on = true;
    track2on = false;
    track3on = false;
    makeSampleInstruments();

//         setOSC(&osc1,1,patch[i].wave,patch[i].loop, patch[i].echo, patch[i].adsr,
//                *song.note_stream[0],patch[i].vol,
//                patch[i].attack, patch[i].decay, patch[i].sustain,patch[i].release,
//                patch[i].maxbend, patch[i].bendrate, patch[i].arpmode, patch[i].overdrive, patch[i].kick );
//

    // Instruments

    int32_t p1 = 1;
    patch[p1].wave = WSQUARE;
    patch[p1].on = 1;
    patch[p1].vol = 143;
    patch[p1].loop = 1;
    patch[p1].echo = 0;

    patch[p1].adsr = 0;
    patch[p1].attack = 0;
    patch[p1].decay = 0;
    patch[p1].sustain = 0;
    patch[p1].release = 0;

    patch[p1].maxbend = 0;
    patch[p1].bendrate = 0;
    patch[p1].arpmode = 2;
    patch[p1].overdrive = 1;
    patch[p1].kick = 0;

    p1 = 0;
    patch[p1].wave = WSAW;
    patch[p1].on = 1;
    patch[p1].vol = 180;
    patch[p1].loop = 1;
    patch[p1].echo = 1;

    patch[p1].adsr = 0;
    patch[p1].attack = 0;
    patch[p1].decay = 5;
    patch[p1].sustain = 0;
    patch[p1].release = 0;

    patch[p1].maxbend = 0;
    patch[p1].bendrate = 0;
    patch[p1].arpmode = 0;
    patch[p1].overdrive = 1;
    patch[p1].kick = 1;

    p1 = 0;
    patch[p1].wave = WSQUARE;
    patch[p1].on = 1;
    patch[p1].vol = 143;
    patch[p1].loop = 1;
    patch[p1].echo = 0;

    patch[p1].adsr = 0;
    patch[p1].attack = 158;
    patch[p1].decay = 44;
    patch[p1].sustain = 230;
    patch[p1].release = 48;

    patch[p1].maxbend = 0;
    patch[p1].bendrate = 0;
    patch[p1].arpmode = 0;
    patch[p1].overdrive = 1;
    patch[p1].kick = 0;

    //const int32_t songLenght = 64;
    const int32_t songLenght = 7;
    const uint8_t pitch[songLenght] =
    {
//        40, 40, 40, 40, 40, 40, 40, 40, 40, 40,
//        40, 40, 40, 40, 40, 40, 40, 40, 40, 40,
//        40, 40, 40, 40, 40, 40, 40, 40, 40, 40,
//        40, 40, 40, 40, 40, 40, 40, 40, 40, 40,
//        40, 40, 40, 40, 40, 40, 40, 40, 40, 40,
//        40, 40, 40, 40, 40, 40, 40, 40, 40, 40,
//        40, 40, 40, 40,
//        40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
//        50, 51, 52, 53, 54, 55, 56, 57, 58, 59,
//        60, 61, 62, 63, 64, 65, 66, 67, 68, 69,
//        70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
//        80, 81, 82, 83, 84, 85, 86, 87, 88, 89,
//        90, 91, 92, 93, 94, 95, 96, 97, 98, 99,
//        100, 101, 102, 103,

//        70, 255,  255, 255, 255, 255, 255, 255, 255, 255,
//        73, 255,  255, 255, 255, 255, 255, 255, 255, 255,
//        76, 255,  255, 255, 255, 255, 255, 255, 255, 255,
//        70, 255,  70, 255, 70, 255, 70, 255, 70, 255,
//        73, 255,  73, 255, 73, 255, 73, 255, 73, 255,
//        76, 255,  76, 255, 76, 255, 76, 255, 76, 255,
//        255,255,  255, 255,
//        73, 255,  73, 255, 73, 255, 73, 255, 73, 255,
//        70, 255,  70, 255, 70, 255, 70, 255, 70, 255,
//        73, 255,  73, 255, 73, 255, 73, 255, 73, 255,
//        73, 255,  73, 255, 73, 255, 73, 255, 73, 255,
//        70, 255,  70, 255, 70, 255, 70, 255, 70, 255,
//        73, 255,  73, 255, 73, 255, 73, 255, 73, 255,
//        255,255,  255, 255,
        47,255,255,255,255,255,47,
    };
    const uint8_t patch[songLenght] =
    {
//        1,0,1,0,1,0,1,0,1,0,
//        1,0,1,0,1,0,1,0,1,0,
//        1,0,1,0,1,0,1,0,1,0,
//        1,0,1,0,1,0,1,0,1,0,
//        1,0,1,0,1,0,1,0,1,0,
//        1,0,1,0,1,0,1,0,1,0,
//        0,0,0,0,
        1,0,0,0,0,0,1,
    };


    const uint8_t pitchAndPatch_old[][2] =
    {
        {77, 1},{255, 0},
        {57, 1},{255, 0},{255, 0},{255, 0},
        {77, 1},{255, 0},
        {57, 1},{255, 0},{255, 0},{255, 0},
        {77, 1},{255, 0},{255, 0},{255, 0},
        {87, 1},{255, 0},{255, 0},{255, 0},
        {97, 1},{255, 0},{255, 0},{255, 0},
        {107, 1},{255, 0},{255, 0},{255, 0},
        {117, 1},{255, 0},{255, 0},{255, 0},
        {127, 1},{255, 0},{255, 0},{255, 0},
        {137, 1},{255, 0},{255, 0},{255, 0},
        {147, 1},{255, 0},{255, 0},{255, 0},
        {157, 1},{255, 0},{255, 0},{255, 0},
    };

    const uint8_t pitchAndPatch[][2] =
    {
        {60, 1},{255, 0},{255, 0},{255, 0},
        {61, 1},{255, 0},{255, 0},{255, 0},
        {62, 1},{255, 0},{255, 0},{255, 0},
        {63, 1},{255, 0},{255, 0},{255, 0},
        {64, 1},{255, 0},{255, 0},{255, 0},
        {65, 1},{255, 0},{255, 0},{255, 0},
    };


    const int32_t channel = 0;
    uint8_t songLen2 = sizeof(pitchAndPatch) / sizeof(pitchAndPatch[0]);
    for(int32_t i=0; i<songLen2;i++)
    {
        block[0].notenumber[i] = pitchAndPatch[i][0];
        block[0].instrument[i] = pitchAndPatch[i][1];
    }

    //initStreams(0);
    song.song_end=0; // last block
    song.song_loop=0; // loop back to
    //song.song_loop = loopTo; //legacy synth code
    //streamsFunction ptr = (streamsFunction)&Tracker::initStreams;
    //registerStreamsCallback(ptr);

    playing=true;

    snd.ampEnable(1);


    // *** The game loop
    while (mygame.isRunning()) {

        if (mygame.update()) {

            fix16_t fxCamAngle = g_playerShip.m_fxAngle - (fix16_pi>>1);

            fix16_t fxCos = fix16_cos(fxCamAngle);
            fix16_t fxSin = fix16_sin(fxCamAngle);

            // Draw sky
            fix16_t fxAnglePositive =  ((fxCamAngle) % (fix16_pi<<1)) +  (fix16_pi<<1);
            int16_t skyX = ((fxAnglePositive>>9) % 22);
            uint16_t skyW = image_sky_long[0];
            uint16_t skyH = image_sky_long[1];
            const uint8_t* skyBitmapPtr = &(image_sky_long[2]);
            DrawBitmapOpaque8bit( skyX-22, 0, skyBitmapPtr, skyW, skyH );

            // ** Draw the road and edges and terrain.
            fix16_t fxCamX = g_playerShip.m_fxX;
            fix16_t fxCamY = g_playerShip.m_fxY;
            fxCamY += -g_playerShip.m_fxCameraBehindPlayerCurrent;
            DrawMode7( fix16_to_int(fxCamX), fix16_to_int(fxCamY), fxCamAngle);

            // Draw 3d objects and check collisions.
            g_playerShip.m_isCollidedToPlayerShip = Draw3dObects(fxCamX, fxCamY, fxCamAngle);

            // Draw the current lap time
            int32_t lapStartX = 110-5;  // 5 pixel margin
            lapStartX -= 5*6; // 5 chars
            DrawLapTime(g_playerShip.m_current_lap_time_ms, lapStartX, 1, fix16_one );

            // Draw the current rank
            if( g_isRace )
                DrawRankNumber(1, 1);

            // Print coordinates on screen
            #if 0
            char text[128];
            mygame.display.setColor(9,3);
            mygame.display.print(0,0, itoa(fix16_to_int(fxCamX),text,10)); mygame.display.print(", ");
            mygame.display.print(itoa(fix16_to_int(fxCamY)+65,text,10)); mygame.display.print("     ");
            mygame.display.print(0,10, itoa(g_playerShip.m_currentRank,text,10)); mygame.display.print(", ");
            mygame.display.print( itoa(g_playerShip.m_activeLapNum,text,10) ); mygame.display.print(", ");
            mygame.display.print( itoa(g_playerShip.m_trackIndex,text,10) ); mygame.display.print("     ");
            #endif

            // Handle menus
            CMenu::MenuMode requestedMenuMode =  g_playerShip.m_requestedMenuMode;
            if( isStartup )
                requestedMenuMode =  CMenu::MenuMode::enumMainMenu;
            menu.HandleMenus(g_isRace, highscore.bestLap_ms, requestedMenuMode );
            g_playerShip.m_requestedMenuMode = CMenu::MenuMode::enumNoMenu;
            isStartup = false;

            if(!menu.m_isOpen)
            {
                // No menu opean

                // Race is ongoing. Move all the ships
                if( g_isRace )
                {
                    for(int32_t i=0; i < g_shipCount; i++)
                    {
                        g_ships[i]->Update();
                    }
                }
                else
                {
                    // Time trial. Only update the player ship.
                    g_playerShip.Update();
                }
            }

            g_frameNum++;

        }  // end if
    }  // end while
}


void ResetGame(bool isRace_)
{
     // Reset game

    g_isRace = isRace_;

    // Init game object.
    InitGameObjectsForTrack1(g_isRace);

    if( g_isRace )
    {
        for(int32_t i=0; i < g_shipCount; i++)
        {
            g_ships[i]->Reset();
        }
    }
    else
    {
        g_playerShip.Reset();
    }

}

// Draw rank number
void DrawRankNumber(int32_t x, int32_t y)
{
    // Draw the current rank
    int32_t playerRank = g_playerShip.m_currentRank;
    if(playerRank>0)
    {
        fix16_t fxScaleFactor = fix16_from_float(1.5);
        const uint8_t* numbersBitmapPtr = &(image_numbers[2]);
        uint32_t numbersBitmapW = 6, numbersBitmapH = 7;
        DrawScaledBitmap8bit(x, y, numbersBitmapPtr + (numbersBitmapW * numbersBitmapH)*playerRank,
                             numbersBitmapW, numbersBitmapH,
                             fix16_to_int(numbersBitmapW*fxScaleFactor),
                             fix16_to_int(numbersBitmapH*fxScaleFactor) );
    }
}

// Init game objects
void InitGameObjectsForTrack1(bool isRace)
{
    #if 1
    // Copy cactus and rock array pointers to the object list.
    int32_t billboardObjectCount = 2*8;
    if( !g_isRace )
        billboardObjectCount = 3*8;

    int32_t i = 0;
    for(; i < billboardObjectCount; i++ )
    {
        g_objects3d[i] = &g_BillboardObjectArray[i];
        g_objects3d[i]->m_fxX = g_timeTrialBilboardObjectsInRom_track1[i].m_fxX;
        g_objects3d[i]->m_fxY = g_timeTrialBilboardObjectsInRom_track1[i].m_fxY;
        g_objects3d[i]->m_bitmap = g_timeTrialBilboardObjectsInRom_track1[i].m_bitmap;
        g_objects3d[i]->m_bitmapW = g_timeTrialBilboardObjectsInRom_track1[i].m_bitmapW;
        g_objects3d[i]->m_bitmapH = g_timeTrialBilboardObjectsInRom_track1[i].m_bitmapH;
        g_objects3d[i]->m_fxScaledWidth = g_timeTrialBilboardObjectsInRom_track1[i].m_fxScaledWidth;
        g_objects3d[i]->m_fxScaledHeight = g_timeTrialBilboardObjectsInRom_track1[i].m_fxScaledHeight;
    }
    #else
    // Copy waypoints
    const uint8_t* spot_bm = billboard_object_bitmaps[27];
    const int16_t spotBmW  = *(spot_bm - 2);
    const int16_t spotBmH  = *(spot_bm - 1);
    const fix16_t fxSpotScaledW  = fix16_from_int(spotBmW);
    const fix16_t fxSpotScaledH  = fix16_from_int(spotBmH);

    for(int32_t i = 0; i < 2*8; i++ )
    {
        int16_t w = i + 6;
        if(w>=waypointCount)
            w = waypointCount - 1;
        g_objects3d[i] = &g_BillboardObjectArray[i];
        g_objects3d[i]->m_fxX = fix16_from_int(waypoints[w].x);
        g_objects3d[i]->m_fxY = fix16_from_int(waypoints[w].y);
        g_objects3d[i]->m_bitmap = spot_bm;
        g_objects3d[i]->m_bitmapW = spotBmW;
        g_objects3d[i]->m_bitmapH = spotBmH;
        g_objects3d[i]->m_fxScaledWidth = fxSpotScaledW;
        g_objects3d[i]->m_fxScaledHeight = ( waypoints[w].fxTargetSpeed == fxDefaultOtherShipSpeedInCorner ) ? fxSpotScaledH>>1 : fxSpotScaledH;
    }
    #endif

    int32_t ii = 0;
    if( !g_isRace )
    {
        // Time trial

        // Player Ship
        g_shipCount = 0;
        ii = i;
        g_objects3d[ii] = &g_playerShip;
        fix16_t fxScaledSizeFactor = fix16_from_float(0.65);
        g_playerShip.m_bitmap = billboard_object_bitmaps[0];
        g_playerShip.m_bitmapW = *(g_objects3d[ii]->m_bitmap - 2);
        g_playerShip.m_bitmapH = *(g_objects3d[ii]->m_bitmap - 1);
        g_playerShip.m_fxScaledWidth = g_objects3d[ii]->m_bitmapW * fxScaledSizeFactor;
        g_playerShip.m_fxScaledHeight = g_objects3d[ii]->m_bitmapH * fxScaledSizeFactor;
        g_playerShip.m_fxVel = 0;
        g_playerShip.m_fxAcc = 0;
        g_playerShip.m_fxDeacc = 0;
        g_playerShip.m_fxRotVel = 0;
        g_playerShip.m_fxMaxSpeed = 0;
        g_playerShip.m_fxCornerSpeed1 = 0;
        g_playerShip.m_fxCornerSpeed2 = 0;
        g_playerShip.m_fxWaypointTargetSpeed = 0;
        g_playerShip.m_fxAngle = fix16_pi>>1;
        g_playerShip.m_activeWaypointIndex = 0;
        g_playerShip.Reset();

    }
    else
    {
        // Race

        // Copy ship array pointers to the object list.
        g_shipCount = 7;
        //g_shipCount = 1;
        fix16_t fxScaledSizeFactor = fix16_from_float(0.65);

        // Player Ship
        int32_t i=0;
        ii = i + (2*8);
        g_objects3d[ii] = &g_playerShip;
        g_objects3d[ii]->m_bitmap = billboard_object_bitmaps[0];
        g_objects3d[ii]->m_bitmapW = *(g_objects3d[ii]->m_bitmap - 2);
        g_objects3d[ii]->m_bitmapH = *(g_objects3d[ii]->m_bitmap - 1);
        g_objects3d[ii]->m_fxScaledWidth = g_objects3d[ii]->m_bitmapW * fxScaledSizeFactor;
        g_objects3d[ii]->m_fxScaledHeight = g_objects3d[ii]->m_bitmapH * fxScaledSizeFactor;
        g_ships[i] = &g_playerShip;
        g_ships[i]->m_fxVel = 0;
        g_ships[i]->m_fxAcc = 0;
        g_ships[i]->m_fxDeacc = 0;
        g_ships[i]->m_fxRotVel = 0;
        g_ships[i]->m_fxMaxSpeed = 0;
        g_ships[i]->m_fxCornerSpeed1 = 0;
        g_ships[i]->m_fxCornerSpeed2 = 0;
        g_ships[i]->m_fxWaypointTargetSpeed = 0;
        g_ships[i]->m_fxAngle = fix16_pi>>1;
        g_ships[i]->m_activeWaypointIndex = 0;
        g_playerShip.Reset();

        // Ship 1: fast in streight road, slow in corners
        i=1;
        ii = i + (2*8);
        g_objects3d[ii] = &g_ShipObjectArray[i];
        g_objects3d[ii]->m_fxX = fix16_from_int(35);
        g_objects3d[ii]->m_fxY = fix16_from_int(600+(2*50));
        g_objects3d[ii]->m_bitmap = billboard_object_bitmaps[17];
        g_objects3d[ii]->m_bitmapW = *(g_objects3d[ii]->m_bitmap - 2);
        g_objects3d[ii]->m_bitmapH = *(g_objects3d[ii]->m_bitmap - 1);
        g_objects3d[ii]->m_fxScaledWidth = g_objects3d[ii]->m_bitmapW * fxScaledSizeFactor;
        g_objects3d[ii]->m_fxScaledHeight = g_objects3d[ii]->m_bitmapH * fxScaledSizeFactor;
        g_ships[i] = &g_ShipObjectArray[i];
        g_ships[i]->m_fxVel = 0;
        g_ships[i]->m_fxAcc = fix16_from_float(0.200);
        g_ships[i]->m_fxDeacc = fix16_from_float(3.0);
        g_ships[i]->m_fxRotVel = fix16_pi / 100;
        g_ships[i]->m_fxMaxSpeed = fxDefaultOtherShipSpeed;
        g_ships[i]->m_fxCornerSpeed1 = fxDefaultOtherShipSpeedInCorner;
        g_ships[i]->m_fxCornerSpeed2 = fxDefaultOtherShipSpeedInSlowCorner;
        g_ships[i]->m_fxWaypointTargetSpeed = g_ships[i]->m_fxMaxSpeed;
        g_ships[i]->m_fxAngle = fix16_pi>>1;
        g_ships[i]->m_activeWaypointIndex = 0;

        // Ship 2: slow in streight road, fast in corners
        i=2;
        ii = i + (2*8);
        g_objects3d[ii] = &g_ShipObjectArray[i];
        g_objects3d[ii]->m_fxX = fix16_from_int(35+50);
        g_objects3d[ii]->m_fxY = fix16_from_int(600+(2*50));
        g_objects3d[ii]->m_bitmap = billboard_object_bitmaps[13];
        g_objects3d[ii]->m_bitmapW = *(g_objects3d[ii]->m_bitmap - 2);
        g_objects3d[ii]->m_bitmapH = *(g_objects3d[ii]->m_bitmap - 1);
        g_objects3d[ii]->m_fxScaledWidth = g_objects3d[ii]->m_bitmapW * fxScaledSizeFactor;
        g_objects3d[ii]->m_fxScaledHeight = g_objects3d[ii]->m_bitmapH * fxScaledSizeFactor;
        g_ships[i] = &g_ShipObjectArray[i];
        g_ships[i]->m_fxVel = 0;
        g_ships[i]->m_fxAcc = fix16_from_float(0.200);
        g_ships[i]->m_fxDeacc = fix16_from_float(3.0);
        g_ships[i]->m_fxRotVel = fix16_pi / 50;
        g_ships[i]->m_fxMaxSpeed = fix16_mul(fxDefaultOtherShipSpeed, fix16_from_float(0.7) );
        g_ships[i]->m_fxCornerSpeed1 = fix16_mul(fxDefaultOtherShipSpeedInCorner, fix16_from_float(1.2) );
        g_ships[i]->m_fxCornerSpeed2 = fix16_mul(fxDefaultOtherShipSpeedInSlowCorner, fix16_from_float(1.2) );
        g_ships[i]->m_fxWaypointTargetSpeed = g_ships[i]->m_fxMaxSpeed;
        g_ships[i]->m_fxAngle = fix16_pi>>1;
        g_ships[i]->m_activeWaypointIndex = 0;

        // Ship 3: slow in streight road, fast in corners
        i=3;
        ii = i + (2*8);
        g_objects3d[ii] = &g_ShipObjectArray[i];
        g_objects3d[ii]->m_fxX = fix16_from_int(35 + 50);
        g_objects3d[ii]->m_fxY = fix16_from_int(600+50);
        g_objects3d[ii]->m_bitmap = billboard_object_bitmaps[1];
        g_objects3d[ii]->m_bitmapW = *(g_objects3d[ii]->m_bitmap - 2);
        g_objects3d[ii]->m_bitmapH = *(g_objects3d[ii]->m_bitmap - 1);
        g_objects3d[ii]->m_fxScaledWidth = g_objects3d[ii]->m_bitmapW * fxScaledSizeFactor;
        g_objects3d[ii]->m_fxScaledHeight = g_objects3d[ii]->m_bitmapH * fxScaledSizeFactor;
        g_ships[i] = &g_ShipObjectArray[i];
        g_ships[i]->m_fxVel = 0;
        g_ships[i]->m_fxAcc = fix16_from_float(0.200);
        g_ships[i]->m_fxDeacc = fix16_from_float(3.0);
        g_ships[i]->m_fxRotVel = fix16_pi / 50;
        g_ships[i]->m_fxMaxSpeed = fix16_mul(fxDefaultOtherShipSpeed, fix16_from_float(0.6) );
        g_ships[i]->m_fxCornerSpeed1 = fix16_mul(fxDefaultOtherShipSpeedInCorner, fix16_from_float(1.2) );
        g_ships[i]->m_fxCornerSpeed2 = fix16_mul(fxDefaultOtherShipSpeedInSlowCorner, fix16_from_float(1.2) );
        g_ships[i]->m_fxWaypointTargetSpeed = g_ships[i]->m_fxMaxSpeed;
        g_ships[i]->m_fxAngle = fix16_pi>>1;
        g_ships[i]->m_activeWaypointIndex = 0;


        // Ship 4: fast in streight road, slow in corners
        i=4;
        ii = i + (2*8);
        g_objects3d[ii] = &g_ShipObjectArray[i];
        g_objects3d[ii]->m_fxX = fix16_from_int(35);
        g_objects3d[ii]->m_fxY = fix16_from_int(600+50);
        g_objects3d[ii]->m_bitmap = billboard_object_bitmaps[9];
        g_objects3d[ii]->m_bitmapW = *(g_objects3d[ii]->m_bitmap - 2);
        g_objects3d[ii]->m_bitmapH = *(g_objects3d[ii]->m_bitmap - 1);
        g_objects3d[ii]->m_fxScaledWidth = g_objects3d[ii]->m_bitmapW * fxScaledSizeFactor;
        g_objects3d[ii]->m_fxScaledHeight = g_objects3d[ii]->m_bitmapH * fxScaledSizeFactor;
        g_ships[i] = &g_ShipObjectArray[i];
        g_ships[i]->m_fxVel = 0;
        g_ships[i]->m_fxAcc = fix16_from_float(0.200);
        g_ships[i]->m_fxDeacc = fix16_from_float(3.0);
        g_ships[i]->m_fxRotVel = fix16_pi / 100;
        g_ships[i]->m_fxMaxSpeed = fix16_mul(fxDefaultOtherShipSpeed, fix16_from_float(0.9) );
        g_ships[i]->m_fxCornerSpeed1 = fxDefaultOtherShipSpeedInCorner;
        g_ships[i]->m_fxCornerSpeed2 = fxDefaultOtherShipSpeedInSlowCorner;
        g_ships[i]->m_fxWaypointTargetSpeed = g_ships[i]->m_fxMaxSpeed;
        g_ships[i]->m_fxAngle = fix16_pi>>1;
        g_ships[i]->m_activeWaypointIndex = 0;

        // Ship 5: slow in streight road, fast in corners
        i=5;
        ii = i + (2*8);
        g_objects3d[ii] = &g_ShipObjectArray[i];
        g_objects3d[ii]->m_fxX = fix16_from_int(35 + 50);
        g_objects3d[ii]->m_fxY = fix16_from_int(600);
        g_objects3d[ii]->m_bitmap = billboard_object_bitmaps[20];
        g_objects3d[ii]->m_bitmapW = *(g_objects3d[ii]->m_bitmap - 2);
        g_objects3d[ii]->m_bitmapH = *(g_objects3d[ii]->m_bitmap - 1);
        g_objects3d[ii]->m_fxScaledWidth = g_objects3d[ii]->m_bitmapW * fxScaledSizeFactor;
        g_objects3d[ii]->m_fxScaledHeight = g_objects3d[ii]->m_bitmapH * fxScaledSizeFactor;
        g_ships[i] = &g_ShipObjectArray[i];
        g_ships[i]->m_fxVel = 0;
        g_ships[i]->m_fxAcc = fix16_from_float(0.200);
        g_ships[i]->m_fxDeacc = fix16_from_float(3.0);
        g_ships[i]->m_fxRotVel = fix16_pi / 50;
        g_ships[i]->m_fxMaxSpeed = fix16_mul(fxDefaultOtherShipSpeed, fix16_from_float(0.4) );
        g_ships[i]->m_fxCornerSpeed1 = fix16_mul(fxDefaultOtherShipSpeedInCorner, fix16_from_float(1.2) );
        g_ships[i]->m_fxCornerSpeed2 = fix16_mul(fxDefaultOtherShipSpeedInSlowCorner, fix16_from_float(1.2) );
        g_ships[i]->m_fxWaypointTargetSpeed = g_ships[i]->m_fxMaxSpeed;
        g_ships[i]->m_fxAngle = fix16_pi>>1;
        g_ships[i]->m_activeWaypointIndex = 0;


        // Ship 6: fast in streight road, slow in corners
        i=6;
        ii = i + (2*8);
        g_objects3d[ii] = &g_ShipObjectArray[i];
        g_objects3d[ii]->m_fxX = fix16_from_int(35);
        g_objects3d[ii]->m_fxY = fix16_from_int(600);
        g_objects3d[ii]->m_bitmap = billboard_object_bitmaps[6];
        g_objects3d[ii]->m_bitmapW = *(g_objects3d[ii]->m_bitmap - 2);
        g_objects3d[ii]->m_bitmapH = *(g_objects3d[ii]->m_bitmap - 1);
        g_objects3d[ii]->m_fxScaledWidth = g_objects3d[ii]->m_bitmapW * fxScaledSizeFactor;
        g_objects3d[ii]->m_fxScaledHeight = g_objects3d[ii]->m_bitmapH * fxScaledSizeFactor;
        g_ships[i] = &g_ShipObjectArray[i];
        g_ships[i]->m_fxVel = 0;
        g_ships[i]->m_fxAcc = fix16_from_float(0.200);
        g_ships[i]->m_fxDeacc = fix16_from_float(3.0);
        g_ships[i]->m_fxRotVel = fix16_pi / 100;
        g_ships[i]->m_fxMaxSpeed = fix16_mul(fxDefaultOtherShipSpeed, fix16_from_float(0.6) );
        g_ships[i]->m_fxCornerSpeed1 = fxDefaultOtherShipSpeedInCorner;
        g_ships[i]->m_fxCornerSpeed2 = fxDefaultOtherShipSpeedInSlowCorner;
        g_ships[i]->m_fxWaypointTargetSpeed = g_ships[i]->m_fxMaxSpeed;
        g_ships[i]->m_fxAngle = fix16_pi>>1;
        g_ships[i]->m_activeWaypointIndex = 0;

    }

   // Set the object count.
    g_objects3dCount = ii + 1;

    // Setup draw list
    static_assert( g_objects3dMaxCount <= g_drawListMaxCount, "error");
    for( int32_t i = 0; i < g_drawListMaxCount; i++)
    {
        if(i < g_objects3dCount)
            g_drawList[i] = g_objects3d[i];
        else
            g_drawList[i] = NULL;
    }

}

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
        g_playerShip.Reset();

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

uint8_t GetTileIndexCommon(int32_t posX, int32_t posY)
{
    // *** Get the tile number from the "map"

    // Raad the game map.
    uint32_t blockDataX = (posX >> 3);
    uint32_t blockDataY = (posY >> 3);
    const uint8_t blockMapX = (blockDataX >> 3);
    const uint8_t blockMapY = (blockDataY >> 3);
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

bool Draw3dObects(fix16_t fxCamPosX, fix16_t fxCamPosY, fix16_t fxAngle)
{
    const fix16_t fxCos = fix16_cos(-fxAngle);
    const fix16_t fxSin = fix16_sin(-fxAngle);
    const fix16_t fxRotCenterX = g_playerShip.m_fxX - fxCamPosX;
    const fix16_t fxRotCenterY = g_playerShip.m_fxY - fxCamPosY;
    const int32_t horizonY = 0 + sceneryH;
    bool isCollidedToPlayerShip = false;

    for( int32_t i = 0; i < g_drawListMaxCount; i++)
    {
         CObject3d* obj = g_drawList[i];


        if( obj != NULL )
        {
            //!!HV
            if(i>15)
                obj = g_drawList[i];

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
                    if( ! isCollidedToPlayerShip && obj != &g_playerShip )  // Only if not already collided and obj is not the player ship itself
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

            //
            int32_t scaledWidth = fix16_to_int((fxScreenTrX - fxScreenBlX));
            int32_t scaledHeight = fix16_to_int((fxScreenTrY - fxScreenBlY));

            // Draw shadow
            if( drawablePrevObj == &g_playerShip )
            {
                uint16_t shadowW = image_shadow[0];
                uint16_t shadowH = image_shadow[1];
                uint16_t shadowScreenOffsetY = 5;
                const uint8_t* shadowBitmapPtr = &(image_shadow[2]);
                DrawScaledBitmap8bit(
                    fix16_to_int(fxScreenBlX) + 63 - (scaledWidth>>1), horizonY -screenShiftY- fix16_to_int(fxScreenBlY) - scaledHeight + shadowScreenOffsetY,
                    shadowBitmapPtr,
                    shadowW, shadowH, scaledWidth, scaledHeight );
            }

            // Draw scaled bitmap
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


