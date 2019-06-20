/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Microsoft Corporation. All rights reserved.
 *  Licensed under the MIT License. See License.txt in the project root for license information.
 *--------------------------------------------------------------------------------------------*/

#import <Cocoa/Cocoa.h>
#include <Carbon/Carbon.h>

#include "string_conversion.h"
#include "keymapping.h"
#include "../deps/chromium/macros.h"

namespace {

std::pair<bool,std::string> ConvertKeyCodeToText(const UCKeyboardLayout* keyboardLayout, int mac_key_code, int modifiers) {

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

  bool isDeadKey = false;
  if (status == noErr && char_count == 0 && dead_key_state != 0) {
    isDeadKey = true;
    status = UCKeyTranslate(
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
  }

  if (status == noErr && char_count == 1 && !std::iscntrl(character)) {
    wchar_t value = character;
    return std::make_pair(isDeadKey, vscode_keyboard::UTF16toUTF8(&value, 1));
  }
  return std::make_pair(false, std::string());
}

} // namespace

namespace vscode_keyboard {


using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::Object;
using v8::String;
using v8::Boolean;
using v8::Array;
using v8::Value;
using v8::Null;

#define USB_KEYMAP(usb, evdev, xkb, win, mac, code, id) {usb, mac, code}
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

  Local<String> _valueIsDeadKey = String::NewFromUtf8(isolate, "valueIsDeadKey");
  Local<String> _withShiftIsDeadKey = String::NewFromUtf8(isolate, "withShiftIsDeadKey");
  Local<String> _withAltGrIsDeadKey = String::NewFromUtf8(isolate, "withAltGrIsDeadKey");
  Local<String> _withShiftAltGrIsDeadKey = String::NewFromUtf8(isolate, "withShiftAltGrIsDeadKey");

  Local<Boolean> _true = v8::True(isolate);
  Local<Boolean> _false = v8::False(isolate);

  TISInputSourceRef source = TISCopyCurrentKeyboardInputSource();
  CFDataRef layout_data = static_cast<CFDataRef>((TISGetInputSourceProperty(source, kTISPropertyUnicodeKeyLayoutData)));
  if (!layout_data) {
    // TISGetInputSourceProperty returns null with  Japanese keyboard layout.
    // Using TISCopyCurrentKeyboardLayoutInputSource to fix NULL return.
    source = TISCopyCurrentKeyboardLayoutInputSource();
    layout_data = static_cast<CFDataRef>((TISGetInputSourceProperty(source, kTISPropertyUnicodeKeyLayoutData)));
    if (!layout_data) {
      // https://developer.apple.com/library/mac/documentation/TextFonts/Reference/TextInputSourcesReference/#//apple_ref/c/func/TISGetInputSourceProperty
      args.GetReturnValue().Set(result);
      return;
    }
  }

  const UCKeyboardLayout* keyboardLayout = reinterpret_cast<const UCKeyboardLayout*>(CFDataGetBytePtr(layout_data));

  size_t cnt = sizeof(usb_keycode_map) / sizeof(usb_keycode_map[0]);

  for (size_t i = 0; i < cnt; ++i) {
    const char *code = usb_keycode_map[i].code;
    int native_keycode = usb_keycode_map[i].native_keycode;

    if (!code || native_keycode >= 0xffff) {
      continue;
    }

    Local<Object> entry = Object::New(isolate);

    std::pair<bool,std::string> value = ConvertKeyCodeToText(keyboardLayout, native_keycode, 0);
    entry->Set(_value, String::NewFromUtf8(isolate, value.second.c_str()));
    entry->Set(_valueIsDeadKey, value.first ? _true : _false);

    std::pair<bool,std::string> withShift = ConvertKeyCodeToText(keyboardLayout, native_keycode, kShiftKeyModifierMask);
    entry->Set(_withShift, String::NewFromUtf8(isolate, withShift.second.c_str()));
    entry->Set(_withShiftIsDeadKey, withShift.first ? _true : _false);

    std::pair<bool,std::string> withAltGr = ConvertKeyCodeToText(keyboardLayout, native_keycode, kAltKeyModifierMask);
    entry->Set(_withAltGr, String::NewFromUtf8(isolate, withAltGr.second.c_str()));
    entry->Set(_withAltGrIsDeadKey, withAltGr.first ? _true : _false);

    std::pair<bool,std::string> withShiftAltGr = ConvertKeyCodeToText(keyboardLayout, native_keycode, kShiftKeyModifierMask | kAltKeyModifierMask);
    entry->Set(_withShiftAltGr, String::NewFromUtf8(isolate, withShiftAltGr.second.c_str()));
    entry->Set(_withShiftAltGrIsDeadKey, withShiftAltGr.first ? _true : _false);

    result->Set(String::NewFromUtf8(isolate, code), entry);
  }
  args.GetReturnValue().Set(result);
}

void _GetCurrentKeyboardLayout(const v8::FunctionCallbackInfo<v8::Value>& args) {
  Isolate* isolate = args.GetIsolate();
  Local<Object> result = Object::New(isolate);

  TISInputSourceRef source = TISCopyCurrentKeyboardInputSource();
  CFStringRef sourceId = (CFStringRef) TISGetInputSourceProperty(source, kTISPropertyInputSourceID);
  if(sourceId) {
    result->Set(String::NewFromUtf8(isolate, "id"), String::NewFromUtf8(isolate, std::string([(NSString *)sourceId UTF8String]).c_str()));
  }

  TISInputSourceRef nameSource = TISCopyCurrentKeyboardInputSource();
  CFStringRef localizedName = (CFStringRef) TISGetInputSourceProperty(nameSource, kTISPropertyLocalizedName);
  if(localizedName) {
    result->Set(String::NewFromUtf8(isolate, "localizedName"), String::NewFromUtf8(isolate, std::string([(NSString *)localizedName UTF8String]).c_str()));
  }

  NSArray* languages = (NSArray *) TISGetInputSourceProperty(source, kTISPropertyInputSourceLanguages);
  if (languages && [languages count] > 0) {
    NSString* lang = [languages objectAtIndex:0];
    if (lang) {
      result->Set(String::NewFromUtf8(isolate, "lang"), String::NewFromUtf8(isolate, std::string([lang UTF8String]).c_str()));
    }
  }

  args.GetReturnValue().Set(result);
}

static v8::Persistent<v8::Function, v8::CopyablePersistentTraits<v8::Function>> _cb;

uv_loop_t *loop = uv_default_loop();
uv_async_t async;

void notificationCallback(CFNotificationCenterRef center, void *observer, CFStringRef name, const void *object, CFDictionaryRef userInfo) {
  uv_async_send(&async);
}

static void asyncSendHandler(uv_async_t *handle) {
  auto isolate = Isolate::GetCurrent();
  v8::HandleScope scope(isolate);
  auto context = isolate->GetCurrentContext();
  auto global = context->Global();

  const int argc = 0;
  v8::Handle<v8::Value> argv[argc];

  auto fn = Local<v8::Function>::New(isolate, _cb);
  fn->Call(global, argc, argv);
}

void _OnDidChangeKeyboardLayout(const v8::FunctionCallbackInfo<v8::Value>& args) {

  uv_async_init(loop, &async, (uv_async_cb)asyncSendHandler);

  auto isolate = Isolate::GetCurrent();
  v8::Handle<v8::Function> arg0 = v8::Handle<v8::Function>::Cast(args[0]);
  v8::Persistent<v8::Function> cb(isolate, arg0);
  _cb = cb;

  CFNotificationCenterRef center = CFNotificationCenterGetDistributedCenter();

  // add an observer
  CFNotificationCenterAddObserver(center, NULL, notificationCallback,
    kTISNotifySelectedKeyboardInputSourceChanged, NULL,
    CFNotificationSuspensionBehaviorDeliverImmediately
  );
}

void _isISOKeyboard(const v8::FunctionCallbackInfo<v8::Value>& args) {
  Isolate* isolate = args.GetIsolate();
  if (KBGetLayoutType(LMGetKbdType()) == kKeyboardISO) {
    args.GetReturnValue().Set(Boolean::New(isolate, true));
  } else {
    args.GetReturnValue().Set(Boolean::New(isolate, false));
  }
}

} // namespace vscode_keyboard
