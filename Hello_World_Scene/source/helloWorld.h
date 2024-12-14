#pragma once

typedef struct 
{
    NNSG2dPaletteData* mainObjPalette;
    NNSG2dCharacterData* mainObjCharacterData;
    NNSG2dCellDataBank* mainCellDataBank;

    oam_buf_t mainOamBuf;

    u16 isAPressed;
} customScene_state_t;

static inline void renderCell(oam_buf_t* oamBuf, const NNSG2dCellData* cell, fx32 x, fx32 y)
{
    NNSG2dFVec2 position = {x, y};
    oamBuf->objCount += NNS_G2dMakeCellToOams(&oamBuf->oam[oamBuf->objCount], 128 - oamBuf->objCount,
                                              cell, NULL, &position, 0, TRUE);
}

