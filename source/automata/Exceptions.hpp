//
// Created by stefan on 8/8/24.
//

#pragma once

#include "utils/Exceptions.hpp"

namespace au::exceptions {

class UnownedAcceptingStateException : public Exception {
public:
  explicit UnownedAcceptingStateException() : Exception {"Attemmpting to mark unowned state as accepting"} {}
};

} // namespace au::exceptions