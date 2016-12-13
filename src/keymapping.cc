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

void AddEntry(Isolate* isolate, std::vector<Local<Object>> &result, std::string key_code, Local<String> value, Local<String> withShift, Local<String> withAltGr, Local<String> withShiftAltGr) {
  Local<Object> entry = Object::New(isolate);

  entry->Set(String::NewFromUtf8(isolate, "key_code"), String::NewFromUtf8(isolate, key_code.c_str()));
  entry->Set(String::NewFromUtf8(isolate, "value"), value);
  entry->Set(String::NewFromUtf8(isolate, "withShift"), withShift);
  entry->Set(String::NewFromUtf8(isolate, "withAltGr"), withAltGr);
  entry->Set(String::NewFromUtf8(isolate, "withShiftAltGr"), withShiftAltGr);

  result.push_back(entry);
}

void GenerateEntries(Isolate* isolate, std::vector<Local<Object>> &result, std::vector<KeyMapping>::iterator it) {
  if (it->value.length() == 0 && it->withShift.length() == 0 && it->withAltGr.length() == 0 && it->withShiftAltGr.length() == 0) {
    return;
  }
  Local<String> keyValue = String::NewFromUtf8(isolate, it->value.c_str());
  Local<String> withShift = String::NewFromUtf8(isolate, it->withShift.c_str());
  Local<String> withAltGr = String::NewFromUtf8(isolate, it->withAltGr.c_str());
  Local<String> withShiftAltGr = String::NewFromUtf8(isolate, it->withShiftAltGr.c_str());
  std::string keyCode = KeyboardCodeToString::GetInstance().ToString(it->key_code);
  if (keyCode == "") {
    // unknown key code?
    return;
  }
  AddEntry(isolate, result, keyCode, keyValue, withShift, withAltGr, withShiftAltGr);
}

void _GetKeyMap(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();

  std::vector<KeyMapping> mapping = GetKeyMapping();

  std::vector<Local<Object>> result;
  for(std::vector<KeyMapping>::iterator it = mapping.begin(); it != mapping.end(); ++it) {
    GenerateEntries(isolate, result, it);
  }

  int resultCount = (int)result.size();
  Local<Array> resultArr = Array::New(isolate, resultCount);
  for (int index = 0; index < resultCount; index++) {
    resultArr->Set(index, result[index]);
  }

  args.GetReturnValue().Set(resultArr);
}

void _GetCurrentKeyboardLayoutName(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();

  std::string keyboard_layout_name = GetCurrentKeyboardLayoutName();

  args.GetReturnValue().Set(String::NewFromUtf8(isolate, keyboard_layout_name.c_str()));
}

void init(Local<Object> exports) {
  NODE_SET_METHOD(exports, "getKeyMap", _GetKeyMap);
  NODE_SET_METHOD(exports, "getCurrentKeyboardLayoutName", _GetCurrentKeyboardLayoutName);
}

NODE_MODULE(addon, init)

} // namespace vscode_keyboard
