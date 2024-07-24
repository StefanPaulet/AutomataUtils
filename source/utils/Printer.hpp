//
// Created by stefan on 7/12/24.
//

#pragma once

#include <format>
#include <ostream>
#include <queue>
#include <unordered_map>

namespace au {
template <typename> struct TreeNodePrinter {
  template <typename N> auto operator()(std::ostream& out, [[maybe_unused]] N const*) const { out << "<Unknown Node>"; }
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

template <typename N> struct DotNodePrinter {
  auto label([[maybe_unused]] N const*) const { return "Unknown node"; }
  auto colour([[maybe_unused]] N const*) const { return "Unknown colour"; }
};

template <typename N> struct DotEdgePrinter {
  auto operator()([[maybe_unused]] N const*, [[maybe_unused]] N const*) const -> std::string { return ""; }
};

template <typename N> class DotNodeVisitor {
public:
  auto id(N const* n) {
    if (auto const it = _ids.find(n); it != _ids.end()) {
      return it->second;
    }
    return _ids.emplace(n, _idInc++).first->second;
  }

  auto visited(unsigned id) {
    if (id >= _visited.size()) {
      _visited.resize(id + 1, false);
    }
    if (_visited[id]) {
      return true;
    }

    _visited[id] = true;
    return false;
  }

private:
  std::unordered_map<N const*, unsigned int> _ids {};
  std::vector<bool> _visited {};
  unsigned _idInc = 0;
};

template <typename> struct GraphNodeChildren {
  template <typename N> auto operator()([[maybe_unused]] N const*) const { return std::array<N const*, 0u> {}; }
};

class DotGraphPrinter {
public:
  DotGraphPrinter() = default;
  DotGraphPrinter(bool oriented, std::string&& name) : _oriented {oriented}, _name {std::move(name)} {}

  template <typename N> auto dump(std::ostream& out, N const* node) const {
    out << std::format("{} {}", (_oriented ? "digraph" : "graph"), _name) << " {\n";
    std::queue<N const*> queue;
    GraphNodeChildren<N> const childrenOf;
    DotNodePrinter<N> np;
    DotNodeVisitor<N> nv;
    DotEdgePrinter<N> ep;

    queue.emplace(node);

    while (!queue.empty()) {
      auto current = queue.front();
      queue.pop();
      auto currentId = nv.id(current);

      if (nv.visited(currentId)) {
        continue;
      }
      out << std::string(indents, ' ')
          << std::format(R"(n{} [label="{}", color="{}"];)", currentId, np.label(current), np.colour(current)) << '\n';
      for (auto const& child : childrenOf(current)) {
        auto edgeLabel = ep(current, child);
        out << std::string(indents, ' ')
            << std::format(R"(n{} -{} n{}{})", currentId, _oriented ? ">" : "-", nv.id(child),
                           !edgeLabel.empty() ? std::format(R"( [label="{}"])", edgeLabel) : "")
            << ";\n";
        queue.push(child);
      }
    }

    out << "}\n";
  }

private:
  bool _oriented {false};
  std::string _name {"mygraph"};
  unsigned indents = 2;
};
} // namespace au