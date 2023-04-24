#include "common.h"
#include "process.h"
#include "snd/sound.h"
#include "math/math.h"
#include "math/quaternion.h"
#include "math/vector.h"
#include "math/matrix.h"
#include "race/sound/collisionSfx.h"
#include "race/collision/dynamicCollision.h"
#include "race/mapobj/mapobj.h"
#include "race/drivers/driver.h"
#include "race/effects/explosion.h"
#include "race/drivers/physicalParams.h"
#include "race/collision/collision.h"
#include "jnlib/spa/spaEmitter.h"
#include "race/raceStatus.h"
#include "race/race.h"
#include "particles/particleManager.h"
#include "race/mapData/mapDataUtil.h"
#include "race/items/driverItem.h"
#include "snd/text2024974.h"
#include "race/minigameController.h"
#include "race/mapobj/water/fallsWaterDestination.h"
#include "race/mapobj/mapobjCollision.h"
#include "race/effects/driftParticleController.h"
#include "race/effects/raceParticles.h"
#include "race/drivers/driverCore.h"
#include "hook.h"


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

static DriverCollisionType handleMushPadCollision(driver_t* driver)
{
    if ((driver->floorDriverColType != DRIVER_COLLISION_TYPE_BOOST || driver->field44 & DRIVER_FIELD44_BIT31)
        && (rconf_getCourse() == COURSE_BEACH_COURSE || rconf_getCourse() == COURSE_OLD_SKY_AGB))
    {
        sfx_21090C4(&driver->soundEmitter, SET_JUMP_PAD_SU, 127, 0);
    }
    driver->deltaPos.y = 0; //set delta pos to 0 to being able to always use the collision
    addBoost(driver, 0, 90);
    driver->flags |= DRIVER_FLAGS_ON_JUMP_PAD;
    switch (rconf_getCCMode()) //Speed values calculated trough inverse proprortionality
        {
            case RACE_CC_MODE_50CC:
                driver->velocityY.y = FX32_CONST(7.14);
                driver->jumpPadSpeed = FX32_CONST(7.86);
                break;
            case RACE_CC_MODE_100CC:
                driver->velocityY.y = FX32_CONST(5.88);
                driver->jumpPadSpeed = FX32_CONST(2.35);
                break;
            default:
                driver->velocityY.y = FX32_CONST(5);
                driver->jumpPadSpeed = FX32_CONST(2);
                break;
        }
    
    rptc_killDriverEffects_copy(driver);
    return DRIVER_COLLISION_TYPE_BOOST; 
}


HOOK_TABLE(
	HOOK_ANSUB(0x02067834, handleMushPadCollision)
);