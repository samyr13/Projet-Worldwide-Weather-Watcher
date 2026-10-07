#include "buttons.h"
#include "modes.h"
#include "grove_rgb.h"
#include <stdio.h>

// Broches du Port A0 du Shield Grove (PA0 = Rouge, PA1 = Vert)
#define BTN_RED_PIN        GPIO_PIN_0
#define BTN_RED_PORT       GPIOA
#define BTN_GREEN_PIN      GPIO_PIN_1
#define BTN_GREEN_PORT     GPIOA

// ⚠️ NIVEAU ACTIF INVERSÉ : 1 au repos, 0 lors de l'appui (Active LOW)
#define BTN_ACTIVE_LEVEL   GPIO_PIN_RESET 

typedef enum {
    BTN_STATE_RELEASED,
    BTN_STATE_PRESSED,
    BTN_STATE_HANDLED
} ButtonState;

static ButtonState red_state = BTN_STATE_RELEASED;
static ButtonState green_state = BTN_STATE_RELEASED;

static uint32_t red_start_time = 0;
static uint32_t green_start_time = 0;

static SystemMode previous_mode = MODE_STANDARD;

/**
 * @brief Initialisation GPIO avec PULLUP pour la logique Active LOW
 */
void buttons_init(void) {
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = BTN_RED_PIN | BTN_GREEN_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP; // Fixe à 3.3V au repos
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    uint8_t red_raw = HAL_GPIO_ReadPin(BTN_RED_PORT, BTN_RED_PIN);
    uint8_t green_raw = HAL_GPIO_ReadPin(BTN_GREEN_PORT, BTN_GREEN_PIN);
    printf("[DIAG] Etat au repos -> PA0 (Rouge): %d | PA1 (Vert): %d (Attendu: 1 au repos)\r\n", red_raw, green_raw);
}

void check_boot_mode(void) {
    buttons_init();

    if (HAL_GPIO_ReadPin(BTN_RED_PORT, BTN_RED_PIN) == BTN_ACTIVE_LEVEL) {
        mode_actuel = MODE_CONFIGURATION;
        printf("[BOOT] Bouton Rouge enfonce -> Entree en MODE CONFIGURATION\r\n");
    } else {
        mode_actuel = MODE_STANDARD;
        printf("[BOOT] Demarrage nominal -> MODE STANDARD\r\n");
    }
    update_led_color(mode_actuel);
}

void process_button_presses(void) {
    uint32_t now = HAL_GetTick();

    // 1. BOUTON VERT (PA1) -> Mode Économique
    uint8_t green_pressed = (HAL_GPIO_ReadPin(BTN_GREEN_PORT, BTN_GREEN_PIN) == BTN_ACTIVE_LEVEL);

    if (green_pressed) {
        switch (green_state) {
            case BTN_STATE_RELEASED:
                green_state = BTN_STATE_PRESSED;
                green_start_time = now;
                printf("[BOUTON] Vert appuye... Maintenez 5 secondes\r\n");
                break;

            case BTN_STATE_PRESSED:
                if ((now - green_start_time) >= LONG_PRESS_TIME_MS) {
                    if (mode_actuel == MODE_ECONOMIQUE) {
                        mode_actuel = previous_mode;
                        printf("[ACTION] Appui 5s Vert -> Retour mode precedent\r\n");
                    } else {
                        previous_mode = mode_actuel;
                        mode_actuel = MODE_ECONOMIQUE;
                        printf("[ACTION] Appui 5s Vert -> Passage en MODE ECONOMIQUE\r\n");
                    }
                    update_led_color(mode_actuel);
                    green_state = BTN_STATE_HANDLED;
                }
                break;

            case BTN_STATE_HANDLED:
                break;
        }
    } else {
        if (green_state == BTN_STATE_PRESSED) {
            printf("[BOUTON] Vert relache trop tôt (%lu ms < 5000 ms)\r\n", now - green_start_time);
        }
        green_state = BTN_STATE_RELEASED;
    }

    // 2. BOUTON ROUGE (PA0) -> Mode Maintenance
    uint8_t red_pressed = (HAL_GPIO_ReadPin(BTN_RED_PORT, BTN_RED_PIN) == BTN_ACTIVE_LEVEL);

    if (red_pressed) {
        switch (red_state) {
            case BTN_STATE_RELEASED:
                red_state = BTN_STATE_PRESSED;
                red_start_time = now;
                printf("[BOUTON] Rouge appuye... Maintenez 5 secondes\r\n");
                break;

            case BTN_STATE_PRESSED:
                if ((now - red_start_time) >= LONG_PRESS_TIME_MS) {
                    if (mode_actuel == MODE_MAINTENANCE) {
                        mode_actuel = previous_mode;
                        printf("[ACTION] Appui 5s Rouge -> Sortie de MAINTENANCE\r\n");
                    } else {
                        previous_mode = mode_actuel;
                        mode_actuel = MODE_MAINTENANCE;
                        printf("[ACTION] Appui 5s Rouge -> Passage en MODE MAINTENANCE\r\n");
                    }
                    update_led_color(mode_actuel);
                    red_state = BTN_STATE_HANDLED;
                }
                break;

            case BTN_STATE_HANDLED:
                break;
        }
    } else {
        if (red_state == BTN_STATE_PRESSED) {
            printf("[BOUTON] Rouge relache trop tôt (%lu ms < 5000 ms)\r\n", now - red_start_time);
        }
        red_state = BTN_STATE_RELEASED;
    }
}

void update_led_color(SystemMode mode) {
    switch (mode) {
        case MODE_STANDARD:
            GroveRGB_SetColor(0, 255, 0);   // VERTE
            printf("[LED] Vert (0, 255, 0) - Mode Standard\r\n");
            break;
        case MODE_CONFIGURATION:
            GroveRGB_SetColor(255, 255, 0); // JAUNE
            printf("[LED] Jaune (255, 255, 0) - Mode Configuration\r\n");
            break;
        case MODE_ECONOMIQUE:
            GroveRGB_SetColor(0, 0, 255);   // BLEUE
            printf("[LED] Bleu (0, 0, 255) - Mode Économique\r\n");
            break;
        case MODE_MAINTENANCE:
            GroveRGB_SetColor(255, 165, 0); // ORANGE
            printf("[LED] Orange (255, 165, 0) - Mode Maintenance\r\n");
            break;
    }
}
