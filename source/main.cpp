#include <iostream>
#include <cstdio>
#include <filesystem>

#include "GrowTreeBME.h" 

int main(int argc, char** argv) {
    for (auto& p : std::filesystem::directory_iterator("data/experiments/45/Matrixes")) {
        std::string filename = p.path().filename().string();
        Matrix matrix(("data/experiments/45/Matrixes/" + filename).c_str());
        TreeWithScore* tree = oneTreeGrowBME(matrix, true);
        treeWrite(tree->tree, ("data/experiments/45/TestTrees/" + filename).data());
    }
    return 0;
}
