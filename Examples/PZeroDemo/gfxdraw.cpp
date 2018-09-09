#include "Pokitto.h"
#include "fix16.h"
#include "main.h"

//
void DrawMode7(int32_t tile2PosX, int32_t tile2PosY, fix16_t fxAngle)
{
    const uint16_t sceneryH = 0;
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
