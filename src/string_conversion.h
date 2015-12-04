#ifndef STRING_CONVERSION_H_
#define STRING_CONVERSION_H_

#include <string>

namespace vscode_keyboard {

std::string UTF16to8(const wchar_t * in);
std::string UTF16toUTF8(const wchar_t * in, int length);

}  // namespace vscode_keyboard

#endif  // STRING_CONVERSION_H_
