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

# define ARRAY 0
# define LENGTH 1
# define MAXLENGTH 2
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
   joe_Object_assign (joe_Object_at(self,ARRAY),joe_Array_New(maxLength));
   joe_Object_assign (joe_Object_at(self,LENGTH),joe_int_New0 ());
   joe_Object_assign (joe_Object_at(self,MAXLENGTH),
                      joe_int_New (maxLength));
   return self;
}

unsigned int
joe_ArrayList_length (joe_Object self)
{
   return (unsigned int) joe_int_value (*joe_Object_at(self,LENGTH));
}

unsigned int
joe_ArrayList_maxLength (joe_Object self)
{
   return (unsigned int) joe_int_value (*joe_Object_at(self,MAXLENGTH));
}

joe_Object *
joe_ArrayList_at (joe_Object self, unsigned int idx)
{
   unsigned int len = joe_ArrayList_length (self);
   if (idx < len) 
      return joe_Object_at (*joe_Object_at(self,ARRAY), idx);
   else
      return 0;
}

void
joe_ArrayList_add (joe_Object self, joe_Object item)
{
   joe_Object *selfVars = joe_Object_array (self);
   joe_int objLen = selfVars[LENGTH];
   joe_int objMaxLen = selfVars[MAXLENGTH];
   joe_Object *theArray = &selfVars[ARRAY];
   unsigned int *len = joe_int_starValue (objLen);
   unsigned int *maxLen = joe_int_starValue (objMaxLen);

   if (*len == *maxLen) {
      unsigned int i;
      joe_Array nArray = joe_Array_New(*maxLen << 1);
      for (i = 0; i < *len; i++) {
         *joe_Object_at(nArray, i) = *joe_Object_at(*theArray, i);
         *joe_Object_at(*theArray, i) = 0;
         (*maxLen)++;
      }
      joe_Object_assign (theArray, nArray);
   }
   joe_Object_assign (joe_Object_at(*theArray, *len), item);
   (*len)++;
}
