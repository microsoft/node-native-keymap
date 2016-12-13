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

std::string GetCurrentKeyboardLayoutName() {
  return "";
}

}  // namespace vscode_keyboard
