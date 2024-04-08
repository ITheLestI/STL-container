#include "lib/bst_tree.h"
#include "lib/traversers.h"

int main() {
  BSTree<int> tree1, tree2;
  tree1.insert(5);
  tree1.insert(3);
  tree1.insert(7);
  tree1.insert(4);
  tree2.insert(1);
  tree2.insert(2);
  tree2.insert(4);
  tree1.merge(tree2);
  for (int i : tree1) {
    std::cout << i << " ";
  }
  std::cout << std::endl;
  return 0;
}
