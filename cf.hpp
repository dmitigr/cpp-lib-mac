// -*- C++ -*-
//
// Copyright 2024 Dmitry Igrishin
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef DMITIGR_MAC_CF_HPP
#define DMITIGR_MAC_CF_HPP

#ifndef __APPLE__
#error dmitigr/mac/cf.hpp is usable only on macOS!
#endif

#include <CoreFoundation/CoreFoundation.h>

namespace dmitigr::mac::cf {

template<typename T>
struct Type_guard final {
  ~Type_guard()
  {
    CFRelease(ref);
  }

  Type_guard(const Type_guard&) = delete;
  Type_guard& operator=(const Type_guard&) = delete;

  Type_guard() = default;

  explicit Type_guard(T ref)
    : ref{ref}
  {}

  T ref{};
};

} // namespace dmitigr::mac::cf

#endif  // DMITIGR_MAC_CF_HPP
