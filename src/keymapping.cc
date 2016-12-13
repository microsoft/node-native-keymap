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
  ui::KeyboardCode key_code = it->key_code;

  std::map<int, std::string>::const_iterator to_string_it = ui::gKeyboardCodeToStringMap.find(key_code);
  if (to_string_it == ui::gKeyboardCodeToStringMap.end()) {
    // unknown key code?
    return;
  }
  AddEntry(isolate, result, to_string_it->second, keyValue, withShift, withAltGr, withShiftAltGr);
}

void GetKeyMap(const FunctionCallbackInfo<Value>& args) {
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

void init(Local<Object> exports) {
  NODE_SET_METHOD(exports, "getKeyMap", GetKeyMap);
}

NODE_MODULE(addon, init)

} // namespace vscode_keyboard
