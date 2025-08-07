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
# include <string.h>

/**
# Class joe_Integer
### extends joe_Object

This class implements a immutable, 64 bits long integer number.

Every literal integer, possibly followed by the letter `l`,
is an instance of this class.

Nearly all the methods accept any kind of number as argument,
i.e. Integers, Float or BigDecimal, and returns a different type of
object depending on the argument:
an operation with a Integer argument returns a Integer,
an operation with a Float oargument returns a Float,
an operation with a BigDecimal argument returns a BigDecimal.
*/

void
joe_Integer_toAscii (int64_t n, char *out) {
   char asc[32];
   char *p = asc + sizeof (asc) - 1;

   *(p--) = 0;
   if (n == 0) {
     *out++ = '0';
     *out = 0;
   } else if (n == 0x8000000000000000) {
     strcpy (out,"-9223372036854775808");
   } else {
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
      while (*p) 
         *(out++) = *(p++);
      *out = 0;
   }
}

static char xdigits[] = "0123456789abcdef";

static void
joe_Integer_toPow2Ascii (int64_t sn, int mask, int shift,char *out) {
   char asc[sizeof(int64_t) * 8 + 1];
   char *p = asc + sizeof (asc) - 1;
   uint64_t n = sn;

   *(p--) = 0;
   if (n == 0) {
     *out++ = '0';
     *out = 0;
   } else {
      for (*(p--) = 0; n != 0; p--) {
         *p = xdigits[n & mask];
         n >>= shift;
      }
      p++;
      while (*p) 
         *(out++) = *(p++);
      *out = 0;
   }
}

int64_t
joe_Integer_fromAscii (char *asc) {
   int64_t Return = 0;
   int negative = *asc == '-';

   if (negative)
      asc++;

   if (*asc == 'x') {
      for (++asc; *asc; asc++) {
         Return <<= 4;
         switch (*asc) {
         case 'A':
         case 'B':
         case 'C':
         case 'D':
         case 'E':
         case 'F':
            Return |= (*asc - 'A' + 10);
            break;
         case 'a':
         case 'b':
         case 'c':
         case 'd':
         case 'e':
         case 'f':
            Return |= (*asc - 'a' + 10);
            break;
         default:
            Return |= (*asc - '0');
            break;
         }
      }
   } else {
      for (;*asc; asc++) {
         Return *= 10; 
         Return += *asc - '0';
      }
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

/**
## add _aNumber_
## + _aNumber_
Returns a new number whose value is the sum of this number + _aNumber_.
*/

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
         joe_Object_assign(retval,
                   joe_Float_New (JOE_INTEGER (self) / divisor));
         return JOE_SUCCESS;
      } else if (JOE_ISCLASS (argv[0], &joe_BigDecimal_Class)) {
         return operation (self, argc, argv, retval, DIVIDE);
      }
   }
   joe_Object_assign(retval, joe_Exception_New ("Integer divide: invalid argument"));
   return JOE_FAILURE;
}

/**
## remainder _aNumber_
## % _aNumber_
Returns a new Integer whose value is the remainder of the division of
this number / _aNumber_.
If _aNumber_ is a Float an exception is thrown
*/

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

/**
## and _aInteger_

Returns the results of a bitwise AND between this number and _aInteger_ 
*/

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

/**
## or _aInteger_

Returns the results of a bitwise OR between this number and _aInteger_ 
*/

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

/**
## xor _aInteger_

Returns the results of a bitwise XOR between this number and _aInteger_ 
*/

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

/**
## shiftl _aInteger_

Return a number equal to this number left shifted by _aInteger_ bits
*/

static int
shiftl (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1 && JOE_ISCLASS(argv[0], &joe_Integer_Class)) {
       joe_Object_assign(retval, 
                joe_Integer_New(JOE_INTEGER (self) << JOE_INTEGER (argv[0])));
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval, joe_Exception_New ("shiftl: invalid argument"));
      return JOE_FAILURE;
   }
}

/**
## shiftr _aInteger_

Return a number equal to this number right shifted by _aInteger_ bits
*/

static int
shiftr (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1 && JOE_ISCLASS(argv[0], &joe_Integer_Class)) {
       joe_Object_assign(retval, 
                joe_Integer_New((uint64_t)JOE_INTEGER (self) >>
                                JOE_INTEGER (argv[0])));
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval, joe_Exception_New ("shiftr: invalid argument"));
      return JOE_FAILURE;
   }
}

/**
## shifta _aInteger_

Returns a number equal to this number arithmetically shifted to the right
by _aInteger_ bits
*/

static int
shifta (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1 && JOE_ISCLASS(argv[0], &joe_Integer_Class)) {
       joe_Object_assign(retval, 
                joe_Integer_New(JOE_INTEGER (self) >> JOE_INTEGER (argv[0])));
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval, joe_Exception_New ("shifta: invalid argument"));
      return JOE_FAILURE;
   }
}

/**
## not

Returns the results of a bitwise NOT on this number 
*/

static int
not (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 0) {
       joe_Object_assign(retval, joe_Integer_New(~JOE_INTEGER (self)));
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval, joe_Exception_New ("not: invalid argument"));
      return JOE_FAILURE;
   }
}

/**
## intValue

Returns this number.
*/

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

/**
## floatValue

Returns a Float containing this number.
*/

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

/**
## bigDecimalValue

Returns a BigDecimal containing this number.
*/

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

/**
## toHexString

Returns a String containing the hexadecimal representation of this number.
*/

static int
toHexString (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 0) {
      char buff[sizeof(int64_t) * 2 + 1];
      joe_Integer_toPow2Ascii (JOE_INTEGER (self), 0x0F, 4, buff);
      joe_Object_assign(retval, joe_String_New (buff));
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval,
                   joe_Exception_New("toHexString: invalid argument(s)"));
      return JOE_FAILURE;
   }
}

/**
## toBinaryString

Returns a String containing the binary representation of this number.
*/

static int
toBinaryString (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 0) {
      char buff[sizeof(int64_t) * 8 + 1];
      joe_Integer_toPow2Ascii (JOE_INTEGER (self), 1, 1, buff);
      joe_Object_assign(retval, joe_String_New (buff));
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval,
                   joe_Exception_New("toBinaryString: invalid argument(s)"));
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
      joe_Object_assign (retval, joe_Integer_New (joe_Integer_signum(self)));
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval,
                   joe_Exception_New("signum: invalid argument(s)"));
      return JOE_FAILURE;
   }
}

/**
## toChar

Returns a String one character long containing the character whose code
is this number.
*/

static int
toChar (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 0) {
      char buff = (char) JOE_INTEGER (self);
      joe_Object_assign(retval, joe_String_New_len (&buff,1));
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval,
                   joe_Exception_New("toChar: invalid argument(s)"));
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
   {"shiftl", shiftl },
   {"shiftr", shiftr },
   {"shifta", shifta },
   {"intValue", intValue },
   {"longValue", intValue },
   {"floatValue", floatValue },
   {"doubleValue", floatValue },
   {"bigDecimalValue", bigDecimalValue },
   {"toHexString", toHexString },
   {"toBinaryString", toBinaryString },
   {"signum", signum },
   {"toChar", toChar },
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

int
joe_Integer_signum (joe_Integer self)
{
   int64_t x = JOE_INTEGER (self);
   return (x > 0 ? 1 : (x < 0 ? -1 : 0));
}

joe_Integer
joe_Integer_add(joe_Object self, int64_t incr)
{
   return joe_Integer_New(JOE_INTEGER(self) + incr);
}
