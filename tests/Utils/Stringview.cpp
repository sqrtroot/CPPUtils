#include <Utils/Stringview.hpp>
#include <absl/numeric/int128.h>
#include <absl/strings/string_view.h>
#include <catch2/catch.hpp>
#include <cxxabi.h>
#include <iostream>
#include <numeric>
#include <sstream>

TEST_CASE("Converting stringview to oldschool char array", "[Stringview]") {
  absl::string_view x     = "Hello world";
  auto              array = to_c_array(x);
  SECTION("String sizes should be equal") {
    REQUIRE(strlen(array.get()) == x.size());
  }
  SECTION("Last elemenent should be null terminator") {
    REQUIRE(array[x.size()] == '\0');
  }
  SECTION("All characters should be the same") {
    for(size_t i = 0; i < x.size(); i++) {
      REQUIRE(x[i] == array[i]);
    }
  }
}

template<typename T>
std::string generate_string(T number) {
  std::ostringstream outStream;
  if(std::is_same<char, T>::value || std::is_same<unsigned char, T>::value) {
    outStream << (int) number;
  } else {
    outStream << number;
  }
  return outStream.str();
}

std::string demangle(const char *name) {
  int status = -4;    // some arbitrary value to eliminate the compiler warning

  // enable c++11 by passing the flag -std=c++11 to g++
  std::unique_ptr<char, void (*)(void *)> res{
    abi::__cxa_demangle(name, NULL, NULL, &status), std::free};

  return (status == 0) ? res.get() : name;
}

using Stringview_TestTypes =
  std::tuple<int, unsigned int, char, unsigned char, float, double, long, unsigned long, long long, unsigned long long>;
TEMPLATE_LIST_TEST_CASE("Converting stringviews to numbers", "[Stringview]", Stringview_TestTypes) {
  constexpr auto min     = std::numeric_limits<TestType>::min();
  constexpr auto max     = std::numeric_limits<TestType>::max();
  constexpr auto epsilon = std::numeric_limits<TestType>::epsilon();
  DYNAMIC_SECTION("Minimum " << demangle(typeid(TestType).name()) << " value " << min
                             << " should be decoded") {
    const auto value = from_stringview<TestType>(generate_string(min));
    REQUIRE(value.has_value());
    if(std::is_integral<TestType>::value) {
      REQUIRE(*value == min);
    } else {
      REQUIRE(*value == Approx(min));
    }
  }
  DYNAMIC_SECTION("Maximum " << demangle(typeid(TestType).name()) << " value " << max
                             << " should be decoded") {
    const auto value = from_stringview<TestType>(generate_string(max));
    REQUIRE(value.has_value());
    if(std::is_integral<TestType>::value) {
      REQUIRE(*value == max);
    } else {
      REQUIRE(*value == Approx(max));
    }
  }

  if(!std::is_floating_point<TestType>::value) {
    DYNAMIC_SECTION("Over/under-flown " << demangle(typeid(TestType).name()) << " should not decode") {
      const auto min_error =
        from_stringview<TestType>(generate_string(absl::int128(min) - 1));
      REQUIRE(!min_error.has_value());
      const auto max_error =
        from_stringview<TestType>(generate_string(absl::int128(max) + 1));
      REQUIRE(!max_error.has_value());
    }
  }

  DYNAMIC_SECTION(demangle(typeid(TestType).name())
                  << "s in range should be decoded") {
    const auto testNumber = GENERATE(take(25, random(min, max)));
    const auto value      = from_stringview<TestType>(generate_string(testNumber));
    DYNAMIC_SECTION(testNumber) {
      REQUIRE(value.has_value());
      if(std::is_integral<TestType>::value) {
        REQUIRE(*value == testNumber);
      } else {
        REQUIRE(*value == Approx(testNumber));
      }
    }
  }
}