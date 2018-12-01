#include "Pokitto.h"
#include "main.h"
#include "menu.h"
#include "gfx_hdr/image_pilots1.h"
#include "gfx_hdr/image_pilots2.h"
#include "gfx_hdr/image_titlescreen.h"

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
    m_isFullScreenView(false)
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

                m_isOpen =  HandleGenericMenu( bestLap_ms, m_cursorPos, "Time trial", "Race", "Select track", "See pilots");
                if( ! m_isOpen )
                {
                    // Restore the original palette.
                    Pokitto::Core::display.load565Palette(palette_pal);

                    //
                    playing = false;
                    setOSC(&osc1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);

                    if(m_cursorPos == 0)
                    {
                        // time trial
                        m_mode = enumContinueMenu;

                         // Reset game
                        ResetGame( false );
                    }
                    else if(m_cursorPos == 1)
                    {
                        // race
                        m_mode = enumContinueMenu;

                         // Reset game
                        ResetGame( true );
                    }
                    else if(m_cursorPos == 2)
                    {
                        // Select track
                        m_mode = enumSelectTrackMenu;
                        m_isOpen = true;
                    }
                    else if(m_cursorPos == 3)
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
    if(bestLap_ms>0)
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
    else if( bestLap_ms > 0 )
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
            Pokitto::Core::display.load565Palette(palette_pal);
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
    if(!m_hasTrackBeenLoaded)
    {
        // The track name
        char* filePathAndNamePFFS = "pgptrack.txt";

        // Clear screen
        mygame.display.setColor(1,1);
        mygame.display.fillRect(0, 0, screenW, screenH);
        mygame.display.setColor(2,1);
        mygame.display.print(5, 5, filePathAndNamePFFS);
        mygame.display.print(5, 30, "Loading the track...");

        // Setup track

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

        // Read from SD
        pokInitSD(); // Call init always.
        (void)fileOpen(filePathAndNamePFFS, FILE_MODE_READONLY);
        const int32_t totalSize = (mapWidth+1)*mapHeight; // added newline
        char myTrack2[totalSize];
        uint8_t blockMapRAM2[mapWidth*mapHeight];
        uint16_t len = fileReadBytes((uint8_t*)myTrack2, totalSize);
        char text[64];
        //if(len!=totalSize)
        //    ShowCrashScreenAndWait("OOPS! PLEASE, RESTART", "POKITTO OR RELOAD", "SOFTWARE.", "LEN!=TOTALSIZE", itoa(len, text, 10));

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

        fileClose(); // close any open files


        // Now pont to the map in RAM.
        blockMap = blockMapRAM;

        // Calc perspective
        fix16_t fxPerspectiveFactor = fix16_from_int(350*screenH);
        for( int32_t y = 0; y<screenH; y++) {

            #if 0 // 3d
             // s = k/(y+15) ==> y+15 = k/s ==> y = k/s -15;
             // y = zk*yk /z -15
             m_perspectiveScaleY[y] = fix16_div(fxPerspectiveFactor, fix16_from_float((float)((y+screenShiftY)*1.0)));
             m_perspectiveScaleX[y] = m_perspectiveScaleY[y];
            #else // 2d
             m_perspectiveScaleY[y] = fix16_from_float(y*40.0);
             m_perspectiveScaleX[y] = fix16_from_float(100*40.0);
            #endif
        }
        m_fxCamAngle = 0;
        m_fxScaleFactor = fix16_from_float(1);

        m_hasTrackBeenLoaded = true;
        m_isFullScreenView = true;
    }
    else
    {
        // Draw track

        // Rotate
        // m_fxCamAngle -= fix16_pi >> 8;

        // Zoom
        m_fxScaleFactor += fix16_from_float(0.2);
        if(m_fxScaleFactor>fix16_from_float(30))
            m_fxScaleFactor = fix16_from_float(30);
        for( int32_t y = 0; y<screenH; y++) {
            m_perspectiveScaleY[y] = y * m_fxScaleFactor;
            m_perspectiveScaleX[y] = 100 * m_fxScaleFactor;
        }

        // ** Draw the road and edges and terrain.
        fix16_t fxCamX = fix16_from_int(1300);
        fix16_t fxCamY = fix16_from_int(1200) - (m_fxScaleFactor*40);
        fix16_t fxRotateCenterX = fxCamX;
        fix16_t fxRotateCenterY = fxCamY;
        //fxRotateCenterX += fix16_from_int(-400);
        //fxRotateCenterY += fix16_from_int(1500);
        DrawMode7( fix16_to_int(fxCamX), fix16_to_int(fxCamY), m_fxCamAngle, fxRotateCenterX, fxRotateCenterY, m_perspectiveScaleX, m_perspectiveScaleY);
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

    // Do not close the view
    return true;
}

//

