// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SYS_STRING_CONVERSIONS_H_
#define SYS_STRING_CONVERSIONS_H_

#import <Foundation/Foundation.h>
#include <string>

std::string CFStringToSTLStringWithEncoding(CFStringRef cfstring);

#endif  // SYS_STRING_CONVERSIONS_H_