/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Microsoft Corporation. All rights reserved.
 *  Licensed under the MIT License. See License.txt in the project root for license information.
 *--------------------------------------------------------------------------------------------*/

#include "keymapping.h"
#include "string_conversion.h"
#include "common.h"

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
    alt_modifier_ = 0;
    meta_modifier_ = 0;
    num_lock_modifier_ = 0;
    mode_switch_modifier_ = 0;
    level3_modifier_ = 0;  // AltGr is often mapped to the level3 modifier
    level5_modifier_ = 0;  // AltGr is mapped to the level5 modifier in the Neo layout family
    effective_group_index_ = 0;

    if (!display) {
      return;
    }

    // See https://www.x.org/releases/X11R7.6/doc/libX11/specs/XKB/xkblib.html#determining_keyboard_state
    XkbStateRec xkb_state;
    XkbGetState(display, XkbUseCoreKbd, &xkb_state);
    effective_group_index_ = xkb_state.group;

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

        if (keysym == XK_Alt_L || keysym == XK_Alt_R) {
          alt_modifier_ = 1 << mod_index;
        }
        if (keysym == XK_Mode_switch) {
          mode_switch_modifier_ = 1 << mod_index;
        }
        if (keysym == XK_Meta_L || keysym == XK_Super_L || keysym == XK_Meta_R || keysym == XK_Super_R) {
          meta_modifier_ = 1 << mod_index;
        }
        if (keysym == XK_Num_Lock) {
          num_lock_modifier_ = 1 << mod_index;
        }
        if (keysym == XK_ISO_Level3_Shift) {
          level3_modifier_ = 1 << mod_index;
        }
        if (keysym == XK_ISO_Level5_Shift) {
          level5_modifier_ = 1 << mod_index;
        }
      }
    }

    XFreeModifiermap(mod_map);
  }

  int XStateFromKeyMod(int keyMod) {
    int x_modifier = 0;

    // Ctrl + Alt => AltGr
    if (keyMod & kControlKeyModifierMask && keyMod & kAltKeyModifierMask) {
      x_modifier |= mode_switch_modifier_;//alt_r_modifier;
    } else if (keyMod & kControlKeyModifierMask) {
      x_modifier |= ControlMask;
    } else if (keyMod & kAltKeyModifierMask) {
      x_modifier |= alt_modifier_;
    }

    if (keyMod & kShiftKeyModifierMask) {
      x_modifier |= ShiftMask;
    }

    if (keyMod & kMetaKeyModifierMask) {
      x_modifier |= meta_modifier_;
    }

    if (keyMod & kNumLockKeyModifierMask) {
      x_modifier |= num_lock_modifier_;
    }

    if (keyMod & kLevel3KeyModifierMask) {
      x_modifier |= level3_modifier_;
    }

    if (keyMod & kLevel5KeyModifierMask) {
      x_modifier |= level5_modifier_;
    }

    // See https://www.x.org/releases/X11R7.6/doc/libX11/specs/XKB/xkblib.html#xkb_state_to_core_protocol_state_transformation
    x_modifier |= (effective_group_index_ << 13);

    return x_modifier;
  }

 private:
  KeyModifierMaskToXModifierMask() {
    Initialize(NULL);
  }

  int alt_modifier_;
  int meta_modifier_;
  int num_lock_modifier_;
  int mode_switch_modifier_;
  int level3_modifier_;
  int level5_modifier_;
  int effective_group_index_;

  KeyModifierMaskToXModifierMask(const KeyModifierMaskToXModifierMask&) = delete;
  KeyModifierMaskToXModifierMask& operator=(const KeyModifierMaskToXModifierMask&) = delete;
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

#define DOM_CODE(usb, evdev, xkb, win, mac, code, id) {usb, xkb, code}
#define DOM_CODE_DECLARATION const KeycodeMapEntry usb_keycode_map[] =
#include "../deps/chromium/dom_code_data.inc"
#undef DOM_CODE
#undef DOM_CODE_DECLARATION

napi_value GetKeyMapImpl(napi_env env, napi_callback_info info) {

  napi_value result;
  NAPI_CALL(env, napi_create_object(env, &result));

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

  size_t cnt = sizeof(usb_keycode_map) / sizeof(usb_keycode_map[0]);

  for (size_t i = 0; i < cnt; ++i) {
    const char *code = usb_keycode_map[i].code;
    int native_keycode = usb_keycode_map[i].native_keycode;

    if (!code || native_keycode <= 0) {
      continue;
    }

    napi_value entry;
    NAPI_CALL(env, napi_create_object(env, &entry));

    key_event->keycode = native_keycode;
    {
      key_event->state = mask_provider->XStateFromKeyMod(0);
      std::string value = GetStrFromXEvent(&event);
      NAPI_CALL(env, napi_set_named_property_string_utf8(env, entry, "value", value.c_str()));
    }

    {
      key_event->state = mask_provider->XStateFromKeyMod(kShiftKeyModifierMask);
      std::string with_shift = GetStrFromXEvent(&event);
      NAPI_CALL(env, napi_set_named_property_string_utf8(env, entry, "withShift", with_shift.c_str()));
    }

    {
      key_event->state = mask_provider->XStateFromKeyMod(kLevel3KeyModifierMask);
      std::string with_alt_gr = GetStrFromXEvent(&event);
      NAPI_CALL(env, napi_set_named_property_string_utf8(env, entry, "withAltGr", with_alt_gr.c_str()));
    }

    {
      key_event->state = mask_provider->XStateFromKeyMod(kShiftKeyModifierMask | kLevel3KeyModifierMask);
      std::string with_shift_alt_gr = GetStrFromXEvent(&event);
      NAPI_CALL(env, napi_set_named_property_string_utf8(env, entry, "withShiftAltGr", with_shift_alt_gr.c_str()));
    }

    {
      // level 5 is important for the Neo layout family
      key_event->state = mask_provider->XStateFromKeyMod(kLevel5KeyModifierMask);
      std::string with_level5 = GetStrFromXEvent(&event);
      NAPI_CALL(env, napi_set_named_property_string_utf8(env, entry, "withLevel5", with_level5.c_str()));
    }

    {
      // level3 + level5 is Level 6 in terms of the Neo layout family. (Shift + level5 has no special meaning.)
      key_event->state = mask_provider->XStateFromKeyMod(kLevel3KeyModifierMask | kLevel5KeyModifierMask);
      std::string with_level3_level5 = GetStrFromXEvent(&event);
      NAPI_CALL(env, napi_set_named_property_string_utf8(env, entry, "withLevel3Level5", with_level3_level5.c_str()));
    }

    NAPI_CALL(env, napi_set_named_property(env, result, code, entry));
  }

  XFlush(display);
  XCloseDisplay(display);

  return result;
}

napi_value GetCurrentKeyboardLayoutImpl(napi_env env, napi_callback_info info) {

  napi_value result;
  NAPI_CALL(env, napi_get_null(env, &result));

  Display *display;
  if (!(display = XOpenDisplay(""))) {
    return result;
  }

  // See https://www.x.org/releases/X11R7.6/doc/libX11/specs/XKB/xkblib.html#determining_keyboard_state
  XkbStateRec xkb_state;
  XkbGetState(display, XkbUseCoreKbd, &xkb_state);
  int effective_group_index = xkb_state.group;

  XkbRF_VarDefsRec vdr;
  char *tmp = NULL;
  int res = XkbRF_GetNamesProp(display, &tmp, &vdr);
  if (res) {
    NAPI_CALL(env, napi_create_object(env, &result));

    NAPI_CALL(env, napi_set_named_property_string_utf8(env, result, "model", vdr.model ? vdr.model : ""));
    NAPI_CALL(env, napi_set_named_property_int32(env, result, "group", effective_group_index));
    NAPI_CALL(env, napi_set_named_property_string_utf8(env, result, "layout", vdr.layout ? vdr.layout : ""));
    NAPI_CALL(env, napi_set_named_property_string_utf8(env, result, "variant", vdr.variant ? vdr.variant : ""));
    NAPI_CALL(env, napi_set_named_property_string_utf8(env, result, "options", vdr.options ? vdr.options : ""));
    NAPI_CALL(env, napi_set_named_property_string_utf8(env, result, "rules", tmp ? tmp : ""));
  }

  XFlush(display);
  XCloseDisplay(display);
  return result;
}

typedef struct {
  int effective_group_index;
  std::string layout;
  std::string variant;
} KbState;

bool KbStatesEqual(KbState *a, KbState *b) {
  return (
    a->effective_group_index == b->effective_group_index
    && a->layout == b->layout
    && a->variant == b->variant
  );
}

void ReadKbState(Display *display, KbState *dst) {
  // See https://www.x.org/releases/X11R7.6/doc/libX11/specs/XKB/xkblib.html#determining_keyboard_state
  // Get effective group index
  XkbStateRec xkb_state;
  XkbGetState(display, XkbUseCoreKbd, &xkb_state);
  dst->effective_group_index = xkb_state.group;

  XkbRF_VarDefsRec vdr;
  char *tmp = NULL;
  int res = XkbRF_GetNamesProp(display, &tmp, &vdr);
  if (res) {
    dst->layout = (vdr.layout ? vdr.layout : "");
    dst->variant = (vdr.variant ? vdr.variant : "");
  } else {
    dst->layout = "";
    dst->variant = "";
  }
}

static void FlushAndCloseDisplay(void *arg) {
  Display *display = static_cast<Display*>(arg);
  XFlush(display);
  XCloseDisplay(display);
}

void* ListenToXEvents(void *arg) {
  NotificationCallbackData *data = static_cast<NotificationCallbackData*>(arg);

  Display *display;
  if (!(display = XOpenDisplay(""))) {
    return NULL;
  }

  pthread_cleanup_push(FlushAndCloseDisplay, display);

  int xkblib_major = XkbMajorVersion;
  int xkblib_minor = XkbMinorVersion;
  if (!XkbLibraryVersion(&xkblib_major, &xkblib_minor)) {
    return NULL;
  }

  int opcode = 0;
  int xkb_base_event_code = 0;
  int xkb_base_error_code = 0;
  if (!XkbQueryExtension(display, &opcode, &xkb_base_event_code, &xkb_base_error_code, &xkblib_major, &xkblib_minor)) {
    return NULL;
  }

  // See https://www.x.org/releases/X11R7.6/doc/libX11/specs/XKB/xkblib.html#xkb_event_types
  // Listen only to the `XkbStateNotify` event
  XkbSelectEvents(display, XkbUseCoreKbd, XkbAllEventsMask, XkbStateNotifyMask);

  KbState last_state;
  ReadKbState(display, &last_state);

  XkbEvent event;
  KbState current_state;
  fd_set in_fds;
  struct timeval tv;
  int x11_fd = ConnectionNumber(display);

  while (true) {
    // See https://stackoverflow.com/a/8592969 which explains
    // the technique of waiting for an XEvent with a timeout

    // Create a File Description Set containing x11_fd
    FD_ZERO(&in_fds);
    FD_SET(x11_fd, &in_fds);

    // Set the timer to 1s.
    tv.tv_usec = 0;
    tv.tv_sec = 1;

    // Wait for X Event or the timer
    select(x11_fd + 1, &in_fds, NULL, NULL, &tv);

    // Handle pending XEvents
    while (XPending(display)) {

      XNextEvent(display, &event.core);

      if (event.type == xkb_base_event_code && event.any.xkb_type == XkbStateNotify) {
        ReadKbState(display, &current_state);
        // printf("current state: %d | %s | %s\n", current_state.effective_group_index, current_state.layout.c_str(), current_state.variant.c_str());
        if (!KbStatesEqual(&last_state, &current_state)) {
          last_state.effective_group_index = current_state.effective_group_index;
          last_state.layout = current_state.layout;
          last_state.variant = current_state.variant;

          InvokeNotificationCallback(data);
        }
      }
    }
  }

  pthread_cleanup_pop(1);

  return NULL;
}

void RegisterKeyboardLayoutChangeListenerImpl(NotificationCallbackData *data) {
  pthread_create(&data->tid, NULL, &ListenToXEvents, data);
}

void DisposeKeyboardLayoutChangeListenerImpl(NotificationCallbackData *data) {
  pthread_cancel(data->tid);
  void *res;
  pthread_join(data->tid, &res);
}

napi_value IsISOKeyboardImpl(napi_env env, napi_callback_info info) {
  return napi_fetch_undefined(env);
}

} // namespace vscode_keyboard
