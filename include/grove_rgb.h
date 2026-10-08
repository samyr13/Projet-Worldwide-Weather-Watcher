#ifndef GROVE_RGB_H
#define GROVE_RGB_H

#include "stm32l4xx_hal.h"

// Initialisation du driver pour la LED Chainable V2 (P9813 sur le port D2)
void GroveRGB_Init(void);

// Commande de la couleur RGB (R, G, B de 0 à 255)
void GroveRGB_SetColor(uint8_t red, uint8_t green, uint8_t blue);

#endif // GROVE_RGB_H
