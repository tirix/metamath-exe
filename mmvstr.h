/*****************************************************************************/
/*               Copyright (C) 1998, NORMAN D. MEGILL                        */
/*****************************************************************************/

/*34567890123456 (79-character line to adjust text window width) 678901234567*/
/*
mmvstr.h - VMS-BASIC variable length string library routines header
This is a collection of useful built-in string functions available in VMS BASIC.
*/

/**************************************************************************

Variable-length string handler
------------------------------

     This collection of string-handling functions emulate most of the
string functions of VMS BASIC.  The objects manipulated by these functions
are strings of a special type called 'vstring' which
have no pre-defined upper length limit but are dynamically allocated
and deallocated as needed.  To use the vstring functions within a program,
all vstrings must be initially set to the null string when declared or
before first used, for example:

        vstring string1 = "";
        vstring stringArray[] = {"","",""};

        vstring bigArray[100][10]; /- Must be initialized before using -/
        int i,j;
        for (i=0; i<100; i++)
          for (j=0; j<10; j++)
            bigArray[i][j] = ""; /- Initialize -/


     After initialization, vstrings should be assigned with the 'let(&'
function only; for example the statements

        let(&string1,"abc");
        let(&string1,string2);
        let(&string1,left(string2,3));

all assign the second argument to 'string1'.  The 'let(&' function must
not be used to initialize a vstring for the first time.

     The 'cat' function emulates the '+' concatenation operator in BASIC.
It has a variable number of arguments, and the last argument should always
be NULL.  For example,

        let(&string1,cat("abc","def",NULL));

assigns "abcdef" to 'string1'.  Warning: 0 will work instead of NULL on the
VAX but not on the Macintosh, so always use NULL.

     All other functions are generally used exactly like their BASIC
equivalents.  For example, the BASIC statement

        let string1$=left$("def",len(right$("xxx",2)))+"ghi"+string2$

is emulated in c as

        let(&string1,cat(left("def",len(right("xxx",2))),"ghi",string2,NULL));

Note that ANSII c does not allow "$" as part of an identifier
name, so the names in c have had the "$" suffix removed.

     The string arguments of the vstring functions may be either standard c
strings or vstrings (except that the first argument of the 'let(&' function
must be a vstring).  The standard c string functions may use vstrings or
vstring functions as their string arguments, as long as the vstring variable
itself (which is a char * pointer) is not modified and no attempt is made to
increase the length of a vstring.  Caution must be excercised when
assigning standard c string pointers to vstrings or the results of
vstring functions, as the memory space may be deallocated when the
'le(&t' function is next executed.  For example,

        char *stdstr; /- A standard c string pointer -/
         ...
        stdstr=left("abc",2);

will assign "ab" to 'stdstr', but this assignment will be lost when the
next 'let(&' function is executed.  To be safe, use 'strcpy':

        char stdstr1[80]; /- A fixed length standard c string -/
         ...
        strcpy(stdstr1,left("abc",2));

Here, of course, the user must ensure that the string copied to 'stdstr1'
does not exceed 79 characters in length.

     The vstring functions allocate temporary memory whenever they are called.
This temporary memory is deallocated whenever a 'let(&' assignment is
made.  The user should be aware of this when using vstring functions
outside of 'let(&' assignments; for example

        for (i=0; i<10000; i++)
          printf("%s\n",left(string1,70));

will allocate another 70 bytes or so of memory each pass through the loop.
If necessary, dummy 'let(&' assignments can be made periodically to clear
this temporary memory:

        for (i=0; i<10000; i++)
          {
          printf("%s\n",left(string1,70));
          let(&dummy,"");
          }

It should be noted that the 'linput' function assigns its target string
with 'let(&' and thus has the same effect as 'let(&'.

************************************************************************/



typedef char* vstring;
#define vstringdef(x) vstring x = ""

/* String assignment - MUST be used to assign vstrings */
void let(vstring *target,vstring source);
/* String concatenation - last argument MUST be NULL */
vstring cat(vstring string1,...);

/* Emulate BASIC linput statement; returns NULL if EOF */
/* Note that linput assigns target string with let(&target,...) */
  /*
    BASIC:  linput "what";a$
    c:      linput(NULL,"what?",&a);

    BASIC:  linput #1,a$                        (error trap on EOF)
    c:      if (!linput(file1,NULL,&a)) break;  (break on EOF)

  */
vstring linput(FILE *stream,vstring ask,vstring *target);

/* Emulation of BASIC string functions */
vstring seg(vstring sin, long p1, long p2);
vstring mid(vstring sin, long p, long l);
vstring left(vstring sin, long n);
vstring right(vstring sin, long n);
vstring edit(vstring sin, long control);
vstring space(long n);
vstring string(long n, char c);
vstring chr(long n);
vstring xlate(vstring sin, vstring control);
vstring date(void);
vstring time_(void);
vstring num(double x);
vstring num1(double x);
vstring str(double x);


long len(vstring s);
long instr(long start, vstring sin, vstring s);
long ascii_(vstring c);
double val(vstring s);

/* Emulation of PROGRESS string functions added 11/25/98 */
vstring entry(long element, vstring list);
long lookup(vstring expression, vstring list);
long numEntries(vstring list);
long entryPosition(long element, vstring list);

/* Routines may/may not be written (lowest priority):
vstring place$(vstring sout);
vstring prod$(vstring sout);
vstring quo$(vstring sout);
*/

/******* Special pupose routines for better
      memory allocation (use with caution) *******/

extern tempAllocStackTop;        /* Top of stack for tempAlloc functon */
extern startTempAllocStack;      /* Where to start freeing temporary allocation
                                    when let() is called (normally 0, except for
                                    nested vstring functions) */

/* Make string have temporary allocation to be released by next let() */
/* Warning:  after makeTempAlloc() is called, the vstring may NOT be
   assigned again with let() */
void makeTempAlloc(vstring s);    /* Make string have temporary allocation to be
                                    released by next let() */


