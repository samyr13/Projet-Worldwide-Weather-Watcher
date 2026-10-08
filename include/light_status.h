#ifndef LIGHT_STATUS_H
#define LIGHT_STATUS_H

#include <stdint.h>
#include "modes.h"

typedef struct {
    uint32_t last_read;
} LightStatus;

void LightStatus_Init(LightStatus *status);
void LightStatus_Update(LightStatus *status, SystemMode mode);

#endif
