#ifndef UTILS_STRINGVIEW_HPP
#define UTILS_STRINGVIEW_HPP

#include <absl/memory/memory.h>
#include <absl/strings/charconv.h>
#include <absl/strings/string_view.h>
#include <absl/types/optional.h>
#include <charconv>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <type_traits>

std::unique_ptr<char[]> to_c_array(const absl::string_view &view) {
  auto array = absl::make_unique<char[]>(view.size() + 1);
  memcpy(array.get(), view.begin(), view.size());
  array[view.size()] = '\0';
  return array;
}

namespace _impl {
template<typename T, typename Enable = void>
struct from_stringview {};

template<typename T>
struct from_stringview<T, typename std::enable_if<std::is_floating_point<T>::value>::type> {
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
struct from_stringview<T, typename std::enable_if<std::is_integral<T>::value && std::is_unsigned<T>::value>::type> {
  absl::optional<T> operator()(const absl::string_view &view) {
    if(view.front() == '-') {
      return absl::nullopt;
    }
    auto c_str = to_c_array(view);
    auto val   = strtoull(c_str.get(), nullptr, 10);
    if(errno || val > std::numeric_limits<T>::max()) {
      errno = 0;
      return absl::nullopt;
    }
    return val;
  }
};
template<typename T>
struct from_stringview<T, typename std::enable_if<std::is_integral<T>::value && !std::is_unsigned<T>::value>::type> {
  absl::optional<T> operator()(const absl::string_view &view) {
    auto c_str = to_c_array(view);
    auto val   = strtoll(c_str.get(), nullptr, 10);
    if(errno || val > std::numeric_limits<T>::max() || val < std::numeric_limits<T>::min()) {
      errno = 0;
      return absl::nullopt;
    }
    return val;
  }
};
}    // namespace _impl
template<typename T>
absl::optional<T> from_stringview(const absl::string_view &view) {
  return _impl::from_stringview<T>()(view);
}

#endif
