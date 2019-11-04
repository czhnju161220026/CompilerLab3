#ifndef __HASH_SET__
#define __HASH_SET__
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "symbol.h"
#define HASH_SIZE 16384

typedef struct Bucket
{
    Symbol *head;
} Bucket;

typedef struct HashSet
{
    int size;
    Bucket *buckets;
} HashSet;

HashSet *initializeHashSet(int size);
unsigned int pjwHash(char *str);
bool isContain(HashSet *hashSet, char *name);
bool insert(HashSet *hashSet, Symbol *symbol);
Symbol *get(HashSet *hashSet, char *name);
void outputHashSet(HashSet *hashSet);
#endif
