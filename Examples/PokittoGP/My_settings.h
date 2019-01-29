/**************************************************************************/
/*!
    @file     My_settings.h
    @author   Hannu Viitala

    @section HOW TO USE My_settings

   My_settings can be used to set project settings inside the mbed online IDE
*/
/**************************************************************************/

#ifndef MY_SETTINGS_H
#define MY_SETTINGS_H

#define PROJ_HIRES 0            //1 = high resolution (220x176) , 0 = low resolution fast mode (110x88)
#define PROJ_MODE13 1	// Spinal's mode13 enabled

#define PROJ_ENABLE_SOUND 1     // 0 = all sound functions disabled
#define PROJ_ENABLE_SYNTH 1
#define PROJ_AUD_FREQ 8000

//#define FIXMATH_NO_64BIT
//#define FIXMATH_NO_OVERFLOW
#define FIXMATH_NO_ROUNDING // this increases the fps from 32 to 42. Not visible difference noticed, at least for now.

//#define PROJ_DEVELOPER_MODE 1
//#define PROJ_SHOW_FPS_COUNTER 1
#define PROJ_FPS 200

#define SCREENCAPTURE 0

#endif
