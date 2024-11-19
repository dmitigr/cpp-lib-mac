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

#include <algorithm>
#include <filesystem>
#include <utility>

#include <CoreFoundation/CoreFoundation.h>

namespace dmitigr::mac::cf {

template<typename T>
class Type_guard final {
public:
  ~Type_guard()
  {
    CFRelease(ref_);
  }

  Type_guard(const Type_guard&) = delete;
  Type_guard& operator=(const Type_guard&) = delete;

  Type_guard() = default;

  explicit Type_guard(T ref)
    : ref_{ref}
  {}

  Type_guard(Type_guard&& rhs) noexcept
    : ref_{rhs.ref}
  {
    rhs.ref_ = {};
  }

  Type_guard& operator=(Type_guard&& rhs) noexcept
  {
    Type_guard tmp{std::move(rhs)};
    swap(tmp);
    return *this;
  }

  void swap(Type_guard& rhs) noexcept
  {
    using std::swap;
    swap(ref_, rhs.ref_);
  }

  T ref() const noexcept
  {
    return ref_;
  }

  explicit operator bool() const noexcept
  {
    return static_cast<bool>(ref());
  }

private:
  T ref_{};
};

// -----------------------------------------------------------------------------

using Bundle = Type_guard<CFBundleRef>;
using String = Type_guard<CFStringRef>;
using Url = Type_guard<CFURLRef>;

// -----------------------------------------------------------------------------
// String
// -----------------------------------------------------------------------------

inline String string_create_no_copy(const char* const str,
  const CFStringEncoding encoding = kCFStringEncodingUTF8)
{
  return String{CFStringCreateWithCStringNoCopy(kCFAllocatorDefault, str,
    encoding, kCFAllocatorNull)};
}

// -----------------------------------------------------------------------------
// Bundle
// -----------------------------------------------------------------------------

inline Bundle bundle_create(const Url& url)
{
  return Bundle{CFBundleCreate(kCFAllocatorDefault, url.ref())};
}

inline Bundle bundle_create(const std::filesystem::path& path)
{
  const auto path_ref = string_create_no_copy(path.c_str());
  const Url url{CFURLCreateWithFileSystemPath(kCFAllocatorDefault,
    path_ref.ref(), kCFURLPOSIXPathStyle, is_directory(path))};
  return Bundle{url};
}

inline void* bundle_function_pointer_for_name(const Bundle& bundle,
  const char* const name) const noexcept
{
  return CFBundleGetFunctionPointerForName(bundle.ref(), CFSTR(name));
}

} // namespace dmitigr::mac::cf

#endif  // DMITIGR_MAC_CF_HPP
