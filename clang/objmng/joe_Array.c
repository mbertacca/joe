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

# include "joe_Array.h"
# include "joe_Integer.h"
# include "joe_Exception.h"

static int
length (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   *retval = joe_Integer_New (joe_Array_length (self));
   return JOE_SUCCESS;
}

static int
get (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1 && joe_Object_instanceOf (argv[0], &joe_Integer_Class)) {
      int idx = joe_Integer_value (argv[0]);
      if (idx >= 0 && idx < joe_Array_length (self)) {
         *retval = *joe_Object_at (self, idx);
         return JOE_SUCCESS;
      } else {
         *retval = joe_Exception_New ("Array: index out of bound");
         return JOE_FAILURE;
      }
   } else {
      *retval = joe_Exception_New ("Array: invalid argument");
      return JOE_FAILURE;
   }
}

static int
set (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 2 && joe_Object_instanceOf (argv[0], &joe_Integer_Class)) {
      int idx = joe_Integer_value (argv[0]);
      if (idx >= 0 && idx < joe_Array_length (self)) {
         joe_Object_assign (joe_Object_at (self, idx), argv[1]);
         *retval = argv[1];
         return JOE_SUCCESS;
      } else {
         *retval = joe_Exception_New ("Array: index out of bound");
         return JOE_FAILURE;
      }
   } else {
      *retval = joe_Exception_New ("Array: invalid argument");
      return JOE_FAILURE;
   }
}

static joe_Method mthds[] = {
  {"length", length },
  {"get", get },
  {"set", set },
  {(void *) 0, (void *) 0}
};

static char *variables[] = { 0 };

joe_Class joe_Array_Class = {
   "joe_Array",
   0,
   0,
   mthds,
   variables,
   &joe_Object_Class,
   0
};

joe_Object
joe_Array_New (unsigned int length)
{
   return joe_Object_New (&joe_Array_Class, length);
}

unsigned int
joe_Array_length (joe_Object self)
{
   return joe_Object_length (self);
}
