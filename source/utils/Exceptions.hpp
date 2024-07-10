//
// Created by stefan on 7/18/24.
//

#pragma once

#include <exception>
#include <string>

namespace au::exceptions {

class Exception : public std::exception {
public:
  explicit Exception(std::string&& message) : _exceptionMessage {std::move(message)} {}

  [[nodiscard]] char const* what() const noexcept override { return _exceptionMessage.c_str(); }

protected:
  std::string _exceptionMessage;
};
} // namespace au::exceptions
