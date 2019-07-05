/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Microsoft Corporation. All rights reserved.
 *  Licensed under the MIT License. See License.txt in the project root for license information.
 *--------------------------------------------------------------------------------------------*/

#include <node.h>
#include <map>

#include "keymapping.h"
#include "common.h"

namespace vscode_keyboard {

napi_status napi_set_named_property_string_utf8(napi_env env, napi_value object, const char *utf8Name, const char *value) {
  napi_value _value;
  NAPI_CALL_RETURN_STATUS(env, napi_create_string_utf8(env, value, NAPI_AUTO_LENGTH, &_value));
  NAPI_CALL_RETURN_STATUS(env, napi_set_named_property(env, object, utf8Name, _value));
  return napi_ok;
}

napi_value napi_fetch_null(napi_env env) {
  napi_value result;
  NAPI_CALL(env, napi_get_null(env, &result));
  return result;
}

napi_value napi_fetch_undefined(napi_env env) {
  napi_value result;
  NAPI_CALL(env, napi_get_undefined(env, &result));
  return result;
}

napi_value napi_fetch_boolean(napi_env env, bool value) {
  napi_value result;
  NAPI_CALL(env, napi_get_boolean(env, value, &result));
  return result;
}

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
