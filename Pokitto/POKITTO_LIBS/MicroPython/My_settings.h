/**************************************************************************/
/*!
    @file     My_settings.h
    @author   XX

    @section HOW TO USE My_settings

   My_settings can be used to set project settings inside the mbed online IDE
*/
/**************************************************************************/

#ifndef MY_SETTINGS_H
#define MY_SETTINGS_H
#define PROJ_HIRES 1 //1 = high resolution (220x176) , 0 = low resolution fast mode (110x88)
#define PROJ_STARTUPLOGO 1
#define PROJ_GAMEBUINO 0
#define PROJ_STREAMING_MUSIC 1
#define PROJ_ENABLE_SYNTH 0
#define PROJ_ENABLE_SOUND       1       // 0 = all sound functions disabled
#define PROJ_AUD_FREQ           11025     // 44100 //22050 //11025
#define PROJ_STREAM_TO_DAC      1       // 1 use DAC for stream, 0 = use PWM for stream
#define PROJ_USE_PWM            1       // 1 use PWM, 0 do not use PWM
#define PROJ_GBSOUND            0       // 1 = use Gamebuino-compatible sound interrupt (choose this or the one below)


#define PROJ_SHOW_FPS_COUNTER

#define SPRITE_COUNT 8

// Python specific

#define PROJ_PYTHON_REPL 0
#define MICROPY_ENABLE_GC 1  // This just means micropython is in use

#ifdef POK_SIM
#define USE_USB_SERIAL_PRINT (0)
#else
#define USE_USB_SERIAL_PRINT (0)
//#define USE_SEGGER_SERIAL_PRINT
#endif

#endif
