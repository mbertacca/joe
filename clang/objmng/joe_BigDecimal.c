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
      *retval = joe_Exception_New ("BigDecimal: not numeric format");
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
         *retval = joe_Exception_New ("BigDecimal: invalid 1st argument");
         return JOE_FAILURE;
      }
      break;
   default:
      *retval = joe_Exception_New ("BigDecimal: invalid argument number");
      return JOE_FAILURE;
   }

   return init_str (self, joe_String_getCharStar(argv[0]), retval);
}

static int
add(joe_Object self, int argc, joe_Object* argv, joe_Object* retval)
{

   if (argc == 1) {
      nDecimal sbd =get_nDecimal (self);
      if (joe_BigDecimal_bigDecimalValue (argv[0], retval) == JOE_SUCCESS) {
         nDecimal res;
         joe_Object tmp = 0;
         nDecimal abd = get_nDecimal (*retval);
         joe_Object_assign (&tmp, *retval);
         res = nDecimal_add (sbd, abd);
         *retval = joe_BigDecimal_New (res);
         nDecimal_delete (res);
         joe_Object_assign (&tmp, 0);
         return JOE_SUCCESS;
      }
   } else {
      *retval = joe_Exception_New("BigDecimal add: invalid argument number");
   }
   return JOE_FAILURE;
}

static int
subtract(joe_Object self, int argc, joe_Object* argv, joe_Object* retval)
{

   if (argc == 1) {
      nDecimal sbd =get_nDecimal (self);
      if (joe_BigDecimal_bigDecimalValue (argv[0], retval) == JOE_SUCCESS) {
         nDecimal res;
         joe_Object tmp = 0;
         nDecimal abd = get_nDecimal (*retval);
         joe_Object_assign (&tmp, *retval);
         res = nDecimal_subtract (sbd, abd);
         *retval = joe_BigDecimal_New (res);
         nDecimal_delete (res);
         joe_Object_assign (&tmp, 0);
         return JOE_SUCCESS;
      }
   } else {
      *retval = joe_Exception_New("BigDecimal subtract: invalid argument number");
   }
   return JOE_FAILURE;
}

static int
multiply(joe_Object self, int argc, joe_Object* argv, joe_Object* retval)
{

   if (argc == 1) {
      nDecimal sbd =get_nDecimal (self);
      if (joe_BigDecimal_bigDecimalValue (argv[0], retval) == JOE_SUCCESS) {
         nDecimal res;
         joe_Object tmp = 0;
         nDecimal abd = get_nDecimal (*retval);
         joe_Object_assign (&tmp, *retval);
         res = nDecimal_multiply (sbd, abd);
         *retval = joe_BigDecimal_New (res);
         nDecimal_delete (res);
         joe_Object_assign (&tmp, 0);
         return JOE_SUCCESS;
      }
   } else {
      *retval = joe_Exception_New("BigDecimal multiply: invalid argument number");
   }
   return JOE_FAILURE;
}

static int
divide (joe_Object self, int argc, joe_Object* argv, joe_Object* retval)
{

   if (argc == 1) {
      nDecimal sbd =get_nDecimal (self);
      if (joe_BigDecimal_bigDecimalValue (argv[0], retval) == JOE_SUCCESS) {
         nDecimal res;
         joe_Object tmp = 0;
         nDecimal abd = get_nDecimal (*retval);
         joe_Object_assign (&tmp, *retval);
         res = nDecimal_divide (sbd, abd, 16);
         *retval = joe_BigDecimal_New (res);
         nDecimal_delete (res);
         joe_Object_assign (&tmp, 0);
         return JOE_SUCCESS;
      }
   } else {
      *retval = joe_Exception_New("BigDecimal divide: invalid argument number");
   }
   return JOE_FAILURE;
}

static int
remainder (joe_Object self, int argc, joe_Object* argv, joe_Object* retval)
{

   if (argc == 1) {
      nDecimal sbd =get_nDecimal (self);
      if (joe_BigDecimal_bigDecimalValue (argv[0], retval) == JOE_SUCCESS) {
         nDecimal res;
         joe_Object tmp = 0;
         nDecimal abd = get_nDecimal (*retval);
         joe_Object_assign (&tmp, *retval);
         res = nDecimal_remainder (sbd, abd, 16);
         *retval = joe_BigDecimal_New (res);
         nDecimal_delete (res);
         joe_Object_assign (&tmp, 0);
         return JOE_SUCCESS;
      }
   } else {
      *retval = joe_Exception_New("BigDecimal remainder: invalid argument number");
   }
   return JOE_FAILURE;
}

static int
joe_BigDecimal_compareTo (joe_BigDecimal self, joe_BigDecimal bd)
{
   joe_Object tmp = 0;
   nDecimal sbd = get_nDecimal (self);
   nDecimal abd = get_nDecimal (bd);
   return nDecimal_compareTo(sbd, abd);
}

static int
equals (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1) {
      if (joe_BigDecimal_bigDecimalValue (argv[0], retval) == JOE_SUCCESS) {
         joe_Object tmp = 0;
         joe_Object_assign (&tmp, *retval);
         if (joe_BigDecimal_compareTo (self, tmp) == 0)
            *retval = joe_Boolean_New_true();
         else
            *retval = joe_Boolean_New_false();
         joe_Object_assign (&tmp, 0);
      } else {
         return JOE_FAILURE;
      }
   } else {
      *retval = joe_Boolean_New_false();
   }
   return JOE_SUCCESS;
}

static int
ne (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1) {
      if (joe_BigDecimal_bigDecimalValue (argv[0], retval) == JOE_SUCCESS) {
         joe_Object tmp = 0;
         joe_Object_assign (&tmp, *retval);
         if (joe_BigDecimal_compareTo (self, tmp) != 0)
            *retval = joe_Boolean_New_true();
         else
            *retval = joe_Boolean_New_false();
         joe_Object_assign (&tmp, 0);
      } else {
         return JOE_FAILURE;
      }
   } else {
      *retval = joe_Boolean_New_false();
   }
   return JOE_SUCCESS;
}

static int
ge (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1) {
      if (joe_BigDecimal_bigDecimalValue (argv[0], retval) == JOE_SUCCESS) {
         joe_Object tmp = 0;
         joe_Object_assign (&tmp, *retval);
         if (joe_BigDecimal_compareTo (self, tmp) >= 0)
            *retval = joe_Boolean_New_true();
         else
            *retval = joe_Boolean_New_false();
         joe_Object_assign (&tmp, 0);
      } else {
         return JOE_FAILURE;
      }
   } else {
      *retval = joe_Boolean_New_false();
   }
   return JOE_SUCCESS;
}

static int
le (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1) {
      if (joe_BigDecimal_bigDecimalValue (argv[0], retval) == JOE_SUCCESS) {
         joe_Object tmp = 0;
         joe_Object_assign (&tmp, *retval);
         if (joe_BigDecimal_compareTo (self, tmp) <= 0)
            *retval = joe_Boolean_New_true();
         else
            *retval = joe_Boolean_New_false();
         joe_Object_assign (&tmp, 0);
      } else {
         return JOE_FAILURE;
      }
   } else {
      *retval = joe_Boolean_New_false();
   }
   return JOE_SUCCESS;
}

static int
gt (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1) {
      if (joe_BigDecimal_bigDecimalValue (argv[0], retval) == JOE_SUCCESS) {
         joe_Object tmp = 0;
         joe_Object_assign (&tmp, *retval);
         if (joe_BigDecimal_compareTo (self, tmp) > 0)
            *retval = joe_Boolean_New_true();
         else
            *retval = joe_Boolean_New_false();
         joe_Object_assign (&tmp, 0);
      } else {
         return JOE_FAILURE;
      }
   } else {
      *retval = joe_Boolean_New_false();
   }
   return JOE_SUCCESS;
}

static int
lt (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1) {
      if (joe_BigDecimal_bigDecimalValue (argv[0], retval) == JOE_SUCCESS) {
         joe_Object tmp = 0;
         joe_Object_assign (&tmp, *retval);
         if (joe_BigDecimal_compareTo (self, tmp) < 0)
            *retval = joe_Boolean_New_true();
         else
            *retval = joe_Boolean_New_false();
         joe_Object_assign (&tmp, 0);
      } else {
         return JOE_FAILURE;
      }
   } else {
      *retval = joe_Boolean_New_false();
   }
   return JOE_SUCCESS;
}

static int
toString (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   joe_Memory mem = *joe_Object_at (self, 0);
   nDecimal num = (nDecimal) joe_Object_getMem (mem);
   char* strNum = nDecimal_toString(num);
   *retval = joe_String_New (nDecimal_toString (num));
   free(strNum);
   return JOE_SUCCESS;
}

static joe_Method mthds[] = {
   {"add", add },
   {"subtract", subtract },
   {"multiply", multiply },
   {"divide", divide },
   {"remainder", remainder },
   {"equals", equals },
   {"ne", ne },
   {"gt", gt },
   {"ge", ge },
   {"lt", lt },
   {"le", le },
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
joe_BigDecimal_New_lng(long num)
{
    return joe_BigDecimal_New(nDecimal_new_lng(num));
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
      *retval = self;
   } else {
      nDecimal bd;
      if (joe_Object_instanceOf(self, &joe_Integer_Class)) {
         bd = nDecimal_new_lng(*((long*)joe_Object_getMem(self)));
      } else if (joe_Object_instanceOf(self, &joe_Float_Class)) {
         bd = nDecimal_new_dbl(*((double*)joe_Object_getMem(self)));
      } else if (joe_Object_instanceOf(self, &joe_String_Class)) {
         bd = nDecimal_new_str (joe_String_getCharStar(self));
         if (nDecimal_wrongAssignment (bd)) {
            *retval = joe_Exception_New ("BigDecimal: not numeric format");
            nDecimal_delete (bd);
            return JOE_FAILURE;
         }
      } else {
         *retval = joe_Exception_New("BigDecimal: not numeric value");
         return JOE_FAILURE;
      }
      *retval = joe_BigDecimal_New (bd);
      nDecimal_delete (bd);
   }
   return JOE_SUCCESS;
}


int joe_BigDecimal_oper(joe_Object self, int argc, joe_Object* argv,
                        joe_Object* retval, enum joe_BigDecimal_ops op) {
   switch (op) {
   case ADD:
      return add (self, argc, argv, retval);
   case SUBTRACT:
      return subtract (self, argc, argv, retval);
   case MULTIPLY:
      return multiply (self, argc, argv, retval);
   case DIVIDE:
      return divide (self, argc, argv, retval);
   case EQUALS:
      return equals (self, argc, argv, retval);
   case REMAINDER:
      return remainder (self, argc, argv, retval);
   case LT:
      return lt (self, argc, argv, retval);
   case GT:
      return gt (self, argc, argv, retval);
   case LE:
      return le (self, argc, argv, retval);
   case GE:
      return ge (self, argc, argv, retval);
   case NE:
      return ne (self, argc, argv, retval);
   }
}
