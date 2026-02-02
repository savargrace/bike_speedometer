#ifndef SLEEP_MODE_H
#define SLEEP_MODE_H
#include <Arduino.h>

bool sleep_update(uint32_t now_ms, bool is_active); 

void enter_sleep();

#endif // SLEEP_MODE_H