#include "ride_time.h"
#include <Arduino.h>


RideTime rideTime = {0u, 0u, 0u, 0u};

// Calculates time elapsed since start of ride
RideTime calc_time(bool is_active)
{
  if (is_active)
  {
    uint32_t now = millis();
    
    if (now - rideTime.last_update_timer > 1000u)
    {
      rideTime.last_update_timer = now;
      rideTime.seconds++;

      if (rideTime.seconds == 60u)
      {
        rideTime.seconds = 0u;
        rideTime.minutes++;
      }

      if (rideTime.minutes == 60u)
      {
        rideTime.minutes = 0u;
        rideTime.hours++;
      }
    }
  } 
  return rideTime;
}