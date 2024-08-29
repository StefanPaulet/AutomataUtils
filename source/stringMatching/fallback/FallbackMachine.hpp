//
// Created by stefan on 8/29/24.
//

#pragma once

#include <automata/state/DfaState.hpp>
#include <unordered_map>

namespace au::sm {
template <typename D> class FallbackMachine {
public:
  explicit FallbackMachine(std::string_view string) {
    _failure.reserve(string.size());
    static_cast<D*>(this)->__build(string);
  }

  auto get(unsigned int idx) const { return _failure[idx]; }

protected:
  std::vector<unsigned int> _failure {};
};
} // namespace au::sm