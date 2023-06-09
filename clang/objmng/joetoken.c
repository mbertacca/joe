/*
 * This source file is part of the "Joe Objects Executor" open source project
 *
 * Copyright 2017 Marco Bertacca (www.bertacca.eu)
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include "joetoken.h"
# include "joestrbuild.h"

typedef struct t_Tokenizer {
   JoeArray tokens;
   JoeStrBuild word;
   enum TokenType status;
   int cRow;
   int cCol;
} *Tokenizer;

Tokenizer
Tokenizer_new (JoeArray tokens)
{
  Tokenizer self = calloc (1, sizeof (struct t_Tokenizer));
  self->tokens = tokens;
  self->word = JoeStrBuild_new();
  self->status = _INIT;
  self->cRow = 0;
  self->cCol = 0;

  return self;
}

void
Tokenizer_delete (Tokenizer self)
{
   JoeStrBuild_delete (self->word);
   free (self);
}

typedef int JOE_BOOL;
# define JOE_TRUE 1
# define JOE_FALSE 0

static void
newToken (Tokenizer self, enum TokenType type)
{
   struct t_Token token;
   token.type = type;
   token.word = JoeStrBuild_toString (self->word);
   token.row = self->cRow; 
   token.col = self->cCol; 
   JoeArray_add (self->tokens, &token);
   JoeStrBuild_clean (self->word);
}

static JOE_BOOL
breakChar (Tokenizer self, char c)
{
   JOE_BOOL Return = JOE_TRUE;
   switch (self->status) {
   case _INIT:
      break;
   case _INTEGER:
      newToken(self, _INTEGER);
      self->status = _INIT;
      break;
   case _FLOAT:
      newToken(self, _FLOAT);
      self->status = _INIT;
      break;
   case _WORD:
      newToken(self, _WORD);
      self->status = _INIT;
      break;
   case _STRING:
      JoeStrBuild_appendChr (self->word, c);
      Return = JOE_FALSE;
      break;
   case _ML_COMMENT:
      Return = JOE_FALSE;
      break;
   default:
      /* Internal error */
      break;
   }
   return Return;
}

void
Tokenizer_tokenize (Tokenizer self, char *line)
{
   int lineLen = strlen (line);
   char *c;

   self->cRow++;
   for (c = line; *c ; c++) {
      self->cCol = c - line + 1;
      switch (*c) {
      case ' ':
      case '\t':
      case '\r':
      case '\n':
         breakChar (self, *c);
         break;
      case '!':
         if (breakChar(self, *c)) {
            if (*(c+1) == '!') {
               JoeStrBuild_appendStr (self->word, "!!");
               newToken(self, _BANGBANG_);
               c++;
            } else {
               JoeStrBuild_appendChr (self->word, *c);
               newToken(self, _BANG_);
            }
         }
         break;
      case '*':
         switch (self->status) {
         case _ML_COMMENT:
            if (*(++c) == '/')
               self->status = _INIT;
            else
               --c;
            break;
         case _STRING:
            JoeStrBuild_appendChr (self->word, *c);
            break;
         case _INIT:
         case _WORD:
         case _INTEGER:
         case _FLOAT:
            if (*(++c) == '>') {
               c = &line[lineLen - 1];
            } else {
               --c;
               if (breakChar (self, *c)) {
                  JoeStrBuild_appendStr (self->word, "multiply");
                  newToken(self, _WORD);
               }
            }
            break;
         default:
            /* Internal error */
            break;
         }
         break;
      case '/':
         if (breakChar(self, *c)) {
            if (*(++c) == '*') {
               self->status = _ML_COMMENT;
            } else {
               c--;
               JoeStrBuild_appendStr (self->word, "divide");
               newToken(self, _WORD);
            }
         }
         break;
      case '=':
         if (breakChar(self, *c)) {
            JoeStrBuild_appendStr (self->word, "equals");
            newToken(self, _WORD);
         }
         break;
      case '<':
         if (breakChar(self, *c)) {
            ++c;
            if (*c == '=') {
               JoeStrBuild_appendStr (self->word, "le");
               newToken(self, _WORD);
            } else if (*c == '>') {
               JoeStrBuild_appendStr (self->word, "ne");
               newToken(self, _WORD);
            } else if (*c == '0' && *(c + 1) == '>') {
                c++;
                JoeStrBuild_appendStr(self->word, "<0>");
                newToken(self, _FALSE);
            } else if (*c == '1' && *(c + 1) == '>') {
                c++;
                JoeStrBuild_appendStr(self->word, "<0>");
                newToken(self, _TRUE);
            } else {
               --c;
               JoeStrBuild_appendStr (self->word, "lt");
               newToken(self, _WORD);
            }
         }
         break;
      case '>':
         if (breakChar(self, *c)) {
            ++c;
            if (*c == '=') {
               JoeStrBuild_appendStr (self->word, "ge");
            } else {
               --c;
               JoeStrBuild_appendStr (self->word, "gt");
            }
            newToken(self, _WORD);
         }
         break;
      case ':':
         if (breakChar(self, *c)) {
            ++c;
            if (*c == '=') {
               JoeStrBuild_appendStr (self->word, ":=");
               newToken(self, _ASSIGN);
            } else {
               --c;
               JoeStrBuild_appendChr (self->word, *c);
               newToken(self, _COLON_);
            }
         }
         break;
      case '-':
         if (breakChar(self, *c)) {
            ++c;
            if (*c >= '0' && *c <= '9') {
               --c;
               self->status = _INTEGER;
               JoeStrBuild_appendChr (self->word, *c);
            } else {
               --c;
               JoeStrBuild_appendStr (self->word, "subtract");
               newToken(self, _WORD);
            }
         }
         break;
      case '+':
         if (breakChar(self, *c)) {
            JoeStrBuild_appendStr (self->word, "add");
            newToken(self, _WORD);
         }
         break;
      case '%':
         if (breakChar(self, *c)) {
            JoeStrBuild_appendStr (self->word, "remainder");
            newToken(self, _WORD);
         }
         break;
      case '(':
         if (breakChar(self, *c)) {
            ++c;
            if (*c == ')') {
               JoeStrBuild_appendStr(self->word, "()");
               newToken(self, _NULL);
            } else {
               c--;
               JoeStrBuild_appendChr (self->word, *c);
               newToken(self, _PAR_OPEN_);
            }
         }
         break;
      case ')':
         if (breakChar(self, *c)) {
            JoeStrBuild_appendChr (self->word, *c);
            newToken(self, _PAR_CLOSE_);
         }
         break;
      case '{':
         if (breakChar(self, *c)) {
            JoeStrBuild_appendChr (self->word, *c);
            newToken(self, _BRACE_OPEN_);
         }
         break;
      case '}':
         if (breakChar(self, *c)) {
            JoeStrBuild_appendChr (self->word, *c);
            newToken(self, _BRACE_CLOSE_);
         }
         break;
      case ',':
         if (breakChar(self, *c)) {
            JoeStrBuild_appendChr (self->word, *c);
            newToken(self, _COMMA_);
         }
         break;
      case ';':
         if (breakChar(self, *c)) {
            JoeStrBuild_appendChr (self->word, *c);
            newToken(self, _SEMICOLON_);
         }
         break;
      case '.':
         ++c;
         if (self->status == _INTEGER && *c >= '0' && *c <= '9') {
            c--;
            JoeStrBuild_appendChr (self->word, *c);
            self->status = _FLOAT;
         } else {
            c--;
            if (breakChar(self, *c)) {
               JoeStrBuild_appendChr (self->word, *c);
               newToken(self, _DOT_);
            }
         }
         break;

      case '"':
         switch (self->status) {
         case _ML_COMMENT:
            break;
         case _INTEGER:
         case _FLOAT:
         case _WORD:
            newToken(self, self->status);
            // PASSTHRU
         case _INIT:
            self->status = _STRING;
            break;
         case _STRING:
            if (*(++c) == '"') {
               JoeStrBuild_appendChr (self->word, *c);
            } else {
               c--;
               newToken(self, _STRING);
               self->status = _INIT;
            }
            break;
         default:
            /* Internal error */
            break;
         }
         break;
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
         switch (self->status) {
         case _ML_COMMENT:
            break;
         case _INIT:
            self->status = _INTEGER;
            JoeStrBuild_appendChr (self->word, *c);
            break;
         case _INTEGER:
         case _FLOAT:
         case _STRING:
         case _WORD:
            JoeStrBuild_appendChr (self->word, *c);
            break;
         default:
            /* Internal error */
            break;
         }
         break;
      case 'e':
      case 'E':
      case 'm':
      case 'M':
         if (self->status == _INTEGER || self->status == _FLOAT) {
            if (*c == 'e' || *c == 'E') {
               char *cp1 = c + 1;
               if (*cp1 >= '0' && *cp1 <= '9') {
                  JoeStrBuild_appendChr(self->word, 'e');
                  self->status = _FLOAT;
                  break;
               }
            } else {
               newToken (self,_DECIMAL);
               self->status = _INIT;
               break;
            }
         }
         // PASSTHRU
      default:
         switch (self->status) {
         case _ML_COMMENT:
            break;
         case _INIT:
            if (c == line && *c == '#' && lineLen > 1
                          && line[1] == '!') {
               c = &line[lineLen - 1];
            } else {
               JoeStrBuild_clean (self->word);
               JoeStrBuild_appendChr (self->word, *c);
               self->status = _WORD;
            }
            break;
         case _INTEGER:
         case _FLOAT:
            breakChar(self, '\0');
            JoeStrBuild_clean (self->word);
            JoeStrBuild_appendChr (self->word, *c);
            self->status = _WORD;
            break;
         case _STRING:
         case _WORD:
            JoeStrBuild_appendChr (self->word, *c);
            break;
         default:
            /* Internal error */
            break;
         }
         break;
      }
   }
   switch (self->status) {
   default:
   case _ML_COMMENT:
   case _INIT:
      break;
   case _INTEGER:
   case _FLOAT:
   case _WORD:
      newToken(self, self->status);
      self->status = _INIT;
      break;
   case _STRING:
      newToken(self, _UNCLOSED_STRING);
      self->status = _INIT;
      break;
   }
}
