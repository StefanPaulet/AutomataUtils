//
// Created by stefan on 7/12/24.
//

#pragma once

#include "RegexTreePrinter.hpp"
#include "alphabet/Alphabet.hpp"
#include "operator/Operator.hpp"
#include <cstring>
#include <format>
#include <iostream>
#include <memory>

namespace au {

struct RegexSyntaxTreeNode {
  RegexSyntaxTreeNode() = default;
  RegexSyntaxTreeNode(RegexSyntaxTreeNode const& other) = delete;
  RegexSyntaxTreeNode(RegexSyntaxTreeNode&&) = default;
  explicit RegexSyntaxTreeNode(char character) : _character {character} {}
  RegexSyntaxTreeNode(char character, std::unique_ptr<RegexSyntaxTreeNode>&& left,
                      std::unique_ptr<RegexSyntaxTreeNode>&& right) :
      _character {character},
      _pLeft {std::move(left)}, _pRight {std::move(right)} {}

  std::unique_ptr<RegexSyntaxTreeNode> _pLeft {nullptr};
  std::unique_ptr<RegexSyntaxTreeNode> _pRight {nullptr};
  char _character {};

  [[nodiscard]] constexpr auto isLeaf() const -> bool { return _pLeft == nullptr; }
};

inline auto operator==(RegexSyntaxTreeNode const& lhs, RegexSyntaxTreeNode const& rhs) noexcept {
  if (&lhs == &rhs) {
    return true;
  }

  auto compare = [](std::unique_ptr<RegexSyntaxTreeNode> const& lhs, std::unique_ptr<RegexSyntaxTreeNode> const& rhs) {
    if (lhs != nullptr) {
      if (rhs != nullptr) {
        return (*lhs == *rhs);
      }
      return false;
    }
    return (rhs == nullptr);
  };
  auto result =
      (lhs._character == rhs._character) && compare(lhs._pLeft, rhs._pLeft) && compare(lhs._pRight, rhs._pRight);

  return result;
}

class RegexSyntaxTree {
public:
  using Node = std::unique_ptr<RegexSyntaxTreeNode>;

  RegexSyntaxTree() = default;
  RegexSyntaxTree(RegexSyntaxTree const&) = delete;
  RegexSyntaxTree(RegexSyntaxTree&&) noexcept = default;
  explicit RegexSyntaxTree(char character) : _pRoot {std::make_unique<RegexSyntaxTreeNode>(character)} {}
  explicit RegexSyntaxTree(std::unique_ptr<RegexSyntaxTreeNode>&& root) : _pRoot {std::move(root)} {}

  [[nodiscard]] auto root() -> Node& { return _pRoot; }
  [[nodiscard]] auto root() const -> Node const& { return _pRoot; }

private:
  Node _pRoot {std::make_unique<RegexSyntaxTreeNode>()};
};

inline auto operator==(RegexSyntaxTree const& lhs, RegexSyntaxTree const& rhs) {
  if (&lhs == &rhs) {
    return true;
  }

  return *lhs.root() == *rhs.root();
}


template <> struct TreeNodePrinter<RegexSyntaxTreeNode> {
  auto operator()(std::basic_ostream<char>& out, RegexSyntaxTreeNode const* node) const {
    if (!node->isLeaf()) {
      out << "Operator " << node->_character;
    } else {
      out << node->_character;
    }
  }
};

template <> struct TreeNodeChildren<RegexSyntaxTreeNode> {
  auto operator()(RegexSyntaxTreeNode const* node) const -> std::vector<RegexSyntaxTreeNode const*> {
    if (node->isLeaf()) {
      return {};
    }
    if (node->_pRight == nullptr) {
      return {node->_pLeft.get()};
    }

    return {node->_pLeft.get(), node->_pRight.get()};
  }
};

template <> struct DotNodePrinter<RegexSyntaxTreeNode> {
  auto id(RegexSyntaxTreeNode const* n) {
    if (auto const it = ids.find(n); it != ids.end()) {
      return it->second;
    }
    return ids.emplace(n, idInc++).first->second;
  }

  auto label(RegexSyntaxTreeNode const* n) const { return std::string(1, n->_character); }

  auto borderColour(RegexSyntaxTreeNode const* n) const {
    if (!n->isLeaf()) {
      return "blue";
    }
    return "black";
  }

  std::unordered_map<RegexSyntaxTreeNode const*, unsigned> ids;
  unsigned idInc = 0;
};

} // namespace au