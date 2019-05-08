/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Microsoft Corporation. All rights reserved.
 *  Licensed under the MIT License. See License.txt in the project root for license information.
 *--------------------------------------------------------------------------------------------*/

#include "keymapping.h"

namespace vscode_keyboard {

NAN_MODULE_INIT(Init) {
  Nan::SetMethod(target, "getKeyMap", GetKeyMap);
  Nan::SetMethod(target, "getCurrentKeyboardLayout", GetCurrentKeyboardLayout);
  Nan::SetMethod(target, "onDidChangeKeyboardLayout", OnDidChangeKeyboardLayout);
  Nan::SetMethod(target, "isISOKeyboard", IsISOKeyboard);
}

NODE_MODULE(addon, Init)

} // namespace vscode_keyboard
