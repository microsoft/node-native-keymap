/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Microsoft Corporation. All rights reserved.
 *  Licensed under the MIT License. See License.txt in the project root for license information.
 *--------------------------------------------------------------------------------------------*/

#ifndef KEYBOARD_WAYLAND_H_
#define KEYBOARD_WAYLAND_H_

#include "keymapping.h"

namespace vscode_keyboard {

// The functions below read the keyboard layout from the Wayland compositor,
// which is the only one that knows it in a Wayland session: XWayland's keymap
// and group are only updated while an X11 window has keyboard focus, and its
// _XKB_RULES_NAMES are never updated.
//
// The default backend is chosen with the NATIVE_KEYMAP_BACKEND environment variable:
//  - unset or `auto`: use the compositor when WAYLAND_DISPLAY is set and it
//    provides a keymap, otherwise the X server. A client without a surface
//    gets the compositor's keymap but not its active layout, so a keymap with
//    several layouts is read from the X server when one is reachable: it
//    tracks the active layout for the focused XWayland window.
//  - `wayland`: only use the compositor.
//  - `x11`: only use the X server.
//
// With `only`, the getters read the compositor whatever the default is: the
// caller asked for the Wayland backend. Otherwise they set `*handled` to false
// when the caller should use the X server.

napi_value GetKeyMapWayland(napi_env env, const KeycodeMapEntry *map, size_t count, bool only, bool *handled);

napi_value GetCurrentKeyboardLayoutWayland(napi_env env, bool only, bool *handled);

// Whether the compositor provides a keymap, which is when the getters return
// one with `only`, and whether the default backend reads it.
void GetWaylandStatus(bool *available, bool *is_default);

// Whether the X server should be listened to as well, because the getters
// can read it by default.
bool ShouldListenToX11();

// Runs on its own thread until the thread is cancelled, and notifies keymap
// changes of the compositor. When the connection is lost, it notifies (the
// functions above now read the X server) and connects again every second.
// Returns right away when the compositor is not used.
void ListenToWaylandEvents(NotificationCallbackData *data);

// Whether an X server can be reached. Implemented in keyboard_x.cc.
bool IsXServerAvailable();

}  // namespace vscode_keyboard

#endif  // KEYBOARD_WAYLAND_H_
