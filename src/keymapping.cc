/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Microsoft Corporation. All rights reserved.
 *  Licensed under the MIT License. See License.txt in the project root for license information.
 *--------------------------------------------------------------------------------------------*/

#include <node.h>
#include <map>

#include "keymapping.h"

namespace vscode_keyboard {

using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::Object;
using v8::String;
using v8::Array;
using v8::Value;

void init(Local<Object> exports) {
  NODE_SET_METHOD(exports, "getKeyMap", _GetKeyMap);
  NODE_SET_METHOD(exports, "getCurrentKeyboardLayout", _GetCurrentKeyboardLayout);
  NODE_SET_METHOD(exports, "onDidChangeKeyboardLayout", _OnDidChangeKeyboardLayout);
  NODE_SET_METHOD(exports, "isISOKeyboard", _isISOKeyboard);
}

NODE_MODULE(addon, init)

} // namespace vscode_keyboard
