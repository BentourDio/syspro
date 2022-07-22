/* 
  Dionysios Mpentour 1115201300115
  
  BloomFilter Data structure and respective functions
*/

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

  newFilter->data = (int *)malloc(sizeof(int) * size);

  for (i = 0; i < size; i++) {
    newFilter->data[i] = 0;
  }

  return newFilter;
}

void BloomFilter_Purge(struct BloomFilter *bf) {
  free(bf->data);
  free(bf);
}

void BloomFilter_Insert(struct BloomFilter *bf, char* key) {
  unsigned long k;
  int index, bit;

  for (int i =0; i<16; i++){
    k = hash_i((unsigned char*)key, i);

    k = k % bf->size;

    index = k / 32;
    bit = k % 32;

    unsigned int donkey = 1;
    donkey = donkey << bit;
    bf->data[index] = bf->data[index] | donkey;
  }
}

void BloomFilter_Search(struct BloomFilter *bf, char* key, int *success) {

  unsigned long k;
  int index, bit;

  for (int i = 0; i < 16; i++) {
    k = hash_i((unsigned char *)key, i);

    k = k % bf->size;

    index = k / 32;
    bit = k % 32;

    unsigned int donkey = 1;
    donkey = donkey << bit;
    
    if ((bf->data[index] & donkey) ==0){
      (*success = 1);
      return;
      }
  }
  (*success =0);
}