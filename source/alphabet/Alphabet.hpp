//
// Created by stefan on 7/10/24.
//

#pragma once

#include "Exceptions.hpp"
#include <string>

namespace au {
class Alphabet {
public:
  Alphabet() = default;
  Alphabet(Alphabet const&) = default;
  Alphabet(Alphabet&&) noexcept = default;
  explicit Alphabet(std::string&& symbols);
  Alphabet(char begin, char end) noexcept(false);

  [[nodiscard]] auto extend(std::string_view newSymbols) const -> Alphabet;
  [[nodiscard]] auto join(Alphabet const& other) const -> Alphabet;
  [[nodiscard]] inline auto const& symbols() const { return _symbols; }
  [[nodiscard]] inline auto contains(char chr) const { return _symbols.find(chr) != std::string::npos; }

private:
  std::string _symbols {};
};

namespace alphabet {
Alphabet const englishLowercaseLetters {'a', 'z'};
Alphabet const englishUppercaseLetters {'A', 'Z'};
Alphabet const englishLetters = englishLowercaseLetters.join(englishUppercaseLetters);
Alphabet const digits {'0', '9'};
Alphabet const alphanumeric = englishLetters.join(digits);
} // namespace alphabet
} // namespace au