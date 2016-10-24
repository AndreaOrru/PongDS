#ifndef __PONG_HPP
#define __PONG_HPP

#include <stdint.h>


void snes_reset();
void snes_vblank();

extern uint8_t palette[0x200];


#endif
