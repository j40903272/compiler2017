#include <stdlib.h>
#include <string.h>
#include "myIntVector.h"


MyIntVector* getMyIntVector(int initialCapacity)
{
    if(initialCapacity < 0)
    {
        exit(EXIT_FAILURE);
    }

    MyIntVector* myIntVectorPtr = (MyIntVector*)malloc(sizeof(MyIntVector));
    myIntVectorPtr->capacity = initialCapacity;
    myIntVectorPtr->size = 0;
    if(initialCapacity != 0)
    {
        myIntVectorPtr->data = (int*)malloc(sizeof(int) * initialCapacity);
    }
    else
    {
        myIntVectorPtr->data = NULL;
    }

    return myIntVectorPtr;
}


void myPushBack(MyIntVector* myIntVector, int val)
{
    if(myIntVector->size == myIntVector->capacity)
    {
        int* oldData = myIntVector->data;
        int oldCapacity = myIntVector->capacity;
        myIntVector->capacity = myIntVector->capacity * 2 + 1;
        myIntVector->data = (int*)malloc(sizeof(int) * myIntVector->capacity);
        memcpy(myIntVector->data, oldData, sizeof(int) * oldCapacity);
        if(oldData)
        {
            free(oldData);
        }
    }

    myIntVector->data[myIntVector->size] = val;
    ++myIntVector->size;
}

void myPopBack(MyIntVector* myIntVector)
{
    if(myIntVector->size == 0)
    {
        exit(EXIT_FAILURE);
    }
    --myIntVector->size;
}

