# Welcome to Python on Pokitto!

import upygame as pygame
import urandom as random
import globals as glob
import data as spritedata
import gameobject as GameObjects
import sprite
import gc
import umachine
from micropython import const

# Setup global variables
glob.viewPortX = 0
glob.viewPortY = 0

screen = pygame.display.set_mode() # full screen
pygame.display.init(True)

# ZX Spectrum palette
pygame.display.set_palette_16bit([
    0, 0x001a, 0xd000, 0xd01a, 0x06a0, 0x06ba, 0xd6a0, 0xd6ba,
    0x10e2, 0x001f, 0xf800, 0xf81f, 0x07e0, 0x07ff, 0xffe0, 0xffff
]);


#########################################
def main():

    # setup sprite group
    all_enemies = sprite.Group()

    # Add hero
    heroGob2 = GameObjects.HeroGameObject(
        [spritedata.herox0, spritedata.herox1, spritedata.herox2, spritedata.herox1 ],
        [[-1,0],[0,0],[-1,0],[-1,0]])
    heroGob2.wx = 100
    heroGob2.wy = 50
    heroGob2.vx = -1
    heroGob2.vy = 0
    heroGob2.animDur = 1;

    # Add enemies

    enemyGob0 = GameObjects.GameObject(
        [spritedata.scissorsSurf0, spritedata.scissorsSurf1, spritedata.scissorsSurf2, spritedata.scissorsSurf1 ],
        [[-2,0],[2,0],[0,0],[0,0]])
    all_enemies.add(enemyGob0)
    enemyGob0.wx = 50
    enemyGob0.wy = -10
    enemyGob0.vx = 0
    enemyGob0.vy = 3
    enemyGob0.animDur = 1;

    enemyGob1 = GameObjects.GameObject(
        [spritedata.clockSurf0, spritedata.clockSurf1, spritedata.clockSurf2, spritedata.clockSurf1 ],
        [[0,0],[0,0],[0,0],[0,0]])
    all_enemies.add(enemyGob1)
    enemyGob1.wx = 80
    enemyGob1.wy = -10
    enemyGob1.vx = 0
    enemyGob1.vy = 3
    enemyGob1.animDur = 1;

   # loop variables
    vx = 0
    vy = 0

    # The main loop
    while True:

        currenFrameTime = umachine.time_ms()

        # Read the Pokitto standard keys
        eventtype = pygame.event.poll()
        if eventtype != pygame.NOEVENT:
            if eventtype.type == pygame.KEYDOWN:
                if eventtype.key == pygame.K_RIGHT:
                    vx = 2
                if eventtype.key == pygame.K_LEFT:
                    vx = -2
                if eventtype.key == pygame.K_UP:
                    vy = -2
                if eventtype.key == pygame.K_DOWN:
                    vy = 2
                if eventtype.key == pygame.BUT_A:
                    shoot = True

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

        # Store velocity
        vy2 = vy
        vx2 = vx

        # *** UPDATE ***

        # Update hero
        #glob.heroGob.update()
        heroGob2.update()
        #heroGob3.update()

        # Move viewport
        #MoveViewport()

        # Update enemies
        all_enemies.update()

        # *** DRAW ***

        # Draw tilemap
        #glob.tilemap.draw(glob.viewPortX, glob.viewPortY)

        # Draw enemies
        all_enemies.draw(screen)

        # Draw hero
        #screen.blit(glob.heroGob.image, glob.heroGob.rect.x, glob.heroGob.rect.y)
        screen.blit(heroGob2.image, heroGob2.rect.x, heroGob2.rect.y)
        #screen.blit(heroGob3.image, heroGob3.rect.x, heroGob3.rect.y)

        glob.frameNum += 1

        pygame.display.flip()

        #umachine.wait(1000)


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
    elif(newVpX + TILEMAP_W_PX < SCREEN_W):
        newVpX = SCREEN_W - TILEMAP_W_PX
    if(newVpY>0):
        newVpY=0
    elif(newVpY + TILEMAP_H_PX < SCREEN_H):
        newVpY = SCREEN_H - TILEMAP_H_PX
    glob.viewPortX = newVpX
    glob.viewPortY = newVpY

    # update hero's position in the viewport
    glob.heroGob.rect.x = glob.heroGob.x + glob.viewPortX
    glob.heroGob.rect.y = glob.heroGob.y + glob.viewPortY


#####################################

# Start main
main()
