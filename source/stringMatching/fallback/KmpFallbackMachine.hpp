//
// Created by stefan on 8/29/24.
//

#pragma once

#include "FallbackMachine.hpp"

namespace au::sm {

class KmpFallbackMachine : public FallbackMachine<KmpFallbackMachine> {
private:
  using Base = FallbackMachine<KmpFallbackMachine>;

public:
  using Base::Base;
  using Base::get;

  auto __build(std::string_view string) {
    _failure.push_back(0);
    auto len = 0u;
    for (auto idx = 1; idx < string.size(); ++idx) {
      while (len > 0 && string[idx] != string[len]) {
        len = _failure[len - 1];
      }
      if (string[idx] == string[len]) {
        ++len;
        _failure.push_back(len);
      } else {
        _failure.push_back(len);
      }
    }
  }
};

} // namespace au::sm