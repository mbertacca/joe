# ifndef joetoken_h
# define joetoken_h  "$Id$"
# include "joearray.h"

enum TokenType {
   _COMMA_ = ',',
   _BANG_ = '!',
   _SEMICOLON_ = ';',
   _COLON_ = ':',
   _DOT_ = '.',
   _PAR_OPEN_ = '(',
   _PAR_CLOSE_ = ')',
   _BRACE_OPEN_ = '{',
   _BRACE_CLOSE_ = '}',

   _INIT = 1000,
   _BANGBANG_,
   _WORD,
   _STRING,
   _UNCLOSED_STRING,
   _INTEGER,
   _FLOAT,
   _DECIMAL,
   _ASSIGN,
   _ML_COMMENT,
   _COMMENT,
   _TRUE,
   _FALSE
};

typedef struct t_Token *Token;
typedef struct t_Token {
   enum TokenType type;
   char *word;
   int row; 
   int col; 
} *Token;

typedef struct t_Tokenizer *Tokenizer;
extern Tokenizer Tokenizer_new (JoeArray tokens);
extern void Tokenizer_delete (Tokenizer self);
extern void Tokenizer_tokenize (Tokenizer self, char *line);

# endif
