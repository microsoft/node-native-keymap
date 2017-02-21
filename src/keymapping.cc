/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Microsoft Corporation. All rights reserved.
 *  Licensed under the MIT License. See License.txt in the project root for license information.
 *--------------------------------------------------------------------------------------------*/

#include <node.h>

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

  if (key_code == ui::KeyboardCode::VKEY_BACK)
    AddEntry(isolate, result, "VKEY_BACK", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_TAB)
    AddEntry(isolate, result, "VKEY_TAB", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_CLEAR)
    AddEntry(isolate, result, "VKEY_CLEAR", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_RETURN)
    AddEntry(isolate, result, "VKEY_RETURN", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_SHIFT)
    AddEntry(isolate, result, "VKEY_SHIFT", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_CONTROL)
    AddEntry(isolate, result, "VKEY_CONTROL", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_MENU)
    AddEntry(isolate, result, "VKEY_MENU", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_PAUSE)
    AddEntry(isolate, result, "VKEY_PAUSE", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_CAPITAL)
    AddEntry(isolate, result, "VKEY_CAPITAL", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_KANA)
    AddEntry(isolate, result, "VKEY_KANA", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_HANGUL)
    AddEntry(isolate, result, "VKEY_HANGUL", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_JUNJA)
    AddEntry(isolate, result, "VKEY_JUNJA", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_FINAL)
    AddEntry(isolate, result, "VKEY_FINAL", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_HANJA)
    AddEntry(isolate, result, "VKEY_HANJA", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_KANJI)
    AddEntry(isolate, result, "VKEY_KANJI", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_ESCAPE)
    AddEntry(isolate, result, "VKEY_ESCAPE", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_CONVERT)
    AddEntry(isolate, result, "VKEY_CONVERT", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_NONCONVERT)
    AddEntry(isolate, result, "VKEY_NONCONVERT", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_ACCEPT)
    AddEntry(isolate, result, "VKEY_ACCEPT", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_MODECHANGE)
    AddEntry(isolate, result, "VKEY_MODECHANGE", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_SPACE)
    AddEntry(isolate, result, "VKEY_SPACE", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_PRIOR)
    AddEntry(isolate, result, "VKEY_PRIOR", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_NEXT)
    AddEntry(isolate, result, "VKEY_NEXT", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_END)
    AddEntry(isolate, result, "VKEY_END", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_HOME)
    AddEntry(isolate, result, "VKEY_HOME", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_LEFT)
    AddEntry(isolate, result, "VKEY_LEFT", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_UP)
    AddEntry(isolate, result, "VKEY_UP", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_RIGHT)
    AddEntry(isolate, result, "VKEY_RIGHT", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_DOWN)
    AddEntry(isolate, result, "VKEY_DOWN", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_SELECT)
    AddEntry(isolate, result, "VKEY_SELECT", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_PRINT)
    AddEntry(isolate, result, "VKEY_PRINT", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_EXECUTE)
    AddEntry(isolate, result, "VKEY_EXECUTE", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_SNAPSHOT)
    AddEntry(isolate, result, "VKEY_SNAPSHOT", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_INSERT)
    AddEntry(isolate, result, "VKEY_INSERT", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_DELETE)
    AddEntry(isolate, result, "VKEY_DELETE", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_HELP)
    AddEntry(isolate, result, "VKEY_HELP", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_0)
    AddEntry(isolate, result, "VKEY_0", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_1)
    AddEntry(isolate, result, "VKEY_1", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_2)
    AddEntry(isolate, result, "VKEY_2", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_3)
    AddEntry(isolate, result, "VKEY_3", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_4)
    AddEntry(isolate, result, "VKEY_4", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_5)
    AddEntry(isolate, result, "VKEY_5", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_6)
    AddEntry(isolate, result, "VKEY_6", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_7)
    AddEntry(isolate, result, "VKEY_7", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_8)
    AddEntry(isolate, result, "VKEY_8", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_9)
    AddEntry(isolate, result, "VKEY_9", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_A)
    AddEntry(isolate, result, "VKEY_A", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_B)
    AddEntry(isolate, result, "VKEY_B", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_C)
    AddEntry(isolate, result, "VKEY_C", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_D)
    AddEntry(isolate, result, "VKEY_D", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_E)
    AddEntry(isolate, result, "VKEY_E", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_F)
    AddEntry(isolate, result, "VKEY_F", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_G)
    AddEntry(isolate, result, "VKEY_G", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_H)
    AddEntry(isolate, result, "VKEY_H", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_I)
    AddEntry(isolate, result, "VKEY_I", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_J)
    AddEntry(isolate, result, "VKEY_J", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_K)
    AddEntry(isolate, result, "VKEY_K", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_L)
    AddEntry(isolate, result, "VKEY_L", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_M)
    AddEntry(isolate, result, "VKEY_M", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_N)
    AddEntry(isolate, result, "VKEY_N", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_O)
    AddEntry(isolate, result, "VKEY_O", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_P)
    AddEntry(isolate, result, "VKEY_P", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_Q)
    AddEntry(isolate, result, "VKEY_Q", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_R)
    AddEntry(isolate, result, "VKEY_R", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_S)
    AddEntry(isolate, result, "VKEY_S", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_T)
    AddEntry(isolate, result, "VKEY_T", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_U)
    AddEntry(isolate, result, "VKEY_U", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_V)
    AddEntry(isolate, result, "VKEY_V", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_W)
    AddEntry(isolate, result, "VKEY_W", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_X)
    AddEntry(isolate, result, "VKEY_X", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_Y)
    AddEntry(isolate, result, "VKEY_Y", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_Z)
    AddEntry(isolate, result, "VKEY_Z", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_LWIN)
    AddEntry(isolate, result, "VKEY_LWIN", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_COMMAND)
    AddEntry(isolate, result, "VKEY_COMMAND", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_RWIN)
    AddEntry(isolate, result, "VKEY_RWIN", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_APPS)
    AddEntry(isolate, result, "VKEY_APPS", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_SLEEP)
    AddEntry(isolate, result, "VKEY_SLEEP", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_NUMPAD0)
    AddEntry(isolate, result, "VKEY_NUMPAD0", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_NUMPAD1)
    AddEntry(isolate, result, "VKEY_NUMPAD1", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_NUMPAD2)
    AddEntry(isolate, result, "VKEY_NUMPAD2", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_NUMPAD3)
    AddEntry(isolate, result, "VKEY_NUMPAD3", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_NUMPAD4)
    AddEntry(isolate, result, "VKEY_NUMPAD4", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_NUMPAD5)
    AddEntry(isolate, result, "VKEY_NUMPAD5", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_NUMPAD6)
    AddEntry(isolate, result, "VKEY_NUMPAD6", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_NUMPAD7)
    AddEntry(isolate, result, "VKEY_NUMPAD7", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_NUMPAD8)
    AddEntry(isolate, result, "VKEY_NUMPAD8", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_NUMPAD9)
    AddEntry(isolate, result, "VKEY_NUMPAD9", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_MULTIPLY)
    AddEntry(isolate, result, "VKEY_MULTIPLY", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_ADD)
    AddEntry(isolate, result, "VKEY_ADD", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_SEPARATOR)
    AddEntry(isolate, result, "VKEY_SEPARATOR", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_SUBTRACT)
    AddEntry(isolate, result, "VKEY_SUBTRACT", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_DECIMAL)
    AddEntry(isolate, result, "VKEY_DECIMAL", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_DIVIDE)
    AddEntry(isolate, result, "VKEY_DIVIDE", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_F1)
    AddEntry(isolate, result, "VKEY_F1", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_F2)
    AddEntry(isolate, result, "VKEY_F2", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_F3)
    AddEntry(isolate, result, "VKEY_F3", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_F4)
    AddEntry(isolate, result, "VKEY_F4", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_F5)
    AddEntry(isolate, result, "VKEY_F5", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_F6)
    AddEntry(isolate, result, "VKEY_F6", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_F7)
    AddEntry(isolate, result, "VKEY_F7", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_F8)
    AddEntry(isolate, result, "VKEY_F8", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_F9)
    AddEntry(isolate, result, "VKEY_F9", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_F10)
    AddEntry(isolate, result, "VKEY_F10", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_F11)
    AddEntry(isolate, result, "VKEY_F11", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_F12)
    AddEntry(isolate, result, "VKEY_F12", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_F13)
    AddEntry(isolate, result, "VKEY_F13", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_F14)
    AddEntry(isolate, result, "VKEY_F14", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_F15)
    AddEntry(isolate, result, "VKEY_F15", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_F16)
    AddEntry(isolate, result, "VKEY_F16", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_F17)
    AddEntry(isolate, result, "VKEY_F17", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_F18)
    AddEntry(isolate, result, "VKEY_F18", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_F19)
    AddEntry(isolate, result, "VKEY_F19", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_F20)
    AddEntry(isolate, result, "VKEY_F20", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_F21)
    AddEntry(isolate, result, "VKEY_F21", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_F22)
    AddEntry(isolate, result, "VKEY_F22", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_F23)
    AddEntry(isolate, result, "VKEY_F23", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_F24)
    AddEntry(isolate, result, "VKEY_F24", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_NUMLOCK)
    AddEntry(isolate, result, "VKEY_NUMLOCK", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_SCROLL)
    AddEntry(isolate, result, "VKEY_SCROLL", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_LSHIFT)
    AddEntry(isolate, result, "VKEY_LSHIFT", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_RSHIFT)
    AddEntry(isolate, result, "VKEY_RSHIFT", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_LCONTROL)
    AddEntry(isolate, result, "VKEY_LCONTROL", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_RCONTROL)
    AddEntry(isolate, result, "VKEY_RCONTROL", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_LMENU)
    AddEntry(isolate, result, "VKEY_LMENU", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_RMENU)
    AddEntry(isolate, result, "VKEY_RMENU", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_BROWSER_BACK)
    AddEntry(isolate, result, "VKEY_BROWSER_BACK", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_BROWSER_FORWARD)
    AddEntry(isolate, result, "VKEY_BROWSER_FORWARD", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_BROWSER_REFRESH)
    AddEntry(isolate, result, "VKEY_BROWSER_REFRESH", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_BROWSER_STOP)
    AddEntry(isolate, result, "VKEY_BROWSER_STOP", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_BROWSER_SEARCH)
    AddEntry(isolate, result, "VKEY_BROWSER_SEARCH", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_BROWSER_FAVORITES)
    AddEntry(isolate, result, "VKEY_BROWSER_FAVORITES", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_BROWSER_HOME)
    AddEntry(isolate, result, "VKEY_BROWSER_HOME", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_VOLUME_MUTE)
    AddEntry(isolate, result, "VKEY_VOLUME_MUTE", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_VOLUME_DOWN)
    AddEntry(isolate, result, "VKEY_VOLUME_DOWN", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_VOLUME_UP)
    AddEntry(isolate, result, "VKEY_VOLUME_UP", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_MEDIA_NEXT_TRACK)
    AddEntry(isolate, result, "VKEY_MEDIA_NEXT_TRACK", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_MEDIA_PREV_TRACK)
    AddEntry(isolate, result, "VKEY_MEDIA_PREV_TRACK", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_MEDIA_STOP)
    AddEntry(isolate, result, "VKEY_MEDIA_STOP", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_MEDIA_PLAY_PAUSE)
    AddEntry(isolate, result, "VKEY_MEDIA_PLAY_PAUSE", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_MEDIA_LAUNCH_MAIL)
    AddEntry(isolate, result, "VKEY_MEDIA_LAUNCH_MAIL", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_MEDIA_LAUNCH_MEDIA_SELECT)
    AddEntry(isolate, result, "VKEY_MEDIA_LAUNCH_MEDIA_SELECT", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_MEDIA_LAUNCH_APP1)
    AddEntry(isolate, result, "VKEY_MEDIA_LAUNCH_APP1", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_MEDIA_LAUNCH_APP2)
    AddEntry(isolate, result, "VKEY_MEDIA_LAUNCH_APP2", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_OEM_1)
    AddEntry(isolate, result, "VKEY_OEM_1", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_OEM_PLUS)
    AddEntry(isolate, result, "VKEY_OEM_PLUS", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_OEM_COMMA)
    AddEntry(isolate, result, "VKEY_OEM_COMMA", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_OEM_MINUS)
    AddEntry(isolate, result, "VKEY_OEM_MINUS", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_OEM_PERIOD)
    AddEntry(isolate, result, "VKEY_OEM_PERIOD", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_OEM_2)
    AddEntry(isolate, result, "VKEY_OEM_2", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_OEM_3)
    AddEntry(isolate, result, "VKEY_OEM_3", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_OEM_4)
    AddEntry(isolate, result, "VKEY_OEM_4", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_OEM_5)
    AddEntry(isolate, result, "VKEY_OEM_5", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_OEM_6)
    AddEntry(isolate, result, "VKEY_OEM_6", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_OEM_7)
    AddEntry(isolate, result, "VKEY_OEM_7", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_OEM_8)
    AddEntry(isolate, result, "VKEY_OEM_8", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_OEM_102)
    AddEntry(isolate, result, "VKEY_OEM_102", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_PROCESSKEY)
    AddEntry(isolate, result, "VKEY_PROCESSKEY", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_PACKET)
    AddEntry(isolate, result, "VKEY_PACKET", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_DBE_SBCSCHAR)
    AddEntry(isolate, result, "VKEY_DBE_SBCSCHAR", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_DBE_DBCSCHAR)
    AddEntry(isolate, result, "VKEY_DBE_DBCSCHAR", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_ATTN)
    AddEntry(isolate, result, "VKEY_ATTN", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_CRSEL)
    AddEntry(isolate, result, "VKEY_CRSEL", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_EXSEL)
    AddEntry(isolate, result, "VKEY_EXSEL", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_EREOF)
    AddEntry(isolate, result, "VKEY_EREOF", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_PLAY)
    AddEntry(isolate, result, "VKEY_PLAY", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_ZOOM)
    AddEntry(isolate, result, "VKEY_ZOOM", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_NONAME)
    AddEntry(isolate, result, "VKEY_NONAME", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_PA1)
    AddEntry(isolate, result, "VKEY_PA1", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_OEM_CLEAR)
    AddEntry(isolate, result, "VKEY_OEM_CLEAR", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_UNKNOWN)
    AddEntry(isolate, result, "VKEY_UNKNOWN", keyValue, withShift, withAltGr, withShiftAltGr);
  if (key_code == ui::KeyboardCode::VKEY_ALTGR)
    AddEntry(isolate, result, "VKEY_ALTGR", keyValue, withShift, withAltGr, withShiftAltGr);
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
  NODE_SET_METHOD(exports, "getCurrentKeyboardLayout", _GetCurrentKeyboardLayout);
}

NODE_MODULE(addon, init)

} // namespace vscode_keyboard
