#ifndef __MY_REGISTER_H__
#define __MY_REGISTER_H__
#include <stdio.h>
#include "myIntVector.h"

#define INT_REGISTER_COUNT 5 
#define INT_WORK_REGISTER_COUNT 2
#define INT_OTHER_REGISTER_COUNT 11
#define FLOAT_REGISTER_COUNT 6
#define FLOAT_WORK_REGISTER_COUNT 2

extern char* zeroRegisterName;

extern char* intRegisterName[];

extern char* intWorkRegisterName[];

extern char* intRegisterName_64[];

extern char* intWorkRegisterName_64[];

extern char* floatRegisterName[];

extern char* floatWorkRegisterName[];


typedef struct IntRegisterTable
{
    int isAllocated[INT_REGISTER_COUNT];
    int is64[INT_REGISTER_COUNT];
} IntRegisterTable;

typedef struct FloatRegisterTable
{
    int isAllocated[FLOAT_REGISTER_COUNT];
} FloatRegisterTable;

typedef struct PseudoRegisterTable
{
    MyIntVector* isAllocatedVector;
} PseudoRegisterTable;

typedef enum ProcessorType
{
    INT_REG,
    FLOAT_REG
} ProcessorType;

extern IntRegisterTable g_intRegisterTable;
extern FloatRegisterTable g_floatRegisterTable;
extern PseudoRegisterTable g_pseudoRegisterTable;
extern int g_pseudoRegisterBeginOffset;

void initializeRegisterTable();

void resetRegisterTable(int maxLocalVariableOffset);

int getRegister(ProcessorType processorType);
void freeRegister(ProcessorType processorType, int registerIndex);
void printStoreRegister(FILE* codeGenOutputFp); 
void printRestoreRegister(FILE* codeGenOutputFp);
int getPseudoRegisterCorrespondingOffset(int pseudoRegisterIndex);

#endif
