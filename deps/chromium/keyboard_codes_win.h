// [13.12.2016] https://cs.chromium.org/chromium/src/ui/events/keycodes/keyboard_codes_win.h

// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_EVENTS_KEYCODES_KEYBOARD_CODES_WIN_H_
#define UI_EVENTS_KEYCODES_KEYBOARD_CODES_WIN_H_

#include <windows.h>
#include <ime.h>
#include <map>

namespace ui {

enum KeyboardCode {
  VKEY_CANCEL = VK_CANCEL,
  VKEY_BACK = VK_BACK,
  VKEY_TAB = VK_TAB,
  VKEY_CLEAR = VK_CLEAR,
  VKEY_RETURN = VK_RETURN,
  VKEY_SHIFT = VK_SHIFT,
  VKEY_CONTROL = VK_CONTROL,
  VKEY_MENU = VK_MENU,  // a.k.a. ALT
  VKEY_PAUSE = VK_PAUSE,
  VKEY_CAPITAL = VK_CAPITAL,
  VKEY_KANA = VK_KANA,
  VKEY_HANGUL = VK_HANGUL,
  VKEY_JUNJA = VK_JUNJA,
  VKEY_FINAL = VK_FINAL,
  VKEY_HANJA = VK_HANJA,
  VKEY_KANJI = VK_KANJI,
  VKEY_ESCAPE = VK_ESCAPE,
  VKEY_CONVERT = VK_CONVERT,
  VKEY_NONCONVERT = VK_NONCONVERT,
  VKEY_ACCEPT = VK_ACCEPT,
  VKEY_MODECHANGE = VK_MODECHANGE,
  VKEY_SPACE = VK_SPACE,
  VKEY_PRIOR = VK_PRIOR,
  VKEY_NEXT = VK_NEXT,
  VKEY_END = VK_END,
  VKEY_HOME = VK_HOME,
  VKEY_LEFT = VK_LEFT,
  VKEY_UP = VK_UP,
  VKEY_RIGHT = VK_RIGHT,
  VKEY_DOWN = VK_DOWN,
  VKEY_SELECT = VK_SELECT,
  VKEY_PRINT = VK_PRINT,
  VKEY_EXECUTE = VK_EXECUTE,
  VKEY_SNAPSHOT = VK_SNAPSHOT,
  VKEY_INSERT = VK_INSERT,
  VKEY_DELETE = VK_DELETE,
  VKEY_HELP = VK_HELP,
  VKEY_0 = '0',
  VKEY_1 = '1',
  VKEY_2 = '2',
  VKEY_3 = '3',
  VKEY_4 = '4',
  VKEY_5 = '5',
  VKEY_6 = '6',
  VKEY_7 = '7',
  VKEY_8 = '8',
  VKEY_9 = '9',
  VKEY_A = 'A',
  VKEY_B = 'B',
  VKEY_C = 'C',
  VKEY_D = 'D',
  VKEY_E = 'E',
  VKEY_F = 'F',
  VKEY_G = 'G',
  VKEY_H = 'H',
  VKEY_I = 'I',
  VKEY_J = 'J',
  VKEY_K = 'K',
  VKEY_L = 'L',
  VKEY_M = 'M',
  VKEY_N = 'N',
  VKEY_O = 'O',
  VKEY_P = 'P',
  VKEY_Q = 'Q',
  VKEY_R = 'R',
  VKEY_S = 'S',
  VKEY_T = 'T',
  VKEY_U = 'U',
  VKEY_V = 'V',
  VKEY_W = 'W',
  VKEY_X = 'X',
  VKEY_Y = 'Y',
  VKEY_Z = 'Z',
  VKEY_LWIN = VK_LWIN,
  VKEY_COMMAND = VKEY_LWIN,  // Provide the Mac name for convenience.
  VKEY_RWIN = VK_RWIN,
  VKEY_APPS = VK_APPS,
  VKEY_SLEEP = VK_SLEEP,
  VKEY_NUMPAD0 = VK_NUMPAD0,
  VKEY_NUMPAD1 = VK_NUMPAD1,
  VKEY_NUMPAD2 = VK_NUMPAD2,
  VKEY_NUMPAD3 = VK_NUMPAD3,
  VKEY_NUMPAD4 = VK_NUMPAD4,
  VKEY_NUMPAD5 = VK_NUMPAD5,
  VKEY_NUMPAD6 = VK_NUMPAD6,
  VKEY_NUMPAD7 = VK_NUMPAD7,
  VKEY_NUMPAD8 = VK_NUMPAD8,
  VKEY_NUMPAD9 = VK_NUMPAD9,
  VKEY_MULTIPLY = VK_MULTIPLY,
  VKEY_ADD = VK_ADD,
  VKEY_SEPARATOR = VK_SEPARATOR,
  VKEY_SUBTRACT = VK_SUBTRACT,
  VKEY_DECIMAL = VK_DECIMAL,
  VKEY_DIVIDE = VK_DIVIDE,
  VKEY_F1 = VK_F1,
  VKEY_F2 = VK_F2,
  VKEY_F3 = VK_F3,
  VKEY_F4 = VK_F4,
  VKEY_F5 = VK_F5,
  VKEY_F6 = VK_F6,
  VKEY_F7 = VK_F7,
  VKEY_F8 = VK_F8,
  VKEY_F9 = VK_F9,
  VKEY_F10 = VK_F10,
  VKEY_F11 = VK_F11,
  VKEY_F12 = VK_F12,
  VKEY_F13 = VK_F13,
  VKEY_F14 = VK_F14,
  VKEY_F15 = VK_F15,
  VKEY_F16 = VK_F16,
  VKEY_F17 = VK_F17,
  VKEY_F18 = VK_F18,
  VKEY_F19 = VK_F19,
  VKEY_F20 = VK_F20,
  VKEY_F21 = VK_F21,
  VKEY_F22 = VK_F22,
  VKEY_F23 = VK_F23,
  VKEY_F24 = VK_F24,
  VKEY_NUMLOCK = VK_NUMLOCK,
  VKEY_SCROLL = VK_SCROLL,
  VKEY_LSHIFT = VK_LSHIFT,
  VKEY_RSHIFT = VK_RSHIFT,
  VKEY_LCONTROL = VK_LCONTROL,
  VKEY_RCONTROL = VK_RCONTROL,
  VKEY_LMENU = VK_LMENU,
  VKEY_RMENU = VK_RMENU,
  VKEY_BROWSER_BACK = VK_BROWSER_BACK,
  VKEY_BROWSER_FORWARD = VK_BROWSER_FORWARD,
  VKEY_BROWSER_REFRESH = VK_BROWSER_REFRESH,
  VKEY_BROWSER_STOP = VK_BROWSER_STOP,
  VKEY_BROWSER_SEARCH = VK_BROWSER_SEARCH,
  VKEY_BROWSER_FAVORITES = VK_BROWSER_FAVORITES,
  VKEY_BROWSER_HOME = VK_BROWSER_HOME,
  VKEY_VOLUME_MUTE = VK_VOLUME_MUTE,
  VKEY_VOLUME_DOWN = VK_VOLUME_DOWN,
  VKEY_VOLUME_UP = VK_VOLUME_UP,
  VKEY_MEDIA_NEXT_TRACK = VK_MEDIA_NEXT_TRACK,
  VKEY_MEDIA_PREV_TRACK = VK_MEDIA_PREV_TRACK,
  VKEY_MEDIA_STOP = VK_MEDIA_STOP,
  VKEY_MEDIA_PLAY_PAUSE = VK_MEDIA_PLAY_PAUSE,
  VKEY_MEDIA_LAUNCH_MAIL = 0xB4,
  VKEY_MEDIA_LAUNCH_MEDIA_SELECT = 0xB5,
  VKEY_MEDIA_LAUNCH_APP1 = 0xB6,
  VKEY_MEDIA_LAUNCH_APP2 = 0xB7,
  VKEY_OEM_1 = VK_OEM_1,
  VKEY_OEM_PLUS = VK_OEM_PLUS,
  VKEY_OEM_COMMA = VK_OEM_COMMA,
  VKEY_OEM_MINUS = VK_OEM_MINUS,
  VKEY_OEM_PERIOD = VK_OEM_PERIOD,
  VKEY_OEM_2 = VK_OEM_2,
  VKEY_OEM_3 = VK_OEM_3,
  VKEY_OEM_4 = VK_OEM_4,
  VKEY_OEM_5 = VK_OEM_5,
  VKEY_OEM_6 = VK_OEM_6,
  VKEY_OEM_7 = VK_OEM_7,
  VKEY_OEM_8 = VK_OEM_8,
  VKEY_OEM_102 = VK_OEM_102,
  VKEY_PROCESSKEY = VK_PROCESSKEY,
  VKEY_PACKET = VK_PACKET,
  VKEY_OEM_ATTN = VK_OEM_ATTN,
  VKEY_OEM_FINISH = VK_OEM_FINISH,
  VKEY_OEM_COPY = VK_OEM_COPY,
  VKEY_DBE_SBCSCHAR = VK_DBE_SBCSCHAR,
  VKEY_DBE_DBCSCHAR = VK_DBE_DBCSCHAR,
  VKEY_OEM_BACKTAB = VK_OEM_BACKTAB,
  VKEY_ATTN = VK_ATTN,
  VKEY_CRSEL = VK_CRSEL,
  VKEY_EXSEL = VK_EXSEL,
  VKEY_EREOF = VK_EREOF,
  VKEY_PLAY = VK_PLAY,
  VKEY_ZOOM = VK_ZOOM,
  VKEY_NONAME = VK_NONAME,
  VKEY_PA1 = VK_PA1,
  VKEY_OEM_CLEAR = VK_OEM_CLEAR,
  VKEY_UNKNOWN = 0,

  // Windows does not have a specific key code for AltGr. We use the unused
  // VK_OEM_AX to represent AltGr, matching the behaviour of Firefox on Linux.
  VKEY_ALTGR = VK_OEM_AX,
};

const std::pair<KeyboardCode, std::string> gKeyboardCodeToStringMap[] = {
  std::make_pair(VKEY_CANCEL, "VKEY_CANCEL"),
  std::make_pair(VKEY_BACK, "VKEY_BACK"),
  std::make_pair(VKEY_TAB, "VKEY_TAB"),
  std::make_pair(VKEY_CLEAR, "VKEY_CLEAR"),
  std::make_pair(VKEY_RETURN, "VKEY_RETURN"),
  std::make_pair(VKEY_SHIFT, "VKEY_SHIFT"),
  std::make_pair(VKEY_CONTROL, "VKEY_CONTROL"),
  std::make_pair(VKEY_MENU, "VKEY_MENU"),
  std::make_pair(VKEY_PAUSE, "VKEY_PAUSE"),
  std::make_pair(VKEY_CAPITAL, "VKEY_CAPITAL"),
  std::make_pair(VKEY_KANA, "VKEY_KANA"),
  std::make_pair(VKEY_HANGUL, "VKEY_HANGUL"),
  std::make_pair(VKEY_JUNJA, "VKEY_JUNJA"),
  std::make_pair(VKEY_FINAL, "VKEY_FINAL"),
  std::make_pair(VKEY_HANJA, "VKEY_HANJA"),
  std::make_pair(VKEY_KANJI, "VKEY_KANJI"),
  std::make_pair(VKEY_ESCAPE, "VKEY_ESCAPE"),
  std::make_pair(VKEY_CONVERT, "VKEY_CONVERT"),
  std::make_pair(VKEY_NONCONVERT, "VKEY_NONCONVERT"),
  std::make_pair(VKEY_ACCEPT, "VKEY_ACCEPT"),
  std::make_pair(VKEY_MODECHANGE, "VKEY_MODECHANGE"),
  std::make_pair(VKEY_SPACE, "VKEY_SPACE"),
  std::make_pair(VKEY_PRIOR, "VKEY_PRIOR"),
  std::make_pair(VKEY_NEXT, "VKEY_NEXT"),
  std::make_pair(VKEY_END, "VKEY_END"),
  std::make_pair(VKEY_HOME, "VKEY_HOME"),
  std::make_pair(VKEY_LEFT, "VKEY_LEFT"),
  std::make_pair(VKEY_UP, "VKEY_UP"),
  std::make_pair(VKEY_RIGHT, "VKEY_RIGHT"),
  std::make_pair(VKEY_DOWN, "VKEY_DOWN"),
  std::make_pair(VKEY_SELECT, "VKEY_SELECT"),
  std::make_pair(VKEY_PRINT, "VKEY_PRINT"),
  std::make_pair(VKEY_EXECUTE, "VKEY_EXECUTE"),
  std::make_pair(VKEY_SNAPSHOT, "VKEY_SNAPSHOT"),
  std::make_pair(VKEY_INSERT, "VKEY_INSERT"),
  std::make_pair(VKEY_DELETE, "VKEY_DELETE"),
  std::make_pair(VKEY_HELP, "VKEY_HELP"),
  std::make_pair(VKEY_0, "VKEY_0"),
  std::make_pair(VKEY_1, "VKEY_1"),
  std::make_pair(VKEY_2, "VKEY_2"),
  std::make_pair(VKEY_3, "VKEY_3"),
  std::make_pair(VKEY_4, "VKEY_4"),
  std::make_pair(VKEY_5, "VKEY_5"),
  std::make_pair(VKEY_6, "VKEY_6"),
  std::make_pair(VKEY_7, "VKEY_7"),
  std::make_pair(VKEY_8, "VKEY_8"),
  std::make_pair(VKEY_9, "VKEY_9"),
  std::make_pair(VKEY_A, "VKEY_A"),
  std::make_pair(VKEY_B, "VKEY_B"),
  std::make_pair(VKEY_C, "VKEY_C"),
  std::make_pair(VKEY_D, "VKEY_D"),
  std::make_pair(VKEY_E, "VKEY_E"),
  std::make_pair(VKEY_F, "VKEY_F"),
  std::make_pair(VKEY_G, "VKEY_G"),
  std::make_pair(VKEY_H, "VKEY_H"),
  std::make_pair(VKEY_I, "VKEY_I"),
  std::make_pair(VKEY_J, "VKEY_J"),
  std::make_pair(VKEY_K, "VKEY_K"),
  std::make_pair(VKEY_L, "VKEY_L"),
  std::make_pair(VKEY_M, "VKEY_M"),
  std::make_pair(VKEY_N, "VKEY_N"),
  std::make_pair(VKEY_O, "VKEY_O"),
  std::make_pair(VKEY_P, "VKEY_P"),
  std::make_pair(VKEY_Q, "VKEY_Q"),
  std::make_pair(VKEY_R, "VKEY_R"),
  std::make_pair(VKEY_S, "VKEY_S"),
  std::make_pair(VKEY_T, "VKEY_T"),
  std::make_pair(VKEY_U, "VKEY_U"),
  std::make_pair(VKEY_V, "VKEY_V"),
  std::make_pair(VKEY_W, "VKEY_W"),
  std::make_pair(VKEY_X, "VKEY_X"),
  std::make_pair(VKEY_Y, "VKEY_Y"),
  std::make_pair(VKEY_Z, "VKEY_Z"),
  std::make_pair(VKEY_LWIN, "VKEY_LWIN"),
  std::make_pair(VKEY_COMMAND, "VKEY_COMMAND"),
  std::make_pair(VKEY_RWIN, "VKEY_RWIN"),
  std::make_pair(VKEY_APPS, "VKEY_APPS"),
  std::make_pair(VKEY_SLEEP, "VKEY_SLEEP"),
  std::make_pair(VKEY_NUMPAD0, "VKEY_NUMPAD0"),
  std::make_pair(VKEY_NUMPAD1, "VKEY_NUMPAD1"),
  std::make_pair(VKEY_NUMPAD2, "VKEY_NUMPAD2"),
  std::make_pair(VKEY_NUMPAD3, "VKEY_NUMPAD3"),
  std::make_pair(VKEY_NUMPAD4, "VKEY_NUMPAD4"),
  std::make_pair(VKEY_NUMPAD5, "VKEY_NUMPAD5"),
  std::make_pair(VKEY_NUMPAD6, "VKEY_NUMPAD6"),
  std::make_pair(VKEY_NUMPAD7, "VKEY_NUMPAD7"),
  std::make_pair(VKEY_NUMPAD8, "VKEY_NUMPAD8"),
  std::make_pair(VKEY_NUMPAD9, "VKEY_NUMPAD9"),
  std::make_pair(VKEY_MULTIPLY, "VKEY_MULTIPLY"),
  std::make_pair(VKEY_ADD, "VKEY_ADD"),
  std::make_pair(VKEY_SEPARATOR, "VKEY_SEPARATOR"),
  std::make_pair(VKEY_SUBTRACT, "VKEY_SUBTRACT"),
  std::make_pair(VKEY_DECIMAL, "VKEY_DECIMAL"),
  std::make_pair(VKEY_DIVIDE, "VKEY_DIVIDE"),
  std::make_pair(VKEY_F1, "VKEY_F1"),
  std::make_pair(VKEY_F2, "VKEY_F2"),
  std::make_pair(VKEY_F3, "VKEY_F3"),
  std::make_pair(VKEY_F4, "VKEY_F4"),
  std::make_pair(VKEY_F5, "VKEY_F5"),
  std::make_pair(VKEY_F6, "VKEY_F6"),
  std::make_pair(VKEY_F7, "VKEY_F7"),
  std::make_pair(VKEY_F8, "VKEY_F8"),
  std::make_pair(VKEY_F9, "VKEY_F9"),
  std::make_pair(VKEY_F10, "VKEY_F10"),
  std::make_pair(VKEY_F11, "VKEY_F11"),
  std::make_pair(VKEY_F12, "VKEY_F12"),
  std::make_pair(VKEY_F13, "VKEY_F13"),
  std::make_pair(VKEY_F14, "VKEY_F14"),
  std::make_pair(VKEY_F15, "VKEY_F15"),
  std::make_pair(VKEY_F16, "VKEY_F16"),
  std::make_pair(VKEY_F17, "VKEY_F17"),
  std::make_pair(VKEY_F18, "VKEY_F18"),
  std::make_pair(VKEY_F19, "VKEY_F19"),
  std::make_pair(VKEY_F20, "VKEY_F20"),
  std::make_pair(VKEY_F21, "VKEY_F21"),
  std::make_pair(VKEY_F22, "VKEY_F22"),
  std::make_pair(VKEY_F23, "VKEY_F23"),
  std::make_pair(VKEY_F24, "VKEY_F24"),
  std::make_pair(VKEY_NUMLOCK, "VKEY_NUMLOCK"),
  std::make_pair(VKEY_SCROLL, "VKEY_SCROLL"),
  std::make_pair(VKEY_LSHIFT, "VKEY_LSHIFT"),
  std::make_pair(VKEY_RSHIFT, "VKEY_RSHIFT"),
  std::make_pair(VKEY_LCONTROL, "VKEY_LCONTROL"),
  std::make_pair(VKEY_RCONTROL, "VKEY_RCONTROL"),
  std::make_pair(VKEY_LMENU, "VKEY_LMENU"),
  std::make_pair(VKEY_RMENU, "VKEY_RMENU"),
  std::make_pair(VKEY_BROWSER_BACK, "VKEY_BROWSER_BACK"),
  std::make_pair(VKEY_BROWSER_FORWARD, "VKEY_BROWSER_FORWARD"),
  std::make_pair(VKEY_BROWSER_REFRESH, "VKEY_BROWSER_REFRESH"),
  std::make_pair(VKEY_BROWSER_STOP, "VKEY_BROWSER_STOP"),
  std::make_pair(VKEY_BROWSER_SEARCH, "VKEY_BROWSER_SEARCH"),
  std::make_pair(VKEY_BROWSER_FAVORITES, "VKEY_BROWSER_FAVORITES"),
  std::make_pair(VKEY_BROWSER_HOME, "VKEY_BROWSER_HOME"),
  std::make_pair(VKEY_VOLUME_MUTE, "VKEY_VOLUME_MUTE"),
  std::make_pair(VKEY_VOLUME_DOWN, "VKEY_VOLUME_DOWN"),
  std::make_pair(VKEY_VOLUME_UP, "VKEY_VOLUME_UP"),
  std::make_pair(VKEY_MEDIA_NEXT_TRACK, "VKEY_MEDIA_NEXT_TRACK"),
  std::make_pair(VKEY_MEDIA_PREV_TRACK, "VKEY_MEDIA_PREV_TRACK"),
  std::make_pair(VKEY_MEDIA_STOP, "VKEY_MEDIA_STOP"),
  std::make_pair(VKEY_MEDIA_PLAY_PAUSE, "VKEY_MEDIA_PLAY_PAUSE"),
  std::make_pair(VKEY_MEDIA_LAUNCH_MAIL, "VKEY_MEDIA_LAUNCH_MAIL"),
  std::make_pair(VKEY_MEDIA_LAUNCH_MEDIA_SELECT, "VKEY_MEDIA_LAUNCH_MEDIA_SELECT"),
  std::make_pair(VKEY_MEDIA_LAUNCH_APP1, "VKEY_MEDIA_LAUNCH_APP1"),
  std::make_pair(VKEY_MEDIA_LAUNCH_APP2, "VKEY_MEDIA_LAUNCH_APP2"),
  std::make_pair(VKEY_OEM_1, "VKEY_OEM_1"),
  std::make_pair(VKEY_OEM_PLUS, "VKEY_OEM_PLUS"),
  std::make_pair(VKEY_OEM_COMMA, "VKEY_OEM_COMMA"),
  std::make_pair(VKEY_OEM_MINUS, "VKEY_OEM_MINUS"),
  std::make_pair(VKEY_OEM_PERIOD, "VKEY_OEM_PERIOD"),
  std::make_pair(VKEY_OEM_2, "VKEY_OEM_2"),
  std::make_pair(VKEY_OEM_3, "VKEY_OEM_3"),
  std::make_pair(VKEY_OEM_4, "VKEY_OEM_4"),
  std::make_pair(VKEY_OEM_5, "VKEY_OEM_5"),
  std::make_pair(VKEY_OEM_6, "VKEY_OEM_6"),
  std::make_pair(VKEY_OEM_7, "VKEY_OEM_7"),
  std::make_pair(VKEY_OEM_8, "VKEY_OEM_8"),
  std::make_pair(VKEY_OEM_102, "VKEY_OEM_102"),
  std::make_pair(VKEY_PROCESSKEY, "VKEY_PROCESSKEY"),
  std::make_pair(VKEY_PACKET, "VKEY_PACKET"),
  std::make_pair(VKEY_OEM_ATTN, "VKEY_OEM_ATTN"),
  std::make_pair(VKEY_OEM_FINISH, "VKEY_OEM_FINISH"),
  std::make_pair(VKEY_OEM_COPY, "VKEY_OEM_COPY"),
  std::make_pair(VKEY_DBE_SBCSCHAR, "VKEY_DBE_SBCSCHAR"),
  std::make_pair(VKEY_DBE_DBCSCHAR, "VKEY_DBE_DBCSCHAR"),
  std::make_pair(VKEY_OEM_BACKTAB, "VKEY_OEM_BACKTAB"),
  std::make_pair(VKEY_ATTN, "VKEY_ATTN"),
  std::make_pair(VKEY_CRSEL, "VKEY_CRSEL"),
  std::make_pair(VKEY_EXSEL, "VKEY_EXSEL"),
  std::make_pair(VKEY_EREOF, "VKEY_EREOF"),
  std::make_pair(VKEY_PLAY, "VKEY_PLAY"),
  std::make_pair(VKEY_ZOOM, "VKEY_ZOOM"),
  std::make_pair(VKEY_NONAME, "VKEY_NONAME"),
  std::make_pair(VKEY_PA1, "VKEY_PA1"),
  std::make_pair(VKEY_OEM_CLEAR, "VKEY_OEM_CLEAR"),
  std::make_pair(VKEY_UNKNOWN, "VKEY_UNKNOWN"),
  std::make_pair(VKEY_ALTGR, "VKEY_ALTGR"),
};


}  // namespace ui

#endif  // UI_EVENTS_KEYCODES_KEYBOARD_CODES_WIN_H_
