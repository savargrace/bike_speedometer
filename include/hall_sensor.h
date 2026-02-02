#ifndef HALL_SENSOR_H
#define HALL_SENSOR_H

#include <Arduino.h>
#include "config.h"

// ISR for Hall Sensor
void sensor_ISR();

// Initializes Hall Sensor
void sensor_init();

// Updates Period Based on Sensor Triggers
void sensor_update_period_us(uint32_t now_us);

// Smoothes readings with EMA filter
uint32_t sensor_denoise_us();

// Getter functions
uint32_t sensor_get_denoised_period_us();

uint32_t sensor_get_last_trigger_time_us();

bool sensor_is_active(uint32_t nowMs);

#endif // HALL_SENSOR_H
