# gameobject.py
# Copyright (C) 2019 Hannu Viitala
#
# The source code in this file is released under the MIT license.
# Go to http://opensource.org/licenses/MIT for the full license details.

import upygame as pygame
import globals as glob
import sprite
import urandom as random
import data

# The game object class.
class GameObject(sprite.Sprite):
    def __init__(self, surfaces, frameOffsets):
        sprite.Sprite.__init__(self)
        self.frames = surfaces
        self.frameOffsets = frameOffsets
        self.currentAnimFrameNum = 0;
        self.image = self.frames[self.currentAnimFrameNum]  # current image
        self.animDur = 3;
        self.animDurCounter = self.animDur;
        self.animDir = 1;

        # velocity and position in the world
        self.vx = 0
        self.vy = 0
        self.wx = 0 # world x
        self.wy = 0 # world y
        # Note. the rect member data is in viewport domain (e.g. screen)
        self.moveDelay = glob.HERO_MOVE_DELAY
        self.moveDelayCounter = 0

        # Position in the viewport
        self.rect = self.frames[self.currentAnimFrameNum].get_rect()

        # Direction to go
        self.dir = 3 # 1=up, 2=right, 3=down, 4=left
        self.randTurnFrameNum = 40
        self.randCheckHeroPosFrameNum = 50
        self.active = True

    def UpdateImage(self):
        self.image = self.frames[self.currentAnimFrameNum]
        self.rect.width = self.image.get_rect().width
        self.rect.height = self.image.get_rect().height

    def update(self):

        if(not self.active): return

        # Delay
        self.moveDelayCounter -= 1
        if(self.moveDelayCounter<=0):
            self.moveDelayCounter = self.moveDelay
        else:
            return

        # Advance frame if animation is set
        if self.animDur > 0:

          #
          self.animDurCounter -= 1

          # if animation duration has elapsed, advance frame
          if self.animDurCounter == 0:
                self.currentAnimFrameNum += self.animDir
                if (self.currentAnimFrameNum >= len(self.frames)):
                    self.currentAnimFrameNum = 0

                # The animation duration
                self.animDurCounter = self.animDur

                # Set current image
                self.UpdateImage()
                self.wx += self.frameOffsets[self.currentAnimFrameNum][0]
                self.wy += self.frameOffsets[self.currentAnimFrameNum][1]


        #print("juice: wx", self.wx,"wy",self.wy)
        # Advance position in the world
        prevX = self.wx
        prevY = self.wy
        if(self.dir == 1): # up
            self.wy -= self.vy
        elif(self.dir == 2): # right
            self.wx += self.vx
        elif(self.dir == 3): # down
            self.wy += self.vy
        else: # right
            self.wx -= self.vx

       # Set the position in the viewport
        self.rect.x = self.wx + glob.viewPortX
        self.rect.y = self.wy + glob.viewPortY

        # Start again from the top
        #if(self.wy>glob.SCREEN_H):
        #    self.wy = -self.rect.height

    # Check all collisions
    def CheckCollisions(self):
        heroWasHit = None
        enemyWasHit = None
        enemyIsOverWall = None
        herocollide = glob.heroGob.rect.colliderect
        heroGobX = glob.heroGob.x
        heroGobY = glob.heroGob.y
        heroGobW = glob.heroGob.rect.width
        heroGobH = glob.heroGob.rect.height
        srect = pygame.Rect(0,0,0,0)

        s = self
        srect.width = s.rect.width
        srect.height = s.rect.height
        srectx = s.wx
        srecty = s.wy
        srect.x = srectx
        srect.y = srecty

        # Check the collision to the wall in the tilemap
        #tileIdList = glob.tilemap.get_tile_ids(srectx, srecty, srectx+srect.width, srecty+srect.height, 8)
        #tl = glob.tileAttributeArray[tileIdList[0]] & glob.TILE_FLAG_BLOCKING
        #tr = glob.tileAttributeArray[tileIdList[1]] & glob.TILE_FLAG_BLOCKING
        #bl = glob.tileAttributeArray[tileIdList[2]] & glob.TILE_FLAG_BLOCKING
        #br = glob.tileAttributeArray[tileIdList[3]] & glob.TILE_FLAG_BLOCKING
        #if( tl + tr + bl + br > 0):
        #    enemyIsOverWall = True

        # Check if the enemy is out-of-screen
        if (srectx<0 or srecty<0 ):
            return heroWasHit, enemyWasHit, enemyIsOverWall

        # Check for the hero hit is by the enemy
        if (abs(heroGobX-srectx<heroGobW) and abs(heroGobY-srecty<heroGobH)):
            if herocollide(s.rect):
                heroWasHit = s

        return heroWasHit, enemyIsOverWall

# The enemy class
class EnemyBounceGameObject(GameObject):

    def __init__(self, surfaces, frameOffsets):
        GameObject.__init__(self, surfaces, frameOffsets)
        self.bounceLimits = [0,0,glob.SCREEN_W,glob.SCREEN_H] # tlx,tly,brx,bry
        self.bounceType=0

    def update(self):

        if(not self.active): return
    
        # Delay
        self.moveDelayCounter -= 1
        if(self.moveDelayCounter<=0):
            self.moveDelayCounter = self.moveDelay
        else:
            return

        # Advance frame if animation is set
        if self.animDur > 0:

          #
          self.animDurCounter -= 1

          # if animation duration has elapsed, advance frame
          if self.animDurCounter == 0:
                self.currentAnimFrameNum += self.animDir
                if (self.currentAnimFrameNum >= len(self.frames)):
                    self.currentAnimFrameNum = 0

                # The animation duration
                self.animDurCounter = self.animDur

                # Set current image
                self.UpdateImage()
                self.wx += self.frameOffsets[self.currentAnimFrameNum][0]
                self.wy += self.frameOffsets[self.currentAnimFrameNum][1]

        # Advance position in the world
        prevX = self.wx
        prevY = self.wy
        if(self.dir == 1): # up
            self.wy -= self.vy
        elif(self.dir == 2): # right
            self.wx += self.vx
        elif(self.dir == 3): # down
            self.wy += self.vy
        else: # right
            self.wx -= self.vx

        #if(self.frames[0] == data.scissorsSurf0): print("scissors")

        # Check for bounce limits
        if( self.wx < self.bounceLimits[0] or
            self.wx >= self.bounceLimits[2]-self.image.get_rect().width or
            self.wy < self.bounceLimits[1] or
            self.wy >= self.bounceLimits[3]-self.image.get_rect().height ):

            #if(self.frames[0] == data.beeQueenSurf_f1): 
                #print("beeQueen met bounce limits")
                #print("*** bounceLimits", self.bounceLimits)
                #print("*** wx", self.wx,"wy", self.wy)
            
            if(self.bounceType==0):  # bounce
                # Met bounce limits => bounce
                self.wx = prevX
                self.wy = prevY
                if(self.dir==1): self.dir=3 # up -> down
                elif(self.dir==3): self.dir=1 # down -> up
                if(self.dir==2): self.dir=4 # right -> left
                elif(self.dir==4): self.dir=2 # left -> right

            if(self.bounceType==1):  # recycle
                # Met bounce limits => bounce
                self.wx = prevX
                self.wy = prevY
                if(self.dir==1): self.wy=self.bounceLimits[3] # up 
                elif(self.dir==3): self.wy=self.bounceLimits[1] # down
                if(self.dir==2): self.wx=self.bounceLimits[0] # right
                elif(self.dir==4): self.wx=self.bounceLimits[2] # left

        # Set the position in the viewport
        self.rect.x = self.wx + glob.viewPortX
        self.rect.y = self.wy + glob.viewPortY


# The hero object class.
# Used for the enemies.
class HeroGameObject(GameObject):

    def __init__(self, surfaces, frameOffsets):
        GameObject.__init__(self, surfaces, frameOffsets)
        self.hmirror = False
        self.tryJump = False
        self.accY = 0
        self.state = 0
        self.counter1 = 0
        self.counter2 = 0
        self.visible = True
        self.isOnAir = False

    def setState(self, state):
        self.state = state

        # Lost hearth, flash for a while
        if(self.state==1):
            #print("setState 1")
            glob.hartCount -= 1
            if(glob.hartCount<=0):
                glob.isGameLost = True
            self.counter1 = 2 # Phase duration
            self.counter2 = 100 # Total duration of the state
            self.visible = False

    def update(self):
        
        #print("herogob:glob.test",glob.test)

        # Lost heart, flash for a while
        if(self.state==1):
            #print("is in setState 1")
            self.counter1 -= 1 # Phase duration
            self.counter2 -= 1 # Total duration of the state

            # Change phase?
            if(self.counter1 <= 0):
                #print("self.counter1",self.counter1)
                self.visible = not self.visible
                self.counter1 = 10

            # Return to normal state?
            if(self.counter2 <= 0):
                #print("self.counter2",self.counter2)
                self.visible = True
                self.state = 0

        # Delay other operations if needed
        self.moveDelayCounter -= 1
        if(self.moveDelayCounter<=0):
            self.moveDelayCounter = self.moveDelay
        else:
            return
        
        # save old values
        prevWx = self.wx
        prevWy = self.wy

        # Advance frame if animation is set
        if self.animDur > 0:

          #
          self.animDurCounter -= 1

          # if animation duration has elapsed, advance frame
          if(self.vx==0 and self.vy==0):

                # stand still

                self.currentAnimFrameNum = 1
                self.image = self.frames[self.currentAnimFrameNum]
                self.animDurCounter = self.animDur

          elif self.animDurCounter == 0:

                self.currentAnimFrameNum += self.animDir
                if (self.currentAnimFrameNum >= len(self.frames)):
                    self.currentAnimFrameNum = 0

                # The animation duration
                self.animDurCounter = self.animDur

                # Set current image
                self.UpdateImage()
                if(self.vx<0):
                    self.hmirror = False
                    self.wx += self.frameOffsets[self.currentAnimFrameNum][0]
                    self.wy += self.frameOffsets[self.currentAnimFrameNum][1]
                elif(self.vx>0):
                    self.hmirror = True
                    self.wx -= self.frameOffsets[self.currentAnimFrameNum][0]
                    self.wy -= self.frameOffsets[self.currentAnimFrameNum][1]

        #print("self.currentAnimFrameNum=",self.currentAnimFrameNum)

        # Try jump?
        if(self.tryJump):
            # Check that is over floor
            self.tryJump = False
            if( not self.isOnAir): # Not going up or down
                #tileId = glob.tilemap.get_tile_id( prevWx, prevWy + self.rect.height+1, 8 )
                #if(glob.tileAttributeArray[tileId] & glob.TILE_FLAG_BLOCKING):
                    self.accY = glob.HERO_JUMP_ACC_Y # jump
                    glob.sound.play_sfx(data.jumpSound, len(data.jumpSound), True)

        # Calc acceleration
        self.accY += glob.GRAVITY
        if(self.accY>7): self.accY = 7
        self.isOnAir = True  # On air, until proven otherwise

        # Advance position in the world
        self.wx += self.vx
        self.wy += self.accY # acceleration

        # Start again from the right edge
        #if(self.wx<-16):
        #    self.wx = glob.SCREEN_W

        # Set the position in the viewport (screen)
        self.rect.x = self.wx + glob.viewPortX
        self.rect.y = self.wy + glob.viewPortY

        # Check collisions
        isOverWallY, isOverWallX, heroWasHitBy = self.CheckCollisions3(prevWx, prevWy)

        # Hero finds coin?
        if(heroWasHitBy == glob.coinGob):
            glob.goldCount += 1
            glob.coinGob.wx = -1000
            glob.sound.play_sfx(data.coinSound, len(data.coinSound), True)
            
        # enemy hits hero?
        elif(heroWasHitBy != None):
            if(heroWasHitBy == glob.all_enemies_list[2] and glob.roomIndex == 4):  # hit by bee queen in beehive
                self.wy = 30
                self.accY = glob.HERO_JUMP_ACC_Y
            self.setState(1)
            glob.sound.play_sfx(data.hitSound, len(data.hitSound), True)

        # If would collide with the wall below, do not move to that direction.
        if isOverWallY:
            if(self.accY>0): # If going down
                # Stop falling down.
                # Land on 8 pixel border.
                #self.wy = (self.wy&0xfff8)
                newWy = (self.wy//8)*8
                if(prevWy <= newWy):
                    self.wy = newWy
                    # Stop acceleration
                    self.accY = 0
                    self.isOnAir=False
                    
        if isOverWallX:
            self.wx = prevWx

        # Set the position in the viewport (screen)
        self.rect.x = self.wx + glob.viewPortX
        self.rect.y = self.wy + glob.viewPortY

    # Check all collisions
    def CheckCollisions3(self,prevWx, prevWy):
        isOverWallX = False
        isOverWallY = False
        heroWasHitBy = None
        s = self
        #screen_srect = pygame.Rect(0,0,0,0)
        #screen_srect.width = s.rect.width
        #screen_srect.height = s.rect.height
        #screen_srect.x = world_x
        #screen_srect.y = world_y
        halfWidth = (s.rect.width>>1)
        world_bottomx = s.wx + halfWidth
        world_bottomy = s.wy + s.rect.height

        # Check collision in y direction
        tileId = glob.tilemap.get_tile_id( prevWx + halfWidth, world_bottomy, 8 )
        #print("checkcoll:",tileId,s.wy)
        if(glob.tileAttributeArray[tileId] & glob.TILE_FLAG_BLOCKING):
            isOverWallY = True

        # Check collision in x direction
        tileId = glob.tilemap.get_tile_id( world_bottomx, prevWy, 8 )
        if(glob.tileAttributeArray[tileId] & glob.TILE_FLAG_BLOCKING and 
            not (glob.tileAttributeArray[tileId] & glob.TILE_FLAG_PLATFORM) ):
            isOverWallX = True

        # Check collision to enemies
        if(self.state==0): # Normal state
            spritecollide = self.rect.colliderect
            for enemy in glob.all_enemies:
                if spritecollide(enemy.rect):
                    #print("collided to enemy", enemy.rect,self.rect)
                    heroWasHitBy = enemy
                    break
                #else:
                    #if(enemy.frames[0] == data.beeQueenSurf_f1): 
                        #print("beeQueen not collided")
                        #print("*** bee y+h", enemy.rect.y+enemy.rect.height,"hero wy", self.rect.y)
                        #print("*** bee y", enemy.rect.y,"h",enemy.rect.height,"hero y", self.rect.y)


        return isOverWallY, isOverWallX, heroWasHitBy

