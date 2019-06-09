# globals.py

# Copyright (C) 2019 Hannu Viitala
#
# The source code in this file is released under the MIT license.
# Go to http://opensource.org/licenses/MIT for the full license details.

import upygame as pygame
import data

SCREEN_W = const(110)
SCREEN_H = const(88)
TILE_W = const(8)
#TILE_ITEM_W = const(16)
TILE_H = const(8)
TILEMAP_W = const(18) # in tiles
TILEMAP_H = const(14) # in tiles
TILEMAP_W_ITEMS = const(TILEMAP_W//2) # in items. One item contais 2 tiles.
TILEMAP_W_PX = const(TILEMAP_W*TILE_W)
TILEMAP_H_PX = const(TILEMAP_H*TILE_H)
ROOM_W_IN_TILES = const(18)
ROOM_H_IN_TILES = const(14)
NUM_OF_ROOMS_IN_MAP_ROW = const(4)
NUM_OF_ROOMS_IN_MAP_COL = const(4)
MAX_ROOM_INDEX = const(NUM_OF_ROOMS_IN_MAP_ROW * NUM_OF_ROOMS_IN_MAP_COL)

GRAVITY = const(1)
HERO_JUMP_ACC_Y = const(-8)
HERO_MOVE_DELAY = const(5)

heroGob = None
coinGob = None
tilemap = None
viewPortX = None
viewPortY = None
tileAttributeArray = None
frameNum = 1
all_enemies = None
all_enemies_list = None
hartCount = 9
goldCount = 0
isGameLost = False
sound = None
roomParameters = []
roomIndex = 0
test = 0

TILE_FLAG_BLOCKING = const(0x1)
TILE_FLAG_PLATFORM = const(0x2)

def HeroWasHit():
    #global rumbleEndTime

    #Set hero back to start position
    heroGob.x = 50
    heroGob.y = 70

    # Rumble!
    #rumbleEndTime = umachine.time_ms() + 100
    