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
# include "joe_BigDecimal.h"
# include "joe_Boolean.h"
# include "joe_Variable.h"
# include "joe_Exception.h"
# include "joe_Null.h"
# include "joe_Selector.h"
# include <stdio.h>
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
getFloat(char* intgr)
{
    return joe_Float_New(atof(intgr));
}

joe_Object
getDecimal (char* num)
{
    return joe_BigDecimal_New_str(num);
}

joe_Object
getTrue()
{
    return joe_Boolean_New_true();
}

joe_Object
getFalse()
{
    return joe_Boolean_New_false();
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
   case _DECIMAL:
       return getDecimal(tk->word);
   case _TRUE:
       return getTrue();
   case _FALSE:
       return getFalse();
   case _NULL:
       return joe_Null_value;
   case _BANGBANG_:
   case _WORD:
      return joe_Variable_New(tk->word);
   default:
      return 0;
   }
}

static void addObjToRpn (JoeArray rpn, joe_Object obj)
{
   joe_Object item = 0;
   joe_Object_assign (&item, obj);
   JoeArray_add (rpn, &item);
}

static void clrRpnArray (JoeArray rpn)
{

   int rpnc = JoeArray_length (rpn);
   int i;
   for (i = 0; i < rpnc; i++)
      joe_Object_assign ((joe_Object*)JoeArray_get (rpn, i), 0);
   JoeArray_clear (rpn);
}

static void compile (JoeParser self, joe_Block block, JoeArrayScan tokens);

static void
newBlock (JoeParser self, JoeArrayScan tokens, joe_Block parent, joe_Block *block)
{
   joe_Object_assign(block, joe_Block_New (parent));
   compile (self, *block, tokens);
}

static void parseReceiver (JoeParser self, joe_Block block,
                                 JoeArray rpn, JoeArrayScan tokens);

static void
parseArguments (JoeParser self, joe_Block block, JoeArray rpn, JoeArrayScan tokens, Token tk)
{
   char *selectorName = tk->word;
   joe_Object obj = 0;
   joe_Selector selector;
   int argc = 0;

   for ( ; ; ) {
      obj = 0;
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
               addObjToRpn (rpn, joe_Null_value);
            } else {
               parseReceiver (self, block, rpn, tokens);
               parClose (self, tokens);
            }
            argc++;
            break;
         case _BRACE_OPEN_:
            newBlock (self, tokens, block, &obj);
            braceClose (self, tokens);
            addObjToRpn (rpn, obj);
            joe_Object_assign (&obj, 0);
            argc++;
            break;
         default:
            unexpectedToken (self, tokens, tk);
         }
      } else {
         if (obj) {
            addObjToRpn (rpn, obj);
            argc++;
         }
      }
      tk = pop(tokens);
      if (tk == 0) {
         break;
      } else if (tk->type != _COMMA_) {
         push(tokens);
         break;
      }
   }
exitFor:
   selector = joe_Selector_New (selectorName, argc);
   addObjToRpn (rpn, selector);
   return;
}

static void
parseSelector (JoeParser self, joe_Block block, JoeArray rpn, JoeArrayScan tokens)
{
   Token tk;

   do {
      tk = pop(tokens);
      if (tk != 0) {
         switch (tk->type) {
         case _WORD:
            parseArguments (self, block, rpn, tokens, tk);
            break;
         case _PAR_CLOSE_:
         case _BRACE_CLOSE_:
            push (tokens);
            //PASSTHRU
         case _SEMICOLON_:
         case _DOT_:
            return;
         default:
            unexpectedToken (self, tokens, tk);
            return;
         }
      }
   } while (tk);
}

static void
parseReceiver (JoeParser self, joe_Block block, JoeArray rpn, JoeArrayScan tokens)
{
   Token tk = pop(tokens);
   joe_Object receiver = 0;
   joe_Block blockRcvr = 0;

   if ((receiver = getValue(self, tk)) == 0) {
     switch (tk->type) {
      case _PAR_OPEN_:
         parseReceiver (self, block, rpn, tokens);
         parClose(self, tokens);
         parseSelector (self, block, rpn, tokens);
         return;
      case _BRACE_OPEN_:
         newBlock (self, tokens, block, &blockRcvr);
         braceClose(self, tokens);
         break;
      case _DOT_:
         push (tokens);
         receiver = joe_Null_value;
         break;
      default:
         unexpectedToken (self, tokens, tk);
      }
   }
   if (blockRcvr) {
      addObjToRpn (rpn, blockRcvr);
      joe_Object_assign (&blockRcvr, 0);
   } else {
      addObjToRpn (rpn, receiver);
   }
   parseSelector (self, block, rpn, tokens);
   return;
}

static void
parseStart (JoeParser self, joe_Block block, JoeArray rpn, JoeArrayScan tokens)
{
   Token tk = pop (tokens);
   // joe_Message msg;
   joe_Variable assignee = 0;

   if (peek(tokens) == _ASSIGN) {
      pop(tokens);
      if (tk->type == _WORD) {
         joe_Object_assign (&assignee, joe_Variable_New (tk->word));
         parseReceiver (self, block, rpn, tokens);
         joe_Block_addMessage (block,
                   joe_Message_New (assignee, JoeArray_length(rpn),JoeArray_getMem(rpn),
                                    self->fileName, tk->row, tk->col));
         joe_Object_assign (&assignee, 0);
/*
          args[2] = message (self, block, tokens, pop(tokens));
          msg = joe_Message_New (joe_Assignment_New(), ":=",
                                 3, args, self->fileName, tk->row, tk->col);
          joe_Block_addMessage (block, msg);
*/
      } else {
         unexpectedToken (self, tokens, tk);
      }
   } else if (tk) {
      push(tokens);
      parseReceiver (self, block, rpn, tokens);
      joe_Block_addMessage (block,
                            joe_Message_New (0, JoeArray_length(rpn),JoeArray_getMem(rpn),
                                             self->fileName, tk->row, tk->col));
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
   JoeArray rpn = JoeArray_new (sizeof(joe_Object), 16);

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
      parseStart (self, block, rpn, tokens);
      clrRpnArray (rpn);
   }
   JoeArray_delete (rpn);
}
joe_Object
JoeParser_getException (JoeParser self)
{
   joe_Object Return = self->exception;
   self->exception = 0;
   return Return;
}

int
JoeParser_compile (JoeParser self, joe_Block block, JoeArray tks) {
   JoeArrayScan tokens = JoeArrayScan_new (tks);
   self->command = joe_Block_getBang (block);
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
   self->fileName = fileName;
   
   return self;
}
void
JoeParser_delete (JoeParser self)
{
   free (self);
}
