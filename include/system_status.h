#ifndef SYSTEM_STATUS_H
#define SYSTEM_STATUS_H

#include "stm32l4xx_hal.h"
#include "cesi_types.h" // Inclus pour le type SystemMode
#include "modes.h" // &lt;--- AJOUTE CETTE LIGNE !

// État de santé des 4 sous-systèmes (1 = OK, 0 = Erreur)
typedef struct {
    uint8_t rtc_ok;  // 0 = Clignote Rouge / Bleu
    uint8_t bme_ok;  // 0 = Clignote Rouge / Vert
    uint8_t gps_ok;  // 0 = Clignote Rouge / Jaune
    uint8_t sd_ok;   // 0 = Clignote Rouge / Blanc
} SystemHealth;

// Structure de suivi pour le clignotement non-bloquant de la LED
typedef struct {
    uint32_t last_toggle_tick;
    uint8_t toggle_state;
} SystemStatus;

// Prototypes des fonctions publiques
void SystemStatus_Init(SystemStatus *status);
void SystemStatus_Update(SystemStatus *status, SystemMode mode, SystemHealth health);

#endif // SYSTEM_STATUS_H
