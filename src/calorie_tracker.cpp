#include <Arduino.h>
#include "calorie_tracker.h"
#include "config.h"

struct CalorieData
{
  uint16_t weight_lbs;
  uint8_t MET;
  float cal_per_hour;
  float cal_burned;
  uint32_t last_sample_ms;
};

static CalorieData calData = {
  .weight_lbs     = RIDER_WEIGHT_LBS,
  .MET            = 0u,
  .cal_per_hour   = 0.0f,
  .cal_burned     = 0.0f,
  .last_sample_ms = 0u
};

void calc_MET(float speed)
{
  if (speed < 1.0f)
  {
    calData.MET = 0u;
    return;
  }
  else if (speed < 10.0f)
  {
    calData.MET = 4u;
  }
  else if (speed < 12.0f)
  {
    calData.MET = 6u;
  }
  else if (speed < 14.0f)
  {
    calData.MET = 8u;
  }
  else if (speed < 16.0f)
  {
    calData.MET = 10u;
  }
  else if (speed < 20.0f)
  {
    calData.MET = 12u;
  }
  else
  {
    calData.MET = 14u;
  }
}

float calc_cal(float speed)
{
  uint32_t now = millis();

  if (calData.last_sample_ms == 0u)
  {
    calData.last_sample_ms = now;
    return calData.cal_per_hour;
  }

  uint32_t sample = now - calData.last_sample_ms;
  if (sample == 0u)
  {
    return calData.cal_per_hour;
  }

  calData.last_sample_ms = now;

  calc_MET(speed);

  if (calData.MET == 0u)
  {
    calData.cal_per_hour = 0.0f;
    return calData.cal_per_hour;
  }

  float weight_kg = static_cast<float>(calData.weight_lbs) * LBS_TO_KG;

  calData.cal_per_hour = static_cast<float>(calData.MET) * 21.0f * weight_kg / 20.0f;

  float sample_hours = static_cast<float>(sample) / MS_PER_HOUR;

  calData.cal_burned += calData.cal_per_hour * sample_hours;

  return calData.cal_per_hour;
}

float update_cal_burned()
{
  return calData.cal_burned;
}