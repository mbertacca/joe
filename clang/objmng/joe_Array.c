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

# include <stdlib.h>
# include "joe_Array.h"
# include "joe_ArrayIterator.h"
# include "joe_Integer.h"
# include "joe_Exception.h"
# include "joe_Block.h"
# include "joe_BreakLoopException.h"
# include "joe_Null.h"
# include "joestrct.h"

/**
# Class joe_Array
### extends joe_Object

This class represents an array.

An array can be created using the following methods:
```
aArray := !newInstance "joe_Object", aSize
aArray := !newArray aSize
aArray := !array [ e1  ... ,eN]
```
*/

static int
ctor (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1 && joe_Object_instanceOf (argv[0], &joe_Integer_Class)) {
      unsigned int size = joe_Integer_value (argv[0]);
      joe_Object_assign (retval,  joe_Array_New(size));
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval,
           joe_Exception_New ("Array ctor: invalid argument(s)"));
      return JOE_FAILURE;
   }
}

/**
## length
## size

Returns the length of this array.

*/

static int
length (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   joe_Object_assign (retval, joe_Integer_New (joe_Array_length (self)));
   return JOE_SUCCESS;
}

/**
## get _aIndex_

Returns the _aIndex th_ element of this array.
*/

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

/**
## set _aIndex_, _aObject_

Replaces the element at _aIndex_ position in this array
with _aObject_.
*/

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

/**
## clean

Replaces all the elements with ()
*/

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

/**
## shift

Returns a new Array like this array without the first element
*/

static int
shift (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 0) {
      size_t length = joe_Array_length (self);
      if (--length >= 0) {
         size_t idx;
         joe_Object_assign (retval, joe_Object_New(&joe_Array_Class, length));
         for (idx = 0; idx < length; idx++) {
            joe_Object_assign (JOE_AT(*retval, idx), *JOE_AT(self, idx + 1));
         }
      } else {
         joe_Object_assign (retval, self);
      }
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval, joe_Exception_New("Array shift: no argument expected"));
      return JOE_FAILURE;
   }
}

/**
## unshift _aObject_

Returns a new Array like this array with the addition of _aObject_
as first element
*/

static int
unshift (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1) {
      size_t idx0, idx1;
      size_t length = joe_Array_length (self);
      joe_Object_assign (retval, joe_Object_New(&joe_Array_Class, length + 1));
      joe_Object_assign (JOE_AT(*retval, 0), argv[0]);
 
      for (idx0 = 0, idx1 = 1; idx0 < length; idx0++,idx1++) {
         joe_Object_assign (JOE_AT(*retval, idx1), *JOE_AT(self, idx0));
      }
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval,
               joe_Exception_New("Array unshift: invalid arguments number"));
      return JOE_FAILURE;
   }
}

/**
## slice _fromIndex_, _toIndex_

Returns the specified range of this array as a new array.
The value at _fromIndex_ is placed into the initial element of the new array.
The length of the returned array will be _toIndex_ - _fromIndex_. 
*/

static int
slice (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 2 && joe_Object_instanceOf (argv[0], &joe_Integer_Class)
                 && joe_Object_instanceOf (argv[1], &joe_Integer_Class)) {
      int64_t from = joe_Integer_value (argv[0]);
      int64_t to = joe_Integer_value (argv[1]);
      if (from >= 0 && to >= from) {
         int idx0, idx1;
         size_t newlen = to - from;
         size_t length = joe_Array_length (self);
         joe_Object_assign (retval, joe_Object_New(&joe_Array_Class, newlen));
         for (idx0=0, idx1=from; idx0 < newlen && idx1 < length; idx0++,idx1++)
            joe_Object_assign (JOE_AT(*retval,idx0), *JOE_AT(self,idx1));
         for ( ; idx0 < newlen; idx0++)
            joe_Object_assign (JOE_AT(*retval,idx0), joe_Null_value);
         return JOE_SUCCESS;
      } else {
         joe_Object_assign(retval,
               joe_Exception_New("Array slice: invalid arguments values"));
         return JOE_FAILURE;
      }
   } else {
      joe_Object_assign(retval,
               joe_Exception_New("Array slice: invalid arguments"));
      return JOE_FAILURE;
   }
}

/**
## add _aObject_

Returns a new Array like this array with the addition of _aObject_
as last element 
*/

static int
add (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1) {
      size_t length = joe_Array_length (self);
      size_t idx;
      joe_Object_assign (retval, joe_Object_New(&joe_Array_Class, length+1));
      for (idx = 0; idx < length; idx++) {
          joe_Object_assign (JOE_AT(*retval, idx), *JOE_AT(self, idx));
      }
      joe_Object_assign (JOE_AT(*retval, idx), argv[0]);
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval, joe_Exception_New("Array add: 1 argument expected"));
      return JOE_FAILURE;
   }
}

/**
## foreach [_aFirstIndex_,] _aBlock_

For each element of this array, _aBlock_ is executed passing the index
as argument. If _aFirstIndex_ is specified then only the items with an
index >= of  _aFirstIndex_ will be scanned.
*/

static int
foreach (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   int start;
   joe_Block blk;
   int rc;
   int len = joe_Array_length (self);
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
                   joe_Exception_New ("Array foreach: invalid argument(s)"));
      return JOE_FAILURE;
   }

   joe_Object_assign(retval, joe_Null_value);
   for (i = 0; i < len; i++) {
      if (i >= start) {
         if ((rc=joe_Object_invoke(blk,"exec",
                                      1, JOE_AT (self, i),
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

/**
## iterator

Returns a ArrayIterator for this array
*/	

static int
iterator (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 0) {
      joe_ArrayIterator_New (self, retval);
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval,
              joe_Exception_New ("Array iterator: invalid argument"));
      return JOE_FAILURE;
   }
}

static joe_Method mthds[] = {
  {"length", length },
  {"size", length },
  {"get", get },
  {"set", set },
  {"foreach", foreach },
  {"clean", clean },
  {"shift", shift },
  {"unshift", unshift },
  {"slice", slice },
  {"add", add },
  {"iterator", iterator },
  {(void *) 0, (void *) 0}
};

static char *variables[] = { 0 };

joe_Class joe_Array_Class = {
   "joe_Array",
   ctor,
   0,
   mthds,
   variables,
   &joe_Object_Class,
   0
};

joe_Object
joe_Array_New(size_t length)
{
   return joe_Object_New(&joe_Array_Class, length);
}

size_t
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
      joe_Object_assign(JOE_AT(self, i), joe_Null_value);
   return self;
}

int
joe_Array_add(joe_Object self, joe_Object obj, joe_Object *retval)
{
   return add (self, 1, &obj, retval);
}
