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

# include "joe_Pointer.h"
# include <stdio.h>
# include "joe_String.h"
# include "joe_Integer.h"
# include "joe_Boolean.h"

static int
toString (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   char buff[32];
   sprintf (buff, "%p", *((void **) joe_Object_getMem(self)));
   joe_Object_assign(retval, joe_String_New (buff));
   return JOE_SUCCESS;
}


static int
stringValue (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   joe_Object_assign(retval,
                     joe_String_New (*((char **) joe_Object_getMem(self))));
   return JOE_SUCCESS;
}


static int
intValue (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   joe_Object_assign(retval,
                     joe_Integer_New (*((int *) joe_Object_getMem(self))));
   return JOE_SUCCESS;
}

static int
longValue (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   joe_Object_assign(retval,
                     joe_Integer_New (*((long *) joe_Object_getMem(self))));
   return JOE_SUCCESS;
}

static int
shortValue (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   joe_Object_assign(retval,
                     joe_Integer_New (*((short *) joe_Object_getMem(self))));
   return JOE_SUCCESS;
}

static int
isNull (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (*((void **) joe_Object_getMem(self)) == NULL)
      joe_Object_assign(retval, joe_Boolean_New_true());
   else
      joe_Object_assign(retval, joe_Boolean_New_false());
   return JOE_SUCCESS;
}

static joe_Method mthds[] = {
   {"isNull", isNull },
   {"shortValue", shortValue },
   {"intValue", intValue },
   {"longValue", longValue },
   {"stringValue", stringValue },
   {"toString", toString },
   {(void *) 0, (void *) 0}
};

joe_Class joe_Pointer_Class = {
   "joe_Pointer",
   0,
   0,
   mthds,
   0,
   &joe_Object_Class,
   0
};

joe_Object
joe_Pointer_New (void *value)
{
   joe_Object self;
   self = joe_Object_New (&joe_Pointer_Class, 0);
   *((void **) joe_Object_getMem(self)) = value;
   return self;
}

void *
joe_Pointer_value (joe_Object self)
{
   return  *((void **) joe_Object_getMem(self));
}