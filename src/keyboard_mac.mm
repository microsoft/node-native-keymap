/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Microsoft Corporation. All rights reserved.
 *  Licensed under the MIT License. See License.txt in the project root for license information.
 *--------------------------------------------------------------------------------------------*/

#include "keymapping.h"

#include "chromium/macros.h"
#include "chromium/sys_string_conversions.h"
#include "string_conversion.h"

#include <Carbon/Carbon.h>
#import <Cocoa/Cocoa.h>

#define USB_KEYMAP(usb, evdev, xkb, win, mac, code, id) \
  { usb, mac, code }
#define USB_KEYMAP_DECLARATION const vscode_keyboard::KeycodeMapEntry usb_keycode_map[] =
#include "chromium/keycode_converter_data.inc"
#undef USB_KEYMAP
#undef USB_KEYMAP_DECLARATION

namespace {

static Nan::Persistent<v8::Function> keyboard_change_cb_;
uv_async_t async_handle_;

std::pair<bool, std::string> ConvertKeyCodeToText(const UCKeyboardLayout* keyboardLayout,
                                                  int mac_key_code, int modifiers) {
  int mac_modifiers = 0;
  if (modifiers & kShiftKeyModifierMask) mac_modifiers |= shiftKey;
  if (modifiers & kControlKeyModifierMask) mac_modifiers |= controlKey;
  if (modifiers & kAltKeyModifierMask) mac_modifiers |= optionKey;
  if (modifiers & kMetaKeyModifierMask) mac_modifiers |= cmdKey;

  // Convert EventRecord modifiers to format UCKeyTranslate accepts. See docs
  // on UCKeyTranslate for more info.
  UInt32 modifier_key_state = (mac_modifiers >> 8) & 0xFF;

  UInt32 dead_key_state = 0;
  UniCharCount char_count = 0;
  UniChar character = 0;
  OSStatus status = UCKeyTranslate(
      keyboardLayout, static_cast<UInt16>(mac_key_code), kUCKeyActionDown, modifier_key_state,
      LMGetKbdLast(), kUCKeyTranslateNoDeadKeysBit, &dead_key_state, 1, &char_count, &character);

  bool isDeadKey = false;
  if (status == noErr && char_count == 0 && dead_key_state != 0) {
    isDeadKey = true;
    status = UCKeyTranslate(keyboardLayout, static_cast<UInt16>(mac_key_code), kUCKeyActionDown,
                            modifier_key_state, LMGetKbdLast(), kUCKeyTranslateNoDeadKeysBit,
                            &dead_key_state, 1, &char_count, &character);
  }

  if (status == noErr && char_count == 1 && !std::iscntrl(character)) {
    wchar_t value = character;
    return std::make_pair(isDeadKey, vscode_keyboard::UTF16toUTF8(&value, 1));
  }
  return std::make_pair(false, std::string());
}

void OnKeyboardChangeNotificationReceived(CFNotificationCenterRef center, void* observer,
                                          CFStringRef name, const void* object,
                                          CFDictionaryRef userInfo) {
  uv_async_send(&async_handle_);
}

void OnKeyboardChangeCallback(uv_async_t* async_handle) {
  Nan::HandleScope scope;
  Nan::AsyncResource resource("vscode_keyboard::OnKeyboardChangeCallback");
  if (!keyboard_change_cb_.IsEmpty()) {
    resource.runInAsyncScope(Nan::GetCurrentContext()->Global(), Nan::New(keyboard_change_cb_), 0,
                             nullptr);
  }
}

}  // namespace

namespace vscode_keyboard {

NAN_METHOD(GetKeyMap) {
  v8::Local<v8::Object> result = Nan::New<v8::Object>();
  TISInputSourceRef source = TISCopyCurrentKeyboardInputSource();
  CFDataRef layout_data =
      static_cast<CFDataRef>((TISGetInputSourceProperty(source, kTISPropertyUnicodeKeyLayoutData)));
  if (!layout_data) {
    // TISGetInputSourceProperty returns null with  Japanese keyboard layout.
    // Using TISCopyCurrentKeyboardLayoutInputSource to fix NULL return.
    source = TISCopyCurrentKeyboardLayoutInputSource();
    layout_data = static_cast<CFDataRef>(
        (TISGetInputSourceProperty(source, kTISPropertyUnicodeKeyLayoutData)));
    if (!layout_data) {
      // https://developer.apple.com/library/mac/documentation/TextFonts/Reference/TextInputSourcesReference/#//apple_ref/c/func/TISGetInputSourceProperty
      info.GetReturnValue().Set(result);
      return;
    }
  }

  const UCKeyboardLayout* keyboardLayout =
      reinterpret_cast<const UCKeyboardLayout*>(CFDataGetBytePtr(layout_data));

  size_t cnt = sizeof(usb_keycode_map) / sizeof(usb_keycode_map[0]);

  for (size_t i = 0; i < cnt; ++i) {
    const char* code = usb_keycode_map[i].code;
    int native_keycode = usb_keycode_map[i].native_keycode;

    if (!code || native_keycode >= 0xffff) {
      continue;
    }

    v8::Local<v8::Object> entry = Nan::New<v8::Object>();

    auto value = ConvertKeyCodeToText(keyboardLayout, native_keycode, 0);
    Nan::Set(entry, Nan::New("value").ToLocalChecked(),
             Nan::New<v8::String>(value.second.data(), value.second.length()).ToLocalChecked());
    Nan::Set(entry, Nan::New("valueIsDeadKey").ToLocalChecked(),
             Nan::New<v8::Boolean>(value.first));

    auto withShift = ConvertKeyCodeToText(keyboardLayout, native_keycode, kShiftKeyModifierMask);
    Nan::Set(
        entry, Nan::New("withShift").ToLocalChecked(),
        Nan::New<v8::String>(withShift.second.data(), withShift.second.length()).ToLocalChecked());
    Nan::Set(entry, Nan::New("withShiftIsDeadKey").ToLocalChecked(),
             Nan::New<v8::Boolean>(withShift.first));

    auto withAltGr = ConvertKeyCodeToText(keyboardLayout, native_keycode, kAltKeyModifierMask);
    Nan::Set(
        entry, Nan::New("withAltGr").ToLocalChecked(),
        Nan::New<v8::String>(withAltGr.second.data(), withAltGr.second.length()).ToLocalChecked());
    Nan::Set(entry, Nan::New("withAltGrIsDeadKey").ToLocalChecked(),
             Nan::New<v8::Boolean>(withAltGr.first));

    auto withShiftAltGr = ConvertKeyCodeToText(keyboardLayout, native_keycode,
                                               kShiftKeyModifierMask | kAltKeyModifierMask);
    Nan::Set(entry, Nan::New("withShiftAltGr").ToLocalChecked(),
             Nan::New<v8::String>(withShiftAltGr.second.data(), withShiftAltGr.second.length())
                 .ToLocalChecked());
    Nan::Set(entry, Nan::New("withShiftAltGrIsDeadKey").ToLocalChecked(),
             Nan::New<v8::Boolean>(withShiftAltGr.first));

    Nan::Set(result, Nan::New(code).ToLocalChecked(), entry);
  }

  info.GetReturnValue().Set(result);
}

NAN_METHOD(GetCurrentKeyboardLayout) {
  v8::Local<v8::Object> result = Nan::New<v8::Object>();

  TISInputSourceRef source = TISCopyCurrentKeyboardInputSource();
  std::string source_id = CFStringToSTLStringWithEncoding(
      reinterpret_cast<CFStringRef>(TISGetInputSourceProperty(source, kTISPropertyInputSourceID)));
  Nan::Set(result, Nan::New("id").ToLocalChecked(),
           Nan::New<v8::String>(source_id.data(), source_id.length()).ToLocalChecked());

  TISInputSourceRef nameSource = TISCopyCurrentKeyboardInputSource();
  CFStringRef localizedName = (CFStringRef) TISGetInputSourceProperty(nameSource, kTISPropertyLocalizedName);
  if(localizedName) {
    result->Set(String::NewFromUtf8(isolate, "localizedName"), String::NewFromUtf8(isolate, std::string([(NSString *)localizedName UTF8String]).c_str()));
  }

  NSArray* languages = (NSArray *) TISGetInputSourceProperty(source, kTISPropertyInputSourceLanguages);
  if (languages && [languages count] > 0) {
    NSString* lang = [languages objectAtIndex:0];
    if (lang) {
      std::string lang_str = CFStringToSTLStringWithEncoding(reinterpret_cast<CFStringRef>(lang));
      Nan::Set(result, Nan::New("lang").ToLocalChecked(),
               Nan::New<v8::String>(lang_str.data(), lang_str.length()).ToLocalChecked());
    }
  }

  info.GetReturnValue().Set(result);
}

NAN_METHOD(OnDidChangeKeyboardLayout) {
  v8::Local<v8::Function> callback = Nan::To<v8::Function>(info[0]).ToLocalChecked();
  keyboard_change_cb_.Reset(callback);

  uv_async_init(Nan::GetCurrentEventLoop(), &async_handle_, &OnKeyboardChangeCallback);

  // add an observer
  CFNotificationCenterAddObserver(CFNotificationCenterGetDistributedCenter(), nullptr,
                                  &OnKeyboardChangeNotificationReceived,
                                  kTISNotifySelectedKeyboardInputSourceChanged, nullptr,
                                  CFNotificationSuspensionBehaviorDeliverImmediately);
}

NAN_METHOD(IsISOKeyboard) {
  if (KBGetLayoutType(LMGetKbdType()) == kKeyboardISO) {
    info.GetReturnValue().Set(Nan::New<v8::Boolean>(true));
  } else {
    info.GetReturnValue().Set(Nan::New<v8::Boolean>(false));
  }
}

}  // namespace vscode_keyboard
