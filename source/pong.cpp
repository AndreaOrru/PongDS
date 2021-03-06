#include <nds.h>
#include "pong.hpp"
#include "rom_bin.h"
#include "w65816.hpp"
#include "snes.hpp"
#include "ds.hpp"


Register A, X, Y, S(0x100), D;
uint32_t B = 0;
Flags P;
uint8_t mem[0x20000];


void snes_reset()
{
snes_reset:
    // Initialize DS graphics mode:
    videoSetMode(MODE_0_2D);
    vramSetBankA(VRAM_A_MAIN_BG_0x06000000);
    vramSetBankB(VRAM_B_MAIN_SPRITE_0x06400000);
    int bg = bgInit(0, BgType_Text4bpp, BgSize_T_256x256, 0, 1);

    /*** Initialize sprite attributes. ***/
    /*STZ_w(OAMADDL);
    LDX_imm_b(0x80);

loc_00800D:
    LDA_imm_b(0x0a);
    STA_b(OAMDATA);
    LDA_imm_b(0xf0);
    STA_b(OAMDATA);
    STA_b(OAMDATA);
    STA_b(OAMDATA);
    DEC_b(X.l);
    BNE(loc_00800D);

    LDX_imm_b(0x20);
    LDA_imm_b(0x55);

loc_008024:
    STA_b(OAMDATA);
    DEC_b(X.l);
    BNE(loc_008024);*/
    /*************************************/
    oamInit(&oamMain, SpriteMapping_1D_32, false);

    /*** Copy palettes. ***/
    /*
    STZ_b(CGADD);
    LDX_imm_w(0x2200);
    STX_w(DMAP0);
    LDX_imm_w(0x82e6);
    STX_w(A1T0L);
    LDA_imm_b(0x00);
    STA_b(A1B0);
    LDX_imm_w(0x0200);
    STX_w(DAS0);
    LDA_imm_b(0x01);
    STA_b(MDMAEN);     // DMA: $0082E6 -> CGRAM ($200 bytes)
    */
    dmaCopy(&rom_bin[0x2E6], BG_PALETTE, 200);
    dmaCopy(&rom_bin[0x2E6], SPRITE_PALETTE, 200);

    /*** Copy background tile map. ***/
    /*LDA_imm_b(0x80);
    STA_b(VMAIN);
    LDX_imm_w(0x0000);
    STX_w(VMADDL);
    LDX_imm_w(0x1801);
    STX_w(DMAP0);
    LDX_imm_w(0x87a6);
    STX_w(A1T0L);
    LDA_imm_b(0x00);
    STA_b(A1B0);
    LDX_imm_w(0x0800);
    STX_w(DAS0);
    LDA_imm_b(0x01);
    STA_b(MDMAEN);     // DMA: $0087A6 -> VRAM ($800 bytes)*/
    MapEntry* map = convertMap((MapEntry*) &rom_bin[0x7A6], 0x800);
    dmaCopy(map, bgGetMapPtr(bg), 0x800);
    delete map;

    /*** Copy background tiles. ***/
    /*LDA_imm_b(0x80);
    STA_b(VMAIN);
    LDX_imm_w(0x1000);
    STX_w(VMADDL);
    LDX_imm_w(0x1801);
    STX_w(DMAP0);
    LDX_imm_w(0x84e6);
    STX_w(A1T0L);
    LDA_imm_b(0x00);
    STA_b(A1B0);
    LDX_imm_w(0x02c0);
    STX_w(DAS0);
    LDA_imm_b(0x01);
    STA_b(MDMAEN);     // DMA: $0084E6 -> VRAM ($2C0 bytes)*/
    uint32_t* tiles = convertTiles(&rom_bin[0x4E6], 0x2C0);
    dmaCopy(tiles, bgGetGfxPtr(bg), 0x2C0);
    delete tiles;

    /*** Copy sprite tiles. ***/
    /*LDA_imm_b(0x80);
    STA_b(VMAIN);
    LDX_imm_w(0x4000);
    STX_w(VMADDL);
    LDX_imm_w(0x1801);
    STX_w(DMAP0);
    LDX_imm_w(0x8fa6);
    STX_w(A1T0L);
    LDA_imm_b(0x00);
    STA_b(A1B0);
    LDX_imm_w(0x0680);
    STX_w(DAS0);
    LDA_imm_b(0x01);
    STA_b(MDMAEN);     // DMA: $008FA6 -> VRAM ($680 bytes)*/
    uint16_t* sprites[52];
    for(int i = 0; i < 52; i++)
    {
        sprites[i] = oamAllocateGfx(&oamMain, SpriteSize_8x8,
                                    SpriteColorFormat_16Color);

        uint32_t* sprite = convertTiles(&rom_bin[0xFA6 + i*32], 32);
        dmaCopy(sprite, sprites[i], 32);
        delete sprite;
    }

    /* Sprite tile base: $8000              *
     * Sprite sizes: 8x8 small, 32x32 large */
    /*LDA_imm_b(0x22);
    STA_b(OBSEL);

    // BG Mode 1, tile size 16x16:
    LDA_imm_b(0x11);
    STA_b(BGMODE);

    // 32x32 tilemap, starting at $0000:
    LDA_imm_b(0x00);
    STA_b(BG1SC);

    // Background tile base at $2000:
    LDA_imm_b(0x01);
    STA_b(BG12NBA);

    // Enable BG1 and Sprites on Main Screen:
    LDA_imm_b(0x11);
    STA_b(TM);*/

    // In-game variables:
    STZ_b(D + 0x0c);
    LDA_imm_b(0x05);
    STA_b(D + 0x04);
    LDA_imm_b(0xe6);
    STA_b(D + 0x06);
    LDA_imm_b(0x32);
    STA_b(D + 0x00);
    LDA_imm_b(0x40);
    STA_b(D + 0x01);
    LDA_imm_b(0x02);
    STA_b(D + 0x03);
    STA_b(D + 0x02);

    mem[5] = 0x55;
    mem[7] = 0x55;

    // Maximum brightness:
    /*LDA_imm_b(0x0f);
      STA_b(INIDISP);*/

    // Enable NMI and Joypad:
    /*LDA_imm_b(0x81);
      STA_b(NMITIMEN);*/

    // Enable interrupts here:
    irqSet(IRQ_VBLANK, snes_vblank);
    swiWaitForVBlank();

loc_0080FE:
    //LDA_b(D + 0x0c);
    //CMP_imm_b(0x00);
    //BEQ(loc_0080FE);

    LDA_b(D + 0x09);
    BIT_imm_b(0x04);
    BEQ(loc_00810E);

    INC_b(D + 0x05);
    INC_b(D + 0x05);

loc_00810E:
    BIT_imm_b(0x08);
    BEQ(loc_008116);

    DEC_b(D + 0x05);
    DEC_b(D + 0x05);

loc_008116:
    LDA_b(D + 0x0b);
    BIT_imm_b(0x04);
    BEQ(loc_008120);

    INC_b(D + 0x07);
    INC_b(D + 0x07);

loc_008120:
    BIT_imm_b(0x08);
    BEQ(loc_008128);

    DEC_b(D + 0x07);
    DEC_b(D + 0x07);

loc_008128:
    CLC();
    LDA_b(D + 0x00);
    ADC_b(D + 0x02);
    STA_b(D + 0x00);
    CLC();
    LDA_b(D + 0x01);
    ADC_b(D + 0x03);
    STA_b(D + 0x01);
    LDA_b(D + 0x00);
    CMP_imm_b(0x15);
    BCS(loc_008153);

    SEC();
    LDA_b(D + 0x05);
    SBC_imm_b(0x08);
    CMP_b(D + 0x01);
    BCS(loc_008153);

    CLC();
    ADC_imm_b(0x28);
    CMP_b(D + 0x01);
    BCC(loc_008153);

    SEC();
    LDA_imm_b(0x00);
    SBC_b(D + 0x02);
    STA_b(D + 0x02);

loc_008153:
    LDA_b(D + 0x00);
    CMP_imm_b(0xe6);
    BCC(loc_008170);

    SEC();
    LDA_b(D + 0x07);
    SBC_imm_b(0x08);
    CMP_b(D + 0x01);
    BCS(loc_008170);

    CLC();
    ADC_imm_b(0x28);
    CMP_b(D + 0x01);
    BCC(loc_008170);

    SEC();
    LDA_imm_b(0x00);
    SBC_b(D + 0x02);
    STA_b(D + 0x02);

loc_008170:
    LDA_b(D + 0x01);
    CMP_imm_b(0x06);
    BCS(loc_00817D);

    SEC();
    LDA_imm_b(0x00);
    SBC_b(D + 0x03);
    STA_b(D + 0x03);

loc_00817D:
    CMP_imm_b(0xd2);
    BCC(loc_008188);

    SEC();
    LDA_imm_b(0x00);
    SBC_b(D + 0x03);
    STA_b(D + 0x03);

loc_008188:
    /*LDA_b(HVBJOY);
    AND_imm_b(0x80);
    BNE(loc_008188);*/

loc_00818F:
    /*LDA_b(HVBJOY);
    AND_imm_b(0x80);
    BEQ(loc_00818F);*/
    swiWaitForVBlank();
    swiWaitForVBlank();

    goto loc_0080FE;
}

void snes_vblank()
{
snes_vblank:
    PHA_b();
    PHX_w();

    // Some VBlank specific stuff:
    /*LDA_imm_b(0x80);
      AND_b(RDNMI);*/

    // Start writing from the beginning of OAM:
    STZ_b(OAMADDL);
    STZ_b(OAMADDH);

    LDA_b(D + 0x00);
    STA_b(OAMDATA);
    int x = A.l;
    LDA_b(D + 0x01);
    STA_b(OAMDATA);
    int y = A.l;
    LDA_imm_b(0x08);
    STA_b(OAMDATA);
    int i = A.l;
    LDA_imm_b(0x38);
    STA_b(OAMDATA);

    oamSet(&oamMain, //main graphics engine context
           0,           //oam index (0 to 127)
           x, y,   //x and y pixle location of the sprite
           0,                    //priority, lower renders last (on top)
           0,					  //this is the palette index if multiple palettes or the alpha value if bmp sprite
           SpriteSize_8x8,
           SpriteColorFormat_16Color,
           oamGetGfxPtr(&oamMain, i), //pointer to the loaded graphics
           -1,                  //sprite rotation data
           false,               //double the size when rotating?
           false,			//hide the sprite?
           false, false, //vflip, hflip
           false	//apply mosaic
        );

    LDA_b(D + 0x04);
    STA_b(OAMDATA);
    x = A.l;
    LDA_b(D + 0x05);
    STA_b(OAMDATA);
    y = A.l;
    LDA_imm_b(0x00);
    STA_b(OAMDATA);
    i = A.l;
    LDA_imm_b(0x38);
    STA_b(OAMDATA);
    for (int j = 0; j < 4; j++) {
    oamSet(&oamMain, //main graphics engine context
           1 + j,           //oam index (0 to 127)
           x+8, y + j*8,   //x and y pixle location of the sprite
           0,                    //priority, lower renders last (on top)
           0,					  //this is the palette index if multiple palettes or the alpha value if bmp sprite
           SpriteSize_8x8,
           SpriteColorFormat_16Color,
           oamGetGfxPtr(&oamMain, i+1 + j*16), //pointer to the loaded graphics
           -1,                  //sprite rotation data
           false,               //double the size when rotating?
           false,			//hide the sprite?
           false, false, //vflip, hflip
           false	//apply mosaic
        );
    }

    LDA_b(D + 0x06);
    STA_b(OAMDATA);
    x = A.l;
    LDA_b(D + 0x07);
    STA_b(OAMDATA);
    y = A.l;
    LDA_imm_b(0x00);
    STA_b(OAMDATA);
    i = A.l;
    LDA_imm_b(0x38);
    STA_b(OAMDATA);
    for (int j = 0; j < 4; j++) {
        oamSet(&oamMain, //main graphics engine context
               10 + j,           //oam index (0 to 127)
               x+8, y + j*8,   //x and y pixle location of the sprite
               0,                    //priority, lower renders last (on top)
               0,					  //this is the palette index if multiple palettes or the alpha value if bmp sprite
               SpriteSize_8x8,
               SpriteColorFormat_16Color,
               oamGetGfxPtr(&oamMain, i+1 + j*16), //pointer to the loaded graphics
               -1,                  //sprite rotation data
               false,               //double the size when rotating?
               false,			//hide the sprite?
               false, false, //vflip, hflip
               false	//apply mosaic
            );
    }
    oamUpdate(&oamMain);

    /*STZ_b(OAMADDL);
    LDA_imm_b(0x01);
    STA_b(OAMADDH);

    LDA_imm_b(0x68);
    STA_b(OAMDATA);
    LDA_imm_b(0x55);
    STA_b(OAMDATA);
    BEQ(loc_0081F6);*/

loc_0081F6:
    //LDA_b(HVBJOY);
    //AND_imm_b(0x01);
    //BEQ(loc_0081F6);

loc_0081FD:
    //LDA_b(HVBJOY);
    //AND_imm_b(0x01);
    //BNE(loc_0081FD);
    scanKeys();
    uint16_t keys = keysHeld();

    X.w = 0;
    if (keys & KEY_UP) {
        X.w |= (1 << 11);
    } else if (keys & KEY_DOWN) {
        X.w |= (1 << 10);
    }

    //LDX_w(JOY1L);
    STX_w(D + 0x08);
    //LDX_w(JOY2L);

    X.w = 0;
    if (keys & KEY_X) {
        X.w |= (1 << 11);
    } else if (keys & KEY_B) {
        X.w |= (1 << 10);
    }
    STX_w(D + 0x0a);

    LDA_b(D + 0x09);
    BIT_imm_b(0x10);
    BEQ(loc_00821B);

    LDA_b(D + 0x0c);
    INC_b(A.l);
    AND_imm_b(0x01);
    STA_b(D + 0x0c);

loc_00821B:
    LDA_b(D + 0x0b);
    BIT_imm_b(0x10);
    BEQ(loc_008228);

    LDA_b(D + 0x0c);
    INC_b(A.l);
    AND_imm_b(0x01);
    STA_b(D + 0x0c);

loc_008228:
    PLX_w();
    PLA_b();
    return;
}
