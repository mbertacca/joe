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

# include "joe_Memory.h"
# include "joe_BigDecimal.h"
# include "joe_Integer.h"
# include "joe_Float.h"
# include "joe_Boolean.h"
# include "joe_String.h"
# include "joe_Exception.h"

# include <stdio.h>
# include <stdlib.h>
# include <string.h>

static char *variables[] = { "data", 0 };

static void
init (joe_Object self, nDecimal bd)
{
   int memsize = nDecimal_memsize (bd);
   joe_Memory mem = joe_Memory_New (memsize);
   memcpy (joe_Object_getMem(mem), bd, memsize);
   joe_Object_assign (joe_Object_at (self, 0), mem);
}

static int
init_str (joe_Object self, char *num, joe_Object *retval)
{
   int Return = JOE_SUCCESS;
   nDecimal bd = nDecimal_new_str (num);

   if (nDecimal_wrongAssignment (bd)) {
      joe_Object_assign(retval, joe_Exception_New ("BigDecimal: not numeric format"));
      Return = JOE_FAILURE;
   } else {
      init (self, bd);
      Return = JOE_SUCCESS;
   }
   nDecimal_delete (bd);
   return Return;
}

static int
ctor (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   switch (argc) {
   case 1:
      if (!joe_Object_instanceOf (argv[0], &joe_String_Class)) {
         joe_Object_assign(retval, joe_Exception_New ("BigDecimal: invalid 1st argument"));
         return JOE_FAILURE;
      }
      break;
   default:
      joe_Object_assign (retval, joe_Exception_New ("BigDecimal: invalid argument number"));
      return JOE_FAILURE;
   }

   return init_str (self, joe_String_getCharStar(argv[0]), retval);
}

static int
add(joe_Object self, int argc, joe_Object* argv, joe_Object* retval)
{
   return joe_BigDecimal_oper(self, argc, argv, retval, ADD);
}

static int
subtract(joe_Object self, int argc, joe_Object* argv, joe_Object* retval)
{
   return joe_BigDecimal_oper(self, argc, argv, retval, SUBTRACT);
}

static int
multiply(joe_Object self, int argc, joe_Object* argv, joe_Object* retval)
{
   return joe_BigDecimal_oper(self, argc, argv, retval, MULTIPLY);
}

static int
divide (joe_Object self, int argc, joe_Object* argv, joe_Object* retval)
{
   return joe_BigDecimal_oper(self, argc, argv, retval, DIVIDE);
}

static int
_remainder (joe_Object self, int argc, joe_Object* argv, joe_Object* retval)
{
   return joe_BigDecimal_oper(self, argc, argv, retval, REMAINDER);
}

static int
equals (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   return joe_BigDecimal_oper(self, argc, argv, retval, EQUALS);
}

static int
ne (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   return joe_BigDecimal_oper(self, argc, argv, retval, NE);
}

static int
ge (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   return joe_BigDecimal_oper(self, argc, argv, retval, GE);
}

static int
le (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   return joe_BigDecimal_oper(self, argc, argv, retval, LE);
}

static int
gt (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   return joe_BigDecimal_oper(self, argc, argv, retval, GT);
}

static int
lt (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   return joe_BigDecimal_oper(self, argc, argv, retval, LT);
}

static int
intValue (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 0) {
      joe_Memory mem = *joe_Object_at (self, 0);
      nDecimal num = (nDecimal) joe_Object_getMem (mem);
      char* strNum = nDecimal_toString(num);
      joe_Object_assign (retval, joe_Integer_New(joe_Integer_fromAscii(strNum)));
      free(strNum);
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
      joe_Memory mem = *joe_Object_at (self, 0);
      nDecimal num = (nDecimal) joe_Object_getMem (mem);
      char* strNum = nDecimal_toString(num);
      joe_Object_assign (retval, joe_Float_New(atof(strNum)));
      free(strNum);
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
      joe_Object_assign (retval, self);
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
      joe_Memory mem = *joe_Object_at (self, 0);
      nDecimal num = (nDecimal) joe_Object_getMem (mem);
      char* strNum = nDecimal_toString(num);
      joe_Object_assign (retval, joe_String_New (nDecimal_toString (num)));
      free(strNum);
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
   {"toString", toString },
  {(void *) 0, (void *) 0}
};

joe_Class joe_BigDecimal_Class = {
   "joe_BigDecimal",
   ctor,
   0,
   mthds,
   variables,
   &joe_Object_Class,
   0
};

joe_Object
joe_BigDecimal_New (nDecimal bd)
{
   joe_Object newObj = joe_Object_New (&joe_BigDecimal_Class, 0);
   init (newObj, bd);
   return newObj;
}

joe_Object
joe_BigDecimal_New_str(char* num)
{
    return joe_BigDecimal_New(nDecimal_new_str(num));
}

joe_Object
joe_BigDecimal_New_dbl(double num)
{
    return joe_BigDecimal_New(nDecimal_new_dbl(num));
}

nDecimal
get_nDecimal (joe_BigDecimal self)
{
   joe_Memory mem = *joe_Object_at (self, 0);
   return (nDecimal) joe_Object_getMem (mem); 
}

int
joe_BigDecimal_bigDecimalValue (joe_Object self, joe_Object *retval)
{
   if (joe_Object_instanceOf(self, &joe_BigDecimal_Class)) {
      joe_Object_assign(retval, self);
   } else {
      nDecimal bd;
      if (joe_Object_instanceOf(self, &joe_Integer_Class)) {
         bd = nDecimal_new_lng(*((int64_t*)joe_Object_getMem(self)));
      } else if (joe_Object_instanceOf(self, &joe_Float_Class)) {
         bd = nDecimal_new_dbl(*((double*)joe_Object_getMem(self)));
      } else if (joe_Object_instanceOf(self, &joe_String_Class)) {
         bd = nDecimal_new_str (joe_String_getCharStar(self));
         if (nDecimal_wrongAssignment (bd)) {
            joe_Object_assign(retval,
                              joe_Exception_New ("BigDecimal: not numeric format"));
            nDecimal_delete (bd);
            return JOE_FAILURE;
         }
      } else {
         joe_Object_assign(retval,
                           joe_Exception_New("BigDecimal: not numeric value"));
         return JOE_FAILURE;
      }
      joe_Object_assign(retval, joe_BigDecimal_New (bd));
      nDecimal_delete (bd);
   }
   return JOE_SUCCESS;
}
int joe_BigDecimal_oper(joe_Object self, int argc, joe_Object* argv,
                        joe_Object* retval, enum joe_BigDecimal_ops op)
{
   if (argc == 1) {
      nDecimal sbd = get_nDecimal (self);
      if (joe_BigDecimal_bigDecimalValue (argv[0], retval) == JOE_SUCCESS) {
         nDecimal res;
         nDecimal abd = get_nDecimal (*retval);
         switch (op) {
         case ADD:
            res = nDecimal_add(sbd, abd);
            joe_Object_assign(retval, joe_BigDecimal_New(res));
            nDecimal_delete(res);
            break;
         case SUBTRACT:
            res = nDecimal_subtract(sbd, abd);
            joe_Object_assign(retval, joe_BigDecimal_New(res));
            nDecimal_delete(res);
            break;
         case MULTIPLY:
            res = nDecimal_multiply(sbd, abd);
            joe_Object_assign(retval, joe_BigDecimal_New(res));
            nDecimal_delete(res);
            break;
         case DIVIDE:
            if (nDecimal_isZero(abd)) {
               joe_Object_assign(retval,
                  joe_Exception_New("joe_BigDecimal /: division by 0"));
               return JOE_FAILURE;
            } else {
               res = nDecimal_divide(sbd, abd, 16);
               joe_Object_assign(retval, joe_BigDecimal_New(res));
               nDecimal_delete(res);
            }
            break;
         case REMAINDER:
            if (nDecimal_isZero(abd)) {
               joe_Object_assign(retval,
                  joe_Exception_New("joe_BigDecimal %: division by 0"));
               return JOE_FAILURE;
            } else {
               res = nDecimal_remainder(sbd, abd);
               joe_Object_assign(retval, joe_BigDecimal_New(res));
               nDecimal_delete(res);
            }
            break;
         case EQUALS:
            if (nDecimal_compareTo (sbd, abd) == 0)
               joe_Object_assign(retval, joe_Boolean_New_true());
            else
               joe_Object_assign(retval, joe_Boolean_New_false());
            break;
         case LT:
            if (nDecimal_compareTo(sbd, abd) < 0)
               joe_Object_assign(retval, joe_Boolean_New_true());
            else
                  joe_Object_assign(retval, joe_Boolean_New_false());
            break;
         case GT:
            if (nDecimal_compareTo(sbd, abd) > 0)
               joe_Object_assign(retval, joe_Boolean_New_true());
            else
               joe_Object_assign(retval, joe_Boolean_New_false());
            break;
         case LE:
            if (nDecimal_compareTo(sbd, abd) <= 0)
               joe_Object_assign(retval, joe_Boolean_New_true());
            else
               joe_Object_assign(retval, joe_Boolean_New_false());
            break;
         case GE:
            if (nDecimal_compareTo(sbd, abd) >= 0)
               joe_Object_assign(retval, joe_Boolean_New_true());
            else
               joe_Object_assign(retval, joe_Boolean_New_false());
            break;
         case NE:
            if (nDecimal_compareTo(sbd, abd) != 0)
               joe_Object_assign(retval, joe_Boolean_New_true());
            else
               joe_Object_assign(retval, joe_Boolean_New_false());
            break;
         default:
            joe_Object_assign(retval,
               joe_Exception_New("Internal error on joe_BigDecimal_oper"));
            return JOE_FAILURE;
         }
         return JOE_SUCCESS;
      }
   } else {
      joe_Object_assign(retval,
               joe_Exception_New("BigDecimal: invalid argument number"));
   }
   return JOE_FAILURE;
}
