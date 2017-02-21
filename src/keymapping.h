/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Microsoft Corporation. All rights reserved.
 *  Licensed under the MIT License. See License.txt in the project root for license information.
 *--------------------------------------------------------------------------------------------*/

#ifndef KEYMAPPING_H_
#define KEYMAPPING_H_

#include <node.h>
#include <string>
#include <vector>
#include "../deps/chromium/keyboard_codes.h"

namespace vscode_keyboard {

struct KeyMapping {
  ui::KeyboardCode key_code;
  std::string value;
  std::string withShift;
  std::string withAltGr;
  std::string withShiftAltGr;
};
typedef struct KeyMapping KeyMapping;

std::vector<KeyMapping> GetKeyMapping();
void _GetCurrentKeyboardLayout(const v8::FunctionCallbackInfo<v8::Value>& args);

}  // namespace vscode_keyboard

#endif  // KEYMAPPING_H_