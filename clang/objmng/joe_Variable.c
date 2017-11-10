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

# include <string.h>
# include "joe_Variable.h"
# include "joe_String.h"

static joe_Method mthds[] = {
  {(void *) 0, (void *) 0}
};

joe_Class joe_Variable_Class = {
   "joe_Variable",
   0,
   0,
   mthds,
   0,
   &joe_String_Class,
   0
};

joe_Object
joe_Variable_New (char *c) {
   int len = strlen (c);
   joe_Object Return;

   Return = joe_Object_New (&joe_Variable_Class, len + 1);
   strcpy (*((char **) joe_Object_getMem(Return)), c);
   return Return;
}
 
char *
joe_String_name (joe_String self)
{
   return *((char **) joe_Object_getMem(self));
}
