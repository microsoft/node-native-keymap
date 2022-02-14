/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Microsoft Corporation. All rights reserved.
 *  Licensed under the MIT License. See License.txt in the project root for license information.
 *--------------------------------------------------------------------------------------------*/

#import <Cocoa/Cocoa.h>
#include <Carbon/Carbon.h>

#include "string_conversion.h"
#include "keymapping.h"
#include "common.h"
#include "../deps/chromium/macros.h"

namespace {

std::pair<bool,std::string> ConvertKeyCodeToText(const UCKeyboardLayout* keyboard_layout, int mac_key_code, int modifiers) {

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
      keyboard_layout,
      static_cast<UInt16>(mac_key_code),
      kUCKeyActionDown,
      modifier_key_state,
      LMGetKbdLast(),
      kUCKeyTranslateNoDeadKeysBit,
      &dead_key_state,
      1,
      &char_count,
      &character);

  bool is_dead_key = false;
  if (status == noErr && char_count == 0 && dead_key_state != 0) {
    is_dead_key = true;
    status = UCKeyTranslate(
        keyboard_layout,
        static_cast<UInt16>(mac_key_code),
        kUCKeyActionDown,
        modifier_key_state,
        LMGetKbdLast(),
        kUCKeyTranslateNoDeadKeysBit,
        &dead_key_state,
        1,
        &char_count,
        &character);
  }

  if (status == noErr && char_count == 1 && !std::iscntrl(character)) {
    wchar_t value = character;
    return std::make_pair(is_dead_key, vscode_keyboard::UTF16toUTF8(&value, 1));
  }
  return std::make_pair(false, std::string());
}

} // namespace

namespace vscode_keyboard {


#define DOM_CODE(usb, evdev, xkb, win, mac, code, id) {usb, mac, code}
#define DOM_CODE_DECLARATION const KeycodeMapEntry usb_keycode_map[] =
#include "../deps/chromium/dom_code_data.inc"
#undef DOM_CODE
#undef DOM_CODE_DECLARATION

napi_value GetKeyMapImpl(napi_env env, napi_callback_info info) {

  napi_value result;
  NAPI_CALL(env, napi_create_object(env, &result));

  TISInputSourceRef source = TISCopyCurrentKeyboardInputSource();
  CFDataRef layout_data = static_cast<CFDataRef>((TISGetInputSourceProperty(source, kTISPropertyUnicodeKeyLayoutData)));
  if (!layout_data) {
    // TISGetInputSourceProperty returns null with  Japanese keyboard layout.
    // Using TISCopyCurrentKeyboardLayoutInputSource to fix NULL return.
    source = TISCopyCurrentKeyboardLayoutInputSource();
    layout_data = static_cast<CFDataRef>((TISGetInputSourceProperty(source, kTISPropertyUnicodeKeyLayoutData)));
    if (!layout_data) {
      // https://developer.apple.com/library/mac/documentation/TextFonts/Reference/TextInputSourcesReference/#//apple_ref/c/func/TISGetInputSourceProperty
      return result;
    }
  }

  const UCKeyboardLayout* keyboard_layout = reinterpret_cast<const UCKeyboardLayout*>(CFDataGetBytePtr(layout_data));

  size_t cnt = sizeof(usb_keycode_map) / sizeof(usb_keycode_map[0]);

  napi_value true_value;
  NAPI_CALL(env, napi_get_boolean(env, true, &true_value));

  napi_value false_value;
  NAPI_CALL(env, napi_get_boolean(env, false, &false_value));

  for (size_t i = 0; i < cnt; ++i) {
    const char *code = usb_keycode_map[i].code;
    int native_keycode = usb_keycode_map[i].native_keycode;

    if (!code || native_keycode >= 0xffff) {
      continue;
    }

    napi_value entry;
    NAPI_CALL(env, napi_create_object(env, &entry));

    {
      std::pair<bool,std::string> value = ConvertKeyCodeToText(keyboard_layout, native_keycode, 0);
      NAPI_CALL(env, napi_set_named_property_string_utf8(env, entry, "value", value.second.c_str()));
      NAPI_CALL(env, napi_set_named_property(env, entry, "valueIsDeadKey", value.first ? true_value : false_value));
    }

    {
      std::pair<bool,std::string> with_shift = ConvertKeyCodeToText(keyboard_layout, native_keycode, kShiftKeyModifierMask);
      NAPI_CALL(env, napi_set_named_property_string_utf8(env, entry, "withShift", with_shift.second.c_str()));
      NAPI_CALL(env, napi_set_named_property(env, entry, "withShiftIsDeadKey", with_shift.first ? true_value : false_value));
    }

    {
      std::pair<bool,std::string> with_alt_gr = ConvertKeyCodeToText(keyboard_layout, native_keycode, kAltKeyModifierMask);
      NAPI_CALL(env, napi_set_named_property_string_utf8(env, entry, "withAltGr", with_alt_gr.second.c_str()));
      NAPI_CALL(env, napi_set_named_property(env, entry, "withAltGrIsDeadKey", with_alt_gr.first ? true_value : false_value));
    }

    {
      std::pair<bool,std::string> with_shift_alt_gr = ConvertKeyCodeToText(keyboard_layout, native_keycode, kShiftKeyModifierMask | kAltKeyModifierMask);
      NAPI_CALL(env, napi_set_named_property_string_utf8(env, entry, "withShiftAltGr", with_shift_alt_gr.second.c_str()));
      NAPI_CALL(env, napi_set_named_property(env, entry, "withShiftAltGrIsDeadKey", with_shift_alt_gr.first ? true_value : false_value));
    }

    NAPI_CALL(env, napi_set_named_property(env, result, code, entry));
  }
  return result;
}

napi_value GetCurrentKeyboardLayoutImpl(napi_env env, napi_callback_info info) {

  napi_value result;
  NAPI_CALL(env, napi_create_object(env, &result));

  TISInputSourceRef source = TISCopyCurrentKeyboardInputSource();
  CFStringRef source_id = (CFStringRef) TISGetInputSourceProperty(source, kTISPropertyInputSourceID);
  if(source_id) {
    NAPI_CALL(env, napi_set_named_property_string_utf8(env, result, "id", std::string([(NSString *)source_id UTF8String]).c_str()));
  }

  TISInputSourceRef name_source = TISCopyCurrentKeyboardInputSource();
  CFStringRef localized_name = (CFStringRef) TISGetInputSourceProperty(name_source, kTISPropertyLocalizedName);
  if(localized_name) {
    NAPI_CALL(env, napi_set_named_property_string_utf8(env, result, "localizedName", std::string([(NSString *)localized_name UTF8String]).c_str()));
  }

  NSArray* languages = (NSArray *) TISGetInputSourceProperty(source, kTISPropertyInputSourceLanguages);
  if (languages && [languages count] > 0) {
    NSString* lang = [languages objectAtIndex:0];
    if (lang) {
      NAPI_CALL(env, napi_set_named_property_string_utf8(env, result, "lang", std::string([lang UTF8String]).c_str()));
    }
  }

  return result;
}

void NotificationCallback(CFNotificationCenterRef center, void *observer, CFStringRef name, const void *object, CFDictionaryRef userInfo) {
  NotificationCallbackData *data = (NotificationCallbackData *)observer;
  InvokeNotificationCallback(data);
}

void RegisterKeyboardLayoutChangeListenerImpl(NotificationCallbackData *data) {
  CFNotificationCenterRef center = CFNotificationCenterGetDistributedCenter();

  // add an observer
  CFNotificationCenterAddObserver(center, data, NotificationCallback,
    kTISNotifySelectedKeyboardInputSourceChanged, NULL,
    CFNotificationSuspensionBehaviorDeliverImmediately
  );
}

void DisposeKeyboardLayoutChangeListenerImpl(NotificationCallbackData *data) {
  CFNotificationCenterRef center = CFNotificationCenterGetDistributedCenter();

  // remove the observer
  CFNotificationCenterRemoveObserver(center, data,
    kTISNotifySelectedKeyboardInputSourceChanged, NULL
  );
}

napi_value IsISOKeyboardImpl(napi_env env, napi_callback_info info) {
  if (KBGetLayoutType(LMGetKbdType()) == kKeyboardISO) {
    return napi_fetch_boolean(env, true);
  } else {
    return napi_fetch_boolean(env, false);
  }
}

} // namespace vscode_keyboard
