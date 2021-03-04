
#include "features.hpp"
#include "../memory/memory.hpp"
#include "../globals/globals.hpp"

void Features::HandleFeatures()
{
    /* while loop */
    while (1)
    {
        /* admin flags / debug camera */
        if ((!hooked["aflags"] && settings->features.fake_admin) || (hooked["aflags"] && !settings->features.fake_admin))
        {
            globals->local_player.GivePlayerFlag(PlayerFlags::IsAdmin);

            hooked["aflags"] = !hooked["aflags"];
        }
    }
}

Features* features = new Features();