# Welcome to Python on Pokitto!

import upygame as pygame
import urandom as random
import globals as glob
import data
import rooms
import gameobject as GameObjects
import sprite
import gc
import umachine
from micropython import const
import gc

# This is needed to be able to change global variables (!)

# Constants
HERO_VP_MOVE_TLX  = const(40)
HERO_VP_MOVE_TLY  = const(40)
HERO_VP_MOVE_BRX  = const(110-40)
HERO_VP_MOVE_BRY  = const(88-40)
HERO_SPEED_X  = const(1)

# Setup global variables
glob.viewPortX = 0
glob.viewPortY = 0

glob.roomIndex = 5
startRoomX = 5
startRoomY = 0

# Create tilemap.
# Note: This has to be at the very beginning, because , for sonme reason, if "print()" was used
# before this it will result garbage when drawing the tilemap
glob.tilemap = pygame.tilemap.Tilemap(glob.TILEMAP_W,glob.TILEMAP_H, data.ramMapPixels)

# Determine if we are running in emulator (or simulator)? 
# The pin Ext2 value is 0 in emulator but not in real HW.
isEmulator = False
joyhatY = umachine.Pin ( umachine.Pin.EXT2, umachine.Pin.ANALOG_IN )
joyhatYValue = joyhatY.value()
#print("joyhatY.value()=",test)
if(joyhatYValue == 0):
    isEmulator = True

screen = pygame.display.set_mode() # full screen
pygame.display.init(False)

# ZX Spectrum palette
pygame.display.set_palette_16bit([
    0, 0x001a, 0xd000, 0xd01a, 0x06a0, 0x06ba, 0xd6a0, 0xd6ba,
    0x0000, 0x001f, 0xf800, 0xf81f, 0x07e0, 0x07ff, 0xffe0, 0xffff
]);


# Initialize sound
glob.sound = pygame.mixer.Sound()
if(isEmulator==False):
    glob.sound.play_from_sd("rap2h.raw")


#########################################
def main():

    # setup sprite group
    glob.all_enemies = sprite.Group()
    glob.all_enemies_list = []

    # Add hero
    glob.heroGob = GameObjects.HeroGameObject(
        [data.herox0, data.herox1, data.herox2, data.herox1 ],
        [[-1,0],[0,0],[-1,0],[-1,0]])
    glob.heroGob.wx = 90 + startRoomX
    glob.heroGob.wy = 70 + startRoomY
    #glob.heroGob.wy = 8 + startRoomY
    glob.heroGob.animDur = 1


    # Add enemies
    
    # Scissors
    dummySurf = data.clockSurf0
    enemyGob = GameObjects.EnemyBounceGameObject([dummySurf], [[0,0]])
    glob.all_enemies.add(enemyGob)
    glob.all_enemies_list.append(enemyGob)

    # Scissors2
    enemyGob = GameObjects.EnemyBounceGameObject([dummySurf], [[0,0]])
    glob.all_enemies.add(enemyGob)
    glob.all_enemies_list.append(enemyGob)

    # Clock
    enemyGob = GameObjects.EnemyBounceGameObject([dummySurf], [[0,0]])
    glob.all_enemies.add(enemyGob)
    glob.all_enemies_list.append(enemyGob)

    # Juice
    enemyGob = GameObjects.GameObject([dummySurf], [[0,0]])
    glob.all_enemies.add(enemyGob)
    glob.all_enemies_list.append(enemyGob)
    glob.coinGob = enemyGob
  
    # Reset room
    ResetRoom(glob.roomIndex)

   # loop variables
    vx = 0
    vy = 0

    # The main loop
    while True:

        currenFrameTime = umachine.time_ms()

        if(glob.isGameLost):
            # Draw the game over window
            pygame.draw.rect(pygame.Rect(1,44-12,108,24), True, 15)
            pygame.draw.rect(pygame.Rect(3,44-10,104,20), True, 8)
            pygame.draw.text(5,44-6,"You did not wake up",15)
            pygame.draw.text(28,44-6+8,"GAME OVER!",15)
            pygame.display.flip()
            continue

        # Read the Pokitto standard keys
        eventtype = pygame.event.poll()
        if eventtype != pygame.NOEVENT:
            if eventtype.type == pygame.KEYDOWN:
                if eventtype.key == pygame.K_RIGHT:
                    vx = HERO_SPEED_X
                if eventtype.key == pygame.K_LEFT:
                    vx = -HERO_SPEED_X
                #if eventtype.key == pygame.K_DOWN:
                #    vy = HERO_SPEED_X
                if eventtype.key == pygame.BUT_A:
                    glob.heroGob.tryJump = True
                    
                if eventtype.key == pygame.BUT_B:
                        # Print free memory amount
                        gc.collect()
                        print ("free B1:",gc.mem_free())
                        ResetRoom(4)
                        gc.collect()
                        print ("free B2:",gc.mem_free())
                if eventtype.key == pygame.BUT_C:
                        gc.collect()
                        print ("free C1:",gc.mem_free())
                        ResetRoom(5)
                        gc.collect()
                        print ("free C2:",gc.mem_free())

            if eventtype.type == pygame.KEYUP:
                if eventtype.key == pygame.K_RIGHT:
                    vx = 0
                if eventtype.key == pygame.K_LEFT:
                    vx = 0
                #if eventtype.key == pygame.K_UP:
                #    vy = 0
                #if eventtype.key == pygame.K_DOWN:
                #    vy = 0
                #if eventtype.key == pygame.BUT_A:
                #    shoot = False

        # Store velocity
        glob.heroGob.vx = vx
        #glob.heroGob.vy = vy

        # *** UPDATE ***
        
      # rare update
        if(glob.frameNum%20==0):
            RareUpdate()

        # Update hero
        #glob.heroGob.update()
        glob.heroGob.update()
        #heroGob3.update()

        # Move viewport
        MoveViewport()

        # Update enemies
        glob.all_enemies.update()

        # *** DRAW ***

        # Draw tilemap
        glob.tilemap.draw(glob.viewPortX, glob.viewPortY)

        # Draw enemies
        glob.all_enemies.draw(screen)

        # Draw hero
        if(glob.heroGob.visible):
            screen.blit(glob.heroGob.image, glob.heroGob.rect.x, glob.heroGob.rect.y, 0, glob.heroGob.hmirror)

        #
        DrawStatusInfo()

        pygame.display.flip()

        glob.frameNum += 1

        #umachine.wait(100)

#########################################
def ChangeTilemap(map):

    for i in range(len(map)):  #(18*14): # 252
        data.ramMapPixels[i] = map[i]

    # Set tile attributes
    glob.tileAttributeArray = bytearray(16)
    glob.tileAttributeArray[0x0] = 0
    glob.tileAttributeArray[0x6] = glob.TILE_FLAG_BLOCKING | glob.TILE_FLAG_PLATFORM
    glob.tileAttributeArray[0x7] = 0
    glob.tileAttributeArray[0x1] = glob.TILE_FLAG_BLOCKING
    glob.tileAttributeArray[0xc] = glob.TILE_FLAG_BLOCKING | glob.TILE_FLAG_PLATFORM
    
    # Initialise the tilemap.
    glob.tilemap.set_tile(0x0, 8, 8, data.backgroundSurf0)
    glob.tilemap.set_tile(0x1, 8, 8, data.wallSurf1)
    glob.tilemap.set_tile(0x6, 8, 8, data.floorSurf1)
    glob.tilemap.set_tile(0x7, 8, 8, data.backgroundSurf1)
    glob.tilemap.set_tile(0xc, 8, 8, data.floorSurf0)

#########################################
def RareUpdate():

    if(glob.heroGob.wx<0-2):
        print("RareUpdate(). change room.")
        glob.roomIndex -= 1
        if(glob.roomIndex < 0): glob.roomIndex = 0

        #
        glob.heroGob.wx = (8 * glob.ROOM_W_IN_TILES) - glob.heroGob.rect.width
        
         # Reset room
        ResetRoom(glob.roomIndex)
        
    elif(glob.heroGob.wx>(8 * glob.ROOM_W_IN_TILES)-glob.heroGob.rect.width-2):
        print("RareUpdate(). change room.")
        glob.roomIndex += 1
        if(glob.roomIndex > glob.MAX_ROOM_INDEX): glob.roomIndex = glob.MAX_ROOM_INDEX
        
        #
        glob.heroGob.wx = 0
        
        # Reset room
        ResetRoom(glob.roomIndex)


#########################################
def GetRoomIndex(worldX, worldY):

    x = worldX // (8 * glob.ROOM_W_IN_TILES) 
    y = worldY // (8 * glob.ROOM_H_IN_TILES)
    roomIndex = x + (y*glob.NUM_OF_ROOMS_IN_MAP_ROW)
    print("new roomIndex", roomIndex)
    
    return roomIndex

#########################################
def DrawStatusInfo():

    # Draw gold bitmap and count
    screen.blit( data.goldSurf, 0, 0)
    pygame.draw.text(8,1,str(glob.goldCount),15);

    # Draw hart bitmap and count
    screen.blit( data.heartSurf, 110-16, 0)
    pygame.draw.text(110-7,1,str(glob.hartCount),15);

    # Draw room name
    pygame.draw.rect(pygame.Rect(0,81,110,7), True, 8);
    pygame.draw.text(10,83,glob.roomParameters[glob.roomIndex][0],15);



#########################################
def MoveViewport():

    # Calculate viewport position
    hero_vpx = glob.heroGob.rect.x
    dx = HERO_VP_MOVE_TLX - hero_vpx
    vpx = 0
    if(dx < 0):
        vpx = 0  # do not move vp
        dx = HERO_VP_MOVE_BRX - hero_vpx
        if(dx > 0):
            vpx = 0  # do not move vp
        else:
            vpx = dx
    else:
        vpx = dx
    hero_vpy = glob.heroGob.rect.y
    dy = HERO_VP_MOVE_TLY - hero_vpy
    vpy = 0
    if(dy < 0):
        vpy = 0
        dy = HERO_VP_MOVE_BRY - hero_vpy
        if(dy > 0):
            vpy = 0  # do not move vp
        else:
            vpy = dy
    else:
        vpy = dy

    # Move viewport
    newVpX = glob.viewPortX + vpx
    newVpY = glob.viewPortY + vpy

    # Check that viewport is not out bounds
    if(newVpX>0):
        newVpX=0
    elif(newVpX + glob.TILEMAP_W_PX < glob.SCREEN_W):
        newVpX = glob.SCREEN_W - glob.TILEMAP_W_PX
    if(newVpY>0):
        newVpY=0
    elif(newVpY + glob.TILEMAP_H_PX < glob.SCREEN_H):
        newVpY = glob.SCREEN_H - glob.TILEMAP_H_PX
    glob.viewPortX = newVpX
    glob.viewPortY = newVpY

    # update hero's position in the viewport
    glob.heroGob.rect.x = glob.heroGob.wx + glob.viewPortX
    glob.heroGob.rect.y = glob.heroGob.wy + glob.viewPortY

#########################################
def ResetRoom(roomIndex):
    
    print("ResetRoom. roomIndex", roomIndex)
    
    ChangeTilemap(data.worldMaps[roomIndex])
    glob.test += 1
    
    roomPos = glob.roomParameters[roomIndex][1]
    roomX = 0
    roomY = 0
    
    # Scissors
    gobIndex=0
    gobParams = glob.roomParameters[roomIndex][gobIndex+2]
    enemy = glob.all_enemies_list[gobIndex]
    if(gobParams == []):
        enemy.wx = -999
        enemy.rect.x = -999
        enemy.active = False
    else:
        enemy.active = True
        enemy.frames = gobParams[0]
        enemy.frameOffsets = gobParams[1]
        enemy.wx = gobParams[2][0] + roomX 
        enemy.wy = gobParams[2][1] + roomY 
        enemy.vx = gobParams[2][2]
        enemy.vy = gobParams[2][3]
        enemy.animDur = gobParams[2][4]
        enemy.dir = gobParams[2][5]
        enemy.moveDelay = gobParams[2][6]
        enemy.bounceLimits[0] = gobParams[2][7] + roomX
        enemy.bounceLimits[1] = gobParams[2][8] + roomY 
        enemy.bounceLimits[2] = gobParams[2][9] + roomX
        enemy.bounceLimits[3] = gobParams[2][10] + roomY 
        enemy.bounceType = gobParams[2][11]
        enemy.currentAnimFrameNum = 0;
        enemy.UpdateImage()
        enemy.animDurCounter = enemy.animDur;
        enemy.moveDelayCounter = 0
    #print("scissors",enemy.frames[0].get_rect())

    # Scissors2
    gobIndex=1
    gobParams = glob.roomParameters[roomIndex][gobIndex+2]
    enemy = glob.all_enemies_list[gobIndex]
    if(gobParams == []):
        enemy.wx = -999
        enemy.rect.x = -999
        enemy.active = False
    else:
        enemy.active = True
        enemy.frames = gobParams[0]
        enemy.frameOffsets = gobParams[1]
        enemy.wx = gobParams[2][0] + roomX 
        enemy.wy = gobParams[2][1] + roomY 
        enemy.vx = gobParams[2][2]
        enemy.vy = gobParams[2][3]
        enemy.animDur = gobParams[2][4]
        enemy.dir = gobParams[2][5]
        enemy.moveDelay = gobParams[2][6]
        enemy.bounceLimits[0] = gobParams[2][7] + roomX
        enemy.bounceLimits[1] = gobParams[2][8] + roomY 
        enemy.bounceLimits[2] = gobParams[2][9] + roomX
        enemy.bounceLimits[3] = gobParams[2][10] + roomY 
        enemy.bounceType = gobParams[2][11]
        enemy.currentAnimFrameNum = 0;
        enemy.UpdateImage()
        enemy.animDurCounter = enemy.animDur;
        enemy.moveDelayCounter = 0
        #print("scissors",enemy.frames[0].get_rect())

    # Clock
    gobIndex=2
    gobParams = glob.roomParameters[roomIndex][gobIndex+2]
    enemy = glob.all_enemies_list[gobIndex]
    if(gobParams == []):
        enemy.wx = -999
        enemy.rect.x = -999
        enemy.active = False
    else:
        enemy.active = True
        enemy.frames = gobParams[0]
        enemy.frameOffsets = gobParams[1]
        enemy.wx = gobParams[2][0] + roomX 
        enemy.wy = gobParams[2][1] + roomY 
        enemy.vx = gobParams[2][2]
        enemy.vy = gobParams[2][3]
        enemy.animDur = gobParams[2][4]
        enemy.dir = gobParams[2][5]
        enemy.moveDelay = gobParams[2][6]
        enemy.bounceLimits[0] = gobParams[2][7] + roomX
        enemy.bounceLimits[1] = gobParams[2][8] + roomY 
        enemy.bounceLimits[2] = gobParams[2][9] + roomX
        enemy.bounceLimits[3] = gobParams[2][10] + roomY 
        enemy.bounceType = gobParams[2][11]
        enemy.currentAnimFrameNum = 0;
        enemy.UpdateImage()
        enemy.animDurCounter = enemy.animDur;
        enemy.moveDelayCounter = 0
    
    # Juice
    gobIndex=3
    gobParams = glob.roomParameters[roomIndex][gobIndex+2]
    enemy = glob.all_enemies_list[gobIndex]
    if(gobParams == []):
        enemy.wx = -999
        enemy.rect.x = -999
        enemy.active = False
    else:
        enemy.active = True
        enemy.frames = gobParams[0]
        enemy.frameOffsets = gobParams[1]
        enemy.wx = gobParams[2][0] + roomX 
        enemy.wy = gobParams[2][1] + roomY 
        enemy.vx = gobParams[2][2]
        enemy.vy = gobParams[2][3]
        enemy.animDur = gobParams[2][4]
        enemy.dir = gobParams[2][5]
        enemy.moveDelay = gobParams[2][6]
        # Not EnemyBounceGameObject
        #enemy.bounceLimits[0] = gobParams[7] + roomX
        #enemy.bounceLimits[1] = gobParams[8] + roomY 
        #enemy.bounceLimits[2] = gobParams[9] + roomX
        #enemy.bounceLimits[3] = gobParams[10] + roomY 
        #enemy.bounceType = gobParams[2][11]
        enemy.currentAnimFrameNum = 0;
        enemy.UpdateImage()
        enemy.animDurCounter = enemy.animDur;
        enemy.moveDelayCounter = 0

#####################################

# Start mai
#premain()
main()
