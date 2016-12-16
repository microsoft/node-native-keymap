/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Microsoft Corporation. All rights reserved.
 *  Licensed under the MIT License. See License.txt in the project root for license information.
 *--------------------------------------------------------------------------------------------*/

#include "keymapping.h"

#include "../deps/chromium/macros.h"

#include "string_conversion.h"

namespace {

void ClearKeyboardBuffer(ui::KeyboardCode key_code, UINT scan_code, BYTE* keyboard_state) {
  memset(keyboard_state, 0, 256);

  wchar_t chars[5];
  int code = 0;
  do {
    code = ::ToUnicode(key_code, scan_code, keyboard_state, chars, 4, 0);
  } while(code < 0);
}

std::string GetStrFromKeyPress(ui::KeyboardCode key_code, int modifiers, BYTE *keyboard_state, ui::KeyboardCode clear_key_code, UINT clear_scan_code) {
  memset(keyboard_state, 0, 256);

  if (modifiers & kShiftKeyModifierMask) {
    keyboard_state[VK_SHIFT] |= 0x80;
  }

  if (modifiers & kControlKeyModifierMask) {
    keyboard_state[VK_CONTROL] |= 0x80;
  }

  if (modifiers & kAltKeyModifierMask) {
    keyboard_state[VK_MENU] |= 0x80;
  }

  UINT scan_code = ::MapVirtualKeyW(key_code, MAPVK_VK_TO_VSC);

  wchar_t chars[5];
  int code = ::ToUnicode(key_code, scan_code, keyboard_state, chars, 4, 0);
  ClearKeyboardBuffer(clear_key_code, clear_scan_code, keyboard_state);

  if (code <= 0 || (code == 1 && iswcntrl(chars[0]))) {
    return std::string();
  }

  return vscode_keyboard::UTF16toUTF8(chars, code);
}

} // namespace

namespace vscode_keyboard {

std::vector<KeyMapping> GetKeyMapping() {
  std::vector<KeyMapping> result;

  ui::KeyboardCode clear_key_code = ui::KeyboardCode::VKEY_DECIMAL;
  UINT clear_scan_code = ::MapVirtualKeyW(clear_key_code, MAPVK_VK_TO_VSC);
  BYTE keyboard_state[256];

  size_t cnt = sizeof(ui::gKeyboardCodeToStringMap) / sizeof(ui::gKeyboardCodeToStringMap[0]);
  for (size_t i = 0; i < cnt; ++i) {
    ui::KeyboardCode key_code = ui::gKeyboardCodeToStringMap[i].first;

    std::string value = GetStrFromKeyPress(key_code, 0, keyboard_state, clear_key_code, clear_scan_code);
    std::string withShift = GetStrFromKeyPress(key_code, kShiftKeyModifierMask, keyboard_state, clear_key_code, clear_scan_code);
    std::string withAltGr = GetStrFromKeyPress(key_code, kControlKeyModifierMask | kAltKeyModifierMask, keyboard_state, clear_key_code, clear_scan_code);
    std::string withShiftAltGr = GetStrFromKeyPress(key_code, kShiftKeyModifierMask | kControlKeyModifierMask | kAltKeyModifierMask, keyboard_state, clear_key_code, clear_scan_code);

    KeyMapping keyMapping = KeyMapping();
    keyMapping.key_code = key_code;
    keyMapping.value = value;
    keyMapping.withShift = withShift;
    keyMapping.withAltGr = withAltGr;
    keyMapping.withShiftAltGr = withShiftAltGr;
    result.push_back(keyMapping);
  }

  return result;
}

std::string GetStringRegKey(std::string path, std::string name) {
  std::string result = "";

  HKEY hKey;
  if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE, path.c_str(), 0, KEY_READ, &hKey)) {
    return result;
  }

  char szBuffer[512];
  DWORD dwBufferSize = sizeof(szBuffer);

  if (ERROR_SUCCESS == RegQueryValueEx(hKey, name.c_str(), 0, NULL, (LPBYTE)szBuffer, &dwBufferSize)) {
    result = szBuffer;
  }

  RegCloseKey(hKey);

  return result;
}

using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::Object;
using v8::String;
using v8::Array;
using v8::Value;
using v8::Null;

void _GetCurrentKeyboardLayout(const v8::FunctionCallbackInfo<v8::Value>& args) {
  Isolate* isolate = args.GetIsolate();

  char chr_layout_name[KL_NAMELENGTH];
  if (!GetKeyboardLayoutName(chr_layout_name)) {
    args.GetReturnValue().Set(Null(isolate));
    return;
  }
  std::string layout_name = chr_layout_name;

  // https://technet.microsoft.com/en-us/library/dd744319(v=ws.10).aspx
  std::string layout_id = GetStringRegKey("System\\CurrentControlSet\\Control\\Keyboard Layouts\\" + layout_name, "Layout Id");
  std::string layout_text = GetStringRegKey("System\\CurrentControlSet\\Control\\Keyboard Layouts\\" + layout_name, "Layout Text");

  Local<Object> result = Object::New(isolate);
  result->Set(String::NewFromUtf8(isolate, "name"), String::NewFromUtf8(isolate, layout_name));
  result->Set(String::NewFromUtf8(isolate, "id"), String::NewFromUtf8(isolate, layout_id));
  result->Set(String::NewFromUtf8(isolate, "text"), String::NewFromUtf8(isolate, layout_text));
  args.GetReturnValue().Set(result);
}

}  // namespace vscode_keyboard
