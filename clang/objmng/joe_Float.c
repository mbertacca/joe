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
# include "joe_BigDecimal.h"
# include "joe_Boolean.h"
# include "joe_String.h"
# include "joe_Exception.h"
# include "joestrct.h"

# include <stdio.h>

/**
# Class joe_Float
### extends joe_Object

This class implements a immutable, double-precision floating-point number.

Every literal number that contains a dot (.) and 
is **not** followed by the letter `m` is an instance of this class.
The E notation is supported as well. 

e.g.: `-123.456`,`-1.23456e02`

All the methods accept any kind of number as argument,
i.e. Float, Integers, or BigDecimal, and returns a different type of
object depending on the argument:
an operation with a Float or Integer argument returns a Float,
an operation with a BigDecimal argument returns a BigDecimal.
*/


static int
operation (joe_Integer self, int argc, joe_Object *argv,
           joe_Object *retval, enum joe_BigDecimal_ops op)
{
   int Return;
   joe_BigDecimal bd = 0;
   joe_Object_assign (&bd, joe_BigDecimal_New_dbl (JOE_FLOAT (self)));
   Return = joe_BigDecimal_oper (bd, argc, argv, retval, op);
   joe_Object_assign (&bd, 0);
   return Return;
}

/**
## add _aNumber_
## + _aNumber_
Returns a new number whose value is the sum of this number + _aNumber_.
*/

static int
add (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1) {
      if (JOE_ISCLASS (argv[0], &joe_Float_Class)) {
         joe_Object_assign(retval, joe_Float_New (JOE_FLOAT (self) +
                                                  JOE_FLOAT (argv[0])));
         return JOE_SUCCESS;
      } else if (JOE_ISCLASS (argv[0], &joe_Integer_Class)) {
         joe_Object_assign(retval, joe_Float_New (JOE_FLOAT (self) +
                                                  JOE_INTEGER (argv[0])));
         return JOE_SUCCESS;
      } else if (JOE_ISCLASS (argv[0], &joe_BigDecimal_Class)) {
         return operation (self, argc, argv, retval, ADD);
      }
   }
   joe_Object_assign(retval, joe_Exception_New ("joe_Float add: invalid argument"));
   return JOE_FAILURE;
}

/**
## subtract _aNumber_
## - _aNumber_
Returns a new number whose value is the difference of
this number - _aNumber_.
*/

static int
subtract (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1) {
      if (JOE_ISCLASS (argv[0], &joe_Float_Class)) {
         joe_Object_assign(retval, joe_Float_New (JOE_FLOAT (self) -
                                                  JOE_FLOAT (argv[0])));
         return JOE_SUCCESS;
      } else if (JOE_ISCLASS (argv[0], &joe_Integer_Class)) {
         joe_Object_assign(retval, joe_Float_New (JOE_FLOAT (self) -
                                                  JOE_INTEGER (argv[0])));
         return JOE_SUCCESS;
      } else if (JOE_ISCLASS (argv[0], &joe_BigDecimal_Class)) {
         return operation (self, argc, argv, retval, SUBTRACT);
      }
   }
   joe_Object_assign(retval, joe_Exception_New ("joe_Float subtract: invalid argument"));
   return JOE_FAILURE;
}

/**
## multiply _aNumber_
## * _aNumber_
Returns a new number whose value is the product of
this number * _aNumber_.
*/

static int
multiply (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1) {
      if (JOE_ISCLASS (argv[0], &joe_Float_Class)) {
         joe_Object_assign(retval, joe_Float_New (JOE_FLOAT (self) *
                                                  JOE_FLOAT (argv[0])));
         return JOE_SUCCESS;
      } else if (JOE_ISCLASS (argv[0], &joe_Integer_Class)) {
         joe_Object_assign(retval, joe_Float_New (JOE_FLOAT (self) *
                                                  JOE_INTEGER (argv[0])));
         return JOE_SUCCESS;
      } else if (JOE_ISCLASS (argv[0], &joe_BigDecimal_Class)) {
         return operation (self, argc, argv, retval, MULTIPLY);
      }
   }
   joe_Object_assign(retval, joe_Exception_New ("joe_Float multiply: invalid argument"));
   return JOE_FAILURE;
}

/**
## divide _aNumber_
## / _aNumber_
Returns a new number whose value is the quotient of
this number / _aNumber_.
*/

static int
divide (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1) {
      if (JOE_ISCLASS (argv[0], &joe_Float_Class)) {
         double divisor = JOE_FLOAT(argv[0]);
         joe_Object_assign(retval, joe_Float_New (JOE_FLOAT (self) / divisor));
         return JOE_SUCCESS;
      } else if (JOE_ISCLASS (argv[0], &joe_Integer_Class)) {
         int64_t divisor =  JOE_INTEGER(argv[0]);
         if (divisor == 0) {
            joe_Object_assign(retval,
               joe_Exception_New("Float divide: division by 0"));
            return JOE_FAILURE;
         } else {
            joe_Object_assign(retval, joe_Float_New (JOE_FLOAT (self) / divisor));
            return JOE_SUCCESS;
         }
      } else if (JOE_ISCLASS (argv[0], &joe_BigDecimal_Class)) {
         return operation (self, argc, argv, retval, DIVIDE);
      }
   }
   joe_Object_assign(retval, joe_Exception_New ("joe_Float divide: invalid argument"));
   return JOE_FAILURE;
}

/**
## remainder _aNumber_
## % _aNumber_
In the current version just raises an exception.
*/

static int
_remainder (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   joe_Object_assign(retval, joe_Exception_New ("joe_Float remainder: operation not implemented"));
   return JOE_FAILURE;
}

/**
## equals _aNumber_
## = _aNumber_

Compares this number with _aNumber_ and
returns <1> if they are equal, <0> otherwise.
*/

static int
equals (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1) {
      if (JOE_ISCLASS(argv[0], &joe_Float_Class)) {
         if (JOE_FLOAT(self) == JOE_FLOAT(argv[0]))
            joe_Object_assign(retval, joe_Boolean_New_true());
         else
            joe_Object_assign(retval, joe_Boolean_New_false());
      } else if (JOE_ISCLASS(argv[0], &joe_Integer_Class)) {
         if (JOE_FLOAT(self) == JOE_INTEGER(argv[0]))
            joe_Object_assign(retval, joe_Boolean_New_true());
         else
            joe_Object_assign(retval, joe_Boolean_New_false());
      } else if (JOE_ISCLASS (argv[0], &joe_BigDecimal_Class)) {
         return operation (self, argc, argv, retval, EQUALS);
      } else
         joe_Object_assign(retval, joe_Boolean_New_false());
     return JOE_SUCCESS;
   }
   joe_Object_assign(retval, joe_Exception_New ("joe_Float =: invalid argument"));
   return JOE_FAILURE;
}

/**
## ne _aNumber_
## <> _aNumber_

Compares this number with _aNumber_ and
returns <0> if they are equal, <1> otherwise.
*/

static int
ne (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1) {
      if (JOE_ISCLASS(argv[0], &joe_Float_Class)) {
         if (JOE_FLOAT(self) != JOE_FLOAT(argv[0]))
            joe_Object_assign(retval, joe_Boolean_New_true());
         else
            joe_Object_assign(retval, joe_Boolean_New_false());      
         return JOE_SUCCESS;
      } else if (JOE_ISCLASS(argv[0], &joe_Integer_Class)) {
         if (JOE_FLOAT(self) != JOE_INTEGER(argv[0]))
            joe_Object_assign(retval, joe_Boolean_New_true());
         else
            joe_Object_assign(retval, joe_Boolean_New_false());
         return JOE_SUCCESS;
      } else if (JOE_ISCLASS (argv[0], &joe_BigDecimal_Class)) {
         return operation (self, argc, argv, retval, NE);
      } else {
         joe_Object_assign(retval, joe_Exception_New("joe_Float <>: invalid argument"));
         return JOE_FAILURE;
      }
   } else {
      joe_Object_assign(retval, joe_Exception_New("joe_Float <>: invalid argument number"));
      return JOE_FAILURE;
   }
}

/**
## ge _aNumber_
## >= _aNumber_

Compares this number with _aNumber_ and
returns <1> if this number is greater or equal to _aNumber_, <0> otherwise.
*/

static int
ge (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1) {
      if (JOE_ISCLASS(argv[0], &joe_Float_Class)) {
         if (JOE_FLOAT(self) >= JOE_FLOAT(argv[0]))
            joe_Object_assign(retval, joe_Boolean_New_true());
         else
            joe_Object_assign(retval, joe_Boolean_New_false());
         return JOE_SUCCESS;
      } else if (JOE_ISCLASS(argv[0], &joe_Integer_Class)) {
         if (JOE_FLOAT(self) >= JOE_INTEGER(argv[0]))
            joe_Object_assign(retval, joe_Boolean_New_true());
         else
            joe_Object_assign(retval, joe_Boolean_New_false());
         return JOE_SUCCESS;
      } else if (JOE_ISCLASS (argv[0], &joe_BigDecimal_Class)) {
         return operation (self, argc, argv, retval, GE);
      } else {
         joe_Object_assign(retval, joe_Exception_New("joe_Float >=: invalid argument"));
         return JOE_FAILURE;
      }
   } else {
      joe_Object_assign(retval, joe_Exception_New("joe_Float >=: invalid argument number"));
      return JOE_FAILURE;
   }
}

/**
## gt _aNumber_
## > _aNumber_

Compares this number with _aNumber_ and
returns <1> if this number is greater than _aNumber_, <0> otherwise.
*/

static int
gt (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1) {
      if (JOE_ISCLASS(argv[0], &joe_Float_Class)) {
         if (JOE_FLOAT(self) > JOE_FLOAT(argv[0]))
            joe_Object_assign(retval, joe_Boolean_New_true());
         else
            joe_Object_assign(retval, joe_Boolean_New_false());
         return JOE_SUCCESS;
      } else if (JOE_ISCLASS(argv[0], &joe_Integer_Class)) {
         if (JOE_FLOAT(self) > JOE_INTEGER(argv[0]))
            joe_Object_assign(retval, joe_Boolean_New_true());
         else
            joe_Object_assign(retval, joe_Boolean_New_false());
         return JOE_SUCCESS;
      } else if (JOE_ISCLASS (argv[0], &joe_BigDecimal_Class)) {
         return operation (self, argc, argv, retval, GT);
      } else {
         joe_Object_assign(retval, joe_Exception_New("joe_Float >: invalid argument"));
         return JOE_FAILURE;
      }
   } else {
      joe_Object_assign(retval, joe_Exception_New("joe_Float >: invalid argument number"));
      return JOE_FAILURE;
   }
}

/**
## le _aNumber_
## <= _aNumber_

Compares this number with _aNumber_ and
returns <1> if this number is less or equal to _aNumber_, <0> otherwise.
*/

static int
le (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1) {
      if (JOE_ISCLASS(argv[0], &joe_Float_Class)) {
         if (JOE_FLOAT(self) <= JOE_FLOAT(argv[0]))
            joe_Object_assign(retval, joe_Boolean_New_true());
         else
            joe_Object_assign(retval, joe_Boolean_New_false());
         return JOE_SUCCESS;
      } else if (JOE_ISCLASS(argv[0], &joe_Integer_Class)) {
         if (JOE_FLOAT(self) <= JOE_INTEGER(argv[0]))
            joe_Object_assign(retval, joe_Boolean_New_true());
         else
            joe_Object_assign(retval, joe_Boolean_New_false());
         return JOE_SUCCESS;
      } else if (JOE_ISCLASS (argv[0], &joe_BigDecimal_Class)) {
         return operation (self, argc, argv, retval, LE);
      } else {
         joe_Object_assign(retval, joe_Exception_New("<=: invalid argument"));
         return JOE_FAILURE;
      }
   } else {
      joe_Object_assign(retval, joe_Exception_New("joe_Float <=: invalid argument number"));
      return JOE_FAILURE;
   }
}

/**
## lt _aNumber_
## < _aNumber_

Compares this number with _aNumber_ and
returns <1> if this number is less than _aNumber_, <0> otherwise.
*/

static int
lt (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1) {
      if (JOE_ISCLASS(argv[0], &joe_Float_Class)) {
         if (JOE_FLOAT(self) < JOE_FLOAT(argv[0]))
            joe_Object_assign(retval, joe_Boolean_New_true());
         else
            joe_Object_assign(retval, joe_Boolean_New_false());
         return JOE_SUCCESS;
      } else if (JOE_ISCLASS(argv[0], &joe_Integer_Class)) {
         if (JOE_FLOAT(self) < JOE_INTEGER(argv[0]))
            joe_Object_assign(retval, joe_Boolean_New_true());
         else
            joe_Object_assign(retval, joe_Boolean_New_false());
         return JOE_SUCCESS;
      } else if (JOE_ISCLASS (argv[0], &joe_BigDecimal_Class)) {
         return operation (self, argc, argv, retval, LT);
      } else {
         joe_Object_assign(retval, joe_Exception_New("joe_Float <: invalid argument"));
         return JOE_FAILURE;
      }
   } else {
      joe_Object_assign(retval, joe_Exception_New("joe_Float <: invalid argument number"));
      return JOE_FAILURE;
   }
}

/**
## intValue

Returns an Integer containing this number.
If this number has a decimal part it is removed.
If the number exceeds the Integer precision the result is undefined.
*/

static int
intValue (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 0) {
      joe_Object_assign(retval, joe_Integer_New((int64_t) JOE_FLOAT(self)));
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval,
                        joe_Exception_New("intValue: invalid argument(s)"));
      return JOE_FAILURE;
   }
}

/**
## floatValue

Returns this number.
*/

static int
floatValue (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 0) {
      joe_Object_assign(retval, self);
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval,
                        joe_Exception_New("floatValue: invalid argument(s)"));
      return JOE_FAILURE;
   }
}

/**
## bigDecimalValue

Returns a BigDecimal containing this number.
*/

static int
bigDecimalValue (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 0) {
      joe_Object_assign (retval, joe_BigDecimal_New_dbl (JOE_FLOAT(self)));
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval,
                   joe_Exception_New("bigDecimalValue: invalid argument(s)"));
      return JOE_FAILURE;
   }
}

/**
## signum

Returns an Integer = 1 if  this number is positive,
 -1 if it is negative an 0 if it is 0.
*/

static int
signum (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 0) {
      joe_Object_assign(retval, joe_Integer_New(joe_Float_signum (self)));
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval,
                   joe_Exception_New("signum: invalid argument(s)"));
      return JOE_FAILURE;
   }
}

/**
## toString

Returns a string representation of this number.
*/

static int
toString (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 0) {
      char buff[512];
      snprintf (buff, sizeof(buff), "%.15g", JOE_FLOAT(self));
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
   {"intValue", intValue },
   {"longValue", intValue },
   {"floatValue", floatValue },
   {"doubleValue", floatValue },
   {"bigDecimalValue", bigDecimalValue },
   {"signum", signum },
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
   1
};

joe_Object
joe_Float_New (double value)
{
   joe_Object self;
   self = joe_Object_New (&joe_Float_Class, 0);
   JOE_FLOAT(self) = value;
   return self;
}

double
joe_Float_value (joe_Float self)
{
   return JOE_FLOAT(self);
}

int
joe_Float_signum (joe_Float self)
{
   double x = JOE_FLOAT (self);
   return (x > 0 ? 1 : (x < 0 ? -1 : 0));
}
