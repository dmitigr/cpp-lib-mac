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

#include "../../base/assert.hpp"
#include "../cf.hpp"

#define ASSERT DMITIGR_ASSERT

int main()
{
  using std::cout;
  using std::cerr;
  using std::endl;
  try {
    namespace mac = dmitigr::mac;

    {
      const auto str = mac::cf::string::create_no_copy("");
      const auto stdstr = mac::cf::string::to_string(str);
      ASSERT(stdstr == "");
    }

    {
      const auto str = mac::cf::string::create_no_copy("Dima");
      const auto stdstr = to_string(str); // ADL test
      ASSERT(stdstr == "Dima");
    }
  } catch (const std::exception& e) {
    cerr << e.what() << endl;
    return 1;
  } catch (...) {
    cerr << "unknown error" << endl;
    return 2;
  }
}