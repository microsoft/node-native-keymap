#ifndef KEYBOARD_H_
#define KEYBOARD_H_

#include <string>
#include <vector>
#include "../deps/chromium/keyboard_codes.h"

namespace vscode_keyboard {

struct KeyMapping {
  ui::KeyboardCode key_code;
  std::string value;
  std::string withShift;
  std::string withAltGr;
  std::string withShiftAltGr;
};
typedef struct KeyMapping KeyMapping;

std::vector<KeyMapping> GetKeyMapping();

}  // namespace vscode_keyboard

#endif  // KEYBOARD_H_