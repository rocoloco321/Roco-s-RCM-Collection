#include "common.h" 
#include "archive.h"
#include "g2du.h"
#include "heap.h"
#include "main2d.h"
#include "oam.h"
#include "process.h"
#include "input/input.h"
#include "scene/displayConfig.h"
#include "helloWorld.h"


/*-----------------------------------------------------------------------------
Custom Scene example - Hello World
Draws "Hello World!" into a text canvas and waits for an A press to be taken back to the title screen
-----------------------------------------------------------------------------*/

//This is a pointer to the struct that holds the information of the state, variables go there. Check the header file.
static customScene_state_t* sCustomSceneState;

//Display Config, this is where the display configuration for the custom scene is saved
static const display_config_base_t sCustomSceneDisplayConfigBase =
{
	1, // The amount of OS_WaitVBlankIntr calls between frames
	GX_PLANEMASK_BG0 | GX_PLANEMASK_BG1 | GX_PLANEMASK_BG2 | GX_PLANEMASK_BG3 | GX_PLANEMASK_OBJ, // Visible Planes for main display
	GX_PLANEMASK_BG0 | GX_PLANEMASK_BG1 | GX_PLANEMASK_BG2 | GX_PLANEMASK_BG3 | GX_PLANEMASK_OBJ, //Visible Planes for sub display
	GX_DISPMODE_GRAPHICS, //Main Display Mode 
	GX_BGMODE_0, //Main BG Mode
	FALSE, //Enables or disables 3D rendering. If enabled BG0 cannot be used.
	GX_BGMODE_0, //Sub BG Mode
	GX_VRAM_BG_128_C, //Main BG Bank
	GX_VRAM_OBJ_32_FG, //Main OBJ Bank
	GX_VRAM_BGEXTPLTT_NONE, //mainBgExtPlttBank
	GX_VRAM_OBJEXTPLTT_NONE, //mainObjExtPlttBank
	GX_VRAM_TEX_01_AB, //texBank
	GX_VRAM_TEXPLTT_0123_E, //texPlttBank
	GX_VRAM_CLEARIMAGE_NONE, //clearImgBank
	GX_VRAM_SUB_BG_32_H, //subBgBank
	GX_VRAM_SUB_OBJ_16_I, //subObjBank
	GX_VRAM_SUB_BGEXTPLTT_NONE, //subBgExtPlttBank
	GX_VRAM_SUB_OBJEXTPLTT_NONE, //subObjExtPlttBank
	GX_VRAM_ARM7_128_D, //arm7Bank
	GX_VRAM_LCDC_NONE //lcdcBank
};

//This is a string buffer that will be used for text rendering
wchar_t buffer[255];



//Init function. This function only is called once during the scene boot process.
static void customScene_init(void)
{
    //allocate memory required by the scene state struct
    sCustomSceneState = proc_alloc(sizeof(customScene_state_t));

    sCustomSceneState->isAPressed = FALSE;

    //Prepare Oam Buffers by clearing them
    oam_clearResetBuffers(&sCustomSceneState->mainOamBuf, NULL);

    //load files required to do the text rendering
    g2du_unpackPaletteData(&sCustomSceneState->mainObjPalette, m2d_getSceneArcFile("customscene_m_o.NCLR"));
    g2du_unpackCharacterData(&sCustomSceneState->mainObjCharacterData, m2d_getSceneArcFile("customscene_m_o.NCGR"));
    sCustomSceneState->mainCellDataBank = NULL;
    g2du_unpackCellBankData(&sCustomSceneState->mainCellDataBank, m2d_getSceneArcFile("customscene_m.NCER"));

    //set display config. required for an scene to work. 
    display_config_t* displayConfig = dc_createDisplayConfig(proc_getCurrentHeap(), &sCustomSceneDisplayConfigBase);
    //Set BG0 specific config. We need to individually configure each used background
    dc_setMainBg0Config(displayConfig, 0, FALSE, GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x1000,
                        GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01);
    dc_setDisplayConfig(displayConfig); //apply display config


    NNSG2dCharCanvas charCanvas;
    NNSG2dTextCanvas textCanvas;

    //init text canvas
    NNS_G2dCharCanvasInitForBG(&charCanvas, sCustomSceneState->mainObjCharacterData->pRawData, 32, 4, NNS_G2D_CHARA_COLORMODE_16);
    NNS_G2dTextCanvasInit(&textCanvas, &charCanvas, m2d_getFont(FONT_LC_S), 1, 0);

    //draw text. Since this is only getting drawn once, we can do it on the init function
    OS_swprintf(buffer, 255, L"Hello World! Press A to return to the title screen.");
    NNS_G2dTextCanvasDrawText(&textCanvas, 128, 8, 15,
        NNS_G2D_HORIZONTALORIGIN_CENTER | NNS_G2D_VERTICALORIGIN_MIDDLE | NNS_G2D_HORIZONTALALIGN_CENTER,
        buffer);

    //Render Cells. Same reason as before.
    renderCell(&sCustomSceneState->mainOamBuf, NNS_G2dGetCellDataByIdx(sCustomSceneState->mainCellDataBank, 0), FX32_CONST(128), FX32_CONST(96)); 

    //Transfer to Vram
    oam_prepareBuffers(&sCustomSceneState->mainOamBuf, NULL);
    oam_applyBuffers(&sCustomSceneState->mainOamBuf, NULL);
    DC_FlushRange(sCustomSceneState->mainObjCharacterData->pRawData, sCustomSceneState->mainObjCharacterData->szByte); //apply text rendering
    GX_LoadOBJPltt(sCustomSceneState->mainObjPalette->pRawData, 0, sCustomSceneState->mainObjPalette->szByte); //send obj pallete to vram
    GX_LoadOBJ(sCustomSceneState->mainObjCharacterData->pRawData, 0, sCustomSceneState->mainObjCharacterData->szByte); //send obj graphics to vram
}


static void miiSel_update(scene_manager_t* sceneManager)
{
    int inputId = input_getMasterInputUnitId();
    if (mutil_checkKeys(inputId, PAD_BUTTON_A) && !sCustomSceneState->isAPressed)
    {
        sceneManager->nextScene = SCENE_TITLE;
        sCustomSceneState->isAPressed = TRUE; //prevent the scene from hammering the title scene and crash the game in the process
    } 
}

//From wikipedia: vblank, is the time between the end of the final visible line of a frame
//or field and the beginning of the first visible line of the next frame or field.
//Functions that modify the display and such should only be called here.
static void miiSel_vblank(void)
{

}


//The finalize function only gets called when the scene is unloaded
static void miiSel_finalize(void)
{
    sCustomSceneState = NULL;
}

//These hooks only apply to the Test scene
#include "hook.h"

HOOK_TABLE(
    HOOK_TNSUB(0x02180610, customScene_init), 
    HOOK_TNSUB(0x0218052C, miiSel_update),
    HOOK_TNSUB(0x02180800, miiSel_vblank),
    HOOK_TNSUB(0x021805F4, miiSel_finalize)
);