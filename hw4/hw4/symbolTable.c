#include "symbolTable.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
// This file is for reference only, you are not required to follow the implementation. //

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
}

void enterIntoHashTrain(int hashIndex, SymbolTableEntry* entry)
{
}

void initializeSymbolTable()
{
    for(int i = 0 ; i < HASH_TABLE_SIZE ; i++)
        symbolTable.hashTable[i] = NULL;
    symbolTable.scopeDisplay = (SymbolTableEntry**)malloc(sizeof(SymbolTableEntry*));
    symbolTable.scopeDisplay[0] = NULL;
    symbolTable.currentLevel = 0;
    symbolTable.scopeDisplayElementCount = 0;

    InsertSymbol("int", createAttrType(TYPE_ATTRIBUTE, SCALAR_TYPE_DESCRIPTOR, INT_TYPE));
    InsertSymbol("float", createAttrType(TYPE_ATTRIBUTE, SCALAR_TYPE_DESCRIPTOR, FLOAT_TYPE));
    InsertSymbol("void", createAttrType(TYPE_ATTRIBUTE, SCALAR_TYPE_DESCRIPTOR, VOID_TYPE));
    InsertSymbol("read", createAttrFunc(FUNCTION_SIGNATURE, INT_TYPE));
    InsertSymbol("fread", createAttrFunc(FUNCTION_SIGNATURE, FLOAT_TYPE));
    InsertSymbol("write", createAttrFunc(FUNCTION_SIGNATURE, VOID_TYPE));
}

void symbolTableEnd()
{
    //puts("symbolTableEnd");
    for(int i = 0 ; i <= symbolTable.currentLevel ; i++){
        SymbolTableEntry* ptr = symbolTable.scopeDisplay[i];
        while(ptr != NULL){
            //printf("free %s\n", ptr->name);
            SymbolTableEntry* tmp = ptr;
            ptr = ptr->nextInSameLevel;
            free(tmp);
        }
    }        
}

SymbolTableEntry* retrieveSymbol(char* symbolName)
{
    int idx = HASH(symbolName);
    SymbolTableEntry* ptr = symbolTable.hashTable[idx];
    while(ptr != NULL)
        if(strcmp(symbolName, ptr->name) == 0)
            break;
    return ptr;
}

SymbolTableEntry* InsertSymbol(char* symbolName, SymbolAttribute* attribute)
{
    //printf("insert %s\n", symbolName);
    int idx = HASH(symbolName);
    SymbolTableEntry* ptr = symbolTable.hashTable[idx];

    SymbolTableEntry* new = newSymbolTableEntry(symbolTable.currentLevel);
    new->name = strdup(symbolName);
    new->nestingLevel = symbolTable.currentLevel;
    new->attribute = attribute;

    while(ptr != NULL && strcmp(ptr->name, symbolName) != 0)
        ptr = ptr->nextInHashChain;

    if(ptr == NULL){//insert new entry
        new->nextInHashChain = symbolTable.hashTable[idx];
        new->prevInHashChain = NULL;
        symbolTable.hashTable[idx] = new;
        new->nextInSameLevel = symbolTable.scopeDisplay[symbolTable.currentLevel];
        symbolTable.scopeDisplay[symbolTable.currentLevel] = new;
    }
    else{//name exist
        new->nextInHashChain = ptr->nextInHashChain;
        new->prevInHashChain = ptr->prevInHashChain;
        new->sameNameInOuterLevel = ptr;
        if(ptr->prevInHashChain == NULL)
            symbolTable.hashTable[idx] = new;
        else
            ptr->prevInHashChain->nextInHashChain = new;
        if(ptr->nextInHashChain != NULL)
            ptr->nextInHashChain->prevInHashChain = new;
        ptr->nextInHashChain = ptr->prevInHashChain = NULL;
    }
    return new;
}

//remove the symbol from the current scope
void removeSymbolEntry(SymbolTableEntry* ptr)
{
    SymbolTableEntry* n = ptr->nextInHashChain;
    SymbolTableEntry* p = ptr->prevInHashChain;
    SymbolTableEntry* o = ptr->sameNameInOuterLevel;
    int idx = HASH(ptr->name);
    free(ptr);
    if(o == NULL){
        if(n != NULL)
            n->prevInHashChain = p;
        if(p != NULL)
            p->nextInHashChain = n;
        else
            symbolTable.hashTable[idx] = n;
    }
    else{
        o->prevInHashChain = p;
        o->nextInHashChain = n;
        if(p != NULL)
            p->nextInHashChain = o;
        else
            symbolTable.hashTable[idx] = o;
        if(n != NULL)
            n->prevInHashChain = o;
    }
}

int declaredLocally(char* symbolName)
{
}

void openScope()
{
    symbolTable.currentLevel++;
    symbolTable.scopeDisplay = realloc(symbolTable.scopeDisplay, (symbolTable.currentLevel+1) * sizeof(SymbolTableEntry**));
    symbolTable.scopeDisplay[symbolTable.currentLevel] = NULL;
}

void closeScope()
{
    SymbolTableEntry* ptr = symbolTable.scopeDisplay[symbolTable.currentLevel];
    symbolTable.currentLevel--;
    while(ptr != NULL){
        SymbolTableEntry* tmp = ptr;
        ptr = ptr->nextInSameLevel;
        removeSymbolEntry(tmp);
    }
    symbolTable.scopeDisplay = realloc(symbolTable.scopeDisplay, (symbolTable.currentLevel+1) * sizeof(SymbolTableEntry*));
}

SymbolAttribute* createAttrType(SymbolAttributeKind attrKind, TypeDescriptorKind tdKind, DATA_TYPE type){
    SymbolAttribute* Attr = (SymbolAttribute*)malloc(sizeof(SymbolAttribute));
    Attr->attributeKind = attrKind;
    TypeDescriptor* TD = (TypeDescriptor*)malloc(sizeof(TypeDescriptor));
    TD->kind = tdKind;
    if(tdKind == SCALAR_TYPE_DESCRIPTOR)
        TD->properties.dataType = type;
    else if(tdKind == ARRAY_TYPE_DESCRIPTOR)
        TD->properties.arrayProperties.elementType = type;
    Attr->attr.typeDescriptor = TD;
    return Attr;
}

SymbolAttribute* createAttrFunc(SymbolAttributeKind attrKind, DATA_TYPE type){
    SymbolAttribute* Attr = (SymbolAttribute*)malloc(sizeof(SymbolAttribute));
    Attr->attributeKind = attrKind;
    Attr->attr.functionSignature = (FunctionSignature*)malloc(sizeof(FunctionSignature));
    Attr->attr.functionSignature->returnType = type;
    Attr->attr.functionSignature->parameterList = NULL;
    Attr->attr.functionSignature->parametersCount = 0;
    return Attr;
}