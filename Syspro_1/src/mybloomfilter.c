#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash_functions.h"
#include "mybloomfilter.h"

struct BloomFilter *BloomFilter_Creation(int size) {
  struct BloomFilter *newFilter;

  int i;

  newFilter = (struct BloomFilter *)malloc(sizeof(struct BloomFilter));

  newFilter->size = size;

  newFilter->data = (char *)malloc(sizeof(char) * size);

  for (i = 0; i < size; i++) {
    newFilter->data[i] = 0;
  }

  return newFilter;
}

void BloomFilter_Purge(struct BloomFilter *bf) {}

void BloomFilter_Insert(struct BloomFilter *bf, char* key) {}

void BloomFilter_Search(struct BloomFilter *bf, char* key) {}