#pragma once
#include "race/mapobj/mapobj.h"
#include "sinThing.h"
#define MOBJ_ID_MII 311
#define MOBJ_TABLE_OFFSET 124
typedef struct
{
    mobj_inst_t mobj;
    u16 nsbtpFrame;
    u16 nsbtpFrameFace;
    fx32 baseElevation;
    sinthing_t fieldA8;
} townmonte_t;

extern mobj_def_t gTownMonteMObjDef;