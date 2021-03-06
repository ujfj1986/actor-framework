// This file is part of CAF, the C++ Actor Framework. See the file LICENSE in
// the main distribution directory for license terms and copyright or visit
// https://github.com/actor-framework/actor-framework/blob/master/LICENSE.

#pragma once

#include "caf/message.hpp"
#include "caf/stream_sink.hpp"

#include "caf/detail/type_traits.hpp"

namespace caf {

namespace detail {

// -- invoke helper to support element-wise and batch-wise processing ----------

struct stream_sink_trait_invoke_one {
  template <class F, class State, class In>
  static void invoke(F& f, State& st, std::vector<In>& xs) {
    for (auto& x : xs)
      f(st, std::move(x));
  }
};

struct stream_sink_trait_invoke_all {
  template <class F, class State, class In>
  static void invoke(F& f, State& st, std::vector<In>& xs) {
    f(st, xs);
  }
};

} // namespace detail

// -- trait implementation -----------------------------------------------------

/// Base type for all sink traits.
template <class State, class In>
struct stream_sink_trait_base {
  /// Defines the state element for the function objects.
  using state = State;

  /// Defines the type of a single stream element.
  using input = In;

  /// Defines a pointer to a sink.
  using pointer = stream_sink_ptr<input>;
};

/// Defines required type aliases for stream sinks.
template <class Fun>
struct stream_sink_trait;

/// Specializes the trait for element-wise processing.
template <class State, class In>
struct stream_sink_trait<void(State&, In)> : stream_sink_trait_base<State, In> {
  /// Defines a helper for dispatching to the processing function object.
  using process = detail::stream_sink_trait_invoke_one;
};

/// Specializes the trait for batch-wise processing.
template <class State, class In>
struct stream_sink_trait<void(State&, std::vector<In>&)>
  : stream_sink_trait_base<State, In> {
  /// Defines a helper for dispatching to the processing function object.
  using process = detail::stream_sink_trait_invoke_all;
};

/// Specializes the trait for batch-wise processing with const references.
template <class State, class In>
struct stream_sink_trait<void(State&, const std::vector<In>&)>
  : stream_sink_trait_base<State, In> {
  /// Defines a helper for dispatching to the processing function object.
  using process = detail::stream_sink_trait_invoke_all;
};

// -- convenience alias --------------------------------------------------------

/// Derives a sink trait from the signatures of Fun and Fin.
template <class Fun>
using stream_sink_trait_t
  = stream_sink_trait<typename detail::get_callable_trait<Fun>::fun_sig>;

} // namespace caf
