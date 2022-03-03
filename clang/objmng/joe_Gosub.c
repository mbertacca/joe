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

# include "joe_Gosub.h"

static char* variables[] = { "label","returnObj" };

static joe_Method mthds[] = {
  {(void *) 0, (void *) 0}
};

joe_Class joe_Gosub_Class = {
   "joe_Gosub",
   0,
   0,
   mthds,
   variables,
   &joe_Object_Class,
   0
};

joe_Object
joe_Gosub_New (joe_String label) {
   joe_Object self;
   self = joe_Object_New (&joe_Gosub_Class, 0);
   joe_Object_assign (joe_Object_getVar (self, "label"), label);

   return self;
}

char *
joe_Gosub_getLabelCharStar (joe_Object self)
{
   return joe_String_getCharStar (*joe_Object_getVar (self, "label"));
}

void
joe_Gosub_setReturnObj (joe_Object self, joe_Object obj)
{
   joe_Object_assign (joe_Object_getVar (self, "returnObj"), obj);
}

joe_Object
joe_Gosub_getReturnObj (joe_Object self)
{
   return *joe_Object_getVar (self, "returnObj");
}

int
joe_Gosub_hasReturnObj (joe_Object self)
{
   return *joe_Object_getVar (self, "returnObj") == 0;
}
