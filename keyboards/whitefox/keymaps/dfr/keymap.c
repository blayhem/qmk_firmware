/*
Copyright 2015 Jun Wako <wakojun@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include QMK_KEYBOARD_H

#include "my_keymap.h"

//Tap Dance Declarations
enum {
  TD_LSFT_CAPS = 0,
  TD_RSFT_CAPS,
  TD_MULTIFN,
};
 

typedef struct {
  bool is_press_action;
  int state;
} tap;

enum {
  SINGLE_TAP = 1,
  SINGLE_HOLD = 2,
  DOUBLE_TAP = 3,
  DOUBLE_HOLD = 4,
  DOUBLE_SINGLE_TAP = 5, //send two single taps
  TRIPLE_TAP = 6,
  TRIPLE_HOLD = 7
};

int cur_dance (qk_tap_dance_state_t *state) {
  if (state->count == 1) {
    if (state->interrupted || !state->pressed)  return SINGLE_TAP;
    //key has not been interrupted, but they key is still held. Means you want to send a 'HOLD'.
    else return SINGLE_HOLD;
  }
  else if (state->count == 2) {
    /*
     * DOUBLE_SINGLE_TAP is to distinguish between typing "pepper", and actually wanting a double tap
     * action when hitting 'pp'. Suggested use case for this return value is when you want to send two
     * keystrokes of the key, and not the 'double tap' action/macro.
    */
    if (state->interrupted) return DOUBLE_SINGLE_TAP;
    else if (state->pressed) return DOUBLE_HOLD;
    else return DOUBLE_TAP;
  }
  //Assumes no one is trying to type the same letter three times (at least not quickly).
  //If your tap dance key is 'KC_W', and you want to type "www." quickly - then you will need to add
  //an exception here to return a 'TRIPLE_SINGLE_TAP', and define that enum just like 'DOUBLE_SINGLE_TAP'
  if (state->count == 3) {
    if (state->interrupted || !state->pressed)  return TRIPLE_TAP;
    else return TRIPLE_HOLD;
  }
  else return 8; //magic number. At some point this method will expand to work for more presses
}

//instanciate an instance of 'tap' for the 'multifn' tap dance.
static tap multifn_tap_state = {
  .is_press_action = true,
  .state = 0
};

void multifn_finished (qk_tap_dance_state_t *state, void *user_data) {
  multifn_tap_state.state = cur_dance(state);
  switch (multifn_tap_state.state) {
    case SINGLE_TAP: qk_leader_start(); break;
    case SINGLE_HOLD: layer_on(1); break;
    case DOUBLE_TAP: layer_invert(1); break;
    case DOUBLE_HOLD: layer_on(2); break;
  }
}

void multifn_reset (qk_tap_dance_state_t *state, void *user_data) {
  switch (multifn_tap_state.state) {
    case SINGLE_HOLD: layer_off(1); break;
    case DOUBLE_HOLD: layer_off(2);
  }
  multifn_tap_state.state = 0;
}

//Tap Dance Definitions
qk_tap_dance_action_t tap_dance_actions[] = {
  //Tap once for Shift, twice for Caps Lock
  [TD_LSFT_CAPS]  = ACTION_TAP_DANCE_DOUBLE(KC_LSFT, KC_CAPS),
  [TD_RSFT_CAPS]  = ACTION_TAP_DANCE_DOUBLE(KC_RSFT, KC_CAPS),
  [TD_MULTIFN]    = ACTION_TAP_DANCE_FN_ADVANCED(NULL, multifn_finished, multifn_reset),
};

//Keycode declarations
enum my_keycodes {
  MC_6 = SAFE_RANGE,  // ñ
  MC_QUOT,
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  static bool is_shift_modded = false;
  switch (keycode) {
    case MC_6:
      RALT_IF_SHIFT_MODDED(KC_6)
      return false;
    case MC_QUOT:
      RALT_IF_SHIFT_MODDED(KC_QUOT)
      return false;
    default:
      return true; // Process all other keycodes normally
  }
}



// Leader key
LEADER_EXTERNS();

void matrix_scan_user(void) {
  LEADER_DICTIONARY() {
    leading = false;
    leader_end();

    SEQ_ONE_KEY(KC_F) {
      // Anything you can do in a macro.
      SEND_STRING("QMK is awesome.");
    }
    SEQ_TWO_KEYS(KC_D, KC_D) {
      SEND_STRING(SS_LCTRL("a")SS_LCTRL("c"));
    }
    SEQ_THREE_KEYS(KC_D, KC_D, KC_S) {
      SEND_STRING("https://start.duckduckgo.com"SS_TAP(X_ENTER));
    }
    SEQ_TWO_KEYS(KC_J, KC_J) {
      register_code(KC_LCTL);
      register_code(KC_V);
      unregister_code(KC_V);
      unregister_code(KC_LCTL);
    }
  }
}

const uint16_t keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /* Layer 0: Default Layer
     * ,---------------------------------------------------------------.
     * |  `|  1|  2|  3|  4|  5|  6|  7|  8|  9|  0|  -|  =|  \|  `|Ins|
     * |---------------------------------------------------------------|
     * |Tab  |  Q|  W|  E|  R|  T|  Y|  U|  I|  O|  P|  [|  ]|Backs|Del|
     * |---------------------------------------------------------------|
     * |Esc/Ct|  A|  S|  D|  F|  G|  H|  J|  K|  L|  ;|  '|Enter   |PgU|
     * |---------------------------------------------------------------|
     * |Shift   |  Z|  X|  C|  V|  B|  N|  M|  ,|  .|  /|Shift |Up |PgD|
     * |---------------------------------------------------------------|
     * |L2  |Alt |Gui |     Space/L1     |Ctrl|Alt |Gui |  |Lef|Dow|Rig|
     * `---------------------------------------------------------------'
     * NOTE: The use of RALT with KC_6, KC_GRV and KC_QUOT allows to type special symbols
     *       without having to press space after.
     */
    [0] = LAYOUT(                                                                                                                                                                               \
        KC_GRV,      KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     MC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,             KC_BSLS,  RALT(KC_GRV),  TG(2),    \
        KC_TAB,            KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,  KC_RBRC,            KC_BSPC,                 KC_DEL,   \
        LCTL_T(KC_ESC),    KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,  KC_NUHS,            KC_ENT,                  KC_PGUP,  \
        TD(TD_LSFT_CAPS),  KC_NUBS,  KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,  TD(TD_RSFT_CAPS),             KC_UP,         KC_PGDN,  \
        MO(2),             KC_LALT,  KC_LGUI,                            LT(1,KC_SPC),                               KC_RCTL,  KC_RALT,  KC_RGUI,            KC_LEFT,  KC_DOWN,       KC_RGHT   \
    ),
    /* Layer 1: Frequent functions
     * ,---------------------------------------------------------------.
     * |   | F1| F2| F3| F4| F5| F6| F7| F8| F9|F10|F11|F12|   |   |   |
     * |---------------------------------------------------------------|
     * |     |   |   |   |   |   |Cop|Ü  |   |   |Pas|   |   |     |   |
     * |---------------------------------------------------------------|
     * |      |   |   |   |   |   |Lef|Dow|Up |Rig|   |   |        |   |
     * |---------------------------------------------------------------|
     * |        |   |   |Ç  |   |   |Ñ  |   |   |   |¿  |      |PgU|   |
     * |---------------------------------------------------------------|
     * |    |    |    |                  |    |    |    |  |Hom|PgD|End|
     * `---------------------------------------------------------------'
     */
    [1] = LAYOUT( \
        _______,  KC_F1,    KC_F2,           KC_F3,       KC_F4,          KC_F5,    KC_F6,            KC_F7,       KC_F8,    KC_F9,    KC_F10,           KC_F11,         KC_F12,   _______,  _______,  _______,  \
        _______,  _______,  LCTL(KC_RIGHT),  RALT(KC_5),  _______,        _______,  LCTL(KC_INSERT),  RALT(KC_Y),  _______,  _______,  LSFT(KC_INSERT),  _______,        _______,  _______,            _______,  \
        _______,  KC_HOME,  KC_PGUP,         KC_PGDN,     KC_END,         _______,  KC_LEFT,          KC_DOWN,     KC_UP,    KC_RGHT,  _______,          _______,        _______,  _______,            _______,  \
        _______,  _______,  _______,         KC_DEL,      RALT(KC_COMM),  _______,  LCTL(KC_LEFT),    RALT(KC_N),  _______,  _______,  _______,          RALT(KC_SLSH),  _______,            KC_PGUP,  _______,  \
        _______,  _______,  _______,                                                _______,                                           _______,          _______,        _______,  KC_HOME,  KC_PGDN,  KC_END    \
    ),
    /* Layer 2: Mouse emulation and other functions
     * ,---------------------------------------------------------------.
     * |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
     * |---------------------------------------------------------------|
     * |     |   |   |   |   |   |   |   |   |   |   |   |   |     |MwU|
     * |---------------------------------------------------------------|
     * |      |   |   |   |   |   |   |   |   |   |   |   |        |MwD|
     * |---------------------------------------------------------------|
     * |        |   |   |   |   |   |   |   |   |   |   |Bt1   |MsU|Bt2|
     * |---------------------------------------------------------------|
     * |    |    |    |      Space       |    |    |    |  |MsL|MsD|MsR|
     * `---------------------------------------------------------------'
     */
    [2] = LAYOUT( \
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______, _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  \
        _______,  _______,  KC_UP,    _______,  _______,  _______,  _______,  KC_MEDIA_REWIND, KC_MPLY,  KC_MEDIA_FAST_FORWARD,  _______,  _______,  _______,  _______,            KC_WH_U,  \
        _______,  KC_LEFT,  KC_DOWN,  KC_RGHT,  _______,  _______,  _______,  KC__VOLDOWN, KC__VOLUP,  KC__MUTE,  _______,  _______,  _______,  _______,            KC_WH_D,  \
        _______,  _______,  KC_WH_U,  KC_WH_D,  KC_BTN2,  KC_BTN1,  _______,  _______, _______,  _______,  _______,  _______,  KC_BTN1,            KC_MS_U,  KC_BTN2,  \
        _______,  _______,  _______,                                KC_SPC,                                _______,  _______,  _______,  KC_MS_L,  KC_MS_D,  KC_MS_R   \
    ),
};
