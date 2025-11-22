#include QMK_KEYBOARD_H

enum layer_names {
    _BASE,
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_BASE] = LAYOUT(
        OM_L, OM_U, OM_R,
        KC_A, OM_D, KC_D,

        KC_Y, KC_U, KC_I,
        KC_H, KC_J, KC_K
    )
};

bool encoder_update_user(uint8_t index, bool clockwise) {
    if (index == 0) {
        if (clockwise) {
            tap_code16(KC_A);   // rotate right
        } else {
            tap_code16(KC_B);   // rotate left
        }
    } else {
        if (clockwise) {
            tap_code16(KC_C);   // move forward
        } else {
            tap_code16(KC_D);   // move backward
        }
    }
    return false;
}
