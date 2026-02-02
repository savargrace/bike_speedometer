#include <Arduino.h>
#include "calculation.h"
#include "config.h"

struct WheelConfig
{
  float   circumference;
  uint8_t magnets;
};

static constexpr WheelConfig wheel = {
  .circumference = 2.0f * PI * (WHEEL_RADIUS_INCHES / INCHES_PER_FOOT) / FEET_PER_MILE,
  .magnets       = MAGNETS_PER_WHEEL
};

struct SpeedData
{
  float    speed_mph;
  float    distance_mi;
  uint32_t last_dist_update_time_ms;
};

static SpeedData data = {
  .speed_mph                = 0.0f, 
  .distance_mi              = 0.0f, 
  .last_dist_update_time_ms = 0u
};


// Calculates Speed in MPH from time between pulse detection and wheel circumference
float calc_speed_us(uint32_t period_us)
{
  if (period_us == 0u)
  {
    data.speed_mph = 0.0f;
  }
  else
  {
    const float miles_per_pulse = (wheel.circumference / wheel.magnets);
    data.speed_mph = miles_per_pulse * (US_PER_HOUR / static_cast<float>(period_us));
  }
  return data.speed_mph;
}

// Updates total distance travelled using current speed and elasped time.
float update_distance()
{
  uint32_t now_ms = millis();

  // Limits how frequently distance updates to a fixed timing interval for stability
  if (now_ms - data.last_dist_update_time_ms > DISTANCE_UPDATE_INTERVAL_MS)
  {
    const uint32_t elapsed_ms = now_ms - data.last_dist_update_time_ms;
    const float elapsed_hours = static_cast<float>(elapsed_ms) / MS_PER_HOUR;
    data.distance_mi += data.speed_mph * elapsed_hours;

    data.last_dist_update_time_ms = now_ms;
  }

  return data.distance_mi;
}
