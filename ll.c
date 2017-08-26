# include <stdlib.h>
# include <string.h>
# include <stdio.h>
# include "ll.h"

// based on linked list code from Prof. Darrell Long

listNode *newNode(const char *old, const char *trans)
{
  listNode *t = (listNode *)malloc(sizeof(listNode));
  t -> oldspeak = strdup(old);
  t -> newspeak = strdup(trans);
  t -> next = NIL;
  return t;
}

void delNode(listNode *n)
{
  free(n -> oldspeak);
  free(n -> newspeak);
  free(n);
  return;
}

void delLL(listNode *n)
{
  while (n != NIL)
  {
    listNode *t = n;
    n = n -> next;
    delNode(t);
  }
  return;
}


// if already in the list, return it, else point at start of list
listNode *insertLL(listNode **n, const char *old, const char *trans)
{
  if(findLL(n, old) == NIL)
  {
    listNode *t = newNode(old, trans);
    t -> next = *n; // added word is new head of the list
    return t;
  }
  else
  {
    return *n;
  }

}

/*
  Finds a word in a linked list. If moveToFront is enabled then when a
  word is found it will be moved to the front of the list. Therefore,
  common words such as "the" will be easily found and search time will
  be decreased.
*/
listNode *findLL(listNode **n, const char *old)
{
  findCall++;
  listNode *previous = NIL; // node before head
  for(listNode *t = *n; t != NIL; t = t -> next)
  {
    seeks++;
    if (strcmp(t -> oldspeak, old) == 0) // if the word is in the list
    {
      if(moveToFront && t -> next != NIL) // if moveToFront has been turned on
      {
        while(t -> next != NIL) // move word until it is at the front of LL
        {
          previous = t;
          t = t -> next;
        }
        previous -> next = NIL;
        t -> next = *n;
        *n = t; // set word to be the new head of the list
      }
      return t;
    }
  }
  return NIL;
}

void printLL(listNode *n)
{
  for(listNode *t = n; t != NIL; t = t -> next)
  {
    if (strlen(t -> newspeak) == 0) // word doesn't have a translation
		{
			printf("%s\n", t -> oldspeak);
		}
    else // word has a translation
    {
      printf("%s -> %s\n", t -> oldspeak, t -> newspeak);
    }
  }
  return;
}
