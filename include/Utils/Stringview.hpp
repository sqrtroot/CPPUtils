#ifndef UTILS_STRINGVIEW_HPP
#define UTILS_STRINGVIEW_HPP

#include <absl/memory/memory.h>
#include <absl/strings/charconv.h>
#include <absl/strings/string_view.h>
#include <absl/types/optional.h>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <type_traits>

namespace FromStringviewImpl {
constexpr int CONVERSION_BASE = 10;

template<typename T, typename Enable = void>
struct FromStringview {};

template<typename T>
struct FromStringview<T, typename std::enable_if<std::is_floating_point<T>::value>::type> {
  absl::optional<T> operator()(const absl::string_view &view) {
    T    out;
    auto ec = absl::from_chars(view.begin(), view.end(), out);
    if(ec.ec != std::errc()) {
      return absl::nullopt;
    }
    return out;
  }
};
template<typename T>
struct FromStringview<T, typename std::enable_if<std::is_integral<T>::value && std::is_unsigned<T>::value>::type> {
  absl::optional<T> operator()(const absl::string_view &view) {
    if(view.front() == '-') {
      return absl::nullopt;
    }
    std::string str(view);
    auto val   = strtoull(str.c_str(), /*__endptr=*/nullptr, CONVERSION_BASE);
    if(errno || val > std::numeric_limits<T>::max()) {
      errno = 0;
      return absl::nullopt;
    }
    return val;
  }
};
template<typename T>
struct FromStringview<T, typename std::enable_if<std::is_integral<T>::value && !std::is_unsigned<T>::value>::type> {
  absl::optional<T> operator()(const absl::string_view &view) {
    std::string str(view);
    auto val   = strtoll(str.c_str(), /*__endptr=*/nullptr, CONVERSION_BASE);
    if(errno || val > std::numeric_limits<T>::max() || val < std::numeric_limits<T>::min()) {
      errno = 0;
      return absl::nullopt;
    }
    return val;
  }
};
}    // namespace FromStringviewImpl
template<typename T>
absl::optional<T> from_stringview(const absl::string_view &view) {
  return FromStringviewImpl::FromStringview<T>()(view);
}

#endif
