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
// The backend is chosen with the NATIVE_KEYMAP_BACKEND environment variable:
//  - unset or `auto`: use the compositor when WAYLAND_DISPLAY is set and it
//    provides a keymap, otherwise the X server.
//  - `wayland`: only use the compositor.
//  - `x11`: only use the X server.
//
// Each function sets `*handled` to false when the caller should use the X server.

napi_value GetKeyMapWayland(napi_env env, const KeycodeMapEntry *map, size_t count, bool *handled);

napi_value GetCurrentKeyboardLayoutWayland(napi_env env, bool *handled);

// Runs on the listener thread until the thread is cancelled or the connection
// to the compositor is lost. Returns false right away when the caller should
// listen to the X server instead.
bool ListenToWaylandEvents(NotificationCallbackData *data);

}  // namespace vscode_keyboard

#endif  // KEYBOARD_WAYLAND_H_
