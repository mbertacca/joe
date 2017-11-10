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

# include "joe_ArrayList.h"
# include "joe_Array.h"
# include "joe_Integer.h"

/*
static int
lenght (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   return JOE_SUCCESS;
}
*/


static joe_Method mthds[] = {
  /* {"length", length } */
  {(void *) 0, (void *) 0}
};

static char *variables[] = { "array", "length", "maxLength", 0 };

joe_Class joe_ArrayList_Class = {
   "joe_ArrayList",
   0,
   0,
   mthds,
   variables,
   &joe_Object_Class,
   0
};


joe_Object
joe_ArrayList_New (unsigned int maxLength)
{
   joe_Object self;
   self = joe_Object_New (&joe_ArrayList_Class, 0);
   joe_Object_assign (joe_Object_getVar(self,"array"),joe_Array_New(maxLength));
   joe_Object_assign (joe_Object_getVar(self,"length"),joe_Integer_New (0));
   joe_Object_assign (joe_Object_getVar(self,"maxLength"),
                                                   joe_Integer_New (maxLength));

   return self;
}

unsigned int
joe_ArrayList_length (joe_Object self)
{
   return joe_Integer_value (*joe_Object_getVar(self,"length"));
}

unsigned int
joe_ArrayList_maxLength (joe_Object self)
{
   return joe_Integer_value (*joe_Object_getVar(self,"maxLength"));
}

joe_Object *
joe_ArrayList_at (joe_Object self, unsigned int idx)
{
   unsigned int len = joe_ArrayList_length (self);
   if (idx < len) 
      return joe_Object_at (*joe_Object_getVar(self,"array"), idx);
   else
      return 0;
}

void
joe_ArrayList_add (joe_Object self, joe_Object item)
{
   joe_Integer objLen = *joe_Object_getVar(self,"length");
   joe_Integer objMaxLen = *joe_Object_getVar(self,"maxLength");
   joe_Object *array = joe_Object_getVar(self,"array");
   unsigned int len = joe_Integer_value (objLen);
   unsigned int maxLen = joe_Integer_value (objMaxLen);

   if (len == maxLen) {
      int i;
      joe_Array nArray = joe_Array_New(maxLen << 1);
      for (i = 0; i < len; i++) {
         *joe_Object_at(nArray, i) = *joe_Object_at(*array, i);
         *joe_Object_at(*array, i) = 0;
          joe_Integer_addMe1 (objMaxLen);
      }
      joe_Object_assign (array, nArray);
   }
   joe_Object_assign (joe_Object_at(*array, len), item);
   joe_Integer_addMe1 (objLen);
}
