#ifndef RIDE_TIME_H
#define RIDE_TIME_H

#include <Arduino.h>

struct RideTime {
  uint8_t seconds;
  uint8_t minutes;
  uint8_t hours;
  uint32_t last_update_timer;
};

// Calculates Time Elasped
RideTime calc_time(bool is_active);

#endif // RIDE_TIME_H