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

# include "joe_Float.h"
# include "joe_Integer.h"
# include "joe_Boolean.h"
# include "joe_String.h"
# include "joe_Exception.h"

# include <stdio.h>

static int
add (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1) {
      if (joe_Object_instanceOf (argv[0], &joe_Float_Class)) {
         *retval = joe_Float_New (*((double *) joe_Object_getMem (self)) +
                                   *((double *) joe_Object_getMem (argv[0])));
         return JOE_SUCCESS;
      } else if (joe_Object_instanceOf (argv[0], &joe_Integer_Class)) {
         *retval = joe_Float_New (*((double *) joe_Object_getMem (self)) +
                                     *((long *) joe_Object_getMem (argv[0])));
         return JOE_SUCCESS;
      }
   }
   *retval = joe_Exception_New ("Float add: invalid argument");
   return JOE_FAILURE;
}

static int
subtract (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1) {
      if (joe_Object_instanceOf (argv[0], &joe_Float_Class)) {
         *retval = joe_Float_New (*((double *) joe_Object_getMem (self)) -
                                   *((double *) joe_Object_getMem (argv[0])));
         return JOE_SUCCESS;
      } else if (joe_Object_instanceOf (argv[0], &joe_Integer_Class)) {
         *retval = joe_Float_New (*((double *) joe_Object_getMem (self)) -
                                     *((long *) joe_Object_getMem (argv[0])));
         return JOE_SUCCESS;
      }
   }
   *retval = joe_Exception_New ("Float subtract: invalid argument");
   return JOE_FAILURE;
}

static int
multiply (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1) {
      if (joe_Object_instanceOf (argv[0], &joe_Float_Class)) {
         *retval = joe_Float_New (*((double *) joe_Object_getMem (self)) *
                                   *((double *) joe_Object_getMem (argv[0])));
         return JOE_SUCCESS;
      } else if (joe_Object_instanceOf (argv[0], &joe_Integer_Class)) {
         *retval = joe_Float_New (*((double *) joe_Object_getMem (self)) *
                                     *((long *) joe_Object_getMem (argv[0])));
         return JOE_SUCCESS;
      }
   }
   *retval = joe_Exception_New ("Float multiply: invalid argument");
   return JOE_FAILURE;
}

static int
divide (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1) {
      if (joe_Object_instanceOf (argv[0], &joe_Float_Class)) {
         *retval = joe_Float_New (*((double *) joe_Object_getMem (self)) /
                                   *((double *) joe_Object_getMem (argv[0])));
         return JOE_SUCCESS;
      } else if (joe_Object_instanceOf (argv[0], &joe_Integer_Class)) {
         *retval = joe_Float_New (*((double *) joe_Object_getMem (self)) /
                                     *((long *) joe_Object_getMem (argv[0])));
         return JOE_SUCCESS;
      }
   }
   *retval = joe_Exception_New ("Float divide: invalid argument");
   return JOE_FAILURE;
}

static int
equals (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1) {
      if (joe_Object_instanceOf(argv[0], &joe_Float_Class)) {
         if (*((double *) joe_Object_getMem (self)) ==
             *((double *) joe_Object_getMem (argv[0])))
            *retval = joe_Boolean_New_true();
         else
            *retval = joe_Boolean_New_false();
      } else if (joe_Object_instanceOf(argv[0], &joe_Integer_Class)) {
         if (*((double *) joe_Object_getMem (self)) ==
               *((long *) joe_Object_getMem (argv[0])))
            *retval = joe_Boolean_New_true();
         else
            *retval = joe_Boolean_New_false();
     } else
        *retval = joe_Boolean_New_false();
     return JOE_SUCCESS;
   }
   *retval = joe_Exception_New ("Float equals: invalid argument");
   return JOE_FAILURE;
}

static int
ne (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1) {
      if (joe_Object_instanceOf(argv[0], &joe_Float_Class)) {
         if (*((double *) joe_Object_getMem (self)) !=
             *((double *) joe_Object_getMem (argv[0])))
            *retval = joe_Boolean_New_true();
         else
            *retval = joe_Boolean_New_false();
         return JOE_SUCCESS;
      } else if (joe_Object_instanceOf(argv[0], &joe_Integer_Class)) {
         if (*((double *) joe_Object_getMem (self)) !=
               *((long *) joe_Object_getMem (argv[0])))
            *retval = joe_Boolean_New_true();
         else
            *retval = joe_Boolean_New_false();
         return JOE_SUCCESS;
      } else {
        *retval = joe_Exception_New("!=: invalid argument");
         return JOE_FAILURE;
      }
   } else {
      *retval = joe_Exception_New("!=: invalid argument number");
      return JOE_FAILURE;
   }
}

static int
ge (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1) {
      if (joe_Object_instanceOf(argv[0], &joe_Float_Class)) {
         if (*((double *) joe_Object_getMem (self)) >=
             *((double *) joe_Object_getMem (argv[0])))
            *retval = joe_Boolean_New_true();
         else
            *retval = joe_Boolean_New_false();
         return JOE_SUCCESS;
      } else if (joe_Object_instanceOf(argv[0], &joe_Integer_Class)) {
         if (*((double *) joe_Object_getMem (self)) >=
               *((long *) joe_Object_getMem (argv[0])))
            *retval = joe_Boolean_New_true();
         else
            *retval = joe_Boolean_New_false();
         return JOE_SUCCESS;
      } else {
        *retval = joe_Exception_New(">=: invalid argument");
         return JOE_FAILURE;
      }
   } else {
      *retval = joe_Exception_New(">=: invalid argument number");
      return JOE_FAILURE;
   }
}

static int
gt (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1) {
      if (joe_Object_instanceOf(argv[0], &joe_Float_Class)) {
         if (*((double *) joe_Object_getMem (self)) >
             *((double *) joe_Object_getMem (argv[0])))
            *retval = joe_Boolean_New_true();
         else
            *retval = joe_Boolean_New_false();
         return JOE_SUCCESS;
      } else if (joe_Object_instanceOf(argv[0], &joe_Integer_Class)) {
         if (*((double *) joe_Object_getMem (self)) >
               *((long *) joe_Object_getMem (argv[0])))
            *retval = joe_Boolean_New_true();
         else
            *retval = joe_Boolean_New_false();
         return JOE_SUCCESS;
      } else {
        *retval = joe_Exception_New(">: invalid argument");
         return JOE_FAILURE;
      }
   } else {
      *retval = joe_Exception_New(">: invalid argument number");
      return JOE_FAILURE;
   }
}

static int
le (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1) {
      if (joe_Object_instanceOf(argv[0], &joe_Float_Class)) {
         if (*((double *) joe_Object_getMem (self)) <=
             *((double *) joe_Object_getMem (argv[0])))
            *retval = joe_Boolean_New_true();
         else
            *retval = joe_Boolean_New_false();
         return JOE_SUCCESS;
      } else if (joe_Object_instanceOf(argv[0], &joe_Integer_Class)) {
         if (*((double *) joe_Object_getMem (self)) <=
               *((long *) joe_Object_getMem (argv[0])))
            *retval = joe_Boolean_New_true();
         else
            *retval = joe_Boolean_New_false();
         return JOE_SUCCESS;
      } else {
        *retval = joe_Exception_New("<=: invalid argument");
         return JOE_FAILURE;
      }
   } else {
      *retval = joe_Exception_New("<=: invalid argument number");
      return JOE_FAILURE;
   }
}

static int
lt (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1) {
      if (joe_Object_instanceOf(argv[0], &joe_Float_Class)) {
         if (*((double *) joe_Object_getMem (self)) <
             *((double *) joe_Object_getMem (argv[0])))
            *retval = joe_Boolean_New_true();
         else
            *retval = joe_Boolean_New_false();
         return JOE_SUCCESS;
      } else if (joe_Object_instanceOf(argv[0], &joe_Integer_Class)) {
         if (*((double *) joe_Object_getMem (self)) <
               *((long *) joe_Object_getMem (argv[0])))
            *retval = joe_Boolean_New_true();
         else
            *retval = joe_Boolean_New_false();
         return JOE_SUCCESS;
      } else {
        *retval = joe_Exception_New("!=: invalid argument");
         return JOE_FAILURE;
      }
   } else {
      *retval = joe_Exception_New("!=: invalid argument number");
      return JOE_FAILURE;
   }
}

static int
toString (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   char buff[32];
   sprintf (buff, "%lf", *((double *) joe_Object_getMem(self)));
   *retval = joe_String_New (buff);
   return JOE_SUCCESS;
}

static joe_Method mthds[] = {
   {"add", add },
   {"subtract", subtract },
   {"multiply", multiply },
   {"divide", divide },
   {"equals", equals },
   {"ne", ne },
   {"gt", gt },
   {"ge", ge },
   {"lt", lt },
   {"le", le },
   {"toString", toString },
  {(void *) 0, (void *) 0}
};

joe_Class joe_Float_Class = {
   "joe_Float",
   0,
   0,
   mthds,
   0,
   &joe_Object_Class,
   0
};

joe_Object
joe_Float_New (double value)
{
   joe_Object self;
   self = joe_Object_New (&joe_Float_Class, 0);
   *((double *) joe_Object_getMem(self)) = value;
   return self;
}

double
joe_Float_value (joe_Object self)
{
   return  *((double *) joe_Object_getMem(self));
}
