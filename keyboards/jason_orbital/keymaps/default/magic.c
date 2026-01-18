#include QMK_KEYBOARD_H
#include "shared.h"
#include "magic.h"

// An enhanced version of SEND_STRING: if Caps Word is active, the Shift key is
// held while sending the string. Additionally, the last key is set such that if
// the Repeat Key is pressed next, it produces `repeat_keycode`.
#define MAGIC_STRING(str, repeat_keycode) \
        magic_send_string_P(PSTR(str), (repeat_keycode))

static void magic_send_string_P(const char* str, uint16_t repeat_keycode) {
    // uint8_t saved_mods = 0;
 
//   if (is_caps_word_on()) { // If Caps Word is on, save the mods and hold Shift.
//     saved_mods = get_mods();
//     register_mods(MOD_BIT(KC_LSFT));
//   }
 
  send_string_with_delay_P(str, TAP_CODE_DELAY);  // Send the string.
  // set_last_keycode(repeat_keycode); // 2024-03-09 Disabled sending of string for mag-rep / rep-mag consistency.
 
  // If Caps Word is on, restore the mods.
//   if (is_caps_word_on()) {
//     set_mods(saved_mods);
//   }
}

static uint16_t last_keycode = KC_NO;
static uint8_t last_mods = 0;

bool should_remember_last_key_user(uint16_t keycode) {
    switch (keycode) {
        case CW_TOGG:
        case KC_ESC:
        case KC_BSPC:
        case KC_DEL:
 
        case LMAGIC:
        case RMAGIC:
            return false;  // Magic keys will ignore the above keycodes.
    }
    return true;  // Other keys can be repeated.
}

bool process_record_user_for_magic(uint16_t keycode, keyrecord_t* record) {
  if (should_remember_last_key_user(keycode)) {
      last_keycode = keycode;
      last_mods = get_mods();
  }
  switch (keycode) {
      case LMAGIC:
          process_left_magic(last_keycode, last_mods);
          // set_last_keycode(KC_SPC);
          return false;
      case RMAGIC:
          process_right_magic(last_keycode, last_mods);
          // set_last_keycode(KC_SPC);
          return false;
  }
  return true;
}

void process_left_magic(uint16_t keycode, uint8_t mods) { // LMAGIC definitions
    switch (keycode) {
        case  KC_B: { MAGIC_STRING("ecause",    KC_NO); } break;
        case  KC_C: { MAGIC_STRING("an",        KC_NO); } break;
      //case  KC_F: { MAGIC_STRING("f",         KC_NO); } break;
        case  KC_G: { MAGIC_STRING("eneral",    KC_NO); } break;
        case  KC_J: { MAGIC_STRING("ust",       KC_NO); } break;
      //case  KC_K: { MAGIC_STRING("k",         KC_NO); } break;
        case  KC_M: { MAGIC_STRING("ent",       KC_NO); } break;
        case  KC_N: { MAGIC_STRING("ion",       KC_NO); } break;
        case  KC_O: { MAGIC_STRING("o",         KC_NO); } break;
        case  KC_P: { MAGIC_STRING("retty",     KC_NO); } break;
        case  KC_Q: { MAGIC_STRING("q",         KC_NO); } break;
        case  KC_T: { MAGIC_STRING("t",         KC_NO); } break;
        case  KC_U: { MAGIC_STRING("u",         KC_NO); } break;
        case  KC_W: { MAGIC_STRING("hich",      KC_NO); } break;
        case  KC_Y: { MAGIC_STRING("y",         KC_NO); } break;
        case  KC_Z: { MAGIC_STRING("z",         KC_NO); } break;
 
        case KC_COMM: { MAGIC_STRING(" and",    KC_NO); } break;
    }
}
 
void process_right_magic(uint16_t keycode, uint8_t mods) { // RMAGIC definitions
    switch (keycode) {
        case  KC_B: { MAGIC_STRING("b",         KC_NO); } break;
        case  KC_C: { MAGIC_STRING("c",         KC_NO); } break;
        case  KC_F: { MAGIC_STRING("f",         KC_NO); } break;
        case  KC_G: { MAGIC_STRING("g",         KC_NO); } break;
        case  KC_J: { MAGIC_STRING("j",         KC_NO); } break;
        case  KC_K: { MAGIC_STRING("k",         KC_NO); } break;
        case  KC_M: { MAGIC_STRING("m",         KC_NO); } break;
        case  KC_N: { MAGIC_STRING("n",         KC_NO); } break;
      //case  KC_O: { MAGIC_STRING("o",         KC_NO); } break;
        case  KC_P: { MAGIC_STRING("p",         KC_NO); } break;
        case  KC_Q: { MAGIC_STRING("ueen",      KC_NO); } break;
      //case  KC_T: { MAGIC_STRING("t",         KC_NO); } break;
      //case  KC_U: { MAGIC_STRING("u",         KC_NO); } break;
        case  KC_W: { MAGIC_STRING("w",         KC_NO); } break;
        case  KC_Y: { MAGIC_STRING("eah",       KC_NO); } break;
      //case  KC_Z: { MAGIC_STRING("z",         KC_NO); } break;
 
        case KC_COMM: { MAGIC_STRING(" but",    KC_NO); } break;
        case  KC_SPC: { MAGIC_STRING("the",     KC_NO); } break;
        case KC_ASTR: { MAGIC_STRING("*N**or**dr**as**s**i**l**", KC_NO); } break;
    }
}