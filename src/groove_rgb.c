#include "grove_rgb.h"

// =========================================================================
// CÂBLAGE EXACT SUR LE PORT D2 DU BASE SHIELD GROVE :
// Fil Jaune (D2 / PA10) -> connecté à CIN (Clock / Horloge) sur le module LED
// Fil Blanc (D3 / PB3)  -> connecté à DIN (Data / Données) sur le module LED
// =========================================================================
#define P9813_CLK_PORT   GPIOA
#define P9813_CLK_PIN    GPIO_PIN_10

#define P9813_DATA_PORT  GPIOB
#define P9813_DATA_PIN   GPIO_PIN_3

/**
 * @brief Petit délai microseconde pour cadencer l'horloge à une vitesse compatible P9813
 */
static void delay_us(uint32_t us) {
    volatile uint32_t count = us * 15;
    while (count--) {
        __NOP();
    }
}

/**
 * @brief Envoi d'un octet de données bit par bit (MSB first)
 */
static void P9813_SendByte(uint8_t data) {
    for (uint8_t i = 0; i < 8; i++) {
        // 1. Positionnement de la donnée sur le fil Blanc (DIN / PB3)
        if (data & 0x80) {
            HAL_GPIO_WritePin(P9813_DATA_PORT, P9813_DATA_PIN, GPIO_PIN_SET);
        } else {
            HAL_GPIO_WritePin(P9813_DATA_PORT, P9813_DATA_PIN, GPIO_PIN_RESET);
        }
        delay_us(1);

        // 2. Impulsion d'horloge sur le fil Jaune (CIN / PA10)
        HAL_GPIO_WritePin(P9813_CLK_PORT, P9813_CLK_PIN, GPIO_PIN_SET);
        delay_us(1);
        HAL_GPIO_WritePin(P9813_CLK_PORT, P9813_CLK_PIN, GPIO_PIN_RESET);
        delay_us(1);

        data <<= 1;
    }
}

/**
 * @brief Initialisation GPIO des broches PA10 (Clock) et PB3 (Data)
 */
void GroveRGB_Init(void) {
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // Configuration Broche Clock (PA10 - Fil Jaune)
    GPIO_InitStruct.Pin = P9813_CLK_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(P9813_CLK_PORT, &GPIO_InitStruct);

    // Configuration Broche Data (PB3 - Fil Blanc)
    GPIO_InitStruct.Pin = P9813_DATA_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(P9813_DATA_PORT, &GPIO_InitStruct);

    // Lignes au repos à 0
    HAL_GPIO_WritePin(P9813_CLK_PORT, P9813_CLK_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(P9813_DATA_PORT, P9813_DATA_PIN, GPIO_PIN_RESET);
}

/**
 * @brief Envoi de la trame de couleur RGB selon le protocole P9813
 */
void GroveRGB_SetColor(uint8_t red, uint8_t green, uint8_t blue) {
    // 1. Trame de début (32 bits à 0)
    for (uint8_t i = 0; i < 4; i++) {
        P9813_SendByte(0x00);
    }

    // 2. Calcul du préfixe d'en-tête (contrôle de parité des bits de couleur)
    uint8_t prefix = 0xC0;
    prefix |= ((~blue & 0xC0) >> 2);
    prefix |= ((~green & 0xC0) >> 4);
    prefix |= ((~red & 0xC0) >> 6);

    // 3. Envoi du paquet : Préfixe, Bleu, Vert, Rouge
    P9813_SendByte(prefix);
    P9813_SendByte(blue);
    P9813_SendByte(green);
    P9813_SendByte(red);

    // 4. Trame de fin (32 bits à 0)
    for (uint8_t i = 0; i < 4; i++) {
        P9813_SendByte(0x00);
    }
}
