#include <nds.h>
#include <stdio.h>
#include "pong.hpp"


void vblank()
{
    // Call the SNES VBlank handler:
    snes_vblank();
}

int main()
{
    // Setup VBlank handler:
    irqSet(IRQ_VBLANK, vblank);

    // RC swag:
    consoleDemoInit();
    iprintf("\n\n\n\n\n\n\n\n\n\n\n         Never Graduate!");

    videoSetMode(MODE_0_2D);
    vramSetBankA(VRAM_A_MAIN_BG);

    // Start the SNES game loop:
    snes_reset();

    return 0;
}
