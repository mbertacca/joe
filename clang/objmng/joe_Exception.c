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
# include <stdlib.h>
# include <string.h>
# include "joe_Exception.h"
# include "joe_String.h"
# include "joe_ArrayList.h"

/**
# Class joe_Exception
### extends joe_Object

This class implements a JOE Exception. An instance of thi class can
be obtained with the following call:
```
   !newInstance "joe_Exception" [ ,aString ]
```
where _aString_ is the exception message when specified.
*/

# define MESSAGE 0
# define STACK 1
# define CAUSED_BY 2
static char *variables[] = { "message", "stack", "causedBy", 0};

static int
ctor (joe_Exception self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc > 0) {
      joe_String msg = 0;
      joe_Object_invoke (argv[0], "toString", 0, 0, &msg);
      joe_Exception_Init (self, msg);
      joe_Object_assign (&msg, 0);
   } else {
      joe_Exception_Init (self, joe_String_New(""));
   }
   return JOE_SUCCESS;
}

/**
## toString

Returns a string representation of this exception
*/
int
joe_Exception_toString (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   joe_ArrayList arList = *joe_Object_at (self,STACK);
   size_t arListLen = joe_ArrayList_length (arList);
   joe_String tmp = 0;

   joe_Object_assign (retval, joe_String_New3 (joe_Object_getClassName(self),
               ": ", joe_String_getCharStar(*joe_Object_at (self,MESSAGE))));

   if (arListLen > 0) {
      size_t i;
      joe_String newLine = joe_String_New ("\r\n");

      joe_Object_invoke (*retval, "add", 1, &newLine, &tmp);
      joe_Object_assign(retval, tmp);
      joe_Object_assign(&tmp, 0);
      for (i = 0; i < arListLen; i++) {
         joe_Object_invoke(*retval,"add",1,joe_ArrayList_at(arList,i),&tmp);
         joe_Object_assign(retval, tmp);
         joe_Object_assign(&tmp, 0);
      }

      joe_Object_delIfUnassigned (&newLine);
   }

   return JOE_SUCCESS;
}

/**
## getMessage

Returns a string with the message associated to this exception
*/

int
joe_Exception_getMessage (joe_Object self,
                          int argc, joe_Object *argv, joe_Object *retval)
{
   joe_Object_assign(retval, *joe_Object_at (self,MESSAGE));
   return JOE_SUCCESS;
}

/**
## throw

Raises this exception
*/

static int
_throw (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   joe_Object_assign(retval, self);
   return JOE_FAILURE;
}

static joe_Method mthds[] = {
  {"throw", _throw },
  {"getMessage", joe_Exception_getMessage },
  {"toString", joe_Exception_toString },
  {(void *) 0, (void *) 0}
};

joe_Class joe_Exception_Class = {
   "joe_Exception",
   ctor,
   0,
   mthds,
   variables,
   &joe_Object_Class,
   0
};

void
joe_Exception_Init (joe_Exception self, joe_String desc)
{
   joe_Object_assign (joe_Object_at (self,MESSAGE), desc);
   joe_Object_assign (joe_Object_at (self,STACK),
                      joe_ArrayList_New (8));
}

joe_Object
joe_Exception_New (char *desc) {
   return joe_Exception_New_string(joe_String_New(desc));
}

joe_Object
joe_Exception_New_string (joe_String desc)
{
   joe_Object self;
   self = joe_Object_New (&joe_Exception_Class, 0);
   joe_Exception_Init (self, desc);
   return self;
}

static const char* stackItemFmt = " ->file=%s, row=%d, col=%d\r\n";

void
joe_Exception_addStack (joe_Exception self, char *fileName, int row, int col)
{
   char *msg = calloc (1, strlen (fileName) + strlen (stackItemFmt) + 32);
   joe_String stkItem = 0;
   joe_ArrayList arList = *joe_Object_at (self,STACK);

   sprintf (msg, stackItemFmt, fileName, row, col);
   stkItem = joe_String_New (msg);
   joe_ArrayList_add (arList, stkItem);

   free (msg);
}
