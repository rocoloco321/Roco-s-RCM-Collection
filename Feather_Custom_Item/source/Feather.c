#include "common.h"
#include "race/items/item.h"
#include "race/drivers/driver.h"
#include "race/items/driverItem.h"
#include "race/raceStatus.h"
#include "race/drivers/driverCore.h"
#include "Feather.h"

/*First Ever Custom Object let's go*/


static void addBoost(driver_t* driver, int a2, int boostTime)
{
    if (driver->colReaction != DRIVER_COLLISION_REACTION_NONE
        || driver->flags & DRIVER_FLAGS_BIT12 || driver->flags2 & DRIVER_FLAGS2_KILLER_MODE)
        return;

    *(vu32*)&driver->flags |= DRIVER_FLAGS_BOOST;
    if (boostTime > driver->boostTimer)
        driver->boostTimer = boostTime;
    if (a2 != 1 && a2 != 3)
        driver->flags |= DRIVER_FLAGS_BIT29;
    sub_210A53C(driver);
    driver->flags &= ~DRIVER_FLAGS_START_BOOST;
    if (driver_isPlayer(driver))
    {
        int boostType;
        if (a2 == 0)
        {
            boostType = 0;
        }
        else
        {
            (volatile void)0;
            boostType = 1;
        }
        rptc_startBoostEffect(boostType, driver);
        cam_setPlayerCameraTargetFov(6000);
        dce_startBlur(5);
        if (a2 == 3)
            sfx_21090C4(&driver->soundEmitter, SET_162, 127, 0);
    }
    else
        sfx_21090C4(&driver->soundEmitter, SET_159, 127, 0);
}

void driver_startFeatherEffect(driver_t* driver) {
    driver_timers_t* timers = &driver->timers;
        

    if (driver->flags2 & DRIVER_FLAGS2_KILLER_MODE)
        return;

}

void Feather_Activate(it_driver_dragitem_t *a1, it_driver_itemslot_t *a2)
{
  driver_startFeatherEffect(a2->driverItemStatus->driver);
      sfx_21090C4(&a2->driverItemStatus->driver->soundEmitter, SET_JUMP_PAD_SU, 127, 0);
    a2->driverItemStatus->driver->deltaPos.y = 0; //set delta pos to 0 to being able to always use the collision
    addBoost(a2->driverItemStatus->driver, 0, 90);
    a2->driverItemStatus->driver->flags |= DRIVER_FLAGS_ON_JUMP_PAD;
    switch (rconf_getCCMode())
        {
            case RACE_CC_MODE_50CC:
                a2->driverItemStatus->driver->velocityY.y = FX32_CONST(7.14);
                a2->driverItemStatus->driver->jumpPadSpeed = FX32_CONST(7.86);
                break;
            case RACE_CC_MODE_100CC:
                a2->driverItemStatus->driver->velocityY.y = FX32_CONST(5.88);
                a2->driverItemStatus->driver->jumpPadSpeed = FX32_CONST(2.35);
                break;
            default:
                a2->driverItemStatus->driver->velocityY.y = FX32_CONST(5);
                a2->driverItemStatus->driver->jumpPadSpeed = FX32_CONST(2);
                break;
        }
    
    rptc_killDriverEffects_copy(a2->driverItemStatus->driver);

  it_takeSlotItems(a2, 1);
  if ( gRaceMultiConfig->current.raceMode == 5 )
    sub_20FE958(11);
}

#include <nitro/codereset.h>

const it_itemconfig_t gFeatherConfig =
{
    TRUE, TRUE, ITEM_KINOKO, 1, 0, 0, Feather_Activate
};


#include "hook.h"
HOOK_TABLE(
    HOOK_DATA32(0x020F4DE8, &gFeatherConfig)
);
