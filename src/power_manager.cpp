#include <power_manager.h>
#include <displaySPI.h>
#include <avr/sleep.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <config.h>


static uint32_t stop_start_time_ms = 0u;
static bool     stop_timer_active = false;

bool sleep_update(uint32_t now_ms, bool is_active)
{
  // Latch stop start time once sensor has transitioned to a stop
  if (is_active)
  {
    stop_start_time_ms = 0u;
    stop_timer_active = false;
    return false;
  }
  else if (!is_active && !stop_timer_active)
  {
    stop_start_time_ms = now_ms;
    stop_timer_active = true;
    return false;
  }
  else if (!is_active && stop_timer_active && now_ms - stop_start_time_ms >= POWER_DOWN_TIMEOUT_MS)
  {
#if DEBUG_MODE
    DEBUG_PRINTLN("=== STOP TIMER DEBUG ===")
    DEBUG_PRINTLN(POWER_DOWN_TIMEOUT_MS);
    DEBUG_PRINTLN(now_ms - stop_start_time_ms);
#endif

    return true;
  }
  return false;
}

void enter_sleep()
{
  display_off();

  cli(); // disable interrupts

  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // power down mode
  sleep_enable();

  sei(); // enable interrupts

  sleep_cpu();
  sleep_disable();

  display_on();
  // sleep timer reset on wake
  stop_start_time_ms = 0u;
  stop_timer_active = false;
}