#include <string>
#include <algorithm>
#include <iostream>
#include <random>

#include "matrix.h"
#include "distance.h"

extern "C" {
    #include "Tree.h"
    #include "TreeWS.h"
    #include "add.h"
}

Tree* growThreeLeavesTree(char* leaf1, char* leaf2, char* leaf3);

TreeWithScore* getBestChildBME(TreeWithScore* treeWS, char* newLeafName, const Matrix& matrix);

TreeWithScore* treeGrowBME(const Matrix& matrix, bool randLeaves);

TreeWithScore* oneTreeGrowBME(const Matrix& matrix, bool randLeaves);

TreeWithScore** multipleTreeGrowBME(const Matrix& matrix, bool randLeaves, unsigned int treeNum);