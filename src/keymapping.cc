/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Microsoft Corporation. All rights reserved.
 *  Licensed under the MIT License. See License.txt in the project root for license information.
 *--------------------------------------------------------------------------------------------*/

#include <node.h>
#include <map>

#include "keymapping.h"

namespace vscode_keyboard {

class KeyboardCodeToString {
public:
  static KeyboardCodeToString& GetInstance() {
    static KeyboardCodeToString instance;
    return instance;
  }

  std::string ToString(ui::KeyboardCode key_code) {
    std::map<int, std::string>::const_iterator it = keyboardCodeToStringMap.find(key_code);
    if (it == keyboardCodeToStringMap.end()) {
      // unknown key code?
      return "";
    }

    return it->second;
  }

private:
  std::map<int, std::string> keyboardCodeToStringMap;

  KeyboardCodeToString() : keyboardCodeToStringMap() {
    size_t cnt = sizeof(ui::gKeyboardCodeToStringMap) / sizeof(ui::gKeyboardCodeToStringMap[0]);
    for (size_t i = 0; i < cnt; ++i) {
      keyboardCodeToStringMap[ui::gKeyboardCodeToStringMap[i].first] = ui::gKeyboardCodeToStringMap[i].second;
    }
  }
};

using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::Object;
using v8::String;
using v8::Array;
using v8::Value;

// void _GetKeyMap(const FunctionCallbackInfo<Value>& args) {
//   Isolate* isolate = args.GetIsolate();
//   Local<String> _keyCode = String::NewFromUtf8(isolate, "keyCode");
//   Local<String> _value = String::NewFromUtf8(isolate, "value");
//   Local<String> _withShift = String::NewFromUtf8(isolate, "withShift");
//   Local<String> _withAltGr = String::NewFromUtf8(isolate, "withAltGr");
//   Local<String> _withShiftAltGr = String::NewFromUtf8(isolate, "withShiftAltGr");

//   std::vector<KeyMapping> mapping = GetKeyMapping();

//   std::vector<Local<Object>> result;
//   for(std::vector<KeyMapping>::iterator it = mapping.begin(); it != mapping.end(); ++it) {
//     if (it->value.length() == 0 && it->withShift.length() == 0 && it->withAltGr.length() == 0 && it->withShiftAltGr.length() == 0) {
//       continue;
//     }

//     std::string keyCode = KeyboardCodeToString::GetInstance().ToString(it->key_code);
//     if (keyCode == "") {
//       // unknown key code?
//       continue;
//     }

//     Local<Object> entry = Object::New(isolate);

//     entry->Set(_keyCode, String::NewFromUtf8(isolate, keyCode.c_str()));
//     entry->Set(_value, String::NewFromUtf8(isolate, it->value.c_str()));
//     entry->Set(_withShift, String::NewFromUtf8(isolate, it->withShift.c_str()));
//     entry->Set(_withAltGr, String::NewFromUtf8(isolate, it->withAltGr.c_str()));
//     entry->Set(_withShiftAltGr, String::NewFromUtf8(isolate, it->withShiftAltGr.c_str()));

//     result.push_back(entry);
//   }

//   int resultCount = (int)result.size();
//   Local<Array> resultArr = Array::New(isolate, resultCount);
//   for (int index = 0; index < resultCount; index++) {
//     resultArr->Set(index, result[index]);
//   }

//   args.GetReturnValue().Set(resultArr);
// }

void init(Local<Object> exports) {
  NODE_SET_METHOD(exports, "getKeyMap", _GetKeyMap);
  NODE_SET_METHOD(exports, "getCurrentKeyboardLayout", _GetCurrentKeyboardLayout);
}

NODE_MODULE(addon, init)

} // namespace vscode_keyboard
