#ifndef RTC_STATUS_H
#define RTC_STATUS_H

#include <stdbool.h>
#include <stdint.h>
#include "modes.h"

typedef struct {
    bool error;
    uint32_t last_check;
    uint32_t last_error_phase;
    bool red_phase;
} RTCStatus;

void RTCStatus_Init(RTCStatus *status);
void RTCStatus_Update(RTCStatus *status, SystemMode mode);

#endif
