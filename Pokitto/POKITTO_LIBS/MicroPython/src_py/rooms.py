# rooms.py

import globals as glob
import data

# roomParameters
glob.roomParameters = [
    [], # room0
    [], # room1
    [], # room2
    [], # room3
    [   # *** room4: Beehive ***
        "Beehive",
        [ 0, 11*8 ],  # room world coordinates
        [   # bee1
            [data.beeSurf_f1, data.beeSurf_f2 ], # frames
            [data.beeSurfOffset_f1,data.beeSurfOffset_f2], # frame offsets
            # wx, wy, vx, vy, adur, dir, del,  bx1,   by1,      bx2,   by2,  bType
            [  0, 10,  1, 0,     1,   2,   2,    -16,  -999,   20*8,   999,      1]
        ],
        [   # bee2
            [data.beeSurf_f1, data.beeSurf_f2 ], # frames
            [data.beeSurfOffset_f1,data.beeSurfOffset_f2], # frame offsets
            # wx, wy, vx, vy, adur, dir, del,  bx1,   by1,   bx2,   by2,  bType
            [ 60, 10,  1,  0,     1,   2,   2,    -16,  -999,   20*8,   999,      1]
        ],
        [   # bee queen
            [data.beeQueenSurf_f1, data.beeQueenSurf_f2 ], # frames
            [[0,0],[0,0]], # frame offsets
            # wx, wy, vx, vy, adur, dir, del,  bx1,   by1,   bx2,   by2,  bType
            [ 105, 80,  1, 0,    1,   2,   3,   105,  -999,    130,   999,      0] 
        ],
        [   # juice
            [data.honeyPotSurf0, data.honeyPotSurf1 ], # frames
            [[0,0],[0,0]], # frame offsets  
            #        wx,        wy, vx, vy, adur, dir, del,  bx1,   by1,   bx2,   by2,  bType
            [  8+(15*8),  28+(9*8),  0, 0,    1,   2,   1, -999,  -999,   999,   999,       0] 
        ]
    ],
    [   # *** room5: Nightmare begins ***
        "Nightmare begins",
        [ 18*8, 11*8 ],  # room world coordinates
        [   # scissors
            [data.scissorsSurf0, data.scissorsSurf1, data.scissorsSurf2, data.scissorsSurf1 ], # frames
            [[-2,0],[2,0],[0,0],[0,0]], # frame offsets
            # wx, wy, vx, vy, adur, dir, del,  bx1,   by1,   bx2,   by2,  bType
            [ 60, 10,  0, 1,     1,   3,   2, -999,     0,   999,  14*8,      0]
        ],
        [
            # scissors2: empty
        ],
        [   # clock
            [data.clockSurf0, data.clockSurf1, data.clockSurf2, data.clockSurf1 ], # frames
            [[0,0],[0,0],[0,0],[0,0]], # frame offsets
            # wx, wy, vx, vy, adur, dir, del,  bx1,   by1,   bx2,   by2,  bType
            [ 20,  9,  1, 0,    1,   2,   3,   20,  -999,    90,   999,      0] 
        ],
        [   # juice
            [data.juiceSurf0, data.juiceSurf1 ], # frames
            [[0,0],[0,0]], # frame offsets  
            # wx, wy, vx, vy, adur, dir, del,  bx1,   by1,   bx2,   by2,  bType
            [  8,  36,  0, 0,    1,   2,   1, -999,  -999,   999,   999,      0] 
        ]
    ],
    [], # room6
    [] # room7
]
