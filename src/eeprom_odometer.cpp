// EEPROM Bike Odometer
// Stores distance in feet for accuracy as well as a monotonic version
//
#include <eeprom_odometer.h>
#include <displaySPI.h>
#include <Arduino.h>
#include <EEPROM.h>
#include <config.h>

// Avoid saving on initizaltion until bike has started moving
static bool first_save_since_stopped = false;

// Flag to avoid saving if no new data was recorded
static bool needs_save = false;

typedef struct
{
  uint32_t reading_ft;
  uint32_t version;
  uint8_t  checksum;
} Odometer;

// Two records, primary and backup, for power loss and corruption resilience
static Odometer primary = {
    .reading_ft = 0u,
    .version = 0u,
    .checksum = 0u,
};

static Odometer backup = {
    .reading_ft = 0u,
    .version = 0u,
    .checksum = 0u,
};

// CRC and Mask Macros
constexpr uint16_t PRIMARY_ADDRESS = 0u;
constexpr uint16_t BACKUP_ADDRESS = PRIMARY_ADDRESS + sizeof(Odometer);

constexpr uint8_t CRC8_MSB_MASK = 0x80;
constexpr uint8_t CRC8_POLY = 0x07;

constexpr uint8_t BYTE_EXTRACT_MASK = 0xFF;

// 8 bit CRC to ensure data validity and choose between primary, backup, or reset
// CRC only uses reading_ft and version and excludes checksum
static uint8_t odom_crc_8(const uint8_t* data, size_t length)
{
  uint8_t crc = 0x00;

  for (size_t i = 0; i < length; i++)
  {
    crc ^= data[i]; // XOR data byte into crc

    for (uint8_t bit = 0; bit < 8; bit++)
    {
      if (crc & CRC8_MSB_MASK) // if MSB = 1
      {
        crc = (crc << 1) ^ CRC8_POLY; // CRC-8 polynomial (x^8 + x^2 + x + 1)
      }
      else
      {
        crc <<= 1;
      }
    }
  }

  return crc;
}

static uint8_t checksum_calc(const Odometer& primary)
{
  uint8_t data[8];

  // Extract each byte using a mask for CRC calculation to avoid issues with compiler padding
  data[0] = static_cast<uint8_t>((primary.reading_ft >> 0) & BYTE_EXTRACT_MASK);
  data[1] = static_cast<uint8_t>((primary.reading_ft >> 8) & BYTE_EXTRACT_MASK);
  data[2] = static_cast<uint8_t>((primary.reading_ft >> 16) & BYTE_EXTRACT_MASK);
  data[3] = static_cast<uint8_t>((primary.reading_ft >> 24) & BYTE_EXTRACT_MASK);

  data[4] = static_cast<uint8_t>((primary.version >> 0) & BYTE_EXTRACT_MASK);
  data[5] = static_cast<uint8_t>((primary.version >> 8) & BYTE_EXTRACT_MASK);
  data[6] = static_cast<uint8_t>((primary.version >> 16) & BYTE_EXTRACT_MASK);
  data[7] = static_cast<uint8_t>((primary.version >> 24) & BYTE_EXTRACT_MASK);

  return odom_crc_8(data, sizeof(data));
}

void odom_init()
{
  EEPROM.get(PRIMARY_ADDRESS, primary);
  uint8_t expected = checksum_calc(primary);

  EEPROM.get(BACKUP_ADDRESS, backup);
  uint8_t expected_backup = checksum_calc(backup);

  // Recovery logic:
  // - Both valid: pick the record with the newest version and update the other
  // - Only one valid: copy it to the other record
  // - Both invalid: reset both records to 0
  if (expected == primary.checksum && expected_backup == backup.checksum)
  {
    if (primary.version >= backup.version)
    {
      backup = primary;
      return;
    }
    else if (primary.version < backup.version)
    {
      primary = backup;
      return;
    }
  }
  else if (expected == primary.checksum && expected_backup != backup.checksum)
  {
    backup = primary;
    EEPROM.put(BACKUP_ADDRESS, backup);
  }
  else if (expected != primary.checksum && expected_backup == backup.checksum)
  {
    primary = backup;
    EEPROM.put(PRIMARY_ADDRESS, primary);
  }
  else
  {
    display_eeprom_corrupt();

    // Reset odometer and write to eeprom if both saves are corrupt
    primary = {};
    backup = {};

    // Reset checksum to ensure validity next write
    primary.checksum = checksum_calc(primary);
    backup.checksum = checksum_calc(backup);

    EEPROM.put(PRIMARY_ADDRESS, primary);
    EEPROM.put(BACKUP_ADDRESS, backup);
  }
}

void odom_update(bool is_active, float new_reading_mi)
{
  uint32_t temp_reading_ft = static_cast<uint32_t>(new_reading_mi * FEET_PER_MILE + 0.5);

  // Only update values if the reading has changed
  if (primary.reading_ft != temp_reading_ft)
  {
    backup = primary;

    primary.reading_ft = temp_reading_ft;
    primary.version++;
    primary.checksum = checksum_calc(primary);

    needs_save = true;
  }

  // Save only when transitioning from moving to stopped to reduce EEPROM wear
  if (!is_active && first_save_since_stopped && needs_save)
  {
    first_save_since_stopped = false;

    EEPROM.put(BACKUP_ADDRESS, backup);
    EEPROM.put(PRIMARY_ADDRESS, primary);
    needs_save = false;
  }
  else if (is_active)
  {
    first_save_since_stopped = true;
  }
}

float odom_read()
{
  return (primary.reading_ft) / 5280.0f;
}
