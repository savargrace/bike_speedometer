#include <Arduino.h>
#include "hall_sensor.h"
#include "config.h"

static volatile uint32_t last_isr_us = 0u;
static volatile uint32_t prev_isr_us = 0u;
static volatile uint16_t pulse_count_isr = 0u;

struct HallSensor
{
  uint32_t last_trigger_time_us;
  uint32_t current_period_us;
  uint32_t previous_period_us;
  uint32_t denoised_period_us;
};

static HallSensor sensor = {
    .last_trigger_time_us = 0u,
    .current_period_us = 0u,
    .previous_period_us = 0u,
    .denoised_period_us = 0u,
};

void sensor_ISR()
{
  uint32_t now_us = micros();
  if (now_us - last_isr_us >= DEBOUNCE_US)
  {
  prev_isr_us = last_isr_us;
  last_isr_us = now_us;
  pulse_count_isr++;
  }
}

void sensor_init()
{
  pinMode(HALL_SENSOR_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(HALL_SENSOR_PIN), sensor_ISR, FALLING);
}

void sensor_update_period_us(uint32_t now_us)
{
  noInterrupts();
  uint32_t prev_pulse_us = prev_isr_us;
  uint32_t last_pulse_us = last_isr_us;
  uint16_t pulse_count = pulse_count_isr;
  pulse_count_isr = 0;
  interrupts();

  #if DEBUG_MODE
  DEBUG_PRINTLN(prev_pulse_us);
  DEBUG_PRINTLN(last_pulse_us);
  DEBUG_PRINTLN(pulse_count);
  #endif

  if (pulse_count >= 1)
  {
    sensor.last_trigger_time_us = last_pulse_us;

    if (prev_pulse_us != 0u)
    {
      const uint32_t period_us = last_pulse_us - prev_pulse_us;
      if (period_us >= MIN_PERIOD_US)
      {
        sensor.current_period_us = period_us;
      }
    }
  }

  if (last_pulse_us != 0u && (now_us - last_pulse_us >= ZERO_TIMEOUT_US))
  {
    sensor.current_period_us = 0u;
  }
}

uint32_t sensor_denoise_us()
{
  // Intilization check to prevent subtraction error
  if (sensor.current_period_us == 0u)
  {
    sensor.denoised_period_us = 0u;
  }
  else if (sensor.current_period_us >= SENSOR_SLOW_SPEED_PERIOD_US)
  {
    // More smoothing for slow speeds
    sensor.denoised_period_us =
        (sensor.current_period_us + 7u * sensor.previous_period_us + 4u) >> 3;
  }
  else
  {
    // Less smoothing for faster speeds
    sensor.denoised_period_us =
        (sensor.current_period_us + 3u * sensor.previous_period_us + 2u) >> 2;
  }

  sensor.previous_period_us = sensor.denoised_period_us;

  return sensor.denoised_period_us;
}

// ================
// Getter Functions
// ================

uint32_t sensor_get_denoised_period_us()
{
  return sensor.denoised_period_us;
}

uint32_t sensor_get_last_trigger_time_us()
{
  return sensor.last_trigger_time_us;
}

bool sensor_is_active(uint32_t current_time)
{
  return (current_time - sensor.last_trigger_time_us) <= ZERO_TIMEOUT_US;
}
