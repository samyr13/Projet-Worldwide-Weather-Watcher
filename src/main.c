#include <stdio.h>
#include "stm32l4xx_hal.h"
#include "setup.h"
#include "cesi_types.h"

// Drivers matériels Grove
#include "grove_rgb.h"
#include "grove_button.h"
#include "grove_light.h"
#include "grove_rtc_ds1307.h"
#include "grove_gps_air530z.h"
#include "grove_bme680.h"
#include "sd_logger.h"

// Modules de gestion du système 
#include "modes.h" 
#include "buttons.h"
#include "rtc_status.h"
#include "light_status.h"

SystemMode mode_actuel = MODE_STANDARD;

int main(void) {
    HAL_Init();
    Global_Init();

    GroveRGB_Init();

    printf("\r\n==============================================\r\n");
    printf("   STATION METEO 3W - INITIALISATION SYSTEME  \r\n");
    printf("==============================================\r\n");

    check_boot_mode();

    RTCStatus rtc_status;
    RTCStatus_Init(&rtc_status);

    LightStatus light_status;
    LightStatus_Init(&light_status);

    while (1) {
        process_button_presses();
        RTCStatus_Update(&rtc_status, mode_actuel);
        LightStatus_Update(&light_status, mode_actuel);

        switch (mode_actuel) {
            case MODE_STANDARD:
                break;
            case MODE_CONFIGURATION:
                break;
            case MODE_ECONOMIQUE:
                break;
            case MODE_MAINTENANCE:
                break;
        }

        HAL_Delay(50);
    }
}
