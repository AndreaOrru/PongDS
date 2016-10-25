#ifndef __PONG_HPP
#define __PONG_HPP

#include <stdint.h>


void snes_reset();
void snes_vblank();

extern uint8_t palette[0x200];
extern uint32_t tiles[0x2C0 / 4];
extern uint32_t spriteTiles[0x680 / 4];
extern uint8_t map[0x800];


#endif
