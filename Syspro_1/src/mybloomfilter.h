#ifndef MYBLOOMFILTER
#define MYBLOOMFILTER


struct BloomFilter {
    int size;
    char * data;
};


struct BloomFilter * BloomFilter_Creation(int size);

void BloomFilter_Purge(struct BloomFilter *bf);

void BloomFilter_Insert(struct BloomFilter *bf, char* key);

void BloomFilter_Search(struct BloomFilter *bf, char* key);

#endif

