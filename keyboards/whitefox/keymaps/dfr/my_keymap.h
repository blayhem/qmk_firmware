#pragma once

#define RALT_IF_SHIFT_MODDED(kc)                                 \
  if (record->event.pressed) {                                   \
    if ( (get_mods() | get_oneshot_mods()) & MOD_MASK_SHIFT ) {  \
      is_shift_modded = true;                                    \
      register_code(KC_RALT);                                    \
    }                                                            \
    register_code(kc);                                           \
  } else {                                                       \
    if (is_shift_modded) {                                       \
      is_shift_modded = false;                                   \
      unregister_code(KC_RALT);                                  \
    }                                                            \
    unregister_code(kc);                                         \
  }
