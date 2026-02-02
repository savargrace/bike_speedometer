#ifndef EEPROM_ODOMETER_H
#define EEPROM_ODOMETER_H

void odom_init();

void odom_update(bool is_active, float new_reading_mi);

float odom_read();

#endif // EEPROM_ODOMETER_H