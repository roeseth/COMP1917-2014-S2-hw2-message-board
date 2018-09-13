/*
   hw2.c

   COMP1917 Computing 1

   Program supplied as a starting point for
   Assignment 2: Message Board

   UNSW Session 2, 2014
   
   Completed by Group #

   Yulun Wu z3486584
   Qisheng Wu z3458792
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#include "hw2.h"

/************************************************************
  This program using a sub-linked-list structure to store threads and their sub-posts.
  Chain of posts ordered in original sequence:

    (*list)POST 1 --next--> POST 2 --next--> POST 3(*tail)

  POST 0: or (MsgNode)threadhead, a node to complement structure as layer 0.
  Chain of threads:

    Main chain: POST 0 --subnext--> POST 1 --crtnext--> POST 2 --crtnext--> POST 3

    POST 1 --subnext--> SUB POST 1 --crtnext--> SUB POST 2 (post1.subtail)
                          |
                          |--subnext-->SUB SUB POST 1 --crtnext--> SUB SUB POST 2

*/

int globalMessageNum = 1;
MsgNode *globalCurrent = NULL; //pointer to current post
MsgNode *globalCrtUndo = NULL; //pointer to the post of previous action

MODE globalUndoMode = NONE; //to record the type of previous action,initialized as NONE

int main( void )
{
  MsgNode *list = NULL;
  MsgNode *tail = NULL; //to record the tail of the main linked list
  
  MsgNode threadhead; //node POST 0
  threadhead.subnext = NULL;
  threadhead.subtail = NULL;
  
  MsgNode *node;
  MsgNode *tempCurrent; //used for checking if Forward/Back function has effect.
  
  char command[MAX_LINE];
  char searchtext[MAX_LINE];

  char c;
  int n; //to record the target message number in jump function

  MODE printmode = BRIEF;
  MODE undo_printmode = NONE; //to record previous mode of printing

  int isFull = TRUE; //to indicate if a detailed message needs to be printed out.
  int undo_isFull = NONE; //to record previous value of isFull

  printPrompt();

  // enter a loop, reading and executing commands from the user
  while( fgets(command,MAX_LINE,stdin) != NULL ) {
    char *p;

    // replace newline with end-of-string character
    if(( p = strchr(command,'\n')) != NULL ) {
      *p = '\0';
    }
    p = command;
    while( isspace(*p)) { // skip any initial spaces
      p++;
    }
    c = *p;

    if( isdigit(c)) {
      sscanf( command,"%d",&n );
      globalCrtUndo = globalCurrent;
      globalCurrent = jumpNode( list, n );
      globalUndoMode = 'K';
      printList( list, globalCurrent, printmode, isFull );
      // INSERT CODE FOR JUMPING TO MESSAGE k
    }
    else switch( c ) {

    case 'a': case 'A': // Add item
      // MODIFY THIS CODE, AS APPROPRIATE
      node = getNode();
      //push node into list and modify tail. This is for BRIEF mode.
      pushNode( node, &list, &tail, BRIEF );
      //push node into main chain for THREAD mode.
      pushNode( node, &(threadhead.subnext), &(threadhead.subtail), THREAD );
      globalCrtUndo = globalCurrent;
      globalCurrent = node;
      globalUndoMode = 'A';
      printList( list, globalCurrent, printmode, isFull );
      break;

    case 'l': case 'L': // Toggle the printing mode
      undo_printmode = printmode;
      printmode = BRIEF;
      undo_isFull = isFull;
      isFull = FALSE;
      globalUndoMode = 'L';
      if( globalCurrent == NULL ) break;
      printList( list, globalCurrent, printmode, isFull );
      break;
    
    case 'p': case 'P':
      undo_isFull = isFull;
      isFull = TRUE;
      globalUndoMode = 'P';
      if( globalCurrent == NULL ) break;
      printList( list, globalCurrent, printmode, isFull );
      break;
    
    case 'f': case 'F':
      if( globalCurrent == NULL ) break;
      tempCurrent = globalCurrent;
      if( tempCurrent != (globalCurrent = seekNxt( list, globalCurrent, printmode )) )
        printList( list, globalCurrent, printmode, isFull );
      globalUndoMode = 'F';
      break;
    
    case 'b': case 'B':
      if( globalCurrent == NULL ) break;
      tempCurrent = globalCurrent;
      if( tempCurrent != (globalCurrent = seekPrev( list, globalCurrent, printmode )) )
        printList( list, globalCurrent, printmode, isFull );
      globalUndoMode = 'B';
      break;
    
    case 'd': case 'D':
      if( globalCurrent == NULL ) break;
      if( deleteMsg( globalCurrent ) )
        printList( list, globalCurrent, printmode, isFull );
      globalUndoMode = 'D';
      break;
      
    case 'r': case 'R':
      if( globalCurrent == NULL ) break;
       //like CASE 'A'. Instead of getNode(), a modified version replyNode() is implemented.
      node = replyNode( globalCurrent );
      pushNode( node, &list, &tail, BRIEF );
      globalCrtUndo = globalCurrent;
      globalCurrent = node;
      globalUndoMode = 'R';
      printList( list, globalCurrent, printmode, isFull );
      break;
    
    case 't': case 'T':
      undo_printmode = printmode;
      printmode = THREAD;
      undo_isFull = isFull;
      isFull = FALSE;
      globalUndoMode = 'T';
      if( globalCurrent == NULL ) break;
      printList( list, globalCurrent, printmode, isFull );
      break;
    
    case 's': case 'S':
      if( globalCurrent == NULL ) break;
      printf( "Search text: \n" );
      fgets( searchtext, MAX_LINE, stdin );
      searchtext[ strlen(searchtext)-1 ] = '\0'; //replce the last '\n' with '\0'
      globalSearch( list, searchtext, printmode );
      break;

    case 'u': case 'U':
      if( globalUndoMode == NONE ) break;
      //Bascilly just reverse what a funtion has done.
      switch( globalUndoMode ) {
        case 'A':
          popNode( &list, &tail, BRIEF );
          popNode( &(threadhead.subnext), &(threadhead.subtail), THREAD );
          globalCurrent = globalCrtUndo;
          globalCrtUndo = NULL;
          globalMessageNum--;
          printList( list, globalCurrent, printmode, isFull );
          break;
        
        case 'L': case 'P': case 'T':
          printmode = undo_printmode;
          undo_printmode = NONE;
          isFull = undo_isFull;
          undo_isFull = NONE;
          printList( list, globalCurrent, printmode, isFull );
          break;

        case 'F':
          tempCurrent = globalCurrent;
          if( tempCurrent != (globalCurrent = seekPrev( list, globalCurrent, printmode )) )
          printList( list, globalCurrent, printmode, isFull );
          break;

        case 'B':
          tempCurrent = globalCurrent;
          if( tempCurrent != (globalCurrent = seekNxt( list, globalCurrent, printmode )) )
          printList( list, globalCurrent, printmode, isFull );
          break;

        case 'K':
          globalCurrent = globalCrtUndo;
          globalCrtUndo = NULL;
          printList( list, globalCurrent, printmode, isFull );
          break;

        case 'D':
          if( undeleteMsg( globalCurrent ) )
          printList( list, globalCurrent, printmode, isFull );
          break;

        case 'R':
          popNode( &list, &tail, BRIEF );
          globalCurrent = globalCrtUndo;
          popNode( &(globalCurrent->subnext), &(globalCurrent->subtail), THREAD );
          globalCrtUndo = NULL;
          globalMessageNum--;
          printList( list, globalCurrent, printmode, isFull );
          break;
      }
      globalUndoMode = NONE;
      break;

    case 'h': case 'H': // Help
      printHelp();
      break;

    case 'q': case 'Q': // Quit
      freeList( list );
      printf("Bye!\n");
      return 0;
      break;
    }

    printPrompt();
  }

  return 0;
}

// INSERT NEW FUNCTIONS, AS APPROPRIATE

/************************************************************
  Print prompt only if output goes to screen
*/
void printPrompt()
{
  if (isatty(fileno(stdin))) {
    printf("Enter command (A,F,B,P,L,D,R,T,S,U,Q, H for Help): ");
  }
}

/************************************************************
  Print the list of commands available to the user,
  and a brief summary of what each command does.
*/
void printHelp()
{
  printf("\n");
  printf(" A - Add\n" );
  printf(" L - List\n" );
  printf(" P - Print\n" );
  printf(" F - Forward\n" );
  printf(" B - Back\n" );
  printf("<k>- jump to message k\n");
  printf(" D - Delete\n");
  printf(" R - Reply\n");
  printf(" T - Threads\n");
  printf(" S - Search\n");
  printf(" U - Undo\n" );
  printf(" Q - Quit\n");
  printf(" H - Help\n");
  printf("\n");
}

/************************************************************
  Allocate space for a new message and get the
  name, date, time and text from standard input.
*/
MsgNode * getNode( void )
{
  MsgNode * new_node;
  new_node = (MsgNode *)malloc(sizeof(MsgNode));
  if( new_node == NULL ) {
     printf("Error: could not allocate memory.\n");
     exit( 1 );
  }
  new_node->messageNum= globalMessageNum++;
  new_node->indent    = 0;
  new_node->deleted   = FALSE;
  new_node->name      = getName();
  getDate( &new_node->date );
  getTime( &new_node->time );
  new_node->text      = getText();
  new_node->inReplyTo = NULL;
  new_node->next      = NULL;
  new_node->crtnext   = NULL;
  new_node->subnext   = NULL;
  new_node->subtail   = NULL;

  return( new_node );
}

/************************************************************
  Reply under node, a "reply" version of getnode()
*/
MsgNode * replyNode( MsgNode * mother_node )
{
  MsgNode *new_reply;
  new_reply = getNode();
  new_reply->inReplyTo = mother_node;
  new_reply->indent = mother_node->indent + 1;
  //push new node into the sub chain starting from the mother node.
  pushNode( new_reply, &(mother_node->subnext), &(mother_node->subtail), THREAD );
  return( new_reply );
}

/************************************************************
  Push new message node to the end of the list
*/
void pushNode( MsgNode * new_node, MsgNode ** head, MsgNode ** last, int mode )
{
    if( *last == NULL ) *head = new_node;
    else {
      if( mode == BRIEF ) (*last)->next = new_node; // if the list the node being pushed in is in level 0
      else if( mode == THREAD ) (*last)->crtnext = new_node;
    }
    *last = new_node;
}

/************************************************************
  Pop message node from the END of the list
  Modes:
    BRIEF: pop from original list (connected by --next-->)
    THREAD: pop from thread chain (connected by --crtnext-->)
*/
void popNode( MsgNode ** head, MsgNode ** last, int mode )
{
  if( *head == *last ) {
    *head = NULL;
    *last = NULL;
  }

  else {
    if( mode == BRIEF ) {
      *last = seekPrev( *head, *last, BRIEF );
      (*last)->next = NULL; // if the list the node being pushed in is in level 0
    }
    else if( mode == THREAD ) {
      *last = seekPrev( *head, *last, UNDOCRT );
      (*last)->crtnext = NULL;
    }
  }
}

/************************************************************
  Read one line of text from standard input,
  store it in a string and return a pointer to the string.
*/
char * getName( void )
{
  char buffer[MAX_LINE];
  char *name;
  int length;
  int ch;
  int i;

  // prompt user for input
  printf( "Name: " );
  // skip any intial newline character
  if(( ch = getchar()) == '\n' ) {
     ch = getchar();
  }
  // read text initially into a buffer
  i=0;
  while( i < MAX_LINE && ch != '\n' && ch != EOF ) {
     buffer[i++] = ch;
     ch = getchar();
  }
  // trim of any trailing whitespace
  while( isspace( buffer[i-1] )) {
    i--;
  }
  // allocate just enough space to store the string
  length = i;
  name = (char *)malloc((length+1)*sizeof(char));
  if( name == NULL ) {
     printf("Error: could not allocate memory.\n");
     exit( 1 );
  }
  // copy text from buffer into new string
  for( i=0; i < length; i++ ) {
     name[i] = buffer[i];
  }
  name[i] = '\0'; // add end-of-string marker

  return( name );
}

/************************************************************
  Read several lines of text from standard input,
  store them in a string and return a pointer to the string.
*/
char * getText( void )
{
  char buffer[MAX_TEXT];
  char *text;
  int length;
  int ch;
  int i;

  printf("Text: ");
  ch = getchar();
  i=0;
  while(( i < MAX_TEXT )&&( ch != EOF )) {
     buffer[i++] = ch;
     ch = getchar();
     // stop when you encounter a dot on a line by itself
     if( i > 1 && ch == '\n' && buffer[i-1] == '.'
                             && buffer[i-2] == '\n' ) {
        ch = EOF;
        i  = i-2; // strip off the dot and newlines
     }
  }
  length = i;
  // allocate just enough space to store the string
  text = (char *)malloc((length+1)*sizeof(char));
  if( text == NULL ) {
     printf("Error: could not allocate memory.\n");
     exit( 1 );
  }
  // copy text from buffer to new string
  for( i=0; i<length; i++ ) {
     text[i] = buffer[i];
  }
  text[i] = '\0'; // add end-of-string marker

  return( text );
}

/************************************************************
  Get date from standard input;
  if date is invalid, ask the user to re-enter it.
*/
void getDate( Date *d )
{
  printf("Date: ");
  while( !scanDate( d ) || !dateOK( d )) {
     printf("Re-enter date in format dd/mm/yy: ");
  }
}

/************************************************************
  Scan date in the format dd/mm/yyyy
*/
int scanDate( Date *d )
{
  char s[MAX_LINE];

  fgets( s, MAX_LINE, stdin );
  if(sscanf(s,"%d/%d/%d",&d->day,&d->month,&d->year)<3){
    return FALSE;
  }
  if( d->year < 100 ) { // turn /12 into /2012, etc.
    d->year = 2000 + d->year;
  }
  return TRUE;
}

/************************************************************
  Get time from standard input;
  if time is invalid, ask the user to re-enter it.
*/
void getTime( Time *t )
{
  printf("Time: ");
  while( !scanTime( t ) || !timeOK( t )) {
     printf("Re-enter time in format hh:mm:ss: ");
  }
}

/************************************************************
  Scan time in the format hh:mm:ss
*/
int scanTime( Time *t )
{
  char s[MAX_LINE];

  fgets( s, MAX_LINE, stdin );
  return(
     sscanf(s,"%d:%d:%d",&t->hour,&t->minute,&t->second)==3);
}

/************************************************************
  Return TRUE if date is valid; FALSE otherwise.
*/
int dateOK( Date *d )
{
  if( ( ((*d).year%4 == 0) && ((*d).year%100 != 0) ) || ((*d).year%400 == 0) ) {
    if( ((*d).month == 1) ||
        ((*d).month == 3) ||
        ((*d).month == 5) ||
        ((*d).month == 7) ||
        ((*d).month == 8) ||
        ((*d).month == 10) ||
        ((*d).month == 12) ) {
        if( ((*d).day <= 31) && ((*d).day >= 1) ) return TRUE;
        else return FALSE;
    }
    else if( ((*d).month != 2) && ((*d).month < 12) ) {
        if( ((*d).day <= 30) && ((*d).day >= 1) ) return TRUE;
        else return FALSE;
    }
    else if( (*d).month < 12 ) {
        if( ((*d).day <= 29) && ((*d).day >= 1) ) return TRUE;
        else return FALSE;
    }
  }
  else {
    if( ((*d).month == 1) ||
        ((*d).month == 3) ||
        ((*d).month == 5) ||
        ((*d).month == 7) ||
        ((*d).month == 8) ||
        ((*d).month == 10) ||
        ((*d).month == 12) ) {
        if( ((*d).day <= 31) && ((*d).day >= 1) ) return TRUE;
        else return FALSE;
    }
    else if( ((*d).month != 2) && ((*d).month < 12) ) {
        if( ((*d).day <= 30) && ((*d).day >= 1) ) return TRUE;
        else return FALSE;
    }
    else if( (*d).month < 12 ) {
        if( ((*d).day <= 28) && ((*d).day >= 1) ) return TRUE;
        else return FALSE;
    }
  }
  return FALSE;
}

/************************************************************
  Return TRUE if time is valid; FALSE otherwise.
*/
int timeOK( Time *t )
{
  return(   t->hour   >= 0 && t->hour   < 24
         && t->minute >= 0 && t->minute < 60
         && t->second >= 0 && t->second < 60 );
}

// INSERT NEW FUNCTIONS, AS APPROPRIATE

/************************************************************
  Print the specified integer in two digits
  (prefixed with '0' if necessary)
*/
void printPadded( int n )
{
  if( n < 10 ) {
    putchar('0');
  }
  printf("%d",n );
}

/************************************************************
  Print date in the format dd/mm/yyyy
*/
void printDate( Date d )
{
  printPadded( d.day );
  putchar('/');
  printPadded( d.month );
  putchar('/');
  printf("%d", d.year );
}

/************************************************************
  Print time in the format hh:mm:ss 
*/
void printTime( Time t )
{
  printPadded( t.hour );
  putchar(':');
  printPadded( t.minute );
  putchar(':');
  printPadded( t.second );
}

/************************************************************
  Print the Name, followed by the first line of the Text of a certain message.
  is_indented:
    TRUE: print with indentation
    FALSE: print without indentation
*/
void printLine( MsgNode * msg, int is_indented )
{
  char *text=msg->text;
  int i=0,j=0;
  if( msg == globalCurrent ) printf( "->%2d ", msg->messageNum );
  else printf( "  %2d ", msg->messageNum );
    
  if( msg->deleted ) {
    printf("[deleted]\n");
  }
  else {
    if( is_indented ) {
      int i;
      for( i = 0; i < (msg->indent); i++ ) printf( "   " );
    }
    printf("%s: ", msg->name );
    while( isspace( text[i] )) {
    i++;
    }
    while( j < 40 && text[i+j] != '\0'
                  && text[i+j] != '\n' ) {
    putchar( text[i+j] );
    j++;
    }
    putchar('\n');
  }
}

/************************************************************
  Print the list in brief.
*/
void printBrief( MsgNode * msg )
{
  while ( msg != NULL ) {
    printLine( msg, FALSE );
    msg = msg->next;
  }
}

/************************************************************
  Print message in Full
*/
void printFull( MsgNode * msg )
{
  if( msg != NULL ) {
    printf("Message %d", msg->messageNum );
    if( msg->deleted ) {
      printf(" has been deleted.\n");
    }
    else {
      printf("\nDate: ");
      printDate( msg->date );
      printf("\nTime: ");
      printTime( msg->time );
      printf("\nName: %s\n", msg->name );
      printf("Text: %s\n", msg->text );
    }
  }
}

/************************************************************
  Using a recursive function to print threads
*/
void printThread( MsgNode * head )
{
  while( head != NULL ) {
    printLine( head, TRUE );
    printThread( head->subnext );
    head = head->crtnext;
  }
}

/************************************************************
  Print message in list
*/
void printList( MsgNode * head, MsgNode * current, int mode, int full )
{
    printf("\n");
    if( full == TRUE ) printFull( current );
    else if( mode == BRIEF ) printBrief( head );
    else printThread( head );
    printf("\n");
}

/************************************************************
  Seek previous message
  Modes:
    BRIEF
    THREAD
    UNDOCRT: like BRIEF mode, but search by --crtnext-->
*/
MsgNode * seekPrev( MsgNode * head, MsgNode * msg, int mode )
{
  if( mode == BRIEF ) {
    if( head != msg ) for( ; head->next != msg ; head = head->next );
    return( head );
  }
  
  else if( mode == THREAD ) {
    if( head == msg ) return( msg );
    if( msg->inReplyTo != NULL ) { //if msg is a thread
      head = (msg->inReplyTo)->subnext;
      if( head == msg ) return( msg->inReplyTo );
    }
    for( ; head->crtnext != msg; head = head->crtnext );
    if( head->subnext != NULL ) return( head->subtail );
    return(head);
  }

  else if( mode == UNDOCRT ) {
    if( head != msg ) for( ; head->crtnext != msg ; head = head->crtnext );
    return( head );
  }

  return( msg ); 
}

/************************************************************
  Seek next message
*/
MsgNode * seekNxt( MsgNode * head, MsgNode * msg, int mode )
{
  if( mode == BRIEF ) {
    if( msg->next != NULL ) msg = msg->next;
    return( msg );
  }
  
  else if( mode == THREAD ) {
    if( msg->subnext != NULL ) return( msg->subnext );
    else if( msg->crtnext != NULL ) return( msg->crtnext );
    else {
      head = msg->inReplyTo;
      while( head->crtnext == NULL ) {
        if( head->inReplyTo == NULL ) return( msg );
        else head = head->inReplyTo;
      }
      return( head->crtnext );
    }
  }

  return( msg );
}

/************************************************************
  Jump to message k
*/
MsgNode * jumpNode( MsgNode * head, int num )
{
  for( ; head->messageNum != num; head = head->next );
  return( head );
}

/************************************************************
  Search all posts
*/
void globalSearch( MsgNode * head, char * text, int mode )
{
  MsgNode *list = head;
  while( head != NULL ) {
    //buffer node is used for temporarily storing modified node.
    MsgNode * buffer;
    buffer = (MsgNode *)malloc(sizeof(MsgNode));
    if( buffer == NULL ) {
      printf("Error: could not allocate memory.\n");
      exit( 1 );
    }
    buffer->name = (char *)malloc((strlen(head->name)+1)*sizeof(char));
    buffer->text = (char *)malloc((strlen(head->text)+1)*sizeof(char));

    int isMatched = FALSE;
    int i = 0;
    int j;
    char *ch;
    
    ch = head->name;
    while( i != strlen( head->name ) ) {
      //search for the keyword from the first character to the last character in NAME
      for( j = 0; toupper( *(ch+i+j) ) == toupper( *(text+j) ); j++ );
        //if the whole keyword matched, changed it to upper case and store it in buffer
      if( j == strlen(text) ) {
        for( j = 0; toupper( *(ch+i+j) ) == toupper( *(text+j) ); j++ )
          buffer->name[i+j] = toupper( *(ch+i+j) );
        isMatched = TRUE;
        i = i + j - 1; //jump to the end of matched text(without '\0')
      }
      else buffer->name[i] = *(ch+i);
      
      i++; //search next char
    }
    
    i = 0;
    ch = head->text;
    while( i != strlen( head->text ) ) {
      //search for the keyword from the first character to the last character in TEXT
      for( j = 0; toupper( *(ch+i+j) ) == toupper( text[j] ); j++ );
      if( j == strlen(text) ) {
        for( j = 0; toupper( *(ch+i+j) ) == toupper( text[j] ); j++ )
          (buffer->text)[i+j] = toupper( *(ch+i+j) );
        isMatched = TRUE;
        i = i + j - 1; //jump to the end of matched text(without '\0')
      }
      else (buffer->text)[i] = *(ch+i);
      
      i++; //search next char
    }
    if( isMatched ) {
      if( head != NULL ) {
        printf("Message %d", head->messageNum );
        if( !head->deleted ) {
          printf("\nDate: ");
          printDate( head->date );
          printf("\nTime: ");
          printTime( head->time );
          printf("\nName: %s\n", buffer->name );
          printf("Text: %s\n\n", buffer->text );
        }
      }
    }
    
    if( mode == BRIEF ) head = head->next;
    if( mode == THREAD ) {
      if( head == seekNxt( list, head, THREAD ) ) head = NULL;
      else head = seekNxt( list, head, THREAD );
    }
  }
}

/************************************************************
  Delete current message
*/
int deleteMsg( MsgNode * msg )
{
  if( msg->deleted == FALSE ) {
    msg->deleted = TRUE;
    return TRUE;
  }
  return FALSE;
}

/************************************************************
  Undelete current message
*/
int undeleteMsg( MsgNode * msg )
{
  if( msg->deleted == TRUE ) {
    msg->deleted = FALSE;
    return TRUE;
  }
  return FALSE;
}


/************************************************************
  Free all memory occupied by linked list of messages
*/
void freeList( MsgNode *head )
{
  MsgNode *node;
  while( head != NULL ) {
    node = head;
    head = head->next;
    free( node->name );
    free( node->text );
    free( node );
  }
}
