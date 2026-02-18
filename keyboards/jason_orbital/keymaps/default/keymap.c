// https://pastebin.com/j0pfKzBR

#include QMK_KEYBOARD_H
#include "features/orbital_mouse.h"
#include "digitizer.h"
#include "analog.h"
#include "magic.h"
#include "shared.h"
#include "host.h"
#include <math.h>
#include "transactions.h"
#include "split_util.h"
#include "timer.h"
#include "gpio.h"

#define HAPTIC_IN1 GP8
#define HAPTIC_IN2 GP9
typedef enum { HAPTIC_IDLE, HAPTIC_FORWARD, HAPTIC_REVERSE } haptic_state_t;
static haptic_state_t haptic_state = HAPTIC_IDLE;
static uint32_t haptic_timer = 0;

typedef struct {
    int16_t dx;
    int16_t dy;
} joy_s2m_t;

#define ANALOG_JOYSTICK_X_AXIS_PIN_LEFT GP28
#define ANALOG_JOYSTICK_Y_AXIS_PIN_LEFT GP29

#define ANALOG_JOYSTICK_X_AXIS_PIN_RIGHT GP27
#define ANALOG_JOYSTICK_Y_AXIS_PIN_RIGHT GP26

enum layer_names {
    _APOLLO,
    _HELIOS,
    _HESTIA,
    _L_MOD,
    _R_MOD,

};


void right_joy_slave_handler(uint8_t in_len, const void *in_data, uint8_t out_len, void *out_data) {
    joy_s2m_t *o = (joy_s2m_t *)out_data;
    o->dx = (int16_t)analogReadPin(ANALOG_JOYSTICK_X_AXIS_PIN_RIGHT) - 498;
    o->dy = (int16_t)analogReadPin(ANALOG_JOYSTICK_Y_AXIS_PIN_RIGHT) - 506;
}

void keyboard_post_init_user(void) {
    transaction_register_rpc(RIGHT_JOY, right_joy_slave_handler);
}

static joy_s2m_t right_joy_cached;


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
    [_APOLLO] = LAYOUT(
        LMAGIC,         KC_L,           KC_D,           KC_C,           TO(_HELIOS),    KC_ENT,
        KC_B,           KC_R,           KC_T,           KC_S,           KC_V,           MO(_L_MOD),
        HAPTIC_TAP,           KC_Q,           KC_M,           KC_W,           KC_G,           KC_SPC,

        KC_TAB,         TO(_HESTIA),    KC_F,           KC_O,           KC_U,           RMAGIC,
        MO(_R_MOD),        KC_J,           KC_H,           KC_A,           KC_E,           KC_X,
        KC_BSPC,     KC_Y,           KC_P,           KC_K,           KC_Z,           KC_I
    ),
    [_HELIOS] = LAYOUT(
        LMAGIC,         KC_SCLN,        _______,        _______,        TO(_HESTIA),    KC_ENT,
        _______,        KC_COMM,        KC_DOT,         KC_QUOT,        KC_DQUO,        MO(_L_MOD),
        KC_ESC,         S(KC_GRAVE),    KC_QUES,        _______,        _______,        KC_SPC,

        KC_TAB,         TO(_APOLLO),    _______,        KC_AMPR,        KC_DLR,         RMAGIC,
        MO(_R_MOD),        _______,        KC_LBRC,        KC_RBRC,        KC_LPRN,        KC_HASH,
        KC_BSPC,     _______,        KC_COLN,        KC_BSLS,        KC_PERC,        KC_RPRN
    ),
    [_HESTIA] = LAYOUT(
        LMAGIC,         KC_LT,          KC_GT,          KC_PIPE,        TO(_APOLLO),    KC_ENT,
        KC_CIRC,        KC_EXLM,        KC_SLSH,        KC_EQL,         MS_BTN1,        MO(_L_MOD),
        KC_PLUS,        KC_GRV,         KC_ASTR,        KC_AT,          _______,        KC_SPC,

        KC_TAB,         TO(_HELIOS),    KC_7,           KC_8,           KC_9,           RMAGIC,
        MO(_R_MOD),        MS_BTN2,        KC_0,           KC_1,           KC_2,           _______,
        KC_BSPC,     _______,        KC_4,           KC_5,           KC_6,           KC_3
    ),
    [_L_MOD] = LAYOUT(
        _______,        _______,        _______,        _______,        _______,        _______,
        _______,        KC_LALT,        KC_LGUI,        KC_LSFT,        _______,        _______,
        KC_LCTL,        _______,        _______,        _______,        _______,        _______,

        _______,        _______,        _______,        _______,        _______,        _______,
        _______,        _______,        _______,        _______,        _______,        _______,
        _______,        _______,        _______,        _______,        _______,        _______
    ),
    [_R_MOD] = LAYOUT(
        _______,        _______,        _______,        _______,        _______,        _______,
        _______,        _______,        _______,        _______,        _______,        _______,
        _______,        _______,        _______,        _______,        _______,        _______,

        _______,        _______,        _______,        _______,        _______,        _______,
        _______,        _______,        KC_LSFT,        KC_LGUI,        KC_LALT,        _______,
        _______,        _______,        _______,        _______,        _______,        KC_LCTL
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

    switch (keycode) {
        case HAPTIC_TAP:
            if (record->event.pressed) {
                setPinOutput(HAPTIC_IN1);
                setPinOutput(HAPTIC_IN2);
                writePinHigh(HAPTIC_IN1);
                writePinLow(HAPTIC_IN2);
                haptic_state = HAPTIC_FORWARD;
                haptic_timer = timer_read32() + 3;
            }
            return false;
    }

    if (record->event.pressed) {
        const bool should_continue_normal_execution = process_record_user_for_magic(keycode, record);
        if (!should_continue_normal_execution) return false;
    }

    return true;
}


void housekeeping_task_user(void) {
    // orbital_mouse_task();
    if (haptic_state != HAPTIC_IDLE && timer_read32() >= haptic_timer) {
        if (haptic_state == HAPTIC_FORWARD) {
            writePinLow(HAPTIC_IN1);
            writePinHigh(HAPTIC_IN2);
            haptic_state = HAPTIC_REVERSE;
            haptic_timer = timer_read32() + 3;
        } else {
            setPinInput(HAPTIC_IN1);
            setPinInput(HAPTIC_IN2);
            haptic_state = HAPTIC_IDLE;
        }
    }

    if (!is_keyboard_master()) return;

    static uint32_t last = 0;
    if (timer_elapsed32(last) < 5) return;
    last = timer_read32();

    joy_s2m_t tmp;
    if (transaction_rpc_exec(RIGHT_JOY, 0, 0, sizeof(tmp), &tmp)) {
        right_joy_cached = tmp;
    }
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
    const int16_t left_dx = (int16_t)analogReadPin(ANALOG_JOYSTICK_X_AXIS_PIN_LEFT) - 520;
    const int16_t left_dy = (int16_t)analogReadPin(ANALOG_JOYSTICK_Y_AXIS_PIN_LEFT) - 505;
    const int16_t right_dx = right_joy_cached.dx;
    const int16_t right_dy = right_joy_cached.dy;

    const float_t speed_multiplier_left = 0.03;
    const float_t speed_multiplier_right = 0.01;
    const float_t deadzone = 30;

    float_t dx = 0;
    float_t dy = 0;

    if (abs(left_dx) > deadzone) dx -= left_dx * speed_multiplier_left;
    if (abs(left_dy) > deadzone) dy -= left_dy * speed_multiplier_left * (-1);
    if (abs(right_dx) > deadzone) dx += right_dx * speed_multiplier_right * (-1);
    if (abs(right_dy) > deadzone) dy += right_dy * speed_multiplier_right;

    if ((dx != 0) || (dy != 0)) {
        report_mouse_t r = mousekey_get_report();
        r.x = dx;
        r.y = dy;
        host_mouse_send(&r);
    }

    // // Don't log during prod, it will destroy performance
    uprintf("hand=%d , X=%d Y=%d X=%d Y=%d\n", is_keyboard_left() ? 0 : 1, left_dx, left_dy, right_dx, right_dy);

    // if (is_keyboard_left()) {
    //     uprintf("left\n");
    // } else {
    //     uprintf("right\n");
    // }
    // int16_t dx = left_dx + right_dx;
    // int16_t dy = left_dy + right_dy;

    // const int16_t joyDeadzone = 70;
    // float mx = (float)dx;
    // float my = (float)dy;

    // if (abs(dx) < joyDeadzone && abs(dy) < joyDeadzone) return;

    // uprintf("dx=%d dy=%d   mx=%f my=%f\n", dx, dy, mx, my);

    // move_by(mx, my);

    // float angle_left = atan2f(my, mx);
    // float orbital_angle = angle_left + 1.57079632679f;
    // set_orbital_mouse_angle(get_orbital_angle_from_radians(orbital_angle));
}
