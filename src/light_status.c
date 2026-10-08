#include "light_status.h"
#include "grove_light.h"
#include "stm32l4xx_hal.h"
#include <stdio.h>

#define LIGHT_READ_INTERVAL_MS 1000U
#define LIGHT_READ_ERROR 0xFFFFU

void LightStatus_Init(LightStatus *status)
{
    GroveLight_Init();
    status->last_read = HAL_GetTick();
}

void LightStatus_Update(LightStatus *status, SystemMode mode)
{
    uint32_t now = HAL_GetTick();

    if (mode != MODE_STANDARD ||
        (now - status->last_read) < LIGHT_READ_INTERVAL_MS) {
        return;
    }

    uint16_t raw_value = GroveLight_ReadRaw();
    status->last_read = now;

    if (raw_value == LIGHT_READ_ERROR) {
        printf("[LUMINOSITE] Erreur de lecture du capteur\r\n");
        return;
    }

    printf("[LUMINOSITE] Valeur brute ADC: %u / 4095\r\n", raw_value);
}
