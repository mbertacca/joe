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
# include "joe_Exception.h"
# include "joe_Integer.h"
# include "joe_Boolean.h"
# include "joe_Block.h"
# include "joe_BreakLoopException.h"
# include "joestrct.h"

# define ARRAY 0
# define LENGTH 1
# define MAXLENGTH 2
static char *variables[] = { "array", "length", "maxLength", 0 };

static int
ctor (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 0) {
      joe_Object_assign (retval,  joe_ArrayList_New(8));
      return JOE_SUCCESS;
    } else if (argc == 1 && joe_Object_instanceOf (argv[0],&joe_Integer_Class)) {
      unsigned int size = joe_Integer_value (argv[0]);
      joe_Object_assign (retval,  joe_Array_New(size));
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval,
           joe_Exception_New ("ArrayList ctor: invalid argument(s)"));
      return JOE_FAILURE;
   }
}

static int
length (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 0) {
      joe_Object_assign(retval,
                        joe_Integer_New (joe_int_value (*JOE_AT (self, LENGTH))));
   } else {
      joe_Object_assign(retval,
                     joe_Exception_New ("ArrayList length: invalid argument"));
      return JOE_FAILURE;
   }
   return JOE_SUCCESS;
}

static int
add (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1) {
      joe_ArrayList_add (self, argv[0]);
      joe_Object_assign(retval, joe_Boolean_True);
   } else {
      joe_Object_assign(retval,
                     joe_Exception_New ("ArrayList add: invalid argument"));
      return JOE_FAILURE;
   }
   return JOE_SUCCESS;
}

static int
get (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1 && joe_Object_instanceOf (argv[0], &joe_Integer_Class)) {
      long length = joe_Integer_value (*joe_Object_at (self, LENGTH));
      int idx = joe_Integer_value (argv[0]);
      if (idx < length) {
         joe_Array array = *joe_Object_at (self, ARRAY);
         joe_Object_assign(retval, *JOE_AT (array, idx));
      } else {
         joe_Object_assign(retval,
                           joe_Exception_New ("ArrayList get: index out of bounds"));
         return JOE_FAILURE;
      }
   } else {
      joe_Object_assign(retval,
                        joe_Exception_New ("ArrayList get: invalid argument"));
      return JOE_FAILURE;
   }
   return JOE_SUCCESS;
}

static int
foreach (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   int start;
   joe_Block blk;
   int rc;
   joe_Array array = *joe_Object_at (self, ARRAY);
   int len = joe_ArrayList_length (self);
   int i;

   if (argc == 1 && joe_Object_instanceOf (argv[0], &joe_Block_Class)) {
      start = 0;
      blk = argv[0];
   } else if (argc == 2 
              && joe_Object_instanceOf (argv[0], &joe_Integer_Class)
              && joe_Object_instanceOf (argv[1], &joe_Block_Class)) {
      start = joe_Integer_value (argv[0]);
      blk = argv[1];
   } else {
      joe_Object_assign(retval,
                   joe_Exception_New ("ArrayList foreach: invalid argument(s)"));
      return JOE_FAILURE;
   }

   for (i = 0; i < len; i++) {
      if (i >= start) {
         if ((rc=joe_Object_invoke(blk,"exec",
                                      1, JOE_AT (array, i),
                                             retval)) != JOE_SUCCESS) {
            if (joe_Object_instanceOf (*retval,
                                         &joe_BreakLoopException_Class)) {
               joe_Object retobj = joe_BreakException_getReturnObj(*retval);
               if (retobj)
                  joe_Object_assign(retval, retobj);
               return JOE_SUCCESS;
            } else {
               return JOE_FAILURE;
            }
         }
      }
   }
   return JOE_SUCCESS;
}

static int
set (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 2 && joe_Object_instanceOf (argv[0], &joe_Integer_Class)) {
      int idx = joe_Integer_value (argv[0]);
      joe_Array array = *joe_Object_at (self, ARRAY);

      if (idx >= 0 && idx < (int) joe_ArrayList_length (self)) {
         joe_Object_assign(retval, *JOE_AT(array, idx));
         joe_Object_assign (JOE_AT(array, idx), argv[1]);
         return JOE_SUCCESS;
      } else {
         joe_Object_assign(retval,
                          joe_Exception_New ("ArrayList set: index out of bounds"));
         return JOE_FAILURE;
      }
   } else {
      joe_Object_assign(retval, joe_Exception_New ("ArrayList set: invalid argument"));
      return JOE_FAILURE;
   }
}

static int
toArray (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 0) {
      int i;
      unsigned int len = joe_ArrayList_length (self);
      joe_Array array = *joe_Object_at (self, ARRAY);

      joe_Object_assign (retval, joe_Array_New(len));
      for (i = 0; i < len; i++)
         joe_Object_assign (JOE_AT(*retval, i), *JOE_AT(array, i));

      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval,
                        joe_Exception_New ("ArrayList toArray: invalid argument"));
      return JOE_FAILURE;
   }
}

static int
isEmpty (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 0) {
      unsigned int len = joe_ArrayList_length (self);
      if (len) {
         joe_Object_assign(retval, joe_Boolean_False);
      } else {
         joe_Object_assign(retval, joe_Boolean_True);
      }
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval, joe_Exception_New ("ArrayList: invalid argument"));
      return JOE_FAILURE;
   }
}


static joe_Method mthds[] = {
   {"add", add },
   {"get", get },
   {"set", set },
   {"isEmpty", isEmpty },
   {"length", length },
   {"size", length },
   {"foreach", foreach },
   {"toArray", toArray },
   {(void *) 0, (void *) 0}
};

joe_Class joe_ArrayList_Class = {
   "joe_ArrayList",
   ctor,
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
   joe_Object_assign (JOE_AT(self,ARRAY),
                      joe_Object_setAcyclic (joe_Array_New(maxLength)));
   joe_Object_assign (JOE_AT(self,LENGTH),joe_int_New0 ());
   joe_Object_assign (JOE_AT(self,MAXLENGTH),
                      joe_int_New (maxLength));
   joe_Object_setAcyclic (self);
   return self;
}

unsigned int
joe_ArrayList_length (joe_Object self)
{
   return (unsigned int) JOE_INT (*JOE_AT(self,LENGTH));
}

unsigned int
joe_ArrayList_maxLength (joe_Object self)
{
   return (unsigned int) JOE_INT (*JOE_AT(self,MAXLENGTH));
}

joe_Object *
joe_ArrayList_at (joe_Object self, unsigned int idx)
{
   unsigned int len = joe_ArrayList_length (self);
   if (idx < len) 
      return JOE_AT(*JOE_AT(self,ARRAY), idx);
   else
      return 0;
}

joe_Object
joe_ArrayList_getArray (joe_Object self)
{
   return *JOE_AT(self,ARRAY);
}
void
joe_ArrayList_add (joe_Object self, joe_Object item)
{
   joe_Object *selfVars = joe_Object_array (self);
   joe_int objLen = selfVars[LENGTH];
   joe_int objMaxLen = selfVars[MAXLENGTH];
   joe_Object *theArray = &selfVars[ARRAY];
   unsigned int *len = (unsigned int *) JOE_INT_STAR (objLen);
   unsigned int *maxLen = (unsigned int *) JOE_INT_STAR (objMaxLen);

   if (*len == *maxLen) {
      unsigned int i;
      joe_Array nArray = joe_Object_setAcyclic(joe_Array_New(*maxLen << 1));
      for (i = 0; i < *len; i++) {
         *JOE_AT(nArray, i) = *JOE_AT(*theArray, i);
         *JOE_AT(*theArray, i) = 0;
         (*maxLen)++;
      }
      joe_Object_assign (theArray, nArray);
   }
   joe_Object_assign (JOE_AT(*theArray, *len), item);
   (*len)++;
}
