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

private:
  T ref_{};
};

// -----------------------------------------------------------------------------

class Bundle final {
public:
  Bundle() = default;

  explicit Bundle(CFURLRef url)
    : handle_{CFBundleCreate(kCFAllocatorDefault, url)}
  {}

  explicit Bundle(const std::filesystem::path& path)
  {
    const Type_guard<CFURLRef> url{
      CFURLCreateWithFileSystemPath(kCFAllocatorDefault, CFSTR(path.c_str()),
        kCFURLPOSIXPathStyle, is_directory(path))};
    handle_ = Bundle{url.ref()};
  }

  void swap(Bundle& rhs) noexcept
  {
    handle_.swap(rhs);
  }

  auto ref() const noexcept
  {
    return handle_.ref();
  }

  void* function_pointer_for_name(const char* const name) const noexcept
  {
    return CFBundleGetFunctionPointerForName(ref(), CFSTR(name));
  }

private:
  Type_guard<CFBundleRef> handle_;
};

} // namespace dmitigr::mac::cf

#endif  // DMITIGR_MAC_CF_HPP
