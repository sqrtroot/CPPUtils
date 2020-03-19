#ifndef UTILS_TUPLE_HPP
#define UTILS_TUPLE_HPP

#include <tuple>
namespace _impl {    // NOLINT
// this could be so much easier with even c++14 but yeah.
template<typename R, size_t S, size_t I>
struct ToPointerImpl {
  template<typename... T>
  constexpr std::array<R, S> operator()(const std::tuple<T...> &r, std::array<R, S> &&arr) const {
    return arr[S - I] = (R) &std::get<S - I>(r),
                   ToPointerImpl<R, S, I - 1>()(r, std::move(arr));
  }
};

template<typename R, size_t S>
struct ToPointerImpl<R, S, 0> {
  template<typename... T>
  constexpr std::array<R, S>
    operator()(const std::tuple<T...> & /*unused*/, std::array<R, S> &&arr) const {
    return std::move(arr);
  }
};
}    // namespace _impl

template<typename R, typename... T>
constexpr std::array<R, sizeof...(T)> to_pointers(const std::tuple<T...> &t) {
  static_assert(std::is_pointer<R>::value, "Type to convert to should be pointer");
  return _impl::ToPointerImpl<R, sizeof...(T), sizeof...(T)>()(
    t, std::array<R, sizeof...(T)>());
}

#endif
