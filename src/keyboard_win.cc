/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Microsoft Corporation. All rights reserved.
 *  Licensed under the MIT License. See License.txt in the project root for license information.
 *--------------------------------------------------------------------------------------------*/

#include "keymapping.h"

#include "../deps/chromium/macros.h"

#include "string_conversion.h"

namespace {

ui::KeyboardCode gAllKeyboardCodes[] = {
  ui::KeyboardCode::VKEY_BACK,
  ui::KeyboardCode::VKEY_TAB,
  ui::KeyboardCode::VKEY_CLEAR,
  ui::KeyboardCode::VKEY_RETURN,
  ui::KeyboardCode::VKEY_SHIFT,
  ui::KeyboardCode::VKEY_CONTROL,
  ui::KeyboardCode::VKEY_MENU,
  ui::KeyboardCode::VKEY_PAUSE,
  ui::KeyboardCode::VKEY_CAPITAL,
  ui::KeyboardCode::VKEY_KANA,
  ui::KeyboardCode::VKEY_HANGUL,
  ui::KeyboardCode::VKEY_JUNJA,
  ui::KeyboardCode::VKEY_FINAL,
  ui::KeyboardCode::VKEY_HANJA,
  ui::KeyboardCode::VKEY_KANJI,
  ui::KeyboardCode::VKEY_ESCAPE,
  ui::KeyboardCode::VKEY_CONVERT,
  ui::KeyboardCode::VKEY_NONCONVERT,
  ui::KeyboardCode::VKEY_ACCEPT,
  ui::KeyboardCode::VKEY_MODECHANGE,
  ui::KeyboardCode::VKEY_SPACE,
  ui::KeyboardCode::VKEY_PRIOR,
  ui::KeyboardCode::VKEY_NEXT,
  ui::KeyboardCode::VKEY_END,
  ui::KeyboardCode::VKEY_HOME,
  ui::KeyboardCode::VKEY_LEFT,
  ui::KeyboardCode::VKEY_UP,
  ui::KeyboardCode::VKEY_RIGHT,
  ui::KeyboardCode::VKEY_DOWN,
  ui::KeyboardCode::VKEY_SELECT,
  ui::KeyboardCode::VKEY_PRINT,
  ui::KeyboardCode::VKEY_EXECUTE,
  ui::KeyboardCode::VKEY_SNAPSHOT,
  ui::KeyboardCode::VKEY_INSERT,
  ui::KeyboardCode::VKEY_DELETE,
  ui::KeyboardCode::VKEY_HELP,
  ui::KeyboardCode::VKEY_0,
  ui::KeyboardCode::VKEY_1,
  ui::KeyboardCode::VKEY_2,
  ui::KeyboardCode::VKEY_3,
  ui::KeyboardCode::VKEY_4,
  ui::KeyboardCode::VKEY_5,
  ui::KeyboardCode::VKEY_6,
  ui::KeyboardCode::VKEY_7,
  ui::KeyboardCode::VKEY_8,
  ui::KeyboardCode::VKEY_9,
  ui::KeyboardCode::VKEY_A,
  ui::KeyboardCode::VKEY_B,
  ui::KeyboardCode::VKEY_C,
  ui::KeyboardCode::VKEY_D,
  ui::KeyboardCode::VKEY_E,
  ui::KeyboardCode::VKEY_F,
  ui::KeyboardCode::VKEY_G,
  ui::KeyboardCode::VKEY_H,
  ui::KeyboardCode::VKEY_I,
  ui::KeyboardCode::VKEY_J,
  ui::KeyboardCode::VKEY_K,
  ui::KeyboardCode::VKEY_L,
  ui::KeyboardCode::VKEY_M,
  ui::KeyboardCode::VKEY_N,
  ui::KeyboardCode::VKEY_O,
  ui::KeyboardCode::VKEY_P,
  ui::KeyboardCode::VKEY_Q,
  ui::KeyboardCode::VKEY_R,
  ui::KeyboardCode::VKEY_S,
  ui::KeyboardCode::VKEY_T,
  ui::KeyboardCode::VKEY_U,
  ui::KeyboardCode::VKEY_V,
  ui::KeyboardCode::VKEY_W,
  ui::KeyboardCode::VKEY_X,
  ui::KeyboardCode::VKEY_Y,
  ui::KeyboardCode::VKEY_Z,
  ui::KeyboardCode::VKEY_LWIN,
  ui::KeyboardCode::VKEY_COMMAND,
  ui::KeyboardCode::VKEY_RWIN,
  ui::KeyboardCode::VKEY_APPS,
  ui::KeyboardCode::VKEY_SLEEP,
  ui::KeyboardCode::VKEY_NUMPAD0,
  ui::KeyboardCode::VKEY_NUMPAD1,
  ui::KeyboardCode::VKEY_NUMPAD2,
  ui::KeyboardCode::VKEY_NUMPAD3,
  ui::KeyboardCode::VKEY_NUMPAD4,
  ui::KeyboardCode::VKEY_NUMPAD5,
  ui::KeyboardCode::VKEY_NUMPAD6,
  ui::KeyboardCode::VKEY_NUMPAD7,
  ui::KeyboardCode::VKEY_NUMPAD8,
  ui::KeyboardCode::VKEY_NUMPAD9,
  ui::KeyboardCode::VKEY_MULTIPLY,
  ui::KeyboardCode::VKEY_ADD,
  ui::KeyboardCode::VKEY_SEPARATOR,
  ui::KeyboardCode::VKEY_SUBTRACT,
  ui::KeyboardCode::VKEY_DECIMAL,
  ui::KeyboardCode::VKEY_DIVIDE,
  ui::KeyboardCode::VKEY_F1,
  ui::KeyboardCode::VKEY_F2,
  ui::KeyboardCode::VKEY_F3,
  ui::KeyboardCode::VKEY_F4,
  ui::KeyboardCode::VKEY_F5,
  ui::KeyboardCode::VKEY_F6,
  ui::KeyboardCode::VKEY_F7,
  ui::KeyboardCode::VKEY_F8,
  ui::KeyboardCode::VKEY_F9,
  ui::KeyboardCode::VKEY_F10,
  ui::KeyboardCode::VKEY_F11,
  ui::KeyboardCode::VKEY_F12,
  ui::KeyboardCode::VKEY_F13,
  ui::KeyboardCode::VKEY_F14,
  ui::KeyboardCode::VKEY_F15,
  ui::KeyboardCode::VKEY_F16,
  ui::KeyboardCode::VKEY_F17,
  ui::KeyboardCode::VKEY_F18,
  ui::KeyboardCode::VKEY_F19,
  ui::KeyboardCode::VKEY_F20,
  ui::KeyboardCode::VKEY_F21,
  ui::KeyboardCode::VKEY_F22,
  ui::KeyboardCode::VKEY_F23,
  ui::KeyboardCode::VKEY_F24,
  ui::KeyboardCode::VKEY_NUMLOCK,
  ui::KeyboardCode::VKEY_SCROLL,
  ui::KeyboardCode::VKEY_LSHIFT,
  ui::KeyboardCode::VKEY_RSHIFT,
  ui::KeyboardCode::VKEY_LCONTROL,
  ui::KeyboardCode::VKEY_RCONTROL,
  ui::KeyboardCode::VKEY_LMENU,
  ui::KeyboardCode::VKEY_RMENU,
  ui::KeyboardCode::VKEY_BROWSER_BACK,
  ui::KeyboardCode::VKEY_BROWSER_FORWARD,
  ui::KeyboardCode::VKEY_BROWSER_REFRESH,
  ui::KeyboardCode::VKEY_BROWSER_STOP,
  ui::KeyboardCode::VKEY_BROWSER_SEARCH,
  ui::KeyboardCode::VKEY_BROWSER_FAVORITES,
  ui::KeyboardCode::VKEY_BROWSER_HOME,
  ui::KeyboardCode::VKEY_VOLUME_MUTE,
  ui::KeyboardCode::VKEY_VOLUME_DOWN,
  ui::KeyboardCode::VKEY_VOLUME_UP,
  ui::KeyboardCode::VKEY_MEDIA_NEXT_TRACK,
  ui::KeyboardCode::VKEY_MEDIA_PREV_TRACK,
  ui::KeyboardCode::VKEY_MEDIA_STOP,
  ui::KeyboardCode::VKEY_MEDIA_PLAY_PAUSE,
  ui::KeyboardCode::VKEY_MEDIA_LAUNCH_MAIL,
  ui::KeyboardCode::VKEY_MEDIA_LAUNCH_MEDIA_SELECT,
  ui::KeyboardCode::VKEY_MEDIA_LAUNCH_APP1,
  ui::KeyboardCode::VKEY_MEDIA_LAUNCH_APP2,
  ui::KeyboardCode::VKEY_OEM_1,
  ui::KeyboardCode::VKEY_OEM_PLUS,
  ui::KeyboardCode::VKEY_OEM_COMMA,
  ui::KeyboardCode::VKEY_OEM_MINUS,
  ui::KeyboardCode::VKEY_OEM_PERIOD,
  ui::KeyboardCode::VKEY_OEM_2,
  ui::KeyboardCode::VKEY_OEM_3,
  ui::KeyboardCode::VKEY_OEM_4,
  ui::KeyboardCode::VKEY_OEM_5,
  ui::KeyboardCode::VKEY_OEM_6,
  ui::KeyboardCode::VKEY_OEM_7,
  ui::KeyboardCode::VKEY_OEM_8,
  ui::KeyboardCode::VKEY_OEM_102,
  ui::KeyboardCode::VKEY_PROCESSKEY,
  ui::KeyboardCode::VKEY_PACKET,
  ui::KeyboardCode::VKEY_DBE_SBCSCHAR,
  ui::KeyboardCode::VKEY_DBE_DBCSCHAR,
  ui::KeyboardCode::VKEY_ATTN,
  ui::KeyboardCode::VKEY_CRSEL,
  ui::KeyboardCode::VKEY_EXSEL,
  ui::KeyboardCode::VKEY_EREOF,
  ui::KeyboardCode::VKEY_PLAY,
  ui::KeyboardCode::VKEY_ZOOM,
  ui::KeyboardCode::VKEY_NONAME,
  ui::KeyboardCode::VKEY_PA1,
  ui::KeyboardCode::VKEY_OEM_CLEAR,
  ui::KeyboardCode::VKEY_UNKNOWN,
  ui::KeyboardCode::VKEY_ALTGR,
};

std::string GetStrFromKeyPress(ui::KeyboardCode key_code, int modifiers, BYTE *keyboard_state) {
  memset(keyboard_state, 0, 256);

  bool hasModifiers = false;

  if (modifiers & kShiftKeyModifierMask) {
    hasModifiers = true;
    keyboard_state[VK_SHIFT] |= 0x80;
  }

  if (modifiers & kControlKeyModifierMask) {
    hasModifiers = true;
    keyboard_state[VK_CONTROL] |= 0x80;
  }

  if (modifiers & kAltKeyModifierMask) {
    hasModifiers = true;
    keyboard_state[VK_MENU] |= 0x80;
  }

  if (!hasModifiers) {
    wchar_t key = LOWORD(::MapVirtualKeyW(key_code, MAPVK_VK_TO_CHAR));
    if (key) {
      return vscode_keyboard::UTF16toUTF8(&key, 1);
    }
  }

  UINT scan_code = ::MapVirtualKeyW(key_code, MAPVK_VK_TO_VSC);

  wchar_t chars[5];
  int code = ::ToUnicode(key_code, scan_code, keyboard_state, chars, 4, 0);

  if (code <= 0 || (code == 1 && iswcntrl(chars[0]))) {
    return std::string();
  }

  return vscode_keyboard::UTF16toUTF8(chars, code);
}

} // namespace

namespace vscode_keyboard {

std::vector<KeyMapping> GetKeyMapping() {
  std::vector<KeyMapping> result;

  BYTE keyboard_state[256];
  for (size_t i = 0; i < arraysize(gAllKeyboardCodes); ++i) {
    ui::KeyboardCode key_code = gAllKeyboardCodes[i];

    std::string value = GetStrFromKeyPress(key_code, 0, keyboard_state);
    std::string withShift = GetStrFromKeyPress(key_code, kShiftKeyModifierMask, keyboard_state);
    std::string withAltGr = GetStrFromKeyPress(key_code, kControlKeyModifierMask | kAltKeyModifierMask, keyboard_state);
    std::string withShiftAltGr = GetStrFromKeyPress(key_code, kShiftKeyModifierMask | kControlKeyModifierMask | kAltKeyModifierMask, keyboard_state);

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

}  // namespace vscode_keyboard
