#ifndef SPEED_CALC_H
#define SPEED_CALC_H

#include <Arduino.h>
#include "config.h"

float calc_speed_us(uint32_t period_us);

float update_distance();

#endif // SPEED_CALC_H
