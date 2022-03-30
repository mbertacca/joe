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
# include "joe_Variable.h"
# include "joe_String.h"
# include "joe_HashMap.h"
# include "joestrct.h"

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
   int len = strlen (c) + 1;
   int hashOffs = len  +  sizeof(unsigned int) - (len % sizeof(unsigned int));
   joe_Object Return;
   char *mem;
   unsigned int *hash;

   Return = joe_Object_New (&joe_Variable_Class, hashOffs + sizeof(unsigned int));
   mem = *((char **) joe_Object_getMem(Return));
   strcpy (mem, c);
   hash = (unsigned int *)(mem + hashOffs);
   *hash = joe_HashMap_hash (Return);
   return Return;
}
 
joe_String
joe_Variable_name (joe_Variable self)
{
   return self;
}

char *
joe_Variable_nameCharStar (joe_Variable self)
{
   return joe_String_getCharStar(self);
}

unsigned int
joe_Variable_hash (joe_Variable self)
{
   if (JOE_ISCLASS(self, &joe_Variable_Class)) {
      char *mem = *((char **) joe_Object_getMem(self));
      int len = strlen (mem) + 1;
      int hashOffs = len  +  sizeof(unsigned int) - (len % sizeof(unsigned int));
      return *((unsigned int *)(mem + hashOffs));
   } else {
      printf ("Not a joe_Variable class!\n");
      return 0;
   }
}