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
    (void)index;  // you effectively have only encoder 0

    // if (is_keyboard_left()) {
    //     // Left hand: rotate tank (change heading)
    //     if (clockwise) {
    //         tap_code16(OM_R);
    //     } else {
    //         tap_code16(OM_L);
    //     }
    // } else {
    //     // Right hand: move forward/back along current heading
    //     if (clockwise) {
    //         tap_code16(OM_U);
    //     } else {
    //         tap_code16(OM_D);
    //     }
    // }

    return false;
}
