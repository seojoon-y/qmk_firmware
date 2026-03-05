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

#define HAPTIC_IN1 GP24
#define HAPTIC_IN2 GP25
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
    _BASE,
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

static bool is_mouse_mode = true;
static uint16_t joy_keycode = KC_NO;
static uint32_t joy_cooldown_timer = 0;
#define JOY_DIR_COOLDOWN_MS 100

typedef struct {
    float    start_angle;
    float    end_angle;
    uint16_t keycode;
} joy_sector_t;

static const joy_sector_t joy_sectors[] = {
    { (float)(M_PI / -4),      (float)(M_PI / 4),       KC_N },
    { (float)(M_PI / 4),       (float)(3 * M_PI / 4),   KC_W },
    { (float)(3 * M_PI / 4),   (float)(5 * M_PI / 4),   KC_S },
    { (float)(5 * M_PI / 4),   (float)(7 * M_PI / 4),   KC_E },
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
        // Left half: bottom row first, top row last
        KC_Z,    KC_X,    KC_C,    KC_V,
        KC_A,    KC_S,    KC_D,    KC_F,
        KC_Q,    KC_W,    KC_E,    KC_R,
        KC_1,    KC_2,    KC_3,    KC_4,
        // Right half: unused
        KC_Z,    KC_X,    _______,    KC_V,
        KC_A,    KC_S,    KC_D,    KC_F,
        KC_Q,    KC_W,    KC_E,    KC_R,
        KC_1,    KC_2,    KC_3,    KC_4
    ),
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
        case MOUSE_TOGGLE_TAP:
            if (record->event.pressed) {
                is_mouse_mode = !is_mouse_mode;
                // Release any held direction key when switching modes
                if (joy_keycode != KC_NO) {
                    unregister_code(joy_keycode);
                    joy_keycode = KC_NO;
                }
                if (haptic_state == HAPTIC_IDLE) {
                    setPinOutput(HAPTIC_IN1);
                    setPinOutput(HAPTIC_IN2);
                    writePinHigh(HAPTIC_IN1);
                    writePinLow(HAPTIC_IN2);
                    haptic_state = HAPTIC_FORWARD;
                    haptic_timer = timer_read32() + 3;
                }
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
            haptic_timer = timer_read32() + 1;
        } else {
            writePinLow(HAPTIC_IN1);
            writePinLow(HAPTIC_IN2);
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


uint8_t get_orbital_angle_from_radians(float rad) {
    float two_pi = 6.283185307179586f;
    float norm = rad / two_pi;
    while (norm < 0.0f) norm += 1.0f;
    while (norm >= 1.0f) norm -= 1.0f;
    return (uint8_t)(norm * 64.0f);
}

void matrix_scan_user(void) {
    return;
    const int16_t left_dx = (int16_t)analogReadPin(ANALOG_JOYSTICK_X_AXIS_PIN_LEFT) - 500;
    const int16_t left_dy = (int16_t)analogReadPin(ANALOG_JOYSTICK_Y_AXIS_PIN_LEFT) - 520;
    const int16_t right_dx = right_joy_cached.dx;
    const int16_t right_dy = right_joy_cached.dy;

    const float_t speed_multiplier_left = 0.03;
    const float_t speed_multiplier_right = 0.01;
    const float_t mouse_deadzone = 30;
    const float_t dir_deadzone = 60;

    float_t dx = 0;
    float_t dy = 0;

    if (abs(left_dx) > mouse_deadzone) dx -= left_dx * speed_multiplier_left;
    if (abs(left_dy) > mouse_deadzone) dy -= left_dy * speed_multiplier_left * (-1);
    if (abs(right_dx) > mouse_deadzone) dx += right_dx * speed_multiplier_right * (-1);
    if (abs(right_dy) > mouse_deadzone) dy += right_dy * speed_multiplier_right;

    if (is_mouse_mode) {
        if ((dx != 0) || (dy != 0)) {
            report_mouse_t r = mousekey_get_report();
            r.x = dx;
            r.y = dy;
            host_mouse_send(&r);
        }
    } else {
        // Direction mode: map left joystick to 4 directional key presses
        float magnitude = sqrtf((float)left_dx * left_dx + (float)left_dy * left_dy);
        uint16_t new_keycode = KC_NO;
        if (magnitude > dir_deadzone) {
            float angle = atan2f(-(float)left_dy, (float)left_dx);
            float cw_from_north = (float)(M_PI / 2) - angle;
            if (cw_from_north < 0.0f) cw_from_north += (float)(2 * M_PI);
            if (cw_from_north >= (float)(2 * M_PI)) cw_from_north -= (float)(2 * M_PI);
            for (uint8_t i = 0; i < sizeof(joy_sectors) / sizeof(joy_sectors[0]); i++) {
                float start = joy_sectors[i].start_angle;
                float end   = joy_sectors[i].end_angle;
                bool in_sector = start < 0.0f
                    ? (cw_from_north >= (float)(2 * M_PI) + start || cw_from_north < end)
                    : (cw_from_north >= start && cw_from_north < end);
                if (in_sector) {
                    new_keycode = joy_sectors[i].keycode;
                    break;
                }
            }
        }
        if (new_keycode != joy_keycode) {
            bool was_center = (joy_keycode == KC_NO);
            joy_keycode = new_keycode;
            if (new_keycode != KC_NO && was_center && timer_elapsed32(joy_cooldown_timer) >= JOY_DIR_COOLDOWN_MS) {
                joy_cooldown_timer = timer_read32();
                register_code(new_keycode);
                unregister_code(new_keycode);
                if (haptic_state == HAPTIC_IDLE) {
                    setPinOutput(HAPTIC_IN1);
                    setPinOutput(HAPTIC_IN2);
                    writePinHigh(HAPTIC_IN1);
                    writePinLow(HAPTIC_IN2);
                    haptic_state = HAPTIC_FORWARD;
                    haptic_timer = timer_read32() + 2;
                }
            }
        }
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
