//
// Created by stefan on 7/10/24.
//

#include "Alphabet.hpp"
#include <algorithm>
#include <numeric>

namespace au {

Alphabet::Alphabet(std::string&& symbols) : _symbols {std::move(symbols)} { std::ranges::sort(_symbols); }


Alphabet::Alphabet(char begin, char end) noexcept(false) {
  if (end - begin < 0) {
    throw exceptions::AlphabetRangeException {};
  }
  _symbols = std::string(end - begin + 1, begin);
  std::iota(_symbols.begin(), _symbols.end(), begin);
}

auto Alphabet::extend(std::string_view newSymbols) const -> Alphabet {
  Alphabet result {};
  auto lIt = _symbols.begin();
  auto rIt = newSymbols.begin();
  while (lIt != _symbols.end() && rIt != newSymbols.end()) {
    if (*lIt < *rIt) {
      result._symbols.push_back(*lIt);
      ++lIt;
    } else {
      result._symbols.push_back(*rIt);
      if (*lIt == *rIt) {
        ++lIt;
      }
      ++rIt;
    }
  }

  while (lIt != _symbols.end()) {
    result._symbols.push_back(*(lIt++));
  }

  while (rIt != newSymbols.end()) {
    result._symbols.push_back(*(rIt++));
  }

  return result;
}

auto Alphabet::join(au::Alphabet const& other) const -> Alphabet { return extend(other._symbols); }
} // namespace au