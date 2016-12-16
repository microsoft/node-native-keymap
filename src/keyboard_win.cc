/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Microsoft Corporation. All rights reserved.
 *  Licensed under the MIT License. See License.txt in the project root for license information.
 *--------------------------------------------------------------------------------------------*/

#include "keymapping.h"

#include "../deps/chromium/macros.h"

#include "string_conversion.h"
#include <windows.h>
#include <ime.h>

namespace {

void ClearKeyboardBuffer(UINT key_code, UINT scan_code, BYTE* keyboard_state) {
  memset(keyboard_state, 0, 256);

  wchar_t chars[5];
  int code = 0;
  do {
    code = ::ToUnicode(key_code, scan_code, keyboard_state, chars, 4, 0);
  } while(code < 0);
}

std::string GetStrFromKeyPress(UINT key_code, int modifiers, BYTE *keyboard_state, UINT clear_key_code, UINT clear_scan_code) {
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

using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::Object;
using v8::String;
using v8::Array;
using v8::Value;
using v8::Null;

#define USB_KEYMAP(usb, evdev, xkb, win, mac, code, id) {usb, win, code}
#define USB_KEYMAP_DECLARATION const KeycodeMapEntry usb_keycode_map[] =
#include "../deps/chromium/keycode_converter_data.inc"
#undef USB_KEYMAP
#undef USB_KEYMAP_DECLARATION

void _GetKeyMap(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  Local<Object> result = Object::New(isolate);
  Local<String> _value = String::NewFromUtf8(isolate, "value");
  Local<String> _withShift = String::NewFromUtf8(isolate, "withShift");
  Local<String> _withAltGr = String::NewFromUtf8(isolate, "withAltGr");
  Local<String> _withShiftAltGr = String::NewFromUtf8(isolate, "withShiftAltGr");

  UINT clear_key_code = VK_DECIMAL;
  UINT clear_scan_code = ::MapVirtualKeyW(clear_key_code, MAPVK_VK_TO_VSC);
  BYTE keyboard_state[256];

  size_t cnt = sizeof(usb_keycode_map) / sizeof(usb_keycode_map[0]);
  for (size_t i = 0; i < cnt; ++i) {
    const char *code = usb_keycode_map[i].code;
    int native_scancode = usb_keycode_map[i].native_keycode;

    if (!code || native_scancode <= 0) {
      continue;
    }

    int native_keycode = ::MapVirtualKeyW(native_scancode, MAPVK_VSC_TO_VK);

    Local<Object> entry = Object::New(isolate);

    std::string value = GetStrFromKeyPress(native_keycode, 0, keyboard_state, clear_key_code, clear_scan_code);
    entry->Set(_value, String::NewFromUtf8(isolate, value.c_str()));

    std::string withShift = GetStrFromKeyPress(native_keycode, kShiftKeyModifierMask, keyboard_state, clear_key_code, clear_scan_code);
    entry->Set(_withShift, String::NewFromUtf8(isolate, withShift.c_str()));

    std::string withAltGr = GetStrFromKeyPress(native_keycode, kControlKeyModifierMask | kAltKeyModifierMask, keyboard_state, clear_key_code, clear_scan_code);
    entry->Set(_withAltGr, String::NewFromUtf8(isolate, withAltGr.c_str()));

    std::string withShiftAltGr = GetStrFromKeyPress(native_keycode, kShiftKeyModifierMask | kControlKeyModifierMask | kAltKeyModifierMask, keyboard_state, clear_key_code, clear_scan_code);
    entry->Set(_withShiftAltGr, String::NewFromUtf8(isolate, withShiftAltGr.c_str()));

    result->Set(String::NewFromUtf8(isolate, code), entry);
  }
  args.GetReturnValue().Set(result);
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
  result->Set(String::NewFromUtf8(isolate, "name"), String::NewFromUtf8(isolate, layout_name.c_str()));
  result->Set(String::NewFromUtf8(isolate, "id"), String::NewFromUtf8(isolate, layout_id.c_str()));
  result->Set(String::NewFromUtf8(isolate, "text"), String::NewFromUtf8(isolate, layout_text.c_str()));
  args.GetReturnValue().Set(result);
}

}  // namespace vscode_keyboard
