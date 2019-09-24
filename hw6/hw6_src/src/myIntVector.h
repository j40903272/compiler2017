#ifndef __MY_INT_VECTOR_H__
#define __MY_INT_VECTOR_H__

typedef struct MyIntVector
{
    int* data;
    int size;
    int capacity;
} MyIntVector;

MyIntVector* getMyIntVector(int initialCapacity);
void myPushBack(MyIntVector* myIntVector, int val);
void myPopBack(MyIntVector* myIntVector);

#endif
