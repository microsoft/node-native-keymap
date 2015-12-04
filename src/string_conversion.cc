/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Microsoft Corporation. All rights reserved.
 *  Licensed under the MIT License. See License.txt in the project root for license information.
 *--------------------------------------------------------------------------------------------*/

#include "string_conversion.h"

namespace vscode_keyboard {

wchar_t conversion_buffer[10];

std::string UTF16toUTF8(const wchar_t * in, int length) {
  if (length < 10) {
    for (int i = 0; i < length; ++i) {
      conversion_buffer[i] = in[i];
    }
    conversion_buffer[length] = 0;
    return UTF16to8(conversion_buffer);
  }

  wchar_t *t = new wchar_t[length + 1];
  for (int i = 0; i < length; ++i) {
    t[i] = in[i];
  }
  t[length] = 0;
  std::string result = UTF16to8(t);
  delete []t;

  return result;
}

// http://stackoverflow.com/a/148766
std::string UTF16to8(const wchar_t * in) {
  std::string out;
  unsigned int codepoint = 0;
  for (;  *in != 0;  ++in) {
    if (*in >= 0xd800 && *in <= 0xdbff) {
      codepoint = ((*in - 0xd800) << 10) + 0x10000;
    } else {
      if (*in >= 0xdc00 && *in <= 0xdfff) {
        codepoint |= *in - 0xdc00;
      } else {
        codepoint = *in;
      }

      if (codepoint <= 0x7f) {
        out.append(1, static_cast<char>(codepoint));
      } else if (codepoint <= 0x7ff) {
        out.append(1, static_cast<char>(0xc0 | ((codepoint >> 6) & 0x1f)));
        out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
      } else if (codepoint <= 0xffff) {
        out.append(1, static_cast<char>(0xe0 | ((codepoint >> 12) & 0x0f)));
        out.append(1, static_cast<char>(0x80 | ((codepoint >> 6) & 0x3f)));
        out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
      } else {
        out.append(1, static_cast<char>(0xf0 | ((codepoint >> 18) & 0x07)));
        out.append(1, static_cast<char>(0x80 | ((codepoint >> 12) & 0x3f)));
        out.append(1, static_cast<char>(0x80 | ((codepoint >> 6) & 0x3f)));
        out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
      }
      codepoint = 0;
    }
  }
  return out;
}

}  // namespace vscode_keyboard
