#pragma once

#include "main.h"

namespace TrackImporter
{
    //Example
    //........
    //./r==,`.
    //.|!..|!.
    //.|!..|!.
    //.|+--j!.
    //.\====%.
    //........

    // Conversion between the ascii char and the block index.
    const int32_t asciiTrackConversionTableCount = 21;
    const char asciiTrackConversionTable[asciiTrackConversionTableCount] = {
        '|',  // 0: The left edge.
        '!',  // 1: The right edge.
        ' ',  // 2: None.
        ':',  // 3: The vertical road centre.
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
        '~',  // 19: The horizontal road centre.
        'o',  // 19: The empty road centre.

    };

    // constants.
    const int32_t maxTrackOrAuthorNameLen = 13;
    const int32_t mapTotalSizeinFile = (mapWidth+1)*mapHeight; // added newline

    bool ReadFromFileAndValidateTrack(
        char* trackPath, char* trackFileName,
        /*OUT*/char* myTrack2, /*OUT*/char* trackName, /*OUT*/char* authorName );
    bool ReadFromROMAndValidateTrack(
        /*OUT*/char* myTrack2, /*OUT*/char* trackName, /*OUT*/char* authorName );
    bool ReadAndValidateTrack( char* myTrack1, uint16_t len, char* trackDirName, char* trackFileName,
        /*OUT*/char* myTrack2, /*OUT*/char* trackName, /*OUT*/char* authorName );

    bool ReadTrackObjectsFromROM();
    // Read the track objects ascii file from SD
    bool ReadTrackObjectsFromFile( char* trackPath, char* trackDirName );
    bool ReadTrackObjects( char* buffer, int32_t blockSize, bool readFromFile );

    void FillBuffer( char* buffer, int32_t blockSize,
        /*OUT*/ uint16_t& len, /*OUT*/char** bufPtr,/*OUT*/char** endPtr, /*OUT*/bool& isBufferLeft );

    char* ReadValue( char* bufPtr, char* endPtr, /*OUT*/ int32_t& value );

    bool ReadAndValidateTextures(char* trackDirPath, char* trackDirName);

    void ConvertAsciiToMapElements( char* myTrack2 );

    void UpdateBBObjects();
}
