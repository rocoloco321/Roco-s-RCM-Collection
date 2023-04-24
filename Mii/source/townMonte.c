#include "common.h"
#include "snd/sound_var.h"
#include "race/mapobj/mapobj.h"
#include "race/mapobj/mapobjModel.h"
#include "townMonte.h"

#pragma auto_inline off

static mobj_model_t* sModel;
static mobj_model_t* sModel2;

static void loadModel(mobj_render_part_t* renderPart)
{
    sModel = mobj_loadModel(renderPart, "Mii.nsbmd");
    mobj_loadNsbtpAnim(sModel, "Mii.nsbtp");
    mobj_setModelPolygonIdAll(sModel, 63);
    sModel2 = mobj_loadModel(renderPart, "Miif.nsbmd");
    mobj_loadNsbtpAnim(sModel2, "Miif.nsbtp");
    mobj_setModelPolygonIdAll(sModel2, 63);
}

static void render(mobj_render_part_t* renderPart, townmonte_t* instance, const MtxFx43* camMtx, u16 alpha)
{
    NNS_G3dGeRestoreMtx(30);
    NNS_G3dGeTranslate(0, instance->baseElevation + instance->fieldA8.value >> 4, 0);
    instance->mobj.mtx._31 = instance->mobj.position.y >> 4;
    NNS_G3dGeMultMtx43(bbm_getYBillboardAtPos(&instance->mobj.position));
    NNS_G3dGeScaleVec(&instance->mobj.scale);
    mobj_setModelNsbtpFrame(sModel, instance->nsbtpFrame);
    mobj_bbModelApplyMaterial(renderPart, sModel);
    mobj_renderModel(renderPart, sModel, &instance->mobj);
    mobj_setModelNsbtpFrame(sModel2, instance->nsbtpFrameFace);
    mobj_bbModelApplyMaterial(renderPart, sModel2);
    mobj_renderModel(renderPart, sModel2, &instance->mobj);
}

static void globalInitLogic(mobj_logic_part_t* logicPart)
{
    if (!logicPart)
        return;

    townmonte_t* instance;
    int i = 0;
    while (i < logicPart->mobjInstanceCount && (instance = (townmonte_t*)logicPart->mobjInstanceList[i]) != NULL)
    {
        instance->nsbtpFrame = mobj_getRng1Random(mobj_getModelNsbtpFrameCount(sModel));
        instance->nsbtpFrameFace = mobj_getRng1Random(mobj_getModelNsbtpFrameCount(sModel2));
        instance->baseElevation = instance->mobj.position.y;
        instance->baseElevation -= instance->mobj.position.y;
        instance->fieldA8.phase = 0;
        instance->fieldA8.baseOffset = 0;
        instance->fieldA8.amplitude = FX32_CONST(4);
        instance->fieldA8.amplitudeVelocity = 0;
        instance->fieldA8.phaseVelocity = 182*4 + mobj_getRng1Random(50);
        instance->fieldA8.phaseAcceleration = 0;
        i++;
    }
}

static void update(mobj_logic_part_t* logicPart, townmonte_t* instance)
{
    updateSinThing2(&instance->fieldA8);
}

static mobj_render_part_t* setupRenderPart()
{
    return mobj_createRenderPart2d(loadModel, (mobj_render_part_instance_render_func_t)render, NULL);
}

static mobj_logic_part_t* setupLogicPart()
{
    return mobj_createLogicPart(globalInitLogic, (mobj_logic_part_instance_update_func_t)update, NULL);
}

mobj_def_t gMiiMObjDef =
{
    0,
    sizeof(townmonte_t),
    NULL,
    mobj_createDummyConfig,
    {setupRenderPart, NULL, NULL},
    setupLogicPart,
    NULL,
    {NULL, NULL, NULL},
    NULL
};

#include "hook.h"

#define MOBJ_TABLE_BASE 0x0216B288
/*
MOBJ uses ID 311, this ID is not present on the MapObj Table, so it doesn't get initialized.
In order to use them we can take over an entry of the table. Since there are 9 unused entries on the table we take over one of them
*/
HOOK_TABLE(
	HOOK_DATA32(MOBJ_TABLE_BASE + MOBJ_TABLE_OFFSET * 0xC, MOBJ_ID_MII),
	HOOK_DATA32(MOBJ_TABLE_BASE + MOBJ_TABLE_OFFSET * 0xC + 4, &gMiiMObjDef)
);
