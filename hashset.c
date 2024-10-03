#include "hashset.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "vector.h"
#include <stdio.h>

struct frequency {
    char ch;		// a particular letter
    int occurrences;	// the number of times it occurs
};

void HashSetNew(hashset *h, int elemSize, int numBuckets,
	HashSetHashFunction hashfn, HashSetCompareFunction comparefn, HashSetFreeFunction freefn){
	assert(elemSize > 0);
	assert(numBuckets > 0);
	assert(hashfn != NULL);
	assert(comparefn != NULL);

	//printf("%d", elemSize);

	int startVecSize = 4;

	h->elemSize = elemSize;
	h->numBuckets = numBuckets;
	h->logLen = 0;

	h->arr = malloc(h->numBuckets * sizeof(vector));
	//printf("a--");
	for(int i = 0; i < h->numBuckets; i++) VectorNew(&(h->arr[i]), elemSize, freefn, startVecSize);
	//printf("a--");
	h->cmpFn = comparefn;
	h->freeFn = freefn;
	h->hashFn = hashfn;

	// for(int i = 0; i < h->numBuckets; i++){
	// 	printf("%d", (int)h->arr[i].base);
	// 	printf("\n");
	// }

}

void HashSetDispose(hashset *h){
	if(h->freeFn != NULL)
		for(int i = 0; i < h->numBuckets; i++) 
			VectorDispose(&(h->arr[i]));
	free(h->arr);
}	

int HashSetCount(const hashset *h){ 
	return h->logLen;
}

void HashSetMap(hashset *h, HashSetMapFunction mapfn, void *auxData){
	assert(mapfn != NULL);
	for(int i = 0; i < h->numBuckets; i++) VectorMap(&(h->arr[i]),mapfn, auxData);
}

static const int kNotFound = -1;
void HashSetEnter(hashset *h, const void *elemAddr){
	//printf("enter \n");
	assert(elemAddr != NULL);
	int hashCode = (h->hashFn(elemAddr, h->numBuckets));
	assert(hashCode >= 0 && hashCode < h->numBuckets);
	//printf("bucket id %d", hashCode);
	//printf("char %d", (char)*(char *)elemAddr);
	vector* bucket = &h->arr[hashCode];
	int found = VectorSearch(bucket,elemAddr,h->cmpFn,0,false);
	//printf("found enter  %d \n",found);
	if(found != kNotFound) VectorReplace(bucket, elemAddr, found);
	else{
		VectorAppend(bucket, elemAddr);
		h->logLen++;
	}
}

void *HashSetLookup(const hashset *h, const void *elemAddr){
	assert(elemAddr != NULL);
	int hashCode = (h->hashFn(elemAddr, h->numBuckets));
	assert(hashCode >= 0 && hashCode < h->numBuckets);
	vector* bucket = &h->arr[hashCode];
	int found = VectorSearch(bucket,elemAddr,h->cmpFn,0,false);
	//printf("found lookup  %d",found);
	if(found == kNotFound) return NULL;
	else return VectorNth(bucket, found);
}
