#include "Pokitto.h"
#include "main.h"
#include "menu.h"

CMenu::CMenu() :
    m_isOpen( false ),
    m_mode(enumMainMenu),
    m_cursorPos(0),
    m_pressedAkeyDownOutsideMenu(false)
{
}

void CMenu::HandleMenus(bool isRace_, uint32_t bestLap_ms, bool doOpen)
{

    if(m_isOpen || mygame.buttons.pressed(BTN_C) || doOpen)
    {
        switch( m_mode )
        {
        case enumMainMenu:
            {
                m_isOpen =  HandleGenericMenu( bestLap_ms, m_cursorPos, "Time trial", "Race", NULL, NULL);
                if( ! m_isOpen )
                {
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

                     // Menu closed
                    m_cursorPos = 0;
               }
           }
           break;

        case enumContinueMenu:
            {
                if(isRace_)
                    m_isOpen =  HandleGenericMenu( bestLap_ms, m_cursorPos, "Restart", "Continue", "Exit race", NULL);
                else
                    m_isOpen =  HandleGenericMenu( bestLap_ms, m_cursorPos, "Restart", "Continue", "Exit time trial", NULL);

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
                        m_mode = enumMainMenu;
                        m_isOpen = true;
                    }

                     // Menu closed
                    m_cursorPos = 0;

               }
            }
            break;

        case enumTimeTrialFinishedMenu:
            {
               m_isOpen =  HandleGenericMenu( bestLap_ms, m_cursorPos, "Restart", "Exit race", NULL, NULL);
                if( ! m_isOpen )
                {
                    // if "Restart" selected, go to main menu
                    if(m_cursorPos == 0)
                        ResetGame(isRace_);

                    // if "Exit race" selected, go to main menu
                    else if(m_cursorPos == 1)
                    {
                        m_mode = enumMainMenu;
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
    if( mygame.buttons.pressed(BTN_A) )
    {
        if(! m_isOpen  )
            m_pressedAkeyDownOutsideMenu = true;
    }

    // Set window
    int32_t winX = 0;
    int32_t winY = 16;
    int32_t winW = 110;
    int32_t winH = 0;

    // Set window height
    if(bestLap_ms>0) winH += 15;
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
    if( bestLap_ms > 0 )
    {
        mygame.display.setColor(2,1);
        //currY += 4;
        mygame.display.print(winX+4,currY,"Best: ");
        //currY -= 1;
        DrawLapTime(bestLap_ms, winX+55, currY, fix16_from_float(1.5) );
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

    else
    {
        m_pressedAkeyDownOutsideMenu = false;
    }

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

