#include <gtest/gtest.h>
#include <algorithm>
#include <iterator>
#include "lib/bst_tree.h"

TEST(BSTree, Insert) {
  BSTree<int> tree;
  tree.insert(5);
  tree.insert(3);
  tree.insert(7);
  ASSERT_EQ(tree.size(), 3);
  ASSERT_TRUE(tree.contains(5));
  ASSERT_TRUE(tree.contains(3));
  ASSERT_TRUE(tree.contains(7));
}

TEST(BSTree, Merge) {
  BSTree<int> tree1;
  BSTree<int> tree2;
  tree1.insert(5);
  tree1.insert(3);
  tree1.insert(7);
  tree1.insert(4);
  tree2.insert(1);
  tree2.insert(2);
  tree2.insert(4);
  tree1.merge(tree2);
  ASSERT_EQ(tree1.size(), 6);
  std::vector<int> res{tree1.begin(), tree1.end()};
  std::vector<int> correct_ans{1, 2, 3, 4, 5, 7};
  ASSERT_EQ(res, correct_ans);
}


TEST(BSTree, InitializerList) {
  BSTree<int> tree = {5, 3, 7, 4};
  ASSERT_EQ(tree.size(), 4);
  std::vector<int> res{tree.begin(), tree.end()};
  std::vector<int> correct_ans{3, 4, 5, 7};
  ASSERT_EQ(res, correct_ans);
}

TEST(BSTree, Find) {
  BSTree<int> tree = {1, 2, 3};
  ASSERT_TRUE(std::find(tree.begin(), tree.end(), 4) == tree.end());
}

TEST(BSTree, STDtransform) {
  BSTree<int> tree = {2, 1, 3};
  std::vector<int> res;
  std::transform(tree.begin(), tree.end(), std::back_inserter(res), [](int x) { return x * x; });
  std::vector<int> correct_ans{1, 4, 9};
  ASSERT_EQ(res, correct_ans);
}