#include "Pokitto.h"
#include "main.h"
#include "menu.h"
#include "gfx_hdr/image_pilots1.h"
#include "gfx_hdr/image_pilots2.h"
#include "gfx_hdr/image_titlescreen.h"

CMenu::CMenu() :
    m_isOpen( false ),
    m_mode(enumNoMenu),
    m_cursorPos(0),
    m_pressedAkeyDownOutsideMenu(false),
    m_pilotPicturePage(0)
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

                m_isOpen =  HandleGenericMenu( bestLap_ms, m_cursorPos, "Time trial", "Race", "Pilots", NULL);
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
                        // race
                        m_mode = enumPilotPictureMenu;
                        m_isOpen = true;
                    }

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

