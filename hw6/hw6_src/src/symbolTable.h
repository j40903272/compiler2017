#ifndef __SYMBOL_TABLE_H__
#define __SYMBOL_TABLE_H__

#include "header.h"

//SYMBOL_TABLE_PREINSERT_NAME
#define SYMBOL_TABLE_INT_NAME "int"
#define SYMBOL_TABLE_FLOAT_NAME "float"
#define SYMBOL_TABLE_VOID_NAME "void"
#define SYMBOL_TABLE_SYS_LIB_READ "read"
#define SYMBOL_TABLE_SYS_LIB_FREAD "fread"
#define HASH_TABLE_SIZE 256


typedef enum SymbolAttributeKind
{
    VARIABLE_ATTRIBUTE,
    TYPE_ATTRIBUTE,
    FUNCTION_SIGNATURE
} SymbolAttributeKind;

typedef enum TypeDescriptorKind
{
    SCALAR_TYPE_DESCRIPTOR,
    ARRAY_TYPE_DESCRIPTOR,
} TypeDescriptorKind;

typedef struct ArrayProperties
{
    int dimension;
    int sizeInEachDimension[MAX_ARRAY_DIMENSION];
    DATA_TYPE elementType;
} ArrayProperties;

typedef struct TypeDescriptor
{
    TypeDescriptorKind kind;
    union
    {
        DATA_TYPE dataType;//kind: SCALAR_TYPE_DESCRIPTOR
        ArrayProperties arrayProperties;//kind: ARRAY_TYPE_DESCRIPTOR
    } properties;
} TypeDescriptor;

typedef struct Parameter
{
    //point to a TypeDescriptor in the symbol table;
    struct Parameter* next;
    TypeDescriptor* type;
    char* parameterName;
} Parameter;

typedef struct FunctionSignature
{
    int parametersCount;
    Parameter* parameterList;
    DATA_TYPE returnType;
} FunctionSignature;

typedef struct SymbolAttribute
{
    SymbolAttributeKind attributeKind;

    union
    {
        TypeDescriptor* typeDescriptor;
        FunctionSignature* functionSignature;
    } attr;
    int offsetInAR;
} SymbolAttribute;

typedef struct SymbolTableEntry
{
    struct SymbolTableEntry* nextInHashChain;
    struct SymbolTableEntry* prevInHashChain;
    struct SymbolTableEntry* nextInSameLevel;
    struct SymbolTableEntry* sameNameInOuterLevel;

    char* name;
    SymbolAttribute* attribute;
    int nestingLevel;

} SymbolTableEntry;

typedef struct SymbolTable
{
    SymbolTableEntry* hashTable[HASH_TABLE_SIZE];
    SymbolTableEntry** scopeDisplay;
    int currentLevel;
    int scopeDisplayElementCount;
} SymbolTable;


void initializeSymbolTable();
void symbolTableEnd();
SymbolTableEntry* retrieveSymbol(char* symbolName);
SymbolTableEntry* enterSymbol(char* symbolName, SymbolAttribute* attribute);
void removeSymbol(char* symbolName);
int declaredLocally(char* symbolName);
void openScope();
void closeScope();

int isGlobalVariable(SymbolTableEntry* symbolTableEntry);
int getVariableSize(TypeDescriptor *typeDescriptor);

#endif
