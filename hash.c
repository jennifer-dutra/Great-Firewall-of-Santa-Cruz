# include <stdlib.h>
# include <stdint.h>
# include <string.h>
# include <stdio.h>
# include "aes.h"
# include "hash.h"

static inline int realLength(int l)
{
  return 16 * (l / 16 + (l % 16 ? 1 : 0));
}

/*
  Hash function was provided by Prof. Darrell Long. and
  I altered it to work for either a BF & HT.
*/
uint32_t hash(uint32_t s[], const char * key)
{
  uint32_t output[4] = {0x0};
  uint32_t sum = 0x0;
  int keyL = strlen(key);
  uint8_t * realKey = (uint8_t *)calloc(realLength(keyL), sizeof(uint8_t));
  memcpy(realKey, key, keyL);

  for (int i = 0; i < realLength(keyL); i += 16)
  {
    AES128_ECB_encrypt((uint8_t *)s, (uint8_t *)realKey + i, (uint8_t *)output);
    sum ^= output [0] ^ output [1] ^ output [2] ^ output [3];
  }
  free(realKey);
  return sum;
}

// create a new hash table of a specified length
hashTable *newHT(uint32_t len, uint32_t b[])
{
  hashTable *ht = (hashTable *)malloc(sizeof(hashTable));
  ht -> l = len;
  ht -> h = (listNode **)malloc(sizeof(listNode *) * len);
  for(int i = 0; i < 4; i++) // copy array values into salt
  {
    (ht -> s)[i] = b[i];
  }
  return ht;
}

// delete a hash table
void delHT(hashTable *ht)
{
  for(uint32_t i = 0; i < ht -> l; i++)
  {
    free(ht -> h[i]);
  }
  free(ht -> h);
  free(ht);
}

// find existing translation entry
listNode *findHT(hashTable *ht, const char *key)
{
  uint32_t index = hash(ht -> s, key); // get index using hash function
  index = index % (ht -> l);           // make sure index isn't greater than HT size
  return findLL(&ht -> h[index], key); // search through LL at this index
}

// create an entry in the hash table
void insertHT(hashTable *ht , const char *key , const char *trans)
{
  uint32_t index = hash(ht -> s, key);
  index = index % (ht -> l);
  ht -> h[index] = insertLL(&ht -> h[index], key, trans); // insert into LL
  return;
}

// print translation with its corresponding index in the hash table
void printHT(const hashTable *ht)
{
  for(uint32_t i = 0; i < ht -> l; i++)
  {
    if(ht -> h[i] != NIL)
    {
      printf("h[%u]: ", i);
      printLL(ht -> h[i]);
    }
  }
}
