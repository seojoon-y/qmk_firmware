#include QMK_KEYBOARD_H
#include "features/orbital_mouse.h"
#include "digitizer.h"

uint8_t encoder_counter = 0;

enum layer_names {
    _BASE,
};

enum custom_keycodes {
    CNR_TL = SAFE_RANGE,
    CNR_TR,
    CNR_BL,
    CNR_BR,
};

static void teleport_corner(uint8_t c) {
    digitizer_in_range_on();
    switch (c) {
        case 0:
            digitizer_set_position(0.0f, 0.0f);
            break;
        case 1:
            digitizer_set_position(1.0f, 0.0f);
            break;
        case 2:
            digitizer_set_position(0.0f, 1.0f);
            break;
        default:
        case 3:
            digitizer_set_position(1.0f, 1.0f);
            break;
    }
    digitizer_in_range_off();
}

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_BASE] = LAYOUT(
        CNR_TL, OM_U, CNR_TR,
        CNR_BL, OM_D, CNR_BR,

        KC_D, OM_L, KC_D,
        KC_B, OM_R, KC_B
    )
};

bool process_record_user(uint16_t keycode, keyrecord_t* record) {
    if (!process_orbital_mouse(keycode, record)) {
        return false;
    }

    if (!record->event.pressed) {
        return true;
    }

    switch (keycode) {
        case CNR_TL:
            teleport_corner(0);
            return false;
        case CNR_TR:
            teleport_corner(1);
            return false;
        case CNR_BL:
            teleport_corner(2);
            return false;
        case CNR_BR:
            teleport_corner(3);
            return false;
    }

    return true;
}

void housekeeping_task_user(void) {
    orbital_mouse_task();
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
