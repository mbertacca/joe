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
# include "joestrct.h"

# include <stdio.h>

void
joe_Integer_toAscii (int64_t n, char *out) {
   char asc[32];
   char *p = asc + sizeof (asc) - 1;

   *(p--) = 0;
   if (n == 0)
     *(p) = '0';
   else {
      int negative = (n < 0);
      if (negative)
         n = ~n + 1;
      for (*(p--) = 0; n != 0; p--) {
         *p = '0' + (n % 10);
         n /= 10;
      }
      if (negative)
         *(p) = '-';
      else
         p++;
   }
   while (*p) 
      *(out++) = *(p++);
   *out = 0;
}

int64_t
joe_Integer_fromAscii (char *asc) {
   int64_t Return = 0;
   int negative = *asc == '-';

   if (negative)
      asc++;

   for (;*asc; asc++) {
      Return *= 10; 
      Return += *asc - '0';
   }
   if (negative)
      Return = ~Return + 1;
   return Return;
}

static int
operation (joe_Integer self, int argc, joe_Object *argv,
           joe_Object *retval, enum joe_BigDecimal_ops op)
{
   int Return;
   joe_BigDecimal bd = 0;
   char asc[32];
   
   joe_Integer_toAscii (JOE_INTEGER(self), asc);
   joe_Object_assign (&bd, joe_BigDecimal_New_str (asc));
   Return = joe_BigDecimal_oper (bd, argc, argv, retval, op);
   joe_Object_assign (&bd, 0);
   return Return;
}

static int
add (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1) {
      if (JOE_ISCLASS (argv[0], &joe_Integer_Class)) {
         joe_Object_assign (retval,
                 joe_Integer_New (JOE_INTEGER (self) + JOE_INTEGER (argv[0])));
         return JOE_SUCCESS;
      } else if (JOE_ISCLASS (argv[0], &joe_Float_Class)) {
         joe_Object_assign(retval,
                 joe_Float_New (JOE_INTEGER (self) + JOE_FLOAT (argv[0])));
         return JOE_SUCCESS;
      } else if (JOE_ISCLASS (argv[0], &joe_BigDecimal_Class)) {
         return operation (self, argc, argv, retval, ADD);
      }
   }
   joe_Object_assign(retval, joe_Exception_New ("Integer add: invalid argument"));
   return JOE_FAILURE;
}

static int
subtract (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1) {
      if (JOE_ISCLASS (argv[0], &joe_Integer_Class)) {
         joe_Object_assign(retval,
                   joe_Integer_New (JOE_INTEGER (self) - JOE_INTEGER (argv[0])));
         return JOE_SUCCESS;
      } else if (JOE_ISCLASS (argv[0], &joe_Float_Class)) {
         joe_Object_assign(retval,
                   joe_Float_New (JOE_INTEGER (self) - JOE_FLOAT (argv[0])));
         return JOE_SUCCESS;
      } else if (JOE_ISCLASS (argv[0], &joe_BigDecimal_Class)) {
         return operation (self, argc, argv, retval, SUBTRACT);
      }
   }
   joe_Object_assign(retval, joe_Exception_New ("Integer subtract: invalid argument"));
   return JOE_FAILURE;
}

static int
multiply (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1) {
      if (JOE_ISCLASS (argv[0], &joe_Integer_Class)) {
         joe_Object_assign(retval,
                   joe_Integer_New (JOE_INTEGER (self) * JOE_INTEGER (argv[0])));
         return JOE_SUCCESS;
      } else if (JOE_ISCLASS (argv[0], &joe_Float_Class)) {
         joe_Object_assign(retval,
                 joe_Float_New (JOE_INTEGER (self) * JOE_FLOAT (argv[0])));
         return JOE_SUCCESS;
      } else if (JOE_ISCLASS (argv[0], &joe_BigDecimal_Class)) {
         return operation (self, argc, argv, retval, MULTIPLY);
      }
   }
   joe_Object_assign(retval, joe_Exception_New ("Integer multiply: invalid argument"));
   return JOE_FAILURE;
}

static int
divide (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1) {
      if (JOE_ISCLASS (argv[0], &joe_Integer_Class)) {
         int64_t divisor = JOE_INTEGER (argv[0]);
         if (divisor == 0) {
            joe_Object_assign(retval,
                              joe_Exception_New("Integer divide: division by 0"));
            return JOE_FAILURE;
         } else {
            joe_Object_assign(retval,
                              joe_Integer_New (JOE_INTEGER (self) / divisor));
            return JOE_SUCCESS;
         }
      } else if (JOE_ISCLASS (argv[0], &joe_Float_Class)) {
         double divisor = JOE_FLOAT (argv[0]);
         if (divisor == 0) {
            joe_Object_assign(retval,
               joe_Exception_New("Integer divide: division by 0"));
            return JOE_FAILURE;
         } else {
            joe_Object_assign(retval,
                   joe_Float_New (JOE_INTEGER (self) / divisor));
            return JOE_SUCCESS;
         }
      } else if (JOE_ISCLASS (argv[0], &joe_BigDecimal_Class)) {
         return operation (self, argc, argv, retval, DIVIDE);
      }
   }
   joe_Object_assign(retval, joe_Exception_New ("Integer divide: invalid argument"));
   return JOE_FAILURE;
}

static int
_remainder (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1) {
      if (JOE_ISCLASS (argv[0], &joe_Integer_Class)) {
         int64_t divisor = JOE_INTEGER (argv[0]);
         if (divisor == 0) {
            joe_Object_assign(retval,
               joe_Exception_New("Integer remainder: division by 0"));
            return JOE_FAILURE;
         } else {
            joe_Object_assign(retval,
                   joe_Integer_New (JOE_INTEGER (self) % divisor));
            return JOE_SUCCESS;
         }
      } else if (JOE_ISCLASS (argv[0], &joe_Float_Class)) {
         joe_Object_assign(retval,
                           joe_Exception_New("Integer remainder: not allowed with Float"));
         return JOE_FAILURE;
      } else if (JOE_ISCLASS (argv[0], &joe_BigDecimal_Class)) {
         return operation (self, argc, argv, retval, REMAINDER);
      }
   }
   joe_Object_assign(retval, joe_Exception_New ("Integer remainder: invalid argument"));
   return JOE_FAILURE;
}

static int
equals (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1) {
      if (JOE_ISCLASS(argv[0], &joe_Integer_Class)) {
         if (JOE_INTEGER (self) == JOE_INTEGER (argv[0]))
            joe_Object_assign(retval, joe_Boolean_New_true());
         else
            joe_Object_assign(retval, joe_Boolean_New_false());
      } else if (JOE_ISCLASS(argv[0], &joe_Float_Class)) {
         if (JOE_INTEGER (self) == JOE_FLOAT (argv[0]))
            joe_Object_assign(retval, joe_Boolean_New_true());
         else
            joe_Object_assign(retval, joe_Boolean_New_false());
      } else if (JOE_ISCLASS (argv[0], &joe_BigDecimal_Class)) {
         return operation (self, argc, argv, retval, EQUALS);
      } else {
         joe_Object_assign(retval, joe_Boolean_New_false());
      }
      return JOE_SUCCESS;
   }
   joe_Object_assign(retval, joe_Exception_New ("Integer equals: invalid argument"));
   return JOE_FAILURE;
}

static int
ne (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1) {
      if (JOE_ISCLASS(argv[0], &joe_Integer_Class)) {
         if (JOE_INTEGER (self) != JOE_INTEGER (argv[0]))
            joe_Object_assign(retval, joe_Boolean_New_true());
         else
            joe_Object_assign(retval, joe_Boolean_New_false());
         return JOE_SUCCESS;
      } else if (JOE_ISCLASS(argv[0], &joe_Float_Class)) {
         if (JOE_INTEGER (self) != JOE_FLOAT (argv[0]))
            joe_Object_assign(retval, joe_Boolean_New_true());
         else
            joe_Object_assign(retval, joe_Boolean_New_false());
         return JOE_SUCCESS;
      } else if (JOE_ISCLASS (argv[0], &joe_BigDecimal_Class)) {
         return operation (self, argc, argv, retval, NE);
      } else {
         joe_Object_assign(retval, joe_Exception_New ("!=: invalid argument"));
         return JOE_FAILURE;
      }
   } else {
      joe_Object_assign(retval, joe_Exception_New ("!=: invalid argument"));
      return JOE_FAILURE;
   }
}

static int
ge (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1) {
      if (JOE_ISCLASS(argv[0], &joe_Integer_Class)) {
         if (JOE_INTEGER (self) >= JOE_INTEGER (argv[0]))
            joe_Object_assign(retval, joe_Boolean_New_true());
         else
            joe_Object_assign(retval,joe_Boolean_New_false());
         return JOE_SUCCESS;
      } else if (JOE_ISCLASS(argv[0], &joe_Float_Class)) {
         if (JOE_INTEGER (self) >= JOE_FLOAT (argv[0]))
            joe_Object_assign(retval, joe_Boolean_New_true());
         else
            joe_Object_assign(retval, joe_Boolean_New_false());
         return JOE_SUCCESS;
      } else if (JOE_ISCLASS (argv[0], &joe_BigDecimal_Class)) {
         return operation (self, argc, argv, retval, GE);
      } else {
         joe_Object_assign(retval, joe_Exception_New (">=: invalid argument"));
         return JOE_FAILURE;
      }
   } else {
      joe_Object_assign(retval, joe_Exception_New (">=: invalid argument"));
      return JOE_FAILURE;
   }
}

static int
gt (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1) {
      if (JOE_ISCLASS(argv[0], &joe_Integer_Class)) {
         if (JOE_INTEGER (self) > JOE_INTEGER (argv[0]))
            joe_Object_assign(retval, joe_Boolean_New_true());
         else
            joe_Object_assign(retval, joe_Boolean_New_false());
         return JOE_SUCCESS;
      } else if (JOE_ISCLASS(argv[0], &joe_Float_Class)) {
         if (JOE_INTEGER (self) > JOE_FLOAT (argv[0]))
            joe_Object_assign(retval, joe_Boolean_New_true());
         else
            joe_Object_assign(retval, joe_Boolean_New_false());
         return JOE_SUCCESS;
      } else if (JOE_ISCLASS (argv[0], &joe_BigDecimal_Class)) {
         return operation (self, argc, argv, retval, GT);
      } else {
         joe_Object_assign(retval, joe_Exception_New ("> :invalid argument"));
         return JOE_FAILURE;
      }
   } else {
      joe_Object_assign(retval, joe_Exception_New (">: invalid argument"));
      return JOE_FAILURE;
   }
}

static int
le (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1) {
      if (JOE_ISCLASS(argv[0], &joe_Integer_Class)) {
         if (JOE_INTEGER (self) <= JOE_INTEGER (argv[0]))
            joe_Object_assign(retval, joe_Boolean_New_true());
         else
            joe_Object_assign(retval, joe_Boolean_New_false());
         return JOE_SUCCESS;
      } else if (JOE_ISCLASS(argv[0], &joe_Float_Class)) {
         if (JOE_INTEGER (self) <= JOE_FLOAT(argv[0]))
            joe_Object_assign(retval, joe_Boolean_New_true());
         else
            joe_Object_assign(retval,joe_Boolean_New_false());
         return JOE_SUCCESS;
      } else if (JOE_ISCLASS (argv[0], &joe_BigDecimal_Class)) {
         return operation (self, argc, argv, retval, LE);
      } else {
         joe_Object_assign(retval, joe_Exception_New ("<=: invalid argument"));
         return JOE_FAILURE;
      }
   } else {
      joe_Object_assign(retval, joe_Exception_New ("<=: invalid argument"));
      return JOE_FAILURE;
   }
}

static int
lt (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1) {
      if (JOE_ISCLASS(argv[0], &joe_Integer_Class)) {
         if (JOE_INTEGER (self) < JOE_INTEGER (argv[0]))
            joe_Object_assign(retval, joe_Boolean_New_true());
         else
            joe_Object_assign(retval, joe_Boolean_New_false());
         return JOE_SUCCESS;
      } else if (JOE_ISCLASS(argv[0], &joe_Float_Class)) {
         if (JOE_INTEGER (self) < JOE_FLOAT (argv[0]))
            joe_Object_assign(retval, joe_Boolean_New_true());
         else
            joe_Object_assign(retval, joe_Boolean_New_false());
         return JOE_SUCCESS;
      } else if (JOE_ISCLASS (argv[0], &joe_BigDecimal_Class)) {
         return operation (self, argc, argv, retval, LT);
      } else {
         joe_Object_assign(retval, joe_Exception_New ("<: invalid argument"));
         return JOE_FAILURE;
      }
   } else {
      joe_Object_assign(retval, joe_Exception_New ("<: invalid argument"));
      return JOE_FAILURE;
   }
}

static int
and (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1 && JOE_ISCLASS(argv[0], &joe_Integer_Class)) {
       joe_Object_assign(retval, 
                joe_Integer_New(JOE_INTEGER (self) & JOE_INTEGER (argv[0])));
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval, joe_Exception_New ("and: invalid argument"));
      return JOE_FAILURE;
   }
}

static int
or (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1 && JOE_ISCLASS(argv[0], &joe_Integer_Class)) {
       joe_Object_assign(retval, 
                joe_Integer_New(JOE_INTEGER (self) | JOE_INTEGER (argv[0])));
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval, joe_Exception_New ("or: invalid argument"));
      return JOE_FAILURE;
   }
}

static int
xor (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1 && JOE_ISCLASS(argv[0], &joe_Integer_Class)) {
       joe_Object_assign(retval, 
                joe_Integer_New(JOE_INTEGER (self) ^ JOE_INTEGER (argv[0])));
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval, joe_Exception_New ("xor: invalid argument"));
      return JOE_FAILURE;
   }
}

static int
not (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 0) {
       joe_Object_assign(retval, joe_Integer_New(~JOE_INTEGER (self)));
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval, joe_Exception_New ("xor: invalid argument"));
      return JOE_FAILURE;
   }
}

static int
intValue (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 0) {
      joe_Object_assign(retval, self);
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval,
                   joe_Exception_New("intValue: invalid argument(s)"));
      return JOE_FAILURE;
   }
}

static int
floatValue (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 0) {
      joe_Object_assign(retval, joe_Float_New (JOE_INTEGER (self)));
            return JOE_SUCCESS;
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval,
                   joe_Exception_New("floatValue: invalid argument(s)"));
      return JOE_FAILURE;
   }
}

static int
bigDecimalValue (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 0) {
      char asc[32];

      joe_Integer_toAscii (JOE_INTEGER (self), asc);
      joe_Object_assign (retval, joe_BigDecimal_New_str (asc));
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval,
                   joe_Exception_New("bigDecimalValue: invalid argument(s)"));
      return JOE_FAILURE;
   }
}

static int
toString (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 0) {
      char buff[32];
      joe_Integer_toAscii (JOE_INTEGER (self), buff);
      joe_Object_assign(retval, joe_String_New (buff));
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval,
                   joe_Exception_New("toString: invalid argument(s)"));
      return JOE_FAILURE;
   }
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
   {"and", and },
   {"or", or },
   {"xor", xor },
   {"not", not },
   {"intValue", intValue },
   {"longValue", intValue },
   {"floatValue", floatValue },
   {"doubleValue", floatValue },
   {"bigDecimalValue", bigDecimalValue },
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
   1
};

joe_Object
joe_Integer_New (int64_t value)
{
   joe_Object self;
   self = joe_Object_New (&joe_Integer_Class, 0);
   JOE_INTEGER (self) = value;
   return self;
}

int64_t
joe_Integer_value (joe_Object self)
{
   return JOE_INTEGER (self);
}

joe_Integer
joe_Integer_add(joe_Object self, int64_t incr)
{
   return joe_Integer_New(JOE_INTEGER(self) + incr);
}
