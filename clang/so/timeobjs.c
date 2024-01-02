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

# include <stdlib.h>
# include "joe_Integer.h"
# include "joe_String.h"

# include <stdio.h>

char *timeVariables[] = {"hour", "min", "sec", 0 };

static int timeCtor (joe_Object self, int argc, joe_Object *argv, joe_Object *retval);

static int
setHour (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   joe_Object_assign (joe_Object_getVar (self, "hour"), argv[0]);
   joe_Object_assign(retval, self);

   return JOE_SUCCESS;
}

static int
setMin (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   joe_Object_assign (joe_Object_getVar (self, "min"), argv[0]);
   joe_Object_assign(retval, self);

   return JOE_SUCCESS;
}

static int
setSec (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   joe_Object_assign (joe_Object_getVar (self, "sec"), argv[0]);
   joe_Object_assign(retval, self);

   return JOE_SUCCESS;
}

static int
toString (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   char *buff = calloc (1, 32);
   sprintf (buff, "%02ld:%02ld:%02ld",
                  joe_Integer_value(*joe_Object_getVar (self, "hour")),
                  joe_Integer_value(*joe_Object_getVar (self, "min")),
                  joe_Integer_value(*joe_Object_getVar (self, "sec")));
   joe_Object_assign(retval, joe_String_New (buff));
   free (buff);
   return JOE_SUCCESS;
}

static joe_Method timeMthds[] = {
   {"setHour", setHour },
   {"setMin", setMin },
   {"setSec", setSec },
   {"toString", toString },
   {(void *) 0, (void *) 0}
};

static joe_Class joe_Time_Class = {
   "example_Time",
   timeCtor,
   0,
   timeMthds,
   timeVariables,
   0 /* &joe_Object_Class */,
   0
};

static int
timeCtor (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   joe_Object_assign (joe_Object_getVar (self, "hour"),
                      joe_Integer_New (0));
   joe_Object_assign (joe_Object_getVar (self, "min"),
                      joe_Integer_New (0));
   joe_Object_assign (joe_Object_getVar (self, "sec"),
                      joe_Integer_New (0)); 
   return JOE_SUCCESS;
}

/* timestamp */

char *timeStampVariables[] = {"year", "month", "day", 0 };

static int timeStampCtor (joe_Object self, int argc, joe_Object *argv, joe_Object *retval);

static int
setYear (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   joe_Object_assign (joe_Object_getVar (self, "year"), argv[0]);
   joe_Object_assign(retval, self);

   return JOE_SUCCESS;
}

static int
setMonth (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   joe_Object_assign (joe_Object_getVar (self, "month"), argv[0]);
   joe_Object_assign(retval, self);

   return JOE_SUCCESS;
}

static int
setDay (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   joe_Object_assign (joe_Object_getVar (self, "day"), argv[0]);
   joe_Object_assign(retval, self);

   return JOE_SUCCESS;
}

static int
ts_toString (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   char *buff = calloc (1, 32);
   joe_String msg = 0;
   joe_String sup = 0;

   sprintf (buff, "%02ld/%02ld/%02ld ",
                  joe_Integer_value(*joe_Object_getVar (self, "year")),
                  joe_Integer_value(*joe_Object_getVar (self, "month")),
                  joe_Integer_value(*joe_Object_getVar (self, "day")));

   joe_Object_invokeSuper (self, "toString", 0, 0, &sup);
   joe_Object_assign (&msg, joe_String_New (buff));
   joe_Object_invoke (msg, "add", 1, &sup, retval);
   joe_Object_assign (&msg, 0);
   joe_Object_assign (&sup, 0);
   free (buff);
   return JOE_SUCCESS;
}

static joe_Method timeStampMthds[] = {
   {"setYear", setYear },
   {"setMonth", setMonth },
   {"setDay", setDay },
   {"toString", ts_toString },
   {(void *) 0, (void *) 0}
};

static joe_Class joe_TimeStamp_Class = {
   "example_TimeStamp",
   timeStampCtor,
   0,
   timeStampMthds,
   timeStampVariables,
   &joe_Time_Class,
   0
};

static int
timeStampCtor (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   timeCtor (self, 0, 0, retval);
   joe_Object_assign (joe_Object_getVar (self, "year"),
                      joe_Integer_New (0));
   joe_Object_assign (joe_Object_getVar (self, "month"),
                      joe_Integer_New (0));
   joe_Object_assign (joe_Object_getVar (self, "day"),
                      joe_Integer_New (0)); 
   return JOE_SUCCESS;
}


JOEOBJ_API void
joe_init ()
{
   joe_Time_Class.extends = joe_Class_getClass("joe_Object");
   joe_Class_registerClass (&joe_Time_Class);
   joe_Class_registerClass (&joe_TimeStamp_Class);
}
