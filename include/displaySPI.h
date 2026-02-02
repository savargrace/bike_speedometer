#ifndef DISPLAYSPI_H
#define DISPLAYSPI_H
#include "ride_time.h"

void display_start_screen();

void display_init();

void display_OLED(float speed, float distance, RideTime time, float cal_per_hour, float cal_burned, float lifetime_mi);

void display_eeprom_corrupt();

void display_off();

void display_on();

#endif // DISPLAYSPI_H
