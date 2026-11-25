/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Microsoft Corporation. All rights reserved.
 *  Licensed under the MIT License. See License.txt in the project root for license information.
 *--------------------------------------------------------------------------------------------*/

#include "keyboard_wayland.h"
#include "string_conversion.h"
#include "common.h"

#include <errno.h>
#include <stdio.h>
#include <poll.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>

#include <string>
#include <vector>

#include <wayland-client.h>
#include <xkbcommon/xkbcommon.h>

#include "../deps/chromium/x/keysym_to_unicode.h"

namespace {

// How long to wait for the compositor to answer a roundtrip.
const int kRoundtripTimeoutMs = 500;
// The registry, the seat's capabilities and the keymap each take a roundtrip.
const int kMaxRoundtrips = 4;
// wl_seat version 5 is the first with wl_seat.release. Newer versions add no
// wl_keyboard events, so the listeners below handle every event we can get.
const uint32_t kMaxSeatVersion = 5;

enum class Backend {
  X11,
  Wayland,
  // NATIVE_KEYMAP_BACKEND=wayland: never fall back to the X server.
  WaylandOnly,
};

struct WaylandState {
  struct wl_display *display = nullptr;
  struct wl_registry *registry = nullptr;
  struct wl_seat *seat = nullptr;
  uint32_t seat_version = 0;
  struct wl_keyboard *keyboard = nullptr;
  std::string keymap;
  bool has_keymap = false;
  uint32_t group = 0;
  bool changed = false;
  bool read_prepared = false;
};

void KeyboardKeymap(void *data, struct wl_keyboard *keyboard, uint32_t format, int32_t fd, uint32_t size) {
  WaylandState *state = static_cast<WaylandState*>(data);
  if (format == WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1 && size > 0) {
    void *mapped = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (mapped != MAP_FAILED) {
      const char *text = static_cast<const char*>(mapped);
      std::string keymap(text, strnlen(text, size));
      munmap(mapped, size);
      if (!state->has_keymap || keymap != state->keymap) {
        state->keymap = keymap;
        state->has_keymap = true;
        state->changed = true;
      }
    }
  }
  close(fd);
}

void KeyboardEnter(void *data, struct wl_keyboard *keyboard, uint32_t serial, struct wl_surface *surface, struct wl_array *keys) {
}

void KeyboardLeave(void *data, struct wl_keyboard *keyboard, uint32_t serial, struct wl_surface *surface) {
}

void KeyboardKey(void *data, struct wl_keyboard *keyboard, uint32_t serial, uint32_t time, uint32_t key, uint32_t key_state) {
}

void KeyboardModifiers(void *data, struct wl_keyboard *keyboard, uint32_t serial, uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked, uint32_t group) {
  // Compositors only send this to the client with keyboard focus, and this
  // connection has no surface, so in practice the group stays 0.
  WaylandState *state = static_cast<WaylandState*>(data);
  if (state->group != group) {
    state->group = group;
    state->changed = true;
  }
}

void KeyboardRepeatInfo(void *data, struct wl_keyboard *keyboard, int32_t rate, int32_t delay) {
}

const struct wl_keyboard_listener kKeyboardListener = {
  KeyboardKeymap,
  KeyboardEnter,
  KeyboardLeave,
  KeyboardKey,
  KeyboardModifiers,
  KeyboardRepeatInfo,
};

void ReleaseKeyboard(WaylandState *state) {
  if (!state->keyboard) {
    return;
  }
  if (wl_keyboard_get_version(state->keyboard) >= WL_KEYBOARD_RELEASE_SINCE_VERSION) {
    wl_keyboard_release(state->keyboard);
  } else {
    wl_keyboard_destroy(state->keyboard);
  }
  state->keyboard = nullptr;
}

void SeatCapabilities(void *data, struct wl_seat *seat, uint32_t capabilities) {
  WaylandState *state = static_cast<WaylandState*>(data);
  bool has_keyboard = (capabilities & WL_SEAT_CAPABILITY_KEYBOARD) != 0;
  if (has_keyboard && !state->keyboard) {
    state->keyboard = wl_seat_get_keyboard(seat);
    wl_keyboard_add_listener(state->keyboard, &kKeyboardListener, state);
  } else if (!has_keyboard && state->keyboard) {
    ReleaseKeyboard(state);
  }
}

void SeatName(void *data, struct wl_seat *seat, const char *name) {
}

const struct wl_seat_listener kSeatListener = {
  SeatCapabilities,
  SeatName,
};

void RegistryGlobal(void *data, struct wl_registry *registry, uint32_t name, const char *interface, uint32_t version) {
  WaylandState *state = static_cast<WaylandState*>(data);
  // Like Chromium, use the first seat.
  if (!state->seat && strcmp(interface, wl_seat_interface.name) == 0) {
    state->seat_version = version < kMaxSeatVersion ? version : kMaxSeatVersion;
    state->seat = static_cast<struct wl_seat*>(wl_registry_bind(registry, name, &wl_seat_interface, state->seat_version));
    wl_seat_add_listener(state->seat, &kSeatListener, state);
  }
}

void RegistryGlobalRemove(void *data, struct wl_registry *registry, uint32_t name) {
}

const struct wl_registry_listener kRegistryListener = {
  RegistryGlobal,
  RegistryGlobalRemove,
};

int64_t NowMs() {
  struct timespec now;
  clock_gettime(CLOCK_MONOTONIC, &now);
  return static_cast<int64_t>(now.tv_sec) * 1000 + now.tv_nsec / 1000000;
}

// Waits up to `timeout_ms` for events and dispatches them.
// Returns 1 when events were read, 0 on timeout and -1 when the connection is lost.
// With `cancellable`, the thread can only be cancelled while it waits.
int DispatchOnce(WaylandState *state, int timeout_ms, bool cancellable) {
  struct wl_display *display = state->display;
  while (wl_display_prepare_read(display) != 0) {
    if (wl_display_dispatch_pending(display) < 0) {
      return -1;
    }
  }
  state->read_prepared = true;

  if (wl_display_flush(display) < 0 && errno != EAGAIN) {
    wl_display_cancel_read(display);
    state->read_prepared = false;
    return -1;
  }

  struct pollfd pfd;
  pfd.fd = wl_display_get_fd(display);
  pfd.events = POLLIN;
  pfd.revents = 0;

  int ret;
  if (cancellable) {
    int old_state;
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &old_state);
    ret = poll(&pfd, 1, timeout_ms);
    pthread_setcancelstate(old_state, NULL);
  } else {
    ret = poll(&pfd, 1, timeout_ms);
  }

  if (ret <= 0) {
    int poll_errno = errno;
    wl_display_cancel_read(display);
    state->read_prepared = false;
    if (ret < 0 && poll_errno != EINTR) {
      return -1;
    }
    return 0;
  }

  state->read_prepared = false;
  if (wl_display_read_events(display) < 0) {
    return -1;
  }
  if (wl_display_dispatch_pending(display) < 0) {
    return -1;
  }
  return 1;
}

void SyncDone(void *data, struct wl_callback *callback, uint32_t serial) {
  *static_cast<bool*>(data) = true;
}

const struct wl_callback_listener kSyncListener = {
  SyncDone,
};

// Like wl_display_roundtrip, but gives up after `timeout_ms`.
bool Roundtrip(WaylandState *state, int timeout_ms) {
  struct wl_callback *callback = wl_display_sync(state->display);
  if (!callback) {
    return false;
  }
  bool done = false;
  wl_callback_add_listener(callback, &kSyncListener, &done);

  int64_t deadline = NowMs() + timeout_ms;
  while (!done) {
    int64_t remaining = deadline - NowMs();
    if (remaining <= 0 || DispatchOnce(state, static_cast<int>(remaining), false) < 0) {
      break;
    }
  }

  wl_callback_destroy(callback);
  return done;
}

void Disconnect(WaylandState *state) {
  ReleaseKeyboard(state);
  if (state->seat) {
    if (state->seat_version >= WL_SEAT_RELEASE_SINCE_VERSION) {
      wl_seat_release(state->seat);
    } else {
      wl_seat_destroy(state->seat);
    }
    state->seat = nullptr;
  }
  if (state->registry) {
    wl_registry_destroy(state->registry);
    state->registry = nullptr;
  }
  if (state->display) {
    wl_display_flush(state->display);
    wl_display_disconnect(state->display);
    state->display = nullptr;
  }
}

// Opens a connection of our own to the compositor and waits for the keymap of
// the first seat. A client without a surface gets the keymap, which the
// compositor sends again whenever it changes, but not the modifiers and group.
bool Connect(WaylandState *state) {
  // WAYLAND_SOCKET is an already connected socket that libwayland takes over
  // (and closes): it belongs to whoever it was passed to, not to us.
  if (getenv("WAYLAND_SOCKET")) {
    return false;
  }
  state->display = wl_display_connect(NULL);
  if (!state->display) {
    return false;
  }
  state->registry = wl_display_get_registry(state->display);
  if (!state->registry) {
    return false;
  }
  wl_registry_add_listener(state->registry, &kRegistryListener, state);
  for (int i = 0; i < kMaxRoundtrips && !state->has_keymap; i++) {
    if (!Roundtrip(state, kRoundtripTimeoutMs)) {
      break;
    }
  }
  return state->has_keymap;
}

Backend ComputeBackend() {
  const char *requested = getenv("NATIVE_KEYMAP_BACKEND");
  if (requested && strcmp(requested, "x11") == 0) {
    return Backend::X11;
  }
  if (requested && strcmp(requested, "wayland") == 0) {
    return Backend::WaylandOnly;
  }
  const char *wayland_display = getenv("WAYLAND_DISPLAY");
  if (!wayland_display || !*wayland_display) {
    return Backend::X11;
  }
  WaylandState state;
  bool connected = Connect(&state);
  Disconnect(&state);
  return connected ? Backend::Wayland : Backend::X11;
}

Backend GetBackend() {
  static const Backend backend = ComputeBackend();
  return backend;
}

std::string KeysymToUtf8(xkb_keysym_t keysym) {
  uint16_t character = ui::GetUnicodeCharacterFromXKeySym(keysym);
  if (!character) {
    return std::string();
  }
  wchar_t value = character;
  return vscode_keyboard::UTF16toUTF8(&value, 1);
}

// Returns the modifiers that the key producing `keysym` sets in `group`, as
// the X server's modifier map does for the X11 backend.
xkb_mod_mask_t FindModifierOfKeysym(struct xkb_keymap *keymap, xkb_keysym_t keysym, xkb_layout_index_t group) {
  xkb_keycode_t min_keycode = xkb_keymap_min_keycode(keymap);
  xkb_keycode_t max_keycode = xkb_keymap_max_keycode(keymap);
  for (xkb_keycode_t keycode = min_keycode; keycode <= max_keycode; keycode++) {
    xkb_layout_index_t num_layouts = xkb_keymap_num_layouts_for_key(keymap, keycode);
    if (num_layouts == 0) {
      continue;
    }
    xkb_layout_index_t key_group = group < num_layouts ? group : 0;
    const xkb_keysym_t *syms = NULL;
    int count = xkb_keymap_key_get_syms_by_level(keymap, keycode, key_group, 0, &syms);
    if (count != 1 || syms[0] != keysym) {
      continue;
    }
    struct xkb_state *state = xkb_state_new(keymap);
    if (!state) {
      continue;
    }
    xkb_state_update_mask(state, 0, 0, 0, 0, 0, group);
    xkb_state_update_key(state, keycode, XKB_KEY_DOWN);
    xkb_mod_mask_t mask = xkb_state_serialize_mods(state, XKB_STATE_MODS_DEPRESSED);
    xkb_state_unref(state);
    if (mask) {
      return mask;
    }
  }
  return 0;
}

// Returns the name of a section of the keymap, such as `pc+us+ru:2+inet(evdev)`
// for `xkb_symbols`. xkbcommon keeps the names that the compositor compiled
// the keymap from.
std::string GetSectionName(const std::string &keymap, const char *section) {
  std::string needle = std::string(section) + " \"";
  size_t start = keymap.find(needle);
  if (start == std::string::npos) {
    return std::string();
  }
  start += needle.size();
  size_t end = keymap.find('"', start);
  if (end == std::string::npos) {
    return std::string();
  }
  return keymap.substr(start, end - start);
}

// Parts of `xkb_symbols` names that come from the model and the options.
bool IsLayout(const std::string &name) {
  static const char *const kNotLayouts[] = {
    "pc", "inet", "group", "level3", "level5", "compose", "ctrl", "altwin",
    "capslock", "terminate", "keypad", "kpdl", "nbsp", "eurosign", "rupeesign",
    "shift", "srvr_ctrl", "parens", "typo", "japan", "korean", "lv3", "lv5",
    "caps", "numpad", "mod_led", "grp_led", "fkeys", "solaris", "sun_vndr",
  };
  if (name.empty()) {
    return false;
  }
  for (const char *not_layout : kNotLayouts) {
    if (name == not_layout) {
      return false;
    }
  }
  return true;
}

std::string Join(const std::vector<std::string> &parts) {
  bool all_empty = true;
  std::string result;
  for (size_t i = 0; i < parts.size(); i++) {
    if (i > 0) {
      result += ",";
    }
    result += parts[i];
    all_empty = all_empty && parts[i].empty();
  }
  return all_empty ? std::string() : result;
}

// Turns `pc+us(intl)+ru:2+inet(evdev)` into layouts `us,ru` and variants `intl,`.
void ParseSymbols(const std::string &symbols, std::string *layout, std::string *variant) {
  std::vector<std::string> layouts;
  std::vector<std::string> variants;
  size_t start = 0;
  while (start < symbols.size()) {
    size_t end = symbols.find_first_of("+|", start);
    if (end == std::string::npos) {
      end = symbols.size();
    }
    std::string part = symbols.substr(start, end - start);
    start = end + 1;

    size_t group = 0;
    size_t colon = part.find(':');
    if (colon != std::string::npos) {
      int number = atoi(part.c_str() + colon + 1);
      group = number > 0 ? number - 1 : 0;
      part = part.substr(0, colon);
    }
    std::string name = part;
    std::string part_variant;
    size_t paren = part.find('(');
    if (paren != std::string::npos) {
      name = part.substr(0, paren);
      size_t close = part.find(')', paren);
      part_variant = part.substr(paren + 1, (close == std::string::npos ? part.size() : close) - paren - 1);
    }
    if (!IsLayout(name) || group >= 4) {
      continue;
    }
    if (layouts.size() <= group) {
      layouts.resize(group + 1);
      variants.resize(group + 1);
    }
    if (layouts[group].empty()) {
      layouts[group] = name;
      variants[group] = part_variant;
    }
  }
  *layout = Join(layouts);
  *variant = Join(variants);
}

struct xkb_keymap *CompileKeymap(struct xkb_context **context, const std::string &text) {
  *context = xkb_context_new(static_cast<enum xkb_context_flags>(XKB_CONTEXT_NO_DEFAULT_INCLUDES | XKB_CONTEXT_NO_ENVIRONMENT_NAMES));
  if (!*context) {
    return NULL;
  }
  xkb_context_set_log_level(*context, XKB_LOG_LEVEL_CRITICAL);
  return xkb_keymap_new_from_string(*context, text.c_str(), XKB_KEYMAP_FORMAT_TEXT_V1, XKB_KEYMAP_COMPILE_NO_FLAGS);
}

// Returns the text of the first `<tag>...</tag>` in `xml` within [from, to).
std::string GetElementText(const std::string &xml, const char *tag, size_t from, size_t to) {
  std::string open = std::string("<") + tag + ">";
  std::string close = std::string("</") + tag + ">";
  size_t start = xml.find(open, from);
  if (start == std::string::npos || start >= to) {
    return std::string();
  }
  start += open.size();
  size_t end = xml.find(close, start);
  if (end == std::string::npos || end > to) {
    return std::string();
  }
  return xml.substr(start, end - start);
}

std::string EscapeXml(const std::string &text) {
  std::string result;
  for (char c : text) {
    switch (c) {
      case '&': result += "&amp;"; break;
      case '<': result += "&lt;"; break;
      case '>': result += "&gt;"; break;
      default: result += c;
    }
  }
  return result;
}

// Finds the layout and variant that xkeyboard-config describes as
// `description` (the name that xkbcommon gives a group, such as
// "German (Neo 2)") in its registry of layouts (rules/evdev.xml).
bool LookupLayout(const std::string &registry, const char *description, std::string *layout, std::string *variant) {
  if (registry.empty() || !description || !*description) {
    return false;
  }
  size_t pos = registry.find("<description>" + EscapeXml(description) + "</description>");
  if (pos == std::string::npos) {
    return false;
  }
  size_t item = registry.rfind("<configItem", pos);
  size_t layout_start = registry.rfind("<layout>", pos);
  if (item == std::string::npos || layout_start == std::string::npos) {
    return false;
  }
  std::string name = GetElementText(registry, "name", item, pos);
  size_t variant_start = registry.rfind("<variant>", pos);
  if (variant_start != std::string::npos && variant_start > layout_start) {
    *layout = GetElementText(registry, "name", layout_start, pos);
    *variant = name;
  } else {
    *layout = name;
    variant->clear();
  }
  return !layout->empty();
}

std::string ReadXkbRegistry() {
  const char *root = getenv("XKB_CONFIG_ROOT");
  std::string path = std::string(root && *root ? root : "/usr/share/X11/xkb") + "/rules/evdev.xml";
  std::string result;
  FILE *file = fopen(path.c_str(), "re");
  if (!file) {
    return result;
  }
  char buffer[65536];
  size_t read;
  while ((read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
    result.append(buffer, read);
  }
  fclose(file);
  return result;
}

// Names the layouts of the keymap, in the terms of `setxkbmap`, from the
// names of its groups.
void GetLayoutsFromGroupNames(const std::string &keymap_text, std::string *layout, std::string *variant) {
  struct xkb_context *context = NULL;
  struct xkb_keymap *keymap = CompileKeymap(&context, keymap_text);
  if (keymap) {
    std::string registry = ReadXkbRegistry();
    std::vector<std::string> layouts;
    std::vector<std::string> variants;
    xkb_layout_index_t count = xkb_keymap_num_layouts(keymap);
    for (xkb_layout_index_t group = 0; group < count && group < 4; group++) {
      std::string group_layout;
      std::string group_variant;
      LookupLayout(registry, xkb_keymap_layout_get_name(keymap, group), &group_layout, &group_variant);
      layouts.push_back(group_layout);
      variants.push_back(group_variant);
    }
    *layout = Join(layouts);
    *variant = Join(variants);
    xkb_keymap_unref(keymap);
  }
  if (context) {
    xkb_context_unref(context);
  }
}

const char *GetEnvOr(const char *name, const char *fallback) {
  const char *value = getenv(name);
  return value && *value ? value : fallback;
}

// The keymap of the compositor and its group, or false when there is none.
bool ReadKeymap(std::string *keymap, uint32_t *group) {
  WaylandState state;
  bool connected = Connect(&state);
  *keymap = state.keymap;
  *group = state.group;
  Disconnect(&state);
  return connected;
}

}  // namespace

namespace vscode_keyboard {

napi_value GetKeyMapWayland(napi_env env, const KeycodeMapEntry *map, size_t count, bool *handled) {
  *handled = false;
  if (GetBackend() == Backend::X11) {
    return NULL;
  }

  std::string keymap_text;
  uint32_t group = 0;
  bool connected = ReadKeymap(&keymap_text, &group);
  if (!connected && GetBackend() != Backend::WaylandOnly) {
    // The compositor went away, for example in a nested session.
    return NULL;
  }
  *handled = true;

  napi_value result;
  NAPI_CALL(env, napi_create_object(env, &result));
  if (!connected) {
    return result;
  }

  struct xkb_context *context = NULL;
  struct xkb_keymap *keymap = CompileKeymap(&context, keymap_text);
  struct xkb_state *state = keymap ? xkb_state_new(keymap) : NULL;
  if (!state) {
    if (keymap) {
      xkb_keymap_unref(keymap);
    }
    if (context) {
      xkb_context_unref(context);
    }
    return result;
  }
  if (group >= xkb_keymap_num_layouts(keymap)) {
    group = 0;
  }

  xkb_mod_index_t shift_index = xkb_keymap_mod_get_index(keymap, XKB_MOD_NAME_SHIFT);
  xkb_mod_mask_t shift = shift_index == XKB_MOD_INVALID ? 0 : (1u << shift_index);
  xkb_mod_mask_t level3 = FindModifierOfKeysym(keymap, XKB_KEY_ISO_Level3_Shift, group);
  xkb_mod_mask_t level5 = FindModifierOfKeysym(keymap, XKB_KEY_ISO_Level5_Shift, group);

  // The same modifier combinations, in the same order, as the X11 backend.
  const char *const names[] = { "value", "withShift", "withAltGr", "withShiftAltGr", "withLevel5", "withLevel3Level5" };
  const xkb_mod_mask_t combinations[] = { 0, shift, level3, shift | level3, level5, level3 | level5 };
  const size_t num_combinations = sizeof(combinations) / sizeof(combinations[0]);

  std::vector<size_t> entries;
  std::vector<std::string> values;
  for (size_t i = 0; i < count; i++) {
    // The keycodes of Chromium's xkb column are evdev codes + 8, as in xkbcommon.
    if (!map[i].code || map[i].native_keycode <= 0) {
      continue;
    }
    xkb_keycode_t keycode = static_cast<xkb_keycode_t>(map[i].native_keycode);
    entries.push_back(i);
    for (size_t j = 0; j < num_combinations; j++) {
      xkb_state_update_mask(state, combinations[j], 0, 0, 0, 0, group);
      values.push_back(KeysymToUtf8(xkb_state_key_get_one_sym(state, keycode)));
    }
  }

  xkb_state_unref(state);
  xkb_keymap_unref(keymap);
  xkb_context_unref(context);

  for (size_t e = 0; e < entries.size(); e++) {
    napi_value entry;
    NAPI_CALL(env, napi_create_object(env, &entry));
    for (size_t j = 0; j < num_combinations; j++) {
      NAPI_CALL(env, napi_set_named_property_string_utf8(env, entry, names[j], values[e * num_combinations + j].c_str()));
    }
    NAPI_CALL(env, napi_set_named_property(env, result, map[entries[e]].code, entry));
  }

  return result;
}

napi_value GetCurrentKeyboardLayoutWayland(napi_env env, bool *handled) {
  *handled = false;
  if (GetBackend() == Backend::X11) {
    return NULL;
  }

  std::string keymap_text;
  uint32_t group = 0;
  bool connected = ReadKeymap(&keymap_text, &group);
  if (!connected && GetBackend() != Backend::WaylandOnly) {
    return NULL;
  }
  *handled = true;

  napi_value result;
  NAPI_CALL(env, napi_get_null(env, &result));
  if (!connected) {
    return result;
  }

  std::string layout;
  std::string variant;
  ParseSymbols(GetSectionName(keymap_text, "xkb_symbols"), &layout, &variant);
  if (layout.empty()) {
    // Compositors that compile their keymap with xkbcommon don't name its sections.
    GetLayoutsFromGroupNames(keymap_text, &layout, &variant);
  }
  // wl_keyboard keycodes are evdev keycodes.
  const char *rules = "evdev";

  NAPI_CALL(env, napi_create_object(env, &result));
  // The keymap does not say which model and options it was compiled from.
  NAPI_CALL(env, napi_set_named_property_string_utf8(env, result, "model", GetEnvOr("XKB_DEFAULT_MODEL", "pc105")));
  NAPI_CALL(env, napi_set_named_property_int32(env, result, "group", static_cast<int>(group)));
  NAPI_CALL(env, napi_set_named_property_string_utf8(env, result, "layout", layout.c_str()));
  NAPI_CALL(env, napi_set_named_property_string_utf8(env, result, "variant", variant.c_str()));
  NAPI_CALL(env, napi_set_named_property_string_utf8(env, result, "options", GetEnvOr("XKB_DEFAULT_OPTIONS", "")));
  NAPI_CALL(env, napi_set_named_property_string_utf8(env, result, "rules", rules));
  return result;
}

static void CleanupListener(void *arg) {
  WaylandState *state = static_cast<WaylandState*>(arg);
  if (state->read_prepared) {
    wl_display_cancel_read(state->display);
    state->read_prepared = false;
  }
  Disconnect(state);
  delete state;
}

bool ListenToWaylandEvents(NotificationCallbackData *data) {
  // libwayland must not be interrupted while it holds its locks: only allow
  // cancellation while waiting for events (see DispatchOnce).
  int old_state;
  pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &old_state);
  if (GetBackend() == Backend::X11) {
    pthread_setcancelstate(old_state, NULL);
    return false;
  }

  bool listened = false;
  WaylandState *state = new WaylandState();
  pthread_cleanup_push(CleanupListener, state);

  if (Connect(state)) {
    listened = true;
    state->changed = false;
    while (true) {
      if (DispatchOnce(state, 1000, true) < 0) {
        break;
      }
      if (state->changed) {
        state->changed = false;
        InvokeNotificationCallback(data);
      }
    }
  }

  pthread_cleanup_pop(1);
  pthread_setcancelstate(old_state, NULL);
  // When no compositor could be reached, listen to the X server instead, unless
  // only the compositor is wanted.
  return listened || GetBackend() == Backend::WaylandOnly;
}

}  // namespace vscode_keyboard
