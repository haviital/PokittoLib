
# Memory usage follow up:
# free 7168 b
# free 6320 b: add 10 laser particles
# free 6000 b: 20 laser particles
# free 5648 b: diffrerent laser bitmaps for each direction(8). DrawFpsDots.

import upygame as pygame
import urandom as random
import example_data as spritedata
import sprite
import gc
import umachine
from micropython import const

SCREEN_W = const(110)
SCREEN_H = const(88)
TILE_W = const(8)
TILE_ITEM_W = const(16)
TILE_H = const(8)
TILEMAP_W = const(16) # in tiles
TILEMAP_H = const(16) # in tiles
TILEMAP_W_ITEMS = const(TILEMAP_W//2) # in items. One item contais 2 tiles.
TILEMAP_W_PX = const(TILEMAP_W*TILE_W)
TILEMAP_H_PX = const(TILEMAP_H*TILE_H)
HERO_VP_MOVE_TLX  = const(40)
HERO_VP_MOVE_TLY  = const(40)
HERO_VP_MOVE_BRX  = const(SCREEN_W-40)
HERO_VP_MOVE_BRY  = const(SCREEN_H-40)

# Joyhat
# mbed::DigitalOut jh_rumble(EXT0);
rumbleMotor = umachine.Pin ( umachine.Pin.EXT0, umachine.Pin.OUT )
rumbleMotor.off()
joyhatX = umachine.Pin ( umachine.Pin.EXT1, umachine.Pin.ANALOG_IN ) #JHAT
joyhatY = umachine.Pin ( umachine.Pin.EXT2, umachine.Pin.ANALOG_IN ) #JHAT
#joyhatB1 = umachine.Pin ( umachine.Pin.EXT15, umachine.Pin.IN )
#joyhatB2 = umachine.Pin ( umachine.Pin.EXT14, umachine.Pin.IN )
test = joyhatY.value()
print("joyhatY.value()=",test)
isHW = True
if(test==0 or test==-1):
    isHW = False
print("isHW=",isHW)


pygame.display.init(False)
pygame.display.set_palette_16bit([
    0, 6438, 0x780f, 0x03ef, 0x7800, 0x001f, 0xf800, 0x7bef,
    58225, 13598, 0xf81f, 0x07ff, 0x07e0, 0x7be0, 0xffe0, 65535
]);

screen = pygame.display.set_mode() # full screen
screenW = screen.get_rect().width
screenH = screen.get_rect().height
viewPortX = 0
viewPortY = 0


# Initialise the mp.
tilemap = pygame.tilemap.Tilemap(TILEMAP_W,TILEMAP_H,spritedata.mapPixels)
tilemap.set_tile(0x0, 8, 8, spritedata.blackTileSurf);
tilemap.set_tile(0xb, 8, 8, spritedata.wallHTileSurf);
tilemap.set_tile(0xc, 8, 8, spritedata.wallVTileSurf);
tilemap.set_tile(0x3, 8, 8, spritedata.wallCornerTileSurf);
tilemap.set_tile(0x7, 8, 8, spritedata.shadowHTileSurf);
tilemap.set_tile(0xd, 8, 8, spritedata.shadowVTileSurf);
tilemap.set_tile(0x2, 8, 8, spritedata.shadowCorner1TileSurf);
tilemap.set_tile(0x4, 8, 8, spritedata.shadowCorner2TileSurf);


print('display ready')

class GameObject(sprite.Sprite):
    def __init__(self, surfaces, frameOffsets):
        sprite.Sprite.__init__(self)
        self.frames = surfaces
        self.frameOffsets = frameOffsets
        self.currentFrameNum = 0;
        self.image = self.frames[self.currentFrameNum]  # current image
        self.animDur = 3;
        self.animDurCounter = self.animDur;
        self.animDir = 1;

        # velocity and position in the world
        self.vx = 0
        self.vy = 0
        self.x = 0
        self.y = 0

        # Position in the viewport
        self.rect = self.frames[0].get_rect()

        #
        iamhero = False

    def setvel(self, vx, vy):
        self.vx = vx
        self.vy = vy

    def update(self):

        #global viewPortX
        #global viewPortY

        # Advance frame if animation is set
        if self.animDur > 0:

            # if animation duration has elapsed, advance frame
            if self.animDurCounter == 0:
                self.currentFrameNum += self.animDir
                if (self.currentFrameNum >= len(self.frames)) or (self.currentFrameNum < 0):
                    self.animDir = - self.animDir
                    self.currentFrameNum += self.animDir

                #
                self.animDurCounter = self.animDur

                # Set current image
                self.image = self.frames[self.currentFrameNum]
                self.x += self.frameOffsets[self.currentFrameNum][0]
                self.y += self.frameOffsets[self.currentFrameNum][1]
            else:
                self.animDurCounter -= 1

        # Advance position in the world
        prevX = self.x
        prevY = self.y
        self.x += self.vx
        self.y += self.vy

        #
        heroWasHit, enemyWasHit, enemyIsOverWall = self.CheckCollisions2()
        if heroWasHit != None:
            frogittoGob.x = 50
            frogittoGob.y = 70

        # if this enemy was hit, set it to out of screen
        if enemyWasHit != None:
            self.x = -100
            self.y = -100

        # if this enemy was hit, set it to out of screen
        if enemyIsOverWall:
            #self.vx = 0
            #self.vy = 0
            self.x = prevX
            self.y = prevY
            #print("isOverWall")

        # Set the position in the viewport
        self.rect.x = self.x + viewPortX
        self.rect.y = self.y + viewPortY

    # Check all collisions
    def CheckCollisions2(self):
        iWasHit = None
        enemyWasHit = None
        enemyIsOverWall = None
        herocollide = frogittoGob.rect.colliderect
        frogittoGobX = frogittoGob.x
        frogittoGobY = frogittoGob.y
        frogittoGobW = frogittoGob.rect.width
        frogittoGobH = frogittoGob.rect.height
        srect = pygame.Rect(0,0,0,0)

        s = self
        srect.width = s.rect.width
        srect.height = s.rect.height
        srectx = s.x
        srecty = s.y
        srect.x = srectx
        srect.y = srecty

        # Check if the enemy is over the wall
        #x = srectx // TILE_ITEM_W
        #y = srecty // TILE_H
        #tilePair = spritedata.mapPixels[x + (y*TILEMAP_W_ITEMS)]

        #tileId = tilemap.get_tile_id(srectx, srecty, 8)
        tileIdList = tilemap.get_tile_ids(srectx, srecty, srectx+srect.width, srecty+srect.height, 8)
        #print("tileIds", tileIds)
        #tileIdDict = collections.Counter(tileIdList)
        #print("Ids="+ str(tileIdList)+", rect="+ str(srectx)+","+str(srecty)+","+str(srectx+srect.width)+","+str(srecty+srect.height))
        #print("Id="+ str(tileId))

        if((tileIdList.count(0)+tileIdList.count(2)+tileIdList.count(4)+tileIdList.count(7)+tileIdList.count(13)) < 4 ): # If not empty
            #
            enemyIsOverWall = True

        if (not self.iamhero):
            # Check if the enemy is out-of-screen
            if ((srectx+10>screenW) or (srecty+10>screenH) or srectx<0 or srecty<0 ):
                return iWasHit, enemyWasHit, enemyIsOverWall

            # Check for the hero hit is by the enemy
            if (abs(frogittoGobX-srectx<frogittoGobW) and abs(frogittoGobY-srecty<frogittoGobH)):
                if herocollide(srect):
                    iWasHit = s

            # Check for the enemy hit by the laser
            # Note: this can be optimized by checking only the bullets that are in yhe same quadrant
            # as the enemy, related to player.
            srectw = srect.width
            srecth = srect.height
            for i in range(numLasers):
                laserx = lasers.xlist[i]
                lasery = lasers.ylist[i]
                if (abs(srectx-laserx) < srectw and abs(srecty-lasery) < srecth ):
                    # Check with more precision
                    laserw = lasers.imagelist[i].get_rect().width
                    laserh = lasers.imagelist[i].get_rect().height
                    if((srectx < laserx + laserw) and (srecty < lasery + laserh) and
                        (srectx + srectw > laserx) and (srecty + srecth > lasery)):
                        enemyWasHit = s

        return iWasHit, enemyWasHit, enemyIsOverWall

# Particle class
class Particle(object):

    def __init__(self, xlist, ylist, vxlist, vylist, imagelist):
        self.partCount = len(xlist)
        self.xlist = xlist
        self.ylist = ylist
        self.vxlist = vxlist
        self.vylist = vylist
        self.imagelist = imagelist

    def update(self):
        for i in range(self.partCount):
            # Advance position
            self.xlist[i] += self.vxlist[i]
            self.ylist[i] += self.vylist[i]

    def draw(self, surface):
        surface_blit = surface.blit
        for i in range(self.partCount):
            surface_blit(self.imagelist[i], self.xlist[i], self.ylist[i])

#test mem
gc.collect()
a = gc.mem_free();
print('1. mem free=',a)

#
all_sprites = sprite.Group()
all_cars = sprite.Group()

#test mem
gc.collect()
b = gc.mem_free();
print('2. mem free=', b)
print('2. diff=', a-b)
a=b

# Create frogitto sprites
frogittoGob = GameObject(
    [spritedata.pokittoSurf_f1, spritedata.pokittoSurf_f2, spritedata.pokittoSurf_f3],
    [[0,0],[0,0],[0,0]])

frogittoGob.iamhero = True

# out of screen
frogittoGob.x = 40
frogittoGob.y = 30

frogittoGob.animDur = 2;

#all_sprites.add(frogittoGob)

#test mem
gc.collect()
b = gc.mem_free();
print('3. mem free=', b)
print('3. diff=', a-b)
a=b

#test mem
gc.collect()
b = gc.mem_free();
print('3.1 mem free=', b)
print('3.1 diff=', a-b)
a=b

#exit()

# Create blue and red car sprites
numEnemies = 0
bluecarSurfList = [spritedata.bluecarSurf_f1, spritedata.bluecarSurf_f2, spritedata.bluecarSurf_f3]
carOffsetList = [[0,0],[0,0],[0,0]]
redcarSurfList = [spritedata.redcarSurf_f1, spritedata.redcarSurf_f2, spritedata.redcarSurf_f3]
for i in range(numEnemies):

    carGob = GameObject(bluecarSurfList, carOffsetList)

    # out of screen
    carGob.x = -100
    carGob.y = -100

    all_sprites.add(carGob)
    all_cars.add(carGob)

    carGob = GameObject(redcarSurfList,  carOffsetList)
    testGob=carGob #!!HV

    # out of screen
    carGob.x = -100
    carGob.y = -100

    all_sprites.add(carGob)
    all_cars.add(carGob)

# Create laser particle
numLasers = 6
xlist = []
ylist = []
vxlist = []
vylist = []
imagelist  = []
for i in range(numLasers):
    xlist.append(-100)
    ylist.append(-100)
    vxlist.append(0)
    vylist.append(0)
    imagelist.append(spritedata.laserSurf_deg90)

lasers = Particle(xlist, ylist, vxlist, vylist, imagelist)

#test mem
gc.collect()
b = gc.mem_free();
print('4. mem free=', b)
print('4. diff=', a-b)
a=b

vx = 0;
vy = 0;
vp_vy = 0
vp_vx = 0
frameNum = 0;
lastY = 0
shoot = False
lastUsedLaser = 0
lastShotTime = 0
while True:

    eventtype = pygame.event.poll()
    if eventtype != pygame.NOEVENT:
        if eventtype.type == pygame.KEYDOWN:
            if eventtype.key == pygame.K_RIGHT:
                vx = 1
            if eventtype.key == pygame.K_LEFT:
                vx = -1
            if eventtype.key == pygame.K_UP:
                vy = -1
            if eventtype.key == pygame.K_DOWN:
                vy = 1
            if eventtype.key == pygame.BUT_A:
                shoot = True

            if eventtype.key == pygame.BUT_B:
                rumbleMotor.on()

        if eventtype.type == pygame.KEYUP:
            if eventtype.key == pygame.K_RIGHT:
                vx = 0
            if eventtype.key == pygame.K_LEFT:
                vx = 0
            if eventtype.key == pygame.K_UP:
                vy = 0
            if eventtype.key == pygame.K_DOWN:
                vy = 0
            if eventtype.key == pygame.BUT_A:
                shoot = False
            if eventtype.key == pygame.BUT_B:
                rumbleMotor.off()

    #
    vy2 = vy
    vx2 = vx

    if( isHW ):
        # Read Joyhat
        # vVlue is between 0, 65535
        # Without the hat the value is between 1000 and 3000
        ly = vy2
        lx = vx2
        joyhat_x = joyhatX.value() #JHAT
        joyhat_y = joyhatY.value() #JHAT
    else:
        if(shoot):
            ly = vy2
            lx = vx2
        else:
            ly = 0
            lx = 0
        joyhat_x = 32000
        joyhat_y = 32000

    if(joyhat_x>36000):
        lx = -1
        shoot = True
    elif(joyhat_x<29000):
        lx = 1
        shoot = True
    if(joyhat_y>36000):
        ly = 1
        shoot = True
    elif(joyhat_y<29000):
        ly = -1
        shoot = True

    # Shoot
    if((ly!=0 or lx!=0) and (frameNum -lastShotTime) > 1):

        # Get vx and vy for the laser
        vxlist[lastUsedLaser] = 0
        vylist[lastUsedLaser] = 0
        if(ly < 0):
            vylist[lastUsedLaser] = -6
        elif(ly > 0):
            vylist[lastUsedLaser] = 6
        if(lx < 0):
            vxlist[lastUsedLaser] = -6
        elif(lx > 0):
            vxlist[lastUsedLaser] = 6

        # Select the bitmap
        if(lx > 0 and ly==0):
            imagelist[lastUsedLaser] = spritedata.laserSurf_deg0
        elif(lx > 0 and ly<0):
            imagelist[lastUsedLaser] = spritedata.laserSurf_deg45
        elif(lx == 0 and ly<0):
            imagelist[lastUsedLaser] = spritedata.laserSurf_deg90
        elif(lx < 0 and ly<0):
            imagelist[lastUsedLaser] = spritedata.laserSurf_deg135
        elif(lx < 0 and ly==0):
            imagelist[lastUsedLaser] = spritedata.laserSurf_deg180
        elif(lx < 0 and ly>0):
            imagelist[lastUsedLaser] = spritedata.laserSurf_deg225
        elif(lx == 0 and ly>0):
            imagelist[lastUsedLaser] = spritedata.laserSurf_deg270
        else: # (lx > 0 and ly>0)
            imagelist[lastUsedLaser] = spritedata.laserSurf_deg315

        # Get x and y for the laser
        offsetx = 2
        if(frameNum%4 < 2 ):
            offsetx = -2
        offsety = offsetx
        if(lx==ly):
          offsety = - offsetx
        xlist[lastUsedLaser] = frogittoGob.x + (frogittoGob.rect.width//2) + offsetx
        ylist[lastUsedLaser] = frogittoGob.y + (frogittoGob.rect.height//2) + offsety
        lastUsedLaser += 1
        if(lastUsedLaser>=numLasers):
            lastUsedLaser = 0
        lastShotTime =  frameNum;

    # Set hero speed
    frogittoGob.setvel(vx2,vy2);

    # Launch a new bug after n frames
    if (frameNum % 13) == 0:
        # Get first free car and set the starting position
        for s in all_cars:
            if s.x < -16:
                s.x = 110
                y = 10 + random.getrandbits(8) * 60 // 256
                if abs(y - lastY) < 7:
                    if y < lastY:
                        y -= 7
                    else:
                        y += 7
                s.y = y
                lastY = y
                s.setvel(-1,0)
                s.currentFrameNum = 0

                break


    # *** update

    # Update hero
    frogittoGob.update()

    # Calculate viewport position
    hero_vpx = frogittoGob.rect.x
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
    hero_vpy = frogittoGob.rect.y
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
    newVpX = viewPortX + vpx
    newVpY = viewPortY + vpy

    # Check that viewport is not out bounds
    if(newVpX>0):
        newVpX=0
    elif(newVpX + TILEMAP_W_PX < SCREEN_W):
        newVpX = SCREEN_W - TILEMAP_W_PX
    if(newVpY>0):
        newVpY=0
    elif(newVpY + TILEMAP_H_PX < SCREEN_H):
        newVpY = SCREEN_H - TILEMAP_H_PX
    viewPortX = newVpX
    viewPortY = newVpY

    # update hero's position in the viewport
    frogittoGob.rect.x = frogittoGob.x + viewPortX
    frogittoGob.rect.y = frogittoGob.y + viewPortY

    # Update enemies
    all_sprites.update()

    # *** Draw ***

    # Draw tilemap
    tilemap.draw(viewPortX, viewPortY)

    # Draw enemies
    all_sprites.draw(screen)

    # Draw hero
    screen.blit(frogittoGob.image, frogittoGob.rect.x, frogittoGob.rect.y)

    # Update and draw lasers
    lasers.update()
    lasers.draw(screen)

    #text = "jx:"+str(joyhatX.value())
    #umachine.draw_text(0,0,text,1)
    #text = "jy:"+str(joyhatY.value())
    #umachine.draw_text(0,10,text,1)
    #text = "b1:"+str(joyhatB1.value())+", b2:"+str(joyhatB2.value())
    #umachine.draw_text(0,20,text,1)


    frameNum += 1
    if frameNum > 1000000:
        frameNum = 0;

    #umachine.wait(2000)

    pygame.display.flip()


