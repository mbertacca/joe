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

# include "joe_Class.h"
# include "joe_Array.h"
# include "joestrct.h"
# include "joe_String.h"
# include "joe_Memory.h"
# include "joe_Null.h"
# include "joe_HashMap.h"

# include <stdio.h>

static char *variables[] = { "clazz", 0 };

static int
ctor (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1) {
      joe_Memory mem = joe_Memory_New (sizeof (joe_Class*));
      joe_Class ** clazz = (void *) joe_Object_getMem(mem);
      *clazz = argv[0]->clazz;
      joe_Object_assign (JOE_AT(self, 0), mem);
      return JOE_SUCCESS;
   } else {
      *retval = 0;
      return JOE_FAILURE;
   }
}

static int
getName (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{  
   joe_Memory mem = *JOE_AT(self, 0);
   joe_Class ** clazz = (void *) joe_Object_getMem(mem);
   joe_Object_assign(retval,  joe_String_New ((*clazz)->name));
   return JOE_SUCCESS;
}

static int
getMethods (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   joe_Memory mem = *JOE_AT(self, 0);
   joe_Class ** clazz = (void *) joe_Object_getMem(mem);
   joe_Method *mthd;
   joe_HashMap methods = 0;
   joe_Object oldValue = 0;
   joe_String key = 0;
   joe_Class *clz;

   joe_Object_assign (&methods, joe_HashMap_New (0));
   for (clz = *clazz; clz; clz = clz->extends) {
      if (clz->mthds) {
         for (mthd = clz->mthds; mthd->name != (void *) 0; mthd++) {
            joe_Object_assign(&key, joe_String_New (mthd->name));
            joe_HashMap_put (methods, key, joe_Null_value, &oldValue);
         }
      }
   }
   joe_Object_assign (retval, joe_HashMap_keys (methods));
   joe_Object_assign (&key, 0);
   joe_Object_assign (&methods, 0);

   return JOE_SUCCESS;
}

static int
toString (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   joe_Memory mem = *JOE_AT(self, 0);
   joe_Class ** clazz = (void *) joe_Object_getMem(mem);
   joe_Object_assign (retval, joe_String_New2 ("class ", (*clazz)->name));
   return JOE_SUCCESS;
}

static joe_Method mthds[] = {
  {"getName",    getName},
  {"getMethods", getMethods},
  {"toString",   toString},
  {(void *) 0, (void *) 0}
};

joe_Class joe_Class_Class = {
   "joe_Class",
   ctor,
   0,
   mthds,
   variables,
   &joe_Object_Class,
   0
};

