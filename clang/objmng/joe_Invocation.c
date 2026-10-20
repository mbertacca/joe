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

/**
# Class joe_Invocation
### extends joe_Object

Represents a reified message invocation within JOE.
Encapsulates an unhandled message call (when the target
object does not possess the requested method), containing
its selector, arguments, and execution block. Enables
dynamic inspection, forwarding, or re-execution via
fallback mechanisms such as _doesNotUnderstand_.
*/

# include "joe_Invocation.h"
# include "joe_Memory.h"
# include "joe_Integer.h"
# include "joe_Array.h"
# include "joe_Exception.h"
# include "joestrct.h"
# include <string.h>

#define SELECTOR 0
#define ARGC 1
#define ARGS 2

static char *variables[] = { "selector", "argc", "args", 0 };

static int
ctor (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   joe_Object_assign(retval, joe_Exception_New (
              "joe_Invocation ctor: this class cannot be instantiated"));
   return JOE_FAILURE;
}

/**
## selector
Returns the name of the message selector as a String.
*/
static int
selector (joe_Object self, int argc, joe_Object *args, joe_Object *retval)
{
   joe_Object_assign (retval, joe_String_New (
                             joe_Selector_name (*JOE_AT(self,SELECTOR))));
   return JOE_SUCCESS;
}

/**
## args
Returns the arguments associated with this invocation as an Array.
*/
static int
args (joe_Object self, int argc, joe_Object *args, joe_Object *retval)
{
   int i;
   joe_Memory mem = *JOE_AT(self,ARGS);
   int retArgc = joe_Integer_value(*JOE_AT(self,ARGC));
   joe_Object *retArgs = (joe_Object *) JOE_MEM (mem);
   joe_Array argsArray = joe_Array_New (retArgc);

   joe_Object_assign(retval, argsArray);

   for (i = 0; i < retArgc; i++)
      joe_Object_assign (JOE_AT(argsArray, i), retArgs[i]);

   return JOE_SUCCESS;
}

/**
## sendTo _aInvocation_
Forwards and re-sends the message supplied as argument to a target object.
Returns the result of the invocation.
*/
static int
sendTo (joe_Object self, int argc, joe_Object *args, joe_Object *retval)
{
   if (argc == 1) {
      int selfArgc = joe_Integer_value(*JOE_AT(self,ARGC));
      joe_Object *selfArgs = (joe_Object *) JOE_MEM (*JOE_AT(self,ARGS));

      return joe_Selector_invoke (*JOE_AT(self,SELECTOR), args[0],
                                   selfArgc, selfArgs, retval);
   } else {
      joe_Object_assign(retval,
           joe_Exception_New ("Invocation sendTo: invalid argument(s)"));
      return JOE_FAILURE;
   }
}


static joe_Method mthds[] = {
  {"selector", selector},
  {"args", args},
  {"sendTo", sendTo},
  {(void *) 0, (void *) 0}
};

joe_Class joe_Invocation_Class = {
   "joe_Invocation",
   ctor,
   0,
   mthds,
   variables,
   &joe_Object_Class,
   0
};


joe_Invocation
joe_Invocation_New (joe_Selector selector, int argc, joe_Object *args) {
   joe_Invocation self = joe_Object_New(&joe_Invocation_Class, 0);
   joe_Memory mem = joe_Memory_New (sizeof (joe_Object*) * argc);

   memcpy (JOE_MEM (mem), args, sizeof (joe_Object*) * argc);

   joe_Object_assign(JOE_AT(self,SELECTOR), selector);
   joe_Object_assign(JOE_AT(self,ARGC), joe_Integer_New(argc));
   joe_Object_assign(JOE_AT(self,ARGS), mem);
   return self;
}

joe_Selector
joe_Invocation_selector (joe_Selector self)
{
   return *JOE_AT(self,SELECTOR);
}
