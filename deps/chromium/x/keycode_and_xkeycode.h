// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef KEYCODE_AND_XKEYCODE_H_
#define KEYCODE_AND_XKEYCODE_H_

namespace ui {

struct KeyCodeAndXKeyCode {
  ui::KeyboardCode key_code;
  int x_key_code;
};
typedef struct KeyCodeAndXKeyCode KeyCodeAndXKeyCode;

// Contains a list of keyboard codes, in order, with their corresponding
// X key code. This list is not complete.
// TODO(kkania): Merge this table with the existing one in
// keyboard_code_conversion_x.cc.
KeyCodeAndXKeyCode gKeyCodeToXKeyCode[] = {
  { ui::VKEY_BACK, 22 },
  { ui::VKEY_TAB, 23 },
  { ui::VKEY_RETURN, 36 },
  { ui::VKEY_SHIFT, 50 },
  { ui::VKEY_CONTROL, 37 },
  { ui::VKEY_MENU, 64 },
  { ui::VKEY_CAPITAL, 66 },
  { ui::VKEY_HANGUL, 130 },
  { ui::VKEY_HANJA, 131 },
  { ui::VKEY_ESCAPE, 9 },
  { ui::VKEY_SPACE, 65 },
  { ui::VKEY_PRIOR, 112 },
  { ui::VKEY_NEXT, 117 },
  { ui::VKEY_END, 115 },
  { ui::VKEY_HOME, 110 },
  { ui::VKEY_LEFT, 113 },
  { ui::VKEY_UP, 111 },
  { ui::VKEY_RIGHT, 114 },
  { ui::VKEY_DOWN, 116 },
  { ui::VKEY_INSERT, 118 },
  { ui::VKEY_DELETE, 119 },
  { ui::VKEY_0, 19 },
  { ui::VKEY_1, 10 },
  { ui::VKEY_2, 11 },
  { ui::VKEY_3, 12 },
  { ui::VKEY_4, 13 },
  { ui::VKEY_5, 14 },
  { ui::VKEY_6, 15 },
  { ui::VKEY_7, 16 },
  { ui::VKEY_8, 17 },
  { ui::VKEY_9, 18 },
  { ui::VKEY_A, 38 },
  { ui::VKEY_B, 56 },
  { ui::VKEY_C, 54 },
  { ui::VKEY_D, 40 },
  { ui::VKEY_E, 26 },
  { ui::VKEY_F, 41 },
  { ui::VKEY_G, 42 },
  { ui::VKEY_H, 43 },
  { ui::VKEY_I, 31 },
  { ui::VKEY_J, 44 },
  { ui::VKEY_K, 45 },
  { ui::VKEY_L, 46 },
  { ui::VKEY_M, 58 },
  { ui::VKEY_N, 57 },
  { ui::VKEY_O, 32 },
  { ui::VKEY_P, 33 },
  { ui::VKEY_Q, 24 },
  { ui::VKEY_R, 27 },
  { ui::VKEY_S, 39 },
  { ui::VKEY_T, 28 },
  { ui::VKEY_U, 30 },
  { ui::VKEY_V, 55 },
  { ui::VKEY_W, 25 },
  { ui::VKEY_X, 53 },
  { ui::VKEY_Y, 29 },
  { ui::VKEY_Z, 52 },
  { ui::VKEY_LWIN, 133 },
  { ui::VKEY_NUMPAD0, 90 },
  { ui::VKEY_NUMPAD1, 87 },
  { ui::VKEY_NUMPAD2, 88 },
  { ui::VKEY_NUMPAD3, 89 },
  { ui::VKEY_NUMPAD4, 83 },
  { ui::VKEY_NUMPAD5, 84 },
  { ui::VKEY_NUMPAD6, 85 },
  { ui::VKEY_NUMPAD7, 79 },
  { ui::VKEY_NUMPAD8, 80 },
  { ui::VKEY_NUMPAD9, 81 },
  { ui::VKEY_MULTIPLY, 63 },
  { ui::VKEY_ADD, 86 },
  { ui::VKEY_SUBTRACT, 82 },
  { ui::VKEY_DECIMAL, 129 },
  { ui::VKEY_DIVIDE, 106 },
  { ui::VKEY_F1, 67 },
  { ui::VKEY_F2, 68 },
  { ui::VKEY_F3, 69 },
  { ui::VKEY_F4, 70 },
  { ui::VKEY_F5, 71 },
  { ui::VKEY_F6, 72 },
  { ui::VKEY_F7, 73 },
  { ui::VKEY_F8, 74 },
  { ui::VKEY_F9, 75 },
  { ui::VKEY_F10, 76 },
  { ui::VKEY_F11, 95 },
  { ui::VKEY_F12, 96 },
  { ui::VKEY_NUMLOCK, 77 },
  { ui::VKEY_SCROLL, 78 },
  { ui::VKEY_OEM_1, 47 },
  { ui::VKEY_OEM_PLUS, 21 },
  { ui::VKEY_OEM_COMMA, 59 },
  { ui::VKEY_OEM_MINUS, 20 },
  { ui::VKEY_OEM_PERIOD, 60 },
  { ui::VKEY_OEM_2, 61 },
  { ui::VKEY_OEM_3, 49 },
  { ui::VKEY_OEM_4, 34 },
  { ui::VKEY_OEM_5, 51 },
  { ui::VKEY_OEM_6, 35 },
  { ui::VKEY_OEM_7, 48 }
};

} // namespace ui

#endif  // KEYCODE_AND_XKEYCODE_H_
