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
        'C',  // 19: The cactus billboard.
        'R',  // 20: The rock billboard.
    };

    // constants.
    const int32_t maxTrackOrAuthorNameLen = 13;
    const int32_t mapTotalSizeinFile = (mapWidth+1)*mapHeight; // added newline

    bool ReadAndValidateTrack(
        char* trackPath, char* trackFileName,
        /*OUT*/char* myTrack2, /*OUT*/char* trackName, /*OUT*/char* authorName );

    bool ReadAndValidateTextures(char* trackDirPath, char* trackDirName);

}
