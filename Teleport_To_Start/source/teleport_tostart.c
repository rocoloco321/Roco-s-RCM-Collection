#include "hook.h"
#include "common.h"
#include "race/mapData/nkm.h"
#include "race/drivers/driver.h"
#include "race/drivers/driverCore.h"

void RespawnHook() {
      driver_t *driver = driver_getById(rconf_getPlayerDriverId());
    if (input_isSetPadPressById(driver->inputId, PAD_BUTTON_SELECT)) {
      //key is being pressed
      nkm_ktps_entry_t *ktps = gMapData->ktps;

      //set position to start position
      driver->position.x = ktps[0].position.x;
      driver->position.y = ktps[0].position.y;
      driver->position.z = ktps[0].position.z;

      //clear velocity
      driver->velocity.x = 0;
      driver->velocity.y = 0;
      driver->velocity.z = 0;
  }
}

HOOK_TABLE(
  HOOK_ANSUB(0x02062000, RespawnHook)
);