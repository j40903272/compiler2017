#ifndef __PRINT_SOURCE_FILE_H__
#define __PRINT_SOURCE_FILE_H__

#include <stdio.h>

#define MAX_LINE_LENGTH 100
#define MAX_CODE_LINE_COUNT 100

extern char g_sourceFile[MAX_LINE_LENGTH][MAX_CODE_LINE_COUNT];

void readSourceFile(char* fileName);
void printSourceFile(FILE* outputFp, int lineNumber);

#endif
