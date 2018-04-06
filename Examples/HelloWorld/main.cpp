#include "Pokitto.h"

Pokitto::Core mygame;

void generateBuffer(uint32_t bufferIndex, uint32_t nextT) {

    uint32_t index= 0;
    for(uint32_t t=nextT; t<nextT+BUFFER_SIZE; t++,index++)
        //buffers[nextBufferIndexToFill][index] = t & (t>>8);
        buffers[bufferIndex][index] = ((t<<1)^((t<<1)+(t>>7)&t>>12))|t>>(4-(1^7&(t>>19)))|t>>7;
        //buffers[bufferIndex][index] = (t*9&t>>4|t*5&t>>7|t*3&t/1024)-1;
}


int main () {
    mygame.begin();
    mygame.display.persistence = true;
    mygame.display.paletteptr[2] = COLOR_YELLOW;

    // Generate 4 buffers ahead
    uint32_t nextT = 0;
    generateBuffer(0, nextT); nextT += BUFFER_SIZE;
    generateBuffer(1, nextT); nextT += BUFFER_SIZE;
    generateBuffer(2, nextT); nextT += BUFFER_SIZE;
    generateBuffer(3, nextT); nextT += BUFFER_SIZE;

    // Set global variables
    currentBuffer = 0;
    currentPtr = buffers[currentBuffer];
    endPtr = currentPtr + BUFFER_SIZE;

    pokPlayStream(); // activate stream
    mygame.sound.ampEnable(true);
    mygame.sound.playMusicStream();

    mygame.display.set_cursor(0,0);
    mygame.display.println("ByteBeat function:");
    mygame.display.println("");
    mygame.display.color = 2;
    mygame.display.println("((t<<1)^((t<<1)+(t>>7)");
    mygame.display.println("&t>>12))|t>>(4-(1^7&");
    mygame.display.println("(t>>19)))|t>>7");

    uint32_t nextBufferIndexToFill = 0;
    while (mygame.isRunning()) {
        if (mygame.update()) {

            if((uint32_t)currentBuffer != nextBufferIndexToFill) {

                // Generate buffer if already played.
                generateBuffer(nextBufferIndexToFill, nextT);

                if(++nextBufferIndexToFill > 3)
                    nextBufferIndexToFill = 0;
                nextT += BUFFER_SIZE;
            }
        }
    }
}
