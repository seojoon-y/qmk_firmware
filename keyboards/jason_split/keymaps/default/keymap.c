#include QMK_KEYBOARD_H

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT(
        KC_Q, KC_W, KC_E,
        KC_A, KC_S, KC_D,

        KC_Y, KC_U, KC_I,
        KC_H, KC_J, KC_K
    )
};

bool encoder_update_user(uint8_t index, bool clockwise) {
    if (clockwise) {
        tap_code16(MS_WHLD);
    } else {
        tap_code16(MS_WHLU);
    }
    return false;
}

