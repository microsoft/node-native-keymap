/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Microsoft Corporation. All rights reserved.
 *  Licensed under the MIT License. See License.txt in the project root for license information.
 *--------------------------------------------------------------------------------------------*/

#import <Cocoa/Cocoa.h>
#include <Carbon/Carbon.h>

#include "string_conversion.h"
#include "keymapping.h"
#include "../deps/chromium/keyboard_code_conversion_mac.h"
#include "../deps/chromium/macros.h"

namespace {

std::string ConvertKeyCodeToText(const UCKeyboardLayout* keyboardLayout, int mac_key_code, int modifiers) {

  int mac_modifiers = 0;
  if (modifiers & kShiftKeyModifierMask)
    mac_modifiers |= shiftKey;
  if (modifiers & kControlKeyModifierMask)
    mac_modifiers |= controlKey;
  if (modifiers & kAltKeyModifierMask)
    mac_modifiers |= optionKey;
  if (modifiers & kMetaKeyModifierMask)
    mac_modifiers |= cmdKey;

  // Convert EventRecord modifiers to format UCKeyTranslate accepts. See docs
  // on UCKeyTranslate for more info.
  UInt32 modifier_key_state = (mac_modifiers >> 8) & 0xFF;

  UInt32 dead_key_state = 0;
  UniCharCount char_count = 0;
  UniChar character = 0;
  OSStatus status = UCKeyTranslate(
      keyboardLayout,
      static_cast<UInt16>(mac_key_code),
      kUCKeyActionDown,
      modifier_key_state,
      LMGetKbdLast(),
      kUCKeyTranslateNoDeadKeysBit,
      &dead_key_state,
      1,
      &char_count,
      &character);

  if (status == noErr && char_count == 1 && !std::iscntrl(character)) {
    wchar_t value = character;
    return vscode_keyboard::UTF16toUTF8(&value, 1);
  }
  return std::string();
}

} // namespace

namespace vscode_keyboard {

std::vector<KeyMapping> GetKeyMapping() {
  std::vector<KeyMapping> result;

  TISInputSourceRef source = TISCopyCurrentKeyboardInputSource();
  CFDataRef layout_data = static_cast<CFDataRef>((TISGetInputSourceProperty(source, kTISPropertyUnicodeKeyLayoutData)));
  if(layout_data == NULL) {
    // https://developer.apple.com/library/mac/documentation/TextFonts/Reference/TextInputSourcesReference/#//apple_ref/c/func/TISGetInputSourceProperty
    // might be NULL
    return result;
  }
  const UCKeyboardLayout* keyboardLayout = reinterpret_cast<const UCKeyboardLayout*>(CFDataGetBytePtr(layout_data));

  for (size_t i = 0; i < arraysize(ui::kKeyCodesMap); ++i) {
    ui::KeyboardCode key_code = ui::kKeyCodesMap[i].keycode;
    int mac_key_code = ui::kKeyCodesMap[i].macKeycode;
    if(mac_key_code < 0) {
      continue;
    }

    std::string value = ConvertKeyCodeToText(keyboardLayout, mac_key_code, 0);
    std::string withShift = ConvertKeyCodeToText(keyboardLayout, mac_key_code, kShiftKeyModifierMask);
    std::string withAltGr = ConvertKeyCodeToText(keyboardLayout, mac_key_code, kAltKeyModifierMask);
    std::string withShiftAltGr = ConvertKeyCodeToText(keyboardLayout, mac_key_code, kShiftKeyModifierMask | kAltKeyModifierMask);

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

} // namespace vscode_keyboard
