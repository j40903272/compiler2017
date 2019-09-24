#include "printSourceFile.h"

char g_sourceFile[MAX_CODE_LINE_COUNT][MAX_LINE_LENGTH];

int printCommentError = 0;

void readSourceFile(char* fileName)
{
    FILE* fp = fopen(fileName, "r");
    if (!fp) {
        printf("Cannot open file \"%s\"", fileName);
        printCommentError = 1;
    } else {
        int index = 0;
        while (fgets(g_sourceFile[index], MAX_LINE_LENGTH, fp)) {
            ++index;
            if (index >= MAX_CODE_LINE_COUNT) {
                printCommentError = 1;
                break;
            }
        }
    }
}

void printSourceFile(FILE* outputFp, int lineNumber)
{
    if (!printCommentError) {
        fprintf(outputFp, "# %s\n", g_sourceFile[lineNumber - 1]);
    }
}
