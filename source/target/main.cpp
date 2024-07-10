//
// Created by stefan on 7/9/24.
//
#include "regex/Regex.hpp"
#include <fstream>
#include <iostream>

using namespace au;

int main() {
  Regex r {"a|(bc+)*|((aab)*cd+)+b*|(a(b(c)*)*)+def"};
  std::cout << r.string() << '\n';
  auto tree = r.parse();
  DotGraphPrinter dgp;
  std::ofstream outfile {"out.out"};
  dgp.dump(outfile, tree.root().get());
  return 0;
}