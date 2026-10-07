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

// Modules de gestion du système 3W
#include "modes.h" 
#include "buttons.h"

// Déclaration unique de la variable globale pour le mode courant
SystemMode mode_actuel = MODE_STANDARD;

int main(void) {
    // 1. Initialisation de la couche HAL STM32
    HAL_Init();

    // 2. Initialisation globale (UART/Peripheriques depuis setup.c)
    Global_Init();

    // 3. Initialisation du driver de la LED RGB Grove
    GroveRGB_Init();

    printf("\r\n==============================================\r\n");
    printf("   STATION METEO 3W - INITIALISATION SYSTEME  \r\n");
    printf("==============================================\r\n");

    // Test au démarrage : Bouton Rouge enfoncé ?
    check_boot_mode();

    // Boucle principale
    while (1) {
        // 1. Détection des appuis de 5s sur les boutons (vert / rouge)
        process_button_presses();

        // 2. Traitement selon le mode courant
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

        // Cadencement de la boucle (50 ms)
        HAL_Delay(50);
    }
}
