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

    // Start the SNES game loop:
    snes_reset();

    return 0;
}
