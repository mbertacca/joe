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

# include "joe_Assignment.h"
# include "joe_Block.h"
# include "joe_Variable.h"

extern void joe_Block_setVariable (joe_Block self,joe_Variable var,joe_Object value);

int
joe_Assignment_assign (joe_Object self,
                       int argc, joe_Object *args, joe_Object *retval)
{
   joe_Block_setVariable (args[0], args[1], args[2]);
   joe_Object_assign (retval, args[2]);
   return JOE_SUCCESS;
}

static joe_Method mthds[] = {
  {":=", joe_Assignment_assign },
  {(void *) 0, (void *) 0}
};

joe_Class joe_Assignment_Class = {
   "joe_Assignment",
   0,
   0,
   mthds,
   0,
   &joe_Object_Class,
   0
};

joe_Object
joe_Assignment_New ()
{
   joe_Object self;
   self = joe_Object_New (&joe_Assignment_Class, 0);
   return self;
}
