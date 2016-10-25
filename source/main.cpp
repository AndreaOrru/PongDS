#include <nds.h>
#include <stdio.h>
#include "pong.hpp"


int main()
{
    // RC swag:
    consoleDemoInit();
    iprintf("\n\n\n\n\n\n\n\n\n\n\n         Never Graduate!");

    // Start the SNES game loop:
    snes_reset();

    return 0;
}
