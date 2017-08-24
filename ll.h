# ifndef NIL
# define NIL (void *) 0
# endif

# ifndef _LL_H

# define _LL_H

# include <stdbool.h>
# include <stdint.h>

extern uint32_t moveToFront;
extern uint32_t seeks;
extern uint32_t findCall;

typedef struct listNode listNode;

struct listNode
{
char *oldspeak, *newspeak;
listNode *next;
};

listNode *newNode(const char *, const char *);

void delNode(listNode *);

void delLL(listNode *);

listNode *insertLL(listNode **, const char *, const char *);

listNode *findLL(listNode **, const char *);

void printLL(listNode *);
# endif
