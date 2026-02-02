#include <Arduino.h>
#include "config.h"
#include "hall_sensor.h"
#include "calculation.h"
#include "ride_time.h"
#include "displaySPI.h"
#include "calorie_tracker.h"
#include "power_manager.h"
#include "eeprom_odometer.h"

static uint32_t period_us = 0u;
static float    speed_mph = 0.0f;
static float    distance_mi = 0.0f;
static float    lifetime_mi = 0.0f;

static RideTime time = {
    .seconds = 0u,
    .minutes = 0u,
    .hours = 0u,
    .last_update_timer = 0u,
};

static float cal_per_hour = 0.0f;
static float cal_burned = 0.0f;

static uint32_t last_display_refresh = 0u;
static uint32_t last_state_update = 0u;

static void update_state(bool is_active);

void setup()
{
  sensor_init();
  display_init();
  odom_init();

#if DEBUG_MODE
  DEBUG_BEGIN(9600);
  DEBUG_PRINTLN("=== Speedometer Debug Mode ===");
#endif
}

void loop()
{
  const uint32_t now_us = micros();
  const uint32_t now_ms = millis();

  sensor_update_period_us(now_us);

  bool is_active = sensor_is_active(now_us);

  // odom_update(is_active, distance_mi);

  if (sleep_update(now_ms, is_active))
  {
    enter_sleep();
  }

  if (now_ms - last_state_update >= STATE_UPDATE_INTERVAL_MS)
  {
    update_state(is_active);

    last_state_update = now_ms;
  }

  if (now_ms - last_display_refresh >= DISPLAY_REFRESH_INTERVAL_MS)
  {
    display_OLED(speed_mph, distance_mi, time, cal_per_hour, cal_burned, lifetime_mi);

    last_display_refresh = now_ms;
  }

#if DEBUG_MODE
  static uint32_t lastDebugPrint = 0;

  if (now_ms - lastDebugPrint > 1000)
  {
    DEBUG_PRINTLN("=== LOOP REFRESH ===");
    DEBUG_PRINT("Period: ");
    DEBUG_PRINT(period_ms);
    DEBUG_PRINTLN(" ms");
    DEBUG_PRINT("Speed: ");
    DEBUG_PRINT(speed_mph);
    DEBUG_PRINTLN(" mph");
    DEBUG_PRINT("Distance: ");
    DEBUG_PRINT(distance_mi);
    DEBUG_PRINTLN(" miles");
    DEBUG_PRINT("Calories Burned: ");
    DEBUG_PRINTLN(cal_burned);
    lastDebugPrint = now_ms;
  }

#endif
}

static void update_state(bool is_active)
{
  period_us = sensor_denoise_us();
  speed_mph = calc_speed_us(period_us);
  distance_mi = update_distance();

  time = calc_time(is_active);

  cal_per_hour = calc_cal(speed_mph);
  cal_burned = update_cal_burned();

  lifetime_mi = odom_read();
}
