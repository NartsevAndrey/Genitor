#include "GrowTreeBME.h"


Tree* growThreeLeavesTree(char* leaf1, char* leaf2, char* leaf3) {
    Tree*  result = treeCreate();
    result->nodes = (Node**)calloc(sizeof(Node*), 4);
    result->leaves = (Node**)calloc(sizeof(Node*), 3);
    Node* node = nodeCreate();
    result->nodes[0] = node;
    node->pos = 0;
    node = leafCreate(leaf1);
    result->nodes[1] = node;
    result->leaves[0] = node;
    node->pos = 1;
    nodeAddNeighbour(result->nodes[0], node, 0);
    nodeAddNeighbour(node, result->nodes[0], 0);
    node = leafCreate(leaf2);
    node->pos = 2;
    result->nodes[2] = node;
    result->leaves[1] = node;
    nodeAddNeighbour(result->nodes[0], node, 0);
    nodeAddNeighbour(node, result->nodes[0], 0);
    node = leafCreate(leaf3);
    node->pos = 3;
    result->nodes[3] = node;
    result->leaves[2] = node;
    nodeAddNeighbour(result->nodes[0], node, 0);
    nodeAddNeighbour(node, result->nodes[0], 0);
    result->leavesNum = 3;
    result->nodesNum = 4;
    return result;
}


TreeWithScore* getBestChildBME(TreeWithScore* treeWS, char* newLeafName, const Matrix& matrix) {
    Tree* parent = treeCopy(treeWS->tree, 0);
    Tree* curChild = treeAddLeaf(treeWS->tree, 0, 0, newLeafName, 1, 1);
    long double score = BME(curChild, matrix);

    TreeWithScore* bestChild = treeWithScoreCreate(curChild, score);
    for (int i = 0; i < parent->nodesNum; ++i) {
        Node* curNode = parent->nodes[i];
        for (int j = 0; j < curNode->neiNum; ++j) {
            if (curNode->neighbours[j]->pos <= i) {
                continue;
            }
            curChild = treeAddLeaf(parent, i, j, newLeafName, 0, 1);
            score = BME(curChild, matrix);
            if (score > bestChild->score) {
                treeDelete(bestChild->tree);
                bestChild->tree = treeCopy(curChild, 0);
                bestChild->score = score;
            }
            parent = treeRemoveLeaf(parent, parent->leavesNum - 1, 0, 0);
        }
    }
    return bestChild;
}


TreeWithScore* treeGrowBME(const Matrix& matrix, bool randLeaves) {
    std::vector<std::string> names = matrix.getNames();

    if (randLeaves) {
        auto rd = std::random_device{};
        auto rng = std::default_random_engine{ rd() };
        std::shuffle(std::begin(names), std::end(names), rng);
    }

    TreeWithScore* result = treeWithScoreCreate(
        growThreeLeavesTree(strdup(names[0].c_str()), strdup(names[1].c_str()), strdup(names[2].c_str())),
        0
    );

    for (int k = 3; k < names.size(); k++) {
        TreeWithScore* newResult = getBestChildBME(result, strdup(names[k].c_str()), matrix);
        treeWithScoreDelete(result);
        result = newResult;
    }

    return result;
}

TreeWithScore* oneTreeGrowBME(const Matrix& matrix, bool randLeaves) {
    TreeWithScore* result = treeGrowBME(matrix, randLeaves);
    return result;
}

TreeWithScore** multipleTreeGrowBME(const Matrix& matrix, bool randLeaves, unsigned int treeNum) {
    TreeWithScore** treeArray = (TreeWithScore**)malloc(sizeof(TreeWithScore*) * treeNum);
    for (int i = 0; i < treeNum; ++i) {
        treeArray[i] = treeGrowBME(matrix, randLeaves);
        std::cout << "Tree number: " << i + 1 << ", score: " << treeArray[i]->score << std::endl;
    }
    treeWithScoreSort(treeArray, treeNum);
    return treeArray;
}