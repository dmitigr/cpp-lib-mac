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
class Handle final {
public:
  ~Handle()
  {
    CFRelease(native_);
  }

  Handle(const Handle&) = delete;
  Handle& operator=(const Handle&) = delete;

  Handle() = default;

  explicit Handle(T native)
    : native_{native}
  {}

  Handle(Handle&& rhs) noexcept
    : native_{rhs.native}
  {
    rhs.native_ = {};
  }

  Handle& operator=(Handle&& rhs) noexcept
  {
    Handle tmp{std::move(rhs)};
    swap(tmp);
    return *this;
  }

  void swap(Handle& rhs) noexcept
  {
    using std::swap;
    swap(native_, rhs.native_);
  }

  T native() const noexcept
  {
    return native_;
  }

  explicit operator bool() const noexcept
  {
    return static_cast<bool>(native());
  }

private:
  T native_{};
};

// -----------------------------------------------------------------------------
// Handle aliases
// -----------------------------------------------------------------------------

using Bundle = Handle<CFBundleRef>;
using String = Handle<CFStringRef>;
using Url = Handle<CFURLRef>;

// -----------------------------------------------------------------------------
// String
// -----------------------------------------------------------------------------

namespace string {

inline String create_no_copy(const char* const str,
  const CFStringEncoding encoding = kCFStringEncodingUTF8)
{
  return String{CFStringCreateWithCStringNoCopy(kCFAllocatorDefault, str,
    encoding, kCFAllocatorNull)};
}

} // namespace string

// -----------------------------------------------------------------------------
// Bundle
// -----------------------------------------------------------------------------

namespace bundle {

inline Bundle create(const Url& url)
{
  return Bundle{CFBundleCreate(kCFAllocatorDefault, url.native())};
}

inline Bundle create(const std::filesystem::path& path)
{
  const auto path_hdl = string_create_no_copy(path.c_str());
  const Url url{CFURLCreateWithFileSystemPath(kCFAllocatorDefault,
    path_hdl.native(), kCFURLPOSIXPathStyle, is_directory(path))};
  return bundle_create(url);
}

inline void* function_pointer_for_name(const Bundle& bundle,
  const char* const name)
{
  return CFBundleGetFunctionPointerForName(bundle.native(),
    string_create_no_copy(name).native());
}

} // namespace bundle

} // namespace dmitigr::mac::cf

#endif  // DMITIGR_MAC_CF_HPP
