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
# define SRC_INFO 3

static char *variables[] = {"receiver","selector","args", "srcInfo", 0};

int
joe_Message_exec (joe_Object self, joe_Block block, joe_Object *retval)
{
   joe_Object receiver = *joe_Object_at (self, RECEIVER);
   char *selector = joe_String_getCharStar(*joe_Object_at (self, SELECTOR));
   joe_Array args = *joe_Object_at (self, ARGS);
   unsigned int argc = joe_Array_length (args);
   joe_Object arg = 0;
   joe_Object lRet = 0;
   joe_Object lReceiver = 0;
   joe_Array lArgs = 0;
   unsigned int i;
   int rc = JOE_SUCCESS;
   int isJOEObject = joe_Object_instanceOf(block, &joe_JOEObject_Class);
   int isAssignment = joe_Object_instanceOf(receiver, &joe_Assignment_Class);
/*
   printf("%s\n", joe_Message_toString(self));
*/
   joe_Object_assign (&lArgs, joe_Array_New (argc));
   if (isAssignment) {
      lReceiver = receiver;
      *joe_Object_at(lArgs, 0) = block;
      i = 1;
   } else {
      i = 0;
   }
   for ( ; i < argc && rc == JOE_SUCCESS; i++) {
      arg = *joe_Object_at (args, i);
      if (joe_Object_instanceOf (arg, &joe_Message_Class)) {
         lRet = 0;
         rc = joe_Message_exec (arg, block, &lRet);
         if (rc == JOE_FAILURE) {
            *joe_Object_at(lArgs, i) = 0;
            *retval = lRet;
            break;
         } else {
            joe_Object_transfer (joe_Object_at(lArgs,i), &lRet);
         }
      } else if (joe_Object_instanceOf (arg, &joe_Variable_Class)) {
         lRet = 0;
         rc = joe_Block_getVar(block, arg, &lRet);
         if (rc == JOE_FAILURE) {
            *joe_Object_at(lArgs, i) = 0;
            *retval = lRet;
            break;
         } else {
            *joe_Object_at(lArgs, i) = lRet;
         }
      } else if (isJOEObject && joe_Object_instanceOf (arg, &joe_Block_Class)) {
         joe_Object_assign (joe_Object_at(lArgs,i), 
                            joe_JOEObject_New (arg, block));
      } else {
         /* joe_Object_assign (joe_Object_at(lArgs, i), arg); */
         *joe_Object_at(lArgs, i) = arg;
      }
   }
   if (rc == JOE_SUCCESS) {
      if (joe_Object_instanceOf (receiver, &joe_Message_Class)) {
         lRet = 0;
         rc = joe_Message_exec (receiver , block, &lRet);
         if (rc == JOE_FAILURE) {
            lReceiver = 0;
            *retval = lRet;
         } else {
            joe_Object_transfer (&lReceiver, &lRet);
         }
      } else if (joe_Object_instanceOf (receiver, &joe_Variable_Class)) {
         lRet = 0;
         rc = joe_Block_getVar(block, receiver, &lRet);
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
         rc = joe_Object_invoke(lReceiver, selector, argc,
                         (joe_Object*) *joe_Object_getMem (lArgs), retval);
      }
   }

   if (isAssignment) {
      *joe_Object_at(lArgs, 0) = 0;
   } else if (lReceiver != receiver) {
         joe_Object_assign(&lReceiver, 0);
   }
   for (i = 0; i < argc; i++)
      if (*joe_Object_at(lArgs, i) == *joe_Object_at(args, i))
         *joe_Object_at(lArgs, i) = 0;
/*
   for (i = 0; i < argc; i++)
      joe_Object_assign (joe_Object_at(lArgs,i), 0);
/*
   for (i = 0; i < argc; i++) {
      arg = *joe_Object_at (args, i);
      if (joe_Object_instanceOf (arg, &joe_Message_Class)
          || (isJOEObject && joe_Object_instanceOf (arg, &joe_Block_Class)))
         joe_Object_assign (joe_Object_at(lArgs,i), 0);
      else
         *joe_Object_at(lArgs, i) = 0;
   }
   if (joe_Object_instanceOf (receiver, &joe_Message_Class)
       || (isJOEObject && joe_Object_instanceOf (lReceiver, &joe_Block_Class)))
      joe_Object_assign (&lReceiver, 0);
*/

   if (rc != JOE_SUCCESS) {
      if (!joe_Object_instanceOf(*retval, &joe_BreakBlockException_Class)) {
         struct s_srcInfo *srcInfo;
         srcInfo = (struct s_srcInfo *)
                  *joe_Object_getMem (*joe_Object_at (self,SRC_INFO));
         joe_Exception_addStack (*retval,
                              srcInfo->fileName, srcInfo->row, srcInfo->col);
      }
   }

   joe_Object_assign (&lArgs, 0);
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
   self = joe_Object_New (&joe_Message_Class, 0);

   joe_Object_assign (joe_Object_at (self,RECEIVER), receiver);
   joe_Object_assign (joe_Object_at (self,SELECTOR), joe_String_New(selector));
   argsArray = joe_Array_New (argc);
   joe_Object_assign (joe_Object_at (self,ARGS), argsArray);
   for (i = 0; i < argc; i++)
      joe_Object_assign (joe_Object_at (argsArray,i), args[i]);
   info = joe_Memory_New (sizeof(struct s_srcInfo));
   joe_Object_assign (joe_Object_at (self,SRC_INFO), info);
   srcInfo = (struct s_srcInfo *) *joe_Object_getMem (info);
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
   joe_Array args = *joe_Object_at(self, ARGS);
   unsigned int argc = joe_Array_length(args);
   joe_Object obj;
   joe_String msg = 0;

   snprintf(buffer, sizeof(buffer), "0x%p %s %s ",
                                    (void*)self,
                                    joe_Object_toString (*joe_Object_at(self, RECEIVER)),
                                    joe_String_getCharStar(*joe_Object_at(self, SELECTOR)));
   for (i = 0; i < argc && (len = sizeof(buffer) - strlen(buffer)) > 0; i++) {
      obj = *joe_Object_at(args, i);
      if (obj) {
         joe_Object_invoke(obj, "toString", 0, 0, &msg);
         strncat (buffer, joe_String_getCharStar(msg), len);
         joe_Object_assign(&msg, 0);
         if (strlen (buffer) < sizeof(buffer))
            strncat(buffer, " ", 1);
      } else {
         strncat(buffer, "[null]", len);
      }
   }
   
   return buffer;
}