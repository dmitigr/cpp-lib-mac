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
#include <optional>
#include <stdexcept>
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
    : native_{rhs.native_}
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
using Dictionary = Handle<CFDictionaryRef>;
using String = Handle<CFStringRef>;
using Url = Handle<CFURLRef>;

// -----------------------------------------------------------------------------
// String
// -----------------------------------------------------------------------------

inline namespace string {

inline String create_no_copy(const char* const str,
  const CFStringEncoding encoding = kCFStringEncodingUTF8)
{
  return String{CFStringCreateWithCStringNoCopy(kCFAllocatorDefault, str,
    encoding, kCFAllocatorNull)};
}

inline std::string to_string(const String& str,
  const CFStringEncoding result_encoding = kCFStringEncodingUTF8)
{
  {
    const char* const c_str = CFStringGetCStringPtr(str.native(),
      result_encoding);
    if (c_str)
      return c_str;
  }

  const auto utf16_chars_count = CFStringGetLength(str.native());
  const auto bytes_count = CFStringGetMaximumSizeForEncoding(utf16_chars_count,
    result_encoding);
  if (bytes_count == kCFNotFound)
    throw std::overflow_error{"cannot convert CFString to std::string"};

  std::string result(bytes_count, 0);
  if (!CFStringGetCString(str.native(), result.data(), result.size() + 1,
      result_encoding))
    throw std::runtime_error{"cannot convert CFString to std::string"};

  const auto e = find_if_not(result.crbegin(), result.crend(),
    [](const auto ch){return !ch;}).base();
  result.resize(e - result.cbegin());
  return result;
}

} // inline namespace string

// -----------------------------------------------------------------------------
// Bundle
// -----------------------------------------------------------------------------

inline namespace bundle {

inline Bundle create(const Url& url)
{
  return Bundle{CFBundleCreate(kCFAllocatorDefault, url.native())};
}

inline Bundle create(const std::filesystem::path& path)
{
  const auto path_hdl = string::create_no_copy(path.c_str());
  const Url url{CFURLCreateWithFileSystemPath(kCFAllocatorDefault,
    path_hdl.native(), kCFURLPOSIXPathStyle, is_directory(path))};
  return bundle::create(url);
}

inline void* function_pointer_for_name(const Bundle& bundle,
  const char* const name)
{
  return CFBundleGetFunctionPointerForName(bundle.native(),
    string::create_no_copy(name).native());
}

} // inline namespace bundle

// -----------------------------------------------------------------------------
// Dictionary
// -----------------------------------------------------------------------------

inline namespace dictionary {

inline Dictionary create(const void** keys, const void** values,
  const CFIndex size,
  const CFDictionaryKeyCallBacks* const key_callbacks,
  const CFDictionaryValueCallBacks* const value_callbacks)
{
  return Dictionary{CFDictionaryCreate(kCFAllocatorDefault, keys, values, size,
    key_callbacks, value_callbacks)};
}

inline std::optional<const void*> value(const Dictionary& dictionary,
  const void* const key)
{
  const void* result{};
  if (CFDictionaryGetValueIfPresent(dictionary.native(), key, &result))
    return result;
  else
    return std::nullopt;
}

} // inline namespace dictionary

} // namespace dmitigr::mac::cf

#endif  // DMITIGR_MAC_CF_HPP
