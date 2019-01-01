#include "Pokitto.h"
#include "main.h"
#include "menu.h"
#include "gfx_hdr/image_pilots1.h"
#include "gfx_hdr/image_pilots2.h"
#include "gfx_hdr/image_titlescreen.h"
#include "gfx_hdr/image_ui_button_up.h"
#include "gfx_hdr/image_ui_button_down.h"
#include "trackImporter.h"

#ifdef POK_SIM
//#include "io.h"
#else
//#include "SDFileSystem.h"
#endif

#pragma GCC diagnostic ignored "-Wwrite-strings"

void ShowCrashScreenAndWait( const char* texLine1, const char* texLine2, const char* texLine3, const char* texLine4, const char* texLine5 );


CMenu::CMenu() :
    m_isOpen( false ),
    m_mode(enumNoMenu),
    m_cursorPos(0),
    m_pressedAkeyDownOutsideMenu(false),
    m_pilotPicturePage(0),
    m_hasTrackBeenLoaded(false),
    m_fxCamAngle(0),
    m_fxScaleFactor(0),
    m_isFullScreenView(false),
    m_sceneryH(sceneryH),
    m_trackNum(0),
    m_trackCount(0),
    m_isTrackOk(false)
{
}

void CMenu::HandleMenus(bool isRace_, uint32_t bestLap_ms, MenuMode requestedMenuMode)
{
     if(requestedMenuMode != enumNoMenu )
        m_mode = requestedMenuMode;

    if(m_isOpen || mygame.buttons.pressed(BTN_C) || (requestedMenuMode != enumNoMenu) )
    {
        // Stop the sound if the menu is opened and no music is played.
        if(!m_isOpen && !playing )
            setOSC(&osc1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
        //
        switch( m_mode )
        {
        case enumStartupEmptyMenu:
            {
                // At startup show "empty" menu until the user presses a button.

                // Load palette ant the title image.
                Pokitto::Core::display.load565Palette(image_titlescreen_pal);
                DrawBitmapOpaque8bit(0, 0, &(image_titlescreen[2]), image_titlescreen[0], image_titlescreen[1] );

                m_isOpen =  HandleGenericMenu( 0, m_cursorPos, NULL, NULL, NULL, NULL);
                if( ! m_isOpen )
                {
                    playing = false;
                    setOSC(&osc1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);

                    // show the main menu next.
                    m_mode = enumMainMenu;
                    m_isOpen = true;

                    // Menu closed
                    m_cursorPos = 0;
               }
           }
           break;

        case enumMainMenu:
            {
                // Load palette ant the title image.
                Pokitto::Core::display.load565Palette(image_titlescreen_pal);
                DrawBitmapOpaque8bit(0, 0, &(image_titlescreen[2]), image_titlescreen[0], image_titlescreen[1] );

                bool isUserTrack = (m_trackNum != 0);
                if(!isUserTrack)  // ROM track
                    m_isOpen =  HandleGenericMenu( /*!!HV bestLap_ms*/0, m_cursorPos, "Time trial", "Race", "Select track", "See pilots");
                else  // User track
                     m_isOpen =  HandleGenericMenu( /*!!HV bestLap_ms*/0, m_cursorPos, "Time trial", "Select track", "See pilots", NULL);

                if( ! m_isOpen )
                {

                    // Restore the original palette.
                    Pokitto::Core::display.load565Palette(g_gamePalette);

                    //
                    playing = false;
                    setOSC(&osc1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);


                    int32_t menuItemNum = 0;
                    //if(m_trackNum == 0)
                    if(m_cursorPos == 0)
                    {
                        // time trial
                        m_mode = enumContinueMenu;

                         // Reset game
                        ResetGame( false );
                    }
                    else if(!isUserTrack && m_cursorPos == 1)
                    {
                        // race
                        m_mode = enumContinueMenu;

                         // Reset game
                        ResetGame( true );
                    }
                    else if((!isUserTrack && m_cursorPos == 2) || (isUserTrack && m_cursorPos == 1))
                    {
                        // Select track
                        m_mode = enumSelectTrackMenu;
                        m_isOpen = true;
                    }
                    else if((!isUserTrack && m_cursorPos == 3) || (isUserTrack && m_cursorPos == 2))
                    {
                        // race
                        m_mode = enumPilotPictureMenu;
                        m_isOpen = true;
                    }

                    // Menu closed
                    m_cursorPos = 0;
               }
           }
           break;

        case enumSelectTrackMenu:
            {
                m_isOpen =  HandleSelectTrackMenu();
                if( ! m_isOpen )
                {
                    m_hasTrackBeenLoaded = false;
                    m_isFullScreenView = false;
                    m_mode = enumMainMenu;
                    m_isOpen = true;

                    // Menu closed
                    m_cursorPos = 0;
                }
            }
            break;

        case enumPilotPictureMenu:
            {
                m_isOpen =  HandlePilotPictureMenu();
                if( ! m_isOpen )
                {
                    m_mode = enumMainMenu;
                    m_isOpen = true;

                    // Menu closed
                    m_cursorPos = 0;
                }
            }
            break;

        case enumContinueMenu:
            {
                if(isRace_)
                {
                    m_isOpen =  HandleGenericMenu( bestLap_ms, m_cursorPos, "Restart", "Continue", "Exit race", NULL);

                    if( ! m_isOpen )
                    {
                         // if "Restart" selected, go to main menu
                        if(m_cursorPos == 0)
                            ResetGame(isRace_);

                       // if "Continue", go to main menu
                        else if(m_cursorPos == 1)
                        {
                            // Continue
                        }

                       // if "Exit race" selected, go to main menu
                        else if(m_cursorPos == 2)
                        {
                            m_mode = enumStartupEmptyMenu;
                            SetupMusic(0);
                            m_isOpen = true;
                        }

                         // Menu closed
                        m_cursorPos = 0;

                   }
                 }
               else  // !isRace_
                {
                    m_isOpen =  HandleGenericMenu( bestLap_ms, m_cursorPos, "Restart", "Exit trial", NULL, NULL);

                    if( ! m_isOpen )
                    {
                         // if "Restart" selected, go to main menu
                        if(m_cursorPos == 0)
                            ResetGame(isRace_);

                       // if "Exit" selected, go to main menu
                        else if(m_cursorPos == 1)
                        {
                            m_mode = enumStartupEmptyMenu;
                            SetupMusic(0);
                            m_isOpen = true;
                        }

                         // Menu closed
                        m_cursorPos = 0;
                   }
               }
           }
            break;

        case enumTimeTrialFinishedMenu:
            {
               m_isOpen =  HandleGenericMenu( bestLap_ms, m_cursorPos, "Restart", "Exit trial", NULL, NULL);
                if( ! m_isOpen )
                {
                    //
                    playing = false;
                    setOSC(&osc1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);

                    // if "Restart" selected, go to main menu
                    if(m_cursorPos == 0)
                        ResetGame(isRace_);

                    // if "Exit race" selected, go to main menu
                    else if(m_cursorPos == 1)
                    {
                        m_mode = enumStartupEmptyMenu;
                        SetupMusic(0);
                        m_isOpen = true;
                    }

                    // Menu closed
                    m_cursorPos = 0;

                }
            }
            break;

        case enumRaceFinishedMenu:
            {
               m_isOpen =  HandleGenericMenu( bestLap_ms, m_cursorPos, "Restart", "Exit race", NULL, NULL);
                if( ! m_isOpen )
                {
                    //
                    playing = false;
                    setOSC(&osc1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);

                     // if "Restart" selected, go to main menu
                    if(m_cursorPos == 0)
                        ResetGame(isRace_);

                    // if "Exit race" selected, go to main menu
                    else if(m_cursorPos == 1)
                    {
                        m_mode = enumStartupEmptyMenu;
                        SetupMusic(0);
                        m_isOpen = true;
                    }

                    // Menu closed
                    m_cursorPos = 0;

                }
            }
            break;

        default:
            break;

        }  // end switch

        // Draw button hints, if any.
        m_animUpButtonHint.Draw();
        m_animDownButtonHint.Draw();

    }  // end if
}

//
bool CMenu::HandleGenericMenu( uint32_t bestLap_ms, int32_t& /*in out */ cursorPos, char* item1, char* item2, char* item3, char* item4)
{
    // If the menu is not yet open, make sure that the A key is pressed down *after* the menu is opened.
    //if( mygame.buttons.pressed(BTN_A) )
    if( mygame.buttons.aBtn() )
    {
        if(! m_isOpen  )
            m_pressedAkeyDownOutsideMenu = true;
    }

    // Set window
    int32_t winX = 0;
    int32_t winY = 16+20;
    int32_t winW = 110;
    int32_t winH = 0;

    // Set window height
    if(m_trackNum==0 && // only for ROM track
       bestLap_ms>0 )
    {
        winH += 15;
        winY -= 5;
    }
    int32_t H = 0;
    if(item1) winH += 10;
    if(item2) winH += 10;
    if(item3) winH += 10;
    if(item4) winH += 10;
    winH += 4;  // margin

    int32_t currY = winY;

    // Draw menu window background
    mygame.display.setColor(1,1);
    if(item1)
        mygame.display.fillRect(winX, winY, winW, winH);

	currY += 4;  // margin
    mygame.display.setInvisibleColor(1);

    // Print Best time
    if( m_mode == enumRaceFinishedMenu )
    {
        mygame.display.setColor(2,1);
        mygame.display.print(winX+4,currY,"Rank: ");
        //DrawLapTime(bestLap_ms, winX+55, currY, fix16_from_float(1.5) );

        // Draw the current rank
        DrawRankNumber(winX+55, currY);

        currY += 15;
    }
    // Print Best time
    else if( m_trackNum==0  && // only for ROM track
             bestLap_ms > 0 )
    {
        mygame.display.setColor(2,1);
        //currY += 4;
        mygame.display.print(winX+4,currY,"Best: ");
        int32_t milliseconds = bestLap_ms;
        int32_t seconds = milliseconds / 1000;
        milliseconds -= seconds*1000;
        mygame.display.print(seconds,10);
        mygame.display.print("'");
        int32_t a10ms = milliseconds / 100;
        mygame.display.print(a10ms,10);
        //currY -= 1;
        //DrawLapTime(bestLap_ms, winX+55, currY, fix16_from_float(1.5) );
        currY += 15;
    }

	// Print menu
	int32_t numItems = 0;
    if(item1)
    {
        if(cursorPos == 0)
        {
            // Draw selection bar
            mygame.display.setColor(3,1);
            mygame.display.fillRect(winX+4, currY, winW, 8);
        }
        mygame.display.setColor(2,1);
        mygame.display.print(winX+4, currY, item1);
        currY += 10;
        numItems++;
    }
    if(item2)
    {
        if(cursorPos == 1)
        {
            // Draw selection bar
            mygame.display.setColor(3,1);
            mygame.display.fillRect(winX+4, currY, winW, 8);
        }
        mygame.display.setColor(2,1);
        mygame.display.print(winX+4, currY, item2);
        currY += 10;
        numItems++;
    }
    if(item3)
    {
        if(cursorPos == 2)
        {
            // Draw selection bar
            mygame.display.setColor(3,1);
            mygame.display.fillRect(winX+4, currY, winW, 8);
        }
        mygame.display.setColor(2,1);
        mygame.display.print(winX+4, currY, item3);
        currY += 10;
        numItems++;
    }
    if(item4)
    {
        if(cursorPos == 3)
        {
            // Draw selection bar
            mygame.display.setColor(3,1);
            mygame.display.fillRect(winX+4, currY, winW, 8);
        }
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

    if(mygame.buttons.released(BTN_A))
    {
        if(! m_pressedAkeyDownOutsideMenu)
        {
            // Close the menu
            return false;
        }
        else
        {
            // Do not close the menu
            m_pressedAkeyDownOutsideMenu = false;
            return true;
        }
    }
    else
    {
        // Do not close the menu
        return true;
    }
}

//
bool CMenu::HandlePilotPictureMenu()
{
    // If the menu is not yet open, make sure that the A key is pressed down *after* the menu is opened.
    if( mygame.buttons.aBtn() )
    {
        if(! m_isOpen  )
            m_pressedAkeyDownOutsideMenu = true;
    }

    if(m_pilotPicturePage == 0)
    {
        // Load palette
        Pokitto::Core::display.load565Palette(image_pilots1_pal);

        // Show picture
        DrawBitmapOpaque8bit(0, 0, &(image_pilots1[2]), image_pilots1[0], image_pilots1[1] );
    }
    else
    {
        // Load palette
        Pokitto::Core::display.load565Palette(image_pilots2_pal);

        // Show picture
        DrawBitmapOpaque8bit(0, 0, &(image_pilots2[2]), image_pilots2[0], image_pilots2[1] );
    }

    if(Pokitto::Core::buttons.released(BTN_A))
    {
        if(! m_pressedAkeyDownOutsideMenu)
        {
            // Close the menu
            Pokitto::Core::display.load565Palette(g_gamePalette);
            return false;
        }
        else
        {
            // Do not close the menu
            m_pressedAkeyDownOutsideMenu = false;
            return true;
        }
    }
    else if(Pokitto::Core::buttons.released(BTN_UP) && m_pilotPicturePage == 1)
    {
        m_pilotPicturePage = 0;

        // Do not close the menu
        return true;
    }
    else if(Pokitto::Core::buttons.released(BTN_DOWN) && m_pilotPicturePage == 0)
    {
        m_pilotPicturePage = 1;

        // Do not close the menu
        return true;
    }
    else
    {
        // Do not close the menu
        return true;
    }
}

//
bool CMenu::HandleSelectTrackMenu()
{
    // The track names

    if(!m_hasTrackBeenLoaded)
    {
        // Clear screen
        mygame.display.setColor(1,1);
        mygame.display.fillRect(0, 0, screenW, screenH);
        mygame.display.setColor(2,1);
        mygame.display.print(1, 30, "Loading");
        mygame.display.print(1, 40, "the track...");
        while (!mygame.update()); // draw now

        // Setup track

        // Read the file list from SD.
        pokInitSD(); // Call init always.

        char* currDirName = NULL;
        #if POK_SIM
        char* tracksDirName = "./pgpdata/tracks/";
        getFirstDirEntry(tracksDirName); // This is always "." in Windows.
        getNextDirEntry(); // This is always ".." in Windows.
        bool isFirstFile = false;
        #else
        char* tracksDirName = "pgpdata/tracks";
        bool isFirstFile = true;
        #endif
        int i = 0;
        //char* fileName = NULL;
        for (; i < dirPathAndNameArrMaxLen; i++)
        {
            // read the file name from SD
            #ifndef POK_SIM
            if(isFirstFile)
                currDirName = getFirstDirEntry(tracksDirName);
            else
                currDirName = getNextDirEntry();
            #else
            currDirName = getNextDirEntry();
            #endif

           if(!currDirName || strlen(currDirName)==0)
                break; // No more files

            if(!isFirstFile)
            {
                 // The currDirName is in the form: "/mydir/"
                // Add the dir to the array
                int32_t length = strlen(currDirName);
                strncpy(m_dirNameArr[i], currDirName+1, length-2);
                m_dirNameArr[i][length-2] = '\0'; // "mydir\0"
            }
            else
            {
                int32_t length = strlen(currDirName);
                strncpy(m_dirNameArr[i], currDirName, length);
                m_dirNameArr[i][length] = '\0'; // "mydir\0"
            }

           isFirstFile = false;

        }  // end for

        i++; // Add one for the ROM track (index 0)
        m_trackCount = (dirPathAndNameArrMaxLen < i) ? dirPathAndNameArrMaxLen : i; // min

        //
        m_hasTrackBeenLoaded = true;
        m_isFullScreenView = true;

        // If the track is not the rom track, load the file.
        m_isTrackOk = true;
        char trackName[TrackImporter::maxTrackOrAuthorNameLen+2] = "Pokitto";
        char authorName[TrackImporter::maxTrackOrAuthorNameLen+2] = "Hanski";
        char myTrack2[TrackImporter::mapTotalSizeinFile] = {0};
        if(m_trackNum != 0)
        {
             // Read and verify track
            m_isTrackOk = TrackImporter::ReadAndValidateTrack(
                tracksDirName, m_dirNameArr[m_trackNum-1],
                /*OUT*/myTrack2, /*OUT*/trackName, /*OUT*/authorName );

            // TODO: reset all textures to ROM textures
            // Read and verify textures
            if( m_isTrackOk )

                m_isTrackOk = TrackImporter::ReadAndValidateTextures(tracksDirName, m_dirNameArr[m_trackNum-1]);
        }

        if(m_isTrackOk)
        {
            // Print the track name.
            char text[64];
            mygame.display.setColor(2,1);
            mygame.display.print(5, 5, trackName);

            // Store the map.
            if(m_trackNum == 0)
            {
                // Restore ROM textures.
                RestoreRomTextures();

                // Now point to the map in ROM.
                blockMap = (uint8_t*)blockMapROM;
            }
            else
            {
                uint8_t blockMapRAM2[mapWidth*mapHeight];
                {
                    // Map of blocks. Defines the whole game field!
                    if( blockMapRAM == NULL )
                        blockMapRAM = new uint8_t[mapWidth*mapHeight];
                    int32_t convTableLen = sizeof(TrackImporter::asciiTrackConversionTable);
                    g_billboardObjectInRamCount = 0;
                    for(int32_t y = 0; y < mapHeight; y++)
                    {
                        for(int32_t x = 0; x < mapWidth; x++)
                        {
                            // Create map
                            int invY = mapHeight - 1 - y; // mirror map vertically
                            int32_t mapWidth2 = mapWidth+1; // added newline
                            char item = myTrack2[invY*mapWidth2 + x];
                            //assert(item!=' ');

                            // Search the item from the conversion table
                            int32_t i=0;
                            for(; i<convTableLen; i++ )
                                if(TrackImporter::asciiTrackConversionTable[i]==item)
                                    break;

                            if(i>=convTableLen || item==' ')
                                break; // error

                            // Check for billboard objects
                            if( i==19 || i==20)
                            {
                                if( g_billboardObjectInRamCount <= g_BillboardObjectArrayMaxCount - 8 )
                                {
                                    int32_t bbIndex = g_billboardObjectInRamCount;
                                    g_billboardObjectInRamCount++;

                                    const uint8_t* sprite_bm = &(g_spriteBitmaps[ i-19 ][ 2 ]);  // "Cactus" or "Rock"
                                    const fix16_t fxSpriteScaledSizeFactor = fix16_from_float(1.0);
                                    const int16_t spriteBmW  = *(sprite_bm - 2);
                                    const int16_t spriteBmH  = *(sprite_bm - 1);

                                    // Init a billboard object.
                                    g_BillboardObjectArray[ bbIndex ].m_fxX = fix16_from_int(x * 64);
                                    g_BillboardObjectArray[ bbIndex ].m_fxY = fix16_from_int(y * 64);
                                    g_BillboardObjectArray[ bbIndex ].m_bitmap = sprite_bm;
                                    g_BillboardObjectArray[ bbIndex ].m_bitmapW = spriteBmW;
                                    g_BillboardObjectArray[ bbIndex ].m_bitmapH = spriteBmH;
                                    g_BillboardObjectArray[ bbIndex ].m_fxScaledWidth = spriteBmW * fxSpriteScaledSizeFactor;
                                    g_BillboardObjectArray[ bbIndex ].m_fxScaledHeight = spriteBmH * fxSpriteScaledSizeFactor;
                                }

                                i = 14; // Convert the map item to terrain/surface item

                            }  // end if

                            blockMapRAM[y*mapWidth + x] = i;

                        }  // end for

                    }  // end for
                }

                // Now point to the map in RAM.
                blockMap = blockMapRAM;
            }

            m_sceneryH = sceneryH;

            // Calc perspective
            for( int32_t y = 0; y<screenH; y++) {

               #if 0 // 2d
                 //m_perspectiveScaleY[y] = fix16_from_float(y*40.0);
                 //m_perspectiveScaleX[y] = fix16_from_float(100*40.0);
               #elif 1
                const fix16_t fxPerspectiveFactor = fix16_from_int(200*screenH);
                m_perspectiveScaleX[y] = fix16_div(fxPerspectiveFactor, fix16_from_float((float)((y+15)*0.25)));
                m_perspectiveScaleY[y] = fix16_mul(fix16_from_float(0.7), m_perspectiveScaleX[y]);

                m_previewX = 500;
                m_previewY = -600;
                m_previewPhase = 1;
                //m_sceneryH = 0;

               #else // 3d
                const fix16_t fxPerspectiveFactor = fix16_from_int(350*screenH);
                 // s = k/(y+15) ==> y+15 = k/s ==> y = k/s -15;
                 // y = zk*yk /z -15
                 m_perspectiveScaleY[y] = fix16_div(fxPerspectiveFactor, fix16_from_float((float)((y+screenShiftY)*1.0)));
                 m_perspectiveScaleX[y] = m_perspectiveScaleY[y];
               #endif
            }
            m_fxCamAngle = 0;
            m_fxScaleFactor = fix16_from_float(1);
            m_test = 0;

            //if()
            {
                int32_t buttonX = (screenW/2) - image_ui_button_down[0];
                int32_t buttonY = screenH - image_ui_button_down[1]-1;
                m_animUpButtonHint.Start( 2*1000, buttonX, buttonY, buttonX, buttonY, (uint8_t*)image_ui_button_down,
                        this, (int32_t)ButtonAnimState::enumShow );
            }

        }
    }
    else if (m_isTrackOk)
    {
        // Draw track

        //mygame.display.setColor(2,1);
        //mygame.display.print(5, 5, m_test); mygame.display.print("KB        ");

        // Preview track movement
        int32_t speed = 20;
        switch( m_previewPhase )
        {
        // Move east.
        case 1:
            m_previewX+=speed;
            if(m_previewX > 1770)
               //m_previewPhase++;
               m_previewPhase=3;
            break;

        // Move west.
        case 3:
            m_previewX-=speed;
            if(m_previewX< 500 )
               //m_previewPhase = 0;
               m_previewPhase = 1;
            break;
        }

        // ** Draw the road and edges and terrain.
        fix16_t fxCamX = fix16_from_int(m_previewX);
        fix16_t fxCamY = fix16_from_int(m_previewY);
        fix16_t fxRotateCenterX = fxCamX;
        fix16_t fxRotateCenterY = fxCamY;

        DrawMode7( fix16_to_int(fxCamX), fix16_to_int(fxCamY), m_fxCamAngle, fxRotateCenterX, fxRotateCenterY, m_perspectiveScaleX, m_perspectiveScaleY, m_sceneryH);
    }

    // If the view is not yet open, make sure that the A key is pressed down *after* the view is opened.
    if( mygame.buttons.aBtn() && ! m_isOpen  )
            m_pressedAkeyDownOutsideMenu = true;

    // If A button is pressed, return to the main menu.
    if(Pokitto::Core::buttons.released(BTN_A))
    {
        if(! m_pressedAkeyDownOutsideMenu)
             return false; // Close the view
        else
            m_pressedAkeyDownOutsideMenu = false; // Ready to close this view on next A button press.
    }
    else if(Pokitto::Core::buttons.released(BTN_UP))
    {
        // Change track
        m_hasTrackBeenLoaded = false;
        if(--m_trackNum < 0)
            m_trackNum = m_trackCount - 1;

        // Do not close the menu
        return true;
    }
    else if(Pokitto::Core::buttons.released(BTN_DOWN))
    {
        // Change track
        m_hasTrackBeenLoaded = false;
        if(++m_trackNum >= m_trackCount)
            m_trackNum = 0;

        // Do not close the menu
        return true;
    }
    //else if(Pokitto::Core::buttons.released(BTN_RIGHT))
    //{
    //    // !!HV TEST
    //    char* test = new char[1024];
    //    if(test) m_test++;
    //    mygame.display.setColor(1,1);
    //    mygame.display.fillRect(0, 0, screenW, screenH);
	//
    //    // Do not close the menu
    //    return true;
    //}

    // Do not close the view
    return true;
}

// Animation finished.
void CMenu::Finished( int32_t par )
{
    ButtonAnimState bas = (ButtonAnimState)par;
    if( bas == ButtonAnimState::enumShow )
    {
//        int32_t buttonX = (screenW/2) - image_ui_button_down[0];
//        int32_t buttonY = screenH - image_ui_button_down[1]-1;
//        m_animUpButtonHint.Start( 2*1000, buttonX, buttonY, buttonX, screenH, (uint8_t*)image_ui_button_down,
//                this, (int32_t)ButtonAnimState::enumDisappear );
    }
    else
    {
        // Inactivate animation
    }
}


