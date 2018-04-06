
import gc
import upygame as pygame
import urandom as random
import sprite
import marsattack_data as gamedata
import marsattack_classes as gameclass
import umachine as pok

print('START')

# Init screen
pygame.display.init()
screen = pygame.display.set_mode() # full screen
screenRect = screen.get_rect()
screenW = screenRect.width
screenH = screenRect.height
# Copyright (C) 2018 Hannu Viitala
# This file is released under MIT license.
# Go to http://opensource.org/licenses/MIT for full license details.

# Create global objects
shipGob = None
bombGob = None
explosionGob = None
explosionGob2 = None
explosionGob3 = None
g_rockpileGobList = []
gameMode="start"
points = 0
g_rockPileSize = 1;
g_numPiles = 8;
g_ship_speed = 2
g_level = 0
g_sound = pygame.mixer.Sound()
#g_soundBufferSize = 512*8  # TODO get this from API
g_soundBufferSize = 512*2  # TODO get this from API
test_num_piles = 8
g_debug_mode = 0

# Level definitions
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

# Main function
def main():

    while True:

        # Choose mode
        if gameMode=="start":
            MainStartScreen()
        elif gameMode=="level done":
            MainLevelDone()
        elif gameMode=="lost game":
            MainLostGame()
        else: # gameMode="play"
            MainPlay()

        # Create the whole level
        RestartLevel()

        # Draw points
        DrawPoints()

def scrollText(y, text, color):
    #if y>34 and y < screenH - 10:
    #    pok.draw_text(4, y, text, color)
    return y+10

def generateBytebeatSound(bufferIndex, startT):
    pos = 0
    for t in range(startT, startT + g_soundBufferSize):

        x = ((t<<1)^((t<<1)+(t>>7)&t>>12))|t>>(4-(1^7&(t>>19)))|t>>7 # perus

        #x = (t|(t>>9|t>>7))*t&(t>>11|t>>9)

        #if t>>6&1:
        #    x = t>>5
        #else:
        #    x = -t>>4

        # https://www.reddit.com/r/bytebeat/comments/20km9l/cool_equations/
        #x = (t>>8&t)*(t>>15&t) # hyvä, vähän ambient
        #x = (t%255&t)-(t>>13&t) #ambientin tapainen
        #x = ((2*(t&1)-1)*t)-(t>>8) # tosi simppeli ambient
        #x = t*(42&t>>10)
        #x = (t>>13|t*24)&(t>>7|t*19)

        g_sound.fill_buffer(bytes([255&x]), 1, bufferIndex, pos)
        pos += 1

# Start screen, main loop
def MainStartScreen():

    global gameMode
    global points
    global g_level
    global g_ship_speed
    global points

    # Background music
    # pre-fill 4 beffers
    startT = 0
    generateBytebeatSound(0, startT)
    startT += g_soundBufferSize
    generateBytebeatSound(1, startT)
    startT += g_soundBufferSize
    generateBytebeatSound(2, startT)
    startT += g_soundBufferSize
    generateBytebeatSound(3, startT)
    startT += g_soundBufferSize
    nextBufferIndexToFill = 0
    g_sound.reset()

    # Set initial palette
    pygame.display.set_palette_16bit([0,65502,49572,65502]);

    # Draw title text
    pok.draw_text(40, 10, "M A R S   A T T A C K", 1)
    pygame.display.update(False)

    # Stop ship
    #shipGob.setvel(0,0);

    # Main loop
    exitLoop = False
    textTopY_fp = 176*1000
    while not exitLoop:

        # Read keys
        eventtype = pygame.event.poll()
        if eventtype != pygame.NOEVENT:
            if eventtype.type == pygame.KEYDOWN:
                if eventtype.key == pygame.BUT_A:

                    # Start the game from the first level
                    g_level = 0
                    g_ship_speed = 2
                    points = 0
                    RestartLevel()
                    gameMode = "play"
                    exitLoop = True
                    points = 0

        # Clear text area
        #screen.fill(0, pygame.Rect(0, 35, screenW, screenH-35))

        # Draw scolled text
        #                         "1234567890123456789012345"
        textY = (textTopY_fp // 1000);
        textY = scrollText(textY, "Over the centuries humans", 2)
        textY = scrollText(textY, "have send thousands of", 2)
        textY = scrollText(textY, "autonomous robots to", 2)
        textY = scrollText(textY, "Mars. Their mission was", 2)
        textY = scrollText(textY, "to prepare the planet for", 2)
        textY = scrollText(textY, "the human colonization. ", 2)
        textY = scrollText(textY, "", 2)
        textY = scrollText(textY, "However, the reports the", 2)
        textY = scrollText(textY, "robots sent to Earth", 2)
        textY = scrollText(textY, "were always", 2)
        textY = scrollText(textY, "discouraging. Despite of", 2)
        textY = scrollText(textY, "numerous efforts, Mars", 2)
        textY = scrollText(textY, "seemed to be a too", 2)
        textY = scrollText(textY, "hostile planet for", 2)
        textY = scrollText(textY, "humans...", 2)
        textY = scrollText(textY, "", 2)
        textY = scrollText(textY, "", 2)
        textY = scrollText(textY, "", 2)
        textY = scrollText(textY, "", 2)
        textY = scrollText(textY, "", 2)
        textY = scrollText(textY, "", 2)
        textY = scrollText(textY, "", 2)
        textY = scrollText(textY, "", 2)
        textY = scrollText(textY, "But that was all a big", 3)
        textY = scrollText(textY, "lie!", 3)
        textY = scrollText(textY, "", 2)
        textY = scrollText(textY, "During the centuries in", 2)
        textY = scrollText(textY, "Mars, the robots had", 2)
        textY = scrollText(textY, "developed an artificial", 2)
        textY = scrollText(textY, "intelligence. Their plan", 2)
        textY = scrollText(textY, "is to build military", 2)
        textY = scrollText(textY, "forces in secrecy, and", 2)
        textY = scrollText(textY, "take the planet to", 2)
        textY = scrollText(textY, "themselves.", 2)
        textY = scrollText(textY, "", 2)
        textY = scrollText(textY, "Then they would turn", 2)
        textY = scrollText(textY, "against their creators", 2)
        textY = scrollText(textY, "and attack to Earth! By a", 2)
        textY = scrollText(textY, "lucky accident, humans", 2)
        textY = scrollText(textY, "got to know their plans", 2)
        textY = scrollText(textY, "before it was too late.", 2)
        textY = scrollText(textY, "", 2)
        textY = scrollText(textY, "Now, your mission is to", 2)
        textY = scrollText(textY, "bomber down all the gun", 2)
        textY = scrollText(textY, "towers the robots have", 2)
        textY = scrollText(textY, "build on the surface of", 2)
        textY = scrollText(textY, "Mars.", 2)
        textY = scrollText(textY, "", 2)
        textY = scrollText(textY, "Unfortunately, due the", 2)
        textY = scrollText(textY, "long trip from Earth to", 2)
        textY = scrollText(textY, "the Mars orbit, you do", 2)
        textY = scrollText(textY, "not have enough fuel for", 2)
        textY = scrollText(textY, "your armed space shuttle,", 2)
        textY = scrollText(textY, "and you are constantly", 2)
        textY = scrollText(textY, "losing height!", 2)
        textY = scrollText(textY, "", 2)
        textY = scrollText(textY, "The destiny of humankind", 2)
        textY = scrollText(textY, "is on your shoulders...", 2)
        textY = scrollText(textY, "", 2)
        textY = scrollText(textY, "", 2)
        textY = scrollText(textY, "", 2)
        textY = scrollText(textY, "", 2)
        textY = scrollText(textY, "", 2)
        textY = scrollText(textY, "", 2)

        # Update states of all gobs
        #all_sprites.update()

        # Draw gobs
        #all_gobs.draw(screen)

        # Draw screen
        #pygame.display.update(False)
        pygame.display.update(True)

        # Draw display to the screen hw
        #dirtyRect = pygame.Rect(10, 2, 12, 10)
        #pygame.display.update(False, dirtyRect, True)  # Draw now, draw only

        textTopY_fp -= 300;

        # Pre-fill sound buffer if not currently used
        if nextBufferIndexToFill != g_sound.get_current_buffer_index():
            generateBytebeatSound(nextBufferIndexToFill, startT)
            startT += g_soundBufferSize
            nextBufferIndexToFill += 1
            if nextBufferIndexToFill > 3:
                nextBufferIndexToFill = 0

# *** Playing main loop
def MainPlay():

    global gameMode
    global points
    global g_level

    # Init
    vx = g_ship_speed;
    vy = 0;
    exitLoop = False
    destroyedPilesCount = 0

    # Main loop
    while not exitLoop:

        # read keys
        eventtype = pygame.event.poll()
        if eventtype != pygame.NOEVENT:
            if eventtype.type == pygame.KEYDOWN:
                if eventtype.key == pygame.BUT_A:

                    # Drop bomb if it is not visible already
                    if not bombGob.visible:
                        bombGob.rect.x = shipGob.rect.x + ((shipGob.rect.width- bombGob.rect.width)//2)
                        bombGob.rect.y = shipGob.rect.y + shipGob.rect.height
                        bombGob.setvel(0,2)
                        bombGob.visible = True

                # Debug: Level up
                if g_debug_mode>0 and eventtype.key == pygame.K_UP:
                    g_level+=1
                    if g_level > 6:
                        g_level = 6;
                    RestartLevel()

                # Debug: Level down
                if g_debug_mode>0 and eventtype.key == pygame.K_DOWN:
                    g_level-=1
                    if g_level < 0:
                        g_level = 0;
                    RestartLevel()

        # Set speed
        shipGob.setvel(vx,vy)

        # Init collision rect for the bomb
        bombGob.collRect2.x = bombGob.rect.x + bombGob.collRect.x
        bombGob.collRect2.y = bombGob.rect.y + bombGob.collRect.y
        bombGob.collRect2.width = bombGob.collRect.width
        bombGob.collRect2.height = bombGob.collRect.height

        # Check bomb collision to rockpile
        hitSprite = sprite.spritecollideany(bombGob, all_rockpiles, IsSpriteCollided)
        if hitSprite != None:

            # Setup explosion
            explosionGob.rect.x = hitSprite.rect.x
            explosionGob.rect.y = hitSprite.rect.y
            explosionGob.vx = 0
            explosionGob.vy = 0
            explosionGob.animDur = 1
            explosionGob.visible = True
            explosionGob.currentFrameNum = 0;
            explosionGob.setSpriteFrame()

            # Calc dirty rect of the rockpile
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

            # Redraw background color in the dirty rect area only
            screen.fill(0, dirtyRect) # TODO: can this be removed?

            # Update pile state
            hitSprite.update()

            # Draw background image in the dirty rect area only
            screen.set_clip(dirtyRect);
            screen.blit(gamedata.backgroundSurf, 0, 0, 15) # no invisible color
            screen.set_clip();

            # Draw the rock pile
            y = hitSprite.rect.y
            screen.blit(hitSprite.image, hitSprite.rect.x, y) # blit head hwsprite
            y += hitSprite.rect.height
            rockSF = hitSprite.frames[1]
            screen.blit(rockSF, hitSprite.rect.x, y) # blit body hwsprite

            # Draw display to the screen hw
            pygame.display.update(False, dirtyRect, True)  # Draw now, draw only

            # Draw points
            points += 10
            DrawPoints()

        # Check the ship collision to rockpile
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

            # Exit and move to "lost game" state
            gameMode = "lost game"
            exitLoop = True

        # update all gobs
        all_sprites.update()

        # Go round
        if shipGob.rect.x > screenW:
            shipGob.rect.x = -shipGob.rect.width
            shipGob.rect.y += 14

        # Draw gobs
        all_gobs.draw(screen)

        # Draw the screen. do not use buffered draw.
        pygame.display.update(True)

# Level done, main loop
def MainLevelDone():

    global gameMode
    global g_level
    global g_ship_speed

    # Draw text
    pok.draw_text(50, 30, " LEVEL COMPLETED!", 1)
    pygame.display.update(False)

    # Stop ship
    shipGob.setvel(0,0);

    # Main loop
    exitLoop = False
    while not exitLoop:

        # read keys
        eventtype = pygame.event.poll()
        if eventtype != pygame.NOEVENT:
            if eventtype.type == pygame.KEYDOWN:
                if eventtype.key == pygame.BUT_A:

                    # Start the next level
                    g_level+=1
                    if g_level>6:
                        g_level = 0
                        g_ship_speed += 1
                    RestartLevel()
                    gameMode = "play"
                    exitLoop = True

        # Update states of all gobs
        all_sprites.update()

        # Draw gobs
        all_gobs.draw(screen)

        # Draw screen
        pygame.display.update(True)

# Game lost, main loop
def MainLostGame():

    global gameMode
    global points
    global g_level
    global g_ship_speed
    global points

    # Draw text
    pok.draw_text(60, 30, "   YOU LOST!", 1)
    text = "YOUR SCORE: " + str(points)
    pok.draw_text(60, 70, text, 1)
    pygame.display.update(False)

    # Stop ship
    shipGob.setvel(0,0);

    # Main loop
    exitLoop = False
    while not exitLoop:

        # Read keys
        eventtype = pygame.event.poll()
        if eventtype != pygame.NOEVENT:
            if eventtype.type == pygame.KEYDOWN:
                if eventtype.key == pygame.BUT_A:

                    # Restart the game from the first level
                    g_level = 0
                    g_ship_speed = 2
                    points = 0
                    RestartLevel()
                    gameMode = "play"
                    exitLoop = True
                    points = 0

        # Update states of all gobs
        all_sprites.update()

        # Draw gobs
        all_gobs.draw(screen)

        # Draw screen
        pygame.display.update(True)

# Create level
def RestartLevel():

    # Set palette
    pygame.display.set_palette_16bit(g_levelParam[g_level][0])

    # Create rock pile sprites
    CreateRockPileSprites(g_levelParam[g_level][1], g_levelParam[g_level][2], g_levelParam[g_level][3], g_levelParam[g_level][4], g_levelParam[g_level][5])

     # Init sprites
    InitSprites()

    # Update states of all piles
    all_rockpiles.update()

    # Draw background image
    screen.blit(gamedata.backgroundSurf, 0, 0, 15) # no invisible color

    # Draw piles
    all_rockpiles.draw(screen)

    # Draw screen
    pygame.display.update(False)

# Callback which is called when the gob anim cycle is finished
def HideGobAfterAnimCycle(gob):
    # Note: only move y so that the dirty rect for a sprite is optimum.
    gob.vy = 999

# Callback which is called when a sprite collision must be checked
def IsSpriteCollided(sprite, s):
    return( sprite.collRect2.colliderect( s.collRect2 ) )

# Change the pile height
def ChangeRockPile(gob, numClones):

    # Reinit the pile
    gob.frames[0] = gob.frames[1] # head rock is the same as body rock
    gob.image = gob.frames[1]
    gob.numClones = numClones
    gob.rect.height = gob.image.get_rect().height
    gob.rect.width = gob.image.get_rect().width
    gob.rect.y = screenH - 5 - (gob.numClones*gob.frames[1].get_rect().height) - gob.frames[0].get_rect().height

    # Collision rect
    gob.collRect2.x = gob.rect.x + gob.collRect.x
    gob.collRect2.y = gob.rect.y + gob.collRect.y
    gob.collRect2.width = gob.collRect.width
    gob.collRect2.height = gob.collRect.height

# Create the rock pile sprites
def CreateRockPileSprites(numPiles, rockPileSize, rockSF, rockHeadSF, rockHead1CollRect):

    global g_rockpileGobList

    for i in range(numPiles):

        # Create or initialize gob
        gob = None
        if len(g_rockpileGobList) <= i:
            # Create gob
            gob = gameclass.GameObject([rockHeadSF, rockSF],[[0,0], [0,0]], screen, screenW, screenH)
            g_rockpileGobList.append(gob)
            all_sprites.add(g_rockpileGobList[i])
            all_rockpiles.add(g_rockpileGobList[i])
        else:
            # Initialize gob
            gob = g_rockpileGobList[i]
            gob.frames = [rockHeadSF, rockSF]
            gob.image = gob.frames[gob.currentFrameNum]  # current image
            gob.rect = gob.frames[0].get_rect()

        # Initialize the rock pile
        gob = g_rockpileGobList[i]
        gob.rect.x = 30 + i*(gob.rect.width+4)
        gob.collRect = rockHead1CollRect
        gob.collRect2 = pygame.Rect(0,0,0,0)
        gob.numClones = random.getrandbits(2) + rockPileSize # =2
        gob.rect.y = screenH - 5 - (gob.numClones*gob.frames[1].get_rect().height) - gob.frames[0].get_rect().height
        gob.visible = True

        # Set the collision rect
        gob.collRect2.x = gob.rect.x + gob.collRect.x
        gob.collRect2.y = gob.rect.y + gob.collRect.y
        gob.collRect2.width = gob.collRect.width
        gob.collRect2.height = gob.collRect.height

# Create or initialize all sprites
def InitSprites():

    global shipGob
    global bombGob
    global explosionGob
    global explosionGob2
    global explosionGob3

    # Create or initialize ship
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

    # Create or initialize bomb
    if bombGob == None:
        bombGob = gameclass.GameObject([gamedata.bombSurf], [[0,0]], screen, screenW, screenH)
        bombGob.collRect = gamedata.bombCollRect
        bombGob.collRect2 = pygame.Rect(0,0,0,0)
        all_sprites.add(bombGob)
        all_gobs.add(bombGob)
    bombGob.setSprite(1, [gamedata.bombColorMap])
    bombGob.rect.x = 999
    bombGob.rect.y = 999

    # Create or initialize explosion
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

    # Create or initialize explosion 2
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

    # Create or initialize explosion 3
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

# Draw points
def DrawPoints():

    dirtyRect = pygame.Rect(10, 2, 8*4, 10)
    screen.fill(0, dirtyRect)

    # Draw points
    pok.draw_text(10, 2, str(points), 1)

    # Update dirty rect
    pygame.display.update(False, dirtyRect, True)


# Main
main()
