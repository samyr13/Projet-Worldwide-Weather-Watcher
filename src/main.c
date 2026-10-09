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

// Modules applicatifs
#include "modes.h" 
#include "buttons.h"
#include "system_status.h"

extern I2C_HandleTypeDef hi2c1;

SystemMode mode_actuel = MODE_STANDARD;

int main(void) {
    // 1. Initialisations matérielles
    HAL_Init();
    Global_Init();
    GroveRGB_Init();
    GroveLight_Init(); // Calibration et préparation de l'ADC

    printf("\r\n==============================================\r\n");
    printf("   STATION METEO 3W - INITIALISATION SYSTEME  \r\n");
    printf("==============================================\r\n");

    check_boot_mode();

    SystemStatus status;
    SystemStatus_Init(&status);

    SystemHealth health = {
        .rtc_ok = 1,
        .bme_ok = 1,
        .gps_ok = 1,
        .sd_ok  = 1
    };

    uint16_t light_value = 0;
    uint32_t last_check_tick = 0;
    uint32_t last_print_tick = 0;

    // 2. Boucle principale (Non-bloquante)
    while (1) {
        uint32_t current_tick = HAL_GetTick();

        // A. Traitement des boutons
        process_button_presses();

        // B. Lecture en direct du capteur de luminosité (ADC PA4)
        light_value = GroveLight_ReadRaw();

        // C. Test dynamique de la présence RTC (toutes les 500 ms)
        if (current_tick - last_check_tick >= 500) {
            last_check_tick = current_tick;
            if (HAL_I2C_IsDeviceReady(&hi2c1, (0x68 << 1), 1, 50) == HAL_OK) {
                health.rtc_ok = 1;
            } else {
                health.rtc_ok = 0; // RTC débranchée -> Clignotement Rouge/Bleu 1 Hz
            }
        }

        // D. Affichage de la luminosité sur la console UART (toutes les 1s)
        if (current_tick - last_print_tick >= 1000) {
            last_print_tick = current_tick;
            printf("[ADC PA4] Luminosite : %u / 4095\r\n", light_value);
        }

        // E. Mise à jour de la LED RGB (Mode ou Alerte)
        SystemStatus_Update(&status, mode_actuel, health);

        switch (mode_actuel) {
            case MODE_STANDARD:     break;
            case MODE_CONFIGURATION:break;
            case MODE_ECONOMIQUE:   break;
            case MODE_MAINTENANCE:  break;
        }
    }
}
