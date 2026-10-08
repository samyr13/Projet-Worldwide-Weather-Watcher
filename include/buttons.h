#ifndef BUTTONS_H
#define BUTTONS_H

#include "stm32l4xx_hal.h"
#include "modes.h"

// Durée de maintien requise (5000 ms = 5 secondes)
#define LONG_PRESS_TIME_MS 5000

// Initialisation matérielle des GPIO (PA0 et PA1)
void buttons_init(void);

// Vérification du bouton rouge au boot (Mode Configuration)
void check_boot_mode(void);

// Traitement sécurisé des appuis de 5s dans la boucle principale
void process_button_presses(void);

// Mise à jour de la LED RGB selon le mode actif
void update_led_color(SystemMode mode);

#endif // BUTTONS_H
