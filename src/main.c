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

// Variable globale unique pour le mode courant
SystemMode mode_actuel = MODE_STANDARD;

int main(void) {
    // Initialisation des couches HAL STM32
    HAL_Init();
    
    // Config de l'horloge système (décommente si nécessaire selon ton setup.h)
    // SystemClock_Config(); 

    // Initialisation de la console UART (pour les printf via ST-LINK)
    // setup_uart_console();

    // Initialisation de la LED RGB Grove
    grove_rgb_init();

    printf("\r\n==============================================\r\n");
    printf("   STATION METEO 3W - INITIALISATION SYSTEME  \r\n");
    printf("==============================================\r\n");

    // Test de boot : Bouton Rouge enfoncé au démarrage ?
    check_boot_mode();

    // Boucle principale
    while (1) {
        // 1. Traitement des appuis de 5s sur les boutons (vert / rouge)
        process_button_presses();

        // 2. Traitement selon le mode courant
        switch (mode_actuel) {
            case MODE_STANDARD:
                // TODO: Acquisition capteurs nominale (LOG_INTERVAL)
                break;

            case MODE_CONFIGURATION:
                // TODO: Écoute des commandes UART pour modifier l'EEPROM
                break;

            case MODE_ECONOMIQUE:
                // TODO: Acquisition espacée (LOG_INTERVAL * 2, GPS 1 cycle/2)
                break;

            case MODE_MAINTENANCE:
                // TODO: Suspension écriture SD et transfert direct sur UART
                break;
        }

        // Cadencement de la boucle principale (50 ms)
        HAL_Delay(50);
    }
}
