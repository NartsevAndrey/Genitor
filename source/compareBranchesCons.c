//#include <stdio.h>
//#include <stdlib.h>
//#include <dirent.h>
//
//#include "Tree.h"
//#include "consensus.h"
//
//typedef struct
//{
//    Branch* branch;
//    long* leafDeep;
//    double boot_score; // bootstrap score
//}BranchInfo;
//
//BranchInfo* branchInfoCreate(size_t leavesNum)
//{
//    int i = 0;
//    BranchInfo* branchInfo = malloc(sizeof(BranchInfo));
//    branchInfo->branch = branchCreate(leavesNum);
//    branchInfo->leafDeep = malloc(sizeof(long) * leavesNum);
//    branchInfo->boot_score = -1;
//
//    for(i = 0; i < leavesNum; ++i)
//    {
//        branchInfo->leafDeep[i] = -1;
//    }
//    return branchInfo;
//}
//
//void branchInfoDelete(BranchInfo* branchInfo)
//{
//    free(branchInfo->leafDeep);
//    branchDelete(branchInfo->branch);
//}
//
//BranchInfo** treeToBranchInfo(Tree* tree, int* permutation)
//{
//    int i = 0;
//    int j = 0;
//    int k = 0;
//    long dist1 = 0;
//    long dist2 = 0;
//    INT p = 0;
//    size_t notTrivialBranchesNum = 0;
//    BranchInfo** branchInfoArr = NULL;
//    BranchInfo** branchInfoArrRmTrivial = NULL;
//    size_t curBranchId = 0;
//    char* isTrivial = NULL;
//
//    if (tree->leavesNum <= 3)
//    {
//        return branchInfoArr;
//    } 
//
//    branchInfoArr = malloc(sizeof(BranchInfo*) * tree->nodesNum);
//    treeWash(tree);
//    for(i = 0; i < tree->nodesNum; ++i)
//    {
//        branchInfoArr[i] = branchInfoCreate(tree->leavesNum);
//    }
//
//    for(i = 0; i < tree->leavesNum; ++i)
//    {// trivial branches
//        p = 1;
//        p = p << (permutation[i] & (intSize - 1));
//        branchInfoArr[tree->leaves[i]->pos]->branch->branch[permutation[i] / intSize] |= p;
//    
//        branchInfoArr[tree->leaves[i]->pos]->leafDeep[i] = 0;
//        branchInfoArr[tree->leaves[i]->pos]->boot_score = 0;
//        tree->leaves[i]->color = BLACK;
//    }
//    
//    NodeStack* stack = nodeStackCreate(tree->nodesNum);
//    Node* curNode = NULL;
//
//    Node* rootNode = tree->leaves[0]->neighbours[0];
//    curNode = rootNode; 
//    Node* nextNode = NULL; 
//
//    nodeStackPush(stack, curNode);
//    curNode->color = GREY;
//
//    while (stack->curSize != 0)
//    {
//        curNode = nodeStackPeek(stack);
//        nextNode = 0;
//        for(i = 0; i < curNode->neiNum; ++i)
//        {
//            if (curNode->neighbours[i]->color == WHITE)
//            {
//                nextNode = curNode->neighbours[i];
//                // also set bootstrap value for branch
//                branchInfoArr[nextNode->pos]->boot_score =
//                    curNode->dist[i];
//            }
//        }
//        if (nextNode)
//        {
//            for(i = 0; i < tree->leavesNum; ++i)
//            {
//                dist1 = treeGetDist(tree, 
//                    curNode->pos, tree->leaves[permutation[i]]->pos);
//                dist2 = treeGetDist(tree, nextNode->pos,
//                        tree->leaves[permutation[i]]->pos);
//                if (dist1 > dist2)
//                {
//                    dist1 = dist2;
//                }
//                branchInfoArr[nextNode->pos]->leafDeep[permutation[i]] = dist1;
//            }
//            nodeStackPush(stack, nextNode);
//            nextNode->color = GREY;
//        }
//        else
//        {
//            for(i = 0; i < curNode->neiNum; ++i)
//            {
//                for(j = 0; j < tree->leavesNum / 64 + 1; ++j)
//                {
//                    branchInfoArr[curNode->pos]->branch->branch[j] |= \
//                            branchInfoArr[curNode->neighbours[i]->pos]->branch->branch[j];
//                }
//            }
//
//            nodeStackPop(stack);
//            curNode->color = BLACK;
//        }
//    }
//
//    isTrivial = calloc(sizeof(char), tree->nodesNum);
//
//    isTrivial[rootNode->pos] = 1;
//    for (i = 0; i < tree->leavesNum; ++i)
//    {
//        isTrivial[tree->leaves[i]->pos] = 1;
//    }
//
//
//    notTrivialBranchesNum = tree->nodesNum - tree->leavesNum - 1; 
//    branchInfoArrRmTrivial = malloc(sizeof(BranchInfo) * notTrivialBranchesNum);
//    j = 0;
//    for(i = 0; i < tree->nodesNum; ++i)
//    {
//        if (!isTrivial[i])
//        {
//            branchNormalize(branchInfoArr[i]->branch);
//            branchInfoArrRmTrivial[j++] = branchInfoArr[i];
//        } 
//    }
//
//    return branchInfoArrRmTrivial;
//}
//
//
//int main(int argsNum, char** args)
//{
//
//    assert(argsNum == 3);
//
//    int i = 0;
//    int j = 0;
//    int k = 0;
//    DIR* d = NULL;;
//    size_t fileNum = 0;
//
//    Tree* etalon = NULL;
//    Tree* tree = NULL;
//    int isAgree = 0;
//    char* pathName = malloc(sizeof(char) * 1000);
//    char** treeNames = NULL;
//    char** etalonNames = NULL;
//    int* permutation = NULL;
//    Branch* temp = NULL;
//    BranchInfo** etalonBranchInfo = NULL;
//    BranchInfo** treeBranchInfo = NULL;
//
//    d = opendir(args[2]);
//    if(!d)
//    {
//        raiseError("Wrong dir name",
//                __FILE__, __FUNCTION__, __LINE__);
//    }
//
//    fprintf(stdout, "tree_file_name;leaf_dist;agree_with_etalon\n");
//    while((dir = readdir(d)) != NULL)
//    {
//        if (
//            strlen(dir->d_name) < 4 ||
//            ( strcmp(".tre", dir->d_name + strlen(dir->d_name) - 4) && 
//            strcmp(".nwk", dir->d_name + strlen(dir->d_name) - 4))
//        )
//        {
//            continue;
//        } 
//        sprintf(pathName, "%s/%s", args[2], dir->d_name);
//
//        tree = treeRead(pathName);
//        treeNames = treeGetNames(tree);
//
//        etalon = treePrune(fullEtalon, treeNames,
//            tree->leavesNum, 1);
//        etalonNames = treeGetNames(etalon);
//        permutation = getRange(0, tree->leavesNum);
//        etalonBranchInfo = treeToBranchInfo(etalon, permutation);
//        free(permutation);
//
//        permutation = calculatePermutation(
//            treeNames, etalonNames,
//            tree->leavesNum);
//
//        if (!permutation)
//        {
//            fprintf(stderr, "Don't find some names in etalon tree\n");
//            exit(1);
//        }
//
//        int branchNum = tree->nodesNum - tree->leavesNum - 1;
//        treeBranchInfo = treeToBranchInfo(tree, permutation);
//
//
//        char isFound = 0;
//        for(i = 0; i < branchNum; ++i)
//        {
//            //write path to tree file to file
//            fprintf(stdout, "%s;\"%ld", pathName, etalonBranchInfo[i]->leafDeep[0]);
//
//            //write leaves deeps in tree
//            for(k = 1; k < tree->leavesNum; ++k)
//            {
//                fprintf(stdout, ";%ld", 
//                        treeBranchInfo[i]->leafDeep[k]);
//            }
//            fprintf(stdout, "\"");
//
//            isFound = 0;
//
//            for(j = 0; (j < branchNum) && (!isFound); ++j) 
//                // search for branch i in etalon
//            {
//                if (branchCompare(treeBranchInfo[i]->branch,
//                            etalonBranchInfo[j]->branch) == 0)
//                {
//                    isFound = 1;
//
//                    fprintf(stdout, ";\"%ld", etalonBranchInfo[j]->leafDeep[0]);
//                    for(k = 1; k < tree->leavesNum; ++k)
//                    {
//                        fprintf(stdout, ";%ld", etalonBranchInfo[j]->leafDeep[k]);
//                    }
//                    fprintf(stdout, "\"");
//                }
//            }
//            if (!isFound)
//            {
//                fprintf(stdout, ";\"None\"");
//            }
//            fprintf(stdout, ";%.01lf\n", treeBranchInfo[i]->boot_score);
//        }
//
//        for(i = 0; i < branchNum; ++i)
//        {
//            branchInfoDelete(etalonBranchInfo[i]);
//            branchInfoDelete(treeBranchInfo[i]);
//        }
//
//        free(treeBranchInfo);
//        free(etalonBranchInfo);
//        treeDelete(tree);
//        treeDelete(etalon);
//        free(permutation);
//    }
//}
