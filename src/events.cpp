#include "dana/events.h"

#include <SDL.h>

namespace dana {

KeyboardEvent::KeyboardEvent(const ScanCodes _code, const KeyCodes _key,
                             const KeyboardStates _state,
                             const uint16_t _modifiers, const bool _repeat)
    : code(_code),
      key(_key),
      state(_state),
      modifiers(_modifiers),
      repeat(_repeat) {}

bool KeyboardEvent::hasModifier(const KeyboardModifiers modifier) const
    noexcept {
  uint16_t modifier_bit;

  switch (modifier) {
    case KeyboardModifiers::SHIFT:
      modifier_bit = KMOD_SHIFT;
      break;
    case KeyboardModifiers::LEFT_SHIFT:
      modifier_bit = KMOD_LSHIFT;
      break;
    case KeyboardModifiers::RIGHT_SHIFT:
      modifier_bit = KMOD_RSHIFT;
      break;
    case KeyboardModifiers::CTRL:
      modifier_bit = KMOD_CTRL;
      break;
    case KeyboardModifiers::LEFT_CTRL:
      modifier_bit = KMOD_LCTRL;
      break;
    case KeyboardModifiers::RIGHT_CTRL:
      modifier_bit = KMOD_RCTRL;
      break;
    case KeyboardModifiers::ALT:
      modifier_bit = KMOD_ALT;
      break;
    case KeyboardModifiers::LEFT_ALT:
      modifier_bit = KMOD_LALT;
      break;
    case KeyboardModifiers::RIGHT_ALT:
      modifier_bit = KMOD_RALT;
      break;
    case KeyboardModifiers::GUI:
      modifier_bit = KMOD_GUI;
      break;
    case KeyboardModifiers::LEFT_GUI:
      modifier_bit = KMOD_LGUI;
      break;
    case KeyboardModifiers::RIGHT_GUI:
      modifier_bit = KMOD_RGUI;
      break;
    case KeyboardModifiers::NUM_LOCK:
      modifier_bit = KMOD_NUM;
      break;
    case KeyboardModifiers::CAPS_LOCK:
      modifier_bit = KMOD_CAPS;
      break;
    case KeyboardModifiers::ALT_GR:
      modifier_bit = KMOD_MODE;
      break;
    case KeyboardModifiers::NONE:
      modifier_bit = 0;
      break;
  }
  return (modifiers & modifier_bit) != 0;
}
}  // namespace dana
