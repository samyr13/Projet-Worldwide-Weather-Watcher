#include "buttons.h"
#include "modes.h"          // ⚠️ Assure-toi que cet include est présent
#include "grove_rgb.h"
#include "grove_button.h"
#include <stdio.h>

// Déclaration de la variable globale définie dans main.c
extern SystemMode mode_actuel;

// Broches GPIO pour le Grove Dual Button
#define BTN_RED_PIN        GPIO_PIN_0
#define BTN_RED_PORT       GPIOA
#define BTN_GREEN_PIN      GPIO_PIN_1
#define BTN_GREEN_PORT     GPIOA

// Variables de chronométrage
static uint32_t red_press_start = 0;
static uint32_t green_press_start = 0;
static uint8_t red_was_pressed = 0;
static uint8_t green_was_pressed = 0;

static SystemMode previous_mode = MODE_STANDARD;

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

void process_button_presses(void) {
    uint32_t now = HAL_GetTick();

    // 1. BOUTON VERT (Mode Économique)
    if (HAL_GPIO_ReadPin(BTN_GREEN_PORT, BTN_GREEN_PIN) == GPIO_PIN_SET) {
        if (!green_was_pressed) {
            green_press_start = now;
            green_was_pressed = 1;
        } else if ((now - green_press_start) >= LONG_PRESS_TIME_MS) {
            if (mode_actuel == MODE_ECONOMIQUE) {
                mode_actuel = previous_mode;
                printf("[ACTION] Appui 5s Vert -> Retour au mode precedent\r\n");
            } else {
                previous_mode = mode_actuel;
                mode_actuel = MODE_ECONOMIQUE;
                printf("[ACTION] Appui 5s Vert -> Passage en MODE ECONOMIQUE\r\n");
            }
            update_led_color(mode_actuel);
            green_was_pressed = 0;
        }
    } else {
        green_was_pressed = 0;
    }

    // 2. BOUTON ROUGE (Mode Maintenance)
    if (HAL_GPIO_ReadPin(BTN_RED_PORT, BTN_RED_PIN) == GPIO_PIN_SET) {
        if (!red_was_pressed) {
            red_press_start = now;
            red_was_pressed = 1;
        } else if ((now - red_press_start) >= LONG_PRESS_TIME_MS) {
            if (mode_actuel == MODE_MAINTENANCE) {
                mode_actuel = previous_mode;
                printf("[ACTION] Appui 5s Rouge -> Sortie de MAINTENANCE\r\n");
            } else {
                previous_mode = mode_actuel;
                mode_actuel = MODE_MAINTENANCE;
                printf("[ACTION] Appui 5s Rouge -> Passage en MODE MAINTENANCE\r\n");
            }
            update_led_color(mode_actuel);
            red_was_pressed = 0;
        }
    } else {
        red_was_pressed = 0;
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
