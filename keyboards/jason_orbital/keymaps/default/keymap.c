#include QMK_KEYBOARD_H

uint8_t encoder_counter = 0;

enum layer_names {
    _BASE,
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_BASE] = LAYOUT(
        KC_A, KC_A, KC_A,
        KC_B, KC_B, KC_B,

        KC_C, KC_C, KC_C,
        KC_D, KC_D, KC_D
    )
};

bool encoder_update_user(uint8_t index, bool clockwise) {
    encoder_counter++;

    if (encoder_counter % 2 == 0) {
        return false;
    }
    
    // if (is_keyboard_left()) {
    if (index == 0) {
        if (clockwise) {
            tap_code16(KC_A);   // rotate right
        } else {
            tap_code16(KC_B);   // rotate left
        }
    } else {
        if (clockwise) {
            tap_code16(KC_D);   // move forward
        } else {
            tap_code16(KC_C);   // move backward
        }
    }
    return false;
}
