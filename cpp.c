#define _CRT_SECURE_NO_WARNINGS

/*
 * CPP main program.
 *
 * Edit history
 * 21-May-84	MM	"Field test" release
 * 23-May-84	MM	Some minor hacks.
 * 30-May-84	ARF	Didn't get enough memory for __DATE__
 *			Added code to read stdin if no input
 *			files are provided.
 * 29-Jun-84	MM	Added ARF's suggestions, Unixifying cpp.
 * 11-Jul-84	MM	"Official" first release (that's what I thought!)
 * 22-Jul-84	MM/ARF/SCK Fixed line number bugs, added cpp recognition
 *			of #line, fixed problems with #include.
 * 23-Jul-84	MM	More (minor) include hacking, some documentation.
 *			Also, redid cpp's #include files
 * 25-Jul-84	MM	#line filename isn't used for #include searchlist
 *			#line format is <number> <optional name>
 * 25-Jul-84	ARF/MM	Various bugs, mostly serious.  Removed homemade doprint
 * 01-Aug-84	MM	Fixed recursion bug, remove extra newlines and
 *			leading whitespace from cpp output.
 * 02-Aug-84	MM	Hacked (i.e. optimized) out blank lines and unneeded
 *			whitespace in general.  Cleaned up unget()'s.
 * 03-Aug-84	Keie	Several bug fixes from Ed Keizer, Vrije Universitet.
 *			-- corrected arg. count in -D and pre-defined
 *			macros.  Also, allow \n inside macro actual parameter
 *			lists.
 * 06-Aug-84	MM	If debugging, dump the preset vector at startup.
 * 12-Aug-84	MM/SCK	Some small changes from Sam Kendall
 * 15-Aug-84	Keie/MM	cerror, cwarn, etc. take a single string arg.
 *			cierror, etc. take a single int. arg.
 *			changed LINE_PREFIX slightly so it can be
 *			changed in the makefile.
 * 31-Aug-84	MM	USENET net.sources release.
 *  7-Sep-84	SCH/ado Lint complaints
 * 10-Sep-84	Keie	Char's can't be signed in some implementations
 * 11-Sep-84	ado	Added -C flag, pathological line number fix
 * 13-Sep-84	ado	Added -E flag (does nothing) and "-" file for stdin.
 * 14-Sep-84	MM	Allow # 123 as a synonym for #line 123
 * 19-Sep-84	MM	scanid always reads to token, make sure #line is
 *			written to a new line, even if -C switch given.
 *			Also, cpp - - reads stdin, writes stdout.
 * 03-Oct-84	ado/MM	Several changes to line counting and keepcomments
 *			stuff.  Also a rewritten control() hasher -- much
 *			simpler and no less "perfect". Note also changes
 *			in cpp3.c to fix numeric scanning.
 * 04-Oct-84	MM	Added recognition of macro formal parameters if
 *			they are the only thing in a string, per the
 *			draft standard.
 * 08-Oct-84	MM	One more attack on scannumber
 * 15-Oct-84	MM/ado	Added -N to disable predefined symbols.  Fixed
 *			linecount if COMMENT_INVISIBLE enabled.
 * 22-Oct-84	MM	Don't evaluate the #if/#ifdef argument if
 *			compilation is supressed.  This prevents
 *			unnecessary error messages in sequences such as
 *			    #ifdef FOO		-- undefined
 *			    #if FOO == 10	-- shouldn't print warning
 * 25-Oct-84	MM	Fixed bug in false ifdef supression.  On vms,
 *			#include <foo> should open foo.h -- this duplicates
 *			the behavior of Vax-C
 * 31-Oct-84	ado/MM	Parametized $ in indentifiers.  Added a better
 *			token concatenator and took out the trial
 *			concatenation code.  Also improved #ifdef code
 *			and cleaned up the macro recursion tester.
 *  2-Nov-84	MM/ado	Some bug fixes in token concatenation, also
 *			a variety of minor (uninteresting) hacks.
 *  6-Nov-84	MM	Happy Birthday.  Broke into 4 files and added
 *			#if sizeof (basic_types)
 *  9-Nov-84	MM	Added -S* for pointer type sizes
 * 13-Nov-84	MM	Split cpp1.c, added vms defaulting
 * 23-Nov-84	MM/ado	-E supresses error exit, added CPP_INCLUDE,
 *			fixed strncpy bug.
 *  3-Dec-84	ado/MM	Added OLD_PREPROCESSOR
 *  7-Dec-84	MM	Stuff in Nov 12 Draft Standard
 * 17-Dec-84	george	Fixed problems with recursive macros
 * 17-Dec-84	MM	Yet another attack on #if's (f/t)level removed.
 * 07-Jan-85	ado	Init defines before doing command line options
 *			so -Uunix works.
 */

 /*)BUILD
	 $(PROGRAM)	= cpp
	 $(FILES)	= { cpp1 cpp2 cpp3 cpp4 cpp5 cpp6 }
	 $(INCLUDE)	= { cppdef.h cpp.h }
	 $(STACK)	= 2000
	 $(TKBOPTIONS)	= {
		 STACK	= 2000
	 }
 */

#ifdef	DOCUMENTATION

title	cpp		C Pre - Processor
index			C pre - processor

synopsis
.s.nf
cpp[-options][infile[outfile]]
.s.f
description

CPP reads a C source file, expands macrosand include
files, and writes an input file for the C compiler.
If no file arguments are given, CPP reads from stdin
and writes to stdout.If one file argument is given,
it will define the input file, while two file arguments
define both inputand output files.The file name "-"
is a synonym for stdin or stdout as appropriate.

The following options are supported.Options may
be given in either case.
.lm + 16
.p - 16
- C		If set, source - file comments are written
to the output file.This allows the output of CPP to be
used as the input to a program, such as lint, that expects
commands embedded in specially - formatted comments.
.p - 16
- Dname = value	Define the name as if the programmer wrote

#define name value

at the start of the first file.If "=value" is not
given, a value of "1" will be used.

On non - unix systems, all alphabetic text will be forced
to upper - case.
.p - 16
- E		Always return "success" to the operating
system, even if errors were detected.Note that some fatal
errors, such as a missing #include file, will terminate
CPP, returning "failure" even if the - E option is given.
.p - 16
- Idirectory	Add this directory to the list of
directories searched for #include "..." and #include <...>
commands.Note that there is no space between the
"-I" and the directory string.More than one - I command
is permitted.On non - Unix systems "directory" is forced
to upper - case.
.p - 16
- N		CPP normally predefines some symbols defining
the target computer and operating system.If - N is specified,
no symbols will be predefined.If - N - N is specified, the
"always present" symbols, __LINE__, __FILE__, and __DATE__
are not defined.
.p - 16
- Stext		CPP normally assumes that the size of
the target computer's basic variable types is the same as the size
of these types of the host computer.  (This can be overridden
	when CPP is compiled, however.)  The - S option allows dynamic
	respecification of these values.  "text" is a string of
	numbers, separated by commas, that specifies correct sizes.
	The sizes must be specified in the exact order :

char short int long float double

If you specify the option as "-S*text", pointers to these
types will be specified. - S * takes one additional argument
for pointer to function(e.g. int (*)())

For example, to specify sizes appropriate for a PDP - 11,
you would write :

c s i l f d func
- S1, 2, 2, 2, 4, 8,
-S * 2, 2, 2, 2, 2, 2, 2

Note that all values must be specified.
.p - 16
- Uname		Undefine the name as if

#undef name

were given.On non - Unix systems, "name" will be forced to
upper - case.
.p - 16
- Xnumber	Enable debugging code.If no value is
given, a value of 1 will be used.  (For maintenence of
	CPP only.)
	.s.lm - 16

	Pre - Defined Variables

	When CPP begins processing, the following variables will
	have been defined(unless the - N option is specified) :
	.s
	Target computer(as appropriate) :
	.s
	pdp11, vax, M68000 m68000 m68k
	.s
	Target operating system(as appropriate) :
	.s
	rsx, rt11, vms, unix
	.s
	Target compiler(as appropriate) :
	.s
	decus, vax11c
	.s
	The implementor may add definitions to this list.
	The default definitions match the definition of the
	host computer, operating system, and C compiler.
	.s
	The following are always available unless undefined(or
		-N was specified twice) :
	.lm + 16
	.p - 12
	__FILE__	The input(or #include) file being compiled
	(as a quoted string).
	.p - 12
	__LINE__	The line number being compiled.
	.p - 12
	__DATE__	The date and time of compilation as
	a Unix ctime quoted string(the trailing newline is removed).
	Thus,
	.s
	printf("Bug at line %s,", __LINE__);
printf(" source file %s", __FILE__);
printf(" compiled on %s", __DATE__);
.s.lm - 16

Draft Proposed Ansi Standard Considerations

The current version of the Draft Proposed Standard
explicitly states that "readers are requested not to specify
or claim conformance to this draft."  Readers and users
of Decus CPP should not assume that Decus CPP conforms
to the standard, or that it will conform to the actual
C Language Standard.

When CPP is itself compiled, many features of the Draft
Proposed Standard that are incompatible with existing
preprocessors may be disabled.See the comments in CPP's
source for details.

The latest version of the Draft Proposed Standard(as reflected
	in Decus CPP) is dated November 12, 1984.

	Comments are removed from the input text.The comment
	is replaced by a single space character.The - C option
	preserves comments, writing them to the output file.

	The '$' character is considered to be a letter.This is
	a permitted extension.

	The following new features of C are processed by CPP :
.s.comment Note : significant spaces, not tabs, .br quotes #if, #elif
.br; ####_#elif expression(_#else _#if)
.br; ####'_\xNNN'             (Hexadecimal constant)
.br; ####'_\a'                (Ascii BELL)
.br; ####'_\v'                (Ascii Vertical Tab)
.br; ####_#if defined NAME    1 if defined, 0 if not
.br; ####_#if defined(NAME)  1 if defined, 0 if not
.br; ####_#if sizeof(basic type)
.br; ####unary +
.br; ####123U, 123LU          Unsigned ints and longs.
.br; ####12.3L                Long double numbers
.br; ####token_#token         Token concatenation
.br; ####_#include token      Expands to filename

The Draft Proposed Standard has extended C, adding a constant
string concatenation operator, where

"foo" "bar"

is regarded as the single string "foobar".  (This does not
	affect CPP's processing but does permit a limited form of
	macro argument substitution into strings as will be discussed.)

	The Standard Committee plans to add token concatenation
	to #define command lines.One suggested implementation
	is as follows : the sequence "Token1#Token2" is treated
	as if the programmer wrote "Token1Token2".This could
	be used as follows :

#line 123
#define ATLINE foo#__LINE__

ATLINE would be defined as foo123.

Note that "Token2" must either have the format of an
identifier or be a string of digits.Thus, the string

#define ATLINE foo#1x3

generates two tokens : "foo1" and "x3".

If the tokens T1 and T2 are concatenated into T3,
this implementation operates as follows :

1. Expand T1 if it is a macro.
2. Expand T2 if it is a macro.
3. Join the tokens, forming T3.
4. Expand T3 if it is a macro.

A macro formal parameter will be substituted into a string
or character constant if it is the only component of that
constant :

#define VECSIZE 123
#define vprint(name, size) \
	      printf("name" "[" "size" "] = {\n")
... vprint(vector, VECSIZE);

expands(effectively) to

vprint("vector[123] = {\n");

Note that this will be useful if your C compiler supports
the new string concatenation operation noted above.
As implemented here, if you write

#define string(arg) "arg"
... string("foo") ...

This implementation generates "foo", rather than the strictly
correct ""foo"" (which will probably generate an error message).
This is, strictly speaking, an error in CPPand may be removed
from future releases.

error messages

Many.CPP prints warning or error messages if you try to
use multiple - byte character constants(non - transportable)
if you #undef a symbol that was not defined, or if your
program has potentially nested comments.

author

Martin Minow

bugs

The #if expression processor uses signed integers only.
I.e, #if 0xFFFFu < 0 may be TRUE.

#endif

#include	<stdio.h>
#include	<ctype.h>
#include	"cppdef.h"
#include	"cpp.h"

	/*
	 * Commonly used global variables:
	 * line		is the current input line number.
	 * wrongline	is set in many places when the actual output
	 *		line is out of sync with the numbering, e.g,
	 *		when expanding a macro with an embedded newline.
	 *
	 * token	holds the last identifier scanned (which might
	 *		be a candidate for macro expansion).
	 * errors	is the running cpp error counter.
	 * infile	is the head of a linked list of input files (extended by
	 *		#include and macros being expanded).  infile always points
	 *		to the current file/macro.  infile->parent to the includer,
	 *		etc.  infile->fd is NULL if this input stream is a macro.
	 */
	int		line;			/* Current line number		*/
int		wrongline;		/* Force #line to compiler	*/
char		token[IDMAX + 1];	/* Current input token		*/
int		errors;			/* cpp error counter		*/
FILEINFO* infile = NULL;		/* Current input file		*/
#if DEBUG
int		debug;			/* TRUE if debugging now	*/
#endif
/*
 * This counter is incremented when a macro expansion is initiated.
 * If it exceeds a built-in value, the expansion stops -- this tests
 * for a runaway condition:
 *	#define X Y
 *	#define Y X
 *	X
 * This can be disabled by falsifying rec_recover.  (Nothing does this
 * currently: it is a hook for an eventual invocation flag.)
 */
int		recursion;		/* Infinite recursion counter	*/
int		rec_recover = TRUE;	/* Unwind recursive macros	*/

/*
 * instring is set TRUE when a string is scanned.  It modifies the
 * behavior of the "get next character" routine, causing all characters
 * to be passed to the caller (except <DEF_MAGIC>).  Note especially that
 * comments and \<newline> are not removed from the source.  (This
 * prevents cpp output lines from being arbitrarily long).
 *
 * inmacro is set by #define -- it absorbs comments and converts
 * form-feed and vertical-tab to space, but returns \<newline>
 * to the caller.  Strictly speaking, this is a bug as \<newline>
 * shouldn't delimit tokens, but we'll worry about that some other
 * time -- it is more important to prevent infinitly long output lines.
 *
 * instring and inmarcor are parameters to the get() routine which
 * were made global for speed.
 */
int		instring = FALSE;	/* TRUE if scanning string	*/
int		inmacro = FALSE;	/* TRUE if #defining a macro	*/

/*
 * work[] and workp are used to store one piece of text in a temporay
 * buffer.  To initialize storage, set workp = work.  To store one
 * character, call save(c);  (This will fatally exit if there isn't
 * room.)  To terminate the string, call save(EOS).  Note that
 * the work buffer is used by several subroutines -- be sure your
 * data won't be overwritten.  The extra byte in the allocation is
 * needed for string formal replacement.
 */
char		work[NWORK + 1];	/* Work buffer			*/
char* workp;			/* Work buffer pointer		*/

/*
 * keepcomments is set TRUE by the -C option.  If TRUE, comments
 * are written directly to the output stream.  This is needed if
 * the output from cpp is to be passed to lint (which uses commands
 * embedded in comments).  cflag contains the permanent state of the
 * -C flag.  keepcomments is always falsified when processing #control
 * commands and when compilation is supressed by a false #if
 *
 * If eflag is set, CPP returns "success" even if non-fatal errors
 * were detected.
 *
 * If nflag is non-zero, no symbols are predefined except __LINE__.
 * __FILE__, and __DATE__.  If nflag > 1, absolutely no symbols
 * are predefined.
 */
int		keepcomments = FALSE;	/* Write out comments flag	*/
int		cflag = FALSE;		/* -C option (keep comments)	*/
int		eflag = FALSE;		/* -E option (never fail)	*/
int		nflag = 0;		/* -N option (no predefines)	*/

/*
 * ifstack[] holds information about nested #if's.  It is always
 * accessed via *ifptr.  The information is as follows:
 *	WAS_COMPILING	state of compiling flag at outer level.
 *	ELSE_SEEN	set TRUE when #else seen to prevent 2nd #else.
 *	TRUE_SEEN	set TRUE when #if or #elif succeeds
 * ifstack[0] holds the compiling flag.  It is TRUE if compilation
 * is currently enabled.  Note that this must be initialized TRUE.
 */
char		ifstack[BLK_NEST] = { TRUE };	/* #if information	*/
char* ifptr = ifstack;		/* -> current ifstack[] */

/*
 * incdir[] stores the -i directories (and the system-specific
 * #include <...> directories.
 */
char* incdir[NINCLUDE];		/* -i directories		*/
char** incend = incdir;		/* -> free space in incdir[]	*/

/*
 * This is the table used to predefine target machine and operating
 * system designators.  It may need hacking for specific circumstances.
 * Note: it is not clear that this is part of the Ansi Standard.
 * The -N option supresses preset definitions.
 */
char* preset[] = {			/* names defined at cpp start	*/
#ifdef	MACHINE
	MACHINE,
#endif
#ifdef	SYSTEM
	SYSTEM,
#endif
#ifdef	COMPILER
	COMPILER,
#endif
#if	DEBUG
	"decus_cpp",			/* Ourselves!			*/
#endif
	NULL				/* Must be last			*/
};

/*
 * The value of these predefined symbols must be recomputed whenever
 * they are evaluated.  The order must not be changed.
 */
char* magic[] = {			/* Note: order is important	*/
	"__LINE__",
	"__FILE__",
	NULL				/* Must be last			*/
};

main(argc, argv)
int		argc;
char* argv[];
{
	register int	i;

#if HOST == SYS_VMS
	argc = getredirection(argc, argv);	/* vms >file and <file	*/
#endif
	initdefines();				/* O.S. specific def's	*/
	i = dooptions(argc, argv);		/* Command line -flags	*/
	switch (i) {
	case 3:
		/*
		 * Get output file, "-" means use stdout.
		 */
		if (!streq(argv[2], "-")) {
#if HOST == SYS_VMS
			/*
			 * On vms, reopen stdout with "vanilla rms" attributes.
			 */
			if ((i = creat(argv[2], 0, "rat=cr", "rfm=var")) == -1
				|| dup2(i, fileno(stdout)) == -1) {
#else
			if (freopen(argv[2], "w", stdout) == NULL) {
#endif
				perror(argv[2]);
				cerror("Can't open output file \"%s\"", argv[2]);
				exit(IO_ERROR);
			}
			}				/* Continue by opening input	*/
	case 2:				/* One file -> stdin		*/
		/*
		 * Open input file, "-" means use stdin.
		 */
		if (!streq(argv[1], "-")) {
			if (freopen(argv[1], "r", stdin) == NULL) {
				perror(argv[1]);
				cerror("Can't open input file \"%s\"", argv[1]);
				exit(IO_ERROR);
			}
			strcpy(work, argv[1]);	/* Remember input filename	*/
			break;
		}				/* Else, just get stdin		*/
	case 0:				/* No args?			*/
	case 1:				/* No files, stdin -> stdout	*/
#if HOST == SYS_UNIX
		work[0] = EOS;		/* Unix can't find stdin name	*/
#else
		fgetname(stdin, work);	/* Vax-11C, Decus C know name	*/
#endif
		break;

	default:
		exit(IO_ERROR);		/* Can't happen			*/
		}
	setincdirs();			/* Setup -I include directories	*/
	addfile(stdin, work);		/* "open" main input file	*/
#if DEBUG
	if (debug > 0)
		dumpdef("preset #define symbols");
#endif
	cppmain();			/* Process main file		*/
	if ((i = (ifptr - &ifstack[0])) != 0) {
#if OLD_PREPROCESSOR
		ciwarn("Inside #ifdef block at end of input, depth = %d", i);
#else
		cierror("Inside #ifdef block at end of input, depth = %d", i);
#endif
	}
	fclose(stdout);
	if (errors > 0) {
		fprintf(stderr, (errors == 1)
			? "%d error in preprocessor\n"
			: "%d errors in preprocessor\n", errors);
		if (!eflag)
			exit(IO_ERROR);
	}
	exit(IO_NORMAL);		/* No errors or -E option set	*/
	}

FILE_LOCAL
cppmain()
/*
 * Main process for cpp -- copies tokens from the current input
 * stream (main file, include file, or a macro) to the output
 * file.
 */
{
	register int		c;		/* Current character	*/
	register int		counter;	/* newlines and spaces	*/
	extern int		output();	/* Output one character	*/

	/*
	 * Explicitly output a #line at the start of cpp output so
	 * that lint (etc.) knows the name of the original source
	 * file.  If we don't do this explicitly, we may get
	 * the name of the first #include file instead.
	 * We also seem to need a blank line following that first #line.
	 */
	sharp();
	putchar('\n');
	/*
	 * This loop is started "from the top" at the beginning of each line
	 * wrongline is set TRUE in many places if it is necessary to write
	 * a #line record.  (But we don't write them when expanding macros.)
	 *
	 * The counter variable has two different uses:  at
	 * the start of a line, it counts the number of blank lines that
	 * have been skipped over.  These are then either output via
	 * #line records or by outputting explicit blank lines.
	 * When expanding tokens within a line, the counter remembers
	 * whether a blank/tab has been output.  These are dropped
	 * at the end of the line, and replaced by a single blank
	 * within lines.
	 */
	for (;;) {
		counter = 0;			/* Count empty lines	*/
		for (;;) {				/* For each line, ...	*/
			while (type[(c = get())] == SPA) /* Skip leading blanks	*/
				;				/* in this line.	*/
			if (c == '\n')			/* If line's all blank,	*/
				++counter;			/* Do nothing now	*/
			else if (c == '#') {		/* Is 1st non-space '#'	*/
				keepcomments = FALSE;	/* Don't pass comments	*/
				counter = control(counter);	/* Yes, do a #command	*/
				keepcomments = (cflag && compiling);
			}
			else if (c == EOF_CHAR)		/* At end of file?	*/
				break;
			else if (!compiling) {		/* #ifdef false?	*/
				skipnl();			/* Skip to newline	*/
				counter++;			/* Count it, too.	*/
			}
			else {
				break;			/* Actual token		*/
			}
		}
		if (c == EOF_CHAR)			/* Exit process at	*/
			break;				/* End of file		*/
			/*
			 * If the loop didn't terminate because of end of file, we
			 * know there is a token to compile.  First, clean up after
			 * absorbing newlines.  counter has the number we skipped.
			 */
		if ((wrongline && infile->fp != NULL) || counter > 4)
			sharp();			/* Output # line number	*/
		else {				/* If just a few, stuff	*/
			while (--counter >= 0)		/* them out ourselves	*/
				putchar('\n');
		}
		/*
		 * Process each token on this line.
		 */
		unget();				/* Reread the char.	*/
		for (;;) {				/* For the whole line,	*/
			do {				/* Token concat. loop	*/
				for (counter = 0; (type[(c = get())] == SPA);) {
#if COMMENT_INVISIBLE
					if (c != COM_SEP)
						counter++;
#else
					counter++;		/* Skip over blanks	*/
#endif
				}
				if (c == EOF_CHAR || c == '\n')
					goto end_line;		/* Exit line loop	*/
				else if (counter > 0)	/* If we got any spaces	*/
					putchar(' ');		/* Output one space	*/
				c = macroid(c);		/* Grab the token	*/
			} while (type[c] == LET && catenate());
			if (c == EOF_CHAR || c == '\n')	/* From macro exp error	*/
				goto end_line;		/* Exit line loop	*/
			switch (type[c]) {
			case LET:
				fputs(token, stdout);	/* Quite ordinary token	*/
				break;


			case DIG:			/* Output a number	*/
			case DOT:			/* Dot may begin floats	*/
				scannumber(c, output);
				break;

			case QUO:			/* char or string const	*/
				scanstring(c, output);	/* Copy it to output	*/
				break;

			default:			/* Some other character	*/
				cput(c);			/* Just output it	*/
				break;
			}				/* Switch ends		*/
		}					/* Line for loop	*/
	end_line:   if (c == '\n') {			/* Compiling at EOL?	*/
		putchar('\n');			/* Output newline, if	*/
		if (infile->fp == NULL)		/* Expanding a macro,	*/
			wrongline = TRUE;		/* Output # line later	*/
	}
	}					/* Continue until EOF	*/
}

output(c)
int		c;
/*
 * Output one character to stdout -- output() is passed as an
 * argument to scanstring()
 */
{
#if COMMENT_INVISIBLE
	if (c != TOK_SEP && c != COM_SEP)
#else
	if (c != TOK_SEP)
#endif
		putchar(c);
}

static char* sharpfilename = NULL;

FILE_LOCAL
sharp()
/*
 * Output a line number line.
 */
{
	register char* name;

	if (keepcomments)			/* Make sure # comes on	*/
		putchar('\n');			/* a fresh, new line.	*/
	printf("//#%s %d", LINE_PREFIX, line);
	if (infile->fp != NULL) {
		name = (infile->progname != NULL)
			? infile->progname : infile->filename;
		if (sharpfilename == NULL
			|| sharpfilename != NULL && !streq(name, sharpfilename)) {
			if (sharpfilename != NULL)
				free(sharpfilename);
			sharpfilename = savestring(name);
			printf(" \"%s\"", name);
		}
	}
	putchar('\n');
	wrongline = FALSE;
}

/*
 *				C P P 2 . C
 *
 *			   Process #control lines
 *
 * Edit history
 * 13-Nov-84	MM	Split from cpp1.c
 */

#include	<stdio.h>
#include	<ctype.h>
#include	"cppdef.h"
#include	"cpp.h"
#if HOST == SYS_VMS
 /*
  * Include the rms stuff.  (We can't just include rms.h as it uses the
  * VaxC-specific library include syntax that Decus CPP doesn't support.
  * By including things by hand, we can CPP ourself.)
  */
#include	<nam.h>
#include	<fab.h>
#include	<rab.h>
#include	<rmsdef.h>
#endif

  /*
   * Generate (by hand-inspection) a set of unique values for each control
   * operator.  Note that this is not guaranteed to work for non-Ascii
   * machines.  CPP won't compile if there are hash conflicts.
   */

#define	L_assert	('a' + ('s' << 1))
#define	L_define	('d' + ('f' << 1))
#define	L_elif		('e' + ('i' << 1))
#define	L_else		('e' + ('s' << 1))
#define	L_endif		('e' + ('d' << 1))
   // Needs to be checked manually, conflicts with #import:
   // #define L_error		('e' + ('r' << 1))
#define	L_if		('i' + (EOS << 1))
#define	L_ifdef		('i' + ('d' << 1))
#define	L_ifndef	('i' + ('n' << 1))
#define	L_include	('i' + ('c' << 1))
#define L_import	('i' + ('p' << 1))
#define	L_line		('l' + ('n' << 1))
#define	L_nogood	(EOS + (EOS << 1))	/* To catch #i		*/
#define	L_pragma	('p' + ('a' << 1))
#define L_undef		('u' + ('d' << 1))
#define L_warning	('w' + ('r' << 1))
#if DEBUG
#define	L_debug		('d' + ('b' << 1))	/* #debug		*/
#define	L_nodebug	('n' + ('d' << 1))	/* #nodebug		*/
#endif

int
control(counter)
int		counter;	/* Pending newline counter		*/
/*
 * Process #control lines.  Simple commands are processed inline,
 * while complex commands have their own subroutines.
 *
 * The counter is used to force out a newline before #line, and
 * #pragma commands.  This prevents these commands from ending up at
 * the end of the previous line if cpp is invoked with the -C option.
 */
{
	register int		c;
	register char* tp;
	register int		hash;
	char* ep;

	c = skipws();
	if (c == '\n' || c == EOF_CHAR)
		return (counter + 1);
	if (!isdigit(c))
		scanid(c);			/* Get #word to token[]		*/
	else {
		unget();			/* Hack -- allow #123 as a	*/
		strcpy(token, "line");	/* synonym for #line 123	*/
	}
	hash = (token[1] == EOS) ? L_nogood : (token[0] + (token[2] << 1));
	switch (hash) {
	case L_assert:	tp = "assert";		break;
	case L_define:	tp = "define";		break;
	case L_elif:	tp = "elif";		break;
	case L_else:	tp = "else";		break;
	case L_endif:	tp = "endif";		break;
		//case L_error:	tp = "error";		break;
	case L_if:		tp = "if";			break;
	case L_ifdef:	tp = "ifdef";		break;
	case L_ifndef:	tp = "ifndef";		break;
	case L_include:	tp = "include";		break;
	case L_import:	tp = "import";		break;
	case L_line:	tp = "line";		break;
	case L_pragma:	tp = "pragma";		break;
	case L_undef:	tp = "undef";		break;
	case L_warning:	tp = "warning";		break;
#if DEBUG
	case L_debug:	tp = "debug";		break;
	case L_nodebug:	tp = "nodebug";		break;
#endif
	default:	hash = L_nogood;
	case L_nogood:	tp = "";		break;
	}
	if (!streq(tp, token))
		hash = L_nogood;
	/*
	 * hash is set to a unique value corresponding to the
	 * control keyword (or L_nogood if we think it's nonsense).
	 */
	if (infile->fp == NULL)
		cwarn("Control line \"%s\" within macro expansion", token);
	if (!compiling) {			/* Not compiling now	*/
		switch (hash) {
		case L_if:				/* These can't turn	*/
		case L_ifdef:			/*  compilation on, but	*/
		case L_ifndef:			/*   we must nest #if's	*/
			if (++ifptr >= &ifstack[BLK_NEST])
				goto if_nest_err;
			*ifptr = 0;			/* !WAS_COMPILING	*/
		case L_line:			/* Many			*/
		/*
		 * Are pragma's always processed?
		 */
		case L_pragma:			/*  options		*/
		case L_include:			/*   are uninteresting	*/
		case L_import:			/* ??? hopefully this fixes something but fucpps the poetry. */
		case L_define:			/*    if we		*/
		case L_undef:			/*     aren't		*/
		case L_assert:			/*      compiling.	*/
		dump_line:	skipnl();			/* Ignore rest of line	*/
			return (counter + 1);
		}
	}
	/*
	 * Make sure that #line and #pragma are output on a fresh line.
	 */
	if (counter > 0 && (hash == L_line || hash == L_pragma)) {
		putchar('\n');
		counter--;
	}
	switch (hash) {
	case L_line:
		/*
		 * Parse the line to update the line number and "progname"
		 * field and line number for the next input line.
		 * Set wrongline to force it out later.
		 */
		c = skipws();
		workp = work;			/* Save name in work	*/
		while (c != '\n' && c != EOF_CHAR) {
			save(c);
			c = get();
		}
		unget();
		save(EOS);
		/*
		 * Split #line argument into <line-number> and <name>
		 * We subtract 1 as we want the number of the next line.
		 */
		line = atoi(work) - 1;		/* Reset line number	*/
		for (tp = work; isdigit(*tp) || type[*tp] == SPA; tp++)
			;				/* Skip over digits	*/
		if (*tp != EOS) {			/* Got a filename, so:	*/
			if (*tp == '"' && (ep = strrchr(tp + 1, '"')) != NULL) {
				tp++;			/* Skip over left quote	*/
				*ep = EOS;			/* And ignore right one	*/
			}
			if (infile->progname != NULL)	/* Give up the old name	*/
				free(infile->progname);	/* if it's allocated.	*/
			infile->progname = savestring(tp);
		}
		wrongline = TRUE;			/* Force output later	*/
		break;

	case L_include:
		doinclude(FALSE);
		break;

	case L_import:
		doinclude(TRUE);
		break;

	case L_define:
		dodefine();
		break;

	case L_undef:
		doundef();
		break;

	case L_else:
		if (ifptr == &ifstack[0])
			goto nest_err;
		else if ((*ifptr & ELSE_SEEN) != 0)
			goto else_seen_err;
		*ifptr |= ELSE_SEEN;
		if ((*ifptr & WAS_COMPILING) != 0) {
			if (compiling || (*ifptr & TRUE_SEEN) != 0)
				compiling = FALSE;
			else {
				compiling = TRUE;
			}
		}
		break;

	case L_elif:
		if (ifptr == &ifstack[0])
			goto nest_err;
		else if ((*ifptr & ELSE_SEEN) != 0) {
		else_seen_err:	cerror("#%s may not follow #else", token);
			goto dump_line;
		}
		if ((*ifptr & (WAS_COMPILING | TRUE_SEEN)) != WAS_COMPILING) {
			compiling = FALSE;		/* Done compiling stuff	*/
			goto dump_line;			/* Skip this clause	*/
		}
		doif(L_if);
		break;
		/* Handled as part of L_nogood/default
	case L_error:
		cerror("#\"%s\" (TODO: Better explanation)", token);
		break;
		*/
	case L_if:
	case L_ifdef:
	case L_ifndef:
		if (++ifptr >= &ifstack[BLK_NEST])
			if_nest_err : cfatal("Too many nested #%s statements", token);
		*ifptr = WAS_COMPILING;
		doif(hash);
		break;

	case L_endif:
		if (ifptr == &ifstack[0]) {
		nest_err:	cerror("#%s must be in an #if", token);
			goto dump_line;
		}
		if (!compiling && (*ifptr & WAS_COMPILING) != 0)
			wrongline = TRUE;
		compiling = ((*ifptr & WAS_COMPILING) != 0);
		--ifptr;
		break;

	case L_assert:
		if (eval() == 0)
			cerror("Preprocessor assertion failure", NULLST);
		break;

	case L_pragma:
		/*
		 * #pragma is provided to pass "options" to later
		 * passes of the compiler.  cpp doesn't have any yet.
		 */
		printf("#pragma ");
		while ((c = get()) != '\n' && c != EOF_CHAR)
			cput(c);
		unget();
		break;

	case L_warning:
		cwarn("#warning encountered", NULLST);
		break;

#if DEBUG
	case L_debug:
		if (debug == 0)
			dumpdef("debug set on");
		debug++;
		break;

	case L_nodebug:
		debug--;
		break;
#endif

	default:
		if (streq(token, "error")) {
			cerror("#error encountered");
			break;
		}
		/*
		 * Undefined #control keyword.
		 * Note: the correct behavior may be to warn and
		 * pass the line to a subsequent compiler pass.
		 * This would allow #asm or similar extensions.
		 */
		cerror("Illegal # command \"%s\"", token);
		break;
	}
	if (hash != L_include && hash != L_import) {
#if OLD_PREPROCESSOR
		/*
		 * Ignore the rest of the #control line so you can write
		 *		#if	foo
		 *		#endif	foo
		 */
		goto dump_line;			/* Take common exit	*/
#else
		if (skipws() != '\n') {
			cwarn("Unexpected text in #control line ignored", NULLST);
			skipnl();
		}
#endif
	}
	return (counter + 1);
}

FILE_LOCAL
doif(hash)
int		hash;
/*
 * Process an #if, #ifdef, or #ifndef.  The latter two are straightforward,
 * while #if needs a subroutine of its own to evaluate the expression.
 *
 * doif() is called only if compiling is TRUE.  If false, compilation
 * is always supressed, so we don't need to evaluate anything.  This
 * supresses unnecessary warnings.
 */
{
	register int		c;
	register int		found;

	if ((c = skipws()) == '\n' || c == EOF_CHAR) {
		unget();
		goto badif;
	}
	if (hash == L_if) {
		unget();
		found = (eval() != 0);	/* Evaluate expr, != 0 is  TRUE	*/
		hash = L_ifdef;		/* #if is now like #ifdef	*/
	}
	else {
		if (type[c] != LET)		/* Next non-blank isn't letter	*/
			goto badif;		/* ... is an error		*/
		found = (lookid(c) != NULL); /* Look for it in symbol table	*/
	}
	if (found == (hash == L_ifdef)) {
		compiling = TRUE;
		*ifptr |= TRUE_SEEN;
	}
	else {
		compiling = FALSE;
	}
	return;

badif:	cerror("#if, #ifdef, or #ifndef without an argument", NULLST);
#if !OLD_PREPROCESSOR
	skipnl();				/* Prevent an extra	*/
	unget();				/* Error message	*/
#endif
	return;
}

FILE_LOCAL
doinclude(isimport)
int		isimport;		/* TRUE if #import (otherwise an #include) */
/*
 * Process the #include or (Objective-C-style) #import control line.
 * There are three variations:
 *	#include "file"		search somewhere relative to the
 *				current source file, if not found,
 *				treat as #include <file>.
 *	#include <file>		Search in an implementation-dependent
 *				list of places.
 *	#include token		Expand the token, it must be one of
 *				"file" or <file>, process as such.
 * #import lines (unlike #includes) ignore additional #imports of the same file.
 * Note: the November 12 draft forbids '>' in the #include <file> format.
 * This restriction is unnecessary and not implemented.
 */
{
	register int		c;
	register int		delim;
#if HOST == SYS_VMS
	char			def_filename[NAM$C_MAXRSS + 1];
#endif

	delim = macroid(skipws());
	if (delim != '<' && delim != '"')
		goto incerr;
	if (delim == '<')
		delim = '>';
	workp = work;
	instring = TRUE;		/* Accept all characters	*/
#ifdef CONTROL_COMMENTS_NOT_ALLOWED
	while ((c = get()) != '\n' && c != EOF_CHAR)
		save(c);			/* Put it away.			*/
	unget();			/* Force nl after includee	*/
	/*
	 * The draft is unclear if the following should be done.
	 */
	while (--workp >= work && *workp == ' ')
		;				/* Trim blanks from filename	*/
	if (*workp != delim)
		goto incerr;
#else
	while ((c = get()) != delim && c != EOF_CHAR)
		save(c);
#endif
	* workp = EOS;			/* Terminate filename		*/
	instring = FALSE;
#if HOST == SYS_VMS
	/*
	 * Assume the default .h filetype.
	 */
	if (!vmsparse(work, ".H", def_filename)) {
		perror(work);		/* Oops.			*/
		goto incerr;
	}
	else if (openinclude(def_filename, (delim == '"'), isimport))
		return;
#else
	if (openinclude(work, (delim == '"'), isimport))
		return;
#endif
	/*
	 * No sense continuing if #include (or #import) file isn't there.
	 */
	cfatal("Cannot open %s file \"%s\"", work, isimport ? "import" : "include");

incerr:	cerror(isimport ? "#import syntax error" : "#include syntax error", NULLST);
	return;
}

static const char** imported = NULL;
static int nimported = 0;
static int importedcap = 0;

FILE_LOCAL int
openinclude(filename, searchlocal, isimport)
char* filename;		/* Input file name		*/
int		searchlocal;		/* TRUE if #include "file"	*/
int		isimport;			/* TRUE if #import */
/*
 * Actually open an include file.  This routine is only called from
 * doinclude() above, but was written as a separate subroutine for
 * programmer convenience.  It searches the list of directories
 * and actually opens the file, linking it into the list of
 * active files.  Returns TRUE if the file was opened, FALSE
 * if openinclude() fails.  No error message is printed.
 *
 * This function has been extended to support #import behaviour as well,
 * which works the same except short-circuits (returning TRUE) if the
 * file has already been #imported. This comparison is done based on
 * name alone, not based on the fully-resolved file path. (Not sure what
 * behaviour exactly, if any, is standardised.)
 */
{
	register char** incptr;
#if HOST == SYS_VMS
#if NWORK < (NAM$C_MAXRSS + 1)
	<< error, NWORK isn't greater than NAM$C_MAXRSS >>
#endif
#endif
		char			tmpname[NWORK];	/* Filename work area	*/

		/* For #import, behaviour is the same as #include, except it doesn't do anything if
		 * if that file's already been imported. So we just check and update our list of
		 * already-imported files before proceeding, returning early if we can.
		 */
	if (isimport) {
		/* Check for matching imports first. */
		int impi;
		for (impi = 0; impi < nimported; impi++) {
			const char* imps = imported[impi];
			if (strcmp(imps, filename) == 0) {
				return TRUE; /* Return early, no further work necessary. */
			}
		}
		/* Otherwise add ourselves, resizing/creating the list if necessary. */
		if (nimported >= importedcap) {
			if (imported == NULL) {
				imported = calloc(100, sizeof(const char*));
				importedcap = 100;
			}
			else {
				imported = realloc(imported, importedcap + 100);
				importedcap += 100;
			}
		}
		if (importedcap == NULL) {
			cfatal("Out of memory?", NULLST);
			return FALSE;
		}
		imported[nimported] = _strdup(filename);
		nimported++;
	}

	if (searchlocal) {
		/*
		 * Look in local directory first
		 */
#if HOST == SYS_UNIX
		 /*
		  * Try to open filename relative to the directory of the current
		  * source file (as opposed to the current directory). (ARF, SCK).
		  */
		if (filename[0] != '/'
			&& hasdirectory(infile->filename, tmpname))
			strcat(tmpname, filename);
		else {
			strcpy(tmpname, filename);
		}
#else
		if (!hasdirectory(filename, tmpname)
			&& hasdirectory(infile->filename, tmpname))
			strcat(tmpname, filename);
		else {
			strcpy(tmpname, filename);
		}
#endif
		if (openfile(tmpname))
			return (TRUE);
	}
	/*
	 * Look in any directories specified by -I command line
	 * arguments, then in the builtin search list.
	 */
	for (incptr = incdir; incptr < incend; incptr++) {
		if (strlen(*incptr) + strlen(filename) >= (NWORK - 1))
			cfatal("Filename work buffer overflow", NULLST);
		else {
#if HOST == SYS_UNIX
			if (filename[0] == '/')
				strcpy(tmpname, filename);
			else {
				sprintf(tmpname, "%s/%s", *incptr, filename);
			}
#else
			if (!hasdirectory(filename, tmpname))
				sprintf(tmpname, "%s%s", *incptr, filename);
#endif
			if (openfile(tmpname))
				return (TRUE);
		}
	}
	return (FALSE);
}

FILE_LOCAL int
hasdirectory(source, result)
char* source;	/* Directory to examine			*/
char* result;	/* Put directory stuff here		*/
/*
 * If a device or directory is found in the source filename string, the
 * node/device/directory part of the string is copied to result and
 * hasdirectory returns TRUE.  Else, nothing is copied and it returns FALSE.
 */
{
#if HOST == SYS_UNIX
	register char* tp;

	if ((tp = strrchr(source, '/')) == NULL)
		return (FALSE);
	else {
		strncpy(result, source, tp - source + 1);
		result[tp - source + 1] = EOS;
		return (TRUE);
	}
#else
#if HOST == SYS_VMS
	if (vmsparse(source, NULLST, result)
		&& result[0] != EOS)
		return (TRUE);
	else {
		return (FALSE);
	}
#else
	/*
	 * Random DEC operating system (RSX, RT11, RSTS/E)
	 */
	register char* tp;

	if ((tp = strrchr(source, ']')) == NULL
		&& (tp = strrchr(source, ':')) == NULL)
		return (FALSE);
	else {
		strncpy(result, source, tp - source + 1);
		result[tp - source + 1] = EOS;
		return (TRUE);
	}
#endif
#endif
}

#if HOST == SYS_VMS

/*
 * EXP_DEV is set if a device was specified, EXP_DIR if a directory
 * is specified.  (Both set indicate a file-logical, but EXP_DEV
 * would be set by itself if you are reading, say, SYS$INPUT:)
 */
#define DEVDIR (NAM$M_EXP_DEV | NAM$M_EXP_DIR)

FILE_LOCAL int
vmsparse(source, defstring, result)
char* source;
char* defstring;	/* non-NULL -> default string.		*/
char* result;	/* Size is at least NAM$C_MAXRSS + 1	*/
/*
 * Parse the source string, applying the default (properly, using
 * the system parse routine), storing it in result.
 * TRUE if it parsed, FALSE on error.
 *
 * If defstring is NULL, there are no defaults and result gets
 * (just) the node::[directory] part of the string (possibly "")
 */
{
	struct FAB	fab = cc$rms_fab;	/* File access block	*/
	struct NAM	nam = cc$rms_nam;	/* File name block	*/
	char		fullname[NAM$C_MAXRSS + 1];
	register char* rp;			/* Result pointer	*/

	fab.fab$l_nam = &nam;			/* fab -> nam		*/
	fab.fab$l_fna = source;			/* Source filename	*/
	fab.fab$b_fns = strlen(source);		/* Size of source	*/
	fab.fab$l_dna = defstring;		/* Default string	*/
	if (defstring != NULLST)
		fab.fab$b_dns = strlen(defstring);	/* Size of default	*/
	nam.nam$l_esa = fullname;		/* Expanded filename	*/
	nam.nam$b_ess = NAM$C_MAXRSS;		/* Expanded name size	*/
	if (sys$parse(&fab) == RMS$_NORMAL) {	/* Parse away		*/
		fullname[nam.nam$b_esl] = EOS;	/* Terminate string	*/
		result[0] = EOS;			/* Just in case		*/
		rp = &result[0];
		/*
		 * Remove stuff added implicitly, accepting node names and
		 * dev:[directory] strings (but not process-permanent files).
		 */
		if ((nam.nam$l_fnb & NAM$M_PPF) == 0) {
			if ((nam.nam$l_fnb & NAM$M_NODE) != 0) {
				strncpy(result, nam.nam$l_node, nam.nam$b_node);
				rp += nam.nam$b_node;
				*rp = EOS;
			}
			if ((nam.nam$l_fnb & DEVDIR) == DEVDIR) {
				strncpy(rp, nam.nam$l_dev, nam.nam$b_dev + nam.nam$b_dir);
				rp += nam.nam$b_dev + nam.nam$b_dir;
				*rp = EOS;
			}
		}
		if (defstring != NULLST) {
			strncpy(rp, nam.nam$l_name, nam.nam$b_name + nam.nam$b_type);
			rp += nam.nam$b_name + nam.nam$b_type;
			*rp = EOS;
			if ((nam.nam$l_fnb & NAM$M_EXP_VER) != 0) {
				strncpy(rp, nam.nam$l_ver, nam.nam$b_ver);
				rp[nam.nam$b_ver] = EOS;
			}
		}
		return (TRUE);
	}
	return (FALSE);
}
#endif

/*
 *				C P P 3 . C
 *
 *		    File open and command line options
 *
 * Edit history
 * 13-Nov-84	MM	Split from cpp1.c
 */

#include	<stdio.h>
#include	<ctype.h>
#include	"cppdef.h"
#include	"cpp.h"
#if DEBUG && (HOST == SYS_VMS || HOST == SYS_UNIX)
#include	<signal.h>
extern int	abort();		/* For debugging		*/
#endif

int
openfile(filename)
char* filename;
/*
 * Open a file, add it to the linked list of open files.
 * This is called only from openfile() above.
 */
{
	register FILE* fp;

	if ((fp = fopen(filename, "r")) == NULL) {
#if DEBUG
		perror(filename);
#endif
		return (FALSE);
	}
#if DEBUG
	if (debug)
		fprintf(stderr, "Reading from \"%s\"\n", filename);
#endif
	addfile(fp, filename);
	return (TRUE);
}

addfile(fp, filename)
FILE* fp;			/* Open file pointer		*/
char* filename;		/* Name of the file		*/
/*
 * Initialize tables for this open file.  This is called from openfile()
 * above (for #include files), and from the entry to cpp to open the main
 * input file.  It calls a common routine, getfile() to build the FILEINFO
 * structure which is used to read characters.  (getfile() is also called
 * to setup a macro replacement.)
 */
{
	register FILEINFO* file;
	extern FILEINFO* getfile();

	file = getfile(NBUFF, filename);
	file->fp = fp;			/* Better remember FILE *	*/
	file->buffer[0] = EOS;		/* Initialize for first read	*/
	line = 1;			/* Working on line 1 now	*/
	wrongline = TRUE;		/* Force out initial #line	*/
}

setincdirs()
/*
 * Append system-specific directories to the include directory list.
 * Called only when cpp is started.
 */
{

#ifdef	CPP_INCLUDE
	* incend++ = CPP_INCLUDE;
#define	IS_INCLUDE	1
#else
#define	IS_INCLUDE	0
#endif

#if HOST == SYS_UNIX
	* incend++ = "/usr/include";
#define	MAXINCLUDE	(NINCLUDE - 1 - IS_INCLUDE)
#endif

#if HOST == SYS_VMS
	extern char* getenv();

	if (getenv("C$LIBRARY") != NULL)
		*incend++ = "C$LIBRARY:";
	*incend++ = "SYS$LIBRARY:";
#define	MAXINCLUDE	(NINCLUDE - 2 - IS_INCLUDE)
#endif

#if HOST == SYS_RSX
	extern int	$$rsts;			/* TRUE on RSTS/E	*/
	extern int	$$pos;			/* TRUE on PRO-350 P/OS	*/
	extern int	$$vms;			/* TRUE on VMS compat.	*/

	if ($$pos) {				/* P/OS?		*/
		*incend++ = "SY:[ZZDECUSC]";	/* C #includes		*/
		*incend++ = "LB:[1,5]";		/* RSX library		*/
	}
	else if ($$rsts) {			/* RSTS/E?		*/
		*incend++ = "SY:@";			/* User-defined account	*/
		*incend++ = "C:";			/* Decus-C library	*/
		*incend++ = "LB:[1,1]";		/* RSX library		*/
	}
	else if ($$vms) {			/* VMS compatibility?	*/
		*incend++ = "C:";
	}
	else {					/* Plain old RSX/IAS	*/
		*incend++ = "LB:[1,1]";
	}
#define	MAXINCLUDE	(NINCLUDE - 3 - IS_INCLUDE)
#endif

#if HOST == SYS_RT11
	extern int	$$rsts;			/* RSTS/E emulation?	*/

	if ($$rsts)
		*incend++ = "SY:@";			/* User-defined account	*/
	*incend++ = "C:";			/* Decus-C library disk	*/
	*incend++ = "SY:";			/* System (boot) disk	*/
#define	MAXINCLUDE	(NINCLUDE - 3 - IS_INCLUDE)
#endif
}

int
dooptions(argc, argv)
int		argc;
char* argv[];
/*
 * dooptions is called to process command line arguments (-Detc).
 * It is called only at cpp startup.
 */
{
	register char* ap;
	register DEFBUF* dp;
	register int		c;
	int			i, j;
	char* arg;
	SIZES* sizp;		/* For -S		*/
	int			size;		/* For -S		*/
	int			isdatum;	/* FALSE for -S*	*/
	int			endtest;	/* For -S		*/

	for (i = j = 1; i < argc; i++) {
		arg = ap = argv[i];
		if (*ap++ != '-' || *ap == EOS)
			argv[j++] = argv[i];
		else {
			c = *ap++;			/* Option byte		*/
			if (islower(c))			/* Normalize case	*/
				c = toupper(c);
			switch (c) {			/* Command character	*/
			case 'C':			/* Keep comments	*/
				cflag = TRUE;
				keepcomments = TRUE;
				break;

			case 'D':			/* Define symbol	*/
#if HOST != SYS_UNIX
				zap_uc(ap);			/* Force define to U.C.	*/
#endif
			/*
			 * If the option is just "-Dfoo", make it -Dfoo=1
			 */
				while (*ap != EOS && *ap != '=')
					ap++;
				if (*ap == EOS)
					ap = "1";
				else
					*ap++ = EOS;
				/*
				 * Now, save the word and its definition.
				 */
				dp = defendel(argv[i] + 2, FALSE);
				dp->repl = savestring(ap);
				dp->nargs = DEF_NOARGS;
				break;

			case 'E':			/* Ignore non-fatal	*/
				eflag = TRUE;		/* errors.		*/
				break;

			case 'I':			/* Include directory	*/
				if (incend >= &incdir[MAXINCLUDE])
					cfatal("Too many include directories", NULLST);
				*incend++ = ap;
				break;

			case 'N':			/* No predefineds	*/
				nflag++;			/* Repeat to undefine	*/
				break;			/* __LINE__, etc.	*/

			case 'S':
				sizp = size_table;
				if (isdatum = (*ap != '*'))	/* If it's just -S,	*/
					endtest = T_FPTR;	/* Stop here		*/
				else {			/* But if it's -S*	*/
					ap++;			/* Step over '*'	*/
					endtest = 0;		/* Stop at end marker	*/
				}
				while (sizp->bits != endtest && *ap != EOS) {
					if (!isdigit(*ap)) {	/* Skip to next digit	*/
						ap++;
						continue;
					}
					size = 0;		/* Compile the value	*/
					while (isdigit(*ap)) {
						size *= 10;
						size += (*ap++ - '0');
					}
					if (isdatum)
						sizp->size = size;	/* Datum size		*/
					else
						sizp->psize = size;	/* Pointer size		*/
					sizp++;
				}
				if (sizp->bits != endtest)
					cwarn("-S, too few values specified in %s", argv[i]);
				else if (*ap != EOS)
					cwarn("-S, too many values, \"%s\" unused", ap);
				break;

			case 'U':			/* Undefine symbol	*/
#if HOST != SYS_UNIX
				zap_uc(ap);
#endif
				if (defendel(ap, TRUE) == NULL)
					cwarn("\"%s\" wasn't defined", ap);
				break;

#if DEBUG
			case 'X':			/* Debug		*/
				debug = (isdigit(*ap)) ? atoi(ap) : 1;
#if (HOST == SYS_VMS || HOST == SYS_UNIX)
				signal(SIGINT, abort);	/* Trap "interrupt"	*/
#endif
				fprintf(stderr, "Debug set to %d\n", debug);
				break;
#endif

			default:			/* What is this one?	*/
				cwarn("Unknown option \"%s\"", arg);
				fprintf(stderr, "The following options are valid:\n\
  -C\t\t\tWrite source file comments to output\n\
  -Dsymbol=value\tDefine a symbol with the given (optional) value\n\
  -Idirectory\t\tAdd a directory to the #include search list\n\
  -N\t\t\tDon't predefine target-specific names\n\
  -Stext\t\tSpecify sizes for #if sizeof\n\
  -Usymbol\t\tUndefine symbol\n");
#if DEBUG
				fprintf(stderr, "  -Xvalue\t\tSet internal debug flag\n");
#endif
				break;
			}			/* Switch on all options	*/
		}				/* If it's a -option		*/
	}				/* For all arguments		*/
	if (j > 3) {
		cerror(
			"Too many file arguments.  Usage: cpp [input [output]]",
			NULLST);
	}
	return (j);			/* Return new argc		*/
}

#if HOST != SYS_UNIX
FILE_LOCAL
zap_uc(ap)
register char* ap;
/*
 * Dec operating systems mangle upper-lower case in command lines.
 * This routine forces the -D and -U arguments to uppercase.
 * It is called only on cpp startup by dooptions().
 */
{
	while (*ap != EOS) {
		/*
		 * Don't use islower() here so it works with Multinational
		 */
		if (*ap >= 'a' && *ap <= 'z')
			*ap = toupper(*ap);
		ap++;
	}
}
#endif

initdefines()
/*
 * Initialize the built-in #define's.  There are two flavors:
 * 	#define decus	1		(static definitions)
 *	#define	__FILE__ ??		(dynamic, evaluated by magic)
 * Called only on cpp startup.
 *
 * Note: the built-in static definitions are supressed by the -N option.
 * __LINE__, __FILE__, and __DATE__ are always present.
 */
{
	register char** pp;
	register char* tp;
	register DEFBUF* dp;
	int			i;
	long long			tvec;
	extern char* ctime();

	/*
	 * Predefine the built-in symbols.  Allow the
	 * implementor to pre-define a symbol as "" to
	 * eliminate it.
	 */
	if (nflag == 0) {
		for (pp = preset; *pp != NULL; pp++) {
			if (*pp[0] != EOS) {
				dp = defendel(*pp, FALSE);
				dp->repl = savestring("1");
				dp->nargs = DEF_NOARGS;
			}
		}
	}
	/*
	 * The magic pre-defines (__FILE__ and __LINE__ are
	 * initialized with negative argument counts.  expand()
	 * notices this and calls the appropriate routine.
	 * DEF_NOARGS is one greater than the first "magic" definition.
	 */
	if (nflag < 2) {
		for (pp = magic, i = DEF_NOARGS; *pp != NULL; pp++) {
			dp = defendel(*pp, FALSE);
			dp->nargs = --i;
		}
#if OK_DATE
		/*
		 * Define __DATE__ as today's date.
		 */
		dp = defendel("__DATE__", FALSE);
		dp->repl = tp = getmem(27);
		dp->nargs = DEF_NOARGS;
		time(&tvec);
		*tp++ = '"';
		strcpy(tp, ctime(&tvec));
		tp[24] = '"';			/* Overwrite newline	*/
#endif
	}
}

#if HOST == SYS_VMS
/*
 * getredirection() is intended to aid in porting C programs
 * to VMS (Vax-11 C) which does not support '>' and '<'
 * I/O redirection.  With suitable modification, it may
 * useful for other portability problems as well.
 */

int
getredirection(argc, argv)
int		argc;
char** argv;
/*
 * Process vms redirection arg's.  Exit if any error is seen.
 * If getredirection() processes an argument, it is erased
 * from the vector.  getredirection() returns a new argc value.
 *
 * Warning: do not try to simplify the code for vms.  The code
 * presupposes that getredirection() is called before any data is
 * read from stdin or written to stdout.
 *
 * Normal usage is as follows:
 *
 *	main(argc, argv)
 *	int		argc;
 *	char		*argv[];
 *	{
 *		argc = getredirection(argc, argv);
 *	}
 */
{
	register char* ap;	/* Argument pointer	*/
	int			i;	/* argv[] index		*/
	int			j;	/* Output index		*/
	int			file;	/* File_descriptor 	*/
	extern int		errno;	/* Last vms i/o error 	*/

	for (j = i = 1; i < argc; i++) {   /* Do all arguments	*/
		switch (*(ap = argv[i])) {
		case '<':			/* <file		*/
			if (freopen(++ap, "r", stdin) == NULL) {
				perror(ap);		/* Can't find file	*/
				exit(errno);	/* Is a fatal error	*/
			}
			break;

		case '>':			/* >file or >>file	*/
			if (*++ap == '>') {	/* >>file		*/
				/*
				 * If the file exists, and is writable by us,
				 * call freopen to append to the file (using the
				 * file's current attributes).  Otherwise, create
				 * a new file with "vanilla" attributes as if the
				 * argument was given as ">filename".
				 * access(name, 2) returns zero if we can write on
				 * the specified file.
				 */
				if (access(++ap, 2) == 0) {
					if (freopen(ap, "a", stdout) != NULL)
						break;	/* Exit case statement	*/
					perror(ap);	/* Error, can't append	*/
					exit(errno);	/* After access test	*/
				}			/* If file accessable	*/
			}
			/*
			 * On vms, we want to create the file using "standard"
			 * record attributes.  creat(...) creates the file
			 * using the caller's default protection mask and
			 * "variable length, implied carriage return"
			 * attributes. dup2() associates the file with stdout.
			 */
			if ((file = creat(ap, 0, "rat=cr", "rfm=var")) == -1
				|| dup2(file, fileno(stdout)) == -1) {
				perror(ap);		/* Can't create file	*/
				exit(errno);	/* is a fatal error	*/
			}			/* If '>' creation	*/
			break;			/* Exit case test	*/

		default:
			argv[j++] = ap;		/* Not a redirector	*/
			break;			/* Exit case test	*/
		}
	}				/* For all arguments	*/
	argv[j] = NULL;			/* Terminate argv[]	*/
	return (j);			/* Return new argc	*/
}
#endif



/*
 *			    C P P 4 . C
 *		M a c r o  D e f i n i t i o n s
 *
 * Edit History
 * 31-Aug-84	MM	USENET net.sources release
 * 04-Oct-84	MM	__LINE__ and __FILE__ must call ungetstring()
 *			so they work correctly with token concatenation.
 *			Added string formal recognition.
 * 25-Oct-84	MM	"Short-circuit" evaluate #if's so that we
 *			don't print unnecessary error messages for
 *			#if !defined(FOO) && FOO != 0 && 10 / FOO ...
 * 31-Oct-84	ado/MM	Added token concatenation
 *  6-Nov-84	MM	Split off eval stuff
 */

#include	<stdio.h>
#include	<ctype.h>
#include	"cppdef.h"
#include	"cpp.h"
 /*
  * parm[], parmp, and parlist[] are used to store #define() argument
  * lists.  nargs contains the actual number of parameters stored.
  */
static char	parm[NPARMWORK + 1];	/* define param work buffer 	*/
static char* parmp;			/* Free space in parm		*/
static char* parlist[LASTPARM];	/* -> start of each parameter	*/
static int	nargs;			/* Parameters for this macro	*/

dodefine()
/*
 * Called from control when a #define is scanned.  This module
 * parses formal parameters and the replacement string.  When
 * the formal parameter name is encountered in the replacement
 * string, it is replaced by a character in the range 128 to
 * 128+NPARAM (this allows up to 32 parameters within the
 * Dec Multinational range).  If cpp is ported to an EBCDIC
 * machine, you will have to make other arrangements.
 *
 * There is some special case code to distinguish
 *	#define foo	bar
 * from	#define foo()	bar
 *
 * Also, we make sure that
 *	#define	foo	foo
 * expands to "foo" but doesn't put cpp into an infinite loop.
 *
 * A warning message is printed if you redefine a symbol to a
 * different text.  I.e,
 *	#define	foo	123
 *	#define foo	123
 * is ok, but
 *	#define foo	123
 *	#define	foo	+123
 * is not.
 *
 * The following subroutines are called from define():
 * checkparm	called when a token is scanned.  It checks through the
 *		array of formal parameters.  If a match is found, the
 *		token is replaced by a control byte which will be used
 *		to locate the parameter when the macro is expanded.
 * textput	puts a string in the macro work area (parm[]), updating
 *		parmp to point to the first free byte in parm[].
 *		textput() tests for work buffer overflow.
 * charput	puts a single character in the macro work area (parm[])
 *		in a manner analogous to textput().
 */
{
	register int		c;
	register DEFBUF* dp;		/* -> new definition	*/
	int			isredefine;	/* TRUE if redefined	*/
	char* old;		/* Remember redefined	*/
	extern int		save();		/* Save char in work[]	*/

	old = NULL; // Avoid uninitialised warning

	if (type[(c = skipws())] != LET)
		goto bad_define;
	isredefine = FALSE;			/* Set if redefining	*/
	if ((dp = lookid(c)) == NULL)		/* If not known now	*/
		dp = defendel(token, FALSE);	/* Save the name	*/
	else {					/* It's known:		*/
		isredefine = TRUE;			/* Remember this fact	*/
		old = dp->repl;			/* Remember replacement	*/
		dp->repl = NULL;			/* No replacement now	*/
	}
	parlist[0] = parmp = parm;		/* Setup parm buffer	*/
	if ((c = get()) == '(') {		/* With arguments?	*/
		nargs = 0;				/* Init formals counter	*/
		do {				/* Collect formal parms	*/
			if (nargs >= LASTPARM)
				cfatal("Too many arguments for macro", NULLST);
			else if ((c = skipws()) == ')')
				break;			/* Got them all		*/
			else if (type[c] != LET)	/* Bad formal syntax	*/
				goto bad_define;
			scanid(c);			/* Get the formal param	*/
			parlist[nargs++] = parmp;	/* Save its start	*/
			textput(token);			/* Save text in parm[]	*/
		} while ((c = skipws()) == ',');	/* Get another argument	*/
		if (c != ')')			/* Must end at )	*/
			goto bad_define;
		c = ' ';				/* Will skip to body	*/
	}
	else {
		/*
		 * DEF_NOARGS is needed to distinguish between
		 * "#define foo" and "#define foo()".
		 */
		nargs = DEF_NOARGS;			/* No () parameters	*/
	}
	if (type[c] == SPA)			/* At whitespace?	*/
		c = skipws();			/* Not any more.	*/
	workp = work;				/* Replacement put here	*/
	inmacro = TRUE;				/* Keep \<newline> now	*/
	while (c != EOF_CHAR && c != '\n') {	/* Compile macro body	*/
#if OK_CONCAT
#if COMMENT_INVISIBLE
		if (c == COM_SEP) {			/* Token concatenation? */
			save(TOK_SEP);			/* Stuff a delimiter	*/
			c = get();
#else
		if (c == '#') {			/* Token concatenation?	*/
			while (workp > work&& type[workp[-1]] == SPA)
				--workp;			/* Erase leading spaces	*/
			save(TOK_SEP);			/* Stuff a delimiter	*/
			c = skipws();			/* Eat whitespace	*/
#endif
			if (type[c] == LET)		/* Another token here?	*/
				;				/* Stuff it normally	*/
			else if (type[c] == DIG) {	/* Digit string after?	*/
				while (type[c] == DIG) {	/* Stuff the digits	*/
					save(c);
					c = get();
				}
				save(TOK_SEP);		/* Delimit 2nd token	*/
			}
			else {
#if ! COMMENT_INVISIBLE
				ciwarn("Strange character after # (%d.)", c);
#endif
			}
			continue;
		}
#endif
		switch (type[c]) {
		case LET:
			checkparm(c, dp);		/* Might be a formal	*/
			break;

		case DIG:				/* Number in mac. body	*/
		case DOT:				/* Maybe a float number	*/
			scannumber(c, save);		/* Scan it off		*/
			break;

		case QUO:				/* String in mac. body	*/
#if STRING_FORMAL
			stparmscan(c, dp);		/* Do string magic	*/
#else
			stparmscan(c);
#endif
			break;

		case BSH:				/* Backslash		*/
			save('\\');
			if ((c = get()) == '\n')
				wrongline = TRUE;
			save(c);
			break;

		case SPA:				/* Absorb whitespace	*/
		/*
		 * Note: the "end of comment" marker is passed on
		 * to allow comments to separate tokens.
		 */
			if (workp[-1] == ' ')		/* Absorb multiple	*/
				break;			/* spaces		*/
			else if (c == '\t')
				c = ' ';			/* Normalize tabs	*/
			/* Fall through to store character			*/
		default:				/* Other character	*/
			save(c);
			break;
		}
		c = get();
		}
	inmacro = FALSE;			/* Stop newline hack	*/
	unget();				/* For control check	*/
	if (workp > work&& workp[-1] == ' ')	/* Drop trailing blank	*/
		workp--;
	*workp = EOS;				/* Terminate work	*/
	dp->repl = savestring(work);		/* Save the string	*/
	dp->nargs = nargs;			/* Save arg count	*/
#if DEBUG
	if (debug)
		dumpadef("macro definition", dp);
#endif
	if (isredefine) {			/* Error if redefined	*/
		if ((old != NULL && dp->repl != NULL && !streq(old, dp->repl))
			|| (old == NULL && dp->repl != NULL)
			|| (old != NULL && dp->repl == NULL)) {
#ifdef STRICT_UNDEF
			cerror("Redefining defined variable \"%s\"", dp->name);
#else
			cwarn("Redefining defined variable \"%s\"", dp->name);
#endif
		}
		if (old != NULL)			/* We don't need the	*/
			free(old);			/* old definition now.	*/
	}
	return;

bad_define:
	cerror("#define syntax error", NULLST);
	inmacro = FALSE;			/* Stop <newline> hack	*/
	}

checkparm(c, dp)
register int	c;
DEFBUF* dp;
/*
 * Replace this param if it's defined.  Note that the macro name is a
 * possible replacement token.  We stuff DEF_MAGIC in front of the token
 * which is treated as a LETTER by the token scanner and eaten by
 * the output routine.  This prevents the macro expander from
 * looping if someone writes "#define foo foo".
 */
{
	register int		i;
	register char* cp;

	scanid(c);				/* Get parm to token[]	*/
	for (i = 0; i < nargs; i++) {		/* For each argument	*/
		if (streq(parlist[i], token)) {	/* If it's known	*/
			save(i + MAC_PARM);		/* Save a magic cookie	*/
			return;				/* And exit the search	*/
		}
	}
	if (streq(dp->name, token))		/* Macro name in body?	*/
		save(DEF_MAGIC);			/* Save magic marker	*/
	for (cp = token; *cp != EOS;)		/* And save		*/
		save(*cp++);			/* The token itself	*/
}

#if STRING_FORMAL
stparmscan(delim, dp)
int		delim;
register DEFBUF* dp;
/*
 * Scan the string (starting with the given delimiter).
 * The token is replaced if it is the only text in this string or
 * character constant.  The algorithm follows checkparm() above.
 * Note that scanstring() has approved of the string.
 */
{
	register int		c;

	/*
	 * Warning -- this code hasn't been tested for a while.
	 * It exists only to preserve compatibility with earlier
	 * implementations of cpp.  It is not part of the Draft
	 * ANSI Standard C language.
	 */
	save(delim);
	instring = TRUE;
	while ((c = get()) != delim
		&& c != '\n'
		&& c != EOF_CHAR) {
		if (type[c] == LET)			/* Maybe formal parm	*/
			checkparm(c, dp);
		else {
			save(c);
			if (c == '\\')
				save(get());
		}
	}
	instring = FALSE;
	if (c != delim)
		cerror("Unterminated string in macro body", NULLST);
	save(c);
}
#else
stparmscan(delim)
int		delim;
/*
 * Normal string parameter scan.
 */
{
	register char* wp;
	register int		i;
	extern int		save();

	wp = workp;			/* Here's where it starts	*/
	if (!scanstring(delim, save))
		return;			/* Exit on scanstring error	*/
	workp[-1] = EOS;		/* Erase trailing quote		*/
	wp++;				/* -> first string content byte	*/
	for (i = 0; i < nargs; i++) {
		if (streq(parlist[i], wp)) {
			*wp++ = MAC_PARM + PAR_MAC;	/* Stuff a magic marker	*/
			*wp++ = (i + MAC_PARM);		/* Make a formal marker	*/
			*wp = wp[-3];			/* Add on closing quote	*/
			workp = wp + 1;			/* Reset string end	*/
			return;
		}
	}
	workp[-1] = wp[-1];		/* Nope, reset end quote.	*/
}
#endif

doundef()
/*
 * Remove the symbol from the defined list.
 * Called from the #control processor.
 */
{
	register int		c;

	if (type[(c = skipws())] != LET)
		cerror("Illegal #undef argument", NULLST);
	else {
		scanid(c);				/* Get name to token[]	*/
		if (defendel(token, TRUE) == NULL) {
#ifdef STRICT_UNDEF
			cwarn("Symbol \"%s\" not defined in #undef", token);
#endif
		}
	}
}

textput(text)
char* text;
/*
 * Put the string in the parm[] buffer.
 */
{
	register int	size;

	size = strlen(text) + 1;
	if ((parmp + size) >= &parm[NPARMWORK])
		cfatal("Macro work area overflow", NULLST);
	else {
		strcpy(parmp, text);
		parmp += size;
	}
}

charput(c)
register int	c;
/*
 * Put the byte in the parm[] buffer.
 */
{
	if (parmp >= &parm[NPARMWORK])
		cfatal("Macro work area overflow", NULLST);
	else {
		*parmp++ = c;
	}
}

/*
 *		M a c r o   E x p a n s i o n
 */

static DEFBUF* macro;		/* Catches start of infinite macro	*/

expand(tokenp)
register DEFBUF* tokenp;
/*
 * Expand a macro.  Called from the cpp mainline routine (via subroutine
 * macroid()) when a token is found in the symbol table.  It calls
 * expcollect() to parse actual parameters, checking for the correct number.
 * It then creates a "file" containing a single line containing the
 * macro with actual parameters inserted appropriately.  This is
 * "pushed back" onto the input stream.  (When the get() routine runs
 * off the end of the macro line, it will dismiss the macro itself.)
 */
{
	register int		c;
	register FILEINFO* file;
	extern FILEINFO* getfile();

#if DEBUG
	if (debug)
		dumpadef("expand entry", tokenp);
#endif
	/*
	 * If no macro is pending, save the name of this macro
	 * for an eventual error message.
	 */
	if (recursion++ == 0)
		macro = tokenp;
	else if (recursion == RECURSION_LIMIT) {
		cerror("Recursive macro definition of \"%s\"", tokenp->name);
		fprintf(stderr, "(Defined by \"%s\")\n", macro->name);
		if (rec_recover) {
			do {
				c = get();
			} while (infile != NULL && infile->fp == NULL);
			unget();
			recursion = 0;
			return;
		}
	}
	/*
	 * Here's a macro to expand.
	 */
	nargs = 0;				/* Formals counter	*/
	parmp = parm;				/* Setup parm buffer	*/
	switch (tokenp->nargs) {
	case (-2):				/* __LINE__		*/
		sprintf(work, "%d", line);
		ungetstring(work);
		break;

	case (-3):				/* __FILE__		*/
		for (file = infile; file != NULL; file = file->parent) {
			if (file->fp != NULL) {
				sprintf(work, "\"%s\"", (file->progname != NULL)
					? file->progname : file->filename);
				ungetstring(work);
				break;
			}
		}
		break;

	default:
		/*
		 * Nothing funny about this macro.
		 */
		if (tokenp->nargs < 0)
			cfatal("Bug: Illegal __ macro \"%s\"", tokenp->name);
		while ((c = skipws()) == '\n')	/* Look for (, skipping	*/
			wrongline = TRUE;		/* spaces and newlines	*/
		if (c != '(') {
			/*
			 * If the programmer writes
			 *	#define foo() ...
			 *	...
			 *	foo [no ()]
			 * just write foo to the output stream.
			 */
			unget();
			cwarn("Macro \"%s\" needs arguments", tokenp->name);
			fputs(tokenp->name, stdout);
			return;
		}
		else if (expcollect()) {		/* Collect arguments	*/
			if (tokenp->nargs != nargs) {	/* Should be an error?	*/
				cwarn("Wrong number of macro arguments for \"%s\"",
					tokenp->name);
			}
#if DEBUG
			if (debug)
				dumpparm("expand");
#endif
		}				/* Collect arguments		*/
	case DEF_NOARGS:		/* No parameters just stuffs	*/
		expstuff(tokenp);		/* Do actual parameters		*/
	}				/* nargs switch			*/
}

FILE_LOCAL int
expcollect()
/*
 * Collect the actual parameters for this macro.  TRUE if ok.
 */
{
	register int	c;
	register int	paren;			/* For embedded ()'s	*/
	extern int	charput();

	for (;;) {
		paren = 0;				/* Collect next arg.	*/
		while ((c = skipws()) == '\n')	/* Skip over whitespace	*/
			wrongline = TRUE;		/* and newlines.	*/
		if (c == ')') {			/* At end of all args?	*/
		/*
		 * Note that there is a guard byte in parm[]
		 * so we don't have to check for overflow here.
		 */
			*parmp = EOS;			/* Make sure terminated	*/
			break;				/* Exit collection loop	*/
		}
		else if (nargs >= LASTPARM)
			cfatal("Too many arguments in macro expansion", NULLST);
		parlist[nargs++] = parmp;		/* At start of new arg	*/
		for (;; c = cget()) {		/* Collect arg's bytes	*/
			if (c == EOF_CHAR) {
				cerror("end of file within macro argument", NULLST);
				return (FALSE);		/* Sorry.		*/
			}
			else if (c == '\\') {		/* Quote next character	*/
				charput(c);			/* Save the \ for later	*/
				charput(cget());		/* Save the next char.	*/
				continue;			/* And go get another	*/
			}
			else if (type[c] == QUO) {	/* Start of string?	*/
				scanstring(c, charput);	/* Scan it off		*/
				continue;			/* Go get next char	*/
			}
			else if (c == '(')		/* Worry about balance	*/
				paren++;			/* To know about commas	*/
			else if (c == ')') {		/* Other side too	*/
				if (paren == 0) {		/* At the end?		*/
					unget();		/* Look at it later	*/
					break;			/* Exit arg getter.	*/
				}
				paren--;			/* More to come.	*/
			}
			else if (c == ',' && paren == 0) /* Comma delimits args	*/
				break;
			else if (c == '\n')		/* Newline inside arg?	*/
				wrongline = TRUE;		/* We'll need a #line	*/
			charput(c);			/* Store this one	*/
		}					/* Collect an argument	*/
		charput(EOS);			/* Terminate argument	*/
#if DEBUG
		if (debug)
			printf("parm[%d] = \"%s\"\n", nargs, parlist[nargs - 1]);
#endif
	}					/* Collect all args.	*/
	return (TRUE);				/* Normal return	*/
}

FILE_LOCAL
expstuff(tokenp)
DEFBUF* tokenp;		/* Current macro being expanded	*/
/*
 * Stuff the macro body, replacing formal parameters by actual parameters.
 */
{
	register int	c;			/* Current character	*/
	register char* inp;			/* -> repl string	*/
	register char* defp;			/* -> macro output buff	*/
	int		size;			/* Actual parm. size	*/
	char* defend;		/* -> output buff end	*/
	int		string_magic;		/* String formal hack	*/
	FILEINFO* file;			/* Funny #include	*/
	extern FILEINFO* getfile();

	file = getfile(NBUFF, tokenp->name);
	inp = tokenp->repl;			/* -> macro replacement	*/
	defp = file->buffer;			/* -> output buffer	*/
	defend = defp + (NBUFF - 1);		/* Note its end		*/
	if (inp != NULL) {
		while ((c = (*inp++ & 0xFF)) != EOS) {
			if (c >= MAC_PARM && c <= (MAC_PARM + PAR_MAC)) {
				string_magic = (c == (MAC_PARM + PAR_MAC));
				if (string_magic)
					c = (*inp++ & 0xFF);
				/*
				 * Replace formal parameter by actual parameter string.
				 */
				if ((c -= MAC_PARM) < nargs) {
					size = strlen(parlist[c]);
					if ((defp + size) >= defend)
						goto nospace;
					/*
					 * Erase the extra set of quotes.
					 */
					if (string_magic && defp[-1] == parlist[c][0]) {
						strcpy(defp - 1, parlist[c]);
						defp += (size - 2);
					}
					else {
						strcpy(defp, parlist[c]);
						defp += size;
					}
				}
			}
			else if (defp >= defend) {
			nospace:	    cfatal("Out of space in macro \"%s\" arg expansion",
				tokenp->name);
			}
			else {
				*defp++ = c;
			}
		}
	}
	*defp = EOS;
#if DEBUG
	if (debug > 1)
		printf("macroline: \"%s\"\n", file->buffer);
#endif
}

#if DEBUG
dumpparm(why)
char* why;
/*
 * Dump parameter list.
 */
{
	register int	i;

	printf("dump of %d parameters (%d bytes total) %s\n",
		nargs, parmp - parm, why);
	for (i = 0; i < nargs; i++) {
		printf("parm[%d] (%d) = \"%s\"\n",
			i + 1, strlen(parlist[i]), parlist[i]);
	}
}
#endif

/*
 *			    C P P 5 . C
 *		E x p r e s s i o n   E v a l u a t i o n
 *
 * Edit History
 * 31-Aug-84	MM	USENET net.sources release
 * 04-Oct-84	MM	__LINE__ and __FILE__ must call ungetstring()
 *			so they work correctly with token concatenation.
 *			Added string formal recognition.
 * 25-Oct-84	MM	"Short-circuit" evaluate #if's so that we
 *			don't print unnecessary error messages for
 *			#if !defined(FOO) && FOO != 0 && 10 / FOO ...
 * 31-Oct-84	ado/MM	Added token concatenation
 *  6-Nov-84	MM	Split from #define stuff, added sizeof stuff
 * 19-Nov-84	ado	#if error returns TRUE for (sigh) compatibility
 */

#include	<stdio.h>
#include	<ctype.h>
#include	"cppdef.h"
#include	"cpp.h"

 /*
  * Evaluate an #if expression.
  */

static char* opname[] = {		/* For debug and error messages	*/
"end of expression", "val", "id",
  "+",   "-",  "*",  "/",  "%",
  "<<", ">>",  "&",  "|",  "^",
  "==", "!=",  "<", "<=", ">=",  ">",
  "&&", "||",  "?",  ":",  ",",
  "unary +", "unary -", "~", "!",  "(",  ")", "(none)",
};

/*
 * opdope[] has the operator precedence:
 *     Bits
 *	  7	Unused (so the value is always positive)
 *	6-2	Precedence (000x .. 017x)
 *	1-0	Binary op. flags:
 *	    01	The binop flag should be set/cleared when this op is seen.
 *	    10	The new value of the binop flag.
 * Note:  Expected, New binop
 * constant	0	1	Binop, end, or ) should follow constants
 * End of line	1	0	End may not be preceeded by an operator
 * binary	1	0	Binary op follows a value, value follows.
 * unary	0	0	Unary op doesn't follow a value, value follows
 *   (		0	0	Doesn't follow value, value or unop follows
 *   )		1	1	Follows value.  Op follows.
 */

static char	opdope[OP_MAX] = {
  0001,					/* End of expression		*/
  0002,					/* Digit			*/
  0000,					/* Letter (identifier)		*/
  0141, 0141, 0151, 0151, 0151,		/* ADD, SUB, MUL, DIV, MOD	*/
  0131, 0131, 0101, 0071, 0071,		/* ASL, ASR, AND,  OR, XOR	*/
  0111, 0111, 0121, 0121, 0121,	0121,	/*  EQ,  NE,  LT,  LE,  GE,  GT	*/
  0061, 0051, 0041, 0041, 0031,		/* ANA, ORO, QUE, COL, CMA	*/
/*
 * Unary op's follow
 */
  0160, 0160, 0160, 0160,		/* NEG, PLU, COM, NOT		*/
  0170, 0013, 0023,			/* LPA, RPA, END		*/
};
/*
 * OP_QUE and OP_RPA have alternate precedences:
 */
#define	OP_RPA_PREC	0013
#define OP_QUE_PREC	0034

 /*
  * S_ANDOR and S_QUEST signal "short-circuit" boolean evaluation, so that
  *	#if FOO != 0 && 10 / FOO ...
  * doesn't generate an error message.  They are stored in optab.skip.
  */
#define S_ANDOR		2
#define S_QUEST		1

typedef struct optab {
	char	op;			/* Operator			*/
	char	prec;			/* Its precedence		*/
	char	skip;			/* Short-circuit: TRUE to skip	*/
} OPTAB;
static int	evalue;			/* Current value from evallex()	*/

#ifdef	nomacargs
FILE_LOCAL int
isbinary(op)
register int	op;
{
	return (op >= FIRST_BINOP && op <= LAST_BINOP);
}

FILE_LOCAL int
isunary(op)
register int	op;
{
	return (op >= FIRST_UNOP && op <= LAST_UNOP);
}
#else
#define	isbinary(op)	(op >= FIRST_BINOP && op <= LAST_BINOP)
#define	isunary(op)	(op >= FIRST_UNOP  && op <= LAST_UNOP)
#endif

/*
 * The following definitions are used to specify basic variable sizes.
 */

#ifndef	S_CHAR
#define	S_CHAR		(sizeof (char))
#endif
#ifndef	S_SINT
#define	S_SINT		(sizeof (short int))
#endif
#ifndef	S_INT
#define	S_INT		(sizeof (int))
#endif
#ifndef	S_LINT
#define	S_LINT		(sizeof (long int))
#endif
#ifndef	S_FLOAT
#define	S_FLOAT		(sizeof (float))
#endif
#ifndef	S_DOUBLE
#define	S_DOUBLE	(sizeof (double))
#endif
#ifndef	S_PCHAR
#define	S_PCHAR		(sizeof (char *))
#endif
#ifndef	S_PSINT
#define	S_PSINT		(sizeof (short int *))
#endif
#ifndef	S_PINT
#define	S_PINT		(sizeof (int *))
#endif
#ifndef	S_PLINT
#define	S_PLINT		(sizeof (long int *))
#endif
#ifndef	S_PFLOAT
#define	S_PFLOAT	(sizeof (float *))
#endif
#ifndef	S_PDOUBLE
#define	S_PDOUBLE	(sizeof (double *))
#endif
#ifndef	S_PFPTR
#define S_PFPTR		(sizeof (int (*)()))
#endif

typedef struct types {
	short	type;			/* This is the bit if		*/
	char* name;			/* this is the token word	*/
} TYPES;

static TYPES basic_types[] = {
	{ T_CHAR,	"char",		},
	{ T_INT,	"int",		},
	{ T_FLOAT,	"float",	},
	{ T_DOUBLE,	"double",	},
	{ T_SHORT,	"short",	},
	{ T_LONG,	"long",		},
	{ T_SIGNED,	"signed",	},
	{ T_UNSIGNED,	"unsigned",	},
	{ 0,		NULL,		},	/* Signal end		*/
};

/*
 * Test_table[] is used to test for illegal combinations.
 */
static short test_table[] = {
	T_FLOAT | T_DOUBLE | T_LONG | T_SHORT,
	T_FLOAT | T_DOUBLE | T_CHAR | T_INT,
	T_FLOAT | T_DOUBLE | T_SIGNED | T_UNSIGNED,
	T_LONG | T_SHORT | T_CHAR,
	0						/* end marker	*/
};

/*
 * The order of this table is important -- it is also referenced by
 * the command line processor to allow run-time overriding of the
 * built-in size values.  The order must not be changed:
 *	char, short, int, long, float, double (func pointer)
 */
SIZES size_table[] = {
	{ T_CHAR,	S_CHAR,		S_PCHAR		},	/* char		*/
	{ T_SHORT,	S_SINT,		S_PSINT		},	/* short int	*/
	{ T_INT,	S_INT,		S_PINT		},	/* int		*/
	{ T_LONG,	S_LINT,		S_PLINT		},	/* long		*/
	{ T_FLOAT,	S_FLOAT,	S_PFLOAT	},	/* float	*/
	{ T_DOUBLE,	S_DOUBLE,	S_PDOUBLE	},	/* double	*/
	{ T_FPTR,	0,		S_PFPTR		},	/* int (*()) 	*/
	{ 0,	0,		0		},	/* End of table	*/
};

int
eval()
/*
 * Evaluate an expression.  Straight-forward operator precedence.
 * This is called from control() on encountering an #if statement.
 * It calls the following routines:
 * evallex	Lexical analyser -- returns the type and value of
 *		the next input token.
 * evaleval	Evaluate the current operator, given the values on
 *		the value stack.  Returns a pointer to the (new)
 *		value stack.
 * For compatiblity with older cpp's, this return returns 1 (TRUE)
 * if a syntax error is detected.
 */
{
	register int	op;		/* Current operator		*/
	register int* valp;		/* -> value vector		*/
	register OPTAB* opp;		/* Operator stack		*/
	int		prec;		/* Op precedence		*/
	int		binop;		/* Set if binary op. needed	*/
	int		op1;		/* Operand from stack		*/
	int		skip;		/* For short-circuit testing	*/
	int		value[NEXP];	/* Value stack			*/
	OPTAB		opstack[NEXP];	/* Operand stack		*/
	extern int* evaleval();	/* Does actual evaluation	*/

	valp = value;
	opp = opstack;
	opp->op = OP_END;		/* Mark bottom of stack		*/
	opp->prec = opdope[OP_END];	/* And its precedence		*/
	opp->skip = 0;			/* Not skipping now		*/
	binop = 0;
again:;
#ifdef	DEBUG_EVAL
	printf("In #if at again: skip = %d, binop = %d, line is: %s",
		opp->skip, binop, infile->bptr);
#endif
	if ((op = evallex(opp->skip)) == OP_SUB && binop == 0)
		op = OP_NEG;			/* Unary minus		*/
	else if (op == OP_ADD && binop == 0)
		op = OP_PLU;			/* Unary plus		*/
	else if (op == OP_FAIL)
		return (1);				/* Error in evallex	*/
#ifdef	DEBUG_EVAL
	printf("op = %s, opdope = %03o, binop = %d, skip = %d\n",
		opname[op], opdope[op], binop, opp->skip);
#endif
	if (op == DIG) {			/* Value?		*/
		if (binop != 0) {
			cerror("misplaced constant in #if", NULLST);
			return (1);
		}
		else if (valp >= &value[NEXP - 1]) {
			cerror("#if value stack overflow", NULLST);
			return (1);
		}
		else {
#ifdef	DEBUG_EVAL
			printf("pushing %d onto value stack[%d]\n",
				evalue, valp - value);
#endif
			* valp++ = evalue;
			binop = 1;
		}
		goto again;
	}
	else if (op > OP_END) {
		cerror("Illegal #if line", NULLST);
		return (1);
	}
	prec = opdope[op];
	if (binop != (prec & 1)) {
		cerror("Operator %s in incorrect context", opname[op]);
		return (1);
	}
	binop = (prec & 2) >> 1;
	for (;;) {
#ifdef	DEBUG_EVAL
		printf("op %s, prec %d., stacked op %s, prec %d, skip %d\n",
			opname[op], prec, opname[opp->op], opp->prec, opp->skip);
#endif
		if (prec > opp->prec) {
			if (op == OP_LPA)
				prec = OP_RPA_PREC;
			else if (op == OP_QUE)
				prec = OP_QUE_PREC;
			op1 = opp->skip;		/* Save skip for test	*/
			/*
			 * Push operator onto op. stack.
			 */
			opp++;
			if (opp >= &opstack[NEXP]) {
				cerror("expression stack overflow at op \"%s\"",
					opname[op]);
				return (1);
			}
			opp->op = op;
			opp->prec = prec;
			skip = (valp[-1] != 0);		/* Short-circuit tester	*/
			/*
			 * Do the short-circuit stuff here.  Short-circuiting
			 * stops automagically when operators are evaluated.
			 */
			if ((op == OP_ANA && !skip)
				|| (op == OP_ORO && skip))
				opp->skip = S_ANDOR;	/* And/or skip starts	*/
			else if (op == OP_QUE)		/* Start of ?: operator	*/
				opp->skip = (op1 & S_ANDOR) | ((!skip) ? S_QUEST : 0);
			else if (op == OP_COL) {	/* : inverts S_QUEST	*/
				opp->skip = (op1 & S_ANDOR)
					| (((op1 & S_QUEST) != 0) ? 0 : S_QUEST);
			}
			else {				/* Other ops leave	*/
				opp->skip = op1;		/*  skipping unchanged.	*/
			}
#ifdef	DEBUG_EVAL
			printf("stacking %s, valp[-1] == %d at %s",
				opname[op], valp[-1], infile->bptr);
			dumpstack(opstack, opp, value, valp);
#endif
			goto again;
		}
		/*
		 * Pop operator from op. stack and evaluate it.
		 * End of stack and '(' are specials.
		 */
		skip = opp->skip;			/* Remember skip value	*/
		switch ((op1 = opp->op)) {		/* Look at stacked op	*/
		case OP_END:			/* Stack end marker	*/
			if (op == OP_EOE)
				return (valp[-1]);		/* Finished ok.		*/
			goto again;			/* Read another op.	*/

		case OP_LPA:			/* ( on stack		*/
			if (op != OP_RPA) {		/* Matches ) on input	*/
				cerror("unbalanced paren's, op is \"%s\"", opname[op]);
				return (1);
			}
			opp--;				/* Unstack it		*/
			/* goto again;			-- Fall through		*/

		case OP_QUE:
			goto again;			/* Evaluate true expr.	*/

		case OP_COL:			/* : on stack.		*/
			opp--;				/* Unstack :		*/
			if (opp->op != OP_QUE) {	/* Matches ? on stack?	*/
				cerror("Misplaced '?' or ':', previous operator is %s",
					opname[opp->op]);
				return (1);
			}
			/*
			 * Evaluate op1.
			 */
		default:				/* Others:		*/
			opp--;				/* Unstack the operator	*/
#ifdef	DEBUG_EVAL
			printf("Stack before evaluation of %s\n", opname[op1]);
			dumpstack(opstack, opp, value, valp);
#endif
			valp = evaleval(valp, op1, skip);
#ifdef	DEBUG_EVAL
			printf("Stack after evaluation\n");
			dumpstack(opstack, opp, value, valp);
#endif
		}					/* op1 switch end	*/
	}					/* Stack unwind loop	*/
}

FILE_LOCAL int
evallex(skip)
int		skip;		/* TRUE if short-circuit evaluation	*/
/*
 * Return next eval operator or value.  Called from eval().  It
 * calls a special-purpose routines for 'char' strings and
 * numeric values:
 * evalchar	called to evaluate 'x'
 * evalnum	called to evaluate numbers.
 */
{
	register int	c, c1, t;

again:  do {					/* Collect the token	*/
	c = skipws();
	if ((c = macroid(c)) == EOF_CHAR || c == '\n') {
		unget();
		return (OP_EOE);		/* End of expression	*/
	}
} while ((t = type[c]) == LET && catenate());
if (t == INV) {				/* Total nonsense	*/
	if (!skip) {
		if (isascii(c) && isprint(c))
			cierror("illegal character '%c' in #if", c);
		else
			cierror("illegal character (%d decimal) in #if", c);
	}
	return (OP_FAIL);
}
else if (t == QUO) {			/* ' or "		*/
	if (c == '\'') {			/* Character constant	*/
		evalue = evalchar(skip);	/* Somewhat messy	*/
#ifdef	DEBUG_EVAL
		printf("evalchar returns %d.\n", evalue);
#endif
		return (DIG);			/* Return a value	*/
	}
	cerror("Can't use a string in an #if", NULLST);
	return (OP_FAIL);
}
else if (t == LET) {			/* ID must be a macro	*/
	if (streq(token, "defined")) {	/* Or defined name	*/
		c1 = c = skipws();
		if (c == '(')			/* Allow defined(name)	*/
			c = skipws();
		if (type[c] == LET) {
			evalue = (lookid(c) != NULL);
			if (c1 != '('		/* Need to balance	*/
				|| skipws() == ')')	/* Did we balance?	*/
				return (DIG);		/* Parsed ok		*/
		}
		cerror("Bad #if ... defined() syntax", NULLST);
		return (OP_FAIL);
	}
	else if (streq(token, "sizeof"))	/* New sizeof hackery	*/
		return (dosizeof());		/* Gets own routine	*/
		/*
		 * The Draft ANSI C Standard says that an undefined symbol
		 * in an #if has the value zero.  We are a bit pickier,
		 * warning except where the programmer was careful to write
		 * 		#if defined(foo) ? foo : 0
		 */
#ifdef STRICT_UNDEF
	if (!skip)
		cwarn("undefined symbol \"%s\" in #if, 0 used", token);
#endif
	evalue = 0;
	return (DIG);
}
else if (t == DIG) {			/* Numbers are harder	*/
	evalue = evalnum(c);
#ifdef	DEBUG_EVAL
	printf("evalnum returns %d.\n", evalue);
#endif
}
else if (strchr("!=<>&|\\", c) != NULL) {
	/*
	 * Process a possible multi-byte lexeme.
	 */
	c1 = cget();			/* Peek at next char	*/
	switch (c) {
	case '!':
		if (c1 == '=')
			return (OP_NE);
		break;

	case '=':
		if (c1 != '=') {		/* Can't say a=b in #if	*/
			unget();
			cerror("= not allowed in #if", NULLST);
			return (OP_FAIL);
		}
		return (OP_EQ);

	case '>':
	case '<':
		if (c1 == c)
			return ((c == '<') ? OP_ASL : OP_ASR);
		else if (c1 == '=')
			return ((c == '<') ? OP_LE : OP_GE);
		break;

	case '|':
	case '&':
		if (c1 == c)
			return ((c == '|') ? OP_ORO : OP_ANA);
		break;

	case '\\':
		if (c1 == '\n')			/* Multi-line if	*/
			goto again;
		cerror("Unexpected \\ in #if", NULLST);
		return (OP_FAIL);
	}
	unget();
}
return (t);
}

FILE_LOCAL int
dosizeof()
/*
 * Process the sizeof (basic type) operation in an #if string.
 * Sets evalue to the size and returns
 *	DIG		success
 *	OP_FAIL		bad parse or something.
 */
{
	register int	c;
	register TYPES* tp;
	register SIZES* sizp;
	register short* testp;
	short		typecode;

	if ((c = skipws()) != '(')
		goto nogood;
	/*
	 * Scan off the tokens.
	 */
	typecode = 0;
	while ((c = skipws())) {
		if ((c = macroid(c)) == EOF_CHAR || c == '\n')
			goto nogood;			/* End of line is a bug	*/
		else if (c == '(') {		/* thing (*)() func ptr	*/
			if (skipws() == '*'
				&& skipws() == ')') {		/* We found (*)		*/
				if (skipws() != '(')	/* Let () be optional	*/
					unget();
				else if (skipws() != ')')
					goto nogood;
				typecode |= T_FPTR;		/* Function pointer	*/
			}
			else {				/* Junk is a bug	*/
				goto nogood;
			}
		}
		else if (type[c] != LET)		/* Exit if not a type	*/
			break;
		else if (!catenate()) {		/* Maybe combine tokens	*/
		/*
		 * Look for this unexpandable token in basic_types.
		 * The code accepts "int long" as well as "long int"
		 * which is a minor bug as bugs go (and one shared with
		 * a lot of C compilers).
		 */
			for (tp = basic_types; tp->name != NULLST; tp++) {
				if (streq(token, tp->name))
					break;
			}
			if (tp->name == NULLST) {
				cerror("#if sizeof, unknown type \"%s\"", token);
				return (OP_FAIL);
			}
			typecode |= tp->type;		/* Or in the type bit	*/
		}
	}
	/*
	 * We are at the end of the type scan.  Chew off '*' if necessary.
	 */
	if (c == '*') {
		typecode |= T_PTR;
		c = skipws();
	}
	if (c == ')') {				/* Last syntax check	*/
		for (testp = test_table; *testp != 0; testp++) {
			if (!bittest(typecode & *testp)) {
				cerror("#if ... sizeof: illegal type combination", NULLST);
				return (OP_FAIL);
			}
		}
		/*
		 * We assume that all function pointers are the same size:
		 *		sizeof (int (*)()) == sizeof (float (*)())
		 * We assume that signed and unsigned don't change the size:
		 *		sizeof (signed int) == (sizeof unsigned int)
		 */
		if ((typecode & T_FPTR) != 0)	/* Function pointer	*/
			typecode = T_FPTR | T_PTR;
		else {				/* Var or var * datum	*/
			typecode &= ~(T_SIGNED | T_UNSIGNED);
			if ((typecode & (T_SHORT | T_LONG)) != 0)
				typecode &= ~T_INT;
		}
		if ((typecode & ~T_PTR) == 0) {
			cerror("#if sizeof() error, no type specified", NULLST);
			return (OP_FAIL);
		}
		/*
		 * Exactly one bit (and possibly T_PTR) may be set.
		 */
		for (sizp = size_table; sizp->bits != 0; sizp++) {
			if ((typecode & ~T_PTR) == sizp->bits) {
				evalue = ((typecode & T_PTR) != 0)
					? sizp->psize : sizp->size;
				return (DIG);
			}
		}					/* We shouldn't fail	*/
		cierror("#if ... sizeof: bug, unknown type code 0x%x", typecode);
		return (OP_FAIL);
	}

nogood:	unget();
	cerror("#if ... sizeof() syntax error", NULLST);
	return (OP_FAIL);
}

FILE_LOCAL int
bittest(value)
/*
 * TRUE if value is zero or exactly one bit is set in value.
 */
{
#if (4096 & ~(-4096)) == 0
	return ((value & ~(-value)) == 0);
#else
	/*
	 * Do it the hard way (for non 2's complement machines)
	 */
	return (value == 0 || value ^ (value - 1) == (value * 2 - 1));
#endif
}

FILE_LOCAL int
evalnum(c)
register int	c;
/*
 * Expand number for #if lexical analysis.  Note: evalnum recognizes
 * the unsigned suffix, but only returns a signed int value.
 */
{
	register int	value;
	register int	base;
	register int	c1;

	if (c != '0')
		base = 10;
	else if ((c = cget()) == 'x' || c == 'X') {
		base = 16;
		c = cget();
	}
	else base = 8;
	value = 0;
	for (;;) {
		c1 = c;
		if (isascii(c) && isupper(c1))
			c1 = tolower(c1);
		if (c1 >= 'a')
			c1 -= ('a' - 10);
		else c1 -= '0';
		if (c1 < 0 || c1 >= base)
			break;
		value *= base;
		value += c1;
		c = cget();
	}
	if (c == 'u' || c == 'U')	/* Unsigned nonsense		*/
		c = cget();
	unget();
	return (value);
}

FILE_LOCAL int
evalchar(skip)
int		skip;		/* TRUE if short-circuit evaluation	*/
/*
 * Get a character constant
 */
{
	register int	c;
	register int	value;
	register int	count;

	instring = TRUE;
	if ((c = cget()) == '\\') {
		switch ((c = cget())) {
		case 'a':				/* New in Standard	*/
#if ('a' == '\a' || '\a' == ALERT)
			value = ALERT;			/* Use predefined value	*/
#else
			value = '\a';			/* Use compiler's value	*/
#endif
			break;

		case 'b':
			value = '\b';
			break;

		case 'f':
			value = '\f';
			break;

		case 'n':
			value = '\n';
			break;

		case 'r':
			value = '\r';
			break;

		case 't':
			value = '\t';
			break;

		case 'v':				/* New in Standard	*/
#if ('v' == '\v' || '\v' == VT)
			value = VT;			/* Use predefined value	*/
#else
			value = '\v';			/* Use compiler's value	*/
#endif
			break;

		case 'x':				/* '\xFF'		*/
			count = 3;
			value = 0;
			while ((((c = get()) >= '0' && c <= '9')
				|| (c >= 'a' && c <= 'f')
				|| (c >= 'A' && c <= 'F'))
				&& (--count >= 0)) {
				value *= 16;
				value += (c <= '9') ? (c - '0') : ((c & 0xF) + 9);
			}
			unget();
			break;

		default:
			if (c >= '0' && c <= '7') {
				count = 3;
				value = 0;
				while (c >= '0' && c <= '7' && --count >= 0) {
					value *= 8;
					value += (c - '0');
					c = get();
				}
				unget();
			}
			else value = c;
			break;
		}
	}
	else if (c == '\'')
		value = 0;
	else value = c;
	/*
	 * We warn on multi-byte constants and try to hack
	 * (big|little)endian machines.
	 */
#if BIG_ENDIAN
	count = 0;
#endif
	while ((c = get()) != '\'' && c != EOF_CHAR && c != '\n') {
		if (!skip)
			ciwarn("multi-byte constant '%c' isn't portable", c);
#if BIG_ENDIAN
		count += BITS_CHAR;
		value += (c << count);
#else
		value <<= BITS_CHAR;
		value += c;
#endif
	}
	instring = FALSE;
	return (value);
}

FILE_LOCAL int*
evaleval(valp, op, skip)
register int* valp;
int		op;
int		skip;		/* TRUE if short-circuit evaluation	*/
/*
 * Apply the argument operator to the data on the value stack.
 * One or two values are popped from the value stack and the result
 * is pushed onto the value stack.
 *
 * OP_COL is a special case.
 *
 * evaleval() returns the new pointer to the top of the value stack.
 */
{
	register int	v1, v2;

	if (isbinary(op))
		v2 = *--valp;
	v1 = *--valp;
#ifdef	DEBUG_EVAL
	printf("%s op %s", (isbinary(op)) ? "binary" : "unary",
		opname[op]);
	if (isbinary(op))
		printf(", v2 = %d.", v2);
	printf(", v1 = %d.\n", v1);
#endif
	switch (op) {
	case OP_EOE:
		break;

	case OP_ADD:
		v1 += v2;
		break;

	case OP_SUB:
		v1 -= v2;
		break;

	case OP_MUL:
		v1 *= v2;
		break;

	case OP_DIV:
	case OP_MOD:
		if (v2 == 0) {
			if (!skip) {
				cwarn("%s by zero in #if, zero result assumed",
					(op == OP_DIV) ? "divide" : "mod");
			}
			v1 = 0;
		}
		else if (op == OP_DIV)
			v1 /= v2;
		else
			v1 %= v2;
		break;

	case OP_ASL:
		v1 <<= v2;
		break;

	case OP_ASR:
		v1 >>= v2;
		break;

	case OP_AND:
		v1 &= v2;
		break;

	case OP_OR:
		v1 |= v2;
		break;

	case OP_XOR:
		v1 ^= v2;
		break;

	case OP_EQ:
		v1 = (v1 == v2);
		break;

	case OP_NE:
		v1 = (v1 != v2);
		break;

	case OP_LT:
		v1 = (v1 < v2);
		break;

	case OP_LE:
		v1 = (v1 <= v2);
		break;

	case OP_GE:
		v1 = (v1 >= v2);
		break;

	case OP_GT:
		v1 = (v1 > v2);
		break;

	case OP_ANA:
		v1 = (v1 && v2);
		break;

	case OP_ORO:
		v1 = (v1 || v2);
		break;

	case OP_COL:
		/*
		 * v1 has the "true" value, v2 the "false" value.
		 * The top of the value stack has the test.
		 */
		v1 = (*--valp) ? v1 : v2;
		break;

	case OP_NEG:
		v1 = (-v1);
		break;

	case OP_PLU:
		break;

	case OP_COM:
		v1 = ~v1;
		break;

	case OP_NOT:
		v1 = !v1;
		break;

	default:
		cierror("#if bug, operand = %d.", op);
		v1 = 0;
	}
	*valp++ = v1;
	return (valp);
}

#ifdef	DEBUG_EVAL
dumpstack(opstack, opp, value, valp)
OPTAB		opstack[NEXP];	/* Operand stack		*/
register OPTAB* opp;		/* Operator stack		*/
int		value[NEXP];	/* Value stack			*/
register int* valp;		/* -> value vector		*/
{
	printf("index op prec skip name -- op stack at %s", infile->bptr);
	while (opp > opstack) {
		printf(" [%2d] %2d  %03o    %d %s\n", opp - opstack,
			opp->op, opp->prec, opp->skip, opname[opp->op]);
		opp--;
	}
	while (--valp >= value) {
		printf("value[%d] = %d\n", (valp - value), *valp);
	}
}
#endif

/*
 *			    C P P 6 . C
 *		S u p p o r t   R o u t i n e s
 *
 * Edit History
 * 25-May-84 MM		Added 8-bit support to type table.
 * 30-May-84 ARF	sharp() should output filename in quotes
 * 02-Aug-84 MM		Newline and #line hacking.  sharp() now in cpp1.c
 * 31-Aug-84 MM		USENET net.sources release
 * 11-Sep-84 ado/MM	Keepcomments, also line number pathological
 * 12-Sep-84 ado/MM	bug if comment changes to space and we unget later.
 * 03-Oct-84 gkr/MM	Fixed scannumber bug for '.e' (as in struct.element).
 * 04-Oct-84 MM		Added ungetstring() for token concatenation
 * 08-Oct-84 MM		Yet another attack on number scanning
 * 31-Oct-84 ado	Parameterized $ in identifiers
 *  2-Nov-84 MM		Token concatenation is messier than I thought
 *  6-Dec-84 MM		\<nl> is everywhere invisible.
 */

#include	<stdio.h>
#include	<ctype.h>
#include	"cppdef.h"
#include	"cpp.h"

 /*
  * skipnl()	skips over input text to the end of the line.
  * skipws()	skips over "whitespace" (spaces or tabs), but
  *		not skip over the end of the line.  It skips over
  *		TOK_SEP, however (though that shouldn't happen).
  * scanid()	reads the next token (C identifier) into token[].
  *		The caller has already read the first character of
  *		the identifier.  Unlike macroid(), the token is
  *		never expanded.
  * macroid()	reads the next token (C identifier) into token[].
  *		If it is a #defined macro, it is expanded, and
  *		macroid() returns TRUE, otherwise, FALSE.
  * catenate()	Does the dirty work of token concatenation, TRUE if it did.
  * scanstring()	Reads a string from the input stream, calling
  *		a user-supplied function for each character.
  *		This function may be output() to write the
  *		string to the output file, or save() to save
  *		the string in the work buffer.
  * scannumber()	Reads a C numeric constant from the input stream,
  *		calling the user-supplied function for each
  *		character.  (output() or save() as noted above.)
  * save()	Save one character in the work[] buffer.
  * savestring()	Saves a string in malloc() memory.
  * getfile()	Initialize a new FILEINFO structure, called when
  *		#include opens a new file, or a macro is to be
  *		expanded.
  * getmem()	Get a specified number of bytes from malloc memory.
  * output()	Write one character to stdout (calling putchar) --
  *		implemented as a function so its address may be
  *		passed to scanstring() and scannumber().
  * lookid()	Scans the next token (identifier) from the input
  *		stream.  Looks for it in the #defined symbol table.
  *		Returns a pointer to the definition, if found, or NULL
  *		if not present.  The identifier is stored in token[].
  * defnedel()	Define enter/delete subroutine.  Updates the
  *		symbol table.
  * get()	Read the next byte from the current input stream,
  *		handling end of (macro/file) input and embedded
  *		comments appropriately.  Note that the global
  *		instring is -- essentially -- a parameter to get().
  * cget()	Like get(), but skip over TOK_SEP.
  * unget()	Push last gotten character back on the input stream.
  * cerror(), cwarn(), cfatal(), cierror(), ciwarn()
  *		These routines format an print messages to the user.
  *		cerror & cwarn take a format and a single string argument.
  *		cierror & ciwarn take a format and a single int (char) argument.
  *		cfatal takes a format and a single string argument.
  */

  /*
   * This table must be rewritten for a non-Ascii machine.
   *
   * Note that several "non-visible" characters have special meaning:
   * Hex 1D DEF_MAGIC -- a flag to prevent #define recursion.
   * Hex 1E TOK_SEP   -- a delimiter for token concatenation
   * Hex 1F COM_SEP   -- a zero-width whitespace for comment concatenation
   */
#if TOK_SEP != 0x1E || COM_SEP != 0x1F || DEF_MAGIC != 0x1D
<< error type table isn't correct >>
#endif

#if OK_DOLLAR
#define	DOL	LET
#else
#define	DOL	000
#endif

char type[256] = {		/* Character type codes    Hex		*/
   END,   000,   000,   000,   000,   000,   000,   000, /* 00		*/
   000,   SPA,   000,   000,   000,   000,   000,   000, /* 08		*/
   000,   000,   000,   000,   000,   000,   000,   000, /* 10		*/
   000,   000,   000,   000,   000,   LET,   000,   SPA, /* 18		*/
   SPA,OP_NOT,   QUO,   000,   DOL,OP_MOD,OP_AND,   QUO, /* 20  !"#$%&'	*/
OP_LPA,OP_RPA,OP_MUL,OP_ADD,   000,OP_SUB,   DOT,OP_DIV, /* 28 ()*+,-./	*/
   DIG,   DIG,   DIG,   DIG,   DIG,   DIG,   DIG,   DIG, /* 30 01234567	*/
   DIG,   DIG,OP_COL,   000, OP_LT, OP_EQ, OP_GT,OP_QUE, /* 38 89:;<=>?	*/
   000,   LET,   LET,   LET,   LET,   LET,   LET,   LET, /* 40 @ABCDEFG	*/
   LET,   LET,   LET,   LET,   LET,   LET,   LET,   LET, /* 48 HIJKLMNO	*/
   LET,   LET,   LET,   LET,   LET,   LET,   LET,   LET, /* 50 PQRSTUVW	*/
   LET,   LET,   LET,   000,   BSH,   000,OP_XOR,   LET, /* 58 XYZ[\]^_	*/
   000,   LET,   LET,   LET,   LET,   LET,   LET,   LET, /* 60 `abcdefg	*/
   LET,   LET,   LET,   LET,   LET,   LET,   LET,   LET, /* 68 hijklmno	*/
   LET,   LET,   LET,   LET,   LET,   LET,   LET,   LET, /* 70 pqrstuvw	*/
   LET,   LET,   LET,   000, OP_OR,   000,OP_NOT,   000, /* 78 xyz{|}~	*/
   000,   000,   000,   000,   000,   000,   000,   000, /*   80 .. FF	*/
   000,   000,   000,   000,   000,   000,   000,   000, /*   80 .. FF	*/
   000,   000,   000,   000,   000,   000,   000,   000, /*   80 .. FF	*/
   000,   000,   000,   000,   000,   000,   000,   000, /*   80 .. FF	*/
   000,   000,   000,   000,   000,   000,   000,   000, /*   80 .. FF	*/
   000,   000,   000,   000,   000,   000,   000,   000, /*   80 .. FF	*/
   000,   000,   000,   000,   000,   000,   000,   000, /*   80 .. FF	*/
   000,   000,   000,   000,   000,   000,   000,   000, /*   80 .. FF	*/
};

skipnl()
/*
 * Skip to the end of the current input line.
 */
{
	register int		c;

	do {				/* Skip to newline	*/
		c = get();
	} while (c != '\n' && c != EOF_CHAR);
}

int
skipws()
/*
 * Skip over whitespace
 */
{
	register int		c;

	do {				/* Skip whitespace	*/
		c = get();
#if COMMENT_INVISIBLE
	} while (type[c] == SPA || c == COM_SEP);
#else
} while (type[c] == SPA);
#endif
return (c);
}

scanid(c)
register int	c;				/* First char of id	*/
/*
 * Get the next token (an id) into the token buffer.
 * Note: this code is duplicated in lookid().
 * Change one, change both.
 */
{
	register char* bp;

	if (c == DEF_MAGIC)			/* Eat the magic token	*/
		c = get();				/* undefiner.		*/
	bp = token;
	do {
		if (bp < &token[IDMAX])		/* token dim is IDMAX+1	*/
			*bp++ = c;
		c = get();
	} while (type[c] == LET || type[c] == DIG);
	unget();
	*bp = EOS;
}

int
macroid(c)
register int		c;
/*
 * If c is a letter, scan the id.  if it's #defined, expand it and scan
 * the next character and try again.
 *
 * Else, return the character.  If type[c] is a LET, the token is in token.
 */
{
	register DEFBUF* dp;

	if (infile != NULL && infile->fp != NULL)
		recursion = 0;
	while (type[c] == LET && (dp = lookid(c)) != NULL) {
		expand(dp);
		c = get();
	}
	return (c);
}

int
catenate()
/*
 * A token was just read (via macroid).
 * If the next character is TOK_SEP, concatenate the next token
 * return TRUE -- which should recall macroid after refreshing
 * macroid's argument.  If it is not TOK_SEP, unget() the character
 * and return FALSE.
 */
{
	register int		c;
	register char* token1;

#if OK_CONCAT
	if (get() != TOK_SEP) {			/* Token concatenation	*/
		unget();
		return (FALSE);
	}
	else {
		token1 = savestring(token);		/* Save first token	*/
		c = macroid(get());			/* Scan next token	*/
		switch (type[c]) {			/* What was it?		*/
		case LET:				/* An identifier, ...	*/
			if (strlen(token1) + strlen(token) >= NWORK)
				cfatal("work buffer overflow doing %s #", token1);
			sprintf(work, "%s%s", token1, token);
			break;

		case DIG:				/* A digit string	*/
			strcpy(work, token1);
			workp = work + strlen(work);
			do {
				save(c);
			} while ((c = get()) != TOK_SEP);
			/*
			 * The trailing TOK_SEP is no longer needed.
			 */
			save(EOS);
			break;

		default:				/* An error, ...	*/
#if ! COMMENT_INVISIBLE
			if (isprint(c))
				cierror("Strange character '%c' after #", c);
			else
				cierror("Strange character (%d.) after #", c);
#endif
			strcpy(work, token1);
			unget();
			break;
		}
		/*
		 * work has the concatenated token and token1 has
		 * the first token (no longer needed).  Unget the
		 * new (concatenated) token after freeing token1.
		 * Finally, setup to read the new token.
		 */
		free(token1);			/* Free up memory	*/
		ungetstring(work);			/* Unget the new thing,	*/
		return (TRUE);
	}
#else
	return (FALSE);				/* Not supported	*/
#endif
}

int
scanstring(delim, outfun)
register int	delim;			/* ' or "			*/
int		(*outfun)();		/* Output function		*/
/*
 * Scan off a string.  Warning if terminated by newline or EOF.
 * outfun() outputs the character -- to a buffer if in a macro.
 * TRUE if ok, FALSE if error.
 */
{
	register int		c;

	instring = TRUE;		/* Don't strip comments		*/
	(*outfun)(delim);
	while ((c = get()) != delim
		&& c != '\n'
		&& c != EOF_CHAR) {

		if (c != DEF_MAGIC)
			(*outfun)(c);
		if (c == '\\')
			(*outfun)(get());
	}
	instring = FALSE;
	if (c == delim) {
		(*outfun)(c);
		return (TRUE);
	}
	else {
		cerror("Unterminated string", NULLST);
		unget();
		return (FALSE);
	}
}

scannumber(c, outfun)
register int	c;				/* First char of number	*/
register int	(*outfun)();			/* Output/store func	*/
/*
 * Process a number.  We know that c is from 0 to 9 or dot.
 * Algorithm from Dave Conroy's Decus C.
 */
{
	register int	radix;			/* 8, 10, or 16		*/
	int		expseen;		/* 'e' seen in floater	*/
	int		signseen;		/* '+' or '-' seen	*/
	int		octal89;		/* For bad octal test	*/
	int		dotflag;		/* TRUE if '.' was seen	*/

	expseen = FALSE;			/* No exponent seen yet	*/
	signseen = TRUE;			/* No +/- allowed yet	*/
	octal89 = FALSE;			/* No bad octal yet	*/
	radix = 10;				/* Assume decimal	*/
	if ((dotflag = (c == '.')) != FALSE) {	/* . something?		*/
		(*outfun)('.');			/* Always out the dot	*/
		if (type[(c = get())] != DIG) {	/* If not a float numb,	*/
			unget();			/* Rescan strange char	*/
			return;				/* All done for now	*/
		}
	}					/* End of float test	*/
	else if (c == '0') {			/* Octal or hex?	*/
		(*outfun)(c);			/* Stuff initial zero	*/
		radix = 8;				/* Assume it's octal	*/
		c = get();				/* Look for an 'x'	*/
		if (c == 'x' || c == 'X') {		/* Did we get one?	*/
			radix = 16;			/* Remember new radix	*/
			(*outfun)(c);			/* Stuff the 'x'	*/
			c = get();			/* Get next character	*/
		}
	}
	for (;;) {				/* Process curr. char.	*/
		/*
		 * Note that this algorithm accepts "012e4" and "03.4"
		 * as legitimate floating-point numbers.
		 */
		if (radix != 16 && (c == 'e' || c == 'E')) {
			if (expseen)			/* Already saw 'E'?	*/
				break;			/* Exit loop, bad nbr.	*/
			expseen = TRUE;			/* Set exponent seen	*/
			signseen = FALSE;		/* We can read '+' now	*/
			radix = 10;			/* Decimal exponent	*/
		}
		else if (radix != 16 && c == '.') {
			if (dotflag)			/* Saw dot already?	*/
				break;			/* Exit loop, two dots	*/
			dotflag = TRUE;			/* Remember the dot	*/
			radix = 10;			/* Decimal fraction	*/
		}
		else if (c == '+' || c == '-') {	/* 1.0e+10		*/
			if (signseen)			/* Sign in wrong place?	*/
				break;			/* Exit loop, not nbr.	*/
			/* signseen = TRUE; */		/* Remember we saw it	*/
		}
		else {				/* Check the digit	*/
			switch (c) {
			case '8': case '9':		/* Sometimes wrong	*/
				octal89 = TRUE;		/* Do check later	*/
			case '0': case '1': case '2': case '3':
			case '4': case '5': case '6': case '7':
				break;			/* Always ok		*/

			case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
			case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
				if (radix == 16)		/* Alpha's are ok only 	*/
					break;			/* if reading hex.	*/
			default:			/* At number end	*/
				goto done;			/* Break from for loop	*/
			}				/* End of switch	*/
		}					/* End general case	*/
		(*outfun)(c);			/* Accept the character	*/
		signseen = TRUE;			/* Don't read sign now	*/
		c = get();				/* Read another char	*/
	}					/* End of scan loop	*/
	/*
	 * When we break out of the scan loop, c contains the first
	 * character (maybe) not in the number.  If the number is an
	 * integer, allow a trailing 'L' for long and/or a trailing 'U'
	 * for unsigned.  If not those, push the trailing character back
	 * on the input stream.  Floating point numbers accept a trailing
	 * 'L' for "long double".
	 */
done:	if (dotflag || expseen) {		/* Floating point?	*/
	if (c == 'l' || c == 'L') {
		(*outfun)(c);
		c = get();			/* Ungotten later	*/
	}
}
else {					/* Else it's an integer	*/
	/*
	 * We know that dotflag and expseen are both zero, now:
	 * dotflag signals "saw 'L'", and
	 * expseen signals "saw 'U'".
	 */
	for (;;) {
		switch (c) {
		case 'l':
		case 'L':
			if (dotflag)
				goto nomore;
			dotflag = TRUE;
			break;

		case 'u':
		case 'U':
			if (expseen)
				goto nomore;
			expseen = TRUE;
			break;

		default:
			goto nomore;
		}
		(*outfun)(c);			/* Got 'L' or 'U'.	*/
		c = get();			/* Look at next, too.	*/
	}
}
nomore:	unget();				/* Not part of a number	*/
if (octal89 && radix == 8)
cwarn("Illegal digit in octal number", NULLST);
}

save(c)
register int	c;
{
	if (workp >= &work[NWORK]) {
		work[NWORK - 1] = '\0';
		cfatal("Work buffer overflow:  %s", work);
	}
	else *workp++ = c;
}

char*
savestring(text)
char* text;
/*
 * Store a string into free memory.
 */
{
	register char* result;

	result = getmem(strlen(text) + 1);
	strcpy(result, text);
	return (result);
}

FILEINFO*
getfile(bufsize, name)
int		bufsize;		/* Line or define buffer size	*/
char* name;			/* File or macro name string	*/
/*
 * Common FILEINFO buffer initialization for a new file or macro.
 */
{
	register FILEINFO* file;
	register int		size;

	size = strlen(name);			/* File/macro name	*/
	file = (FILEINFO*)getmem(sizeof(FILEINFO) + bufsize + size);
	file->parent = infile;			/* Chain files together	*/
	file->fp = NULL;			/* No file yet		*/
	file->filename = savestring(name);	/* Save file/macro name	*/
	file->progname = NULL;			/* No #line seen yet	*/
	file->unrecur = 0;			/* No macro fixup	*/
	file->bptr = file->buffer;		/* Initialize line ptr	*/
	file->buffer[0] = EOS;			/* Force first read	*/
	file->line = 0;				/* (Not used just yet)	*/
	if (infile != NULL)			/* If #include file	*/
		infile->line = line;		/* Save current line	*/
	infile = file;				/* New current file	*/
	line = 1;				/* Note first line	*/
	return (file);				/* All done.		*/
}

char*
getmem(size)
int		size;
/*
 * Get a block of free memory.
 */
{
	register char* result;
	extern char* malloc();

	if ((result = malloc((unsigned)size)) == NULL)
		cfatal("Out of memory", NULLST);
	return (result);
}

/*
 *			C P P   S y m b o l   T a b l e s
 */

 /*
  * SBSIZE defines the number of hash-table slots for the symbol table.
  * It must be a power of 2.
  */
#ifndef	SBSIZE
#define	SBSIZE	64
#endif
#define	SBMASK	(SBSIZE - 1)
#if (SBSIZE ^ SBMASK) != ((SBSIZE * 2) - 1)
<< error, SBSIZE must be a power of 2 >>
#endif

static DEFBUF* symtab[SBSIZE];	/* Symbol table queue headers	*/

DEFBUF*
lookid(c)
int	c;				/* First character of token	*/
/*
 * Look for the next token in the symbol table.  Returns token in "token".
 * If found, returns the table pointer;  Else returns NULL.
 */
{
	register int		nhash;
	register DEFBUF* dp;
	register char* np;
	int			temp = 0;
	int			isrecurse;	/* For #define foo foo	*/

	np = token;
	nhash = 0;
	if ((isrecurse = (c == DEF_MAGIC)))	/* If recursive macro	*/
		c = get();				/* hack, skip DEF_MAGIC	*/
	do {
		if (np < &token[IDMAX]) {		/* token dim is IDMAX+1	*/
			*np++ = c;			/* Store token byte	*/
			nhash += c;			/* Update hash value	*/
		}
		c = get();				/* And get another byte	*/
	} while (type[c] == LET || type[c] == DIG);
	unget();				/* Rescan terminator	*/
	*np = EOS;				/* Terminate token	*/
	if (isrecurse)				/* Recursive definition	*/
		return (NULL);			/* undefined just now	*/
	nhash += (np - token);			/* Fix hash value	*/
	dp = symtab[nhash & SBMASK];		/* Starting bucket	*/
	while (dp != (DEFBUF*)NULL) {		/* Search symbol table	*/
		if (dp->hash == nhash		/* Fast precheck	*/
			&& (temp = strcmp(dp->name, token)) >= 0)
			break;
		dp = dp->link;			/* Nope, try next one	*/
	}
	return ((temp == 0) ? dp : NULL);
}

DEFBUF*
defendel(name, delete)
char* name;
int		delete;			/* TRUE to delete a symbol	*/
/*
 * Enter this name in the lookup table (delete = FALSE)
 * or delete this name (delete = TRUE).
 * Returns a pointer to the define block (delete = FALSE)
 * Returns NULL if the symbol wasn't defined (delete = TRUE).
 */
{
	register DEFBUF* dp;
	register DEFBUF** prevp;
	register char* np;
	int			nhash;
	int			temp;
	int			size;

	for (nhash = 0, np = name; *np != EOS;)
		nhash += *np++;
	size = (np - name);
	nhash += size;
	prevp = &symtab[nhash & SBMASK];
	while ((dp = *prevp) != (DEFBUF*)NULL) {
		if (dp->hash == nhash
			&& (temp = strcmp(dp->name, name)) >= 0) {
			if (temp > 0)
				dp = NULL;			/* Not found		*/
			else {
				*prevp = dp->link;		/* Found, unlink and	*/
				if (dp->repl != NULL)	/* Free the replacement	*/
					free(dp->repl);		/* if any, and then	*/
				free((char*)dp);		/* Free the symbol	*/
			}
			break;
		}
		prevp = &dp->link;
	}
	if (!delete) {
		dp = (DEFBUF*)getmem(sizeof(DEFBUF) + size);
		dp->link = *prevp;
		*prevp = dp;
		dp->hash = nhash;
		dp->repl = NULL;
		dp->nargs = 0;
		strcpy(dp->name, name);
	}
	return (dp);
}

#if DEBUG

dumpdef(why)
char* why;
{
	register DEFBUF* dp;
	register DEFBUF** syp;

	printf("CPP symbol table dump %s\n", why);
	for (syp = symtab; syp < &symtab[SBSIZE]; syp++) {
		if ((dp = *syp) != (DEFBUF*)NULL) {
			printf("symtab[%d]\n", (syp - symtab));
			do {
				dumpadef((char*)NULL, dp);
			} while ((dp = dp->link) != (DEFBUF*)NULL);
		}
	}
}

dumpadef(why, dp)
char* why;			/* Notation			*/
register DEFBUF* dp;
{
	register char* cp;
	register int		c;

	printf(" \"%s\" [%d]", dp->name, dp->nargs);
	if (why != NULL)
		printf(" (%s)", why);
	if (dp->repl != NULL) {
		printf(" => ");
		for (cp = dp->repl; (c = *cp++ & 0xFF) != EOS;) {
			if (c >= MAC_PARM && c <= (MAC_PARM + PAR_MAC))
				printf("<%d>", c - MAC_PARM);
			else if (isprint(c) || c == '\n' || c == '\t')
				putchar(c);
			else if (c < ' ')
				printf("<^%c>", c + '@');
			else
				printf("<\\0%o>", c);
		}
	}
	else {
		printf(", no replacement.");
	}
	putchar('\n');
}
#endif

/*
 *			G E T
 */

int
get()
/*
 * Return the next character from a macro or the current file.
 * Handle end of file from #include files.
 */
{
	register int		c;
	register FILEINFO* file;
	register int		popped;		/* Recursion fixup	*/

	popped = 0;
get_from_file:
	if ((file = infile) == NULL)
		return (EOF_CHAR);
newline:
#if 0
	printf("get(%s), recursion %d, line %d, bptr = %d, buffer \"%s\"\n",
		file->filename, recursion, line,
		file->bptr - file->buffer, file->buffer);
#endif
	/*
	 * Read a character from the current input line or macro.
	 * At EOS, either finish the current macro (freeing temp.
	 * storage) or read another line from the current input file.
	 * At EOF, exit the current file (#include) or, at EOF from
	 * the cpp input file, return EOF_CHAR to finish processing.
	 */
	if ((c = *file->bptr++ & 0xFF) == EOS) {
		/*
		 * Nothing in current line or macro.  Get next line (if
		 * input from a file), or do end of file/macro processing.
		 * In the latter case, jump back to restart from the top.
		 */
		if (file->fp == NULL) {		/* NULL if macro	*/
			popped++;
			recursion -= file->unrecur;
			if (recursion < 0)
				recursion = 0;
			infile = file->parent;		/* Unwind file chain	*/
		}
		else {				/* Else get from a file	*/
			if ((file->bptr = fgets(file->buffer, NBUFF, file->fp))
				!= NULL) {
#if DEBUG
				if (debug > 1) {		/* Dump it to stdout	*/
					printf("\n#line %d (%s), %s",
						line, file->filename, file->buffer);
				}
#endif
				goto newline;		/* process the line	*/
			}
			else {
				fclose(file->fp);		/* Close finished file	*/
				if ((infile = file->parent) != NULL) {
					/*
					 * There is an "ungotten" newline in the current
					 * infile buffer (set there by doinclude() in
					 * cpp1.c).  Thus, we know that the mainline code
					 * is skipping over blank lines and will do a
					 * #line at its convenience.
					 */
					wrongline = TRUE;	/* Need a #line now	*/
				}
			}
		}
		/*
		 * Free up space used by the (finished) file or macro and
		 * restart input from the parent file/macro, if any.
		 */
		free(file->filename);		/* Free name and	*/
		if (file->progname != NULL)		/* if a #line was seen,	*/
			free(file->progname);		/* free it, too.	*/
		free((char*)file);		/* Free file space	*/
		if (infile == NULL)			/* If at end of file	*/
			return (EOF_CHAR);		/* Return end of file	*/
		line = infile->line; 		/* Reset line number	*/
		goto get_from_file;			/* Get from the top.	*/
	}
	/*
	 * Common processing for the new character.
	 */
	if (c == DEF_MAGIC && file->fp != NULL)	/* Don't allow delete	*/
		goto newline;			/* from a file		*/
	if (file->parent != NULL) {		/* Macro or #include	*/
		if (popped != 0)
			file->parent->unrecur += popped;
		else {
			recursion -= file->parent->unrecur;
			if (recursion < 0)
				recursion = 0;
			file->parent->unrecur = 0;
		}
	}
	if (c == '\n')				/* Maintain current	*/
		++line;				/* line counter		*/
	if (instring)				/* Strings just return	*/
		return (c);				/* the character.	*/
	else if (c == '/') {			/* Comment?		*/
		instring = TRUE;			/* So get() won't loop	*/
		if ((c = get()) != '*') {		/* Next byte '*'?	*/
			instring = FALSE;		/* Nope, no comment	*/
			unget();			/* Push the char. back	*/
			return ('/');			/* Return the slash	*/
		}
		if (keepcomments) {			/* If writing comments	*/
			putchar('/');			/* Write out the	*/
			putchar('*');			/*   initializer	*/
		}
		for (;;) {				/* Eat a comment	*/
			c = get();
		test:		if (keepcomments && c != EOF_CHAR)
			cput(c);
		switch (c) {
		case EOF_CHAR:
			cerror("EOF in comment", NULLST);
			return (EOF_CHAR);

		case '/':
			if ((c = get()) != '*')	/* Don't let comments	*/
				goto test;		/* Nest.		*/
#ifdef STRICT_COMMENTS
			cwarn("Nested comments", NULLST);
#endif
			/* Fall into * stuff	*/
		case '*':
			if ((c = get()) != '/')	/* If comment doesn't	*/
				goto test;		/* end, look at next	*/
			instring = FALSE;		/* End of comment,	*/
			if (keepcomments) {		/* Put out the comment	*/
				cput(c);		/* terminator, too	*/
			}
			/*
			 * A comment is syntactically "whitespace" --
			 * however, there are certain strange sequences
			 * such as
			 *		#define foo(x)	(something)
			 *			foo|* comment *|(123)
			 *       these are '/' ^           ^
			 * where just returning space (or COM_SEP) will cause
			 * problems.  This can be "fixed" by overwriting the
			 * '/' in the input line buffer with ' ' (or COM_SEP)
			 * but that may mess up an error message.
			 * So, we peek ahead -- if the next character is
			 * "whitespace" we just get another character, if not,
			 * we modify the buffer.  All in the name of purity.
			 */
			if (*file->bptr == '\n'
				|| type[*file->bptr & 0xFF] == SPA)
				goto newline;
#if COMMENT_INVISIBLE
			/*
			 * Return magic (old-fashioned) syntactic space.
			 */
			return ((file->bptr[-1] = COM_SEP));
#else
			return ((file->bptr[-1] = ' '));
#endif

		case '\n':			/* we'll need a #line	*/
			if (!keepcomments)
				wrongline = TRUE;	/* later...		*/
		default:			/* Anything else is	*/
			break;			/* Just a character	*/
		}				/* End switch		*/
		}					/* End comment loop	*/
	}					/* End if in comment	*/
	else if (!inmacro && c == '\\') {	/* If backslash, peek 	*/
		if ((c = get()) == '\n') {		/* for a <nl>.  If so,	*/
			wrongline = TRUE;
			goto newline;
		}
		else {				/* Backslash anything	*/
			unget();			/* Get it later		*/
			return ('\\');			/* Return the backslash	*/
		}
	}
	else if (c == '\f' || c == VT)		/* Form Feed, Vertical	*/
		c = ' ';				/* Tab are whitespace	*/
	return (c);				/* Just return the char	*/
}

unget()
/*
 * Backup the pointer to reread the last character.  Fatal error
 * (code bug) if we backup too far.  unget() may be called,
 * without problems, at end of file.  Only one character may
 * be ungotten.  If you need to unget more, call ungetstring().
 */
{
	register FILEINFO* file;

	if ((file = infile) == NULL)
		return;			/* Unget after EOF		*/
	if (--file->bptr < file->buffer)
		cfatal("Too much pushback", NULLST);
	if (*file->bptr == '\n')	/* Ungetting a newline?		*/
		--line;			/* Unget the line number, too	*/
}

ungetstring(text)
char* text;
/*
 * Push a string back on the input stream.  This is done by treating
 * the text as if it were a macro.
 */
{
	register FILEINFO* file;
	extern FILEINFO* getfile();

	file = getfile(strlen(text) + 1, "");
	strcpy(file->buffer, text);
}

int
cget()
/*
 * Get one character, absorb "funny space" after comments or
 * token concatenation
 */
{
	register int	c;

	do {
		c = get();
#if COMMENT_INVISIBLE
	} while (c == TOK_SEP || c == COM_SEP);
#else
} while (c == TOK_SEP);
#endif
return (c);
}

/*
 * Error messages and other hacks.  The first byte of severity
 * is 'S' for string arguments and 'I' for int arguments.  This
 * is needed for portability with machines that have int's that
 * are shorter than  char *'s.
 */

static
domsg(severity, format, arg)
char* severity;		/* "Error", "Warning", "Fatal"	*/
char* format;		/* Format for the error message	*/
char* arg;			/* Something for the message	*/
/*
 * Print filenames, macro names, and line numbers for error messages.
 */
{
	register char* tp;
	register FILEINFO* file;

	fprintf(stderr, "%sline %d, %s: ", MSG_PREFIX, line, &severity[1]);
	if (*severity == 'S')
		fprintf(stderr, format, arg);
	else
		fprintf(stderr, format, (int)arg);
	putc('\n', stderr);
	if ((file = infile) == NULL)
		return;				/* At end of file	*/
	if (file->fp != NULL) {
		tp = file->buffer;			/* Print current file	*/
		fprintf(stderr, "%s", tp);		/* name, making sure	*/
		if (tp[strlen(tp) - 1] != '\n')	/* there's a newline	*/
			putc('\n', stderr);
	}
	while ((file = file->parent) != NULL) {	/* Print #includes, too	*/
		if (file->fp == NULL)
			fprintf(stderr, "from macro %s\n", file->filename);
		else {
			tp = file->buffer;
			fprintf(stderr, "from file %s, line %d:\n%s",
				(file->progname != NULL)
				? file->progname : file->filename,
				file->line, tp);
			if (tp[strlen(tp) - 1] != '\n')
				putc('\n', stderr);
		}
	}
}

cerror(format, sarg)
char* format;
char* sarg;		/* Single string argument		*/
/*
 * Print a normal error message, string argument.
 */
{
	domsg("SError", format, sarg);
	errors++;
}

cierror(format, narg)
char* format;
int		narg;		/* Single numeric argument		*/
/*
 * Print a normal error message, numeric argument.
 */
{
	domsg("IError", format, (char*)narg);
	errors++;
}

cfatal(format, sarg)
char* format;
char* sarg;			/* Single string argument	*/
/*
 * A real disaster
 */
{
	domsg("SFatal error", format, sarg);
	exit(IO_ERROR);
}

cwarn(format, sarg)
char* format;
char* sarg;			/* Single string argument	*/
/*
 * A non-fatal error, string argument.
 */
{
	domsg("SWarning", format, sarg);
}

ciwarn(format, narg)
char* format;
int		narg;			/* Single numeric argument	*/
/*
 * A non-fatal error, numeric argument.
 */
{
	domsg("IWarning", format, (char*)narg);
}



