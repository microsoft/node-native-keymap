// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chromium/sys_string_conversions.h"

#include <vector>

std::string CFStringToSTLStringWithEncoding(CFStringRef cfstring) {
  CFIndex length = CFStringGetLength(cfstring);
  if (length == 0) return std::string();

  CFRange whole_string = CFRangeMake(0, length);
  CFIndex out_size;
  CFIndex converted = CFStringGetBytes(cfstring, whole_string, kCFStringEncodingUTF8,
                                       0,      // lossByte
                                       false,  // isExternalRepresentation
                                       NULL,   // buffer
                                       0,      // maxBufLen
                                       &out_size);
  if (converted == 0 || out_size == 0) return std::string();

  // out_size is the number of UInt8-sized units needed in the destination.
  // A buffer allocated as UInt8 units might not be properly aligned to
  // contain elements of std::string::value_type.  Use a container for the
  // proper value_type, and convert out_size by figuring the number of
  // value_type elements per UInt8.  Leave room for a NUL terminator.
  typename std::string::size_type elements =
      out_size * sizeof(UInt8) / sizeof(typename std::string::value_type) + 1;

  std::vector<typename std::string::value_type> out_buffer(elements);
  converted = CFStringGetBytes(cfstring, whole_string, kCFStringEncodingUTF8,
                               0,      // lossByte
                               false,  // isExternalRepresentation
                               reinterpret_cast<UInt8*>(&out_buffer[0]), out_size,
                               NULL);  // usedBufLen
  if (converted == 0) return std::string();

  out_buffer[elements - 1] = '\0';
  return std::string(&out_buffer[0], elements - 1);
}
