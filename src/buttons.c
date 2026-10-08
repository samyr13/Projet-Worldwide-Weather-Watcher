#include "buttons.h"
#include "modes.h"
#include "grove_rgb.h"
#include <stdio.h>

// Broches du Port A0 du Shield Grove (PA0 = Rouge, PA1 = Vert)
#define BTN_RED_PIN        GPIO_PIN_0
#define BTN_RED_PORT       GPIOA
#define BTN_GREEN_PIN      GPIO_PIN_1
#define BTN_GREEN_PORT     GPIOA

// ⚠️ Active LOW (1 au repos, 0 appuyé)
#define BTN_ACTIVE_LEVEL   GPIO_PIN_RESET 

typedef enum {
    BTN_STATE_RELEASED,  // Relâché
    BTN_STATE_PRESSED,   // Appui en cours
    BTN_STATE_HANDLED    // Action exécutée (verrouillé jusqu'au relâchement)
} ButtonState;

static ButtonState red_state = BTN_STATE_RELEASED;
static ButtonState green_state = BTN_STATE_RELEASED;

static uint32_t red_start_time = 0;
static uint32_t green_start_time = 0;

static SystemMode previous_mode = MODE_STANDARD;
static uint32_t configuration_last_activity = 0;

/**
 * @brief Initialisation GPIO avec PULLUP (1 au repos, 0 lors de l'appui)
 */
void buttons_init(void) {
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = BTN_RED_PIN | BTN_GREEN_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/**
 * @brief Test du bouton rouge au démarrage (Mode Configuration)
 */
void check_boot_mode(void) {
    buttons_init();

    HAL_Delay(10);

    if (HAL_GPIO_ReadPin(BTN_RED_PORT, BTN_RED_PIN) == BTN_ACTIVE_LEVEL) {
        mode_actuel = MODE_CONFIGURATION;
        configuration_last_activity = HAL_GetTick();
        printf("[BOOT] Bouton Rouge enfonce au Reset -> MODE CONFIGURATION\r\n");
    } else {
        mode_actuel = MODE_STANDARD;
        printf("[BOOT] Demarrage nominal -> MODE STANDARD\r\n");
    }
    update_led_color(mode_actuel);
}

/**
 * @brief Gestion des appuis de 5s pour basculer les modes
 */
void process_button_presses(void) {
    uint32_t now = HAL_GetTick();

    if (mode_actuel == MODE_CONFIGURATION &&
        (now - configuration_last_activity) >= CONFIGURATION_TIMEOUT_MS) {
        mode_actuel = MODE_STANDARD;
        update_led_color(mode_actuel);
        printf("[TIMEOUT] 30 minutes sans activite -> MODE STANDARD\r\n");
    }

    // ==========================================
    // 1. BOUTON VERT (PA1) -> Mode Économique
    // ==========================================
    uint8_t green_pressed = (HAL_GPIO_ReadPin(BTN_GREEN_PORT, BTN_GREEN_PIN) == BTN_ACTIVE_LEVEL);

    if (green_pressed) {
        switch (green_state) {
            case BTN_STATE_RELEASED:
                green_state = BTN_STATE_PRESSED;
                green_start_time = now;
                if (mode_actuel == MODE_CONFIGURATION) {
                    configuration_last_activity = now;
                }
                printf("[BOUTON] Vert appuye... Maintenez 5s\r\n");
                break;

            case BTN_STATE_PRESSED:
                if ((now - green_start_time) >= LONG_PRESS_TIME_MS) {
                    if (mode_actuel == MODE_STANDARD) {
                        previous_mode = MODE_STANDARD;
                        mode_actuel = MODE_ECONOMIQUE;
                        printf("[ACTION] Appui 5s Vert -> Passage en MODE ECONOMIQUE\r\n");
                        update_led_color(mode_actuel);
                    } else {
                        printf("[ACTION] Appui 5s Vert ignore : MODE ECONOMIQUE accessible depuis STANDARD uniquement\r\n");
                    }
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

    // ==========================================
    // 2. BOUTON ROUGE (PA0) -> Mode Maintenance
    // ==========================================
    uint8_t red_pressed = (HAL_GPIO_ReadPin(BTN_RED_PORT, BTN_RED_PIN) == BTN_ACTIVE_LEVEL);

    if (red_pressed) {
        switch (red_state) {
            case BTN_STATE_RELEASED:
                red_state = BTN_STATE_PRESSED;
                red_start_time = now;
                if (mode_actuel == MODE_CONFIGURATION) {
                    configuration_last_activity = now;
                }
                printf("[BOUTON] Rouge appuye... Maintenez 5s\r\n");
                break;

            case BTN_STATE_PRESSED:
                if ((now - red_start_time) >= LONG_PRESS_TIME_MS) {
                    if (mode_actuel == MODE_MAINTENANCE) {
                        mode_actuel = previous_mode;
                        printf("[ACTION] Appui 5s Rouge -> Sortie de Maintenance\r\n");
                        update_led_color(mode_actuel);
                    } else if (mode_actuel == MODE_STANDARD || mode_actuel == MODE_ECONOMIQUE) {
                        previous_mode = mode_actuel;
                        mode_actuel = MODE_MAINTENANCE;
                        printf("[ACTION] Appui 5s Rouge -> Passage en MODE MAINTENANCE\r\n");
                        update_led_color(mode_actuel);
                    } else {
                        printf("[ACTION] Appui 5s Rouge ignore dans MODE CONFIGURATION\r\n");
                    }
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

/**
 * @brief Commande de la LED RGB selon le mode
 */
void update_led_color(SystemMode mode) {
    switch (mode) {
        case MODE_STANDARD:
            GroveRGB_SetColor(0, 255, 0);   // Vert
            printf("[LED] Vert (0, 255, 0) - Mode Standard\r\n");
            break;
        case MODE_CONFIGURATION:
            GroveRGB_SetColor(255, 255, 0); // Jaune
            printf("[LED] Jaune (255, 255, 0) - Mode Configuration\r\n");
            break;
        case MODE_ECONOMIQUE:
            GroveRGB_SetColor(0, 0, 255);   // Bleu
            printf("[LED] Bleu (0, 0, 255) - Mode Économique\r\n");
            break;
        case MODE_MAINTENANCE:
            GroveRGB_SetColor(255, 165, 0); // Orange
            printf("[LED] Orange (255, 165, 0) - Mode Maintenance\r\n");
            break;
    }
}