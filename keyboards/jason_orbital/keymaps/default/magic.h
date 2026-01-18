#pragma once

#include <stdint.h>

void process_left_magic(uint16_t keycode, uint8_t mods);
void process_right_magic(uint16_t keycode, uint8_t mods);
bool process_record_user_for_magic(uint16_t keycode, keyrecord_t* record);
