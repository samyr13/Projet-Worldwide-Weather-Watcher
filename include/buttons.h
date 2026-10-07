#ifndef BUTTONS_H
#define BUTTONS_H

#include "stm32l4xx_hal.h"
#include "modes.h"

// Durée de maintien requise pour basculer de mode (5 secondes = 5000 ms)
#define LONG_PRESS_TIME_MS 5000

// Vérification du bouton rouge au démarrage (Mode Configuration)
void check_boot_mode(void);

// Traitement continu des appuis de 5s dans la boucle principale
void process_button_presses(void);

// Mise à jour de la couleur de la LED RGB en fonction du mode actif
void update_led_color(SystemMode mode);

#endif // BUTTONS_H
