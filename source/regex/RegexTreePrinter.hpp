//
// Created by stefan on 7/12/24.
//

#pragma once

#include <ostream>
#include <queue>

namespace au {
template <typename> struct TreeNodePrinter {
  template <typename N> auto operator()(std::ostream& out, [[maybe_unused]] N const*) const { out << "<Unknonw Node>"; }
};

template <typename> struct TreeNodeChildren {
  template <typename N> auto operator()([[maybe_unused]] N const*) const { return std::array<N const*, 0u> {}; }
};

struct TreeGraphPrinter {
  template <typename N> auto dump(std::ostream& out, N const* node) const { dump(out, node, ""); }

  template <typename N> auto dump(std::ostream& out, N const* node, std::string const& indent) const -> void {
    TreeNodeChildren<N> const nc;
    auto ind = [&indent, this](bool last) {
      auto li = indent;
      for (auto& c : li) {
        if (c == '`') {
          c = ' ';
        }
      }
      if (last) {
        return li + "`" + std::string(indents - 1, ' ');
      }

      return li + "|" + std::string(indents - 1, ' ');
    };

    auto dumpNode = [&out, &indent](N const* n) {
      TreeNodePrinter<N> const np;
      out << indent;
      np(out, n);
      out << "\n";
    };

    dumpNode(node);
    auto children = nc(node);
    auto it = children.begin();
    while (it != children.end()) {
      auto child = *it;
      ++it;
      auto last = (it == children.end());
      auto ni = ind(last);
      dump(out, child, ni);
    }
  }

  unsigned indents = 2;
};

template <typename> struct DotNodePrinter {
  template <typename N> auto operator()(std::ostream& out, [[maybe_unused]] N const*) const { out << "Unknown Node"; }
};

struct DotGraphPrinter {
  template <typename N> auto dump(std::ostream& out, N const* node) const {
    out << "graph regex_tree {\n";
    std::queue<N const*> queue;
    TreeNodeChildren<N> const nc;
    DotNodePrinter<N> np;
    queue.emplace(node);
    while (!queue.empty()) {
      auto n = queue.front();
      queue.pop();
      out << std::string(indents, ' ');
      out << "n" << np.id(n) << R"( [label=")" << np.label(n) << R"(", color=")" << np.borderColour(n) << R"("];)"
          << "\n";
      auto const ch = nc(n);
      if (!ch.empty()) {
        out << std::string(indents, ' ') << "n" << np.id(n) << " -- {";
        auto it = ch.begin();
        while (it != ch.end()) {
          queue.emplace(*it);
          out << "n" << np.id(*it);
          ++it;
          if (it != ch.end()) {
            out << ", ";
          }
        }
        out << "};\n";
      }
    }

    out << "}\n";
  }

  unsigned indents = 2;
};
} // namespace au