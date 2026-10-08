#ifndef BUTTONS_H
#define BUTTONS_H

#include "stm32l4xx_hal.h"
#include "modes.h"

// Durée de maintien requise (5 secondes = 5000 ms)
#define LONG_PRESS_TIME_MS 5000

// Initialisation des broches GPIO pour le Port A0 Grove (PA0 et PA1)
void buttons_init(void);

// Test du bouton rouge au démarrage (Mode Configuration)
void check_boot_mode(void);

// Gestion des transitions de modes (appuis 5s)
void process_button_presses(void);

// Mise à jour de la couleur de la LED RGB
void update_led_color(SystemMode mode);

#endif // BUTTONS_H
