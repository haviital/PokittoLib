print('*** start python script')
import gc
import upygame as pygame
import urandom as random
import sprite
import marsattack_data as gamedata
import marsattack_classes as gameclass
import umachine as pok
#import utime
#import math # not currently supported for HW build

# *** INIT SCREEN AND CRATE GLOBAL OBJECTS
pygame.display.init()
screen = pygame.display.set_mode() # full screen
screenRect = screen.get_rect()
screenW = screenRect.width
screenH = screenRect.height
shipGob = None
bombGob = None
explosionGob = None
explosionGob2 = None
explosionGob3 = None
g_rockpileGobList = []
gameMode="play"
points = 0
g_rockPileSize = 1;
g_numPiles = 8;
g_ship_speed = 2
g_level = 0
test_num_piles = 8
g_debug_mode = 0
g_levelParam = [
    # palette, number of piles, max height, body bitmap,head bitmap, head collision rect
    [[60683,32963,49572,65502], test_num_piles, 0, gamedata.rock1Surf, gamedata.rockHead1Surf, gamedata.rockHead1CollRect],
    [[34490,4648,11215,63455], test_num_piles, 1, gamedata.rock2Surf, gamedata.rockHead2Surf, gamedata.rockHead2CollRect],
    [[36590,6722,13283,61405], test_num_piles, 1, gamedata.rock3Surf, gamedata.rockHead3Surf, gamedata.rockHead3CollRect],
    [[62916,16800,31456,65300], test_num_piles, 2, gamedata.rock6Surf, gamedata.rockHead6Surf, gamedata.rockHead6CollRect],
    [[58427,30926,43283,65439], test_num_piles, 2, gamedata.rock5Surf, gamedata.rockHead5Surf, gamedata.rockHead5CollRect],
    [[42260,10565,19049,61309], test_num_piles, 3, gamedata.rock7Surf, gamedata.rockHead7Surf, gamedata.rockHead7CollRect],
    [[62480,43073,55393,65438], test_num_piles, 3, gamedata.rock4Surf, gamedata.rockHead4Surf, gamedata.rockHead4CollRect],
]

# Create sprite lists
all_sprites = sprite.Group()
all_rockpiles = gameclass.RockPileGroup()
all_gobs = gameclass.GobGroup()

# Set palette
pygame.display.set_palette_16bit([60683,32963,49572,65502]);

print('*** display ready')

# *** MAIN
def main():

    # Create whole level
    RestartLevel()

    while True:

        print('gameMode', gameMode)

        # Draw points
        DrawPoints()

        # Choose mode
        if gameMode=="start":
            MainPlay()
        elif gameMode=="level done":
            MainLevelDone()
        elif gameMode=="lost game":
            MainLostGame()
        else: # gameMode="play"
            MainPlay()

# *** MAIN PLAY
def MainPlay():

    global gameMode
    global points
    global g_level

    vx = g_ship_speed;
    vy = 0;
    exitLoop = False
    destroyedPilesCount = 0
    while not exitLoop:

        # read keys
        eventtype = pygame.event.poll()
        if eventtype != pygame.NOEVENT:
            if eventtype.type == pygame.KEYDOWN:
                if eventtype.key == pygame.BUT_A:
                    if not bombGob.visible:
                        bombGob.rect.x = shipGob.rect.x + ((shipGob.rect.width- bombGob.rect.width)//2)
                        bombGob.rect.y = shipGob.rect.y + shipGob.rect.height
                        bombGob.setvel(0,2)
                        bombGob.visible = True

                if g_debug_mode>0 and eventtype.key == pygame.K_UP:
                    g_level+=1
                    if g_level > 6:
                        g_level = 6;
                    RestartLevel()

                if g_debug_mode>0 and eventtype.key == pygame.K_DOWN:
                    g_level-=1
                    if g_level < 0:
                        g_level = 0;
                    RestartLevel()

        #vx = 3
        shipGob.setvel(vx,vy)

        # Check bomb collision
        bombGob.collRect2.x = bombGob.rect.x + bombGob.collRect.x
        bombGob.collRect2.y = bombGob.rect.y + bombGob.collRect.y
        bombGob.collRect2.width = bombGob.collRect.width
        bombGob.collRect2.height = bombGob.collRect.height
        hitSprite = sprite.spritecollideany(bombGob, all_rockpiles, IsSpriteCollided)
        if hitSprite != None:

            #start = utime.ticks_us()

            # setup explosion
            explosionGob.rect.x = hitSprite.rect.x
            explosionGob.rect.y = hitSprite.rect.y
            explosionGob.vx = 0
            explosionGob.vy = 0
            explosionGob.animDur = 1
            explosionGob.visible = True
            explosionGob.currentFrameNum = 0;
            explosionGob.setSpriteFrame()

            #print('bcoll1', utime.ticks_us()-start);

            # Calc dirty rect
            #totalH =  hitSprite.rect.height + (hitSprite.numClones * hitSprite.frames[1].get_rect().height)
            totalH =  hitSprite.rect.height + hitSprite.frames[1].get_rect().height
            dirtyRect = pygame.Rect(hitSprite.rect.x, hitSprite.rect.y, hitSprite.rect.width, totalH)

            if hitSprite.numClones > 0:
                # Reduce pile height by one rock
                ChangeRockPile(hitSprite, hitSprite.numClones-1)
            else:
                # Hide pile as there is only one rock left in pile
                hitSprite.rect.y = 999
                hitSprite.collRect2.y = 999
                destroyedPilesCount += 1
                if destroyedPilesCount >= g_levelParam[g_level][1]:
                    gameMode="level done"
                    exitLoop = True

            # Redraw background and piles
            screen.fill(0, dirtyRect)
            #all_rockpiles.update()
            hitSprite.update()

            #screen.fill(0, dirtyRect)
            screen.set_clip(dirtyRect);
            screen.blit(gamedata.backgroundSurf, 0, 0, 15) # no invisible color
            screen.set_clip();
            #all_rockpiles.draw(screen)

            # draw the rock pile
            y = hitSprite.rect.y
            screen.blit(hitSprite.image, hitSprite.rect.x, y) # blit head hwsprite
            y += hitSprite.rect.height
            rockSF = hitSprite.frames[1]
            screen.blit(rockSF, hitSprite.rect.x, y) # blit body hwsprite

            # Update dirty rect
            #print('dirtyRect=', dirtyRect)

            pygame.display.update(False, dirtyRect, True)  # Draw now, draw only

            # Draw points
            points += 10
            DrawPoints()

        # Check ship collision
        hitSprite = sprite.spritecollideany( shipGob, all_rockpiles )
        if hitSprite != None:
            # setup explosion 2
            explosionGob2.rect.x = shipGob.rect.x
            explosionGob2.rect.y = shipGob.rect.y
            explosionGob2.animDur = 3
            explosionGob2.visible = True
            explosionGob2.currentFrameNum = 0;
            explosionGob2.setSpriteFrame()

            # setup explosion 3
            explosionGob3.rect.x = shipGob.rect.x + explosionGob2.rect.width
            explosionGob3.rect.y = shipGob.rect.y
            explosionGob3.animDur = 3
            explosionGob3.visible = True
            explosionGob3.currentFrameNum = 0;
            explosionGob3.setSpriteFrame()

            # Hide ship
            shipGob.vy = 999
            shipGob.disableAfterOutOfScreen = True

            gameMode="lost game"
            exitLoop = True

        ### Update and Draw


        # update all gobs
        all_sprites.update()

        # go round
        if shipGob.rect.x > screenW:
            shipGob.rect.x = -shipGob.rect.width
            shipGob.rect.y += 14

        # Win ?
        #if (shipGob.rect.y > (screenH-25)
        #        and shipGob.rect.x > 100):
        #    vx = 0  # land here!
        #    gameMode="level done"
        #    exitLoop = True

        # Draw gobs
        all_gobs.draw(screen)

        # Draw screen
        pygame.display.update(True)

# *** MAIN LEVEL DONE
def MainLevelDone():

    global gameMode
    global g_level
    global g_ship_speed

    pok.draw_text(50, 30, " LEVEL COMPLETED!", 1)
    pygame.display.update(False)

    exitLoop = False
    shipGob.setvel(0,0); # stop ship
    while not exitLoop:

        # read keys
        eventtype = pygame.event.poll()
        if eventtype != pygame.NOEVENT:
            if eventtype.type == pygame.KEYDOWN:
                if eventtype.key == pygame.BUT_A:
                    # Create whole level
                    g_level+=1
                    if g_level>6:
                        g_level = 0
                        g_ship_speed += 1
                    RestartLevel()
                    gameMode="play"
                    exitLoop = True

        ### Update and Draw

        # update all gobs
        all_sprites.update()

        # Draw gobs
        all_gobs.draw(screen)

        # Draw screen
        pygame.display.update(True)

# *** MAIN GAME LOST
def MainLostGame():

    global gameMode
    global points
    global g_level
    global g_ship_speed
    global points

    pok.draw_text(60, 30, "   YOU LOST!", 1)
    text = "YOUR SCORE: " + str(points)
    pok.draw_text(60, 70, text, 1)
    pygame.display.update(False)

    exitLoop = False
    shipGob.setvel(0,0); # stop ship
    while not exitLoop:

        # read keys
        eventtype = pygame.event.poll()
        if eventtype != pygame.NOEVENT:
            if eventtype.type == pygame.KEYDOWN:
                if eventtype.key == pygame.BUT_A:
                    # Create whole level
                    g_level = 0
                    g_ship_speed = 2
                    points = 0
                    RestartLevel()
                    gameMode="play"
                    exitLoop = True
                    points = 0

        ### Update and Draw

        # update all gobs
        all_sprites.update()

        # Draw gobs
        all_gobs.draw(screen)

        # Draw screen
        pygame.display.update(True)

def RestartLevel():

    # Clear sprite lists
    #all_sprites.empty()
    #all_rockpiles.empty()
    #all_gobs.empty()

    # Set palette
    pygame.display.set_palette_16bit(g_levelParam[g_level][0])

    print('create level. g_level=',g_level)

    # Create rock pile sprites
    CreateRockPileSprites(g_levelParam[g_level][1], g_levelParam[g_level][2], g_levelParam[g_level][3], g_levelParam[g_level][4], g_levelParam[g_level][5])

     # Init sprites
    InitSprites()

    all_rockpiles.update()

    # draw screen once.
    #screen.fill(0)
    # Draw background
    screen.blit(gamedata.backgroundSurf, 0, 0, 15) # no invisible color
    all_rockpiles.draw(screen)
    pygame.display.update(False)
    #pygame.display.update(False)


# Callback which is called when the gob anim cycle is finished
def HideGobAfterAnimCycle(gob):
    # Note: only move y so that the dirty rect for a sprite is optimum.
    gob.vy = 999

def IsSpriteCollided(sprite, s):
    return( sprite.collRect2.colliderect( s.collRect2 ) )

#
def ChangeRockPile(gob, numClones):
    gob.frames[0] = gob.frames[1] # head rock is the same as body rock
    gob.image = gob.frames[1]
    gob.numClones = numClones
    gob.rect.height = gob.image.get_rect().height
    gob.rect.width = gob.image.get_rect().width
    gob.rect.y = screenH - 5 - (gob.numClones*gob.frames[1].get_rect().height) - gob.frames[0].get_rect().height

    #
    gob.collRect2.x = gob.rect.x + gob.collRect.x
    gob.collRect2.y = gob.rect.y + gob.collRect.y
    gob.collRect2.width = gob.collRect.width
    gob.collRect2.height = gob.collRect.height

def CreateRockPileSprites(numPiles, rockPileSize, rockSF, rockHeadSF, rockHead1CollRect):

    # Create rockpile sprites
    #numPiles = (screenW - (8*2)) // (gamedata.rockSurf_f0.get_rect().width-2+5)
    #pileIndexList = [0,1,2,3,4,5,6,7]
    #towerSurfaceList = [
    #    gamedata.rock1Surf, gamedata.rock2Surf, gamedata.rock3Surf, gamedata.rock4Surf,
    #    gamedata.rock5Surf, gamedata.rock6Surf, gamedata.rock7Surf, gamedata.rock7Surf ]
    #towerHeadSurfaceList = [
    #    gamedata.rockHead1Surf, gamedata.rockHead2Surf, gamedata.rockHead3Surf, gamedata.rockHead4Surf,
    #    gamedata.rockHead5Surf, gamedata.rockHead6Surf, gamedata.rockHead7Surf, gamedata.rock7Surf ]
    # numPiles = len(pileIndexList)
    #for i in range(numPiles):

    global g_rockpileGobList

    for i in range(numPiles):
        #rockSF = towerSurfaceList[pileIndexList[i]]

        # Only put head to some towers
        #if(random.getrandbits(3) > 3 ):
        #if True:
        #    rockHeadSF = towerHeadSurfaceList[pileIndexList[i]]
        #else:
        #   rockHeadSF = rockSF

        #rockSF = gamedata.rock1Surf
        #rockHeadSF = gamedata.rockHead1Surf

        # Create gob
        gob = None
        if len(g_rockpileGobList) <= i:
            gob = gameclass.GameObject([rockHeadSF, rockSF],[[0,0], [0,0]], screen, screenW, screenH)
            g_rockpileGobList.append(gob)
            all_sprites.add(g_rockpileGobList[i])
            all_rockpiles.add(g_rockpileGobList[i])
        else:
            gob = g_rockpileGobList[i]
            gob.frames = [rockHeadSF, rockSF]
            gob.image = gob.frames[gob.currentFrameNum]  # current image
            gob.rect = gob.frames[0].get_rect()

        # Set pos
        gob = g_rockpileGobList[i]
        gob.rect.x = 30 + i*(gob.rect.width+4)
        gob.collRect = rockHead1CollRect
        gob.collRect2 = pygame.Rect(0,0,0,0)
        gob.numClones = random.getrandbits(2) + rockPileSize # =2
        gob.rect.y = screenH - 5 - (gob.numClones*gob.frames[1].get_rect().height) - gob.frames[0].get_rect().height
        gob.visible = True

        #
        gob.collRect2.x = gob.rect.x + gob.collRect.x
        gob.collRect2.y = gob.rect.y + gob.collRect.y
        gob.collRect2.width = gob.collRect.width
        gob.collRect2.height = gob.collRect.height

# *** MAIN
def InitSprites():

    global shipGob
    global bombGob
    global explosionGob
    global explosionGob2
    global explosionGob3

    # Create ship
    if shipGob == None:
        shipGob = gameclass.GameObject([gamedata.shipF1Surf], [[0,0]], screen, screenW, screenH)
        all_sprites.add(shipGob)
        all_gobs.add(shipGob)
        shipGob.setSprite(0, [[60683,32963,49572,65502]])
    shipGob.rect.x = 50
    shipGob.rect.y = 3
    shipGob.animDur = 0;
    shipGob.disableAfterOutOfScreen = False
    shipGob.visible = True

    # Create bomb
    if bombGob == None:
        bombGob = gameclass.GameObject([gamedata.bombSurf], [[0,0]], screen, screenW, screenH)
        bombGob.collRect = gamedata.bombCollRect
        bombGob.collRect2 = pygame.Rect(0,0,0,0)
        all_sprites.add(bombGob)
        all_gobs.add(bombGob)
    bombGob.setSprite(1, [gamedata.bombColorMap])
    bombGob.rect.x = 999
    bombGob.rect.y = 999

    # Create explosion
    if explosionGob == None:
        explosionGob = gameclass.GameObject(
            [gamedata.explosion1f1Surf, gamedata.explosion1f2Surf, gamedata.explosion1f3Surf, gamedata.explosion1f4Surf],
            [[0,0],[0,0],[0,0],[0,0]], screen, screenW, screenH
        )
        all_sprites.add(explosionGob)
        all_gobs.add(explosionGob)
        explosionGob.setSprite(2, [gamedata.explosion1ColorMap, gamedata.explosion1ColorMap, gamedata.explosion1ColorMap, gamedata.explosion1ColorMap])
    explosionGob.rect.x = 999
    explosionGob.rect.y = 999
    explosionGob.shouldCycleAnim = False;
    explosionGob.callAfterAnimCycle = HideGobAfterAnimCycle
    explosionGob.setSpriteFrame()
    explosionGob.visible = False

    # Create explosion 2
    if explosionGob2 == None:
        explosionGob2 = gameclass.GameObject(
            [gamedata.explosion1f1Surf, gamedata.explosion1f2Surf, gamedata.explosion1f3Surf, gamedata.explosion1f4Surf],
            [[0,0],[0,0],[0,0],[0,0]], screen, screenW, screenH
        )
        all_sprites.add(explosionGob2)
        all_gobs.add(explosionGob2)
        explosionGob2.setSprite(3, [gamedata.explosion1ColorMap, gamedata.explosion1ColorMap, gamedata.explosion1ColorMap, gamedata.explosion1ColorMap])
    explosionGob2.rect.x = 999
    explosionGob2.rect.y = 999
    explosionGob2.shouldCycleAnim = False;
    explosionGob2.callAfterAnimCycle = HideGobAfterAnimCycle
    explosionGob2.setSpriteFrame()
    explosionGob2.visible = False

    # Create explosion 3
    if explosionGob3 == None:
        explosionGob3 = gameclass.GameObject(
            [gamedata.explosion1f1Surf, gamedata.explosion1f2Surf, gamedata.explosion1f3Surf, gamedata.explosion1f4Surf],
            [[0,0],[0,0],[0,0],[0,0]], screen, screenW, screenH
        )
        all_sprites.add(explosionGob3)
        all_gobs.add(explosionGob3)
        explosionGob3.setSprite(4, [gamedata.explosion1ColorMap, gamedata.explosion1ColorMap, gamedata.explosion1ColorMap, gamedata.explosion1ColorMap])
    explosionGob3.rect.x = 999
    explosionGob3.rect.y = 999
    explosionGob3.shouldCycleAnim = False;
    explosionGob3.callAfterAnimCycle = HideGobAfterAnimCycle
    explosionGob3.setSpriteFrame()
    explosionGob3.visible = False



# Draw points and update rect
def DrawPoints():

    dirtyRect = pygame.Rect(10, 2, 8*4, 10)
    screen.fill(0, dirtyRect)

    # Draw points
    pok.draw_text(10, 2, str(points), 1)

    # Update dirty rect
    #print('rect=', updateRect)
    pygame.display.update(False, dirtyRect, True)


# **** START MAIN
main()
