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

// Déclaration unique de la variable globale du mode
SystemMode mode_actuel = MODE_STANDARD;

int main(void) {
    // Initialisation des couches HAL STM32
    HAL_Init();

    // Initialisation du driver de la LED RGB (nom avec majuscules)
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
                // TODO: Acquisition capteurs nominale
                break;

            case MODE_CONFIGURATION:
                // TODO: Écoute des commandes UART
                break;

            case MODE_ECONOMIQUE:
                // TODO: Acquisition espacée
                break;

            case MODE_MAINTENANCE:
                // TODO: Transfert direct sur UART
                break;
        }

        // Période de scrutation (50 ms)
        HAL_Delay(50);
    }
}
