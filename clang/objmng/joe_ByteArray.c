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
# include <string.h>
# include "joe_ByteArray.h"
# include "joe_Memory.h"
# include "joe_Exception.h"
# include "joe_Integer.h"
# include "joestrct.h"

# define MEMORY 0
# define OFFSET 1
# define LENGTH 2

static char *variables[] = { "memory", "offset", "length", 0 };


static void
init (joe_Object self, joe_Object mem, int offset, int length)
{
   if (mem == (void *) 0)
      mem = joe_Memory_New (offset + length);
   joe_Object_assign (JOE_AT (self, MEMORY), mem);
   joe_Object_assign (JOE_AT (self, OFFSET), joe_Integer_New (offset));
   joe_Object_assign (JOE_AT (self, LENGTH), joe_Integer_New (length));
}

static int
ctor (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1 && joe_Object_instanceOf (argv[0],&joe_Integer_Class)) {
      init (self, (void*) 0, 0, JOE_INT (argv[0]));
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval,
           joe_Exception_New ("ByteArray ctor: invalid argument(s)"));
      return JOE_FAILURE;
   }
}

static int
length (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 0) {
      joe_Object_assign(retval, *JOE_AT (self, LENGTH));
   } else {
      joe_Object_assign(retval,
                     joe_Exception_New ("ByteArray length: invalid argument"));
      return JOE_FAILURE;
   }
   return JOE_SUCCESS;
}

static int
_init (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 0 ||
       (argc == 1 && joe_Object_instanceOf (argv[0],&joe_String_Class))) {
      int offset = joe_Integer_value(*JOE_AT (self, OFFSET));
      int length = joe_Integer_value(*JOE_AT (self, LENGTH));
      joe_Object mem = *JOE_AT (self, MEMORY);
      int slen = argc == 1 ? joe_String_length (argv[0]) : 0;
      int i, end = offset + length;
      char c = 0;
      char *charray;

      if (joe_Object_instanceOf (mem, &joe_ByteArray_Class))
         mem =  *JOE_AT (mem, MEMORY);
      charray = (*(char **) joe_Object_getMem(mem));
      if (slen > 0)
         c = *joe_String_getCharStar (argv[0]);
      for (i = offset; i < end; i++)
         charray[i] = c;
      joe_Object_assign(retval, self);
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval,
                     joe_Exception_New ("ByteArray init: invalid arguments"));
      return JOE_FAILURE;
   }
}

static int
set (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1) {
      int offset = joe_Integer_value(*JOE_AT (self, OFFSET));
      int length = joe_Integer_value(*JOE_AT (self, LENGTH));
      joe_Object mem = *JOE_AT (self, MEMORY);

      if (joe_Object_instanceOf (mem, &joe_ByteArray_Class))
         mem =  *JOE_AT (mem, MEMORY);

      if (joe_Object_instanceOf (argv[0],&joe_String_Class)) {
         int minLen = joe_String_length (argv[0]);
         if (minLen > length)
            minLen = length;
         memcpy (&(*(char **) joe_Object_getMem(mem))[offset],
                 joe_String_getCharStar(argv[0]),
                 minLen);
         joe_Object_assign(retval, self);
         return JOE_SUCCESS;
      } else if (joe_Object_instanceOf (argv[0],&joe_ByteArray_Class)) {
         int offsrc = joe_Integer_value (*JOE_AT (argv[0], OFFSET));
         int minLen = joe_Integer_value (*JOE_AT (argv[0], LENGTH));
         joe_Object src = *JOE_AT (argv[0], MEMORY);
         if (joe_Object_instanceOf (src, &joe_ByteArray_Class))
            src = *JOE_AT (src, MEMORY);
         if (minLen > length)
            minLen = length;
         memcpy (&(*(char **) joe_Object_getMem(mem))[offset],
                 &(*(char **) joe_Object_getMem(src))[offsrc],
                 minLen);
         joe_Object_assign(retval, self);
         return JOE_SUCCESS;
      } else {
         joe_Object_assign(retval,
                     joe_Exception_New ("ByteArray set: invalid parameter"));
         return JOE_FAILURE;
      }
   } else {
      joe_Object_assign(retval,
                     joe_Exception_New ("ByteArray set: invalid argument"));
      return JOE_FAILURE;
   }
}

static int
child (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 2 && joe_Object_instanceOf (argv[0],&joe_Integer_Class)
                 && joe_Object_instanceOf (argv[1],&joe_Integer_Class)) {
      joe_ByteArray newobj = joe_ByteArray_New (self,
                                                joe_Integer_value(argv[0]),
                                                joe_Integer_value(argv[1]));
      if (newobj != (void *) 0) {
         joe_Object_assign(retval, newobj);
         return JOE_SUCCESS;
      } else {
         joe_Object_assign(retval,
                     joe_Exception_New ("ByteArray child: invalid parameters"));
         return JOE_FAILURE;
      }
   } else {
      joe_Object_assign(retval,
                     joe_Exception_New ("ByteArray child: invalid arguments"));
      return JOE_FAILURE;
   }
}

static int
toString (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 0) {
      int offset = joe_Integer_value(*JOE_AT (self, OFFSET));
      int length = joe_Integer_value(*JOE_AT (self, LENGTH));
      joe_Object mem = *JOE_AT (self, MEMORY);
      if (joe_Object_instanceOf (mem, &joe_ByteArray_Class))
         mem =  *JOE_AT (mem, MEMORY);
      joe_Object_assign (retval,
               joe_String_New_len (&(*(char **) joe_Object_getMem(mem))[offset],
                                   length));
       return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval,
                     joe_Exception_New ("ByteArray toString: invalid argument"));
      return JOE_FAILURE;
   }
}

static joe_Method mthds[] = {
   {"toString", toString },
   {"init", _init },
   {"set", set },
   {"length", length },
   {"child", child },
   {(void *) 0, (void *) 0}
};

joe_Class joe_ByteArray_Class = {
   "joe_ByteArray",
   ctor,
   0,
   mthds,
   variables,
   &joe_Object_Class,
   0
};

joe_Object
joe_ByteArray_New (joe_Object par, int offset, int length) {
   joe_ByteArray self = (void *) 0;
   if (par == (void *) 0) {
      if (offset >= 0 && length > 0) {
         self = joe_Object_New(&joe_ByteArray_Class, 0);
         init (self, par, offset, length);
      }
   } else {
      joe_Object mem = par;
      while (joe_Object_instanceOf (mem,&joe_ByteArray_Class)) {
         int poffs = joe_Integer_value(*JOE_AT (mem, OFFSET));
         offset += poffs;
         par = mem;
         mem = *JOE_AT (par, MEMORY);
      }
      if (joe_Object_instanceOf (mem,&joe_Memory_Class)) {
         int totlen = joe_Integer_value(*JOE_AT (par, LENGTH));
         if (offset >= 0) {
            if (totlen >= offset + length) {
               self = joe_Object_New(&joe_ByteArray_Class, 0);
               init (self, par, offset, length);
            }
         }
      }
   }
   return self;
}

char *
joe_ByteArray_getCharStar (joe_ByteArray self) {
   int offset = joe_Integer_value(*JOE_AT (self, OFFSET));
   joe_Object mem = *JOE_AT (self, MEMORY);

   if (joe_Object_instanceOf (mem, &joe_ByteArray_Class))
      mem =  *JOE_AT (mem, MEMORY);

   return &(*(char **) joe_Object_getMem(mem))[offset];
}
