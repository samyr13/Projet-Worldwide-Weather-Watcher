#include "system_status.h"
#include "grove_rgb.h"
#include "modes.h" // &lt;--- AJOUTE CETTE LIGNE !

void SystemStatus_Init(SystemStatus *status) {
    status->last_toggle_tick = 0;
    status->toggle_state = 0;
}

void SystemStatus_Update(SystemStatus *status, SystemMode mode, SystemHealth health) {
    uint32_t current_tick = HAL_GetTick();

    // 1. GESTION DES ERREURS (Clignotement à 1 Hz = alternance toutes les 500 ms)
    if (!health.rtc_ok || !health.bme_ok || !health.gps_ok || !health.sd_ok) {
        if (current_tick - status->last_toggle_tick >= 500) {
            status->last_toggle_tick = current_tick;
            status->toggle_state = !status->toggle_state;

            if (status->toggle_state) {
                GroveRGB_SetColor(255, 0, 0); // Phase 1 : Toujours ROUGE
            } else {
                // Phase 2 : Couleur spécifique selon la panne
                if (!health.rtc_ok)       GroveRGB_SetColor(0, 0, 255);     // BLEU (Erreur RTC)
                else if (!health.bme_ok)  GroveRGB_SetColor(0, 255, 0);     // VERT (Erreur BME680)
                else if (!health.gps_ok)  GroveRGB_SetColor(255, 255, 0);   // JAUNE (Erreur GPS)
                else if (!health.sd_ok)   GroveRGB_SetColor(255, 255, 255); // BLANC (Erreur SD)
            }
        }
        return;
    }

    // 2. FONCTIONNEMENT NOMINAL : Couleur fixe selon le mode actif
    switch (mode) {
        case MODE_STANDARD:     GroveRGB_SetColor(0, 255, 0);   break; // Vert
        case MODE_CONFIGURATION:GroveRGB_SetColor(255, 255, 0); break; // Jaune
        case MODE_ECONOMIQUE:   GroveRGB_SetColor(0, 0, 255);   break; // Bleu
        case MODE_MAINTENANCE:  GroveRGB_SetColor(255, 165, 0); break; // Orange
    }
}
