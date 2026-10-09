#include "rtc_status.h"
#include "buttons.h"
#include "grove_rgb.h"
#include "grove_rtc_ds1307.h"
#include <stdio.h>

#define RTC_CHECK_INTERVAL_MS 1000U
#define RTC_ERROR_PHASE_MS 500U

void RTCStatus_Init(RTCStatus *status)
{
    uint32_t now = HAL_GetTick();

    status->error = !GroveRTC_Init();
    status->last_check = now;
    status->last_error_phase = now;
    status->red_phase = true;

    if (status->error) {
        printf("[RTC] Erreur d'acces a l'horloge\r\n");
    }
}

void RTCStatus_Update(RTCStatus *status, SystemMode mode)
{
    uint32_t now = HAL_GetTick();

    if ((now - status->last_check) >= RTC_CHECK_INTERVAL_MS) {
        RTC_DateTime date_time;
        bool previous_error = status->error;

        status->error = !GroveRTC_GetDateTime(&date_time);
        status->last_check = now;

        if (status->error != previous_error) {
            if (status->error) {
                printf("[RTC] Erreur d'acces a l'horloge\r\n");
                status->last_error_phase = now;
                status->red_phase = true;
            } else {
                printf("[RTC] Communication retablie\r\n");
                update_led_color(mode);
            }
        }
    }

    if (status->error &&
        (now - status->last_error_phase) >= RTC_ERROR_PHASE_MS) {
        status->last_error_phase = now;
        status->red_phase = !status->red_phase;
        GroveRGB_SetColor(status->red_phase ? 255 : 0,
                          0,
                          status->red_phase ? 0 : 255);
    }
}
