/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Microsoft Corporation. All rights reserved.
 *  Licensed under the MIT License. See License.txt in the project root for license information.
 *--------------------------------------------------------------------------------------------*/

#include "keymapping.h"
#include "string_conversion.h"

#include <X11/XKBlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/XKBrules.h>

#include "../deps/chromium/macros.h"
#include "../deps/chromium/x/keysym_to_unicode.h"

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

using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::Object;
using v8::String;
using v8::Array;
using v8::Value;
using v8::Null;

#define USB_KEYMAP(usb, evdev, xkb, win, mac, code, id) {usb, xkb, code}
#define USB_KEYMAP_DECLARATION const KeycodeMapEntry usb_keycode_map[] =
#include "../deps/chromium/keycode_converter_data.inc"
#undef USB_KEYMAP
#undef USB_KEYMAP_DECLARATION

void _GetKeyMap(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  Local<Object> result = Object::New(isolate);
  Local<String> _value = String::NewFromUtf8(isolate, "value");
  Local<String> _withShift = String::NewFromUtf8(isolate, "withShift");
  Local<String> _withAltGr = String::NewFromUtf8(isolate, "withAltGr");
  Local<String> _withShiftAltGr = String::NewFromUtf8(isolate, "withShiftAltGr");

  Display *display;
  if (!(display = XOpenDisplay(""))) {
    args.GetReturnValue().Set(result);
    return;
  }

  XEvent event;
  memset(&event, 0, sizeof(XEvent));
  XKeyEvent* key_event = &event.xkey;
  key_event->display = display;
  key_event->type = KeyPress;

  KeyModifierMaskToXModifierMask *mask_provider = &KeyModifierMaskToXModifierMask::GetInstance();
  mask_provider->Initialize(display);

  size_t cnt = sizeof(usb_keycode_map) / sizeof(usb_keycode_map[0]);

  for (size_t i = 0; i < cnt; ++i) {
    const char *code = usb_keycode_map[i].code;
    int native_keycode = usb_keycode_map[i].native_keycode;

    if (!code || native_keycode <= 0) {
      continue;
    }

    Local<Object> entry = Object::New(isolate);

    key_event->keycode = native_keycode;
    key_event->state = 0;
    std::string value = GetStrFromXEvent(&event);
    entry->Set(_value, String::NewFromUtf8(isolate, value.c_str()));

    key_event->state = mask_provider->XModFromKeyMod(kShiftKeyModifierMask);
    std::string withShift = GetStrFromXEvent(&event);
    entry->Set(_withShift, String::NewFromUtf8(isolate, withShift.c_str()));

    key_event->state = mask_provider->XModFromKeyMod(kControlKeyModifierMask | kAltKeyModifierMask);
    std::string withAltGr = GetStrFromXEvent(&event);
    entry->Set(_withAltGr, String::NewFromUtf8(isolate, withAltGr.c_str()));

    key_event->state = mask_provider->XModFromKeyMod(kShiftKeyModifierMask | kControlKeyModifierMask | kAltKeyModifierMask);
    std::string withShiftAltGr = GetStrFromXEvent(&event);
    entry->Set(_withShiftAltGr, String::NewFromUtf8(isolate, withShiftAltGr.c_str()));

    result->Set(String::NewFromUtf8(isolate, code), entry);
  }

  XFlush(display);
  XCloseDisplay(display);

  args.GetReturnValue().Set(result);
}

void _GetCurrentKeyboardLayout(const v8::FunctionCallbackInfo<v8::Value>& args) {
  Isolate* isolate = args.GetIsolate();

  Display *display;
  if (!(display = XOpenDisplay(""))) {
    args.GetReturnValue().Set(Null(isolate));
    return;
  }

  XkbRF_VarDefsRec vdr;
  char *tmp = NULL;
  int res = XkbRF_GetNamesProp(display, &tmp, &vdr);
  if (res) {
    Local<Object> result = Object::New(isolate);
    result->Set(String::NewFromUtf8(isolate, "model"), String::NewFromUtf8(isolate, vdr.model ? vdr.model : ""));
    result->Set(String::NewFromUtf8(isolate, "layout"), String::NewFromUtf8(isolate, vdr.layout ? vdr.layout : ""));
    result->Set(String::NewFromUtf8(isolate, "variant"), String::NewFromUtf8(isolate, vdr.variant ? vdr.variant : ""));
    result->Set(String::NewFromUtf8(isolate, "options"), String::NewFromUtf8(isolate, vdr.options ? vdr.options : ""));
    result->Set(String::NewFromUtf8(isolate, "rules"), String::NewFromUtf8(isolate, tmp ? tmp : ""));
    args.GetReturnValue().Set(result);
  } else {
    args.GetReturnValue().Set(Null(isolate));
  }

  XFlush(display);
  XCloseDisplay(display);
}

void _OnDidChangeKeyboardLayout(const v8::FunctionCallbackInfo<v8::Value>& args) {

}

void _isISOKeyboard(const v8::FunctionCallbackInfo<v8::Value>& args) {

}

} // namespace vscode_keyboard
