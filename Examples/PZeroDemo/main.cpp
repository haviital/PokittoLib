#include "Pokitto.h"
#include "fix16.h"
#include "main.h"
#include "gfx_hdr/GameData.h"

Pokitto::Core mygame;
Pokitto::Sound snd;

// Prototypes
void SetObject( uint16_t index, fix16_t fxX, fix16_t fxY, const uint8_t* bitmap, fix16_t fxScaledWidth, fix16_t fxScaledHeight);
void HandleGameKeys();
void HandleSetupMenu(int32_t& lastListPos);
void DrawMode7(int32_t tile2PosX, int32_t tile2PosY, fix16_t fxAngle);
uint8_t GetTileIndex(int32_t tile2PosX, int32_t tile2PosY, fix16_t fxAngle, int32_t getX, int32_t getY);
void DrawScaledBitmap8bitMipmap(int32_t posX, int32_t posY, const uint8_t* pokbitmapPtrs[], uint32_t scaledW, uint32_t scaledH );
void DrawScaledBitmap8bit(int32_t posX, int32_t posY, const uint8_t* bitmapPtr, uint32_t bitmapW, uint32_t bitmapH, uint32_t scaledW, uint32_t scaledH );
void Draw3dObects(fix16_t fxPosX, fix16_t fxPosY, fix16_t fxAngle);

const int32_t KRotCenterX = 0;
const int32_t KRotCenterY = -44;
const fix16_t fxMaxSpeedCollided = fix16_one>>1;
const fix16_t fxInitialRotVel = fix16_pi / 1000;
const fix16_t fxRotAccFactor = fix16_from_float(1.1);

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

struct Object3d
{
    fix16_t fxX;
    fix16_t fxY;
    fix16_t fxScaledWidth;
    fix16_t fxScaledHeight;
    const uint8_t* bitmap;
    int16_t bitmapW;
    int16_t bitmapH;

    fix16_t fxXInView;
    fix16_t fxYInView;
    fix16_t fxDistancePot;
};

/*
struct DrawListItem
{
    Object3d* obj;
    fix16_t fxDistancePot;
};
*/

// Drawing order list
const uint32_t drawListMaxCount = 100;
Object3d* drawList[drawListMaxCount] = {0};

// Other cars in uv-plane
const uint32_t objects3dMaxCount = 32;
uint32_t objects3dCount = 0;
Object3d objects3d[ objects3dMaxCount ] = {0};

// 4x4 bitmap
const uint8_t otherCarBitmap[] =
{
    0x02,0x02,0x02,0x02,
    0x02,0x02,0x02,0x02,
    0x02,0x02,0x02,0x02,
    0x02,0x02,0x02,0x02,
};
// 4x4 bitmap
const uint8_t otherCarBitmap2[] =
{
    0x03,0x03,0x03,0x03,
    0x03,0x03,0x03,0x03,
    0x03,0x03,0x03,0x03,
    0x03,0x03,0x03,0x03,
};
// Main
int main () {

    // Initialize variables.
    const fix16_t fxMaxSpeed = fix16_one*6;
    fix16_t fxVelOld = -1;
    fix16_t fxCos = fix16_one;
    fix16_t fxSin = 0;
    bool prevCollided = false;
    bool isManualRotation = true;
    bool isSetupMenuActive = false;
    int32_t lastSetupListPos = 1;
    //fix16_t fxShipX = fix16_from_int(5);
    //fix16_t fxShipY = fix16_from_float(80);

    // *** Setup PokittoLib

    mygame.begin();
    mygame.display.load565Palette(palette_pal);
    mygame.display.persistence = 1;
    #ifdef POK_SIM
    mygame.setFrameRate(60);
    #endif // PROJ_SIM

    // Setup cars
    fix16_t fxCarOffsetX = fix16_from_int(0);
    fix16_t fxCarOffsetY = fix16_from_int(650);
    fix16_t fxCarStepY = fix16_from_int(80);
    fix16_t fxRoadWidth = fix16_from_int(97+30);
    fix16_t fxScaledSizeFactor = fix16_from_float(0.6);
    fix16_t fxCactusScaledSizeFactor = fix16_from_float(0.8);

    // Set cactuses
    const uint8_t* cactus_bm = billboard_object_bitmaps[25];
    uint8_t i = 0;
    SetObject(i++, fix16_from_int(3), fix16_from_int(632), cactus_bm,
              *(cactus_bm - 2) * fxCactusScaledSizeFactor,
              *(cactus_bm - 1) * fxCactusScaledSizeFactor );
    SetObject(i++, fix16_from_int(243), fix16_from_int(1465), cactus_bm,
              *(cactus_bm - 2) * fxCactusScaledSizeFactor,
              *(cactus_bm - 1) * fxCactusScaledSizeFactor );
    SetObject(i++, fix16_from_int(706), fix16_from_int(1425), cactus_bm,
              *(cactus_bm - 2) * fxCactusScaledSizeFactor,
              *(cactus_bm - 1) * fxCactusScaledSizeFactor );
    SetObject(i++, fix16_from_int(667), fix16_from_int(2024), cactus_bm,
              *(cactus_bm - 2) * fxCactusScaledSizeFactor,
              *(cactus_bm - 1) * fxCactusScaledSizeFactor );
    SetObject(i++, fix16_from_int(1174), fix16_from_int(1844), cactus_bm,
              *(cactus_bm - 2) * fxCactusScaledSizeFactor,
              *(cactus_bm - 1) * fxCactusScaledSizeFactor );
    SetObject(i++, fix16_from_int(2050), fix16_from_int(1851), cactus_bm,
              *(cactus_bm - 2) * fxCactusScaledSizeFactor,
              *(cactus_bm - 1) * fxCactusScaledSizeFactor );
    SetObject(i++, fix16_from_int(1922), fix16_from_int(1130), cactus_bm,
              *(cactus_bm - 2) * fxCactusScaledSizeFactor,
              *(cactus_bm - 1) * fxCactusScaledSizeFactor );
    SetObject(i++, fix16_from_int(1986), fix16_from_int(35), cactus_bm,
              *(cactus_bm - 2) * fxCactusScaledSizeFactor,
              *(cactus_bm - 1) * fxCactusScaledSizeFactor );

    // Set stones
    const uint8_t* stone_bm = billboard_object_bitmaps[26];
    SetObject(i++, fix16_from_int(131), fix16_from_int(1056), stone_bm,
              fix16_from_int(*(stone_bm - 2)), fix16_from_int(*(stone_bm - 1)) );
    SetObject(i++, fix16_from_int(582), fix16_from_int(1469), stone_bm,
              fix16_from_int(*(stone_bm - 2)), fix16_from_int(*(stone_bm - 1)) );
    SetObject(i++, fix16_from_int(953), fix16_from_int(2042), stone_bm,
              fix16_from_int(*(stone_bm - 2)), fix16_from_int(*(stone_bm - 1)) );
    SetObject(i++, fix16_from_int(1839), fix16_from_int(1920), stone_bm,
              fix16_from_int(*(stone_bm - 2)), fix16_from_int(*(stone_bm - 1)) );
    SetObject(i++, fix16_from_int(521), fix16_from_int(127), stone_bm,
              fix16_from_int(*(stone_bm - 2)), fix16_from_int(*(stone_bm - 1)) );
    SetObject(i++, fix16_from_int(581), fix16_from_int(505), stone_bm,
              fix16_from_int(*(stone_bm - 2)), fix16_from_int(*(stone_bm - 1)) );
    SetObject(i++, fix16_from_int(333), fix16_from_int(634), stone_bm,
              fix16_from_int(*(stone_bm - 2)), fix16_from_int(*(stone_bm - 1)) );
    SetObject(i++, fix16_from_int(6), fix16_from_int(1220), stone_bm,
              fix16_from_int(*(stone_bm - 2)), fix16_from_int(*(stone_bm - 1)) );

    // Set cars
    SetObject(i++, fix16_from_int(42), fix16_from_int(1290), billboard_object_bitmaps[1],
              *(billboard_object_bitmaps[1] - 2) * fxScaledSizeFactor,
              *(billboard_object_bitmaps[1] - 1) * fxScaledSizeFactor );
    SetObject(i++, fix16_from_int(72), fix16_from_int(1290), billboard_object_bitmaps[2],
              *(billboard_object_bitmaps[2] - 2) * fxScaledSizeFactor,
              *(billboard_object_bitmaps[2] - 1) * fxScaledSizeFactor );
//    SetObject(i++, fix16_from_int(1320), fix16_from_int(1975), billboard_object_bitmaps[2],
//              *(billboard_object_bitmaps[2] - 2) * fxScaledSizeFactor,
//              *(billboard_object_bitmaps[2] - 1) * fxScaledSizeFactor );
    SetObject(i++, fix16_from_int(1960), fix16_from_int(1479), billboard_object_bitmaps[3],
              *(billboard_object_bitmaps[3] - 2) * fxScaledSizeFactor,
              *(billboard_object_bitmaps[3] - 1) * fxScaledSizeFactor );
    SetObject(i++, fix16_from_int(1922), fix16_from_int(430), billboard_object_bitmaps[18],
              *(billboard_object_bitmaps[18] - 2) * fxScaledSizeFactor,
              *(billboard_object_bitmaps[18] - 1) * fxScaledSizeFactor );
    SetObject(i++, fix16_from_int(1624), fix16_from_int(80), billboard_object_bitmaps[5],
              *(billboard_object_bitmaps[5] - 2) * fxScaledSizeFactor,
              *(billboard_object_bitmaps[5] - 1) * fxScaledSizeFactor );
    SetObject(i++, fix16_from_int(1012),fix16_from_int(-190), billboard_object_bitmaps[6],
              *(billboard_object_bitmaps[6] - 2) * fxScaledSizeFactor,
              *(billboard_object_bitmaps[6] - 1) * fxScaledSizeFactor );
    SetObject(i++, fix16_from_int(554),fix16_from_int(412), billboard_object_bitmaps[10],
              *(billboard_object_bitmaps[10] - 2) * fxScaledSizeFactor,
              *(billboard_object_bitmaps[10] - 1) * fxScaledSizeFactor );
    SetObject(i++, fix16_from_int(236),fix16_from_int(598), billboard_object_bitmaps[14],
              *(billboard_object_bitmaps[14] - 2) * fxScaledSizeFactor,
              *(billboard_object_bitmaps[14] - 1) * fxScaledSizeFactor );

    objects3dCount = i;
#if 0
    for(uint32_t i = 0; i < objects3dCount/4; i++)
    {
        // left side
        objects3d[i].fxX = fix16_from_int(0)+fxCarOffsetX;
        objects3d[i].fxY = (fxCarStepY*i)+fxCarOffsetY;
        objects3d[i].bitmap = billboard_object_bitmaps[25];
        objects3d[i].bitmapW =*(billboard_object_bitmaps[25] - 2);
        objects3d[i].bitmapH =*(billboard_object_bitmaps[25] - 1);
        objects3d[i].fxScaledWidth = fix16_from_int(objects3d[i].bitmapW);
        objects3d[i].fxScaledHeight = fix16_from_int(objects3d[i].bitmapH);

        // Cars row in the middle
        objects3d[i+(objects3dCount/4)].fxX = fxRoadWidth/3+fxCarOffsetX;
        objects3d[i+(objects3dCount/4)].fxY = (fxCarStepY*i)+fxCarOffsetY;
        objects3d[i+(objects3dCount/4)].bitmap = billboard_object_bitmaps[i];
        objects3d[i+(objects3dCount/4)].bitmapW =*(billboard_object_bitmaps[i] - 2);
        objects3d[i+(objects3dCount/4)].bitmapH =*(billboard_object_bitmaps[i] - 1);
        objects3d[i+(objects3dCount/4)].fxScaledWidth = objects3d[i+(objects3dCount/4)].bitmapW * fxScaledSizeFactor;
        objects3d[i+(objects3dCount/4)].fxScaledHeight = objects3d[i+(objects3dCount/4)].bitmapH * fxScaledSizeFactor;

        // Cars row in the middle
        objects3d[i+(2*objects3dCount/4)].fxX = 2*fxRoadWidth/3+fxCarOffsetX;
        objects3d[i+(2*objects3dCount/4)].fxY = (fxCarStepY*i)+fxCarOffsetY;
        objects3d[i+(2*objects3dCount/4)].bitmap = billboard_object_bitmaps[i+8];26
        objects3d[i+(2*objects3dCount/4)].bitmapW =*(billboard_object_bitmaps[i+8] - 2);
        objects3d[i+(2*objects3dCount/4)].bitmapH =*(billboard_object_bitmaps[i+8] - 1);
        objects3d[i+(2*objects3dCount/4)].fxScaledWidth = objects3d[i+(2*objects3dCount/4)].bitmapW * fxScaledSizeFactor;
        objects3d[i+(2*objects3dCount/4)].fxScaledHeight = objects3d[i+(2*objects3dCount/4)].bitmapH * fxScaledSizeFactor;

        // right side
        objects3d[i+(3*objects3dCount/4)].fxX = fxRoadWidth+fxCarOffsetX;
        objects3d[i+(3*objects3dCount/4)].fxY = (fxCarStepY*i)+fxCarOffsetY;
        objects3d[i+(3*objects3dCount/4)].bitmap = billboard_object_bitmaps[26];
        objects3d[i+(3*objects3dCount/4)].bitmapW =*(billboard_object_bitmaps[26] - 2);
        objects3d[i+(3*objects3dCount/4)].bitmapH =*(billboard_object_bitmaps[26] - 1);
        objects3d[i+(3*objects3dCount/4)].fxScaledWidth = fix16_from_int(objects3d[i+(3*objects3dCount/4)].bitmapW);
        objects3d[i+(3*objects3dCount/4)].fxScaledHeight = fix16_from_int(objects3d[i+(3*objects3dCount/4)].bitmapH);
    }
#endif

    static_assert( objects3dMaxCount <= drawListMaxCount, "error");
    for( int32_t i = 0; i < objects3dCount; i++)
    {
        drawList[i] = &(objects3d[i]);
    }

    // *** Setup sound

    int tonefreq=0;
    uint8_t amplitude = 255;//127;
    uint8_t wavetype = 1, wavetypeCrash = 4, arpmode=1;
    snd.ampEnable(1);
    snd.playTone(1,tonefreq,amplitude,wavetype,arpmode);
    //snd.playTone(1,100,255,0);

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

     // *** Draw scenery

    uint16_t skyW = image_sky[0];
    uint16_t skyH = image_sky[1];
    const uint8_t* skyBitmapPtr = &(image_sky[2]);
    for( int32_t x = 0; x<110; x+=skyW)
        DrawScaledBitmap8bit( x, 0, skyBitmapPtr, skyW, skyH, skyW, skyH );

    // *** The game loop
    while (mygame.isRunning()) {

        if (mygame.update()) {

            // ** Draw the road and edges and terrain.

            DrawMode7(fix16_to_int(fxCamX), fix16_to_int(fxCamY), fxAngle);

            // Draw 3d objects
            Draw3dObects(fxCamX, fxCamY, fxAngle);


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

            // Check collision
            prevCollided = collided;
            uint8_t tileIndex = GetTileIndex(fix16_to_int(fxCamX), fix16_to_int(fxCamY), fxAngle, 55, 56);
            if( tileIndex != 5 && tileIndex != 6 ) {
                collided = true;
                wavetype = 5;
            }
            else {
                collided = false;
                wavetype = 1;
            }

            // Draw scenery behind the menu
            if( isSetupMenuActive )
            {
                uint16_t skyW = image_sky[0];
                uint16_t skyH = image_sky[1];
                const uint8_t* skyBitmapPtr = &(image_sky[2]);
                for( int32_t x = 0; x<110; x+=skyW)
                    DrawScaledBitmap8bit( x, 0, skyBitmapPtr, skyW, skyH, skyW, skyH );
            }

            // Print coordinates on screen
            #if 1
            char text[128];
            mygame.display.print(0,0, itoa(fix16_to_int(fxCamX),text,10));
            mygame.display.print(", ");
            mygame.display.print(itoa(fix16_to_int(fxCamY)+65,text,10));
            mygame.display.print("     ");
            #endif

            // Open/close the setup menu
            if(mygame.buttons.pressed(BTN_C))
            {
               // Setup menu open close
               isSetupMenuActive = ! isSetupMenuActive;
            }

            if( isSetupMenuActive )
            {
                // Draw and handle the setup menu keys.
                HandleSetupMenu(/*OUT*/ lastSetupListPos);
            }
            else
            {
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

                    // Break
                    if(fxVel>fxMaxSpeedCollided) {
                        fxVel = fxVel - (fix16_one>>4);
                        if(fxVel<0)
                            fxVel = 0;
                    }
                    else if(fxVel<-fxMaxSpeedCollided){
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
            }
        }
    }
}

// Handle keys
void HandleGameKeys()
{

#if 0
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
    const int32_t reqRotateCenterX = tile2PosX + 0;
    const int32_t reqRotateCenterY = tile2PosY + 44;
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

void SetObject( uint16_t index, fix16_t fxX, fix16_t fxY, const uint8_t* bitmap, fix16_t fxScaledWidth, fix16_t fxScaledHeight)
{
    objects3d[index].fxX = fxX;
    objects3d[index].fxY = fxY;
    objects3d[index].bitmap = bitmap;
    objects3d[index].bitmapW =*(bitmap - 2);
    objects3d[index].bitmapH =*(bitmap - 1);
    objects3d[index].fxScaledWidth = fxScaledWidth;
    objects3d[index].fxScaledHeight = fxScaledHeight;
}

void Draw3dObects(fix16_t fxCamPosX, fix16_t fxCamPosY, fix16_t fxAngle)
{
    const fix16_t fxCos = fix16_cos(-fxAngle);
    const fix16_t fxSin = fix16_sin(-fxAngle);
    const fix16_t fxRotCenterX = fix16_from_int(0);
    const fix16_t fxRotCenterY = fix16_from_int(35);
    const int32_t horizonY = 0 + sceneryH;

    for( int32_t i = 0; i < drawListMaxCount; i++)
    {
        Object3d* obj = drawList[i];
        if( obj != NULL )
        {
            fix16_t fxX = obj->fxX;
            fix16_t fxY = obj->fxY;

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
                obj->fxDistancePot = fix16_max;
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

                obj->fxXInView = fxX;
                obj->fxYInView = fxY;

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
                    obj->fxDistancePot = fix16_max;
                else
                    obj->fxDistancePot = fxDistancePot;

            }  // end if

        }  // end if

        // Project and draw previous object, if not the first item in the list and the previous object is not too far (invisible).
        if( i > 0 && drawList[i-1]->fxDistancePot != fix16_max )
        {
            // If this object is farther than the previous one. Swap (bubble sort) before drawing.
            Object3d* drawablePrevObj = drawList[i-1];
            if( obj != NULL && obj->fxDistancePot > drawablePrevObj->fxDistancePot )
            {
                // Swap
                drawList[i-1] = obj;
                drawList[i] = drawablePrevObj;
            }

            // * Project 3D to 2D

            // Get the object bitmap size
            const uint8_t* bitmapData = drawablePrevObj->bitmap;
            uint32_t bitmapW = drawablePrevObj->bitmapW;
            uint32_t bitmapH = drawablePrevObj->bitmapH;
            fix16_t fxScaledWidth = drawablePrevObj->fxScaledWidth;
            fix16_t fxScaledHeight = drawablePrevObj->fxScaledHeight;

            // Bottom left corner
            fix16_t fx3dX = drawablePrevObj->fxXInView;
            fix16_t fx3dZ = drawablePrevObj->fxYInView;
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
            if( bitmapData != billboard_object_bitmaps[1] )
            {
                DrawScaledBitmap8bit(
                    fix16_to_int(fxScreenBlX) + 63 - (scaledWidth>>1), horizonY -screenShiftY- fix16_to_int(fxScreenBlY) - scaledHeight,
                    bitmapData,
                    bitmapW, bitmapH, scaledWidth, scaledHeight );
            }
            else
            {
                const uint8_t* pokbitmapPtrs[3] = {billboard_object_bitmaps_m0[1], billboard_object_bitmaps_m1[1], billboard_object_bitmaps_m2[1]};
                //uint32_t bitmapWArray[3] = {(uint32_t)*(billboard_object_bitmaps[1]-2), (uint32_t)*(billboard_object_bitmaps_m1[1]-2), (uint32_t)*(billboard_object_bitmaps_m2[1]-2)};
                //uint32_t bitmapHArray[3] = {(uint32_t)*(billboard_object_bitmaps[1]-1), (uint32_t)*(billboard_object_bitmaps_m1[1]-1), (uint32_t)*(billboard_object_bitmaps_m2[1]-1)};
                DrawScaledBitmap8bitMipmap(
                    fix16_to_int(fxScreenBlX) + 63 - (scaledWidth>>1), horizonY -screenShiftY- fix16_to_int(fxScreenBlY) - scaledHeight,
                    pokbitmapPtrs, scaledWidth, scaledHeight );
            }


        }  // end if

        // end of list?
         if( obj == NULL ) break;


    }  // end for
}
