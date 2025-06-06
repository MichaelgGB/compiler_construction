/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "miniJava.y"

// Topmost C block: Keep only very basic includes or forward declarations
// that do not depend on ast.h types yet.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Externs for Flex variables that Bison needs to know about.
// yylex() is declared later. yyerror() is defined at the bottom.
extern int yylineno;
extern char *yytext;
extern FILE *yyin;

#line 85 "miniJava.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "miniJava.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_TOKEN_INT_LIT = 3,              /* TOKEN_INT_LIT  */
  YYSYMBOL_TOKEN_FLOAT_LIT = 4,            /* TOKEN_FLOAT_LIT  */
  YYSYMBOL_TOKEN_CHAR_LIT = 5,             /* TOKEN_CHAR_LIT  */
  YYSYMBOL_TOKEN_STRING_LIT = 6,           /* TOKEN_STRING_LIT  */
  YYSYMBOL_TOKEN_ID = 7,                   /* TOKEN_ID  */
  YYSYMBOL_TOKEN_CLASS = 8,                /* TOKEN_CLASS  */
  YYSYMBOL_TOKEN_PUBLIC = 9,               /* TOKEN_PUBLIC  */
  YYSYMBOL_TOKEN_STATIC = 10,              /* TOKEN_STATIC  */
  YYSYMBOL_TOKEN_VOID = 11,                /* TOKEN_VOID  */
  YYSYMBOL_TOKEN_INT = 12,                 /* TOKEN_INT  */
  YYSYMBOL_TOKEN_CHAR = 13,                /* TOKEN_CHAR  */
  YYSYMBOL_TOKEN_BOOLEAN = 14,             /* TOKEN_BOOLEAN  */
  YYSYMBOL_TOKEN_IF = 15,                  /* TOKEN_IF  */
  YYSYMBOL_TOKEN_ELSE = 16,                /* TOKEN_ELSE  */
  YYSYMBOL_TOKEN_WHILE = 17,               /* TOKEN_WHILE  */
  YYSYMBOL_TOKEN_RETURN = 18,              /* TOKEN_RETURN  */
  YYSYMBOL_TOKEN_TRUE = 19,                /* TOKEN_TRUE  */
  YYSYMBOL_TOKEN_FALSE = 20,               /* TOKEN_FALSE  */
  YYSYMBOL_TOKEN_FINAL = 21,               /* TOKEN_FINAL  */
  YYSYMBOL_TOKEN_PRINT = 22,               /* TOKEN_PRINT  */
  YYSYMBOL_TOKEN_PLUS = 23,                /* TOKEN_PLUS  */
  YYSYMBOL_TOKEN_MINUS = 24,               /* TOKEN_MINUS  */
  YYSYMBOL_TOKEN_MULTIPLY = 25,            /* TOKEN_MULTIPLY  */
  YYSYMBOL_TOKEN_DIVIDE = 26,              /* TOKEN_DIVIDE  */
  YYSYMBOL_TOKEN_MOD = 27,                 /* TOKEN_MOD  */
  YYSYMBOL_TOKEN_EQ = 28,                  /* TOKEN_EQ  */
  YYSYMBOL_TOKEN_NEQ = 29,                 /* TOKEN_NEQ  */
  YYSYMBOL_TOKEN_LT = 30,                  /* TOKEN_LT  */
  YYSYMBOL_TOKEN_GT = 31,                  /* TOKEN_GT  */
  YYSYMBOL_TOKEN_LEQ = 32,                 /* TOKEN_LEQ  */
  YYSYMBOL_TOKEN_GEQ = 33,                 /* TOKEN_GEQ  */
  YYSYMBOL_TOKEN_AND = 34,                 /* TOKEN_AND  */
  YYSYMBOL_TOKEN_OR = 35,                  /* TOKEN_OR  */
  YYSYMBOL_TOKEN_NOT = 36,                 /* TOKEN_NOT  */
  YYSYMBOL_TOKEN_ASSIGN = 37,              /* TOKEN_ASSIGN  */
  YYSYMBOL_TOKEN_LPAREN = 38,              /* TOKEN_LPAREN  */
  YYSYMBOL_TOKEN_RPAREN = 39,              /* TOKEN_RPAREN  */
  YYSYMBOL_TOKEN_LBRACE = 40,              /* TOKEN_LBRACE  */
  YYSYMBOL_TOKEN_RBRACE = 41,              /* TOKEN_RBRACE  */
  YYSYMBOL_TOKEN_LBRACKET = 42,            /* TOKEN_LBRACKET  */
  YYSYMBOL_TOKEN_RBRACKET = 43,            /* TOKEN_RBRACKET  */
  YYSYMBOL_TOKEN_SEMICOLON = 44,           /* TOKEN_SEMICOLON  */
  YYSYMBOL_TOKEN_COMMA = 45,               /* TOKEN_COMMA  */
  YYSYMBOL_TOKEN_IF_WITHOUT_ELSE = 46,     /* TOKEN_IF_WITHOUT_ELSE  */
  YYSYMBOL_YYACCEPT = 47,                  /* $accept  */
  YYSYMBOL_program = 48,                   /* program  */
  YYSYMBOL_class_body_items = 49,          /* class_body_items  */
  YYSYMBOL_class_declaration = 50,         /* class_declaration  */
  YYSYMBOL_main_method_declaration = 51,   /* main_method_declaration  */
  YYSYMBOL_statements = 52,                /* statements  */
  YYSYMBOL_statement = 53,                 /* statement  */
  YYSYMBOL_block_statement_core = 54,      /* block_statement_core  */
  YYSYMBOL_variable_declaration = 55,      /* variable_declaration  */
  YYSYMBOL_type = 56,                      /* type  */
  YYSYMBOL_assignment_core = 57,           /* assignment_core  */
  YYSYMBOL_if_statement_core = 58,         /* if_statement_core  */
  YYSYMBOL_while_statement_core = 59,      /* while_statement_core  */
  YYSYMBOL_print_statement_core = 60,      /* print_statement_core  */
  YYSYMBOL_return_statement_core = 61,     /* return_statement_core  */
  YYSYMBOL_expression = 62,                /* expression  */
  YYSYMBOL_logical_or_expression = 63,     /* logical_or_expression  */
  YYSYMBOL_logical_and_expression = 64,    /* logical_and_expression  */
  YYSYMBOL_equality_expression = 65,       /* equality_expression  */
  YYSYMBOL_relational_expression = 66,     /* relational_expression  */
  YYSYMBOL_additive_expression = 67,       /* additive_expression  */
  YYSYMBOL_multiplicative_expression = 68, /* multiplicative_expression  */
  YYSYMBOL_unary_expression = 69,          /* unary_expression  */
  YYSYMBOL_primary_expression = 70,        /* primary_expression  */
  YYSYMBOL_identifier = 71,                /* identifier  */
  YYSYMBOL_integer_literal = 72,           /* integer_literal  */
  YYSYMBOL_float_literal = 73,             /* float_literal  */
  YYSYMBOL_char_literal = 74,              /* char_literal  */
  YYSYMBOL_string_literal = 75             /* string_literal  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;


/* Second part of user prologue.  */
#line 56 "miniJava.y"

    ProgramNode *root_ast_node = NULL; // Your global AST root - DEFINITION MOVED HERE

    // Forward declare yylex() which is provided by Flex.
    // yyerror() is defined at the bottom of this file.
    extern int yylex(void);
    void yyerror(const char *s);

#line 203 "miniJava.tab.c"


#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_uint8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
             && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE) \
             + YYSIZEOF (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  6
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   127

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  47
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  29
/* YYNRULES -- Number of rules.  */
#define YYNRULES  69
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  131

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   301


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   124,   124,   133,   142,   150,   162,   179,   191,   192,
     197,   199,   201,   203,   205,   207,   209,   214,   219,   221,
     223,   228,   230,   232,   234,   239,   241,   246,   248,   253,
     258,   263,   265,   270,   274,   275,   279,   280,   284,   285,
     286,   290,   291,   292,   293,   294,   298,   299,   300,   304,
     305,   306,   307,   311,   312,   314,   319,   321,   323,   325,
     327,   329,   331,   333,   335,   340,   347,   351,   355,   359
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "TOKEN_INT_LIT",
  "TOKEN_FLOAT_LIT", "TOKEN_CHAR_LIT", "TOKEN_STRING_LIT", "TOKEN_ID",
  "TOKEN_CLASS", "TOKEN_PUBLIC", "TOKEN_STATIC", "TOKEN_VOID", "TOKEN_INT",
  "TOKEN_CHAR", "TOKEN_BOOLEAN", "TOKEN_IF", "TOKEN_ELSE", "TOKEN_WHILE",
  "TOKEN_RETURN", "TOKEN_TRUE", "TOKEN_FALSE", "TOKEN_FINAL",
  "TOKEN_PRINT", "TOKEN_PLUS", "TOKEN_MINUS", "TOKEN_MULTIPLY",
  "TOKEN_DIVIDE", "TOKEN_MOD", "TOKEN_EQ", "TOKEN_NEQ", "TOKEN_LT",
  "TOKEN_GT", "TOKEN_LEQ", "TOKEN_GEQ", "TOKEN_AND", "TOKEN_OR",
  "TOKEN_NOT", "TOKEN_ASSIGN", "TOKEN_LPAREN", "TOKEN_RPAREN",
  "TOKEN_LBRACE", "TOKEN_RBRACE", "TOKEN_LBRACKET", "TOKEN_RBRACKET",
  "TOKEN_SEMICOLON", "TOKEN_COMMA", "TOKEN_IF_WITHOUT_ELSE", "$accept",
  "program", "class_body_items", "class_declaration",
  "main_method_declaration", "statements", "statement",
  "block_statement_core", "variable_declaration", "type",
  "assignment_core", "if_statement_core", "while_statement_core",
  "print_statement_core", "return_statement_core", "expression",
  "logical_or_expression", "logical_and_expression", "equality_expression",
  "relational_expression", "additive_expression",
  "multiplicative_expression", "unary_expression", "primary_expression",
  "identifier", "integer_literal", "float_literal", "char_literal",
  "string_literal", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-111)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int8 yypact[] =
{
       7,     6,    32,  -111,  -111,   -23,  -111,  -111,    17,    23,
       2,  -111,  -111,    58,  -111,  -111,    12,     6,    38,    14,
       6,  -111,    26,     6,  -111,    27,     1,    41,     1,  -111,
    -111,  -111,  -111,  -111,  -111,     1,     1,     1,  -111,    46,
      48,    -5,    29,     4,    49,  -111,  -111,    42,  -111,  -111,
    -111,  -111,    44,  -111,  -111,  -111,    50,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,    45,  -111,    48,    -5,    29,    29,     4,     4,     4,
       4,    49,    49,  -111,  -111,  -111,    47,    33,  -111,  -111,
      53,    54,     1,    55,    56,    33,  -111,    51,    52,  -111,
    -111,    57,    59,   -28,     1,     1,  -111,     1,  -111,  -111,
    -111,  -111,  -111,  -111,     1,     1,    60,    63,    65,  -111,
      62,    33,    33,  -111,    61,    90,  -111,     1,    33,  -111,
    -111
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,     0,     0,     2,    65,     0,     1,     3,     0,     0,
      21,    22,    23,     0,     6,     5,     0,     0,     0,     0,
       0,     4,    18,     0,    24,     0,     0,     0,     0,    66,
      67,    68,    69,    60,    61,     0,     0,     0,    19,    33,
      34,    36,    38,    41,    46,    49,    53,    62,    56,    57,
      58,    59,     0,    20,    54,    55,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    64,    35,    37,    39,    40,    42,    43,    44,
      45,    47,    48,    50,    51,    52,     0,     8,     7,    63,
       0,     0,    31,     0,     0,     8,    16,     0,     0,    12,
      13,     0,     0,     0,     0,     0,    32,     0,    17,     9,
      10,    11,    14,    15,     0,     0,     0,     0,     0,    25,
       0,     0,     0,    30,     0,    27,    29,     0,     0,    26,
      28
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
    -111,  -111,  -111,  -111,  -111,    13,  -110,    36,   101,    97,
    -111,  -111,  -111,  -111,  -111,   -27,  -111,    66,    64,   -18,
       5,   -13,   -33,  -111,    -1,  -111,  -111,  -111,  -111
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
       0,     2,     8,     3,    15,    94,    95,    96,    97,    17,
      98,    99,   100,   101,   102,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
       5,    53,    54,    55,    29,    30,    31,    32,     4,   114,
      56,   125,   126,     4,   115,     1,    22,     7,   130,    25,
      33,    34,    27,    59,    60,    35,     9,    65,    66,    10,
      11,    12,     6,    18,    83,    84,    85,    36,    13,    37,
       4,    75,    76,    86,    19,    10,    11,    12,    90,    23,
      91,    92,    81,    82,    13,    93,    21,    24,    14,    61,
      62,    63,    64,    26,    28,   106,    77,    78,    79,    80,
      10,    11,    12,    87,    67,    68,    69,   116,   117,    52,
     118,    57,    58,    71,    70,    87,   103,   119,   120,    72,
      89,   104,   105,   107,   103,   110,   111,   108,   127,   121,
     129,   112,   122,   113,   123,   124,   128,    88,   109,    16,
      20,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     103,   103,    74,    73,     0,     0,     0,   103
};

static const yytype_int16 yycheck[] =
{
       1,    28,    35,    36,     3,     4,     5,     6,     7,    37,
      37,   121,   122,     7,    42,     8,    17,    40,   128,    20,
      19,    20,    23,    28,    29,    24,     9,    23,    24,    12,
      13,    14,     0,    10,    67,    68,    69,    36,    21,    38,
       7,    59,    60,    70,    42,    12,    13,    14,    15,    11,
      17,    18,    65,    66,    21,    22,    44,    43,    41,    30,
      31,    32,    33,    37,    37,    92,    61,    62,    63,    64,
      12,    13,    14,    40,    25,    26,    27,   104,   105,    38,
     107,    35,    34,    39,    42,    40,    87,   114,   115,    39,
      43,    38,    38,    38,    95,    44,    44,    41,    37,    39,
     127,    44,    39,    44,    39,    43,    16,    71,    95,     8,
      13,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     121,   122,    58,    57,    -1,    -1,    -1,   128
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     8,    48,    50,     7,    71,     0,    40,    49,     9,
      12,    13,    14,    21,    41,    51,    55,    56,    10,    42,
      56,    44,    71,    11,    43,    71,    37,    71,    37,     3,
       4,     5,     6,    19,    20,    24,    36,    38,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    38,    62,    69,    69,    62,    35,    34,    28,
      29,    30,    31,    32,    33,    23,    24,    25,    26,    27,
      42,    39,    39,    64,    65,    66,    66,    67,    67,    67,
      67,    68,    68,    69,    69,    69,    62,    40,    54,    43,
      15,    17,    18,    22,    52,    53,    54,    55,    57,    58,
      59,    60,    61,    71,    38,    38,    62,    38,    41,    52,
      44,    44,    44,    44,    37,    42,    62,    62,    62,    62,
      62,    39,    39,    39,    43,    53,    53,    37,    16,    62,
      53
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    47,    48,    49,    49,    49,    50,    51,    52,    52,
      53,    53,    53,    53,    53,    53,    53,    54,    55,    55,
      55,    56,    56,    56,    56,    57,    57,    58,    58,    59,
      60,    61,    61,    62,    63,    63,    64,    64,    65,    65,
      65,    66,    66,    66,    66,    66,    67,    67,    67,    68,
      68,    68,    68,    69,    69,    69,    70,    70,    70,    70,
      70,    70,    70,    70,    70,    71,    72,    73,    74,    75
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     0,     3,     2,     5,     7,     0,     2,
       2,     2,     1,     1,     2,     2,     1,     3,     2,     4,
       5,     1,     1,     1,     3,     3,     6,     5,     7,     5,
       4,     1,     2,     1,     1,     3,     1,     3,     1,     3,
       3,     1,     3,     3,     3,     3,     1,     3,     3,     1,
       3,     3,     3,     1,     2,     2,     1,     1,     1,     1,
       1,     1,     1,     4,     3,     1,     1,     1,     1,     1
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF

/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (0)
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)


/* YYLOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

# ifndef YYLOCATION_PRINT

#  if defined YY_LOCATION_PRINT

   /* Temporary convenience wrapper in case some people defined the
      undocumented and private YY_LOCATION_PRINT macros.  */
#   define YYLOCATION_PRINT(File, Loc)  YY_LOCATION_PRINT(File, *(Loc))

#  elif defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static int
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
{
  int res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += YYFPRINTF (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += YYFPRINTF (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += YYFPRINTF (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += YYFPRINTF (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += YYFPRINTF (yyo, "-%d", end_col);
    }
  return res;
}

#   define YYLOCATION_PRINT  yy_location_print_

    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT(File, Loc)  YYLOCATION_PRINT(File, &(Loc))

#  else

#   define YYLOCATION_PRINT(File, Loc) ((void) 0)
    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT  YYLOCATION_PRINT

#  endif
# endif /* !defined YYLOCATION_PRINT */


# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value, Location); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  YY_USE (yylocationp);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  YYLOCATION_PRINT (yyo, yylocationp);
  YYFPRINTF (yyo, ": ");
  yy_symbol_value_print (yyo, yykind, yyvaluep, yylocationp);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)],
                       &(yylsp[(yyi + 1) - (yynrhs)]));
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, yylsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
{
  YY_USE (yyvaluep);
  YY_USE (yylocationp);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Location data for the lookahead symbol.  */
YYLTYPE yylloc
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

    /* The location stack: array, bottom, top.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls = yylsa;
    YYLTYPE *yylsp = yyls;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

  /* The locations where the error started and ended.  */
  YYLTYPE yyerror_range[3];



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  yylsp[0] = yylloc;
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;
        YYLTYPE *yyls1 = yyls;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yyls1, yysize * YYSIZEOF (*yylsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
        yyls = yyls1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
        YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      yyerror_range[1] = yylloc;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location. */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  yyerror_range[1] = yyloc;
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* program: class_declaration  */
#line 125 "miniJava.y"
    {
        (yyval.program_node) = create_program_node((yyvsp[0].class_decl_node), (yylsp[0]).first_line);
        root_ast_node = (yyval.program_node); // This assignment should now be type-correct
    }
#line 1372 "miniJava.tab.c"
    break;

  case 3: /* class_body_items: %empty  */
#line 133 "miniJava.y"
    {
        (yyval.parsed_class_contents_ptr) = (ParsedClassContents*)calloc(1, sizeof(ParsedClassContents));
        if (!(yyval.parsed_class_contents_ptr)) {
            yyerror("Memory allocation failed for class contents collector");
            YYABORT;
        }
        (yyval.parsed_class_contents_ptr)->main_method = NULL;
        (yyval.parsed_class_contents_ptr)->var_decls = NULL;
    }
#line 1386 "miniJava.tab.c"
    break;

  case 4: /* class_body_items: class_body_items variable_declaration TOKEN_SEMICOLON  */
#line 143 "miniJava.y"
    {
        (yyval.parsed_class_contents_ptr) = (yyvsp[-2].parsed_class_contents_ptr);
        (yyval.parsed_class_contents_ptr)->var_decls = create_variable_declaration_list_node((yyvsp[-1].var_decl_node), (yyval.parsed_class_contents_ptr)->var_decls);
        if (!(yyval.parsed_class_contents_ptr)->var_decls && (yyvsp[-1].var_decl_node)) {
             yyerror("Memory allocation failed for var_decl list node"); YYABORT;
        }
    }
#line 1398 "miniJava.tab.c"
    break;

  case 5: /* class_body_items: class_body_items main_method_declaration  */
#line 151 "miniJava.y"
    {
        (yyval.parsed_class_contents_ptr) = (yyvsp[-1].parsed_class_contents_ptr);
        if ((yyval.parsed_class_contents_ptr)->main_method != NULL) {
            yyerror("Error: Multiple main methods defined. Using the last one encountered.");
            // Consider: free_main_method_node($$->main_method);
        }
        (yyval.parsed_class_contents_ptr)->main_method = (yyvsp[0].main_method_node);
    }
#line 1411 "miniJava.tab.c"
    break;

  case 6: /* class_declaration: TOKEN_CLASS identifier TOKEN_LBRACE class_body_items TOKEN_RBRACE  */
#line 163 "miniJava.y"
    {
        if (!(yyvsp[-1].parsed_class_contents_ptr)->main_method) {
            yyerror("Error: Class must contain a main method.");
            // YYABORT; // Or handle by creating a default/error node
        }
        (yyval.class_decl_node) = create_class_declaration_node(
            (yyvsp[-3].identifier_node),
            (yyvsp[-1].parsed_class_contents_ptr)->main_method,
            (yyvsp[-1].parsed_class_contents_ptr)->var_decls,
            (yylsp[-4]).first_line
        );
        free((yyvsp[-1].parsed_class_contents_ptr));
    }
#line 1429 "miniJava.tab.c"
    break;

  case 7: /* main_method_declaration: TOKEN_PUBLIC TOKEN_STATIC TOKEN_VOID identifier TOKEN_LPAREN TOKEN_RPAREN block_statement_core  */
#line 180 "miniJava.y"
    {
        // Optional: Check if $4->name is "main"
        // if (strcmp($4->name, "main") != 0) {
        //     yyerror("Main method must be named 'main'");
        // }
        (yyval.main_method_node) = create_main_method_node((yyvsp[-3].identifier_node), (yyvsp[0].block_stmt_node), (yylsp[-6]).first_line);
    }
#line 1441 "miniJava.tab.c"
    break;

  case 8: /* statements: %empty  */
#line 191 "miniJava.y"
    { (yyval.statement_list_node) = NULL; }
#line 1447 "miniJava.tab.c"
    break;

  case 9: /* statements: statement statements  */
#line 193 "miniJava.y"
    { (yyval.statement_list_node) = create_statement_list_node((yyvsp[-1].statement_node), (yyvsp[0].statement_list_node)); }
#line 1453 "miniJava.tab.c"
    break;

  case 10: /* statement: variable_declaration TOKEN_SEMICOLON  */
#line 198 "miniJava.y"
    { (yyval.statement_node) = create_variable_declaration_statement((yyvsp[-1].var_decl_node), (yylsp[-1]).first_line); }
#line 1459 "miniJava.tab.c"
    break;

  case 11: /* statement: assignment_core TOKEN_SEMICOLON  */
#line 200 "miniJava.y"
    { (yyval.statement_node) = create_assignment_statement((yyvsp[-1].assignment_node), (yylsp[-1]).first_line); }
#line 1465 "miniJava.tab.c"
    break;

  case 12: /* statement: if_statement_core  */
#line 202 "miniJava.y"
    { (yyval.statement_node) = create_if_statement_wrapper_node((yyvsp[0].if_stmt_node), (yylsp[0]).first_line); }
#line 1471 "miniJava.tab.c"
    break;

  case 13: /* statement: while_statement_core  */
#line 204 "miniJava.y"
    { (yyval.statement_node) = create_while_statement_wrapper_node((yyvsp[0].while_stmt_node), (yylsp[0]).first_line); }
#line 1477 "miniJava.tab.c"
    break;

  case 14: /* statement: print_statement_core TOKEN_SEMICOLON  */
#line 206 "miniJava.y"
    { (yyval.statement_node) = create_print_statement_wrapper_node((yyvsp[-1].print_stmt_node), (yylsp[-1]).first_line); }
#line 1483 "miniJava.tab.c"
    break;

  case 15: /* statement: return_statement_core TOKEN_SEMICOLON  */
#line 208 "miniJava.y"
    { (yyval.statement_node) = create_return_statement_wrapper_node((yyvsp[-1].return_stmt_node), (yylsp[-1]).first_line); }
#line 1489 "miniJava.tab.c"
    break;

  case 16: /* statement: block_statement_core  */
#line 210 "miniJava.y"
    { (yyval.statement_node) = create_block_statement_wrapper_node((yyvsp[0].block_stmt_node), (yylsp[0]).first_line); }
#line 1495 "miniJava.tab.c"
    break;

  case 17: /* block_statement_core: TOKEN_LBRACE statements TOKEN_RBRACE  */
#line 215 "miniJava.y"
    { (yyval.block_stmt_node) = create_block_node((yyvsp[-1].statement_list_node), (yylsp[-2]).first_line); }
#line 1501 "miniJava.tab.c"
    break;

  case 18: /* variable_declaration: type identifier  */
#line 220 "miniJava.y"
    { (yyval.var_decl_node) = create_variable_declaration_node((yyvsp[-1].type_node), (yyvsp[0].identifier_node), NULL, 0, (yylsp[-1]).first_line); }
#line 1507 "miniJava.tab.c"
    break;

  case 19: /* variable_declaration: type identifier TOKEN_ASSIGN expression  */
#line 222 "miniJava.y"
    { (yyval.var_decl_node) = create_variable_declaration_node((yyvsp[-3].type_node), (yyvsp[-2].identifier_node), (yyvsp[0].expression_node), 0, (yylsp[-3]).first_line); }
#line 1513 "miniJava.tab.c"
    break;

  case 20: /* variable_declaration: TOKEN_FINAL type identifier TOKEN_ASSIGN expression  */
#line 224 "miniJava.y"
    { (yyval.var_decl_node) = create_variable_declaration_node((yyvsp[-3].type_node), (yyvsp[-2].identifier_node), (yyvsp[0].expression_node), 1, (yylsp[-4]).first_line); }
#line 1519 "miniJava.tab.c"
    break;

  case 21: /* type: TOKEN_INT  */
#line 229 "miniJava.y"
    { (yyval.type_node) = create_type_node(TYPE_INT, (yylsp[0]).first_line); }
#line 1525 "miniJava.tab.c"
    break;

  case 22: /* type: TOKEN_CHAR  */
#line 231 "miniJava.y"
    { (yyval.type_node) = create_type_node(TYPE_CHAR, (yylsp[0]).first_line); }
#line 1531 "miniJava.tab.c"
    break;

  case 23: /* type: TOKEN_BOOLEAN  */
#line 233 "miniJava.y"
    { (yyval.type_node) = create_type_node(TYPE_BOOLEAN, (yylsp[0]).first_line); }
#line 1537 "miniJava.tab.c"
    break;

  case 24: /* type: TOKEN_INT TOKEN_LBRACKET TOKEN_RBRACKET  */
#line 235 "miniJava.y"
    { (yyval.type_node) = create_type_node(TYPE_INT_ARRAY, (yylsp[-2]).first_line); }
#line 1543 "miniJava.tab.c"
    break;

  case 25: /* assignment_core: identifier TOKEN_ASSIGN expression  */
#line 240 "miniJava.y"
    { (yyval.assignment_node) = create_assignment_node((yyvsp[-2].identifier_node), NULL, (yyvsp[0].expression_node), (yylsp[-2]).first_line); }
#line 1549 "miniJava.tab.c"
    break;

  case 26: /* assignment_core: identifier TOKEN_LBRACKET expression TOKEN_RBRACKET TOKEN_ASSIGN expression  */
#line 242 "miniJava.y"
    { (yyval.assignment_node) = create_assignment_node((yyvsp[-5].identifier_node), (yyvsp[-3].expression_node), (yyvsp[0].expression_node), (yylsp[-5]).first_line); }
#line 1555 "miniJava.tab.c"
    break;

  case 27: /* if_statement_core: TOKEN_IF TOKEN_LPAREN expression TOKEN_RPAREN statement  */
#line 247 "miniJava.y"
    { (yyval.if_stmt_node) = create_if_node((yyvsp[-2].expression_node), (yyvsp[0].statement_node), NULL, (yylsp[-4]).first_line); }
#line 1561 "miniJava.tab.c"
    break;

  case 28: /* if_statement_core: TOKEN_IF TOKEN_LPAREN expression TOKEN_RPAREN statement TOKEN_ELSE statement  */
#line 249 "miniJava.y"
    { (yyval.if_stmt_node) = create_if_node((yyvsp[-4].expression_node), (yyvsp[-2].statement_node), (yyvsp[0].statement_node), (yylsp[-6]).first_line); }
#line 1567 "miniJava.tab.c"
    break;

  case 29: /* while_statement_core: TOKEN_WHILE TOKEN_LPAREN expression TOKEN_RPAREN statement  */
#line 254 "miniJava.y"
    { (yyval.while_stmt_node) = create_while_node((yyvsp[-2].expression_node), (yyvsp[0].statement_node), (yylsp[-4]).first_line); }
#line 1573 "miniJava.tab.c"
    break;

  case 30: /* print_statement_core: TOKEN_PRINT TOKEN_LPAREN expression TOKEN_RPAREN  */
#line 259 "miniJava.y"
    { (yyval.print_stmt_node) = create_print_node((yyvsp[-1].expression_node), (yylsp[-3]).first_line); }
#line 1579 "miniJava.tab.c"
    break;

  case 31: /* return_statement_core: TOKEN_RETURN  */
#line 264 "miniJava.y"
    { (yyval.return_stmt_node) = create_return_node(NULL, (yylsp[0]).first_line); }
#line 1585 "miniJava.tab.c"
    break;

  case 32: /* return_statement_core: TOKEN_RETURN expression  */
#line 266 "miniJava.y"
    { (yyval.return_stmt_node) = create_return_node((yyvsp[0].expression_node), (yylsp[-1]).first_line); }
#line 1591 "miniJava.tab.c"
    break;

  case 33: /* expression: logical_or_expression  */
#line 270 "miniJava.y"
                          { (yyval.expression_node) = (yyvsp[0].expression_node); }
#line 1597 "miniJava.tab.c"
    break;

  case 34: /* logical_or_expression: logical_and_expression  */
#line 274 "miniJava.y"
                           { (yyval.expression_node) = (yyvsp[0].expression_node); }
#line 1603 "miniJava.tab.c"
    break;

  case 35: /* logical_or_expression: logical_or_expression TOKEN_OR logical_and_expression  */
#line 275 "miniJava.y"
                                                          { (yyval.expression_node) = create_binary_expression_node((yyvsp[-2].expression_node), TOKEN_OR, (yyvsp[0].expression_node), (yylsp[-1]).first_line); }
#line 1609 "miniJava.tab.c"
    break;

  case 36: /* logical_and_expression: equality_expression  */
#line 279 "miniJava.y"
                        { (yyval.expression_node) = (yyvsp[0].expression_node); }
#line 1615 "miniJava.tab.c"
    break;

  case 37: /* logical_and_expression: logical_and_expression TOKEN_AND equality_expression  */
#line 280 "miniJava.y"
                                                         { (yyval.expression_node) = create_binary_expression_node((yyvsp[-2].expression_node), TOKEN_AND, (yyvsp[0].expression_node), (yylsp[-1]).first_line); }
#line 1621 "miniJava.tab.c"
    break;

  case 38: /* equality_expression: relational_expression  */
#line 284 "miniJava.y"
                          { (yyval.expression_node) = (yyvsp[0].expression_node); }
#line 1627 "miniJava.tab.c"
    break;

  case 39: /* equality_expression: equality_expression TOKEN_EQ relational_expression  */
#line 285 "miniJava.y"
                                                       { (yyval.expression_node) = create_binary_expression_node((yyvsp[-2].expression_node), TOKEN_EQ, (yyvsp[0].expression_node), (yylsp[-1]).first_line); }
#line 1633 "miniJava.tab.c"
    break;

  case 40: /* equality_expression: equality_expression TOKEN_NEQ relational_expression  */
#line 286 "miniJava.y"
                                                        { (yyval.expression_node) = create_binary_expression_node((yyvsp[-2].expression_node), TOKEN_NEQ, (yyvsp[0].expression_node), (yylsp[-1]).first_line); }
#line 1639 "miniJava.tab.c"
    break;

  case 41: /* relational_expression: additive_expression  */
#line 290 "miniJava.y"
                        { (yyval.expression_node) = (yyvsp[0].expression_node); }
#line 1645 "miniJava.tab.c"
    break;

  case 42: /* relational_expression: relational_expression TOKEN_LT additive_expression  */
#line 291 "miniJava.y"
                                                       { (yyval.expression_node) = create_binary_expression_node((yyvsp[-2].expression_node), TOKEN_LT, (yyvsp[0].expression_node), (yylsp[-1]).first_line); }
#line 1651 "miniJava.tab.c"
    break;

  case 43: /* relational_expression: relational_expression TOKEN_GT additive_expression  */
#line 292 "miniJava.y"
                                                       { (yyval.expression_node) = create_binary_expression_node((yyvsp[-2].expression_node), TOKEN_GT, (yyvsp[0].expression_node), (yylsp[-1]).first_line); }
#line 1657 "miniJava.tab.c"
    break;

  case 44: /* relational_expression: relational_expression TOKEN_LEQ additive_expression  */
#line 293 "miniJava.y"
                                                        { (yyval.expression_node) = create_binary_expression_node((yyvsp[-2].expression_node), TOKEN_LEQ, (yyvsp[0].expression_node), (yylsp[-1]).first_line); }
#line 1663 "miniJava.tab.c"
    break;

  case 45: /* relational_expression: relational_expression TOKEN_GEQ additive_expression  */
#line 294 "miniJava.y"
                                                        { (yyval.expression_node) = create_binary_expression_node((yyvsp[-2].expression_node), TOKEN_GEQ, (yyvsp[0].expression_node), (yylsp[-1]).first_line); }
#line 1669 "miniJava.tab.c"
    break;

  case 46: /* additive_expression: multiplicative_expression  */
#line 298 "miniJava.y"
                              { (yyval.expression_node) = (yyvsp[0].expression_node); }
#line 1675 "miniJava.tab.c"
    break;

  case 47: /* additive_expression: additive_expression TOKEN_PLUS multiplicative_expression  */
#line 299 "miniJava.y"
                                                             { (yyval.expression_node) = create_binary_expression_node((yyvsp[-2].expression_node), TOKEN_PLUS, (yyvsp[0].expression_node), (yylsp[-1]).first_line); }
#line 1681 "miniJava.tab.c"
    break;

  case 48: /* additive_expression: additive_expression TOKEN_MINUS multiplicative_expression  */
#line 300 "miniJava.y"
                                                              { (yyval.expression_node) = create_binary_expression_node((yyvsp[-2].expression_node), TOKEN_MINUS, (yyvsp[0].expression_node), (yylsp[-1]).first_line); }
#line 1687 "miniJava.tab.c"
    break;

  case 49: /* multiplicative_expression: unary_expression  */
#line 304 "miniJava.y"
                     { (yyval.expression_node) = (yyvsp[0].expression_node); }
#line 1693 "miniJava.tab.c"
    break;

  case 50: /* multiplicative_expression: multiplicative_expression TOKEN_MULTIPLY unary_expression  */
#line 305 "miniJava.y"
                                                              { (yyval.expression_node) = create_binary_expression_node((yyvsp[-2].expression_node), TOKEN_MULTIPLY, (yyvsp[0].expression_node), (yylsp[-1]).first_line); }
#line 1699 "miniJava.tab.c"
    break;

  case 51: /* multiplicative_expression: multiplicative_expression TOKEN_DIVIDE unary_expression  */
#line 306 "miniJava.y"
                                                            { (yyval.expression_node) = create_binary_expression_node((yyvsp[-2].expression_node), TOKEN_DIVIDE, (yyvsp[0].expression_node), (yylsp[-1]).first_line); }
#line 1705 "miniJava.tab.c"
    break;

  case 52: /* multiplicative_expression: multiplicative_expression TOKEN_MOD unary_expression  */
#line 307 "miniJava.y"
                                                         { (yyval.expression_node) = create_binary_expression_node((yyvsp[-2].expression_node), TOKEN_MOD, (yyvsp[0].expression_node), (yylsp[-1]).first_line); }
#line 1711 "miniJava.tab.c"
    break;

  case 53: /* unary_expression: primary_expression  */
#line 311 "miniJava.y"
                       { (yyval.expression_node) = (yyvsp[0].expression_node); }
#line 1717 "miniJava.tab.c"
    break;

  case 54: /* unary_expression: TOKEN_MINUS unary_expression  */
#line 313 "miniJava.y"
    { (yyval.expression_node) = create_unary_expression_node(TOKEN_MINUS, (yyvsp[0].expression_node), (yylsp[-1]).first_line); }
#line 1723 "miniJava.tab.c"
    break;

  case 55: /* unary_expression: TOKEN_NOT unary_expression  */
#line 315 "miniJava.y"
    { (yyval.expression_node) = create_unary_expression_node(TOKEN_NOT, (yyvsp[0].expression_node), (yylsp[-1]).first_line); }
#line 1729 "miniJava.tab.c"
    break;

  case 56: /* primary_expression: integer_literal  */
#line 320 "miniJava.y"
    { (yyval.expression_node) = create_literal_expression_node((yyvsp[0].literal_node), (yylsp[0]).first_line); }
#line 1735 "miniJava.tab.c"
    break;

  case 57: /* primary_expression: float_literal  */
#line 322 "miniJava.y"
    { (yyval.expression_node) = create_literal_expression_node((yyvsp[0].literal_node), (yylsp[0]).first_line); }
#line 1741 "miniJava.tab.c"
    break;

  case 58: /* primary_expression: char_literal  */
#line 324 "miniJava.y"
    { (yyval.expression_node) = create_literal_expression_node((yyvsp[0].literal_node), (yylsp[0]).first_line); }
#line 1747 "miniJava.tab.c"
    break;

  case 59: /* primary_expression: string_literal  */
#line 326 "miniJava.y"
    { (yyval.expression_node) = create_literal_expression_node((yyvsp[0].literal_node), (yylsp[0]).first_line); }
#line 1753 "miniJava.tab.c"
    break;

  case 60: /* primary_expression: TOKEN_TRUE  */
#line 328 "miniJava.y"
    { (yyval.expression_node) = create_boolean_literal_expression_node(1, (yylsp[0]).first_line); }
#line 1759 "miniJava.tab.c"
    break;

  case 61: /* primary_expression: TOKEN_FALSE  */
#line 330 "miniJava.y"
    { (yyval.expression_node) = create_boolean_literal_expression_node(0, (yylsp[0]).first_line); }
#line 1765 "miniJava.tab.c"
    break;

  case 62: /* primary_expression: identifier  */
#line 332 "miniJava.y"
    { (yyval.expression_node) = create_identifier_expression_node((yyvsp[0].identifier_node), (yylsp[0]).first_line); }
#line 1771 "miniJava.tab.c"
    break;

  case 63: /* primary_expression: identifier TOKEN_LBRACKET expression TOKEN_RBRACKET  */
#line 334 "miniJava.y"
    { (yyval.expression_node) = create_array_access_expression_node((yyvsp[-3].identifier_node), (yyvsp[-1].expression_node), (yylsp[-3]).first_line); }
#line 1777 "miniJava.tab.c"
    break;

  case 64: /* primary_expression: TOKEN_LPAREN expression TOKEN_RPAREN  */
#line 336 "miniJava.y"
    { (yyval.expression_node) = (yyvsp[-1].expression_node); }
#line 1783 "miniJava.tab.c"
    break;

  case 65: /* identifier: TOKEN_ID  */
#line 341 "miniJava.y"
    {
        (yyval.identifier_node) = create_identifier_node((yyvsp[0].string_val), (yylsp[0]).first_line);
    }
#line 1791 "miniJava.tab.c"
    break;

  case 66: /* integer_literal: TOKEN_INT_LIT  */
#line 347 "miniJava.y"
                  { (yyval.literal_node) = create_int_literal_node((yyvsp[0].int_val), (yylsp[0]).first_line); }
#line 1797 "miniJava.tab.c"
    break;

  case 67: /* float_literal: TOKEN_FLOAT_LIT  */
#line 351 "miniJava.y"
                    { (yyval.literal_node) = create_float_literal_node((yyvsp[0].float_val), (yylsp[0]).first_line); }
#line 1803 "miniJava.tab.c"
    break;

  case 68: /* char_literal: TOKEN_CHAR_LIT  */
#line 355 "miniJava.y"
                   { (yyval.literal_node) = create_char_literal_node((yyvsp[0].char_val), (yylsp[0]).first_line); }
#line 1809 "miniJava.tab.c"
    break;

  case 69: /* string_literal: TOKEN_STRING_LIT  */
#line 360 "miniJava.y"
    {
        (yyval.literal_node) = create_string_literal_node((yyvsp[0].string_val), (yylsp[0]).first_line);
    }
#line 1817 "miniJava.tab.c"
    break;


#line 1821 "miniJava.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  yyerror_range[1] = yylloc;
  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, &yylloc);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp, yylsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  ++yylsp;
  YYLLOC_DEFAULT (*yylsp, yyerror_range, 2);

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp, yylsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 365 "miniJava.y"


// Standard Bison error reporting function
void yyerror(const char *s) {
    fprintf(stderr, "Syntax Error on line %d near token '%s': %s\n", yylineno, yytext, s);
}

// The main function is typically in a separate file (e.g., main.c or your driver file)
// to keep the parser file focused on grammar.
/*
int main(int argc, char **argv) {
    if (argc > 1) {
        yyin = fopen(argv[1], "r");
        if (!yyin) {
            perror(argv[1]);
            return 1;
        }
    } else {
        yyin = stdin; // Default to stdin if no file is provided
    }

    int parse_result = yyparse(); // Call the parser

    if (yyin != stdin) {
        fclose(yyin);
    }

    if (parse_result == 0) { // 0 indicates success
        printf("Parsing successful!\n");
        if (root_ast_node) {
            printf("AST Root created.\n");
            // Here you would typically call functions for semantic analysis,
            // code generation, printing the AST, etc.
            // For example: print_ast(root_ast_node, 0);
            // And finally, free the AST: free_ast(root_ast_node);
        } else {
            printf("Parsing successful, but no AST root was created (grammar issue?).\n");
        }
        return 0;
    } else { // Non-zero indicates failure
        printf("Parsing failed.\n");
        return 1;
    }
}
*/
