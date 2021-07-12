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
# include "joe_Block.h"
# include "joe_BreakLoopException.h"
# include "joestrct.h"

static int
length (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   joe_Object_assign (retval, joe_Integer_New (joe_Array_length (self)));
   return JOE_SUCCESS;
}

static int
get (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1 && joe_Object_instanceOf (argv[0], &joe_Integer_Class)) {
      int idx = joe_Integer_value (argv[0]);
      if (idx >= 0 && idx < (int) joe_Array_length (self)) {
         joe_Object_assign(retval, *JOE_AT (self, idx));
         return JOE_SUCCESS;
      } else {
         joe_Object_assign(retval, joe_Exception_New ("Array: index out of bound"));
         return JOE_FAILURE;
      }
   } else {
      joe_Object_assign(retval, joe_Exception_New ("Array: invalid argument"));
      return JOE_FAILURE;
   }
}

static int
set (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 2 && joe_Object_instanceOf (argv[0], &joe_Integer_Class)) {
      int idx = joe_Integer_value (argv[0]);
      if (idx >= 0 && idx < (int) joe_Array_length (self)) {
         joe_Object_assign (JOE_AT(self, idx), argv[1]);
         joe_Object_assign(retval, argv[1]);
         return JOE_SUCCESS;
      } else {
         joe_Object_assign(retval, joe_Exception_New ("Array: index out of bound"));
         return JOE_FAILURE;
      }
   } else {
      joe_Object_assign(retval, joe_Exception_New ("Array: invalid argument"));
      return JOE_FAILURE;
   }
}

static int
clean(joe_Object self, int argc, joe_Object* argv, joe_Object* retval)
{
   if (argc == 0) {
       joe_Object_assign (retval, joe_Array_clean(self));
       return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval, joe_Exception_New("Array: no argument expected"));
      return JOE_FAILURE;
   }
}


static int
foreach (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1) {
      if (joe_Object_instanceOf (argv[0], &joe_Block_Class)) {
         int rc;
         joe_Object lretval = 0;
         int len = joe_Array_length (self);
         int i;
         for (i = 0; i < len; i++) {
            if ((rc=joe_Object_invoke(argv[0],"exec",
                                      1, JOE_AT (self, i),
                                                   &lretval)) != JOE_SUCCESS) {
               if (joe_Object_instanceOf (lretval,
                                            &joe_BreakLoopException_Class)) {
                  joe_Object retobj = joe_BreakException_getReturnObj(lretval);
                  if (retobj)
                     joe_Object_assign(retval, retobj);
                  joe_Object_assign (&lretval, 0);
                  return JOE_SUCCESS;
               } else {
                  joe_Object_assign(retval, lretval);
                  joe_Object_assign(&lretval, 0);
                  return JOE_FAILURE;
               }
            } else {
              joe_Object_assign(retval, lretval);
              joe_Object_assign(&lretval, 0);
            }
         }
         return JOE_SUCCESS;
      } else {
         joe_Object_assign(retval,
                           joe_Exception_New ("Array foreach: block expected"));
         return JOE_FAILURE;
      }
   }
   joe_Object_assign(retval,
                   joe_Exception_New ("Array foreach: invalid argument number"));
   return JOE_FAILURE;
}

static joe_Method mthds[] = {
  {"length", length },
  {"get", get },
  {"set", set },
  {"foreach", foreach },
  {"clean", clean },
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
joe_Array_New(unsigned int length)
{
   return joe_Object_New(&joe_Array_Class, length);
}

unsigned int
joe_Array_length (joe_Object self)
{
   return joe_Object_length (self);
}

joe_Array
joe_Array_clean(joe_Object self)
{
   int len = (int)joe_Array_length(self);
   int i;
   for (i = 0; i < len; i++)
      joe_Object_assign(JOE_AT(self, i), 0);
   return self;
}
