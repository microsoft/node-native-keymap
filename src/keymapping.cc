/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Microsoft Corporation. All rights reserved.
 *  Licensed under the MIT License. See License.txt in the project root for license information.
 *--------------------------------------------------------------------------------------------*/

#include <node.h>
#include <map>

#include "keymapping.h"
#include "common.h"

namespace vscode_keyboard {

napi_value Init(napi_env env, napi_value exports) {
  {
    napi_value getKeyMap;
    NAPI_CALL(env, napi_create_function(env, NULL, 0, _GetKeyMap, NULL, &getKeyMap));
    NAPI_CALL(env, napi_set_named_property(env, exports, "getKeyMap", getKeyMap));
  }
  {
    napi_value getCurrentKeyboardLayout;
    NAPI_CALL(env, napi_create_function(env, NULL, 0, _GetCurrentKeyboardLayout, NULL, &getCurrentKeyboardLayout));
    NAPI_CALL(env, napi_set_named_property(env, exports, "getCurrentKeyboardLayout", getCurrentKeyboardLayout));
  }
  {
    napi_value onDidChangeKeyboardLayout;
    NAPI_CALL(env, napi_create_function(env, NULL, 0, _OnDidChangeKeyboardLayout, NULL, &onDidChangeKeyboardLayout));
    NAPI_CALL(env, napi_set_named_property(env, exports, "onDidChangeKeyboardLayout", onDidChangeKeyboardLayout));
  }
  {
    napi_value isISOKeyboard;
    NAPI_CALL(env, napi_create_function(env, NULL, 0, _isISOKeyboard, NULL, &isISOKeyboard));
    NAPI_CALL(env, napi_set_named_property(env, exports, "isISOKeyboard", isISOKeyboard));
  }

  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)

} // namespace vscode_keyboard
