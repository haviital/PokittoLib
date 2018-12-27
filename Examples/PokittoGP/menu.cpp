#include "Pokitto.h"
#include "main.h"
#include "menu.h"
#include "gfx_hdr/image_pilots1.h"
#include "gfx_hdr/image_pilots2.h"
#include "gfx_hdr/image_titlescreen.h"
#include "imageformat.h"

#ifdef POK_SIM
//#include "io.h"
#else
//#include "SDFileSystem.h"
#endif

#pragma GCC diagnostic ignored "-Wwrite-strings"

void ShowCrashScreenAndWait( const char* texLine1, const char* texLine2, const char* texLine3, const char* texLine4, const char* texLine5 );

// Local constants.
const int32_t maxTrackOrAuthorNameLen = 13;
const int32_t mapTotalSizeinFile = (mapWidth+1)*mapHeight; // added newline


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
        }
    }
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

        //Example
        //........
        //./r==,`.
        //.|!..|!.
        //.|!..|!.
        //.|+--j!.
        //.\====%.
        //........

        // Conversion between the ascii char and the block index.
        const char asciiTrackConversionTable[20] = {
            '|',  // 0: The left edge.
            '!',  // 1: The right edge.
            ' ',  // 2: None.
            ' ',  // 3: None
            '=',  // 4: The top edge
            '-',  // 5: The bottom edge
            '\\', // 6: The outer corner of the 1st quarter.
            '+',  // 7: The inner corner of the 1st quarter.
            '/',  // 8: The outer corner of the 4th quarter.
            'r',  // 9: The inner corner of the 4th quarter.
            '%',  // 10: The outer corner of the 2nd quarter.
            'j',  // 11: The inner corner of the 2nd quarter.
            '`',  // 12: The outer corner of the 3rd quarter.
            ',',  // 13: The inner corner of the 3rd quarter.
            '.',  // 14: The surface.
            '#',  // 15: The starting grid, left side.
            '*',  // 16: The starting grid, right side.
            'X',  // 17: The halfway mark, left side.
            'x',  // 18: The halfway mark, right side.
        };

//
bool CMenu::HandleSelectTrackMenu()
{
    // The track names
    int32_t filePathAndNameArrMaxLen = 10;
    int32_t filePathAndNameMaxLen = 30;
    char fileNameArr[filePathAndNameArrMaxLen][filePathAndNameMaxLen] = {0};

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

        #if POK_SIM
        char* dirName = "./pgpdata/tracks/";
        getFirstFile("txt",dirName);
        #else
        char* dirName = "pgpdata/tracks";
        bool isFirstFile = true;
        #endif
        int i = 0;
        char* fileName = NULL;
        for (; i < filePathAndNameArrMaxLen; i++)
        {
            // read the file name from SD
            #ifndef POK_SIM
            if(isFirstFile)
            {
                fileName = getFirstFile("",dirName);
                isFirstFile = false;
            }
            else
                fileName = getNextFile("");
            #else
            fileName = getNextFile("txt");
            #endif

           if(!fileName || strlen(fileName)==0)
                break; // No more files

            // Add the file to the array
            strncpy(fileNameArr[i], fileName, filePathAndNameMaxLen);
            fileNameArr[i][filePathAndNameMaxLen-1] = '\0';
        }
        i++; // Add one for the ROM track (index 0)
        m_trackCount = (filePathAndNameArrMaxLen < i) ? filePathAndNameArrMaxLen : i; // min

        //
        m_hasTrackBeenLoaded = true;
        m_isFullScreenView = true;

        // If the track is not the rom track, load the file.
        m_isTrackOk = true;
        char trackName[maxTrackOrAuthorNameLen+2] = "Pokitto";
        char authorName[maxTrackOrAuthorNameLen+2] = "Hanski";
        char myTrack2[mapTotalSizeinFile] = {0};
        if(m_trackNum != 0)
        {
             // Read and verify track
            m_isTrackOk = ReadAndValidateTrack(
                dirName, fileNameArr[m_trackNum-1],
                /*OUT*/myTrack2, /*OUT*/trackName, /*OUT*/authorName );

            // TODO: reset all textures to ROM textures

            // Read and verify textures
            if( m_isTrackOk )
                m_isTrackOk = ReadAndValidateTextures(dirName);
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
                    int32_t convTableLen = sizeof(asciiTrackConversionTable);
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
                                if(asciiTrackConversionTable[i]==item)
                                    break;

                            if(i>=convTableLen || item==' ')
                                break; // error
                            blockMapRAM[y*mapWidth + x] = i;
                        }
                    }
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

//
//
bool CMenu::ReadAndValidateTrack(
    char* trackPath, char* trackFileName,
    /*OUT*/char* myTrack2, /*OUT*/char* trackName, /*OUT*/char* authorName )
{
    // Clear screen
    mygame.display.setColor(1,1);
    mygame.display.fillRect(0, 0, screenW, screenH);
    mygame.display.setColor(2,1);

    // Read the track ascii file from SD
    char myTrack1[mapTotalSizeinFile*2] = {0};
    //uint8_t blockMapRAM2[mapWidth*mapHeight];
    char filePathAndName[128] = {0};
    strcat(filePathAndName, trackPath);
    #ifndef POK_SIM
    strcat(filePathAndName, "/");
    #endif
    strcat(filePathAndName, trackFileName);
    uint8_t err = fileOpen(filePathAndName, FILE_MODE_READONLY);
    if(err)
    {
        mygame.display.setColor(3,1);mygame.display.print(1, 30, trackFileName);mygame.display.setColor(2,1);
        mygame.display.print(1, 40, "File is not");
        mygame.display.print(1, 50, "found!");
        fileClose(); // close any open files
        return false;
    }
    uint16_t len = fileReadBytes((uint8_t*)myTrack1, mapTotalSizeinFile*2);
    fileClose(); // close any open files

    // If a file contains only the decimal bytes 9–13, 32–126, it's probably a pure ASCII text file.
    for(int32_t i=0; i<len; i++)
    {
        if(! ( (myTrack1[i]>=9 && myTrack1[i]<=13) || (myTrack1[i]>=32 && myTrack1[i]<=126) ) )
        {
            mygame.display.setColor(3,1);mygame.display.print(1, 30, trackFileName);mygame.display.setColor(2,1);
            mygame.display.print(1, 40, "Not an ascii");
            mygame.display.print(1, 50, "file!");
            return false;
        }
    }

    // Read the track name
    int32_t pos = 0;
    const char lineFeed = 10, carriageReturn=13;
    int32_t i=0;
    for(;myTrack1[pos]!=lineFeed && myTrack1[pos]!=carriageReturn && i<=maxTrackOrAuthorNameLen;pos++, i++)
        trackName[i] = myTrack1[pos];


    // Check for error.
    if(i > maxTrackOrAuthorNameLen-1)
    {
        mygame.display.setColor(3,1);mygame.display.print(1, 30, trackFileName);mygame.display.setColor(2,1);
        mygame.display.print(1, 40, "Circuit name");
        mygame.display.print(1, 50, "too long!");
        return false;
    }
    trackName[i]='\0';

    // Skip extra LF and CR chars
    for(;myTrack1[pos]==lineFeed || myTrack1[pos]==carriageReturn;pos++);

    // Read the author name
    i=0;
    for(;myTrack1[pos]!=lineFeed && myTrack1[pos]!=carriageReturn && i<=maxTrackOrAuthorNameLen;pos++,i++)
        authorName[i] = myTrack1[pos];

    // Check for error.
    if(i > maxTrackOrAuthorNameLen-1)
    {
        mygame.display.setColor(3,1);mygame.display.print(1, 30, trackFileName);mygame.display.setColor(2,1);
        mygame.display.print(1, 40, "Author name");
        mygame.display.print(1, 50, "too long!");
        return false;
    }
    authorName[i]='\0';

    // Skip extra LF and CR chars
    for(;myTrack1[pos]==lineFeed || myTrack1[pos]==carriageReturn;pos++);

    // Copy the map to the output array.
    int32_t currentPosInLine = 0;
    int32_t currentLineNum = 0;
    for(int32_t i=0; i<mapTotalSizeinFile && pos<len; i++, pos++)
    {
        char c = myTrack1[pos];

        if(c==lineFeed || c==carriageReturn )
        {

            //
            if(currentPosInLine<mapWidth)
            {
                // Too short line
                mygame.display.setColor(3,1);mygame.display.print(1, 30, trackFileName);mygame.display.setColor(2,1);
                mygame.display.print(1, 40, "Line is too");
                mygame.display.print(1, 50, "short!");
                mygame.display.print(1, 60, "Line:");mygame.display.print(currentLineNum+3);
                return false;
            }

            // Next line.
            myTrack2[i] = lineFeed;
            currentPosInLine = 0;
            currentLineNum++;

            // Skip extra LF and CR chars
            pos++;
            for(;pos<len && (myTrack1[pos]==lineFeed || myTrack1[pos]==carriageReturn);pos++);
            pos--;



        }
        else if(currentPosInLine>=mapWidth)
        {
            // Too long line
            mygame.display.setColor(3,1);mygame.display.print(1, 30, trackFileName);mygame.display.setColor(2,1);
            mygame.display.print(1, 40, "Line is too");
            mygame.display.print(1, 50, "long!");
            mygame.display.print(1, 60, "Line:");mygame.display.print(currentLineNum+3);
            return false;
        }

        if(currentLineNum>=mapHeight )
        {
            // Too many lines
            mygame.display.setColor(3,1);mygame.display.print(1, 30, trackFileName);mygame.display.setColor(2,1);
            mygame.display.print(1, 40, "Too many");
            mygame.display.print(1, 50, "lines!");
            mygame.display.print(1, 60, "Line:");mygame.display.print(currentLineNum+3);
            return false;
        }

        // If this is not the last char on the line, copy it to the map.
        if(myTrack2[i] != lineFeed)
        {
            // Check that the char is valid.
            int32_t convTableLen = sizeof(asciiTrackConversionTable);
            int32_t tableIndex=0;
            for(; tableIndex<convTableLen; tableIndex++ )
                if(asciiTrackConversionTable[tableIndex]==c)
                    break;
            if(tableIndex>=convTableLen || c==' ')
            {
                // Not a valid char.
                mygame.display.setColor(3,1);mygame.display.print(1, 30, trackFileName);mygame.display.setColor(2,1);
                mygame.display.print(1, 40, "Invalid");
                mygame.display.print(1, 50, "char:\"");mygame.display.print(c);mygame.display.print("\"!");
                mygame.display.print(1, 60, "Line:");mygame.display.print(currentLineNum+3);
                mygame.display.print(1, 70, "Row:");mygame.display.print(currentPosInLine+1);
                return false;
            }

            // Store the char to the map.
            myTrack2[i] = c;
            currentPosInLine++;
        }

    }  // end for

    // Check that we found enough lines.
    if( currentLineNum<mapHeight-1 )
    {
        // Track file is too short
        mygame.display.setColor(3,1);mygame.display.print(1, 30, trackFileName);mygame.display.setColor(2,1);
        mygame.display.print(1, 40, "Too few lines");
        mygame.display.print(1, 50, "at line:");mygame.display.print(currentLineNum+3);
        return false;
    }

    // Check that the last line was not too short.
    if(currentPosInLine<mapWidth)
    {
        // Too short line
        mygame.display.setColor(3,1);mygame.display.print(1, 30, trackFileName);mygame.display.setColor(2,1);
        mygame.display.print(1, 40, "Line is too");
        mygame.display.print(1, 50, "short!");
        mygame.display.print(1, 60, "Line:");mygame.display.print(currentLineNum+3);
        return false;
    }
}

//
bool CMenu::ReadAndValidateTextures(char* dirPath)
{
    RestoreRomTextures();

    struct STextureFileParam
    {
        int32_t w,h;
        int32_t numOfParts;
    };

    const STextureFileParam textureFileParamArr[] =
    {
        {0,0,0},
        {16,24,4},  // 1: ball
        {8,12,1},   // 2: road1
        {8,12,1},   // 3: road2
        {16,24,4},  // 4: terrain
        {16,24,4},  // 5: start grid
        {8,12,1},   // 6: waypoint light
    };

    // Try to read all textures.
    char filePathAndName[128] = {0};
    int32_t textureFileParamArrSize = sizeof(textureFileParamArr) / sizeof(textureFileParamArr[0]);
    for(int32_t fileIndex=0, texIndex=0; fileIndex < textureFileParamArrSize; fileIndex++,texIndex++)
    {

        //
        char fileName[] = "tex00.bmp";
        int32_t tens = fileIndex / 10;
        int32_t ones = fileIndex - (tens * 10);
        fileName[3] = '0' + tens;
        fileName[4] = '0' + ones;

        strcpy(filePathAndName, dirPath);
        #ifndef POK_SIM
        strcat(filePathAndName, "/");
        #endif
        strcat(filePathAndName, fileName);

        // Read texture file.
        uint16_t* palette = NULL; // Gets the ownership.
        uint8_t* bitmap = NULL; // Gets the ownership.
        int err = openImageFileFromSD(filePathAndName, /*OUT*/&palette, /*OUT*/&bitmap);
        free(palette); palette = NULL;

        //
        if(err == -1 || err == -2)
        {
            if( textureFileParamArr[ fileIndex].numOfParts == 4 )
                texIndex+=3;
        }
//        else if(err == -2)
//        {
//            // Cannot open
//            mygame.display.setColor(3,1);mygame.display.print(1, 30, fileName);mygame.display.setColor(2,1);
//            mygame.display.print(1, 40, "Cannot");
//            mygame.display.print(1, 50, "open image.");
//            return false;
//        }
        else if(err == -6)
        {
            // Wrong palette size
            mygame.display.setColor(3,1);mygame.display.print(1, 30, fileName);mygame.display.setColor(2,1);
            mygame.display.print(1, 40, "Width should");
            mygame.display.print(1, 50, "be: ");mygame.display.print(textureFileParamArr[ fileIndex].w);
            return false;
        }
        else if(err == -10)
        {
            // Wrong palette size
            mygame.display.setColor(3,1);mygame.display.print(1, 30, fileName);mygame.display.setColor(2,1);
            mygame.display.print(1, 40, "Color count");
            mygame.display.print(1, 50, "not 256.");
            return false;
        }
        else if(err == -12)
        {
            // OOM
            mygame.display.setColor(3,1);mygame.display.print(1, 30, fileName);mygame.display.setColor(2,1);
            mygame.display.print(1, 40, "Out of");
            mygame.display.print(1, 50, "memory.");
            return false;
        }
        else if(err != 0)
        {
            // Other error
            mygame.display.setColor(3,1);mygame.display.print(1, 30, fileName);mygame.display.setColor(2,1);
            mygame.display.print(1, 40, "Invalid");
            mygame.display.print(1, 50, "image format");
            return false;
        }
        else
        {
            // Image ok.

            int32_t w = bitmap[0];
            int32_t h = bitmap[1];

            // Check size
            if( textureFileParamArr[ fileIndex].w !=  w)
            {
                // Wrong width
                mygame.display.setColor(3,1);mygame.display.print(1, 30, fileName);mygame.display.setColor(2,1);
                mygame.display.print(1, 40, "Width should");
                mygame.display.print(1, 50, "be: ");mygame.display.print(textureFileParamArr[ fileIndex].w);
                mygame.display.print(1, 60, "Is:");mygame.display.print(w);
                free(bitmap);
                return false;
            }
            if( textureFileParamArr[ fileIndex].h !=  h)
            {
                // Wrong height
                mygame.display.setColor(3,1);mygame.display.print(1, 30, fileName);mygame.display.setColor(2,1);
                mygame.display.print(1, 40, "Height");
                mygame.display.print(1, 50, "should");
                mygame.display.print(1, 60, "be: ");mygame.display.print(textureFileParamArr[ fileIndex].h);
                mygame.display.print(1, 70, "Is:");mygame.display.print(h);
                free(bitmap);
                return false;
            }

            if( textureFileParamArr[ fileIndex].numOfParts == 1 )
            {
                current_texture_bitmaps[texIndex] = bitmap+2;
                current_texture_bitmaps_mm1[texIndex] =  current_texture_bitmaps[texIndex] + (w * w);
                current_texture_bitmaps_mm2[texIndex] =  current_texture_bitmaps[texIndex] + (w * w) + (w>>1);
                bitmap = NULL;
            }
            else if( textureFileParamArr[ fileIndex].numOfParts == 4 )
            {
                // 16x24
                //
                uint8_t* tmpBitmap0 = (uint8_t *) malloc((8*12) + 2);  // header takes 2 bytes
                uint8_t* tmpBitmap1 = (uint8_t *) malloc((8*12) + 2);  // header takes 2 bytes
                uint8_t* tmpBitmap2 = (uint8_t *) malloc((8*12) + 2);  // header takes 2 bytes
                uint8_t* tmpBitmap3 = (uint8_t *) malloc((8*12) + 2);  // header takes 2 bytes
                tmpBitmap0[0] = 8; tmpBitmap0[1] = 12; // width & height
                tmpBitmap1[0] = 8; tmpBitmap1[1] = 12; // width & height
                tmpBitmap2[0] = 8; tmpBitmap2[1] = 12; // width & height
                tmpBitmap3[0] = 8; tmpBitmap3[1] = 12; // width & height
                uint8_t* tmpBitmapData0 = &(tmpBitmap0[2]);
                uint8_t* tmpBitmapData1 = &(tmpBitmap1[2]);
                uint8_t* tmpBitmapData2 = &(tmpBitmap2[2]);
                uint8_t* tmpBitmapData3 = &(tmpBitmap3[2]);
                uint8_t* readBitmapData = &(bitmap[2]);
                for(int32_t x=0; x<8; x++)
                {
                    for(int32_t y=0; y<8; y++)
                    {
                        tmpBitmapData0[y*8+x] = readBitmapData[y*16+x];
                        tmpBitmapData1[y*8+x] = readBitmapData[y*16+x+8];
                        tmpBitmapData2[y*8+x] = readBitmapData[(y+8)*16+x];
                        tmpBitmapData3[y*8+x] = readBitmapData[(y+8)*16+x+8];
                    }
                }
                current_texture_bitmaps[texIndex] = tmpBitmapData0;
                current_texture_bitmaps[texIndex+1] = tmpBitmapData1;
                current_texture_bitmaps[texIndex+2] = tmpBitmapData2;
                current_texture_bitmaps[texIndex+3] = tmpBitmapData3;

                // MIPMAP 1 (4x4 pixels)
                uint8_t* readBitmapDataMM = readBitmapData + (16*16);
                for(int32_t x=0; x<4; x++)
                {
                    for(int32_t y=0; y<4; y++)
                    {
                        tmpBitmapData0[(y+8)*8+x] = readBitmapDataMM[y*16     + x];
                        tmpBitmapData1[(y+8)*8+x] = readBitmapDataMM[y*16     + x+4];
                        tmpBitmapData2[(y+8)*8+x] = readBitmapDataMM[(y+4)*16 + x];
                        tmpBitmapData3[(y+8)*8+x] = readBitmapDataMM[(y+4)*16 + x+4];
                    }
                }
                current_texture_bitmaps_mm1[texIndex] = tmpBitmapData0 + (8 * 8);
                current_texture_bitmaps_mm1[texIndex+1] = tmpBitmapData1 + (8 * 8);
                current_texture_bitmaps_mm1[texIndex+2] = tmpBitmapData2 + (8 * 8);
                current_texture_bitmaps_mm1[texIndex+3] = tmpBitmapData3 + (8 * 8);

                // MIPMAP 2 (2x2 pixels)
                uint8_t* readBitmapDataMMM = readBitmapDataMM + 8;
                for(int32_t x=0; x<2; x++)
                {
                    for(int32_t y=0; y<2; y++)
                    {
                        tmpBitmapData0[(y+8)*8+x+4] = readBitmapDataMMM[y*16     + x];
                        tmpBitmapData1[(y+8)*8+x+4] = readBitmapDataMMM[y*16     + x+2];
                        tmpBitmapData2[(y+8)*8+x+4] = readBitmapDataMMM[(y+2)*16 + x];
                        tmpBitmapData3[(y+8)*8+x+4] = readBitmapDataMMM[(y+2)*16 + x+2];
                    }
                }
                current_texture_bitmaps_mm2[texIndex] = tmpBitmapData0 + (8 * 8) + 4;
                current_texture_bitmaps_mm2[texIndex+1] = tmpBitmapData1 + (8 * 8) + 4;
                current_texture_bitmaps_mm2[texIndex+2] = tmpBitmapData2 + (8 * 8) + 4;
                current_texture_bitmaps_mm2[texIndex+3] = tmpBitmapData3 + (8 * 8) + 4;

                texIndex+=3;

                //
                free(bitmap); bitmap = NULL;
            }
        }
    }  // end for

    return true;
}
