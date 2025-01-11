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

# include "joe_Execute.h"
# include "joeparser.h"
# include "joetoken.h"
# include "joe_Exception.h"
# include "joe_Memory.h"
# include "joe_String.h"
# include "joe_Block.h"
# include "joe_Null.h"
# include "joestrct.h"

# include <stdlib.h>

typedef struct s_Execute {
   JoeArray tokens;
   Tokenizer tokenizer;
   int compiled;
} *Execute;

# define DATA 0
# define BLOCK 1
# define NAME 2
static char *variables[] = { "data", "block", "name", 0 };

static int
init (joe_Object self, joe_Block block, joe_String name, joe_Bang bang)
{
   joe_Memory mem = joe_Memory_New (sizeof (struct s_Execute));
   Execute exec = (void *) joe_Object_getMem(mem);

   exec->tokens = 0;
   exec->tokenizer = 0;
   
   joe_Object_assign (JOE_AT(self, DATA), mem);
   joe_Object_assign (JOE_AT(self, BLOCK), joe_Block_New (block, bang));
   joe_Object_assign (JOE_AT(self,NAME), name);
    
   return JOE_SUCCESS;
}

static int
ctor (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   joe_Block block = 0;
   joe_String name = 0;
   joe_Bang bang = 0;
   int syntaxError = JOE_FALSE;

   if (argc > 0) {
      if (joe_Object_instanceOf(argv[0], &joe_Block_Class)) {
         block = argv[0];
      } else if (!joe_Object_instanceOf(argv[0], &joe_Null_Class)) {
         syntaxError = JOE_TRUE;
      }
      if (argc > 1) {
         if (joe_Object_instanceOf(argv[1], &joe_String_Class)) {
            name = argv[1];
         } else if (joe_Object_instanceOf(argv[1], &joe_Bang_Class)) {
            bang = argv[1];
            if (argc == 3) {
              if (joe_Object_instanceOf(argv[2], &joe_String_Class)) {
                  name = argv[2];
               } else {
                  syntaxError = JOE_TRUE;
               }
            } else if (argc > 3) {
               syntaxError = JOE_TRUE;
            }
         } else {
            syntaxError = JOE_TRUE;
         }
      }
   }
   if (syntaxError) {
      joe_Object_assign(retval,
                        joe_Exception_New ("execute ctor: invalid argument"));
      return JOE_FAILURE;
   }
   if (name == 0)
      return init (self, block, joe_String_New ("<execute>"), bang);
   else
      return init (self, block, name, bang);
}


static int
add (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1 && JOE_ISCLASS(argv[0], &joe_String_Class)) {
      joe_Memory mem = (void *) *JOE_AT(self, DATA);
      Execute exec = (void *) joe_Object_getMem(mem);
      char *line = joe_String_getCharStar (argv[0]);
      if (!exec->tokenizer) {
         exec->tokens = JoeArray_new (sizeof (struct t_Token), 8);
         exec->tokenizer = Tokenizer_new (exec->tokens);
      }
      Tokenizer_tokenize (exec->tokenizer, line);
      joe_Object_assign(retval, self);
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval,
                        joe_Exception_New ("execute add: invalid argument"));
      return JOE_FAILURE;
   }
}

static int
clear (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   joe_Memory mem = (void *) *JOE_AT(self, DATA);
   Execute exec = (void *) joe_Object_getMem(mem);
   joe_Block block =  *JOE_AT(self, BLOCK);
   if (exec->tokenizer) {
      int i;
      for (i = JoeArray_length(exec->tokens) - 1; i >= 0; i--)
         free (((struct t_Token*) JoeArray_get(exec->tokens, i))->word);
      JoeArray_delete (exec->tokens);
      Tokenizer_delete (exec->tokenizer);
      exec->tokens = 0;
      exec->tokenizer = 0;
   }
   joe_Block_removeMessages(block);
   exec->compiled = 0;
   joe_Object_assign(retval, self);
   return JOE_SUCCESS;
}

static int
exec (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   joe_Memory mem = (void *) *JOE_AT(self, DATA);
   Execute exec = (void *) joe_Object_getMem(mem);
   joe_Block block =  *JOE_AT(self, BLOCK);
   joe_String name = *JOE_AT(self, NAME);

   if (!exec->compiled) {
      int rc;
      JoeParser parser = JoeParser_new (joe_String_getCharStar(name));
      int tklen = JoeArray_length(exec->tokens);
      if (tklen > 0 &&
          ((struct t_Token*) JoeArray_get(exec->tokens, 0))->type == _COLON_) {

         joe_Object_assign(retval,
                      joe_Exception_New ("execute exec: syntax not permitted"));
         return JOE_FAILURE;
      } else if (tklen > 1 &&
          ((struct t_Token*) JoeArray_get(exec->tokens, 1))->type == _COLON_) {
         joe_Object_assign(retval,
                      joe_Exception_New ("execute exec: syntax not permitted"));
         return JOE_FAILURE;
      }
      rc = JoeParser_compile (parser, block, exec->tokens);
      if (rc != JOE_SUCCESS) {
         joe_Object_assign (retval, JoeParser_getException (parser));
         JoeParser_delete (parser);
         return JOE_FAILURE;
      }
      JoeParser_delete (parser);
      exec->compiled = 1;
   }
   if (joe_Block_outer_exec (block, argc, argv, retval) != JOE_SUCCESS) {
      return JOE_FAILURE;
   } else {
      return JOE_SUCCESS;
   }
}

static joe_Method mthds[] = {
  {"add", add },
  {"clear", clear },
  {"exec", exec },
  {(void *) 0, (void *) 0}
};

joe_Class joe_Execute_Class = {
   "joe_Execute",
   ctor,
   0,
   mthds,
   variables,
   &joe_Object_Class,
   0
};

joe_Object
joe_Execute_New (joe_Block parent, char *name)
{
   joe_Object self;

   self = joe_Object_New (&joe_Execute_Class, 0);
   if (name)
      init (self, parent, joe_String_New (name), 0);
   else
      init (self, parent, joe_String_New ("<Execute>"), 0);

   return self;
}

int
joe_Execute_add (joe_Execute self, char *line)
{
   joe_Object retval = 0;
   joe_String sline = 0;
   int Return;

   joe_Object_assign (&sline, joe_String_New (line));
   Return = add (self, 1, &sline, &retval);
   joe_Object_assign (&sline, 0);
   joe_Object_assign (&retval, 0);

   return Return;
}

int
joe_Execute_clear (joe_Execute self)
{
   joe_Object retval = 0;
   int Return;

   Return = clear (self, 0, 0, &retval);
   joe_Object_assign (&retval, 0);

   return Return;
}

int
joe_Execute_exec (joe_Execute self, int argc, joe_Object *args,
                                              joe_Object *retval)
{
   int Return;

   Return = exec (self, argc, args, retval);

   return Return;
}
