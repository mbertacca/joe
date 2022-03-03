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
# include "joe_Message.h"
# include "joe_Assignment.h"
# include "joe_Memory.h"
# include "joe_Block.h"
# include "joe_JOEObject.h"
# include "joe_String.h"
# include "joe_Array.h"
# include "joe_Variable.h"
# include "joe_Exception.h"
# include "joe_BreakBlockException.h"
# include "joe_Selector.h"
# include "joestrct.h"
# include <string.h>

# define MAX_IDENTIFIER_LEN 31

struct s_srcInfo {
   char *fileName;
   int row;
   int col;
};

# define RECEIVER 0
# define SELECTOR 1
# define ARGS     2
# define EXECARGS 3
# define SRC_INFO 4

static char *variables[] = {"receiver","selector","args","execArgs","srcInfo", 0};

int
joe_Message_exec (joe_Object self, joe_Block block, joe_Object *retval)
{
   joe_Object *selfVars = joe_Object_array (self);
   joe_Object receiver = selfVars[RECEIVER];
   joe_Selector selector = selfVars[SELECTOR];
   joe_Array argsobj = selfVars[ARGS];
   unsigned int argc = joe_Array_length (argsobj);
   joe_Object *args = *JOE_MEM(argsobj);
   joe_Object arg = 0;
   joe_Object lRet = 0;
   joe_Object lReceiver = 0;
   joe_Array *lArgs;
   unsigned int i;
   int rc = JOE_SUCCESS;
   int isJOEObject = JOE_ISCLASS(block,&joe_JOEObject_Class);
   int isAssignment = JOE_ISCLASS(receiver,&joe_Assignment_Class);
/*
   printf("%s\n", joe_Message_toString(self));
*/
   lArgs = *JOE_MEM(selfVars[EXECARGS]);
   if (isAssignment) {
      lReceiver = receiver;
      lArgs[0] = block;
      lArgs[1] = args[1];
      i = 2;
   } else {
      i = 0;
   }
   for ( ; i < argc && rc == JOE_SUCCESS; i++) {
      if ((arg = args[i]) == 0) {
         lArgs[i] = 0;
      } else if (JOE_ISCLASS(arg,&joe_Message_Class)) {
         lRet = 0;
         rc = joe_Message_exec (arg, block, &lRet);
         if (rc == JOE_FAILURE) {
            lArgs[i] = 0;
            *retval = lRet;
            break;
         } else {
            joe_Object_transfer (&lArgs[i], &lRet);
         }
      } else if (JOE_ISCLASS (arg, &joe_Variable_Class)) {
         lRet = 0;
         rc = joe_Block_getVariable(block, arg, &lRet);
         if (rc == JOE_FAILURE) {
            lArgs[i] = 0;
            *retval = lRet;
            break;
         } else {
            lArgs[i] = lRet;
         }
      } else if (JOE_ISCLASS (arg, &joe_Block_Class)) {
         if (isJOEObject) {
            joe_Object_assign (&lArgs[i], 
                               joe_Block_clone (arg, block));
         } else {
            joe_Block_setParent (arg, block);
            lArgs[i] = arg;
         }
      } else {
         /* joe_Object_assign (JOE_AT(lArgs, i), arg); */
         lArgs[i] = arg;
      }
   }
   if (rc == JOE_SUCCESS) {
      if (JOE_ISCLASS (receiver, &joe_Message_Class)) {
         lRet = 0;
         rc = joe_Message_exec (receiver , block, &lRet);
         if (rc == JOE_FAILURE) {
            lReceiver = 0;
            *retval = lRet;
         } else {
            joe_Object_transfer (&lReceiver, &lRet);
         }
      } else if (JOE_ISCLASS (receiver, &joe_Variable_Class)) {
         lRet = 0;
         rc = joe_Block_getVariable(block, receiver, &lRet);
         if (rc == JOE_FAILURE) {
            lReceiver = 0;
            joe_Object_transfer(retval, &lRet);
         } else {
            lReceiver = lRet;
         }
         if (isJOEObject && joe_Object_instanceOf(lReceiver,&joe_Block_Class)){
            joe_Object obj = 0;
            joe_Object_assign (&obj, joe_JOEObject_New (lReceiver, block));
            joe_Object_transfer(&lReceiver, &obj);
         }
      } else {
         lReceiver = receiver;
      }
   }
   if (rc == JOE_SUCCESS) {
      if (lReceiver == 0) {
         joe_Object_assign(retval, joe_Exception_New ("exec: null receiver"));
         rc = JOE_FAILURE;
      } else {
         /* rc = joe_Object_invoke(lReceiver, selector, argc,lArgs, retval); */
         rc = joe_Selector_invoke (selector, lReceiver, argc, lArgs, retval);
      }
   }

   if (isAssignment) {
      lArgs[0] = 0;
   } else if (lReceiver != receiver) {
      joe_Object_assign(&lReceiver, 0);
   }
   for (i = 0; i < argc; i++)
      if (lArgs[i] == args[i])
         lArgs[i] = 0;
      else
         joe_Object_assign (&lArgs[i],0);
/*
   for (i = 0; i < argc; i++)
      joe_Object_assign (JOE_AT(lArgs,i), 0);
*/
/*
   for (i = 0; i < argc; i++) {
      arg = *JOE_AT(args, i);
      if (joe_Object_instanceOf (arg, &joe_Message_Class)
          || (isJOEObject && joe_Object_instanceOf (arg, &joe_Block_Class)))
         joe_Object_assign (JOE_AT(lArgs,i), 0);
      else
         *JOE_AT(lArgs, i) = 0;
   }
   if (joe_Object_instanceOf (receiver, &joe_Message_Class)
       || (isJOEObject && joe_Object_instanceOf (lReceiver, &joe_Block_Class)))
      joe_Object_assign (&lReceiver, 0);
*/

   if (rc != JOE_SUCCESS) {
      if (!joe_Object_instanceOf(*retval, &joe_BreakBlockException_Class)) {
         struct s_srcInfo *srcInfo;
         srcInfo = (struct s_srcInfo *)
                  *JOE_MEM(*JOE_AT(self,SRC_INFO));
         joe_Exception_addStack (*retval,
                              srcInfo->fileName, srcInfo->row, srcInfo->col);
      }
   }

   return rc;
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
joe_Message_New (joe_Object receiver, char *selector,
                 int argc, joe_Object *args, char *fileName, int row, int col) {
   int i;
   struct s_srcInfo *srcInfo;
   joe_Memory info = 0;
   joe_Object self = 0;
   joe_Array argsArray = 0;
   joe_Array execArgs = 0;
   self = joe_Object_New (&joe_Message_Class, 0);

   joe_Object_assign (JOE_AT(self,RECEIVER), receiver);
   joe_Object_assign (JOE_AT(self,SELECTOR), joe_Selector_New(selector));
   argsArray = joe_Object_setAcyclic(joe_Array_New (argc));
   joe_Object_assign (JOE_AT(self,ARGS), argsArray);
   for (i = 0; i < argc; i++)
      joe_Object_assign (JOE_AT(argsArray,i), args[i]);
   execArgs = joe_Object_setAcyclic(joe_Array_New (argc));
   joe_Object_assign (JOE_AT(self,EXECARGS), execArgs);
   info = joe_Memory_New (sizeof(struct s_srcInfo));
   joe_Object_assign (JOE_AT(self,SRC_INFO), info);
   srcInfo = (struct s_srcInfo *) *JOE_MEM(info);
   srcInfo->fileName = fileName;
   srcInfo->row = row;
   srcInfo->col = col;

   return self;
}

static char buffer[512];
char *
joe_Message_toString (joe_Message self)
{
   unsigned int i, len;
   joe_Array args = *JOE_AT(self, ARGS);
   unsigned int argc = joe_Array_length(args);
   joe_Object obj;
   joe_String msg = 0;

   snprintf(buffer, sizeof(buffer), "0x%p %s %s ",
                                    (void*)self,
                                    joe_Object_toString (*JOE_AT(self, RECEIVER)),
                                    joe_Selector_name(*JOE_AT(self, SELECTOR)));
   for (i = 0; i < argc && (len = sizeof(buffer) - strlen(buffer)) > 0; i++) {
      obj = *JOE_AT(args, i);
      if (obj) {
         joe_Object_invoke(obj, "toString", 0, 0, &msg);
         strncat (buffer, joe_String_getCharStar(msg), len);
         joe_Object_assign(&msg, 0);
         if (strlen (buffer) < sizeof(buffer))
            strcat(buffer, " ");
      } else {
         strncat(buffer, "[null]", len);
      }
   }
   
   return buffer;
}