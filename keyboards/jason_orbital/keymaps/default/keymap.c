#include QMK_KEYBOARD_H
#include "features/orbital_mouse.h"
#include "digitizer.h"
#include "analog.h"
#include <math.h>


#define ANALOG_JOYSTICK_X_AXIS_PIN_LEFT GP26
#define ANALOG_JOYSTICK_Y_AXIS_PIN_LEFT GP27

#define ANALOG_JOYSTICK_X_AXIS_PIN_RIGHT GP28
#define ANALOG_JOYSTICK_Y_AXIS_PIN_RIGHT GP29


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
        KC_1, KC_L, KC_D, KC_C, KC_Z, KC_SPC,
        KC_B, KC_R, KC_T, KC_S, KC_V, KC_SPC,
        KC_N, KC_Q, KC_M, KC_W, KC_G, KC_SPC,

        KC_A, KC_B, KC_C, KC_D, KC_E, KC_BSPC,
        KC_G, KC_H, KC_I, KC_J, KC_K, KC_BSPC,
        KC_M, KC_N, KC_O, KC_P, KC_Q, KC_BSPC
    )
};

// CNR_TL, OM_U, CNR_TR,
// CNR_BL, OM_D, CNR_BR,

// KC_D, OM_L, KC_D,
// KC_B, OM_R, KC_B

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
    bool is_test = true;

    // if (encoder_counter % 2 == 0) {
    //     return false;
    // }

    if (index == 0) {
        if (clockwise) {
            if (is_test) {
                tap_code16(KC_A);
            } else {
                // orbital_mouse_instant_turn(-1);
                orbital_mouse_instant_step(1);
            }
        } else {
            if (is_test) {
                tap_code16(KC_B);
            } else {
                // orbital_mouse_instant_turn(1);
                orbital_mouse_instant_step(1);
            }
                
        }
    } else {
        if (clockwise) {
            if (is_test) {
                tap_code16(KC_D);
            } else {
                orbital_mouse_instant_step(1);
            }
                
        } else {
            if (is_test) {
                tap_code16(KC_C);
            } else {
                orbital_mouse_instant_step(1);
            }
        }
    }
    return false;
}

uint8_t get_orbital_angle_from_radians(float rad) {
    float two_pi = 6.283185307179586f;
    float norm = rad / two_pi;
    while (norm < 0.0f) norm += 1.0f;
    while (norm >= 1.0f) norm -= 1.0f;
    return (uint8_t)(norm * 64.0f);
}

void matrix_scan_user(void) {
    int16_t left_dx = (int16_t)analogReadPin(ANALOG_JOYSTICK_X_AXIS_PIN_LEFT) - 516.5;
    int16_t left_dy = (int16_t)analogReadPin(ANALOG_JOYSTICK_Y_AXIS_PIN_LEFT) - 516.5;

    int16_t right_dx = (int16_t)analogReadPin(ANALOG_JOYSTICK_X_AXIS_PIN_RIGHT) - 261;
    int16_t right_dy = (int16_t)analogReadPin(ANALOG_JOYSTICK_Y_AXIS_PIN_RIGHT) - 224;

    int16_t dx = left_dx + right_dx;
    int16_t dy = left_dy + right_dy;

    uprintf("X=%d Y=%d\n", dx, dy);

    const int16_t joyDeadzone = 70;
    float mx = (float)dx;
    float my = (float)dy;

    if (abs(dx) < joyDeadzone && abs(dy) < joyDeadzone) return;

    // uprintf("dx=%d dy=%d   mx=%f my=%f\n", dx, dy, mx, my);

    // move_by(mx, my);

    float angle_left = atan2f(my, mx);
    float orbital_angle = angle_left + 1.57079632679f;
    set_orbital_mouse_angle(get_orbital_angle_from_radians(orbital_angle));
}
