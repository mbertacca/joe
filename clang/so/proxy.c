/*
 * This source file is part of the "Joe Objects Executor" open source project
 *
 * Copyright 2026 Marco Bertacca (www.bertacca.eu)
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
# include <stdlib.h>
# include "joe_Exception.h"
# include "joe_Invocation.h"
# include "joe_String.h"

# include <stdio.h>

char *variables[] = {"object", 0 };
joe_Class *Invocation_Class;

static int ctor (joe_Object self, int argc, joe_Object *argv, joe_Object *retval);

static int
doesNotUnderstand (joe_Object self,
                   int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1 && joe_Object_instanceOf(argv[0], Invocation_Class)) {
      joe_String selName = 0;
      int rc = joe_Object_invoke (argv[0], "selector", 0, 0, &selName);

      printf ("@@@>=%s\n", joe_String_getCharStar(selName));
      joe_Object_assign (&selName, 0);

      rc = joe_Object_invoke (argv[0], "sendTo",
                         1, joe_Object_getVar (self, "object"), retval);
      return rc;
   }
   joe_Object_assign(retval,
                  joe_Exception_New ("doesNotUnderstand: invalid arguments"));
   return JOE_FAILURE;
}

static joe_Method methods[] = {
   {"doesNotUnderstand", doesNotUnderstand },
   {(void *) 0, (void *) 0}
};

static joe_Class joe_Proxy_Class = {
   "example_Proxy",
   ctor,
   0,
   methods,
   variables,
   0/* &joe_Object_Class */,
   0
};

static int
ctor (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1) {
      joe_Object_assign (joe_Object_getVar (self, "object"), argv[0]);
   } else {
      joe_Object_assign(retval,
                  joe_Exception_New ("exampleProxy ctor: invalid arguments"));
      return JOE_FAILURE;
   }
   return JOE_SUCCESS;
}

JOEOBJ_API void
joe_init ()
{
   Invocation_Class = joe_Class_getClass("joe_Invocation");
   joe_Proxy_Class.extends = joe_Class_getClass("joe_Object");
   joe_Class_registerClass (&joe_Proxy_Class);
}
