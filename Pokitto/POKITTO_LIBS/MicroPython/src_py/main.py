
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
g_soundBufferSize = g_sound.get_soundbuffer_size()
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

def generateBytebeatSound(bufferIndex, startT):
    pos = 0
    arr = bytearray(g_soundBufferSize)
    for t in range(startT, startT + g_soundBufferSize):

        #arr[pos] = t & (t>>8)
        #arr[pos] = ((t<<1)^((t<<1)+(t>>7)&t>>12))|t>>(4-(1^7&(t>>19)))|t>>7 # perus # hyvä, ei toimi 16khz

        #arr[pos] = (t|(t>>9|t>>7))*t&(t>>11|t>>9)

        #if t>>6&1:
        #    x = t>>5
        #else:
        #    x = -t>>4

        # https://www.reddit.com/r/bytebeat/comments/20km9l/cool_equations/
        #arr[pos] = (t>>8&t)*(t>>15&t) # hyvä, vähän ambient
        #arr[pos] = (t%255&t)-(t>>13&t) #ambientin tapainen. # Hyvä, toimii 16 khz
        #arr[pos] = ((2*(t&1)-1)*t)-(t>>8) # tosi simppeli ambient #kokeilie. toimii 16 khz
        #arr[pos] = t*(42&t>>10)
        #arr[pos] = (t>>13|t*24)&(t>>7|t*19)

        # https://github.com/kragen/viznut-music
        #arr[pos] = ((t*(t>>8|t>>9)&46&t>>8))^(t&t>>13|t>>6) # ihan ok, 16 khz
        #arr[pos] = t*5&(t>>7)|t*3&(t*4>>10) # hyvä, mutta vähän tylsä 16 khz
        #arr[pos] = t*((t>>5|t>>8)>>(t>>16))
        #arr[pos] = t*((t>>12|t>>8)&63&t>>4) # hyvä, mutta aggressiivinen. 11 khz

        arr[pos] = t * ((t>>7|t>>3)&27&t>>13)

        pos += 1

    # Send data
    g_sound.fill_buffer(arr, g_soundBufferSize, bufferIndex, 0)

def printTextLine(drawTextLineNum, textList):
    #print('drawTextLineNum=',drawTextLineNum)
    #print('textList=',textList[drawTextLineNum])
    if drawTextLineNum>-1 and drawTextLineNum<len(textList):
        x = 4
        offsetY = 50
        textLine = drawTextLineNum % 12
        y = textLine*10 + offsetY
        dirtyRect = pygame.Rect(x, y, screenW, 10)
        #screen.fill(0, dirtyRect)
        #print('drawTextLineNum2=',drawTextLineNum)
        pok.draw_text(x, y, textList[drawTextLineNum], 2)
        pygame.display.update(False, dirtyRect, True)

def CreateTexts():

    textList = []
    textList.append("Over the centuries humans")
    textList.append("have send thousands of")
    textList.append("autonomous robots to")
    textList.append("Mars. Their mission was")
    textList.append("to prepare the planet for")
    textList.append("the human colonization. ")
    textList.append("")
    textList.append("However, the reports the")
    textList.append("robots sent to Earth")
    textList.append("were always")
    textList.append("discouraging. Despite of")
    textList.append("numerous efforts, Mars")
    textList.append("seemed to be a too")
    textList.append("hostile planet for")
    textList.append("humans...")
    textList.append("")
    textList.append("But that was all a big")
    textList.append("lie!")
    textList.append("")
    textList.append("During the centuries in")
    textList.append("Mars, the robots had")
    textList.append("developed an artificial")
    textList.append("intelligence. Their plan")
    textList.append("is to build military")
    textList.append("forces in secrecy, and")
    textList.append("take the planet to")
    textList.append("themselves.")
    textList.append("")
    textList.append("Then they would turn")
    textList.append("against their creators")
    textList.append("and attack to Earth! By a")
    textList.append("lucky accident, humans")
    textList.append("got to know their plans")
    textList.append("before it was too late.")
    textList.append("")
    textList.append("Now, your mission is to")
    textList.append("bomber down all the gun")
    textList.append("towers the robots have")
    textList.append("build on the surface of")
    textList.append("Mars.")
    textList.append("")
    textList.append("Unfortunately, due the")
    textList.append("long trip from Earth to")
    textList.append("the Mars orbit, you do")
    textList.append("not have enough fuel for")
    textList.append("your armed space shuttle,")
    textList.append("and you are constantly")
    textList.append("losing height!")
    textList.append("")
    textList.append("The destiny of humankind")
    textList.append("is on your shoulders...")
    textList.append("")
    textList.append("")
    textList.append("")
    textList.append("")
    textList.append("")
    textList.append("")
    textList.append("Coding by Hannu Viitala.")
    textList.append("Graphics by")
    textList.append("Jari-Pekka Flinck.")
    textList.append("ByteBeat music algorithm")
    textList.append("by x.")

    return textList

# Start screen, main loop
def MainStartScreen():

    global gameMode
    global points
    global g_level
    global g_ship_speed
    global points

    # Set palette
    pygame.display.set_palette_16bit([0,47556,60264,59196])

    # Draw background image
    screen.blit(gamedata.logoSurf, 8, 5, 0) # no invisible color

    # Clear text area
    #screen.fill(0, pygame.Rect(0, 35, screenW, screenH-35))

    # Draw scolled text
    #                         "1234567890123456789012345"
    #textY = (printTextAtPos_fp // 1000);
    #textY = 30

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
    g_sound.play()

    # Set initial palette
    #pygame.display.set_palette_16bit([0,65502,49572,65502]);

    # Draw title text
    #pok.draw_text(40, 10, "M A R S   A T T A C K", 1)
    pygame.display.update(False)

    # Stop ship
    #shipGob.setvel(0,0);

    textList = CreateTexts()

    # Main loop
    exitLoop = False
    printTextAtPos_fp = 0
    lastDrawnLineNum = -1
    while not exitLoop:

        # Read keys
        eventtype = pygame.event.poll()
        if eventtype != pygame.NOEVENT:
            if eventtype.type == pygame.KEYDOWN:
                if eventtype.key == pygame.BUT_A:

                    # Start the game from the first level
                    g_sound.pause()
                    g_level = 0
                    g_ship_speed = 2
                    points = 0
                    RestartLevel()
                    gameMode = "play"
                    exitLoop = True
                    points = 0

        # Draw current pos in soud buffer
        #dirtyRect = pygame.Rect(50, 2, 8*4, 10)
        #screen.fill(0, dirtyRect)
        # Draw points
        #text = str(g_sound.get_current_soundbuffer_index())+", "+str(nextBufferIndexToFill)
        #pok.draw_text(50, 2, text, 1)
        #print(text)
        # Update dirty rect
        #pygame.display.update(False, dirtyRect, True)


        # Update states of all gobs
        #all_sprites.update()

        # Draw gobs
        #all_gobs.draw(screen)

        drawTextLineNum = printTextAtPos_fp//1000
        #print('drawTextLineNum',drawTextLineNum)
        if lastDrawnLineNum < drawTextLineNum:
            if drawTextLineNum % 12 == 0:
                screen.fill(0, pygame.Rect(0, 40, screenW, screenH-40))
                pygame.display.update(False)

            printTextLine(drawTextLineNum, textList)
            lastDrawnLineNum = drawTextLineNum

        #
        pygame.display.update(True)

        # Draw screen
        #pygame.display.update(False)


        # Draw display to the screen hw
        #dirtyRect = pygame.Rect(10, 2, 12, 10)
        #pygame.display.update(False, dirtyRect, True)  # Draw now, draw only

        #print('lastDrawnLineNum=',lastDrawnLineNum)
        printTextAtPos_fp += 50;
        if drawTextLineNum > 75:
            printTextAtPos_fp = 0
            lastDrawnLineNum = -1

        # Pre-fill sound buffer if not currently used
        while nextBufferIndexToFill != g_sound.get_current_soundbuffer_index():
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
