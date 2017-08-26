# include <stdint.h>
# include <stdio.h>
# include <string.h>
# include <ctype.h>
# include <getopt.h>
# include <math.h>
# include <stdlib.h>

# include "bf.h"
# include "hash.h"
# include "ll.h"

# define BADSPEAK "/afs/cats.ucsc.edu/users/g/darrell/badspeak.txt"
# define NEWSPEAK "/afs/cats.ucsc.edu/users/g/darrell/newspeak.txt"

# define TRUE 1
# define FALSE 0
# ifndef NIL
# define NIL (void *) 0
# endif

// Stats counters
uint32_t dictionary = 0;      // number of words in badspeak dictionary
uint32_t translations = 0;    // number of newspeak translations
uint32_t text = 0;            // number of words read from stdin
uint32_t seeks = 0;           // everytime a linked list is iterated by one step
uint32_t findCall = 0;        // average number of seeks per findLL call
uint32_t bitsSet = 0;         // number of bits set in a bloom filter

uint32_t wordLen = 100;       // maximum length for any word
uint32_t moveToFront = TRUE;  // MTF rule true by default

// User input
extern char* yytext;
extern FILE* yyin;

int yylex(void);


/*
  The badspeak dictionary contains a list of words that are forbidden.
  These words are set in the bloom filter so that the program can quickly
  check whether a word from stdin is in one of the dictionaries. It is then added
  to the hash table with no translation. If a translataion is found in the
  newspeak dictionary, it will be added to the entry in the hash table.
*/

void loadBadspeak(bloomF *f1, bloomF *f2, hashTable *h)
{
  FILE *badspeakf;
  char badWord[wordLen];
  printf("\nLoading badspeak dictionary...\n");
  badspeakf = fopen(BADSPEAK, "r");
  if(badspeakf == NIL)
  {
    perror("Failed");
    exit(EXIT_FAILURE);
  }
  else
  {
    printf("Done.\n");
  }
  while(fscanf(badspeakf, "%s \n", badWord) == 1)
  {
    // set words in both bloom filters and then insert them into the hash table
    dictionary++;
    setBF(f1, badWord);
    setBF(f2, badWord);
    insertHT(h, badWord, ""); // add word with no translation
  }
  fclose(badspeakf);
}


/*
  The newspeak dictionary contains a list of translations. It takes selected
  words and gives their suggested "newspeak" replacement.
  Ex. quicksort --> doubleplusgoodsort
  The old version of the word is set in the bloom filter and then the
  translation is added to the hash table.
*/

void loadTranslations(bloomF *f1, bloomF *f2, hashTable *h)
{
  FILE *newspeakf;
  char old[wordLen];
  char new[wordLen];
  printf("Loading newspeak dictionary...\n");
  newspeakf = fopen(NEWSPEAK, "r");
  if(newspeakf == NIL)
  {
    perror("Failed");
    exit(EXIT_FAILURE);
  }
  else
  {
    printf("Done.\n");
  }
  while(fscanf(newspeakf, "%s %s \n", old, new) == 2)
  {
    translations++;
    setBF(f1, old);
    setBF(f2, old);

    listNode *bothLists = findHT(h, old); // search for word in hash table
    if(bothLists == NIL) // word is not found in both the badspeak and newspeak lists
    {
      insertHT(h, old, new); // add new translation to hash table
    }
    else
    {
      free(bothLists -> newspeak);
      bothLists -> newspeak = strdup(new); // add newspeak translation for badspeak word
    }
  }
  fclose(newspeakf);
}


/*
  Print corresponding letter depending on the results.
  Any suggested translations or bad words used will be printed after the letter.
*/

void printLetter(uint32_t crimeCommitted, uint32_t foundTranslations, listNode* goodSpeak, listNode* thoughtCrime)
{
  if(crimeCommitted == FALSE && foundTranslations == FALSE) // no suggestions or thoughtcrime
  {
    printf("\nDear Comrade,\n\n"
    "Big brother thanks you for using goodspeak. We are doubleplus proud of your\nuse of newspeak words.\n\n");
  }
  else if(foundTranslations && crimeCommitted == FALSE) // no thoughtcrime with suggestions
  {
    printf("\nDear Comrade,\n\n"
    "Submitting your text helps to preserve feelings and prevent badthink.\nSome of the words that you used are not goodspeak.\nThe list shows how to turn the oldspeak words into newspeak.\n\n");
    printLL(goodSpeak);
    printf("\n");
  }
  else if(crimeCommitted) // thoughtcrime was committed
  {
    printf("\nDear Comrade,\n\n"
    "You have chosen to use degenerate words that may cause hurt\nfeelings or cause your comrades to think unpleasant thoughts.\nThis is doubleplus bad. To correct your wrongthink and\nsave community consensus we will be sending you to joycamp\nadministered by Miniluv.\n\n"
    "Your errors:\n\n");
    printLL(thoughtCrime);
    printf("\n");
    if(foundTranslations)
    {
      printf("\nThink on these words during your vacation!\n\n");
      printLL(goodSpeak);
      printf("\n");
    }
  }
}


/*
  If the user chooses to suppress the letter, stats will be printed instead.
*/

void printStats(bloomF *f1, bloomF *f2)
{
  float average = (float)seeks/findCall;
  float density1 = (float)countBF(f1)/lenBF(f1);
  float density2 = (float)countBF(f2)/lenBF(f2);
  printf("\nSeeks: %d, Average: %f, Translations: %d, Dictionary: %d, Text: %d", seeks, average, translations, dictionary, text);
  printf("\nDensities: %f, %f\n", density1, density2);
}


int main(int argc, char *argv[])
{
  uint32_t bfSize = pow(2, 20);       // default bloom filter size
  uint32_t htSize = 10000;            // default hash table size
  uint32_t stats = FALSE;             // letter is printed by default
  uint32_t crimeCommitted = FALSE;    // if text included forbidden words
  uint32_t foundTranslations = FALSE; // if words used have translations
  int c;                              // for option flags

  // User option flags
  while ((c = getopt (argc, argv, "sh:f:mb")) != -1)
  {
    switch(c)
    {
      case 's': // suppress letter and print statistics instead
      {
        stats = TRUE;
        break;
      }
      case 'h': // user can specify the number of hash table entries
      {
        htSize = atoi(optarg);
        break;
      }
      case 'f': // user can specify the number of bloom filter entries
      {
        bfSize = atoi(optarg);
        break;
      }
      case 'm': // move to front rule will be used (increases efficiency of findLL)
      {
        moveToFront = true;
        break;
      }
      case 'b': // move to front rule will not be used
      {
        moveToFront = false;
        break;
      }
    }
  }

  // Salts
  uint32_t initA[] = {0xDeadD00d, 0xFadedBee, 0xBadAb0de, 0xC0c0aB0a}; // First Bloom filter
  uint32_t initB[] = {0xDeadBeef, 0xFadedB0a, 0xCafeD00d, 0xC0c0aB0a}; // Second Bloom filter
  uint32_t initH[] = {0xDeadD00d, 0xFadedBee, 0xBadAb0de, 0xC0c0Babe}; // Hash table

  bloomF *f1 = newBF(bfSize, initA);
  bloomF *f2 = newBF(bfSize, initB);
  hashTable *h = newHT(htSize, initH);

  // Load dictionaries for approved and forbidden words
  loadBadspeak(f1, f2, h);
  loadTranslations(f1, f2, h);

  printf("\nEnter some text so that we can ensure you are using goodspeak:\n");

  /*
    All user input is read in using the flex lexical analyzer. Each word is
    checked to see if it is in one of the dictionaries. If it is a member of both
    bloom filters then the hash table is checked for a translation. All bad
    words used and translations found are stored in a linked list to be printed later.
  */

  int tokenID = 0;
  yyin = stdin;

  listNode *thoughtCrime = NIL; // list of words read in with no translation
  listNode *goodSpeak = NIL;    // list of words read in with translation

  while(tokenID != -1)
	{
    text++;
    tokenID = yylex();
    char userInput[wordLen];
    strcpy(userInput, yytext);
    listNode *word; // if a translation is found for current word it will be stored here

    // change all user input chars to lower case
    for(uint32_t i = 0; i < strlen(yytext); i++)
    {
      userInput[i] = tolower(userInput[i]);
    }

    if(memBF(f1, userInput) && memBF(f2, userInput)) // word is member of both bloom filters
    {
      word = findHT(h, userInput); // search for a translation
      if(word != NIL && strlen(word -> newspeak) != 0) // there is a translation
      {
        foundTranslations = TRUE;
        goodSpeak = insertLL(&goodSpeak, word -> oldspeak, word -> newspeak); // store word in linked list
      }
      else // no translation found
      {
        crimeCommitted = TRUE;
        thoughtCrime = insertLL(&thoughtCrime, word -> oldspeak, word -> newspeak); // store word in linked list
      }
    }
	}

  printf("\nText analyzed.\n");

  if(stats)
  {
    printStats(f1, f2);
  }
  else
  {
    printLetter(crimeCommitted, foundTranslations, goodSpeak, thoughtCrime);
  }

  delLL(goodSpeak);
  delLL(thoughtCrime);
  delBF(f1);
  delBF(f2);
  delHT(h);
  return 0;
}
