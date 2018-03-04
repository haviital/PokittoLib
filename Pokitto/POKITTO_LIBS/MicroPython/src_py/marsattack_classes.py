# Copyright (C) 2018 Hannu Viitala
# This file is released under MIT license.
# Go to http://opensource.org/licenses/MIT for full license details.

import upygame as pygame
import urandom as random
import sprite

# Game object (gob) class
class GameObject(sprite.Sprite):
    def __init__(self, surfaces, frameOffsets, screen, screenW, screenH):

        sprite.Sprite.__init__(self)
        self.frames = surfaces
        self.frameOffsets = frameOffsets
        self.currentFrameNum = 0;
        self.image = self.frames[self.currentFrameNum]  # current image
        self.animDur = 0;
        self.animDurCounter = self.animDur;
        self.shouldCycleAnim = True
        self.callAfterAnimCycle = None
        self.vx = 0
        self.vy = 0
        self.rect = self.frames[0].get_rect()
        self.collRect = None
        self.collRect2 = None
        self.numClones = 0
        self.spriteIndex = None
        self.currentColorMap = None
        self.disableAfterOutOfScreen = True
        self.visible = True
        self.screen = screen
        self.screenW = screenW
        self.screenH = screenH

    # Remove sprite
    def remove_internal(self, group):

        # Remove sprite
        if self.spriteIndex != None:
            self.screen.setHwSprite( None, None, self.spriteIndex, 0, 0, True)

        # Remove
        del self.__g[group]

    # Setup sprite
    def setSprite(self, spriteIndex, colorMaps):
        self.spriteIndex = spriteIndex
        self.colorMaps = colorMaps
        self.currentColorMap = self.colorMaps[self.currentFrameNum]
        self.screen.setHwSprite(self.image, self.currentColorMap, self.spriteIndex, self.rect.x, self.rect.y, True)

    # Set the animation frame for the sprite
    def setSpriteFrame(self):
        self.image = self.frames[self.currentFrameNum]
        self.currentColorMap = self.colorMaps[self.currentFrameNum]
        self.screen.setHwSprite(self.image, self.currentColorMap, self.spriteIndex, self.rect.x, self.rect.y, False)

    # Set sprite velocity
    def setvel(self, vx, vy):
        self.vx = vx
        self.vy = vy

    # Update the state of the sprite
    def update(self):

        if self.visible:

            # Advance frame if animation is set
            if self.animDur > 0:

                # If animation duration has elapsed, advance frame
                if self.animDurCounter == 0:

                    oldFrameNum = self.currentFrameNum
                    if (self.currentFrameNum + 1) < len(self.frames):
                        self.currentFrameNum += 1
                    else:
                        # Call callback
                        if self.callAfterAnimCycle:
                            self.callAfterAnimCycle(self)
                        if self.shouldCycleAnim:
                            # Cycle animation
                            self.currentFrameNum = 0
                        else:
                            # Stop animation
                            self.animDur = 0

                    # If the frame has been changed, draw it.
                    if oldFrameNum != self.currentFrameNum:
                        self.animDurCounter = self.animDur

                        # Set current image
                        self.image = self.frames[self.currentFrameNum]

                        # Set the current sprite
                        if self.spriteIndex != None:
                            self.setSpriteFrame()

                # No animation this time
                else:
                    self.animDurCounter -= 1

            # Check out-of-screen
            if self.disableAfterOutOfScreen:
                if (self.rect.x < -self.rect.width or self.rect.x > self.screenW or
                        self.rect.y < -self.rect.height or self.rect.y > self.screenH):
                    self.visible = False

            # Advance position
            self.rect.x += self.vx
            self.rect.y += self.vy

# GobGroup class
class GobGroup(sprite.Group):

    # Init
    def __init__(self, *sprites):
        sprite.Group.__init__(self, sprites)
        #self.refreshRate = refreshRate

    # Draw all sprites
    def draw(self, surface):
        sprites = self.sprites()
        surface_blit = surface.blit
        surface_setSprite = surface.setHwSpritePos
        for spr in sprites:
            if spr.visible:
                # Draw either as bitmap or HW sprite
                if spr.spriteIndex is None:
                    # Draw bitmap to the screen buffer
                    self.spritedict[spr] = surface_blit(spr.image, spr.rect.x, spr.rect.y)
                else:
                    # Setup HW sprite for drawing
                    self.spritedict[spr] = spr.rect
                    surface_setSprite(spr.spriteIndex, spr.rect.x, spr.rect.y);

        self.lostsprites = []

# Rock pile group class
class RockPileGroup(sprite.Group):

    # Init
    def __init__(self, *sprites):
        sprite.Group.__init__(self, sprites)

    # Draw all rockpiles
    def draw(self, surface):
        sprites = self.sprites()
        surface_blit = surface.blit

        # draw sprite and pile of clones
        for spr in sprites:
            if spr.visible:
                # Draw the top sprite
                y = spr.rect.y
                self.spritedict[spr] = surface_blit(spr.image, spr.rect.x, y)

                # Draw body sprites
                y += spr.rect.height
                rockSF = spr.frames[1]
                h = rockSF.get_rect().height
                for i in range(1, spr.numClones+1):
                    surface_blit(rockSF, spr.rect.x, y)
                    y += h
        self.lostsprites = []

