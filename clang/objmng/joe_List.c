/*
 * This source file is part of the "Joe Objects Executor" open source project
 *
 * Copyright 2021 Marco Bertacca (www.bertacca.eu)
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

# include "joe_List.h"
# include "joe_Array.h"
# include "joe_Boolean.h"
# include "joe_Integer.h"
# include "joe_Exception.h"
# include "joe_BreakLoopException.h"
# include "joe_Null.h"

#define OBJ 0
#define NEXT 1
#define PREV 2

static char *item_vars[] = { "obj", "next", "prev", 0 };

static joe_Class item_clazz = {
   "joe_List$Item",
   0,
   0,
   0,
   item_vars,
   &joe_Object_Class,
   0
};

static joe_Object
item_New ()
{
   joe_Object self = joe_Object_New (&item_clazz, 0);
   joe_Object_assign (joe_Object_at (self, OBJ), 0);
   joe_Object_assign (joe_Object_at (self, PREV), 0);
   joe_Object_assign (joe_Object_at (self, NEXT), 0);
   return self;
}

joe_ListItem
joe_ListItem_next (joe_ListItem self)
{
   return *joe_Object_at (self, NEXT);
}

joe_Object
joe_ListItem_value (joe_ListItem self)
{
   return *joe_Object_at (self, OBJ);
}

# define FIRST  0
# define LAST   1
# define LENGTH 2
static char *variables[] = { "first", "last", "length", 0 };

static int
ctor (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   joe_Object_assign(joe_Object_at(self,FIRST), 0);
   joe_Object_assign(joe_Object_at(self,LAST), 0);
   joe_Object_assign(joe_Object_at(self,LENGTH), joe_Integer_New (0));
   return JOE_SUCCESS;
}

static int
add (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1) {
      joe_List_push (self, argv[0]);
      joe_Object_assign (retval, argv[0]);
   } else {
      joe_Object_assign(retval, joe_Exception_New ("List add: invalid argument"));
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
         joe_ListItem item;
         if (idx > (length >> 1)) {
            item = *joe_Object_at (self, LAST);
            for (idx = length - idx - 1; idx > 0; idx--) {
               item = *joe_Object_at (item, PREV);
            }
         } else {
            item = *joe_Object_at (self, FIRST);
            for (  ; idx > 0; idx--) {
               item = *joe_Object_at (item, NEXT);
            }
         }
         joe_Object_assign(retval, *joe_Object_at (item, OBJ));
      } else {
         joe_Object_assign(retval,
                           joe_Exception_New ("List get: index out of bound"));
         return JOE_FAILURE;
      }
   } else {
      joe_Object_assign(retval,
                        joe_Exception_New ("List get: invalid argument"));
      return JOE_FAILURE;
   }
   return JOE_SUCCESS;
}

static int
peek (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 0) {
      joe_ListItem item = *joe_Object_at (self, LAST);
      if (item)
         joe_Object_assign(retval, *joe_Object_at (item, OBJ));
      else
         joe_Object_assign(retval, joe_Null_value);
   } else {
      joe_Object_assign(retval,
                        joe_Exception_New ("List peek: invalid argument"));
      return JOE_FAILURE;
   }
   return JOE_SUCCESS;
}

static int
pop (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 0) {
      long length = joe_Integer_value (*joe_Object_at (self, LENGTH));
      if (length > 0) {
         joe_List_pop (self, retval);
      } else {
         joe_Object_assign(retval, joe_Exception_New ("List pop: empty list"));
         return JOE_FAILURE;
      }
   } else {
      joe_Object_assign(retval,
                        joe_Exception_New ("List pop: invalid argument"));
      return JOE_FAILURE;
   }
   return JOE_SUCCESS;
}

static int
length (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 0) {
      joe_Object_assign(retval,
                        *joe_Object_at (self, LENGTH));
   } else {
      joe_Object_assign(retval,
                        joe_Exception_New ("List length: invalid argument"));
      return JOE_FAILURE;
   }
   return JOE_SUCCESS;
}

static int
empty (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 0) {
      if (joe_Integer_value(*joe_Object_at(self, LENGTH)) > 0)
         joe_Object_assign(retval, joe_Boolean_New_false());
      else
         joe_Object_assign(retval, joe_Boolean_New_true());
   } else {
      joe_Object_assign(retval,
                        joe_Exception_New ("List empty: invalid argument"));
      return JOE_FAILURE;
   }
   return JOE_SUCCESS;
}


static int
foreach (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   int start;
   joe_Block blk;
   int cnt;
   joe_Object *obj;
   int rc;
   joe_ListItem item = *joe_Object_at (self, FIRST);

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
                   joe_Exception_New ("List foreach: invalid argument(s)"));
      return JOE_FAILURE;
   }

   for (item = *joe_Object_at (self, FIRST), cnt = 0; item != 0;
        item = *joe_Object_at (item, NEXT), cnt++) {
      obj = joe_Object_at (item, OBJ);
      if (cnt >= start) {
         if ((rc=joe_Object_invoke(blk,"exec", 1, obj,
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
static joe_Method mthds[] = {
  {"add", add },
  {"push", add },
  {"pop", pop },
  {"peek", peek },
  {"get", get },
  {"empty", empty },
  {"length", length },
  {"foreach", foreach },
  {(void *) 0, (void *) 0}
};


joe_Class joe_List_Class = {
   "joe_List",
   ctor,
   0,
   mthds,
   variables,
   &joe_Object_Class,
   0
};

joe_List
joe_List_New ()
{
   joe_Object retval = 0;
   joe_List self = joe_Object_New(&joe_List_Class, 0);
   ctor (self, 0, 0, &retval);
   return self;
}

void
joe_List_push (joe_List self, joe_Object obj)
{
   long length = joe_Integer_value(*joe_Object_at(self, LENGTH));
   joe_ListItem item = item_New();
   joe_Object_assign(joe_Object_at(item, OBJ), obj);
   if (length == 0) {
      joe_Object_assign(joe_Object_at(item, NEXT), 0);
      joe_Object_assign(joe_Object_at(item, PREV), 0);
      joe_Object_assign(joe_Object_at(self, FIRST), item);
      joe_Object_assign(joe_Object_at(self, LAST), item);
      joe_Object_assign(joe_Object_at(self, LENGTH), joe_Integer_New(1));
   } else {
      joe_Object lastItem = *joe_Object_at(self, LAST);
      joe_Object_assign(joe_Object_at(item, PREV), lastItem);
      joe_Object_assign(joe_Object_at(lastItem, NEXT), item);
      joe_Object_assign(joe_Object_at(self, LAST), item);
      joe_Object_assign(joe_Object_at(self, LENGTH),
                        joe_Integer_New(++length));
   }
}

void
joe_List_pop(joe_List self, joe_Object *retval)
{
   long length = joe_Integer_value(*joe_Object_at(self, LENGTH));
   if (length > 0) {
      joe_ListItem item = *joe_Object_at(self, LAST);
      joe_ListItem prev = *joe_Object_at(item, PREV);

      joe_Object_assign(retval, *joe_Object_at(item, OBJ));

      if (prev)
         joe_Object_assign(joe_Object_at(prev, NEXT), 0);
      joe_Object_assign(joe_Object_at(self, LAST), prev);
      joe_Object_assign(joe_Object_at(self, LENGTH),
                        joe_Integer_New(--length));
   }
}

int
joe_List_empty(joe_List self)
{
   return !(joe_Integer_value(*joe_Object_at(self, LENGTH)) > 0);
}

joe_ListItem
joe_List_getFirstItem(joe_List self)
{
   return *joe_Object_at (self, FIRST);
}
