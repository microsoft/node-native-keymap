/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Microsoft Corporation. All rights reserved.
 *  Licensed under the MIT License. See License.txt in the project root for license information.
 *--------------------------------------------------------------------------------------------*/

#include "keymapping.h"
#include "string_conversion.h"

#include <X11/XKBlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "../deps/chromium/macros.h"
#include "../deps/chromium/x/keysym_to_unicode.h"
#include "../deps/chromium/x/keycode_and_xkeycode.h"

typedef struct _XDisplay XDisplay;

namespace {

class KeyModifierMaskToXModifierMask {
 public:
  static KeyModifierMaskToXModifierMask& GetInstance() {
    static KeyModifierMaskToXModifierMask instance;
    return instance;
  }

  void Initialize(Display* display) {
    alt_modifier = 0;
    meta_modifier = 0;
    num_lock_modifier = 0;
    mode_switch_modifier = 0;

    if (!display) {
      return;
    }

    XModifierKeymap* mod_map = XGetModifierMapping(display);
    int max_mod_keys = mod_map->max_keypermod;
    for (int mod_index = 0; mod_index <= 8; ++mod_index) {
      for (int key_index = 0; key_index < max_mod_keys; ++key_index) {
        int key = mod_map->modifiermap[mod_index * max_mod_keys + key_index];
        if (!key) {
          continue;
        }

        int keysym = XkbKeycodeToKeysym(display, key, 0, 0);
        if (!keysym) {
          continue;
        }

        // TODO: Also check for XK_ISO_Level3_Shift	0xFE03

        if (keysym == XK_Alt_L || keysym == XK_Alt_R) {
          alt_modifier = 1 << mod_index;
        }
        if (keysym == XK_Mode_switch) {
          mode_switch_modifier = 1 << mod_index;
        }
        if (keysym == XK_Meta_L || keysym == XK_Super_L || keysym == XK_Meta_R || keysym == XK_Super_R) {
          meta_modifier = 1 << mod_index;
        }
        if (keysym == XK_Num_Lock) {
          num_lock_modifier = 1 << mod_index;
        }
      }
    }

    XFreeModifiermap(mod_map);
  }

  int XModFromKeyMod(int keyMod) {
    int x_modifier = 0;

    // Ctrl + Alt => AltGr
    if (keyMod & kControlKeyModifierMask && keyMod & kAltKeyModifierMask) {
      x_modifier |= mode_switch_modifier;//alt_r_modifier;
    } else if (keyMod & kControlKeyModifierMask) {
      x_modifier |= ControlMask;
    } else if (keyMod & kAltKeyModifierMask) {
      x_modifier |= alt_modifier;
    }

    if (keyMod & kShiftKeyModifierMask) {
      x_modifier |= ShiftMask;
    }

    if (keyMod & kMetaKeyModifierMask) {
      x_modifier |= meta_modifier;
    }

    if (keyMod & kNumLockKeyModifierMask) {
      x_modifier |= num_lock_modifier;
    }

    return x_modifier;
  }

 private:
  KeyModifierMaskToXModifierMask() {
    Initialize(NULL);
  }

  int alt_modifier;
  int meta_modifier;
  int num_lock_modifier;
  int mode_switch_modifier;

  DISALLOW_COPY_AND_ASSIGN(KeyModifierMaskToXModifierMask);
};

std::string GetStrFromXEvent(const XEvent* xev) {
  const XKeyEvent* xkey = &xev->xkey;
  KeySym keysym = XK_VoidSymbol;
  XLookupString(const_cast<XKeyEvent*>(xkey), NULL, 0, &keysym, NULL);
  uint16_t character = ui::GetUnicodeCharacterFromXKeySym(keysym);

  if (!character)
    return std::string();

  wchar_t value = character;

  return vscode_keyboard::UTF16toUTF8(&value, 1);
}

} // namespace


namespace vscode_keyboard {

std::vector<KeyMapping> GetKeyMapping() {
  std::vector<KeyMapping> result;

  Display *display;
  if (!(display = XOpenDisplay(""))) {
    return result;
  }

  XEvent event;
  memset(&event, 0, sizeof(XEvent));
  XKeyEvent* key_event = &event.xkey;
  key_event->display = display;
  key_event->type = KeyPress;

  KeyModifierMaskToXModifierMask *mask_provider = &KeyModifierMaskToXModifierMask::GetInstance();
  mask_provider->Initialize(display);

  for (size_t i = 0; i < arraysize(ui::gKeyCodeToXKeyCode); ++i) {
    ui::KeyboardCode key_code = ui::gKeyCodeToXKeyCode[i].key_code;
    int x_key_code = ui::gKeyCodeToXKeyCode[i].x_key_code;

    key_event->keycode = x_key_code;

    key_event->state = 0;
    std::string value = GetStrFromXEvent(&event);

    key_event->state = mask_provider->XModFromKeyMod(kShiftKeyModifierMask);
    std::string withShift = GetStrFromXEvent(&event);

    key_event->state = mask_provider->XModFromKeyMod(kControlKeyModifierMask | kAltKeyModifierMask);
    std::string withAltGr = GetStrFromXEvent(&event);

    key_event->state = mask_provider->XModFromKeyMod(kShiftKeyModifierMask | kControlKeyModifierMask | kAltKeyModifierMask);
    std::string withShiftAltGr = GetStrFromXEvent(&event);

    KeyMapping keyMapping = KeyMapping();
    keyMapping.key_code = key_code;
    keyMapping.value = value;
    keyMapping.withShift = withShift;
    keyMapping.withAltGr = withAltGr;
    keyMapping.withShiftAltGr = withShiftAltGr;
    result.push_back(keyMapping);
  }

  XFlush(display);
  XCloseDisplay(display);

  return result;
}

} // namespace vscode_keyboard
