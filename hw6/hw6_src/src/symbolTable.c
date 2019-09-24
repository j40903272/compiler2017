#include "symbolTable.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int HASH(char * str) {
	int idx=0;
	while (*str){
		idx = idx << 1;
		idx+=*str;
		str++;
	}
	return (idx & (HASH_TABLE_SIZE-1));
}

SymbolTable symbolTable;

SymbolTableEntry* newSymbolTableEntry(int nestingLevel)
{
    SymbolTableEntry* symbolTableEntry = (SymbolTableEntry*)malloc(sizeof(SymbolTableEntry));
    symbolTableEntry->nextInHashChain = NULL;
    symbolTableEntry->prevInHashChain = NULL;
    symbolTableEntry->nextInSameLevel = NULL;
    symbolTableEntry->sameNameInOuterLevel = NULL;
    symbolTableEntry->attribute = NULL;
    symbolTableEntry->name = NULL;
    symbolTableEntry->nestingLevel = nestingLevel;
    return symbolTableEntry;
}

void removeFromHashTrain(int hashIndex, SymbolTableEntry* entry)
{
    if(entry->prevInHashChain)
    {
        entry->prevInHashChain->nextInHashChain = entry->nextInHashChain;
    }
    else
    {
        symbolTable.hashTable[hashIndex] = entry->nextInHashChain;
    }


    if(entry->nextInHashChain)
    {
        entry->nextInHashChain->prevInHashChain = entry->prevInHashChain;
    }

    entry->nextInHashChain = NULL;
    entry->prevInHashChain = NULL;
}

void enterIntoHashTrain(int hashIndex, SymbolTableEntry* entry)
{
    SymbolTableEntry* chainHead = symbolTable.hashTable[hashIndex];
    if(chainHead)
    {
        chainHead->prevInHashChain = entry;
        entry->nextInHashChain = chainHead;
    }
    symbolTable.hashTable[hashIndex] = entry;
}

void initializeSymbolTable()
{
    symbolTable.currentLevel = 0;
    symbolTable.scopeDisplayElementCount = 10;
    symbolTable.scopeDisplay = (SymbolTableEntry**)malloc(symbolTable.scopeDisplayElementCount * sizeof(SymbolTableEntry*));
    int index = 0;
    for(index = 0; index != symbolTable.scopeDisplayElementCount; ++index)
    {
        symbolTable.scopeDisplay[index] = NULL;
    }
    for(index = 0; index != HASH_TABLE_SIZE; ++index)
    {
        symbolTable.hashTable[index] = NULL;
    }

    SymbolAttribute* intAttribute = (SymbolAttribute*)malloc(sizeof(SymbolAttribute));
    intAttribute->attributeKind = TYPE_ATTRIBUTE;
    intAttribute->attr.typeDescriptor = (TypeDescriptor*)malloc(sizeof(TypeDescriptor));
    intAttribute->attr.typeDescriptor->kind = SCALAR_TYPE_DESCRIPTOR;
    intAttribute->attr.typeDescriptor->properties.dataType = INT_TYPE;
    intAttribute->offsetInAR = -1;
    enterSymbol(SYMBOL_TABLE_INT_NAME, intAttribute);

    SymbolAttribute* floatAttribute = (SymbolAttribute*)malloc(sizeof(SymbolAttribute));
    floatAttribute->attributeKind = TYPE_ATTRIBUTE;
    floatAttribute->attr.typeDescriptor = (TypeDescriptor*)malloc(sizeof(TypeDescriptor));
    floatAttribute->attr.typeDescriptor->kind = SCALAR_TYPE_DESCRIPTOR;
    floatAttribute->attr.typeDescriptor->properties.dataType = FLOAT_TYPE;
    floatAttribute->offsetInAR = -1;
    enterSymbol(SYMBOL_TABLE_FLOAT_NAME, floatAttribute);

    SymbolAttribute* voidAttribute = (SymbolAttribute*)malloc(sizeof(SymbolAttribute));
    voidAttribute->attributeKind = TYPE_ATTRIBUTE;
    voidAttribute->attr.typeDescriptor = (TypeDescriptor*)malloc(sizeof(TypeDescriptor));
    voidAttribute->attr.typeDescriptor->kind = SCALAR_TYPE_DESCRIPTOR;
    voidAttribute->attr.typeDescriptor->properties.dataType = VOID_TYPE;
    voidAttribute->offsetInAR = -1;
    enterSymbol(SYMBOL_TABLE_VOID_NAME, voidAttribute);

    SymbolAttribute* readAttribute = NULL;
    readAttribute = (SymbolAttribute*)malloc(sizeof(SymbolAttribute));
    readAttribute->attributeKind = FUNCTION_SIGNATURE;
    readAttribute->attr.functionSignature = (FunctionSignature*)malloc(sizeof(FunctionSignature));
    readAttribute->attr.functionSignature->returnType = INT_TYPE;
    readAttribute->attr.functionSignature->parameterList = NULL;
    readAttribute->attr.functionSignature->parametersCount = 0;
    readAttribute->offsetInAR = -1;
    enterSymbol(SYMBOL_TABLE_SYS_LIB_READ, readAttribute);

    SymbolAttribute* freadAttribute = NULL;
    freadAttribute = (SymbolAttribute*)malloc(sizeof(SymbolAttribute));
    freadAttribute->attributeKind = FUNCTION_SIGNATURE;
    freadAttribute->attr.functionSignature = (FunctionSignature*)malloc(sizeof(FunctionSignature));
    freadAttribute->attr.functionSignature->returnType = FLOAT_TYPE;
    freadAttribute->attr.functionSignature->parameterList = NULL;
    freadAttribute->attr.functionSignature->parametersCount = 0;
    freadAttribute->offsetInAR = -1;
    enterSymbol(SYMBOL_TABLE_SYS_LIB_FREAD, freadAttribute);
}

void symbolTableEnd()
{
    // clean up
}

SymbolTableEntry* retrieveSymbol(char* symbolName)
{
    int hashIndex = HASH(symbolName);
    SymbolTableEntry* hashChain = symbolTable.hashTable[hashIndex];
    while(hashChain)
    {
        if(strcmp(hashChain->name, symbolName) == 0)
        {
            return hashChain;
        }
        else
        {
            hashChain = hashChain->nextInHashChain;
        }
    }
    return NULL;
}

SymbolTableEntry* enterSymbol(char* symbolName, SymbolAttribute* attribute)
{
    int hashIndex = HASH(symbolName);
    SymbolTableEntry* hashChain = symbolTable.hashTable[hashIndex];
    SymbolTableEntry* newEntry = newSymbolTableEntry(symbolTable.currentLevel);
    newEntry->attribute = attribute;
    newEntry->name = symbolName;

    while(hashChain)
    {
        if(strcmp(hashChain->name, symbolName) == 0)
        {
            if(hashChain->nestingLevel == symbolTable.currentLevel)
            {
                printf("void enterSymbol(...): ID \'%s\' is redeclared(at the same level#%d).\n", symbolName, symbolTable.currentLevel);
                free(newEntry);
                return NULL;
            }
            else
            {
                removeFromHashTrain(hashIndex, hashChain);
                newEntry->sameNameInOuterLevel = hashChain;
                break;
            }
        }
        else
        {
            hashChain = hashChain->nextInHashChain;
        }
    }
    enterIntoHashTrain(hashIndex, newEntry);
    newEntry->nextInSameLevel = symbolTable.scopeDisplay[symbolTable.currentLevel];
    symbolTable.scopeDisplay[symbolTable.currentLevel] = newEntry;
    
    return newEntry;
}

//remove the symbol from the current scope
void removeSymbol(char* symbolName)
{
    int hashIndex = HASH(symbolName);
    SymbolTableEntry* hashChain = symbolTable.hashTable[hashIndex];
    while(hashChain)
    {
        if(strcmp(hashChain->name, symbolName) == 0)
        {
            if(hashChain->nestingLevel != symbolTable.currentLevel)
            {
                printf("void removeSymbol(...) Error: try to removed ID \'%s\' from the scope other than currentScope.\n", symbolName);
                return;
            }
            else
            {
                removeFromHashTrain(hashIndex, hashChain);
                if(hashChain->sameNameInOuterLevel)
                {
                    enterIntoHashTrain(hashIndex, hashChain->sameNameInOuterLevel);
                }
                break;
            }
        }
        else
        {
            hashChain = hashChain->nextInHashChain;
        }
    }

    if(!hashChain)
    {
        printf("void removeSymbol(...) Error: try to removed ID \'%s\' not in the symbol table.\n", symbolName);
        return;
    }

    SymbolTableEntry* tmpPrev = NULL;
    SymbolTableEntry* scopeChain = symbolTable.scopeDisplay[symbolTable.currentLevel];
    while(scopeChain)
    {
        if(strcmp(scopeChain->name, symbolName) == 0)
        {
            if(tmpPrev)
            {
                tmpPrev->nextInSameLevel = scopeChain->nextInSameLevel;
            }
            else
            {
                symbolTable.scopeDisplay[symbolTable.currentLevel] = scopeChain->nextInSameLevel;
            }
            free(scopeChain);
            break;
        }
        else
        {
            tmpPrev = scopeChain;
            scopeChain = scopeChain->nextInSameLevel;
        }
    }
}

int declaredLocally(char* symbolName)
{
    int hashIndex = HASH(symbolName);
    SymbolTableEntry* hashChain = symbolTable.hashTable[hashIndex];
    while(hashChain)
    {
        if(strcmp(hashChain->name, symbolName) == 0)
        {
            if(hashChain->nestingLevel == symbolTable.currentLevel)
            {
                return 1;
            }
            else
            {
                return 0;
            }
        }
        else
        {
            hashChain = hashChain->nextInHashChain;
        }
    }
    return 0;
}

void openScope()
{
    ++symbolTable.currentLevel;
    if(symbolTable.currentLevel == symbolTable.scopeDisplayElementCount)
    {
        SymbolTableEntry** oldScopeDisplay = symbolTable.scopeDisplay;
        symbolTable.scopeDisplay = (SymbolTableEntry**)malloc(symbolTable.scopeDisplayElementCount * 2 * sizeof(SymbolTableEntry*));
        memcpy(symbolTable.scopeDisplay, oldScopeDisplay, symbolTable.scopeDisplayElementCount * sizeof(SymbolTableEntry*));
        int index = 0;
        for(index = symbolTable.scopeDisplayElementCount; index != symbolTable.scopeDisplayElementCount * 2; ++index)
        {
            symbolTable.scopeDisplay[index] = NULL;
        }
        symbolTable.scopeDisplayElementCount = 2 * symbolTable.scopeDisplayElementCount;
        free(oldScopeDisplay);
    }
    //
    symbolTable.scopeDisplay[symbolTable.currentLevel] = NULL;
    //
}

void closeScope()
{
    if(symbolTable.currentLevel < 0)
    {
        printf("void closeScope(): Error: current level < 0. No scope can be close.\n");
        return;
    }
    SymbolTableEntry* scopeChain = symbolTable.scopeDisplay[symbolTable.currentLevel];
    SymbolTableEntry* nextScopeChain = NULL;
    while(scopeChain)
    {
        int hashIndex = HASH(scopeChain->name);
        removeFromHashTrain(hashIndex, scopeChain);
        if(scopeChain->sameNameInOuterLevel)
        {
            enterIntoHashTrain(hashIndex, scopeChain->sameNameInOuterLevel);
        }
        nextScopeChain = scopeChain->nextInSameLevel;
        scopeChain = nextScopeChain;
    }
    //
    symbolTable.scopeDisplay[symbolTable.currentLevel] = NULL;
    //
    --symbolTable.currentLevel;
}


int isGlobalVariable(SymbolTableEntry* symbolTableEntry)
{
    return symbolTableEntry->nestingLevel == 0;
}


int getVariableSize(TypeDescriptor *typeDescriptor)
{
    if(typeDescriptor->kind == SCALAR_TYPE_DESCRIPTOR)
    {
        //INT_TYPE and FLOAT_TYPE
        return 4;
    }
    else if(typeDescriptor->kind == ARRAY_TYPE_DESCRIPTOR)
    {
        ArrayProperties* arrayPropertiesPtr = &(typeDescriptor->properties.arrayProperties);
        
        int arrayElementCount = 1;
        int index = 0;
        for(index = 0; index < arrayPropertiesPtr->dimension; ++index)
        {
            arrayElementCount *= arrayPropertiesPtr->sizeInEachDimension[index];
        }

        return arrayElementCount * 4;
    }
    else
    {
        printf("Error in int getVariableSize(TypeDescriptor *typeDescriptor)\n");
        return -1;
    }
}
