//
// Created by stefan on 8/6/24.
//

#pragma once

namespace au {

struct SetHasher {

  template <typename R> auto operator()(R&& set) const noexcept -> std::size_t {
    auto stateHash = std::hash<std::remove_cvref_t<decltype(*set.begin())>> {};
    std::size_t hash = set.size();
    for (auto const* value : set) {
      hash ^= stateHash(value);
    }
    return hash;
  }
};
} // namespace au
