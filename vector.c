#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

void VectorNew(vector *v, int elemSize, VectorFreeFunction freeFn, int initialAllocation){
    assert(initialAllocation >= 0);

    v->allocLen = initialAllocation;
    v->logLen = 0;
    v->elemSize = elemSize;
    v->freeFunc = freeFn;

    v->base = malloc(v->allocLen * v->elemSize);
    assert(v->base != NULL);
}
void doubleSize(vector *v){
    v->allocLen *= 2;
    if(v->allocLen == 0) v->allocLen = 1;
    v->base = realloc(v->base, v->elemSize * v->allocLen);
}

void VectorDispose(vector *v){
    if(v->freeFunc != NULL) for(int i = 0; i < v->logLen; i++) v->freeFunc((char *)v->base + i * v->elemSize);
    free(v->base);
}


int VectorLength(const vector *v){
    return v->logLen;
}

void *VectorNth(const vector *v, int position){
    assert(position >= 0 && position < v->logLen);
    return (char *)v->base  + position * v->elemSize; 
}

void VectorReplace(vector *v, const void *elemAddr, int position){
    assert(position >= 0 && position < v->logLen);
    void* ptr = (char *)v->base + position * v->elemSize;
    if(v->freeFunc != NULL) v->freeFunc(ptr);
    memcpy(ptr, elemAddr, v->elemSize);
}

void VectorInsert(vector *v, const void *elemAddr, int position){
    assert(position  >= 0 && position <= v->logLen);
    if(v->logLen == v->allocLen) doubleSize(v);

    int numBytesToMove =  (v->logLen - position) * v->elemSize;
    void* ptr = (char *)v->base + position * v->elemSize;

    if (numBytesToMove!=0) memmove((char*)ptr + v->elemSize, ptr, numBytesToMove);
    memcpy(ptr,elemAddr, v->elemSize);

    v->logLen++;
}

void VectorAppend(vector *v, const void *elemAddr){
    if(v->logLen == v->allocLen) doubleSize(v);
    void* ptr = (char *)v->base + v->logLen * v->elemSize;
    memcpy(ptr,elemAddr,v->elemSize);
    v->logLen++;
}

void VectorDelete(vector *v, int position){
    assert(position >= 0 || position < v->logLen);

    int numBytesToMove =  (v->logLen - 1 - position) * v->elemSize;
    void* ptr = (char *)v->base + position * v->elemSize;

    if (numBytesToMove!=0) memmove(ptr, (char*)ptr + v->elemSize,  numBytesToMove);
    v->logLen--;
}

void VectorSort(vector *v, VectorCompareFunction compare){
    assert(compare != NULL);
    qsort(v->base, v->logLen, v->elemSize, compare);
}

void VectorMap(vector *v, VectorMapFunction mapFn, void *auxData){
    assert(mapFn != NULL);
    for(int i = 0; i < v->logLen; i++){
        void* ptr = (char*)v->base + i * v->elemSize;
        mapFn(ptr,auxData);
    }
}

static const int kNotFound = -1;

void* lsearch(const vector *v, const void *key, VectorCompareFunction searchFn, int startIndex){
    for(int i = startIndex; i < v->logLen; i++){
        void* ptr = (char*)v->base + i * v->elemSize;
        if(searchFn(ptr, key) == 0) return ptr;
    }
    return NULL;
}

int VectorSearch(const vector *v, const void *key, VectorCompareFunction searchFn, int startIndex, bool isSorted){
    assert(searchFn != NULL);
    assert(key != NULL);
    assert(startIndex >=0 && startIndex <= v->logLen);
    void* ptr;
    if(isSorted) ptr = bsearch(key, v->base, v->logLen, v->elemSize, searchFn);
    else ptr = lsearch(v,key,searchFn,startIndex);
    if(ptr == NULL) return kNotFound;
    return (int)((char*) ptr - (char*)v->base) / v->elemSize;
} 
