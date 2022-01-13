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
    alt_modifier = 0;
    meta_modifier = 0;
    num_lock_modifier = 0;
    mode_switch_modifier = 0;
    level3_modifier = 0;  // AltGr is often mapped to the level3 modifier
    level5_modifier = 0;  // AltGr is mapped to the level5 modifier in the Neo layout family
    effective_group_index = 0;

    if (!display) {
      return;
    }

    // See https://www.x.org/releases/X11R7.6/doc/libX11/specs/XKB/xkblib.html#determining_keyboard_state
    XkbStateRec xkbState;
    XkbGetState(display, XkbUseCoreKbd, &xkbState);
    effective_group_index = xkbState.group;

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
        if (keysym == XK_ISO_Level3_Shift) {
          level3_modifier = 1 << mod_index;
        }
        if (keysym == XK_ISO_Level5_Shift) {
          level5_modifier = 1 << mod_index;
        }
      }
    }

    XFreeModifiermap(mod_map);
  }

  int XStateFromKeyMod(int keyMod) {
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

    if (keyMod & kLevel3KeyModifierMask) {
      x_modifier |= level3_modifier;
    }

    if (keyMod & kLevel5KeyModifierMask) {
      x_modifier |= level5_modifier;
    }

    // See https://www.x.org/releases/X11R7.6/doc/libX11/specs/XKB/xkblib.html#xkb_state_to_core_protocol_state_transformation
    x_modifier |= (effective_group_index << 13);

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
  int level3_modifier;
  int level5_modifier;
  int effective_group_index;

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

napi_value _GetKeyMap(napi_env env, napi_callback_info info) {

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
      std::string withShift = GetStrFromXEvent(&event);
      NAPI_CALL(env, napi_set_named_property_string_utf8(env, entry, "withShift", withShift.c_str()));
    }

    {
      key_event->state = mask_provider->XStateFromKeyMod(kLevel3KeyModifierMask);
      std::string withAltGr = GetStrFromXEvent(&event);
      NAPI_CALL(env, napi_set_named_property_string_utf8(env, entry, "withAltGr", withAltGr.c_str()));
    }

    {
      key_event->state = mask_provider->XStateFromKeyMod(kShiftKeyModifierMask | kLevel3KeyModifierMask);
      std::string withShiftAltGr = GetStrFromXEvent(&event);
      NAPI_CALL(env, napi_set_named_property_string_utf8(env, entry, "withShiftAltGr", withShiftAltGr.c_str()));
    }

    {
      // level 5 is important for the Neo layout family
      key_event->state = mask_provider->XStateFromKeyMod(kLevel5KeyModifierMask);
      std::string withLevel5 = GetStrFromXEvent(&event);
      NAPI_CALL(env, napi_set_named_property_string_utf8(env, entry, "withLevel5", withLevel5.c_str()));
    }

    {
      // level3 + level5 is Level 6 in terms of the Neo layout family. (Shift + level5 has no special meaning.)
      key_event->state = mask_provider->XStateFromKeyMod(kLevel3KeyModifierMask | kLevel5KeyModifierMask);
      std::string withLevel3Level5 = GetStrFromXEvent(&event);
      NAPI_CALL(env, napi_set_named_property_string_utf8(env, entry, "withLevel3Level5", withLevel3Level5.c_str()));
    }

    NAPI_CALL(env, napi_set_named_property(env, result, code, entry));
  }

  XFlush(display);
  XCloseDisplay(display);

  return result;
}

napi_value _GetCurrentKeyboardLayout(napi_env env, napi_callback_info info) {

  napi_value result;
  NAPI_CALL(env, napi_get_null(env, &result));

  Display *display;
  if (!(display = XOpenDisplay(""))) {
    return result;
  }

  // See https://www.x.org/releases/X11R7.6/doc/libX11/specs/XKB/xkblib.html#determining_keyboard_state
  XkbStateRec xkbState;
  XkbGetState(display, XkbUseCoreKbd, &xkbState);
  int effective_group_index = xkbState.group;

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

static void NotifyJS(napi_env env, napi_value func, void* context, void* data) {
  // env may be NULL if nodejs is shutting down
  if (env != NULL) {
    napi_value global;
    NAPI_CALL_RETURN_VOID(env, napi_get_global(env, &global));

    std::vector<napi_value> argv;
    NAPI_CALL_RETURN_VOID(env, napi_call_function(env, global, func, argv.size(), argv.data(), NULL));
  }
}

static void FinalizeThreadsafeFunction(napi_env env, void* raw_data, void* hint) {
  NotificationCallbackData *data;
  napi_get_instance_data(env, (void**)&data);
  // TODO: signal to the other thread that it should break the while (true) loop
  // only then consider uncommenting the next line:
  // pthread_join(data->tid, NULL);
  data->tsfn = NULL;
}

typedef struct {
  int effective_group_index;
  std::string layout;
  std::string variant;
} KbState;

bool kbStatesEqual(KbState *a, KbState *b) {
  return (
    a->effective_group_index == b->effective_group_index
    && a->layout == b->layout
    && a->variant == b->variant
  );
}

void readKbState(Display *display, KbState *dst) {
  // See https://www.x.org/releases/X11R7.6/doc/libX11/specs/XKB/xkblib.html#determining_keyboard_state
  // Get effective group index
  XkbStateRec xkbState;
  XkbGetState(display, XkbUseCoreKbd, &xkbState);
  dst->effective_group_index = xkbState.group;

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

void* listenToXEvents(void *arg) {
  NotificationCallbackData *data = static_cast<NotificationCallbackData*>(arg);

  Display *display;
  if (!(display = XOpenDisplay(""))) {
    return NULL;
  }

  int xkblibMajor = XkbMajorVersion;
  int xkblibMinor = XkbMinorVersion;
  if (!XkbLibraryVersion(&xkblibMajor, &xkblibMinor)) {
    return NULL;
  }

  int opcode = 0;
  int xkbBaseEventCode = 0;
  int xkbBaseErrorCode = 0;
  if (!XkbQueryExtension(display, &opcode, &xkbBaseEventCode, &xkbBaseErrorCode, &xkblibMajor, &xkblibMinor)) {
    return NULL;
  }

  // See https://www.x.org/releases/X11R7.6/doc/libX11/specs/XKB/xkblib.html#xkb_event_types
  // Listen only to the `XkbStateNotify` event
  XkbSelectEvents(display, XkbUseCoreKbd, XkbAllEventsMask, XkbStateNotifyMask);

  KbState lastState;
  readKbState(display, &lastState);

  XkbEvent event;
  KbState currentState;
  while (true) {
    XNextEvent(display, &event.core);

    if (event.type == xkbBaseEventCode && event.any.xkb_type == XkbStateNotify) {
      readKbState(display, &currentState);
      // printf("current state: %d | %s | %s\n", currentState.effective_group_index, currentState.layout.c_str(), currentState.variant.c_str());
      if (!kbStatesEqual(&lastState, &currentState)) {
        lastState.effective_group_index = currentState.effective_group_index;
        lastState.layout = currentState.layout;
        lastState.variant = currentState.variant;

        if (data->tsfn == NULL) {
          // This indicates we are in the shutdown phase and the thread safe function has been finalized
          return NULL;
        }

        // No need to call napi_acquire_threadsafe_function because
        // the refcount is set to 1 in the main thread.
        napi_call_threadsafe_function(data->tsfn, NULL, napi_tsfn_blocking);
      }
    }
  }

  napi_release_threadsafe_function(data->tsfn, napi_tsfn_release);

  return NULL;
}

napi_value _OnDidChangeKeyboardLayout(napi_env env, napi_callback_info info) {
  size_t argc = 2;
  napi_value args[2];
  NotificationCallbackData *data;
  NAPI_CALL(env, napi_get_instance_data(env, (void**)&data));
  NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, NULL, NULL));

  NAPI_ASSERT(env, argc == 1, "Wrong number of arguments. Expects a single argument.");

  napi_valuetype valuetype0;
  NAPI_CALL(env, napi_typeof(env, args[0], &valuetype0));
  NAPI_ASSERT(env, valuetype0 == napi_function, "Wrong type of arguments. Expects a function as first argument.");

  napi_value func = args[0];

  napi_value resource_name;
  NAPI_CALL(env, napi_create_string_utf8(env, "onDidChangeKeyboardLayoutCallback", NAPI_AUTO_LENGTH, &resource_name));

  // Convert the callback retrieved from JavaScript into a thread-safe function
  NAPI_CALL(env, napi_create_threadsafe_function(env, func, NULL, resource_name, 0, 1, NULL,
                                                 FinalizeThreadsafeFunction, NULL, NotifyJS,
                                                 &data->tsfn));

  pthread_create(&data->tid, NULL, &listenToXEvents, data);

  return napi_fetch_undefined(env);
}

napi_value _isISOKeyboard(napi_env env, napi_callback_info info) {
  return napi_fetch_undefined(env);
}

} // namespace vscode_keyboard
