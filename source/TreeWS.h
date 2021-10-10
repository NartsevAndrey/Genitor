#ifndef __TREE_WS__
#define __TREE_WS__
#include "Tree.h"
#include <stdlib.h>

typedef struct
{
    Tree* tree;
    long double score;
} TreeWithScore;

TreeWithScore* treeWithScoreCreate(Tree* tree, long double score);

void treeWithScoreDelete(TreeWithScore* treeWS);

int treeWithScoreCompare(const void* treeWS1, const void* treeWS2);

void treeWithScoreSort(TreeWithScore** treeWSArray, size_t size);
#endif
