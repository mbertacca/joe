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

# include "joeparser.h"
# include "joetoken.h"
# include "joestrbuild.h"
# include "joearrayscan.h"
# include "joe_Assignment.h"
# include "joe_Bang.h"
# include "joe_String.h"
# include "joe_Integer.h"
# include "joe_Float.h"
# include "joe_Variable.h"
# include "joe_Exception.h"
# include <stdlib.h>

# define pop(t) ((Token)JoeArrayScan_next(t))
# define push(t) ((Token)JoeArrayScan_prev(t))

struct t_JoeParser
{
   joe_Object command;
   joe_Object exception;
   int row;
   int col;
   char *fileName;
};

joe_Object message(JoeParser self,joe_Block block,JoeArrayScan tokens,Token tk);

static int
peek (JoeArrayScan tokens)
{
   Token tk = (Token) JoeArrayScan_peek(tokens);
   if (tk)
      return tk->type;
   else
      return 0;
}

static void
exception (JoeParser self, char *msg, JoeArrayScan tokens, Token tk)
{
   if (self->exception == 0) {
      JoeStrBuild fullMsg = JoeStrBuild_new ();
      JoeStrBuild_appendStr (fullMsg, msg);
      if (tk)
         JoeStrBuild_appendStr (fullMsg, tk->word);
      else
         JoeStrBuild_appendStr (fullMsg, "<end-of-file>");
      JoeStrBuild_appendStr (fullMsg, ", file=");
      JoeStrBuild_appendStr (fullMsg, self->fileName);
      if (tk) {
         JoeStrBuild_appendStr (fullMsg, ", line=");
         JoeStrBuild_appendInt (fullMsg, tk->row);
         JoeStrBuild_appendStr (fullMsg, ", col~");
         JoeStrBuild_appendInt (fullMsg, tk->col);
      }
      self->exception = joe_Exception_New (JoeStrBuild_toString (fullMsg));
      JoeStrBuild_delete (fullMsg);
   }
   JoeArrayScan_clean (tokens);
}

int
unexpectedToken (JoeParser self, JoeArrayScan tokens, Token tk)
{
   exception (self,  "Unexpected token ", tokens, tk);
   return JOE_FAILURE;
}


joe_Object
getString (char *str)
{
   return joe_String_New (str);
}

joe_Object
getInteger (char *intgr)
{
   return joe_Integer_New (atol(intgr));
}

joe_Object
getFloat (char *intgr)
{
   return joe_Float_New (atof(intgr));
}

int
braceClose (JoeParser self, JoeArrayScan tokens)
{
   Token tk = pop(tokens);
   if (tk == 0 || tk->type != _BRACE_CLOSE_) {
       exception (self,  "Expected close brace found ", tokens, tk);
       return JOE_FAILURE;
   }
   return JOE_SUCCESS;
}

int
parClose (JoeParser self, JoeArrayScan tokens)
{
   Token tk = pop(tokens);
   if (tk == 0 || tk->type != _PAR_CLOSE_) {
       exception (self,  "Expected close parenthesis found ", tokens, tk);
       return JOE_FAILURE;
   }
   return JOE_SUCCESS;
}


joe_Object
getValue (JoeParser self, Token tk) {

   switch (tk->type) {
   case _BANG_:
      return self->command;
   case _STRING:
      return getString(tk->word);
   case _INTEGER:
      return getInteger(tk->word);
   case _FLOAT:
      return getFloat(tk->word);
   case _BANGBANG_:
   case _WORD:
      return joe_Variable_New(tk->word);
   default:
      return 0;
   }
}

static void compile (JoeParser self, joe_Block block, JoeArrayScan tokens);

static joe_Block
newBlock (JoeParser self, JoeArrayScan tokens, joe_Block parent)
{
   joe_Block block = joe_Block_New (parent);
   compile (self, block, tokens);

   return block;
}

joe_Message
arguments (JoeParser self, joe_Block block, JoeArrayScan tokens,
           joe_Object receiver, Token selector)
{
   joe_Message Return = 0;
   joe_Object obj;
   Token tk;
   JoeArray args = JoeArray_new (sizeof (joe_Object), 16);
   int argsLen;

    for ( ; ; ) {
       tk = pop (tokens);
       if (tk != 0 && (obj = getValue(self, tk)) == 0) {
         switch (tk->type) {

         case _DOT_:
            push(tokens);
            // PASSTHRU
         case _SEMICOLON_:
            goto exitFor;
         case _PAR_CLOSE_:
         case _BRACE_CLOSE_:
            push(tokens);
            goto exitFor;
         case _PAR_OPEN_:
            if (peek(tokens) == _PAR_CLOSE_) {
               pop(tokens);
               goto exitFor;
            } else {
               Token tk1 = pop (tokens);
               joe_Object msg = message (self, block, tokens, tk1);
               JoeArray_add (args, &msg);
               parClose (self, tokens);
            }
            break;
         case _BRACE_OPEN_:
            obj = newBlock (self, tokens, block);
            JoeArray_add (args, &obj);
            braceClose (self, tokens);
            break;
         default:
            unexpectedToken (self, tokens, tk);
         }
      } else {
         JoeArray_add (args, &obj);
      }
      tk = pop(tokens);
      if (tk == 0) {
         goto exitFor;
      } else if (tk->type != _COMMA_) {
         push(tokens);
         goto exitFor;
      }
   }
exitFor:

   argsLen = JoeArray_length (args);
   Return = joe_Message_New ((joe_Object) receiver, selector->word,
                             argsLen, (joe_Object *)JoeArray_getMem (args),
                             self->fileName, selector->row, selector->col);
/*
   if (args.size() == 0) {
      Return = new ExecMessage (receiver, selector, null, fName);
   } else {
      Object argsAr[] = args.toArray();
      Return = new ExecMessage (receiver, selector, argsAr, fName);
   }
*/
   tk = pop(tokens);
   if (tk != 0) {
      switch (tk->type) {
      case _WORD:
         Return = arguments (self, block, tokens, Return, tk);
         break;
      case _DOT_:
      case _SEMICOLON_:
         break;
      case _BRACE_CLOSE_:
      case _PAR_CLOSE_:
         push(tokens);
         break;
      default:
         unexpectedToken (self, tokens, tk);
      }
   }

   JoeArray_delete (args);
   return Return;
}

joe_Object
message (JoeParser self, joe_Block block, JoeArrayScan tokens, Token tk)
{
   joe_Object receiver = 0;

   if ((receiver = getValue(self, tk)) == 0) {
     switch (tk->type) {
      case _PAR_OPEN_:
         receiver = message (self, block, tokens, pop(tokens));
         parClose(self, tokens);
         break;
      case _BRACE_OPEN_:
         receiver = newBlock (self, tokens, block);
         braceClose(self, tokens);
         break;
      case _DOT_:
         return 0;
      default:
         unexpectedToken (self, tokens, tk);
      }
   }

   tk = pop(tokens);
   if (tk != 0) {
      switch (tk->type) {
      case _WORD:
         return arguments (self, block, tokens, receiver, tk);
      case _PAR_CLOSE_:
      case _BRACE_CLOSE_:
         push (tokens);
         //PASSTHRU
      case _SEMICOLON_:
      case _DOT_:
         return receiver;
      default:
         unexpectedToken (self, tokens, tk);
      }
   }
   return (joe_Object) receiver;
}

static void
assignment (JoeParser self, joe_Block block, JoeArrayScan tokens)
{
   Token tk = pop (tokens);
   joe_Message msg;

   if (peek(tokens) == _ASSIGN) {
      pop(tokens);
      if (tk->type == _WORD) {
/*
          joe_Object args[2];
          args[0] = joe_String_New (tk->word);
          args[1] = message (self, block, tokens, pop(tokens));
          msg = joe_Message_New (block, "assign",
                                 2, args, self->fileName, tk->row, tk->col);
*/
          joe_Object args[3];
          args[0] = 0;
          args[1] = joe_String_New (tk->word);
          args[2] = message (self, block, tokens, pop(tokens));
          msg = joe_Message_New (joe_Assignment_New(), ":=",
                                 3, args, self->fileName, tk->row, tk->col);
          joe_Block_addMessage (block, msg);
      } else {
         unexpectedToken (self, tokens, tk);
      }
   } else {
      msg = message (self, block, tokens, tk);
      joe_Block_addMessage (block, msg);
   }
}

void
setBlockArguments (JoeParser self, joe_Block block, JoeArrayScan tokens)
{
   Token tk;
   while ((tk = pop(tokens)) != 0 && tk->type == _WORD) {
      joe_Block_addArgName (block, tk->word);
      if ((tk = pop(tokens)) != 0 && tk->type != _COMMA_)
         break;
   }
   if (tk->type != _DOT_)
      unexpectedToken (self, tokens, tk);
}

static void
compile (JoeParser self, joe_Block block, JoeArrayScan tokens)
{
   Token tk;

   if (peek(tokens) == _WORD) {
      Token name = pop (tokens);
      if (peek(tokens) == _COLON_) {
         pop (tokens);
         setBlockArguments (self, block, tokens);
         joe_Block_setName (block, name->word);
/*
      } else if (peek(tokens) == _DOT_) {
         joe_Block_setName (block, name->word);
         push(tokens);
*/
      } else {
         push(tokens);
      }
   } else if (peek(tokens) == _COLON_) {
      pop (tokens);
      setBlockArguments (self, block, tokens);
   }
   while ((tk = JoeArrayScan_peek(tokens)) && tk->type != _BRACE_CLOSE_) {
      assignment (self, block, tokens);
   }
}
joe_Object
JoeParser_getException (JoeParser self)
{
   return self->exception;
}

int
JoeParser_compile (JoeParser self, joe_Block block, JoeArray tks) {
   JoeArrayScan tokens = JoeArrayScan_new (tks);
   compile (self, block, tokens);
   JoeArrayScan_delete (tokens);
   if (self->exception == 0)
      return JOE_SUCCESS;
   else
      return JOE_FAILURE;
}

JoeParser
JoeParser_new (char *fileName)
{
   JoeParser self = calloc (1, sizeof (struct t_JoeParser));
   joe_Object_assign (&self->command, joe_Bang_New ());
   self->fileName = fileName;
   
   return self;
}
void
JoeParser_delete (JoeParser self)
{
   joe_Object_assign (&self->command, 0);
   free (self);
}
