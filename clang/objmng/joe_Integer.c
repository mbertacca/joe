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

# include "joe_Integer.h"
# include "joe_Float.h"
# include "joe_BigDecimal.h"
# include "joe_Boolean.h"
# include "joe_String.h"
# include "joe_Exception.h"

# include <stdio.h>

static int
operation (joe_Integer self, int argc, joe_Object *argv,
           joe_Object *retval, enum joe_BigDecimal_ops op)
{
   int Return;
   joe_BigDecimal bd = 0;
   joe_Object_assign (&bd, 
              joe_BigDecimal_New_lng (*((long *) joe_Object_getMem (self))));
   Return = joe_BigDecimal_oper (bd, argc, argv, retval, op);
   joe_Object_assign (&bd, 0);
   return Return;
}

static int
add (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1) {
      if (joe_Object_instanceOf (argv[0], &joe_Integer_Class)) {
         *retval = joe_Integer_New (*((long *) joe_Object_getMem (self)) +
                                    *((long *) joe_Object_getMem (argv[0])));
         return JOE_SUCCESS;
      } else if (joe_Object_instanceOf (argv[0], &joe_Float_Class)) {
         *retval = joe_Float_New (*((long *) joe_Object_getMem (self)) +
                                *((double *) joe_Object_getMem (argv[0])));
         return JOE_SUCCESS;
      } else if (joe_Object_instanceOf (argv[0], &joe_BigDecimal_Class)) {
         return operation (self, argc, argv, retval, ADD);
      }
   }
   *retval = joe_Exception_New ("Integer add: invalid argument");
   return JOE_FAILURE;
}

static int
subtract (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1) {
      if (joe_Object_instanceOf (argv[0], &joe_Integer_Class)) {
         *retval = joe_Integer_New (*((long *) joe_Object_getMem (self)) -
                                    *((long *) joe_Object_getMem (argv[0])));
         return JOE_SUCCESS;
      } else if (joe_Object_instanceOf (argv[0], &joe_Float_Class)) {
         *retval = joe_Float_New (*((long *) joe_Object_getMem (self)) -
                                *((double *) joe_Object_getMem (argv[0])));
         return JOE_SUCCESS;
      } else if (joe_Object_instanceOf (argv[0], &joe_BigDecimal_Class)) {
         return operation (self, argc, argv, retval, SUBTRACT);
      }
   }
   *retval = joe_Exception_New ("Integer subtract: invalid argument");
   return JOE_FAILURE;
}

static int
multiply (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1) {
      if (joe_Object_instanceOf (argv[0], &joe_Integer_Class)) {
         *retval = joe_Integer_New (*((long *) joe_Object_getMem (self)) *
                                    *((long *) joe_Object_getMem (argv[0])));
         return JOE_SUCCESS;
      } else if (joe_Object_instanceOf (argv[0], &joe_Float_Class)) {
         *retval = joe_Float_New (*((long *) joe_Object_getMem (self)) *
                                *((double *) joe_Object_getMem (argv[0])));
         return JOE_SUCCESS;
      } else if (joe_Object_instanceOf (argv[0], &joe_BigDecimal_Class)) {
         return operation (self, argc, argv, retval, MULTIPLY);
      }
   }
   *retval = joe_Exception_New ("Integer multiply: invalid argument");
   return JOE_FAILURE;
}

static int
divide (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1) {
      if (joe_Object_instanceOf (argv[0], &joe_Integer_Class)) {
         *retval = joe_Integer_New (*((long *) joe_Object_getMem (self)) /
                                    *((long *) joe_Object_getMem (argv[0])));
         return JOE_SUCCESS;
      } else if (joe_Object_instanceOf (argv[0], &joe_Float_Class)) {
         *retval = joe_Float_New (*((long *) joe_Object_getMem (self)) /
                                *((double *) joe_Object_getMem (argv[0])));
         return JOE_SUCCESS;
      } else if (joe_Object_instanceOf (argv[0], &joe_BigDecimal_Class)) {
         return operation (self, argc, argv, retval, DIVIDE);
      }
   }
   *retval = joe_Exception_New ("Integer divide: invalid argument");
   return JOE_FAILURE;
}

static int
_remainder (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1) {
      if (joe_Object_instanceOf (argv[0], &joe_Integer_Class)) {
         *retval = joe_Integer_New (*((long *) joe_Object_getMem (self)) %
                                    *((long *) joe_Object_getMem (argv[0])));
         return JOE_SUCCESS;
      } else if (joe_Object_instanceOf (argv[0], &joe_Float_Class)) {
          *retval = joe_Exception_New("Integer remainder: not allowed with Float");
         return JOE_FAILURE;
      } else if (joe_Object_instanceOf (argv[0], &joe_BigDecimal_Class)) {
         return operation (self, argc, argv, retval, REMAINDER);
      }
   }
   *retval = joe_Exception_New ("Integer remainder: invalid argument");
   return JOE_FAILURE;
}

static int
equals (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1) {
      if (joe_Object_instanceOf(argv[0], &joe_Integer_Class)) {
         if (*((long *) joe_Object_getMem (self)) ==
             *((long *) joe_Object_getMem (argv[0])))
            *retval = joe_Boolean_New_true();
         else
            *retval = joe_Boolean_New_false();
      } else if (joe_Object_instanceOf(argv[0], &joe_Float_Class)) {
         if (*((long *) joe_Object_getMem (self)) ==
             *((double *) joe_Object_getMem (argv[0])))
            *retval = joe_Boolean_New_true();
         else
            *retval = joe_Boolean_New_false();
      } else if (joe_Object_instanceOf (argv[0], &joe_BigDecimal_Class)) {
         return operation (self, argc, argv, retval, EQUALS);
      } else {
         *retval = joe_Boolean_New_false();
      }
      return JOE_SUCCESS;
   }
   *retval = joe_Exception_New ("Integer equals: invalid argument");
   return JOE_FAILURE;
}

static int
ne (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1) {
      if (joe_Object_instanceOf(argv[0], &joe_Integer_Class)) {
         if (*((long *) joe_Object_getMem (self)) !=
             *((long *) joe_Object_getMem (argv[0])))
            *retval = joe_Boolean_New_true();
         else
            *retval = joe_Boolean_New_false();
         return JOE_SUCCESS;
      } else if (joe_Object_instanceOf(argv[0], &joe_Float_Class)) {
         if (*((long *) joe_Object_getMem (self)) !=
             *((double *) joe_Object_getMem (argv[0])))
            *retval = joe_Boolean_New_true();
         else
            *retval = joe_Boolean_New_false();
         return JOE_SUCCESS;
      } else if (joe_Object_instanceOf (argv[0], &joe_BigDecimal_Class)) {
         return operation (self, argc, argv, retval, NE);
      } else {
        *retval = joe_Exception_New ("!=: invalid argument");
         return JOE_FAILURE;
      }
   } else {
      *retval = joe_Exception_New ("!=: invalid argument");
      return JOE_FAILURE;
   }
}

static int
ge (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1) {
      if (joe_Object_instanceOf(argv[0], &joe_Integer_Class)) {
         if (*((long *) joe_Object_getMem (self)) >=
             *((long *) joe_Object_getMem (argv[0])))
            *retval = joe_Boolean_New_true();
         else
            *retval = joe_Boolean_New_false();
         return JOE_SUCCESS;
      } else if (joe_Object_instanceOf(argv[0], &joe_Float_Class)) {
         if (*((long *) joe_Object_getMem (self)) >=
             *((double *) joe_Object_getMem (argv[0])))
            *retval = joe_Boolean_New_true();
         else
            *retval = joe_Boolean_New_false();
         return JOE_SUCCESS;
      } else if (joe_Object_instanceOf (argv[0], &joe_BigDecimal_Class)) {
         return operation (self, argc, argv, retval, GE);
      } else {
        *retval = joe_Exception_New (">=: invalid argument");
         return JOE_FAILURE;
      }
   } else {
      *retval = joe_Exception_New (">=: invalid argument");
      return JOE_FAILURE;
   }
}

static int
gt (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1) {
      if (joe_Object_instanceOf(argv[0], &joe_Integer_Class)) {
         if (*((long *) joe_Object_getMem (self)) >
             *((long *) joe_Object_getMem (argv[0])))
            *retval = joe_Boolean_New_true();
         else
            *retval = joe_Boolean_New_false();
         return JOE_SUCCESS;
      } else if (joe_Object_instanceOf(argv[0], &joe_Float_Class)) {
         if (*((long *) joe_Object_getMem (self)) >
             *((double *) joe_Object_getMem (argv[0])))
            *retval = joe_Boolean_New_true();
         else
            *retval = joe_Boolean_New_false();
         return JOE_SUCCESS;
      } else if (joe_Object_instanceOf (argv[0], &joe_BigDecimal_Class)) {
         return operation (self, argc, argv, retval, GT);
      } else {
        *retval = joe_Exception_New ("> :invalid argument");
         return JOE_FAILURE;
      }
   } else {
      *retval = joe_Exception_New (">: invalid argument");
      return JOE_FAILURE;
   }
}

static int
le (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1) {
      if (joe_Object_instanceOf(argv[0], &joe_Integer_Class)) {
         if (*((long *) joe_Object_getMem (self)) <=
             *((long *) joe_Object_getMem (argv[0])))
            *retval = joe_Boolean_New_true();
         else
            *retval = joe_Boolean_New_false();
         return JOE_SUCCESS;
      } else if (joe_Object_instanceOf(argv[0], &joe_Float_Class)) {
         if (*((long *) joe_Object_getMem (self)) <=
             *((double *) joe_Object_getMem (argv[0])))
            *retval = joe_Boolean_New_true();
         else
            *retval = joe_Boolean_New_false();
         return JOE_SUCCESS;
      } else if (joe_Object_instanceOf (argv[0], &joe_BigDecimal_Class)) {
         return operation (self, argc, argv, retval, LE);
      } else {
        *retval = joe_Exception_New ("<=: invalid argument");
         return JOE_FAILURE;
      }
   } else {
      *retval = joe_Exception_New ("<=: invalid argument");
      return JOE_FAILURE;
   }
}

static int
lt (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1) {
      if (joe_Object_instanceOf(argv[0], &joe_Integer_Class)) {
         if (*((long *) joe_Object_getMem (self)) <
             *((long *) joe_Object_getMem (argv[0])))
            *retval = joe_Boolean_New_true();
         else
            *retval = joe_Boolean_New_false();
         return JOE_SUCCESS;
      } else if (joe_Object_instanceOf(argv[0], &joe_Float_Class)) {
         if (*((long *) joe_Object_getMem (self)) <
             *((double *) joe_Object_getMem (argv[0])))
            *retval = joe_Boolean_New_true();
         else
            *retval = joe_Boolean_New_false();
         return JOE_SUCCESS;
      } else if (joe_Object_instanceOf (argv[0], &joe_BigDecimal_Class)) {
         return operation (self, argc, argv, retval, LT);
      } else {
        *retval = joe_Exception_New ("<: invalid argument");
         return JOE_FAILURE;
      }
   } else {
      *retval = joe_Exception_New ("<: invalid argument");
      return JOE_FAILURE;
   }
}

static int
toString (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   char buff[32];
   sprintf (buff, "%ld", *((long *) joe_Object_getMem(self)));
   *retval = joe_String_New (buff);
   return JOE_SUCCESS;
}

static joe_Method mthds[] = {
   {"add", add },
   {"subtract", subtract },
   {"multiply", multiply },
   {"divide", divide },
   {"remainder", _remainder },
   {"equals", equals },
   {"ne", ne },
   {"gt", gt },
   {"ge", ge },
   {"lt", lt },
   {"le", le },
   {"toString", toString },
  {(void *) 0, (void *) 0}
};

joe_Class joe_Integer_Class = {
   "joe_Integer",
   0,
   0,
   mthds,
   0,
   &joe_Object_Class,
   0
};

joe_Object
joe_Integer_New (long value)
{
   joe_Object self;
   self = joe_Object_New (&joe_Integer_Class, 0);
   *((long *) joe_Object_getMem(self)) = value;
   return self;
}

long
joe_Integer_value (joe_Object self)
{
   return  *((long *) joe_Object_getMem(self));
}

joe_Integer
joe_Integer_addMe1 (joe_Object self)
{
   (*((long *) joe_Object_getMem(self)))++;
   return self;
}

joe_Integer
joe_Integer_subMe1 (joe_Object self)
{
   (*((long *) joe_Object_getMem(self)))--;
   return self;
}
