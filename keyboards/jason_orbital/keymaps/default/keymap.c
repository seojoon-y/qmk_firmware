#include QMK_KEYBOARD_H

#include "features/orbital_mouse.h"

uint8_t encoder_counter = 0;

enum layer_names {
    _BASE,
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_BASE] = LAYOUT(
        KC_A, OM_U, KC_A,
        KC_B, OM_D, KC_B,

        KC_C, OM_L, KC_C,
        KC_D, OM_R, KC_D
    )
};

bool process_record_user(uint16_t keycode, keyrecord_t* record) {
  if (!process_orbital_mouse(keycode, record)) { return false; }

  // Your macros ...
  return true;
}

void housekeeping_task_user(void) {
  orbital_mouse_task();

  // Other tasks ...
}

bool encoder_update_user(uint8_t index, bool clockwise) {
    encoder_counter = (encoder_counter + 1) % 2;
    bool is_test = false;

    if (encoder_counter % 2 == 0) {
        return false;
    }

    if (index == 0) {
        if (clockwise) {
            if (is_test) tap_code16(KC_A);
            orbital_mouse_instant_turn(-1);
        } else {
            if (is_test) tap_code16(KC_B);
            orbital_mouse_instant_turn(1);
        }
    } else {
        if (clockwise) {
            if (is_test) tap_code16(KC_D);
            orbital_mouse_instant_step(-1);
        } else {
            if (is_test) tap_code16(KC_C);
            orbital_mouse_instant_step(1);
        }
    }
    return false;
}
