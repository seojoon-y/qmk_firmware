// https://pastebin.com/j0pfKzBR

#include QMK_KEYBOARD_H
#include "features/orbital_mouse.h"
#include "digitizer.h"
#include "analog.h"
#include "magic.h"
#include "shared.h"
#include <math.h>


#define ANALOG_JOYSTICK_X_AXIS_PIN_LEFT GP26
#define ANALOG_JOYSTICK_Y_AXIS_PIN_LEFT GP27

#define ANALOG_JOYSTICK_X_AXIS_PIN_RIGHT GP28
#define ANALOG_JOYSTICK_Y_AXIS_PIN_RIGHT GP29

enum layer_names {
    _BASE,
    _LEFT,
    _RIGHT
};

// static void teleport_corner(uint8_t c) {
//     digitizer_in_range_on();
//     switch (c) {
//         case 0:
//             digitizer_set_position(0.0f, 0.0f);
//             break;
//         case 1:
//             digitizer_set_position(1.0f, 0.0f);
//             break;
//         case 2:
//             digitizer_set_position(0.0f, 1.0f);
//             break;
//         default:
//         case 3:
//             digitizer_set_position(1.0f, 1.0f);
//             break;
//     }
//     digitizer_in_range_off();
// }

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_BASE] = LAYOUT(
        LMAGIC,     KC_L,       KC_D,       KC_C,       KC_1,       KC_ENT,
        KC_B,       KC_R,       KC_T,       KC_S,       KC_V,       KC_SPC,
        KC_N,       KC_Q,       KC_M,       KC_W,       KC_G,       TO(_LEFT),

        KC_TAB,     KC_1,       KC_F,       KC_O,       KC_U,       RMAGIC,
        KC_BSPC,    KC_J,       KC_H,       KC_A,       KC_E,       KC_X,
        TO(_RIGHT), KC_Y,       KC_P,       KC_K,       KC_Z,       KC_I
    ),
    [_LEFT] = LAYOUT(
        LMAGIC,     KC_SCLN,    _______,    _______,    KC_1,       KC_ENT,
        _______,    KC_COMM,    KC_DOT,     KC_QUOT,    KC_DQUO,    KC_SPC,
        KC_ESC,     S(KC_GRAVE),KC_QUES,    _______,    _______,    TO(_RIGHT),
        
        KC_TAB,     KC_1,       _______,    KC_AMPR,    KC_DLR,     RMAGIC,
        KC_BSPC,    _______,    KC_LBRC,    KC_RBRC,    KC_LPRN,    KC_HASH,
        TO(_BASE),  _______,    KC_COLN,    KC_BSLS,    KC_PERC,    KC_RPRN
    ),
    [_RIGHT] = LAYOUT(
        LMAGIC,     KC_LT,      KC_GT,      KC_PIPE,    KC_1,       KC_ENT,
        KC_CIRC,    KC_EXLM,    KC_SLSH,    KC_EQL,     _______,    KC_SPC,
        KC_PLUS,    KC_GRV,     KC_ASTR,    KC_AT,      _______,    TO(_BASE),

        KC_TAB,     KC_1,       KC_7,       KC_8,       KC_9,       RMAGIC,
        KC_BSPC,    _______,    KC_0,       KC_1,       KC_2,       _______,
        TO(_LEFT),  _______,    KC_4,       KC_5,       KC_6,       KC_3
    )
};

// CNR_TL, OM_U, CNR_TR,
// CNR_BL, OM_D, CNR_BR,

// KC_D, OM_L, KC_D,
// KC_B, OM_R, KC_B

static inline bool is_orbital_path(uint16_t keycode) {
    switch (keycode) {
        case CNR_TL:
        case CNR_TR:
        case CNR_BL:
        case CNR_BR:
            return true;
        default:
            return false;
    }
}

bool process_record_user(uint16_t keycode, keyrecord_t* record) {
    // if (is_orbital_path(keycode)) {
    //     if (!process_orbital_mouse(keycode, record)) {
    //         return false;
    //     }

    //     if (!record->event.pressed) {
    //         return true;
    //     }

    //     switch (keycode) {
    //         case CNR_TL: teleport_corner(0); return false;
    //         case CNR_TR: teleport_corner(1); return false;
    //         case CNR_BL: teleport_corner(2); return false;
    //         case CNR_BR: teleport_corner(3); return false;
    //     }

    //     return true;
    // }

    // if (!process_custom_shift_keys(keycode, record)) { return false; }

    if (record->event.pressed) {
        const bool should_continue_normal_execution = process_record_user_for_magic(keycode, record);
        if (!should_continue_normal_execution) return false;
    }

    return true;
}


// Key overrides
const custom_shift_key_t custom_shift_keys[] = {
  {KC_COMM, KC_QUES},
  {KC_DOT,  KC_EXLM},
  {KC_SLSH, KC_BSLS},
  {KC_BSPC, KC_DEL},
 
  {KC_1, KC_1}, // disable shifting
  {KC_2, KC_2},
  {KC_3, KC_3},
  {KC_4, KC_4},
  {KC_5, KC_5},
  {KC_6, KC_6},
  {KC_7, KC_7},
  {KC_8, KC_8},
  {KC_9, KC_9},
  {KC_0, KC_0},
  {KC_EQL, KC_EQL},
  {KC_GRV, KC_GRV},
  {KC_LBRC, KC_LBRC},
  {KC_RBRC, KC_RBRC},
 
};

uint8_t NUM_CUSTOM_SHIFT_KEYS =
    sizeof(custom_shift_keys) / sizeof(custom_shift_key_t);

void housekeeping_task_user(void) {
    orbital_mouse_task();
}

bool encoder_update_user(uint8_t index, bool clockwise) {
    // orbital_mouse_instant_step(1);
    if (index == 0) {
        if (clockwise) {
            tap_code16(KC_DOWN);
        } else {
            tap_code16(KC_UP);
        }
    } else {
        if (clockwise) {
            tap_code16(KC_LEFT);
        } else {
            tap_code16(KC_RIGHT);
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

    // Don't log during prod, it will destroy performance
    // uprintf("X=%d Y=%d\n", dx, dy);

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
