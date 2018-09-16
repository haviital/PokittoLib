#include "Pokitto.h"
#include "fix16.h"
#include "main.h"

//
void DrawMode7(int32_t tile2PosX, int32_t tile2PosY, fix16_t fxAngle)
{
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

    for( uint8_t y=0; y<screenH-sceneryH ; y++ ) {

        fix16_t fxZ = PerspectiveScaleY[y];
        fix16_t fxstepXFromY = PerspectiveScaleX[y];
        fix16_t fxFinalY =  fxZ + fix16_from_int(tile2PosY);

        // *** Step for scaling
        fxStepX = fxstepXFromY >> 7;
        fix16_t fxStepXInU = fix16_mul(fxStepX, fxCos);
        fix16_t fxStepXInV = fix16_mul(fxStepX, fxSin);

         // *** Shear the scanline to move horizontal origo to the middle
        fix16_t fxFinalX = -(fxstepXFromY>>1) + fix16_from_int(tile2PosX);

        // Rotate the starting point in texture (u,v)
        fix16_t fxU2 = fix16_mul(fxFinalX, fxCos) - fix16_mul(fxFinalY, fxSin) - fxRotatedCenterDiffX;
        fix16_t fxV2 = fix16_mul(fxFinalX, fxSin) + fix16_mul(fxFinalY, fxCos) - fxRotatedCenterDiffY;

        uint32_t finalU = fix16_to_int( fxU2 );
        uint32_t finalV = fix16_to_int( fxV2 );

       for( uint8_t x=0; x<110 ;  ) {

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
                tileIndex = blockData[blockDataIndex][(blockDataX & 0x7) + ((blockDataY & 0x7)*8)];
           }
            else
            {
                // Terrain surface tiles
                tileIndex = 7 + (blockDataX&1) + ((blockDataY&1)<<1);
            }

            // Get the tile.
            if( fxStepX >= fix16_from_int(2) )
                tileBitmapPtr = (uint8_t*)current_texture_bitmaps_mm2[tileIndex]; // Background tile
            else if( fxStepX >= fix16_from_float(1) )
                tileBitmapPtr = (uint8_t*)current_texture_bitmaps_mm1[tileIndex]; // Background tile
            else
                tileBitmapPtr = (uint8_t*)current_texture_bitmaps[tileIndex]; // Background tile

            // *** Draw one tile row fully.

            uint32_t finalUCurrTileIndex = finalU & 0xffffffff8;
            uint32_t finalVCurrTileIndex = finalV & 0xffffffff8;

            do {

                // Draw the texture pixel.
                 uint8_t color;
                if( fxStepX >= fix16_from_int(2) )
                    color = *(tileBitmapPtr + (((finalV & 0x7)>>2)*tileW) + ((finalU & 0x7)>>2));  // 1/2 size mipmap
                else if( fxStepX >= fix16_from_float(1) )
                    color = *(tileBitmapPtr + (((finalV & 0x7)>>1)*tileW) + ((finalU & 0x7)>>1));  // 1/2 size mipmap
                else
                    color = *(tileBitmapPtr + ((finalV & 0x7)*texW) + (finalU & 0x7));  // original size bitmap
                *scrptr++ = color;

                // Next texture coordinates.
                fxU2 += fxStepXInU;
                fxV2 += fxStepXInV;
                finalU = fix16_to_int( fxU2 );
                finalV = fix16_to_int( fxV2 );
                x++;

                // Exit condition
                if( (finalU & 0xffffffff8) != finalUCurrTileIndex ||
                    (finalV & 0xffffffff8) != finalVCurrTileIndex ||
                    x > 109
                )
                    break;

            } while(true);

         }  // end for

    }  // end for
}

// Draw ans scale the foreground tiles, i.e. text as a tile map.
// TODO: top-y-clipping
void DrawScaledBitmap8bit(int32_t posX, int32_t posY, const uint8_t* bitmapPtr, uint32_t bitmapW, uint32_t bitmapH, uint32_t scaledW, uint32_t scaledH )
{
    // Sanity check

    if((-posX)>=(int32_t)scaledW || posX >= screenW ||
       scaledW <= 0 ||  scaledW > 256 ||
       scaledH <= 0 ||  scaledH > 256 ||
       posY >= screenH || posY < 0    // y-clipping  from top not implemented yet. Preventing overflow.
    )
        return;

    uint8_t* scrptr = mygame.display.getBuffer(); // 8-bit screen buffer

    // Screen coordinates, x,y. Bitmap coordinates: u, v

    fix16_t fxStepXInU = fix16_div(fix16_from_int(bitmapW), fix16_from_int(scaledW));
    fix16_t fxStepXInV = fix16_div(fix16_from_int(bitmapH), fix16_from_int(scaledH));
    fix16_t fxV = 0;
    uint32_t finalV = 0;

    // clip

    fix16_t fxClippedStartU = 0;
    fix16_t fxClippedStartV = 0;
    uint8_t clippedStartU = 0;
    uint8_t clippedStartV = 0;
    uint8_t clippedScaledWidth = scaledW;
    uint8_t clippedScaledHeight = scaledH;
    //uint8_t startBitmapX = posX;
    //uint8_t endBitmapX = posX+scaledW;
    if(posX < 0) {
        fxClippedStartU = fxStepXInU * (-posX);
        clippedStartU = fix16_to_int( fxClippedStartU );
        clippedScaledWidth = scaledW + posX;
    }
    else
        scrptr += posX;  // Bitmap starting position on screen

    if(posX+scaledW > screenW) {
        clippedScaledWidth -=  posX + scaledW - screenW;
    }

     if(posY+scaledH > screenH) {
        clippedScaledHeight -=  posY + scaledH - screenH;
    }

    // Precompute x indices
    uint8_t xIndices [256];
    fix16_t fxU = fxClippedStartU;
    uint32_t finalU = clippedStartU;
    for( uint8_t x=0; x<clippedScaledWidth; x++) {
        xIndices[x] = finalU;
        fxU += fxStepXInU;
        finalU = fix16_to_int( fxU ); // is rounding applied?
    }

    if(clippedScaledWidth < 8)
    {
       // Draw
        for( uint32_t y=posY; y<clippedScaledHeight+posY ; y++ ) {

            uint8_t* screenScanlinePtr = scrptr + (y * mygame.display.width);
            const uint8_t* bitmapScanlinePtr = bitmapPtr + (finalV*bitmapW);
            fxU = fxClippedStartU;
            uint32_t finalU = clippedStartU;

            // *** Draw one pixel row.
            for( uint32_t x=0; x<clippedScaledWidth; x++) {

                // Draw pixel.
                uint8_t color = *(bitmapScanlinePtr + xIndices[x]);
                if(color)
                    *screenScanlinePtr = color;
                screenScanlinePtr++;

            }  // end for

            fxV += fxStepXInV;
            finalV = fix16_to_int( fxV );

       }  // end for
    }
    else // bitmapW >= 8 pixels
    {
       // Draw
        for( uint32_t y=posY; y<clippedScaledHeight+posY ; y++ ) {

            uint8_t* screenScanlinePtr = scrptr + (y * mygame.display.width);
            const uint8_t* bitmapScanlinePtr = bitmapPtr + (finalV*bitmapW);
            fxU = fxClippedStartU;
            uint32_t finalU = clippedStartU;

            // *** Draw one pixel row.
            uint32_t x = 0;
            uint8_t color = 0;

            while(clippedScaledWidth - x >= 8)
            {
                // Draw 8 pixels, unrolled.
                color = *(bitmapScanlinePtr + xIndices[x++]);
                if(color) *screenScanlinePtr = color;
                screenScanlinePtr++;
                color = *(bitmapScanlinePtr + xIndices[x++]);
                if(color) *screenScanlinePtr = color;
                screenScanlinePtr++;
                color = *(bitmapScanlinePtr + xIndices[x++]);
                if(color) *screenScanlinePtr = color;
                screenScanlinePtr++;
                color = *(bitmapScanlinePtr + xIndices[x++]);
                if(color) *screenScanlinePtr = color;
                screenScanlinePtr++;
                color = *(bitmapScanlinePtr + xIndices[x++]);
                if(color) *screenScanlinePtr = color;
                screenScanlinePtr++;
                color = *(bitmapScanlinePtr + xIndices[x++]);
                if(color) *screenScanlinePtr = color;
                screenScanlinePtr++;
                color = *(bitmapScanlinePtr + xIndices[x++]);
                if(color) *screenScanlinePtr = color;
                screenScanlinePtr++;
                color = *(bitmapScanlinePtr + xIndices[x++]);
                if(color) *screenScanlinePtr = color;
                screenScanlinePtr++;
            }

            // Draw the rest of the pixels.
            for( ; x<clippedScaledWidth;) {

                // Draw pixel.
                uint8_t color = *(bitmapScanlinePtr + xIndices[x++]);
                if(color) *screenScanlinePtr = color;
                screenScanlinePtr++;

            }  // end for

            fxV += fxStepXInV;
            finalV = fix16_to_int( fxV );

       }  // end for
    }
}

