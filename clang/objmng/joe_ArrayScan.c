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

# include "joe_ArrayScan.h"
# include "joe_Integer.h"
# include "joe_Boolean.h"
# include "joe_Exception.h"

# define ARRAY 0
# define IDX 1
static char *variables[] = { "array", "idx", 0 };

static void
init (joe_Object self, joe_Array array)
{
   joe_Object_assign (joe_Object_at(self, ARRAY), array);
   joe_Object_assign (joe_Object_at(self, IDX), joe_Integer_New (-1));
}

static int
ctor (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1 && joe_Object_instanceOf (argv[0], &joe_Array_Class)) {
      init (self, argv[0]);
      *retval = self;
      return JOE_SUCCESS;
   } else {
     *retval = joe_Exception_New ("ArrayScan: invalid argument");
      return JOE_FAILURE;
   }

   return JOE_SUCCESS;
}

static int
hasNext (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (joe_ArrayScan_hasNext (self))
      *retval = joe_Boolean_New_true();
   else
      *retval = joe_Boolean_New_false();
   return JOE_SUCCESS;
}

static int
next (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (joe_ArrayScan_hasNext (self)) {
      *retval = joe_ArrayScan_next(self);
      return JOE_SUCCESS;
   } else {
      *retval = joe_Exception_New ("ArrayScan: no next item");
      return JOE_FAILURE;
   }
}

static int
peek (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (joe_ArrayScan_hasPeek (self)) {
      *retval = joe_ArrayScan_peek(self);
      return JOE_SUCCESS;
   } else {
      *retval = joe_Exception_New ("ArrayScan: no current item");
      return JOE_FAILURE;
   }
}

static int
hasPeek (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (joe_ArrayScan_hasPeek (self))
      *retval = joe_Boolean_New_true();
   else
      *retval = joe_Boolean_New_false();
   return JOE_SUCCESS;
}

static int
back (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (joe_ArrayScan_hasPeek (self)) {
      joe_ArrayScan_back(self);
      *retval = 0;
      return JOE_SUCCESS;
   } else {
      *retval = joe_Exception_New ("ArrayScan: no previous item");
      return JOE_FAILURE;
   }
}

static joe_Method mthds[] = {
  {"hasNext", hasNext },
  {"next", next },
  {"peek", peek },
  {"hasPeek", hasPeek },
  {"back", back },
  {(void *) 0, (void *) 0}
};

joe_Class joe_ArrayScan_Class = {
   "joe_ArrayScan",
   ctor,
   0,
   mthds,
   variables,
   &joe_Object_Class,
   0
};

joe_Object
joe_ArrayScan_New (joe_Array array)
{
   joe_Object self;
   self = joe_Object_New (&joe_ArrayScan_Class, 0);
   init (self, array);

   return self;
}

int
joe_ArrayScan_hasNext (joe_ArrayScan self)
{
   int Return = 0;
   joe_Object array = *joe_Object_at(self, ARRAY);
   joe_Object joeidx = *joe_Object_at(self, IDX);
   int arrayLen = joe_Array_length (array);
   int idx = joe_Integer_value (joeidx);
   if (++idx < arrayLen) {
      Return = 1;
   }
   return Return;
}

joe_Object
joe_ArrayScan_next (joe_ArrayScan self)
{
   joe_Object Return = 0;
   joe_Object array = *joe_Object_at(self, ARRAY);
   joe_Object joeidx = *joe_Object_at(self, IDX);
   int arrayLen = joe_Array_length (array);
   int idx = joe_Integer_value (joeidx);
   if (++idx < arrayLen) {
      Return = *joe_Object_at (array, idx);
      joe_Integer_addMe1 (joeidx);
   }
   return Return;
}

int
joe_ArrayScan_hasPeek (joe_ArrayScan self)
{
   int Return = 0;
   joe_Object joeidx = *joe_Object_at(self, IDX);
   int idx = joe_Integer_value (joeidx);
   if (idx >= 0) {
      Return = 1;
   }
   return Return;
}

void
joe_ArrayScan_back (joe_ArrayScan self)
{
   joe_Object joeidx = *joe_Object_at(self, IDX);
   int idx = joe_Integer_value (joeidx);
   if (idx >= 0) {
      idx--;
      joe_Integer_subMe1 (joeidx);
   }
}

joe_Object
joe_ArrayScan_peek (joe_ArrayScan self)
{
   joe_Object Return = 0;
   joe_Object array = *joe_Object_at(self, ARRAY);
   joe_Object joeidx = *joe_Object_at(self, IDX);
   int idx = joe_Integer_value (joeidx);
   if (idx >= 0) {
      Return = *joe_Object_at (array, idx);
   }
   return Return;
}
