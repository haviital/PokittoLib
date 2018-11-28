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
void DrawMode7(int32_t tile2PosX, int32_t tile2PosY, fix16_t fxAngle);
uint8_t GetTileIndex(int32_t tile2PosX, int32_t tile2PosY, fix16_t fxAngle, int32_t getX, int32_t getY);
bool Draw3dObects(fix16_t fxPosX, fix16_t fxPosY, fix16_t fxAngle);
void InitMusicPlayer();

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

uint8_t* blockMapRAM = NULL;

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
    bool isStartup = true;
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

    // Initialize the music player
    InitMusicPlayer();
    snd.ampEnable(1);

    uint32_t noteIndex = 0;
    int notenumber = 50;

    // *** The game loop
    while (mygame.isRunning()) {

        if (mygame.update()) {

            fix16_t fxCamAngle = g_playerShip.m_fxAngle - (fix16_pi>>1);

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
            {
                requestedMenuMode =  CMenu::MenuMode::enumStartupEmptyMenu;
                SetupMusic(0);
            }

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

    return 0;
}


void InitMusicPlayer()
{
    // Set tempo
    uint32_t tempo = 45;
    samplespertick = (float)((60.0f/(float)tempo)*POK_AUD_FREQ)/16;

    // Reset all
    emptySong();
    emptyBlocks();
    initStreams(0);
    setOSC(&osc1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
    setOSC(&osc2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
    setOSC(&osc3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
    track1on = true;  // Only one simultaneous sound
    track2on = false;
    track3on = false;
    makeSampleInstruments();

    // Song end block.
    song.song_end=0;

    // Loop back to block.
    song.song_loop=0;

    // Setup instruments

    // Instrument 1:  keyboard
    int32_t p1 = 1;
    patch[p1].wave = WSQUARE;
    patch[p1].on = 1;
    patch[p1].vol = 150;
    patch[p1].loop = 0;
    patch[p1].echo = 0;

    patch[p1].adsr = 0;
    patch[p1].attack = 0;
    patch[p1].decay = 102;
    patch[p1].sustain = 62;
    patch[p1].release = 23;

    patch[p1].maxbend = 0;
    patch[p1].bendrate = 0;
    patch[p1].arpmode = 2;
    patch[p1].overdrive = 1;
    patch[p1].kick = 0;

    // Instrument 2:  silence, used for pauses.
    p1 = 2;
    patch[p1].wave = WOFF;
    patch[p1].on = 1;
    patch[p1].vol = 0;
    patch[p1].loop = 0;
    patch[p1].echo = 0;

    patch[p1].adsr = 0;
    patch[p1].attack = 0;
    patch[p1].decay = 0;
    patch[p1].sustain = 0;
    patch[p1].release = 0;

    patch[p1].maxbend = 0;
    patch[p1].bendrate = 0;
    patch[p1].arpmode = 0;
    patch[p1].overdrive = 0;
    patch[p1].kick = 0;


    // This starts the music
    //snd.ampEnable(1);
    //playing = true;

}

void SetupMusic(int32_t songNumber)
{
    // *** Songs

    playing = false; // stop playing during the settings are changed

    // Clear any previous data.
    emptyBlocks();
    setOSC(&osc1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
    setOSC(&osc2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
    setOSC(&osc3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);

    // Reset song.
    emptySong();


    // Fanfare 1
    const uint8_t Fanfare1pitchAndPatch[][2] =
    {
        // yuio-oopo- => 9-,11-,12-,13--,13,13,15-,13-
        {59, 1},{255, 0},
        {61, 1},{255, 0},
        {62, 1},{255, 0},
        {63, 1},{255, 0},{255, 0},{255, 0},
        {65, 1},{255, 0},
        {63, 1},{255, 0},{255, 0},{255, 0},
        {255, 2}
    };

    // Fanfare 2
    const uint8_t Fanfare2pitchAndPatch[][2] =
    {
        // ry-ry-ry-u-9- => 5,9-,5,9-,5,9-,11-,13-
        {55, 1},{255, 0},
        {59, 1},{255, 0},{255, 0},{255, 0},
        {55, 1},{255, 0},
        {59, 1},{255, 0},{255, 0},{255, 0},
        {55, 1},{255, 0},
        {59, 1},{255, 0},{255, 0},{255, 0},
        {61, 1},{255, 0},{255, 0},{255, 0},
        {63, 1},{255, 0},{255, 0},{255, 0},
        {255, 2}

    };

    // Startup music.
    #define MUSIC_STEP 5
    const uint8_t StartupMusicPitchAndPatch[][2] =
    {
        // yyuyiuiuy =>  2-askelta alempaa

        {50+MUSIC_STEP, 1},{255, 0},
        {50+MUSIC_STEP, 1},{255, 0},
        {52+MUSIC_STEP, 1},{255, 0},
        {50+MUSIC_STEP, 1},{255, 0},
        {53+MUSIC_STEP, 1},{255, 0},
        {52+MUSIC_STEP, 1},{255, 0},
        {53+MUSIC_STEP, 1},{255, 0},
        {52+MUSIC_STEP, 1},{255, 0},
        {50+MUSIC_STEP, 1},{255, 0},{255, 0},{255, 0},{255, 0},{255, 0},{255, 0},{255, 0},

        {255, 2},  // pause
        {255, 0}, {255, 0}, {255, 0},{255, 0}, {255, 0},{255, 0}, {255, 0},

        {48+MUSIC_STEP, 1},{255, 0},
        {48+MUSIC_STEP, 1},{255, 0},
        {50+MUSIC_STEP, 1},{255, 0},
        {48+MUSIC_STEP, 1},{255, 0},
        {51+MUSIC_STEP, 1},{255, 0},
        {50+MUSIC_STEP, 1},{255, 0},
        {51+MUSIC_STEP, 1},{255, 0},
        {50+MUSIC_STEP, 1},{255, 0},
        {48+MUSIC_STEP, 1},{255, 0},{255, 0},{255, 0},{255, 0},{255, 0},{255, 0},{255, 0},

        {255, 2}, // pause
        {255, 0}, {255, 0}, {255, 0}, {255, 0}, {255, 0},
        {255, 0}, {255, 0},

        {50, 1},{255, 0},
        {50, 1},{255, 0},
        {52, 1},{255, 0},
        {50, 1},{255, 0},
        {53, 1},{255, 0},
        {52, 1},{255, 0},
        {53, 1},{255, 0},
        {52, 1},{255, 0},
        {50, 1},{255, 0},{255, 0},{255, 0},{255, 0},{255, 0},{255, 0},{255, 0},

        {255, 2},  // pause
        {255, 0}, {255, 0}, {255, 0},{255, 0}, {255, 0},{255, 0}, {255, 0},

        {48, 1},{255, 0},
        {48, 1},{255, 0},
        {50, 1},{255, 0},
        {48, 1},{255, 0},
        {51, 1},{255, 0},
        {50, 1},{255, 0},
        {51, 1},{255, 0},
        {50, 1},{255, 0},
        {48, 1},{255, 0},{255, 0},{255, 0},{255, 0},{255, 0},{255, 0},{255, 0},
        {255, 2},  // pause

    };

    const uint8_t* pitchAndPatchPtr = NULL;
    uint8_t songLen = 0;
    if(songNumber==0)
    {
        pitchAndPatchPtr = (const uint8_t*)StartupMusicPitchAndPatch;
        songLen = sizeof(StartupMusicPitchAndPatch) / sizeof(StartupMusicPitchAndPatch[0]);
        song.song_end=1; // Song end block.
        song.song_loop=0; // Loop back to block.
    }
    else if(songNumber==1)
    {
        pitchAndPatchPtr = (const uint8_t*)Fanfare1pitchAndPatch;
        songLen = sizeof(Fanfare1pitchAndPatch) / sizeof(Fanfare1pitchAndPatch[0]);
        song.song_end=0; // Song end block.
        song.song_loop=-1; // Loop back to block.
    }
    else
    {
        pitchAndPatchPtr = (const uint8_t*)Fanfare2pitchAndPatch;
        songLen = sizeof(Fanfare2pitchAndPatch) / sizeof(Fanfare2pitchAndPatch[0]);
         song.song_end=0; // Song end block.
        song.song_loop=-1; // Loop back to block.
    }

    // Store Fanfare 1 to the block 0.
    int32_t blockNum = 0;
    for(int32_t i=0, ii=0; i<songLen;i++,ii++)
    {
        if(ii>=PATTERNLENGTH)
        {
            blockNum += 3;
            ii = 0;
        }

        block[blockNum].notenumber[ii] = *(pitchAndPatchPtr+(i*2));
        block[blockNum].instrument[ii] = *(pitchAndPatchPtr+(i*2)+1);
    }

    // This starts the music
    snd.ampEnable(1);
    playing = true;

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


