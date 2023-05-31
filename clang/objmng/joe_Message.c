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
# include "joe_Message.h"
# include "joe_Assignment.h"
# include "joe_Memory.h"
# include "joe_Block.h"
# include "joe_JOEObject.h"
# include "joe_String.h"
# include "joe_StringBuilder.h"
# include "joe_Array.h"
# include "joe_ArrayList.h"
# include "joe_Variable.h"
# include "joe_Exception.h"
# include "joe_BreakBlockException.h"
# include "joe_Selector.h"
# include "joe_Execute.h"
# include "joe_Null.h"
# include "joestrct.h"
# include <string.h>

struct s_srcInfo {
   char *fileName;
   int row;
   int col;
};

# define SRC_INFO  0
# define ASSIGNEE  1
# define RPN       2

static joe_Object execStack[4096];
static int execIdx = 0;
static int execSize = sizeof (execStack) / sizeof(execStack[0]);

static char *variables[] = {"srcInfo", "assignee", "rpn", 0};

extern void joe_Block_setVarValue (joe_Block self,joe_Variable var,joe_Object value);

joe_Message
joe_Message_clone (joe_Message self, joe_Block parent)
{
   joe_Message Return = Return = joe_Object_clone (self);
   joe_Array rpnOld = *JOE_AT(self, RPN), rpnNew;
   int rpnc = joe_Array_length (rpnOld);
   joe_Object *rpnItOld;
   int i;

   joe_Object_assign (JOE_AT(Return,RPN), rpnNew = joe_Array_New(rpnc));

   for (i = 0; i < rpnc; i++) {
      rpnItOld = JOE_AT(rpnOld, i);
      if (JOE_ISCLASS ((*rpnItOld), &joe_Block_Class))
         joe_Object_assign (JOE_AT(rpnNew, i), joe_Block_clone(*rpnItOld, parent));
      else
         joe_Object_assign (JOE_AT(rpnNew, i), *rpnItOld);
   }
   return Return;
}

int
joe_Message_exec (joe_Object self, joe_Block block, joe_Object *retval)
{
   joe_Array rpn = *JOE_AT(self, RPN);
   int rpnc = joe_Array_length (rpn);
   int i;
   int rc = JOE_SUCCESS;
   joe_Object rpnItem;
   int idx0 = execIdx;
   int topIdx = 0;
   joe_Object receiver;
   int argc;
   joe_Object *argv;
   joe_Object lretval = 0;
   joe_Variable assignee = *JOE_AT(self,ASSIGNEE);

    /* joe_Message_toString(self); */

   if (execIdx + rpnc > execSize) {
      joe_Object_assign(retval, joe_Exception_New ("Stack Overflow Error"));
      return JOE_FAILURE;
   }

   for (i = 0; i < rpnc && rc == JOE_SUCCESS; i++) {
      rpnItem = *JOE_AT(rpn, i);
      if (JOE_ISCLASS (rpnItem, &joe_Selector_Class)) {
         argc = joe_Selector_getArgc (rpnItem);
         topIdx = execIdx;
         topIdx -= argc;
         argv = &execStack[topIdx];
         topIdx --;
         receiver = execStack[topIdx];
         lretval = 0;
         rc = joe_Selector_invoke (rpnItem, receiver, argc, argv, &lretval);
         for ( ;topIdx < execIdx; execIdx--)
             joe_Object_assign (&execStack[execIdx], 0);
         joe_Object_transfer (&execStack[execIdx++], &lretval);
      } else if (JOE_ISCLASS (rpnItem, &joe_Variable_Class)) {
         rc = joe_Block_getVarValue(block, rpnItem, &execStack[execIdx++]);
      } else {
         joe_Object_assign (&execStack[execIdx++], rpnItem);
      }
   }
   joe_Object_transfer (retval, &execStack[--execIdx]);
   if (rc == JOE_SUCCESS) {
      if (assignee) {
         joe_Block_setVarValue (block, assignee, *retval);
      }
   } else {
      for ( ; execIdx >= idx0; execIdx--)
          joe_Object_assign (&execStack[execIdx], 0);
      execIdx++;
      if (!joe_Object_instanceOf(*retval, &joe_BreakBlockException_Class)) {
         struct s_srcInfo *srcInfo;
         srcInfo = (struct s_srcInfo *)
                  *JOE_MEM(*JOE_AT(self,SRC_INFO));
         joe_Exception_addStack (*retval,
                              srcInfo->fileName, srcInfo->row, srcInfo->col);
      }
   }
   if (idx0 != execIdx) { printf ("\nErrore Stack %d != %d\n", idx0, execIdx); }
   if (*retval && JOE_ISCLASS ((*retval), &joe_WeakReference_Class)) {
      lretval = 0;
      joe_Object_assign (&lretval, joe_WeakReference_get (*retval));
      joe_Object_transfer (retval, &lretval);
   }
   return rc;
}

static void
show (joe_Object value)
{
   fflush (stdout);
   if (value) {
      joe_String str = 0;
      joe_Object_invoke (value, "toString", 0, 0, &str);

      fprintf (stderr, "---> %s\n", joe_String_getCharStar(str));
      joe_Object_assign (&str, 0);
   }
   fflush (stderr);
}

# define MAXLINELEN 256
static char *line = 0;

int
joe_Message_debug (joe_Object self, int *debug,
                   joe_Block block, joe_Object *retval)
{
   joe_String toString = 0;
   int i;

   if (line == 0) {
      line = calloc (MAXLINELEN, 1);
      fprintf (stderr, "* JOE debugger *\n");
      fprintf (stderr, "<enter> run current message;\n");
      fprintf (stderr, ".       run current message stepping over blocks;\n");
      fprintf (stderr, ".s      skip current message;\n");
      fprintf (stderr, ".e      exit debug mode;\n");
      fprintf (stderr, ".q      quit program.\n\n");
      fprintf (stderr, "otherwise run the command you enter.\n\n");
   }
   for ( ; ; ) {
      int rc;
      joe_Message_toString (self, &toString);
      fprintf (stderr, "***  %s\n", joe_String_getCharStar (toString));
      joe_Object_assign (&toString, 0);

      fprintf (stderr, "dbg: ");
      fflush (stderr);
      if (fgets (line, MAXLINELEN, stdin) == NULL)
         strcpy (line,".q");
      i = strlen (line);
      while (i > 0 && line[--i] <= ' ')
         line[i] = 0;
      if (*line == 0) {
         rc = joe_Message_exec (self, block, retval);
         if (rc == JOE_SUCCESS) {
            show (*retval);
         }
         return rc;
      } else if (strcmp (line,".") == 0) {
         *debug = 0;
         rc = joe_Message_exec (self, block, retval);
         *debug = 1;
         return rc;
      } else if (strcmp (line,".s") == 0) {
         joe_Object_assign (retval, joe_Null_value);
         return JOE_SUCCESS;
      } else if (strcmp (line,".e") == 0) {
         *debug = 0;
         return joe_Message_exec (self, block, retval);
      } else if (strcmp (line,".q") == 0) {
         *debug = 0;
         exit (0);
      } else {
         joe_Execute exec = 0;
         joe_Object_assign (&exec, joe_Execute_New (block, ":debug:"));
         joe_Execute_add (exec, line);
         *debug = 0;
         joe_Execute_exec (exec, 0, 0, retval);
         show (*retval);
         *debug = 1;
         joe_Object_assign (&exec, 0);
      }
   }
}

int
joe_Message_isLabel (joe_Message self, joe_String name)
{
   joe_Array rpn = *JOE_AT(self, RPN);

   if (joe_Array_length (rpn) == 1) {
       joe_Object obj = *JOE_AT(rpn, 0);
       if (JOE_ISCLASS (obj, &joe_String_Class) &&
           joe_String_compare (obj, name) == 0)
          return JOE_TRUE;
   }
   return JOE_FALSE;
}

static joe_Method mthds[] = {
  /* {"exec", joe_Message_exec}, */
  {(void *) 0, (void *) 0}
};

joe_Class joe_Message_Class = {
   "joe_Message",
   0,
   0,
   mthds,
   variables,
   &joe_Object_Class,
   0
};

joe_Object
joe_Message_New (joe_Variable assignee, int argc, joe_Object *argv,
                  char *fileName, int row, int col) {
   int i;
   struct s_srcInfo *srcInfo;
   joe_Memory info = 0;
   joe_Object self = 0;
   joe_Object rpn = 0;

   self = joe_Object_New (&joe_Message_Class, 0);

   joe_Object_assign (JOE_AT(self,ASSIGNEE), assignee);

   info = joe_Memory_New (sizeof(struct s_srcInfo));
   joe_Object_assign (JOE_AT(self,SRC_INFO), info);
   srcInfo = (struct s_srcInfo *) *JOE_MEM(info);
   srcInfo->fileName = fileName;
   srcInfo->row = row;
   srcInfo->col = col;

   joe_Object_assign (JOE_AT(self,RPN), rpn = joe_Array_New(argc));

   for (i = 0; i < argc; i++) {
      joe_Object_assign(JOE_AT(rpn, i), argv[i]);
   };

   return self;
}

int
joe_Message_toString(joe_Message self, joe_String *retval)
{
   joe_Array rpn = *JOE_AT(self, RPN);
   joe_Variable assignee = *JOE_AT(self,ASSIGNEE);
   int rpnc = joe_Array_length (rpn);
   joe_Object obj;
   int i;
   struct s_srcInfo *srcInfo = (struct s_srcInfo *)
                                *JOE_MEM(*JOE_AT(self,SRC_INFO));
   joe_StringBuilder Return = 0;
   joe_Object_assign (&Return, joe_StringBuilder_New ());

   joe_StringBuilder_appendChar (Return, '<');
   joe_StringBuilder_appendInt (Return, srcInfo->row);
   joe_StringBuilder_appendChar (Return, ',');
   joe_StringBuilder_appendInt (Return, srcInfo->col);
   joe_StringBuilder_appendChar (Return, '>');

   if (assignee) {
      joe_StringBuilder_appendChar (Return, ' ');
      joe_StringBuilder_appendCharStar (Return,
                                    joe_Variable_nameCharStar (assignee));
      joe_StringBuilder_appendCharStar (Return, ":=");
   }

   for (i = 0; i < rpnc; i++) {
      obj = *JOE_AT(rpn, i);
      if (obj) {
         joe_Object msg = 0;
         joe_Object_invoke(obj, "toString", 0, 0, &msg);
         joe_StringBuilder_appendChar (Return, ' ');
         joe_StringBuilder_appendCharStar (Return, joe_String_getCharStar(msg));
         joe_Object_assign(&msg, 0);
      } else {
         joe_StringBuilder_appendCharStar (Return, "()");
      }
   }
   joe_Object_assign (retval, joe_StringBuilder_toString (Return));
   joe_Object_assign (&Return, 0);

   return JOE_SUCCESS;
}
