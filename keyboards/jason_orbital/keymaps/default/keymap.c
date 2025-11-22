#include QMK_KEYBOARD_H

enum layer_names {
    _BASE,
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_BASE] = LAYOUT(
        OM_L, OM_U, OM_R,
        KC_A, OM_D, KC_D,

        OM_L, OM_U, OM_R,
        KC_G, OM_D, KC_F
    )
};

bool encoder_update_user(uint8_t index, bool clockwise) {
    if (is_keyboard_left()) {
        if (clockwise) {
            tap_code16(KC_E);   // rotate right
        } else {
            tap_code16(KC_F);   // rotate left
        }
    } else {
        if (clockwise) {
            tap_code16(KC_G);   // move forward
        } else {
            tap_code16(KC_H);   // move backward
        }
    }
    return false;
}
