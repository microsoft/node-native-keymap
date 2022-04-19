/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Microsoft Corporation. All rights reserved.
 *  Licensed under the MIT License. See License.txt in the project root for license information.
 *--------------------------------------------------------------------------------------------*/

#include "keymapping.h"
#include "common.h"

#include "../deps/chromium/macros.h"

#include "string_conversion.h"
#include <windows.h>
#include <Msctf.h>
#include <ime.h>

namespace {

void ClearKeyboardBuffer(UINT key_code, UINT scan_code, BYTE* keyboard_state) {
  memset(keyboard_state, 0, 256);

  wchar_t chars[5];
  int code = 0;
  do {
    code = ::ToUnicode(key_code, scan_code, keyboard_state, chars, 4, 0);
  } while (code < 0);
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

  if (code == -1) {
    // dead key
    if (chars[0] == 0 || iswcntrl(chars[0])) {
      return std::string();
    }
    code = 1;
  }

  ClearKeyboardBuffer(clear_key_code, clear_scan_code, keyboard_state);

  if (code <= 0 || (code == 1 && iswcntrl(chars[0]))) {
    return std::string();
  }

  return vscode_keyboard::UTF16toUTF8(chars, code);
}

} // namespace

namespace vscode_keyboard {

#define DOM_CODE(usb, evdev, xkb, win, mac, code, id) {usb, win, code}
#define DOM_CODE_DECLARATION const KeycodeMapEntry usb_keycode_map[] =
#include "../deps/chromium/dom_code_data.inc"
#undef DOM_CODE
#undef DOM_CODE_DECLARATION

typedef struct {
  int vkey;
  const char* str_vkey;
} VKeyStrEntry;

const char* VKeyToStr(int vkey) {
  switch (vkey) {
  case VK_LBUTTON: return "VK_LBUTTON"; // Left mouse button
  case VK_RBUTTON: return "VK_RBUTTON"; // Right mouse button
  case VK_CANCEL: return "VK_CANCEL"; // Control-break processing
  case VK_MBUTTON: return "VK_MBUTTON"; // Middle mouse button (three-button mouse)
  case VK_XBUTTON1: return "VK_XBUTTON1"; // X1 mouse button
  case VK_XBUTTON2: return "VK_XBUTTON2"; // X2 mouse button
  case VK_BACK: return "VK_BACK"; // BACKSPACE key
  case VK_TAB: return "VK_TAB"; // TAB key
  case VK_CLEAR: return "VK_CLEAR"; // CLEAR key
  case VK_RETURN: return "VK_RETURN"; // ENTER key
  case VK_SHIFT: return "VK_SHIFT"; // SHIFT key
  case VK_CONTROL: return "VK_CONTROL"; // CTRL key
  case VK_MENU: return "VK_MENU"; // ALT key
  case VK_PAUSE: return "VK_PAUSE"; // PAUSE key
  case VK_CAPITAL: return "VK_CAPITAL"; // CAPS LOCK key
  case VK_KANA: return "VK_KANA"; // IME Kana mode
  //case VK_HANGUL: return "VK_HANGUEL"; // IME Hangul mode
  case VK_JUNJA: return "VK_JUNJA"; // IME Junja mode
  case VK_FINAL: return "VK_FINAL"; // IME final mode
  case VK_HANJA: return "VK_HANJA"; // IME Hanja mode
  //case VK_KANJI: return "VK_KANJI"; // IME Kanji mode
  case VK_ESCAPE: return "VK_ESCAPE"; // ESC key
  case VK_CONVERT: return "VK_CONVERT"; // IME convert
  case VK_NONCONVERT: return "VK_NONCONVERT"; // IME nonconvert
  case VK_ACCEPT: return "VK_ACCEPT"; // IME accept
  case VK_MODECHANGE: return "VK_MODECHANGE"; // IME mode change request
  case VK_SPACE: return "VK_SPACE"; // SPACEBAR
  case VK_PRIOR: return "VK_PRIOR"; // PAGE UP key
  case VK_NEXT: return "VK_NEXT"; // PAGE DOWN key
  case VK_END: return "VK_END"; // END key
  case VK_HOME: return "VK_HOME"; // HOME key
  case VK_LEFT: return "VK_LEFT"; // LEFT ARROW key
  case VK_UP: return "VK_UP"; // UP ARROW key
  case VK_RIGHT: return "VK_RIGHT"; // RIGHT ARROW key
  case VK_DOWN: return "VK_DOWN"; // DOWN ARROW key
  case VK_SELECT: return "VK_SELECT"; // SELECT key
  case VK_PRINT: return "VK_PRINT"; // PRINT key
  case VK_EXECUTE: return "VK_EXECUTE"; // EXECUTE key
  case VK_SNAPSHOT: return "VK_SNAPSHOT"; // PRINT SCREEN key
  case VK_INSERT: return "VK_INSERT"; // INS key
  case VK_DELETE: return "VK_DELETE"; // DEL key
  case VK_HELP: return "VK_HELP"; // HELP key

  case '0': return "VK_0";
  case '1': return "VK_1";
  case '2': return "VK_2";
  case '3': return "VK_3";
  case '4': return "VK_4";
  case '5': return "VK_5";
  case '6': return "VK_6";
  case '7': return "VK_7";
  case '8': return "VK_8";
  case '9': return "VK_9";
  case 'A': return "VK_A";
  case 'B': return "VK_B";
  case 'C': return "VK_C";
  case 'D': return "VK_D";
  case 'E': return "VK_E";
  case 'F': return "VK_F";
  case 'G': return "VK_G";
  case 'H': return "VK_H";
  case 'I': return "VK_I";
  case 'J': return "VK_J";
  case 'K': return "VK_K";
  case 'L': return "VK_L";
  case 'M': return "VK_M";
  case 'N': return "VK_N";
  case 'O': return "VK_O";
  case 'P': return "VK_P";
  case 'Q': return "VK_Q";
  case 'R': return "VK_R";
  case 'S': return "VK_S";
  case 'T': return "VK_T";
  case 'U': return "VK_U";
  case 'V': return "VK_V";
  case 'W': return "VK_W";
  case 'X': return "VK_X";
  case 'Y': return "VK_Y";
  case 'Z': return "VK_Z";

  case VK_LWIN: return "VK_LWIN"; // Left Windows key (Natural keyboard)
  case VK_RWIN: return "VK_RWIN"; // Right Windows key (Natural keyboard)
  case VK_APPS: return "VK_APPS"; // Applications key (Natural keyboard)
  case VK_SLEEP: return "VK_SLEEP"; // Computer Sleep key
  case VK_NUMPAD0: return "VK_NUMPAD0"; // Numeric keypad 0 key
  case VK_NUMPAD1: return "VK_NUMPAD1"; // Numeric keypad 1 key
  case VK_NUMPAD2: return "VK_NUMPAD2"; // Numeric keypad 2 key
  case VK_NUMPAD3: return "VK_NUMPAD3"; // Numeric keypad 3 key
  case VK_NUMPAD4: return "VK_NUMPAD4"; // Numeric keypad 4 key
  case VK_NUMPAD5: return "VK_NUMPAD5"; // Numeric keypad 5 key
  case VK_NUMPAD6: return "VK_NUMPAD6"; // Numeric keypad 6 key
  case VK_NUMPAD7: return "VK_NUMPAD7"; // Numeric keypad 7 key
  case VK_NUMPAD8: return "VK_NUMPAD8"; // Numeric keypad 8 key
  case VK_NUMPAD9: return "VK_NUMPAD9"; // Numeric keypad 9 key
  case VK_MULTIPLY: return "VK_MULTIPLY"; // Multiply key
  case VK_ADD: return "VK_ADD"; // Add key
  case VK_SEPARATOR: return "VK_SEPARATOR"; // Separator key
  case VK_SUBTRACT: return "VK_SUBTRACT"; // Subtract key
  case VK_DECIMAL: return "VK_DECIMAL"; // Decimal key
  case VK_DIVIDE: return "VK_DIVIDE"; // Divide key
  case VK_F1: return "VK_F1"; // F1 key
  case VK_F2: return "VK_F2"; // F2 key
  case VK_F3: return "VK_F3"; // F3 key
  case VK_F4: return "VK_F4"; // F4 key
  case VK_F5: return "VK_F5"; // F5 key
  case VK_F6: return "VK_F6"; // F6 key
  case VK_F7: return "VK_F7"; // F7 key
  case VK_F8: return "VK_F8"; // F8 key
  case VK_F9: return "VK_F9"; // F9 key
  case VK_F10: return "VK_F10"; // F10 key
  case VK_F11: return "VK_F11"; // F11 key
  case VK_F12: return "VK_F12"; // F12 key
  case VK_F13: return "VK_F13"; // F13 key
  case VK_F14: return "VK_F14"; // F14 key
  case VK_F15: return "VK_F15"; // F15 key
  case VK_F16: return "VK_F16"; // F16 key
  case VK_F17: return "VK_F17"; // F17 key
  case VK_F18: return "VK_F18"; // F18 key
  case VK_F19: return "VK_F19"; // F19 key
  case VK_F20: return "VK_F20"; // F20 key
  case VK_F21: return "VK_F21"; // F21 key
  case VK_F22: return "VK_F22"; // F22 key
  case VK_F23: return "VK_F23"; // F23 key
  case VK_F24: return "VK_F24"; // F24 key
  case VK_NUMLOCK: return "VK_NUMLOCK"; // NUM LOCK key
  case VK_SCROLL: return "VK_SCROLL"; // SCROLL LOCK key
  case VK_LSHIFT: return "VK_LSHIFT"; // Left SHIFT key
  case VK_RSHIFT: return "VK_RSHIFT"; // Right SHIFT key
  case VK_LCONTROL: return "VK_LCONTROL"; // Left CONTROL key
  case VK_RCONTROL: return "VK_RCONTROL"; // Right CONTROL key
  case VK_LMENU: return "VK_LMENU"; // Left MENU key
  case VK_RMENU: return "VK_RMENU"; // Right MENU key
  case VK_BROWSER_BACK: return "VK_BROWSER_BACK"; // Browser Back key
  case VK_BROWSER_FORWARD: return "VK_BROWSER_FORWARD"; // Browser Forward key
  case VK_BROWSER_REFRESH: return "VK_BROWSER_REFRESH"; // Browser Refresh key
  case VK_BROWSER_STOP: return "VK_BROWSER_STOP"; // Browser Stop key
  case VK_BROWSER_SEARCH: return "VK_BROWSER_SEARCH"; // Browser Search key
  case VK_BROWSER_FAVORITES: return "VK_BROWSER_FAVORITES"; // Browser Favorites key
  case VK_BROWSER_HOME: return "VK_BROWSER_HOME"; // Browser Start and Home key
  case VK_VOLUME_MUTE: return "VK_VOLUME_MUTE"; // Volume Mute key
  case VK_VOLUME_DOWN: return "VK_VOLUME_DOWN"; // Volume Down key
  case VK_VOLUME_UP: return "VK_VOLUME_UP"; // Volume Up key
  case VK_MEDIA_NEXT_TRACK: return "VK_MEDIA_NEXT_TRACK"; // Next Track key
  case VK_MEDIA_PREV_TRACK: return "VK_MEDIA_PREV_TRACK"; // Previous Track key
  case VK_MEDIA_STOP: return "VK_MEDIA_STOP"; // Stop Media key
  case VK_MEDIA_PLAY_PAUSE: return "VK_MEDIA_PLAY_PAUSE"; // Play/Pause Media key
  case VK_LAUNCH_MAIL: return "VK_LAUNCH_MAIL"; // Start Mail key
  case VK_LAUNCH_MEDIA_SELECT: return "VK_LAUNCH_MEDIA_SELECT"; // Select Media key
  case VK_LAUNCH_APP1: return "VK_LAUNCH_APP1"; // Start Application 1 key
  case VK_LAUNCH_APP2: return "VK_LAUNCH_APP2"; // Start Application 2 key
  case VK_OEM_1: return "VK_OEM_1"; // Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the ';:' key
  case VK_OEM_PLUS: return "VK_OEM_PLUS"; // For any country/region, the '+' key
  case VK_OEM_COMMA: return "VK_OEM_COMMA"; // For any country/region, the ',' key
  case VK_OEM_MINUS: return "VK_OEM_MINUS"; // For any country/region, the '-' key
  case VK_OEM_PERIOD: return "VK_OEM_PERIOD"; // For any country/region, the '.' key
  case VK_OEM_2: return "VK_OEM_2"; // Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '/?' key
  case VK_OEM_3: return "VK_OEM_3"; // Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '`~' key
  case 0xC1: return "VK_ABNT_C1"; // Brazilian (ABNT) Keyboard
  case 0xC2: return "VK_ABNT_C2"; // Brazilian (ABNT) Keyboard
  case VK_OEM_4: return "VK_OEM_4"; // Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '[{' key
  case VK_OEM_5: return "VK_OEM_5"; // Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '\|' key
  case VK_OEM_6: return "VK_OEM_6"; // Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the ']}' key
  case VK_OEM_7: return "VK_OEM_7"; // Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the 'single-quote/double-quote' key
  case VK_OEM_8: return "VK_OEM_8"; // Used for miscellaneous characters; it can vary by keyboard.
  case VK_OEM_102: return "VK_OEM_102"; // Either the angle bracket key or the backslash key on the RT 102-key keyboard
  case VK_PROCESSKEY: return "VK_PROCESSKEY"; // IME PROCESS key
  case VK_PACKET: return "VK_PACKET"; // Used to pass Unicode characters as if they were keystrokes. The VK_PACKET key is the low word of a 32-bit Virtual Key value used for non-keyboard input methods. For more information, see Remark in KEYBDINPUT, SendInput, WM_KEYDOWN, and WM_KEYUP // 0xE8
  case VK_ATTN: return "VK_ATTN"; // Attn key
  case VK_CRSEL: return "VK_CRSEL"; // CrSel key
  case VK_EXSEL: return "VK_EXSEL"; // ExSel key
  case VK_EREOF: return "VK_EREOF"; // Erase EOF key
  case VK_PLAY: return "VK_PLAY"; // Play key
  case VK_ZOOM: return "VK_ZOOM"; // Zoom key
  case VK_NONAME: return "VK_NONAME"; // Reserved
  case VK_PA1: return "VK_PA1"; // PA1 key
  case VK_OEM_CLEAR: return "VK_OEM_CLEAR"; // Clear key
  }

  return "VK_UNKNOWN";
}

class UseForegroundKeyboardLayoutScope {
 public:
  UseForegroundKeyboardLayoutScope() : original_layout_(GetKeyboardLayout(0)) {
    if (auto window = GetForegroundWindow()) {
      const auto thread_id = GetWindowThreadProcessId(window, nullptr);
      ActivateKeyboardLayout(GetKeyboardLayout(thread_id), 0);
    }
  }

  ~UseForegroundKeyboardLayoutScope() {
    ActivateKeyboardLayout(original_layout_, 0);
  }

  UseForegroundKeyboardLayoutScope(const UseForegroundKeyboardLayoutScope&) = delete;
  UseForegroundKeyboardLayoutScope& operator=(const UseForegroundKeyboardLayoutScope&) = delete;

 private:
  HKL original_layout_ = nullptr;
};

napi_value GetKeyMapImpl(napi_env env, napi_callback_info info) {
  UseForegroundKeyboardLayoutScope use_foreground_keyboard_layout;

  napi_value result;
  NAPI_CALL(env, napi_create_object(env, &result));

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

    napi_value entry;
    NAPI_CALL(env, napi_create_object(env, &entry));

    NAPI_CALL(env, napi_set_named_property_string_utf8(env, entry, "vkey", VKeyToStr(native_keycode)));

    std::string value = GetStrFromKeyPress(native_keycode, 0, keyboard_state, clear_key_code, clear_scan_code);
    NAPI_CALL(env, napi_set_named_property_string_utf8(env, entry, "value", value.c_str()));

    std::string with_shift = GetStrFromKeyPress(native_keycode, kShiftKeyModifierMask, keyboard_state, clear_key_code, clear_scan_code);
    NAPI_CALL(env, napi_set_named_property_string_utf8(env, entry, "withShift", with_shift.c_str()));

    std::string with_alt_gr = GetStrFromKeyPress(native_keycode, kControlKeyModifierMask | kAltKeyModifierMask, keyboard_state, clear_key_code, clear_scan_code);
    NAPI_CALL(env, napi_set_named_property_string_utf8(env, entry, "withAltGr", with_alt_gr.c_str()));

    std::string with_shift_alt_gr = GetStrFromKeyPress(native_keycode, kShiftKeyModifierMask | kControlKeyModifierMask | kAltKeyModifierMask, keyboard_state, clear_key_code, clear_scan_code);
    NAPI_CALL(env, napi_set_named_property_string_utf8(env, entry, "withShiftAltGr", with_shift_alt_gr.c_str()));

    NAPI_CALL(env, napi_set_named_property(env, result, code, entry));
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

napi_value GetCurrentKeyboardLayoutImpl(napi_env env, napi_callback_info info) {
  UseForegroundKeyboardLayoutScope use_foreground_keyboard_layout;

  char chr_layout_name[KL_NAMELENGTH];
  if (!GetKeyboardLayoutName(chr_layout_name)) {
    return napi_fetch_null(env);
  }
  std::string layout_name = chr_layout_name;

  // https://docs.microsoft.com/windows-hardware/manufacture/desktop/windows-language-pack-default-values
  std::string layout_id = GetStringRegKey("System\\CurrentControlSet\\Control\\Keyboard Layouts\\" + layout_name, "Layout Id");
  std::string layout_text = GetStringRegKey("System\\CurrentControlSet\\Control\\Keyboard Layouts\\" + layout_name, "Layout Text");

  napi_value result;
  NAPI_CALL(env, napi_create_object(env, &result));
  NAPI_CALL(env, napi_set_named_property_string_utf8(env, result, "name", layout_name.c_str()));
  NAPI_CALL(env, napi_set_named_property_string_utf8(env, result, "id", layout_id.c_str()));
  NAPI_CALL(env, napi_set_named_property_string_utf8(env, result, "text", layout_text.c_str()));
  return result;
}

class TfInputListener : public ITfInputProcessorProfileActivationSink {
private:
  NotificationCallbackData *data_;
  ULONG ref_count_;
  ITfSource *source_;
  DWORD cookie_;

public:
  explicit TfInputListener(NotificationCallbackData *data) {
    data_ = data;
    ref_count_ = 1;
    source_ = NULL;
    cookie_ = TF_INVALID_COOKIE;
  }

  void StartListening() {
    HRESULT hr;

    ITfThreadMgr* thread_mgr;
    hr = CoCreateInstance(CLSID_TF_ThreadMgr, NULL, CLSCTX_INPROC_SERVER, IID_ITfThreadMgr, (void**)&thread_mgr);
    if (!SUCCEEDED(hr)) {
      printf("native-keymap: Could not create ITfThreadMgr.\n");
      return;
    }

    hr = thread_mgr->QueryInterface(IID_ITfSource, (LPVOID*)&source_);
    if (!SUCCEEDED(hr)) {
      printf("native-keymap: Could not obtain ITfSource.\n");
      thread_mgr->Release();
      return;
    }

    hr = source_->AdviseSink(IID_ITfInputProcessorProfileActivationSink,
      static_cast<ITfInputProcessorProfileActivationSink*>(this),
      &cookie_);

    if (!SUCCEEDED(hr)) {
      printf("native-keymap: Could not register ITfInputProcessorProfileActivationSink.\n");
    }
    thread_mgr->Release();
  }

  void StopListening() {
    if (source_ != NULL) {
      if (cookie_ != TF_INVALID_COOKIE) {
        source_->UnadviseSink(cookie_);
        cookie_ = TF_INVALID_COOKIE;
      }
      source_->Release();
      source_ = NULL;
    }
  }

  virtual ~TfInputListener() {
    this->StopListening();
  }

  virtual HRESULT STDMETHODCALLTYPE OnActivated(
    /* [in] */ DWORD dwProfileType,
    /* [in] */ LANGID langid,
    /* [in] */ __RPC__in REFCLSID clsid,
    /* [in] */ __RPC__in REFGUID catid,
    /* [in] */ __RPC__in REFGUID guidProfile,
    /* [in] */ HKL hkl,
    /* [in] */ DWORD dwFlags) override {

    InvokeNotificationCallback(data_);
    return S_OK;
  }

  // IUnknown methods
  ULONG STDMETHODCALLTYPE AddRef() override {
    return InterlockedIncrement(&ref_count_);
  }

  ULONG STDMETHODCALLTYPE Release() override {
    ULONG newCount = InterlockedDecrement(&ref_count_);
    if (0 == newCount) {
      delete this;
    }
    return newCount;
  }

  virtual HRESULT STDMETHODCALLTYPE QueryInterface(IID const& riid, void** ppvObject) override {
    if (__uuidof(IUnknown) == riid || __uuidof(ITfInputProcessorProfileActivationSink) == riid) {
      *ppvObject = this;
      this->AddRef();
      return S_OK;
    }
    *ppvObject = nullptr;
    return E_FAIL;
  }
};

void RegisterKeyboardLayoutChangeListenerImpl(NotificationCallbackData *data) {
  TfInputListener* listener = new TfInputListener(data);
  listener->StartListening();
  data->listener = listener;
}

void DisposeKeyboardLayoutChangeListenerImpl(NotificationCallbackData *data) {
  TfInputListener* listener = static_cast<TfInputListener*>(data->listener);
  listener->Release();
}

napi_value IsISOKeyboardImpl(napi_env env, napi_callback_info info) {
  return napi_fetch_undefined(env);
}

}  // namespace vscode_keyboard
