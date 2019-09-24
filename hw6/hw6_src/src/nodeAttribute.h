#ifndef __NODE_ARRTIBUTE_H__
#define __NODE_ARRTIBUTE_H__

#define MAX_ARRAY_DIMENSION 10
typedef enum
{
    INT,
    FLOAT,
    ARRAY
} DATA_TYPE;

typedef struct DataTypeDescriptor ElementTypeDescriptor;

typedef struct ArrayProperties
{
    int dimension;
    int sizeInEachDimension[MAX_ARRAY_DIMENSION];
    //point to a DataTypeDescriptor in the symbol table;
    ElementTypeDescriptor *elementType;
} ArrayProperties;

typedef struct Parameter
{
    //point to a DataTypeDescriptor in the symbol table;
    DataTypeDescriptor* type;
} Parameter;

typedef struct ParameterList
{
    Parameter parameter;
    struct ParameterList* next;
} ParameterList;

typedef struct ParameterList ParameterList;

typedef struct TypeDescriptor ReturnTypeDescriptor;

typedef struct DataTypeDescriptor
{
    DATA_TYPE type;
    union
    {
        ArrayProperties arrayProperties;
    } properties;
} DataTypeDescriptor;

typedef struct FunctionSignature
{
    int parametersCount;
    ParameterList* parameterList;
    //point to a DataTypeDescriptor in the symbol table;
    DataTypeDescriptor *returnType;
} FunctionSignature;

#endif
