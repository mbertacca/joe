/*
 * This source file is part of the "Joe Objects Executor" open source project
 *
 * Copyright 2024 Marco Bertacca (www.bertacca.eu)
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

# include "joe_ArrayIterator.h"
# include "joe_Exception.h"
# include "joe_Integer.h"
# include "joe_Boolean.h"
# include "joestrct.h"

# define ARRAY 0
# define CURIDX 1

static char *variables[] = { "array", "curidx", 0 };

static int
ctor (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1 && joe_Object_instanceOf (argv[0],&joe_Array_Class)) {
      joe_Object_assign(JOE_AT(self, ARRAY), argv[0]);
      joe_Object_assign(JOE_AT(self, CURIDX), joe_int_New0());
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval,
           joe_Exception_New ("ArrayIterator ctor: invalid argument(s)"));
      return JOE_FAILURE;
   }
}

static int
hasNext (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 0) {
      int64_t curidx = joe_Integer_value(*joe_Object_at(self, CURIDX));
      int64_t arsize =  joe_Array_length (*joe_Object_at(self, ARRAY));
      if (curidx < arsize)
         joe_Object_assign(retval, joe_Boolean_New_true());
      else
         joe_Object_assign(retval, joe_Boolean_New_false());
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval,
           joe_Exception_New ("ArrayIterator hasNext: invalid argument(s)"));
      return JOE_FAILURE;
   }
}

static int
next (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 0) {
      int64_t curidx = joe_Integer_value(*JOE_AT(self, CURIDX));
      joe_Array array = *JOE_AT(self, ARRAY);
      int64_t arsize =  joe_Array_length (array);

      if (curidx < arsize) {
         joe_Object_assign (retval, *JOE_AT (array, curidx));
         joe_Integer_addMe (*JOE_AT(self, CURIDX), 1);
         return JOE_SUCCESS;
      } else {
         joe_Object_assign(retval, joe_Exception_New ("Array: index out of bound"));
         return JOE_FAILURE;
      }
   } else {
      joe_Object_assign(retval,
           joe_Exception_New ("ArrayIterator next: invalid argument(s)"));
      return JOE_FAILURE;
   }
}


static joe_Method mthds[] = {
  {"hasNext", hasNext },
  {"next",    next },
  {(void *) 0, (void *) 0}
};


joe_Class joe_ArrayIterator_Class = {
   "joe_ArrayIterator",
   ctor,
   0,
   mthds,
   variables,
   &joe_Object_Class,
   0
};


int
joe_ArrayIterator_New (joe_Array array, joe_ArrayIterator *retval)
{
   joe_Object_assign (retval, joe_Object_New(&joe_ArrayIterator_Class, 0));
   return ctor (*retval, 1, &array, retval);
}
