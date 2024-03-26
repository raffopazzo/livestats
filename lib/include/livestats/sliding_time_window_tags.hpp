#pragma once

#include <type_traits>

namespace livestats {

template <std::size_t Millis> using sliding_time_window_tag_t = std::integral_constant<std::size_t, Millis>;
template <std::size_t Millis> inline constexpr auto sliding_time_window_tag = sliding_time_window_tag_t<Millis>{};

} // namespace livestats

