# ifndef NIL
# define NIL (void *) 0
# endif
# ifndef _BF_H
# define _BF_H
# include <stdint.h>
# include <stdlib.h>
# include <stdio.h>
# include "hash.h"

typedef struct bloomF {
  uint8_t *v;    // Vector
  uint32_t l;    // Length
  uint32_t s[4]; // Salt
}bloomF;


// Create a new Bloom Filter of a given length and hash function.
static inline bloomF * newBF(uint32_t len, uint32_t b[])
{
  bloomF *f;                                        // create pointer to bloom filter
  f = (bloomF *)calloc(1, sizeof(bloomF));          // allocate space for BF
  f -> l = len;                                     // assign length of BF
  f -> v = (uint8_t *)calloc(len, sizeof(uint8_t)); // allocate space for bitVector
  for(int i = 0; i < 4; i++)                        // copy array values into salt
  {
    (f -> s)[i] = b[i];
  }
  return f;
}

// Delete a bloom filter
static inline void delBF(bloomF *x)
{
  free(x -> v); // free bit vector
  free(x);      // free pointer
}

// Return the value of the length of BF
static inline uint32_t lenBF(bloomF *x)
{
  return x -> l;
}

// Each function has its own hash function, determined by the salt.
uint32_t hashBF(bloomF *x, const char * key)
{
  uint32_t index = hash((x -> s), key);
  index = index % lenBF(x);
  return index;
}

// Return the value of position k in the Bloom filter
static inline uint32_t valBF(bloomF *x, uint32_t k)
{
  return ((x -> v)[k >> 3] & (01 << (k & 07))) >> (k & 07);
}

// Count number of 1 bits in the Bloom filter
static inline uint32_t countBF(bloomF *x)
{
  uint32_t count = 0;

  for (uint32_t i = 0; i < lenBF(x); i++)
  {
    if(valBF(x, i))
    {
      count++;
    }
  }
  return count;
}

// Set an entry in the Bloom filter
static inline void setBF(bloomF *x, char * key)
{
  uint32_t k = hashBF(x, key);
  (x -> v)[k >> 3] |= (01 << (k & 07)); // set bit
}

// Clear an entry in the Bloom filter
static inline void clrBF(bloomF *x, char * key)
{
  uint32_t k = hashBF(x, key);
  (x -> v)[k >> 3] &= ~(01 << (k & 07)); // clear bit
}

// Check membership in the Bloom filter
static inline uint32_t memBF(bloomF *x, char * key)
{
  // hash with same hash functions
  // see if those values are in bitV
  uint32_t k = hashBF(x, key);

  if(valBF(x, k))
  {
    return 1; // it is a member
  }
  else
  {
    return 0; // it is not a member
  }
}

// Print the bloom filter
static inline void printBF(bloomF *x)
{
  for(uint32_t i = 0; i < lenBF(x); i++)
  {
    if(valBF(x, i))
    {
      printf("1");
    }
    else
    {
      printf("0");
    }
  }
}

# endif
