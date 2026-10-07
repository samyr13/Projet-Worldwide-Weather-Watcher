#include "buttons.h"
#include "grove_rgb.h"     // Utilise votre driver Grove RGB
#include "grove_button.h"  // Utilise votre driver Grove Button
#include <stdio.h>

// Broches d'entrée pour le Grove Dual Button
#define BTN_RED_PIN        GPIO_PIN_0
#define BTN_RED_PORT       GPIOA
#define BTN_GREEN_PIN      GPIO_PIN_1
#define BTN_GREEN_PORT     GPIOA

// Variables internes de chronométrage
static uint32_t red_press_start = 0;
static uint32_t green_press_start = 0;
static uint8_t red_was_pressed = 0;
static uint8_t green_was_pressed = 0;

// Mémoire du dernier mode avant bascule
static SystemMode previous_mode = MODE_STANDARD;

/**
 * @brief Vérifie au boot si le bouton rouge est maintenu pour passer en Mode Config.
 */
void check_boot_mode(void) {
    if (HAL_GPIO_ReadPin(BTN_RED_PORT, BTN_RED_PIN) == GPIO_PIN_SET) {
        mode_actuel = MODE_CONFIGURATION;
        printf("[BOOT] Bouton Rouge enfonce -> Entree en MODE CONFIGURATION\r\n");
    } else {
        mode_actuel = MODE_STANDARD;
        printf("[BOOT] Demarrage nominal -> MODE STANDARD\r\n");
    }
    update_led_color(mode_actuel);
}

/**
 * @brief Traite les appuis longs de 5s sur les boutons Vert et Rouge.
 */
void process_button_presses(void) {
    uint32_t now = HAL_GetTick();

    // -------------------------------------------------------------
    // 1. BOUTON VERT : Bascule Mode Économique
    // -------------------------------------------------------------
    if (HAL_GPIO_ReadPin(BTN_GREEN_PORT, BTN_GREEN_PIN) == GPIO_PIN_SET) {
        if (!green_was_pressed) {
            green_press_start = now;  // Top départ
            green_was_pressed = 1;
        } else if ((now - green_press_start) >= LONG_PRESS_TIME_MS) {
            // Appui de 5s confirmé
            if (mode_actuel == MODE_ECONOMIQUE) {
                mode_actuel = previous_mode;
                printf("[ACTION] Appui 5s Vert -> Retour au mode precedent\r\n");
            } else {
                previous_mode = mode_actuel;
                mode_actuel = MODE_ECONOMIQUE;
                printf("[ACTION] Appui 5s Vert -> Passage en MODE ECONOMIQUE\r\n");
            }
            update_led_color(mode_actuel);
            green_was_pressed = 0; // Réinitialisation de l'état
        }
    } else {
        green_was_pressed = 0; // Bouton relâché
    }

    // -------------------------------------------------------------
    // 2. BOUTON ROUGE : Bascule Mode Maintenance
    // -------------------------------------------------------------
    if (HAL_GPIO_ReadPin(BTN_RED_PORT, BTN_RED_PIN) == GPIO_PIN_SET) {
        if (!red_was_pressed) {
            red_press_start = now; // Top départ
            red_was_pressed = 1;
        } else if ((now - red_press_start) >= LONG_PRESS_TIME_MS) {
            // Appui de 5s confirmé
            if (mode_actuel == MODE_MAINTENANCE) {
                mode_actuel = previous_mode;
                printf("[ACTION] Appui 5s Rouge -> Sortie de MAINTENANCE\r\n");
            } else {
                previous_mode = mode_actuel;
                mode_actuel = MODE_MAINTENANCE;
                printf("[ACTION] Appui 5s Rouge -> Passage en MODE MAINTENANCE\r\n");
            }
            update_led_color(mode_actuel);
            red_was_pressed = 0; // Réinitialisation
        }
    } else {
        red_was_pressed = 0; // Bouton relâché
    }
}

/**
 * @brief Pilote la couleur de la LED RGB selon les spécifications 3W.
 */
void update_led_color(SystemMode mode) {
    switch (mode) {
        case MODE_STANDARD:
            grove_rgb_set_color(0, 255, 0);   // VERTE
            printf("[LED] Vert (0, 255, 0) - Mode Standard\r\n");
            break;
        case MODE_CONFIGURATION:
            grove_rgb_set_color(255, 255, 0); // JAUNE
            printf("[LED] Jaune (255, 255, 0) - Mode Configuration\r\n");
            break;
        case MODE_ECONOMIQUE:
            grove_rgb_set_color(0, 0, 255);   // BLEUE
            printf("[LED] Bleu (0, 0, 255) - Mode Économique\r\n");
            break;
        case MODE_MAINTENANCE:
            grove_rgb_set_color(255, 165, 0); // ORANGE
            printf("[LED] Orange (255, 165, 0) - Mode Maintenance\r\n");
            break;
    }
}
