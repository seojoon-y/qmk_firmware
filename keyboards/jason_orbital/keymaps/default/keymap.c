#include QMK_KEYBOARD_H
#include "features/orbital_mouse.h"

enum layer_names {
    _BASE,
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_BASE] = LAYOUT(
        KC_Q, KC_W, KC_E,
        KC_A, KC_S, KC_D,

        KC_Y, KC_U, KC_I,
        KC_H, KC_J, KC_K
    )
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (!process_orbital_mouse(keycode, record)) {
        return false;
    }
    return true;
}

void housekeeping_task_user(void) {
    orbital_mouse_task();
}

bool encoder_update_user(uint8_t index, bool clockwise) {
    switch (index) {
        case 0:
            if (clockwise) {
                tap_code16(OM_R);
            } else {
                tap_code16(OM_L);
            }
            break;
        case 1:
            if (clockwise) {
                tap_code16(OM_U);
            } else {
                tap_code16(OM_D);
            }
            break;
    }
    return false;
}
