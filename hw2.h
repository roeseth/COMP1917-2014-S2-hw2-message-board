/**
   hw2.h

   COMP1917 Computing 1

   Program supplied as a starting point for
   Assignment 2: Message Board

   UNSW Session 2, 2014
*/

#define TRUE           1
#define FALSE          0
#define NONE          -1

#define MAX_LINE     256
#define MAX_TEXT    4096

//Used for printing mode
#define BRIEF 0
#define THREAD 1
#define UNDOCRT 2

typedef struct date    Date;
typedef struct time    Time;
typedef struct msgNode MsgNode;
typedef int            MODE;

struct date {
  int day;
  int month;
  int year;
};

struct time {
  int hour;
  int minute;
  int second;
};

struct msgNode {
  int   messageNum;
  int   indent;
  int   deleted;
  char *name;
  Date  date;
  Time  time;
  char *text;
  MsgNode *inReplyTo;
  MsgNode *next;
  MsgNode *crtnext;
  MsgNode *subnext;
  MsgNode *subtail;

};

// INSERT NEW FUNCTION PROTOTYPES, AS APPROPRIATE

void   printPrompt();
void     printHelp();
MsgNode   *getNode( void );
MsgNode *replyNode( MsgNode * mother_node );
void      pushNode( MsgNode * new_node, MsgNode ** head, MsgNode ** last, int mode );
void       popNode( MsgNode ** head, MsgNode ** last, int mode );
char      *getName( void );
char      *getText( void );
void       getDate( Date *d );
int       scanDate( Date *d );
void       getTime( Time *t );
int       scanTime( Time *t );
int         dateOK( Date *d );
int         timeOK( Time *t );
void   printPadded( int n );
void     printDate( Date d );
void     printTime( Time t );
void     printLine( MsgNode * msg, int is_indented );
void    printBrief( MsgNode * msg );
void     printFull( MsgNode * msg );
void   printThread( MsgNode * head );
void     printList( MsgNode * head, MsgNode * current, int mode, int full );
MsgNode  *seekPrev( MsgNode * head, MsgNode * msg, int mode );
MsgNode   *seekNxt( MsgNode * head, MsgNode * msg, int mode );
MsgNode  *jumpNode( MsgNode * head, int num );
void  globalSearch( MsgNode * head, char * text, int mode );
int      deleteMsg( MsgNode * msg );
int    undeleteMsg( MsgNode * msg );
void      freeList( MsgNode *list );
