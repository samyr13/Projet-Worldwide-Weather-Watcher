#include <stdio.h>
#include "setup.h"
#include "cesi_types.h"
#include "grove_rgb.h"
#include "grove_button.h"
#include "grove_light.h"
#include "grove_rtc_ds1307.h"
#include "grove_gps_air530z.h"
#include "grove_bme680.h"
#include "sd_logger.h"

#include "modes.h"

SystemMode mode_actuel = MODE_STANDARD;

void setup() {
    // Initialisation
}

void loop() {
    switch (mode_actuel) {
        case MODE_STANDARD:
            // Traitement mode standard (LED Verte)
            break;
        case MODE_CONFIGURATION:
            // Traitement mode config (LED Jaune)
            break;
        case MODE_ECONOMIQUE:
            // Traitement mode éco (LED Bleue)
            break;
        case MODE_MAINTENANCE:
            // Traitement mode maintenance (LED Orange)
            break;
    }
}


int main(void)
{
    Global_Init();

    while (1)
    {
        
    }
}