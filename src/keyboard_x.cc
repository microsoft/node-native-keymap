/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Microsoft Corporation. All rights reserved.
 *  Licensed under the MIT License. See License.txt in the project root for
 *  license information.
 *--------------------------------------------------------------------------------------------*/

#include "keymapping.h"

#include "chromium/macros.h"
#include "chromium/x/keysym_to_unicode.h"
#include "string_conversion.h"

#include <X11/XKBlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/XKBrules.h>

#define USB_KEYMAP(usb, evdev, xkb, win, mac, code, id) \
  { usb, xkb, code }
#define USB_KEYMAP_DECLARATION \
  const vscode_keyboard::KeycodeMapEntry usb_keycode_map[] =
#include "chromium/keycode_converter_data.inc"
#undef USB_KEYMAP
#undef USB_KEYMAP_DECLARATION

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
        if (keysym == XK_Meta_L || keysym == XK_Super_L ||
            keysym == XK_Meta_R || keysym == XK_Super_R) {
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
      x_modifier |= mode_switch_modifier;  // alt_r_modifier;
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
  KeyModifierMaskToXModifierMask() { Initialize(NULL); }

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

  if (!character) return std::string();

  wchar_t value = character;

  return vscode_keyboard::UTF16toUTF8(&value, 1);
}

}  // namespace

namespace vscode_keyboard {

NAN_METHOD(GetKeyMap) {
  v8::Local<v8::Object> result = Nan::New<v8::Object>();

  Display* display;
  if (!(display = XOpenDisplay(""))) {
    info.GetReturnValue().Set(result);
    return;
  }

  XEvent event;
  memset(&event, 0, sizeof(XEvent));
  XKeyEvent* key_event = &event.xkey;
  key_event->display = display;
  key_event->type = KeyPress;

  KeyModifierMaskToXModifierMask* mask_provider =
      &KeyModifierMaskToXModifierMask::GetInstance();
  mask_provider->Initialize(display);

  size_t cnt = sizeof(usb_keycode_map) / sizeof(usb_keycode_map[0]);

  for (size_t i = 0; i < cnt; ++i) {
    const char* code = usb_keycode_map[i].code;
    int native_keycode = usb_keycode_map[i].native_keycode;

    if (!code || native_keycode <= 0) {
      continue;
    }

    v8::Local<v8::Object> entry = Nan::New<v8::Object>();

    key_event->keycode = native_keycode;
    key_event->state = 0;
    std::string value = GetStrFromXEvent(&event);
    Nan::Set(
        entry, Nan::New("value").ToLocalChecked(),
        Nan::New<v8::String>(value.data(), value.length()).ToLocalChecked());

    key_event->state = mask_provider->XModFromKeyMod(kShiftKeyModifierMask);
    std::string withShift = GetStrFromXEvent(&event);
    Nan::Set(entry, Nan::New("withShift").ToLocalChecked(),
             Nan::New<v8::String>(withShift.data(), withShift.length())
                 .ToLocalChecked());

    key_event->state = mask_provider->XModFromKeyMod(kControlKeyModifierMask |
                                                     kAltKeyModifierMask);
    std::string withAltGr = GetStrFromXEvent(&event);
    Nan::Set(entry, Nan::New("withAltGr").ToLocalChecked(),
             Nan::New<v8::String>(withAltGr.data(), withAltGr.length())
                 .ToLocalChecked());

    key_event->state = mask_provider->XModFromKeyMod(
        kShiftKeyModifierMask | kControlKeyModifierMask | kAltKeyModifierMask);
    std::string withShiftAltGr = GetStrFromXEvent(&event);
    Nan::Set(
        entry, Nan::New("withShiftAltGr").ToLocalChecked(),
        Nan::New<v8::String>(withShiftAltGr.data(), withShiftAltGr.length())
            .ToLocalChecked());

    Nan::Set(result, Nan::New(code).ToLocalChecked(), entry);
  }

  XFlush(display);
  XCloseDisplay(display);

  info.GetReturnValue().Set(result);
}

NAN_METHOD(GetCurrentKeyboardLayout) {
  Display* display;
  if (!(display = XOpenDisplay(""))) {
    info.GetReturnValue().SetNull();
    return;
  }

  XkbRF_VarDefsRec vdr;
  char* tmp = NULL;
  int res = XkbRF_GetNamesProp(display, &tmp, &vdr);
  if (res) {
    v8::Local<v8::Object> result = Nan::New<v8::Object>();
    Nan::Set(result, Nan::New("model").ToLocalChecked(),
             Nan::New(vdr.model).ToLocalChecked());
    Nan::Set(result, Nan::New("layout").ToLocalChecked(),
             Nan::New(vdr.layout).ToLocalChecked());
    Nan::Set(result, Nan::New("variant").ToLocalChecked(),
             Nan::New(vdr.variant).ToLocalChecked());
    Nan::Set(result, Nan::New("options").ToLocalChecked(),
             Nan::New(vdr.options).ToLocalChecked());
    Nan::Set(result, Nan::New("rules").ToLocalChecked(),
             Nan::New(tmp).ToLocalChecked());
    info.GetReturnValue().Set(result);
  } else {
    info.GetReturnValue().SetNull();
  }

  XFlush(display);
  XCloseDisplay(display);
}

NAN_METHOD(OnDidChangeKeyboardLayout) {}

NAN_METHOD(IsISOKeyboard) {}

}  // namespace vscode_keyboard
