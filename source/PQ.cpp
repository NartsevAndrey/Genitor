#include <stdio.h>
#include <assert.h>

#include "growTreeBME.h"
#include "genitor.h"

extern "C" {
    #include "TreeWS.h"
    #include "HashAlignment.h"
    #include "PWM.h"
    #include "Tree.h"
}

#define TRUE 1
#define FALSE 0

void printLongHelp() {
    printf("PQ version 1.6\n");
    printf("General options:\n");
    printf(" -matrix <FileName> \n");
    printf("       File with distance matrix\n");
    printf("       Required, no default value\n");
    printf(" -out <FileName>\n");
    printf("       Output file\n");
    printf("       Default: pq_out.tre\n");

    printf("\n");
    printf("Optimization options:\n");
    printf(" -grType <String>\n");
    printf("      \"one\": grow a single tree by stepwise addition of leaves,\n");
    printf("      \"multiple\":  grow many trees\n");
    printf("      Default: \"multiple\"\n");
    printf("      --treeNum <Positive integer>\n");
    printf("           for \"multiple\" option: number of trees to grow\n");
    printf("           Default: 10\n");
    printf("      --chType <String>\n");
    printf("           \"bestScore\" : choose tree with best score\n");
    printf("           \"genitor\" : search for optimal tree with genetic algorithm\n");
    printf("                --iterNum <Positive integer>\n");
    printf("                    for \"genitor\" option: number of iterations to perform\n");
    printf("                    Default: 10 times tree size\n");
    printf("                --iterNew <Positive integer>\n");
    printf("                    for \"genitor\" option: number of iterations during which leader hasn't changed to stop after\n");
    printf("                    Default: 5 times tree size\n");
    printf("                --iterLim <Positive integer>\n");
    printf("                    for \"genitor\" option: number of failures to stop after\n");
    printf("                    Default: 10\n");
    printf("      Default: \"bestScore\"\n");
    printf("      Growing is not performed if an initial tree is given\n");
    printf(" -randLeaves <0 or 1>\n");
    printf("      If 1, shuffle the input order of sequences\n");
    printf("      Switching to 0 makes sense for grType = \"one\")\n");
    printf("      Default: 1\n");
    return;
} /* printLongHelp */

void printHelp(char* command) {
    /* printf("Help message for PQ ver 1.5\n"); */
    printf("Usage:\n");
    printf("%s -matrix <FileName> -out <FileName>\n", command);
    printf("\t[-grType <one|multiple> [-randLeaves <0|1>]] [--treeNum <int>]\n");
    printf("\t\t\t[--iterNum <int>] [--iterNew <int>] [--iterLim <int>]\n");
    printf("\t\t[--trTime <int>] [--initTemp <int>] [--mcStyle <1|2|3>]]\n");
    printf("\t\t[-distrFile <FileName>]\n");
    printf("\t[-resultTreeNum <int>] [-sampleType <simple|bootstrap|jackknife> ]\n");
    printf("\t\t[--removeFraction <int>]\n");
    printf("For description of parameters run %s -h\n", command);
    puts("Press Enter to continue");
    getchar();
    return;
}

int main(int argc, char** argv) {
    Matrix matrix;
    char* outFileName = NULL;
    unsigned i;
    unsigned j;
    int randLeaves = 1;
    char* grType;
    unsigned treeNum = 10;
    unsigned iterNum = 0;
    unsigned iterNew = 0;
    unsigned iterLim = 10;
    unsigned long int trTime = 1000;
    unsigned int initTemp = 1000;
    unsigned int mcStyle = 1;
    char* sampleType;
    char* distrFileName = NULL;
    unsigned randTreeNum = 10;
    unsigned resultTreeNum = 1;
    double removeFraction = 0.5;
    char extended = 1;
    char* param;
    int known;
    int startOptionsNum;
    TreeWithScore* result = NULL;
    TreeWithScore** trees;
    Tree** treesTemp;
    char** treeNames;
    char** seqNames;
    int* permutation;
    TreeWithScore** neighbours;
    FILE* distrFile;
    TreeWithScore** treeSample;
    FILE* inTreeStream;
    char* newick;
    unsigned newickSize;
    unsigned end;
    char* chType;

    /* default values for string parameters */
    grType = (char*)malloc(sizeof(char) * 12);
    strcpy(grType, "multiple");
    chType = (char*)malloc(sizeof(char) * 12);
    strcpy(chType, "bestScore");
    sampleType = (char*)malloc(sizeof(char) * 12);
    strcpy(sampleType, "simple");

    /* Help */
    if (argc == 1) {
        printHelp(argv[0]);
        exit(0);
    }
    if ((strcmp(argv[1], "-h") == 0) || (strcmp(argv[1], "-help") == 0) || (strcmp(argv[1], "--help") == 0)) {
        printLongHelp();
        exit(0);
    }

    /* *********************************************
     *    Parsing command line and input
     *    *****************************************/

    param = (char*)malloc(sizeof(char) * 128);
    startOptionsNum = 1;
    while (startOptionsNum < argc) {
        strcpy(param, argv[startOptionsNum]);
        
        known = 0;      /* we do not recognize this argument yet */
        if (param[0] == '-') {
            if (strcmp(param, "-matrix") == 0) {
                known = 1;
                if (startOptionsNum + 1 < argc) {
                    char* matrixFileName = (char*)malloc(sizeof(char) * (strlen(argv[startOptionsNum + 1] + 1)));
                    strcpy(matrixFileName, argv[startOptionsNum + 1]);
                    matrix = Matrix(matrixFileName);
                }
                // !!!
                printf("Start computing\n");
            }
            if (strcmp(param, "-out") == 0) {
                known = 1;
                if (startOptionsNum + 1 < argc) {
                    outFileName = (char*)malloc(sizeof(char) * (strlen(argv[startOptionsNum + 1] + 1)));
                    strcpy(outFileName, argv[startOptionsNum + 1]);
                }
                printf("Start computing\n");
            }
            if (strcmp(param, "-storeHash") == 0) {
                known = 1;
                fprintf(stderr, "Warning: setting -storeHash has no effect in this version, it is always 1\n");
            }
            if (strcmp(param, "-randLeaves") == 0) {
                known = 1;
                if (startOptionsNum + 1 < argc) {
                    randLeaves = atoi(argv[startOptionsNum + 1]);
                }
            }
            if (strcmp(param, "-grType") == 0) {
                known = 1;
                if (startOptionsNum + 1 < argc) {
                    if (strlen(argv[startOptionsNum + 1]) < 12) {
                        strcpy(grType, argv[startOptionsNum + 1]);
                    }
                    else {
                        fprintf(stderr, "Unknown grType: %s -- ignored\n", argv[startOptionsNum + 1]);
                    }
                }
            }
            if (strcmp(param, "--treeNum") == 0) {
                known = 1;
                if (startOptionsNum + 1 < argc) {
                    treeNum = atoi(argv[startOptionsNum + 1]);
                }
            }
            if (strcmp(param, "--chType") == 0) {
                known = 1;
                if (startOptionsNum + 1 < argc) {
                    chType = argv[startOptionsNum + 1];
                }
            }
            if (strcmp(param, "--iterNum") == 0) {
                known = 1;
                if (startOptionsNum + 1 < argc) {
                    iterNum = atoi(argv[startOptionsNum + 1]);
                }
            }
            if (strcmp(param, "--iterNew") == 0) {
                known = 1;
                if (startOptionsNum + 1 < argc) {
                    iterNew = atoi(argv[startOptionsNum + 1]);
                }
            }
            if (strcmp(param, "--iterLim") == 0) {
                known = 1;
                if (startOptionsNum + 1 < argc) {
                    iterLim = atoi(argv[startOptionsNum + 1]);
                }
            }
            if (strcmp(param, "--trTime") == 0) {
                known = 1;
                if (startOptionsNum + 1 < argc) {
                    trTime = atoi(argv[startOptionsNum + 1]);
                }
            }
            if (strcmp(param, "--initTemp") == 0) {
                known = 1;
                if (startOptionsNum + 1 < argc) {
                    initTemp = atoi(argv[startOptionsNum + 1]);
                }
            }
            if (strcmp(param, "--mcStyle") == 0) {
                known = 1;
                if (startOptionsNum + 1 < argc) {
                    mcStyle = atoi(argv[startOptionsNum + 1]);
                }
            }
            if (strcmp(param, "-distrFile") == 0) {
                known = 1;
                if (startOptionsNum + 1 < argc)  {
                    distrFileName = (char*)malloc(sizeof(char) * (strlen(argv[startOptionsNum + 1] + 1)));
                    strcpy(distrFileName, argv[startOptionsNum + 1]);
                }
            }
            if (strcmp(param, "--randTreeNum") == 0) {
                known = 1;
                if (startOptionsNum + 1 < argc) {
                    randTreeNum = atoi(argv[startOptionsNum + 1]);
                }
            }
            if (strcmp(param, "-resultTreeNum") == 0) {
                known = 1;
                if (startOptionsNum + 1 < argc) {
                    resultTreeNum = atoi(argv[startOptionsNum + 1]);
                }
            }
            if (strcmp(param, "-sampleType") == 0) {
                known = 1;
                if (startOptionsNum + 1 < argc) {
                    sampleType = (char*)malloc(sizeof(char) *
                        (strlen(argv[startOptionsNum + 1]) + 1));
                    strcpy(sampleType, argv[startOptionsNum + 1]);
                }
            }
            if (strcmp(param, "--removeFraction") == 0) {
                known = 1;
                if (startOptionsNum + 1 < argc) {
                    removeFraction = atof(argv[startOptionsNum + 1]);
                }
            }
            if (strcmp(param, "--extended") == 0) {
                known = 1;
                if (startOptionsNum + 1 < argc) {
                    extended = atoi(argv[startOptionsNum + 1]);
                }
            }
        } /* if argument starts with '-' */
        if (known == 0) /* we cannot recognize this argument name ... */ {
            printf("%s\n", argv[startOptionsNum]);
            fprintf(stderr, "Wrong command line argument: %s\n", param);
            exit(1);
        }
        else {
            startOptionsNum += 2;
        }
    }  /* while */

    if (outFileName == NULL) {
        fprintf(stderr, "Warning: no output file name is given; the result will be written to \"pq_out.tre\"\n");
        outFileName = (char*)malloc(sizeof(char) * 12);
        strcpy(outFileName, "pq_out.tre");
    }

    /***********************************************************
     *                     Calculations                        *
     ***********************************************************/

    std::vector<TreeWithScore*> resultTrees(resultTreeNum);
    for (j = 0; j < resultTreeNum; ++j) {
        if (strcmp(grType, "one") == 0) {
            result = oneTreeGrowBME(matrix, randLeaves);
        }
        else if (strcmp(grType, "multiple") == 0)
        {
            trees = multipleTreeGrowBME(matrix, randLeaves, treeNum);

            if (strcmp(chType, "bestScore") == 0) {
                result = trees[treeNum - 1];
                for (i = 0; i < treeNum - 1; i++) {
                    treeWithScoreDelete(trees[i]);
                }
            }
            else if ((strcmp(chType, "genitor") == 0)) {
                printf("starting genitor\n");
                if (iterNum == 0) {
                    iterNum = 10 * trees[0]->tree->leavesNum;
                }
                if (iterNew == 0) {
                    iterNew = 5 * trees[0]->tree->leavesNum;
                }
                result = genitor(trees, treeNum, matrix, iterNum, iterNew, iterLim); // !!!!
                for (i = 0; i < treeNum; ++i) {
                    treeWithScoreDelete(trees[i]);
                }
            }
            else {
                fprintf(stderr, "Wrong value for chType: %s, PQ:main\n", chType);
                exit(1);
            }
        }
        else if (strcmp(grType, "saturation") == 0) {
            fprintf(stderr, "Option \"%s\" of grType is not ready yet\n", grType);
            exit(1);
        }
        else {
            fprintf(stderr, "Wrong value for grType: %s\n", grType);
            exit(1);
        }

        std::cout << "Score of result tree is " << result->score << std::endl;
        resultTrees[j] = result;
    }
    /* Output */

    for (i = 0; i < resultTreeNum; ++i) {
        std::string filename(outFileName);
        treeWrite(resultTrees[i]->tree, (filename + "_" + std::to_string(i)).data());
    }

    return 0;
} /* main */