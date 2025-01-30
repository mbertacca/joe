/*
 * This source file is part of the "Joe Objects Executor" open source project
 *
 * Copyright 2021 Marco Bertacca (www.bertacca.eu)
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
# include "joe_Selector.h"
# include "joe_Memory.h"
# include "joe_JOEObject.h"
# include "joe_Exception.h"
# include "joestrct.h"

typedef struct s_Selector {
   int (*method)(JOE_METHOD_ARGS);
   joe_Object argRcvr;
   joe_Object actualRcvr;
   joe_Class *argClazz;
   int argc;
} Selector;

static joe_Method *blkExecMthd;

#define SELECTOR 0

static char *variables[] = { "selector", 0 };


static int
toString (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   joe_Object_assign(retval, joe_String_New (joe_Selector_toString (self)));
   return JOE_SUCCESS;
}

static joe_Method mthds[] = {
  {"toString", toString },
  {(void *) 0, (void *) 0}
};

joe_Class joe_Selector_Class = {
   "joe_Selector",
   0,
   0,
   mthds,
   variables,
   &joe_Object_Class,
   0
};


joe_Selector
joe_Selector_New (char *name, int argc)
{
   joe_Object self = joe_Object_New (&joe_Selector_Class, 0);
   int nameLen =  strlen(name) + 1;
   joe_Memory mem = joe_Memory_New (sizeof (Selector) + nameLen);
   Selector *selector = (Selector*) joe_Object_getMem (mem);
   char *mname = ((char *) selector) + sizeof(Selector);
   strcpy (mname, name);
   selector->argc = argc;
   joe_Object_assign (JOE_AT(self, SELECTOR), mem);
   blkExecMthd = joe_Object_getMethod (&joe_Block_Class, "exec");

   return self;
}

int
joe_Selector_getArgc (joe_Selector self)
{
   joe_Memory mem = *JOE_AT(self, 0);
   Selector *selector = (Selector*) JOE_MEM (mem) ;

   return selector->argc;
}


int
joe_Selector_invoke (joe_Object self, joe_Object receiver,
                     int argc, joe_Object *argv, joe_Object *retval)
{
   joe_Memory mem = *JOE_AT(self, 0);
   Selector *selector = (Selector*) JOE_MEM (mem);

   if (selector->argRcvr == receiver) {
      if (selector->argRcvr) {
         return selector->method (selector->actualRcvr, argc, argv, retval);
      } else {
         joe_Object_assign (retval, joe_Exception_New ("Void receiver"));
         return JOE_FAILURE;
      }
   } else if (selector->argClazz == receiver->clazz) {
      selector->argRcvr = receiver;
      selector->actualRcvr = receiver;
      return selector->method (selector->actualRcvr, argc, argv, retval);
   } else {
      joe_Class *clazz = joe_Object_getClass (receiver);
      char *selName;
      joe_Method *mthd;

      selector->argRcvr = receiver;

      if (clazz == &joe_WeakReference_Class) {
         receiver = joe_WeakReference_get(receiver);
         clazz = joe_Object_getClass(receiver);
      }

      selName = ((char *) selector) + sizeof(Selector);
      if (clazz == &joe_JOEObject_Class) {
         joe_Block actualRcvr = joe_JOEObject_getReceiver (receiver, selName);
         if (actualRcvr != 0) {
            mthd = blkExecMthd;
            selector->method = mthd->mthd;
            selector->actualRcvr = actualRcvr;
            selector->argClazz = 0;
            return selector->method (selector->actualRcvr, argc, argv, retval);
         }
      }
      selector->argClazz = receiver->clazz;
      mthd = joe_Object_getMethod (clazz, selName);
      if (mthd) {
         selector->method = mthd->mthd;
         selector->actualRcvr = receiver;
         return selector->method (selector->actualRcvr, argc, argv, retval);
      } else {
         char buffer[512];
         if (joe_Object_instanceOf (receiver,&joe_Block_Class)) {
            snprintf (buffer, sizeof(buffer),
                     "Method not found: %s in class %s, name=%s",
                      selName, clazz->name,
                     joe_String_getCharStar(joe_Block_getName(receiver)));
         } else {
            snprintf (buffer, sizeof(buffer), "Method not found: %s in class %s",
                      selName, clazz->name);
         }
         joe_Object_assign (retval, joe_Exception_New (buffer));
         selector->argRcvr = 0;
         selector->actualRcvr = 0;
         selector->argClazz = 0;
         return JOE_FAILURE;
      }
   }
/*

   if (clazz == &joe_WeakReference_Class) {
      self = joe_WeakReference_get(self);
      clazz = self->clazz;
   }
   if (clazz == &joe_JOEObject_Class) {
      joe_Block receiver = joe_JOEObject_getReceiver (self, name);
      if (receiver != 0) {
         mthd = joe_Object_getMethod (&joe_Block_Class, "exec");
         return mthd->mthd (receiver, argc, argv, retval);
      }
   }

   mthd = joe_Object_getMethod (clazz, name);
   if (mthd) {
      Return = mthd->mthd (self, argc, argv, retval);
   } else {
      sprintf (buffer, "Method not found: %s in class %s",
                 name, clazz->name);
      joe_Object_assign (retval, joe_Exception_New (buffer));
      Return = JOE_FAILURE;
   }
*/
}

char *
joe_Selector_name (joe_Object self)
{
   joe_Memory mem = *JOE_AT(self, 0);
   Selector *selector = (Selector*) JOE_MEM (mem) ;
   return ((char *) selector) + sizeof(Selector);
}

static char buffer[128];

char *
joe_Selector_toString (joe_Object self)
{
   joe_Memory mem = *JOE_AT(self, 0);
   Selector *selector = (Selector*) JOE_MEM (mem);

   snprintf (buffer, sizeof(buffer), "%s#%d",
            ((char *) selector) + sizeof(Selector),
            selector->argc);
   return buffer;
}
