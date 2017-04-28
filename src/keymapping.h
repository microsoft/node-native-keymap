/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Microsoft Corporation. All rights reserved.
 *  Licensed under the MIT License. See License.txt in the project root for license information.
 *--------------------------------------------------------------------------------------------*/

#ifndef KEYMAPPING_H_
#define KEYMAPPING_H_

#include <node.h>
#include <uv.h>
#include <string>
#include <vector>
#include "../deps/chromium/keyboard_codes.h"

namespace vscode_keyboard {

// This structure is used to define the keycode mapping table.
// It is defined here because the unittests need access to it.
typedef struct {
  // USB keycode:
  //  Upper 16-bits: USB Usage Page.
  //  Lower 16-bits: USB Usage Id: Assigned ID within this usage page.
  uint32_t usb_keycode;

  // Contains one of the following:
  //  On Linux: XKB scancode
  //  On Windows: Windows OEM scancode
  //  On Mac: Mac keycode
  int native_keycode;

  // The UIEvents (aka: DOM4Events) |code| value as defined in:
  // http://www.w3.org/TR/DOM-Level-3-Events-code/
  const char* code;
} KeycodeMapEntry;

void _GetKeyMap(const v8::FunctionCallbackInfo<v8::Value>& args);
void _GetCurrentKeyboardLayout(const v8::FunctionCallbackInfo<v8::Value>& args);
void _OnDidChangeKeyboardLayout(const v8::FunctionCallbackInfo<v8::Value>& args);
void _isISOKeyboard(const v8::FunctionCallbackInfo<v8::Value>& args);

}  // namespace vscode_keyboard

#endif  // KEYMAPPING_H_