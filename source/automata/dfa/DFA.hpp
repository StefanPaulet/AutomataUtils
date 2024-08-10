//
// Created by stefan on 8/3/24.
//

#pragma once

#include "automata/Automata.hpp"
#include "automata/nfa/NFA.hpp"
#include "automata/state/DfaState.hpp"
#include "utils/Hasher.hpp"
#include <ranges>
#include <set>
#include <unordered_set>

namespace au {

class RegexToDfaParser;
class DfaMinimizer;

class DfaAutomata : public Automata<DfaAutomata, DfaState> {
private:
  friend class RegexToDfaParser;

public:
  using Automata::Automata;
  using RegexParser = RegexToDfaParser;

  auto __simulate(std::string_view const str) const -> std::tuple<bool, DfaState const*> {
    auto* currentState = start();
    for (auto chr : str) {
      currentState = currentState->next(chr);
    }
    if (isAccepting(currentState)) {
      return {true, currentState};
    }
    return {false, nullptr};
  }

  template <typename M = DfaMinimizer> [[nodiscard]] auto minimize() const -> DfaAutomata {
    return M {}.minimize(*this);
  }
};

class RegexToDfaParser {
private:
  struct RegexTreeDfaParsingEnhancer {
    auto operator()(RegexSyntaxTree&& initialTree) const -> RegexSyntaxTree {
      constexpr auto endMarker = '#';
      auto endMarkerNode = std::make_unique<RegexSyntaxTreeNode>(endMarker);
      auto newRoot = std::make_unique<RegexSyntaxTreeNode>(operators::catOp.op, std::move(initialTree.root()),
                                                           std::move(endMarkerNode));
      return RegexSyntaxTree {std::move(newRoot)};
    }
  };
  using Node = RegexSyntaxTreeNode const*;

  template <typename Target> using NodeMap = std::unordered_map<Node, Target>;

  using NodePosMap = NodeMap<std::unordered_set<Node>>;
  using NodeNullableMap = NodeMap<bool>;

  struct PosContainer {
    NodeNullableMap null {};
    NodePosMap first {};
    NodePosMap last {};
    NodePosMap follow {};
  };

  auto character(PosContainer& pos, Node node) const {
    pos.null.emplace(node, false);
    pos.first[node].emplace(node);
    pos.last[node].emplace(node);
  }

  auto composition(PosContainer& pos, Node node, Node lhs, Node rhs) const {
    pos.null.emplace(node, pos.null.at(lhs) && pos.null.at(rhs));

    auto&& [fpNode, _1] = pos.first.emplace(node, pos.first.at(lhs));
    auto const& rhsFp = pos.first.at(rhs);
    if (pos.null.at(lhs)) {
      fpNode->second.insert(rhsFp.begin(), rhsFp.end());
    }

    auto&& [lpNode, _2] = pos.last.emplace(node, pos.last.at(rhs));
    if (pos.null.at(rhs)) {
      auto const& lhsLp = pos.last.at(lhs);
      lpNode->second.insert(lhsLp.begin(), lhsLp.end());
    }

    for (auto const* lhsLast : pos.last.at(lhs)) {
      pos.follow[lhsLast].insert(rhsFp.begin(), rhsFp.end());
    }
  }

  auto disjunction(PosContainer& pos, Node node, Node lhs, Node rhs) const {
    pos.null.emplace(node, pos.null.at(lhs) || pos.null.at(rhs));

    auto const& rhsFp = pos.first.at(rhs);
    pos.first.emplace(node, pos.first.at(lhs)).first->second.insert(rhsFp.begin(), rhsFp.end());

    auto const& lhsLp = pos.last.at(lhs);
    pos.last.emplace(node, pos.last.at(rhs)).first->second.insert(lhsLp.begin(), lhsLp.end());
  }

  auto kleeneClosure(PosContainer& pos, Node node, Node child) const {
    pos.null.emplace(node, true);
    pos.first.emplace(node, pos.first.at(child));
    pos.last.emplace(node, pos.last.at(child));

    auto const& childFp = pos.first.at(child);
    for (auto const* childLp : pos.last.at(child)) {
      pos.follow[childLp].insert(childFp.begin(), childFp.end());
    }
  }

  auto fillPos(PosContainer& pos, Regex const& regex) const -> RegexSyntaxTree::Node {
    std::stack<RegexSyntaxTreeNode const*> nodeStack {};
    auto getNextNode = [&nodeStack]() -> RegexSyntaxTreeNode const* {
      auto const* next = nodeStack.top();
      nodeStack.pop();
      return next;
    };

    auto operatorSwitch = [this, &getNextNode, &pos](Node node, Operator const& op) {
      using namespace operators;
      if (op == starOp) {
        kleeneClosure(pos, node, getNextNode());
        return;
      }
      auto rhs = getNextNode();
      auto lhs = getNextNode();
      if (op == catOp) {
        composition(pos, node, lhs, rhs);
        return;
      }
      if (op == orOp) {
        disjunction(pos, node, lhs, rhs);
        return;
      }

      assert(false && "Unknown operator found");
    };

    using enum RegexSyntaxTreeTraversal::Order;
    auto tree = RegexTreeDfaParsingEnhancer {}(regex.parse());
    for (auto const* node : RegexSyntaxTreeTraversal {POSTORDER, tree.root().get()}) {
      if (auto [isOp, op] = regex.operators().isOperator(node->_character); isOp) {
        operatorSwitch(node, op);
      } else {
        character(pos, node);
      }
      nodeStack.push(node);
    }

    return std::move(tree.root());
  }

public:
  auto parse(Regex const& regex) const {
    PosContainer pos {};
    auto root = fillPos(pos, regex);
    auto const* endMarkerNode = root->_pRight.get();

    DfaAutomata dfa {};
    auto const& start = pos.first.at(root.get());
    std::unordered_map<std::unordered_set<Node>, DfaState*, SetHasher> stateMap {};
    std::queue<std::unordered_set<Node>> groupQueue {};

    stateMap.emplace(start, dfa._start);
    groupQueue.push(start);

    while (!groupQueue.empty()) {
      auto currState = groupQueue.front();
      groupQueue.pop();

      std::unordered_map<char, std::unordered_set<Node>> nextStatesSet {};
      for (auto const* node : currState) {
        if (node != endMarkerNode) {
          auto nextState = pos.follow.at(node);
          nextStatesSet[node->_character].insert(nextState.begin(), nextState.end());
        }
      }

      for (auto const& [sym, group] : nextStatesSet) {
        if (!stateMap.contains(group)) {
          stateMap.emplace(group, dfa.allocate());
          groupQueue.push(group);
        }
        auto newState = stateMap.at(group);
        stateMap.at(currState)->addTransition(sym, newState);
        if (group.contains(endMarkerNode)) {
          dfa.markAccepting(newState);
        }
      }
    }

    return dfa;
  }
};

class NfaToDfaParser {
private:
  using StateSet = std::unordered_set<NfaState const*>;
  using StateQueue = std::queue<StateSet>;

public:
  auto parse(NfaAutomata const& source) -> DfaAutomata {
    DfaAutomata dfa {};

    auto startState = source.closure(source.start());

    StateQueue stateQueue;
    stateQueue.push(startState);

    std::unordered_map<StateSet, DfaState*, SetHasher> visitedStates;
    visitedStates.emplace(startState, dfa.start());

    std::vector<DfaState*> endStates;

    while (!stateQueue.empty()) {
      auto currentState = stateQueue.front();
      stateQueue.pop();

      std::set<char> nextSyms {};
      for (auto const& curr : currentState) {
        for (auto const& sym : std::views::keys(curr->transitions())) {
          if (sym.has_value()) {
            nextSyms.emplace(sym.value());
          }
        }
      }

      for (auto const& nextSym : nextSyms) {
        auto nextState = source.closure(source.next(currentState, nextSym));
        auto nextStIt = visitedStates.find(nextState);
        if (nextStIt == visitedStates.end()) {
          nextStIt = visitedStates.emplace(nextState, dfa.allocate()).first;
          stateQueue.push(nextState);
          if (std::get<0>(source.isAccepting(nextState))) {
            endStates.push_back(nextStIt->second);
          }
        }
        visitedStates.at(currentState)->addTransition(nextSym, nextStIt->second);
      }
    }

    for (auto const* state : endStates) {
      dfa.markAccepting(state);
    }
    return dfa;
  }
};

class DfaMinimizer {
private:
  using StateSet = std::vector<DfaState const*>;
  using Partition = std::vector<StateSet>;
  using StateQueue = std::queue<StateSet>;

  template <typename P> auto findGroup(P&& partition, DfaState const* state) const {
    for (auto& set : std::forward<P>(partition)) {
      if (std::ranges::find(set, state) != set.end()) {
        return set;
      }
    }
    assert(false && "State not part of any set found");
  }

  auto mergeParitions(Partition const& first, Partition const& second) const -> Partition {
    Partition result;
    for (auto const& fs : first) {
      for (auto const& ss : second) {
        auto intersection = StateSet {};
        for (auto const* el : fs) {
          if (std::ranges::find(ss, el) != ss.end()) {
            intersection.push_back(el);
          }
        }
        result.push_back(intersection);
      }
    }

    return result;
  }

  auto initialPartition(DfaAutomata const& automata) const -> Partition {
    Partition result {2};

    for (auto const* state : automata.states()) {
      if (automata.isAccepting(state)) {
        result[1].push_back(state);
      } else {
        result[0].push_back(state);
      }
    }

    return result;
  }

  auto makePartition(Partition const& currentPartition, StateSet const& set) const {
    std::unordered_set<char> symSet;
    Partition newSets {};
    newSets.emplace_back();

    for (auto const* state : set) {
      newSets.back().push_back(state);
      for (auto const sym : std::views::keys(state->transitions())) {
        symSet.emplace(sym);
      }
    }

    for (auto const sym : symSet) {
      std::unordered_map<StateSet, StateSet, SetHasher> transitionMap {};
      for (auto const* state : set) {
        auto nextState = state->next(sym);
        StateSet targetSet;
        if (nextState != DfaState::deadState()) {
          targetSet = findGroup(currentPartition, nextState);
        } else {
          continue;
        }
        transitionMap[targetSet].push_back(state);
      }
      if (transitionMap.size() == 1) {
        continue;
      }

      Partition newPartition;
      for (auto const& [_, newSet] : transitionMap) {
        newPartition.emplace_back();
        for (auto const* el : newSet) {
          newPartition.back().push_back(el);
        }
      }

      newSets = mergeParitions(newSets, newPartition);
    }

    return newSets;
  }

public:
  auto minimize(DfaAutomata const& dfa) const -> DfaAutomata {
    auto part = initialPartition(dfa);

    for (auto modified = true; modified;) {
      modified = false;
      for (auto oldPart = part; auto const& group : oldPart) {
        if (auto newGroups = makePartition(oldPart, group); newGroups.size() > 1) {
          std::erase(part, group);
          for (auto const& newGroup : newGroups) {
            part.emplace_back(newGroup);
          }
          modified = true;
        }
      }
    }

    DfaAutomata result {};

    std::unordered_map<StateSet, DfaState*, SetHasher> representatives {};

    auto startGroup = findGroup(part, dfa.start());
    std::unordered_set<StateSet, SetHasher> accGroups {};
    for (auto const* acc : dfa.accepting()) {
      accGroups.emplace(findGroup(part, acc));
    }

    for (auto const& group : part) {
      if (group == startGroup) {
        representatives.emplace(group, result.start());
        continue;
      }
      if (accGroups.contains(group)) {
        representatives.emplace(group, result.createAccepting());
        continue;
      }
      representatives.emplace(group, result.allocate());
    }

    for (auto const& group : part) {
      for (auto const& [sym, link] : group.front()->transitions()) {
        auto targetGroup = findGroup(part, link);
        representatives.at(group)->addTransition(sym, representatives.at(targetGroup));
      }
    }

    return result;
  }
};

} // namespace au