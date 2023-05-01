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
# include "joe_Null.h"
# include "joe_Variable.h"
# include "joe_String.h"
# include "joe_HashMap.h"
# include "joestrct.h"

# define NAME  0
# define HASH  1
# define DEPTH 2
# define INDEX 3
static char *item_vars[] = { "name", "hash","depth", "index",  0 };

static joe_Method mthds[] = {
  {(void *) 0, (void *) 0}
};

joe_Class joe_Variable_Class = {
   "joe_Variable",
   0,
   0,
   mthds,
   item_vars,
   &joe_Object_Class,
   0
};

joe_Object
joe_Variable_New_String (joe_String name, int depth, int index) {
   joe_Object self = joe_Object_New (&joe_Variable_Class, 0);
   unsigned int hash = joe_HashMap_hash (name);

   joe_Object_assign (JOE_AT(self, NAME), name);
   joe_Object_assign (JOE_AT(self, HASH), joe_int_New (hash));
   joe_Object_assign (JOE_AT(self, DEPTH), joe_int_New(depth));
   joe_Object_assign (JOE_AT(self, INDEX), joe_int_New(index));

   return self;
}
/*
joe_Object
joe_Variable_New (char *c) {
   return joe_Variable_New_String (joe_String_New (c));
}
 */
joe_String
joe_Variable_name (joe_Variable self)
{
   return *JOE_AT(self, NAME);
}

char *
joe_Variable_nameCharStar (joe_Variable self)
{
   return joe_String_getCharStar(*JOE_AT(self, NAME));
}

unsigned int
joe_Variable_hash (joe_Variable self)
{
   return  JOE_INT(*JOE_AT(self, HASH));
}

int
joe_Variable_getDepth (joe_Variable self)
{
   return  JOE_INT(*JOE_AT(self, DEPTH));
}

int
joe_Variable_getIndex (joe_Variable self)
{
   return  JOE_INT(*JOE_AT(self, INDEX));
}
