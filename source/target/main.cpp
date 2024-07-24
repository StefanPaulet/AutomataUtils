//
// Created by stefan on 7/9/24.
//
#include <automata/nfa/NFA.hpp>
#include <fstream>

using namespace au;

int main() {

  auto r = Regex {"(a|b)*abb"};
  auto nfa = NfaAutomata {r};
  DotGraphPrinter dg {true, "nfa"};
  std::ofstream out {"out.out"};
  dg.dump(out, nfa.start().get());
  return 0;
}
