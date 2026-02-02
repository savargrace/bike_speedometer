#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ===================
// User Configuration
// ===================
constexpr uint8_t RIDER_WEIGHT_LBS = 195u;
constexpr float   WHEEL_RADIUS_INCHES = 13.75f;
constexpr uint8_t HALL_SENSOR_PIN = 2u;
constexpr uint8_t MAGNETS_PER_WHEEL = 2u;

// =================
// Unit Definitions
// =================
constexpr uint8_t  INCHES_PER_FOOT = 12u;
constexpr uint16_t FEET_PER_MILE = 5280u;

constexpr uint32_t US_PER_SEC = 1000000u;
constexpr uint32_t MS_PER_SEC = 1000u;
constexpr uint32_t SEC_PER_MIN = 60u;
constexpr uint32_t MIN_PER_HOUR = 60u;
constexpr float    MS_PER_HOUR = static_cast<float>(MS_PER_SEC) * SEC_PER_MIN * MIN_PER_HOUR;
constexpr float    US_PER_HOUR = static_cast<float>(US_PER_SEC) * SEC_PER_MIN * MIN_PER_HOUR;

constexpr float    LBS_TO_KG = 0.45359237f;

// ==================
// Timing Parameters
// ==================
constexpr uint32_t DEBOUNCE_US = 5000u;
constexpr uint32_t MIN_PERIOD_US = 3000u; // ignore pulses faster than this
constexpr uint32_t ZERO_TIMEOUT_US = 30u * US_PER_SEC;

constexpr uint32_t DISTANCE_UPDATE_INTERVAL_MS = 250u;
constexpr uint32_t SENSOR_SLOW_SPEED_PERIOD_US = 500000u;

constexpr uint32_t DISPLAY_REFRESH_INTERVAL_MS = 500u;
constexpr uint32_t STATE_UPDATE_INTERVAL_MS = 200u;
constexpr uint32_t POWER_DOWN_TIMEOUT_MS = 30000u;

// ======================
// Display Configuration
// ======================
constexpr uint8_t SPI_CS_PIN = 10;
constexpr uint8_t SPI_DC_PIN = 9;
constexpr uint8_t SPI_RES_PIN = 8;



// ======
// Debug
// ======
#if DEBUG_MODE
#  define DEBUG_BEGIN(baud) Serial.begin(baud);
#  define DEBUG_PRINTLN(x) Serial.println(x);
#  define DEBUG_PRINT(x) Serial.print(x);
#else
#  define DEBUG_BEGIN(baud) ((void)0)
#  define DEBUG_PRINTLN(x) ((void)0)
#  define DEBUG_PRINT(x) ((void)0)
#endif

#endif // CONFIG_H
