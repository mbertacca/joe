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
# include <math.h>
# include "joe_Exception.h"
# include "joe_Float.h"
# include "joe_Integer.h"

static joe_Class* float_Class;
static joe_Class* integer_Class;

# define FUNC1(f) \
static int \
joe_##f (joe_Object self, int argc, joe_Object *argv, joe_Object *retval) \
{ \
   if (argc == 1) { \
      if (joe_Object_instanceOf (argv[0], float_Class)) { \
         joe_Object_assign(retval, \
                   joe_Float_New (f (joe_Float_value(argv[0])))); \
         return JOE_SUCCESS; \
      } else if (joe_Object_instanceOf (argv[0], integer_Class)) { \
         joe_Object_assign(retval, \
                   joe_Float_New (f ((double) joe_Integer_value(argv[0])))); \
         return JOE_SUCCESS; \
      } \
   } \
   joe_Object_assign(retval, \
                        joe_Exception_New (#f ": invalid argument(s)")); \
   return JOE_FAILURE; \
}

# define FUNC2(f) \
static int \
joe_##f (joe_Object self, int argc, joe_Object *argv, joe_Object *retval) \
{ \
   if (argc == 2) { \
      if (joe_Object_instanceOf (argv[0], float_Class)) { \
         if (joe_Object_instanceOf (argv[1], float_Class)) { \
            joe_Object_assign(retval, \
                   joe_Float_New (f (joe_Float_value(argv[0]), \
                                     joe_Float_value(argv[1])))); \
            return JOE_SUCCESS; \
         } else if (joe_Object_instanceOf (argv[1], integer_Class)) { \
            joe_Object_assign(retval, \
                   joe_Float_New (f (joe_Float_value(argv[0]), \
                            (double) joe_Integer_value(argv[1])))); \
            return JOE_SUCCESS; \
         } \
      } else if (joe_Object_instanceOf (argv[0], integer_Class)) { \
         if (joe_Object_instanceOf (argv[1], float_Class)) { \
            joe_Object_assign(retval, \
                   joe_Float_New (f ((double) joe_Integer_value(argv[0]), \
                                              joe_Float_value(argv[1])))); \
            return JOE_SUCCESS; \
         } else if (joe_Object_instanceOf (argv[1], integer_Class)) { \
            joe_Object_assign(retval, \
                   joe_Float_New (f ((double) joe_Integer_value(argv[0]), \
                                     (double) joe_Integer_value(argv[1])))); \
            return JOE_SUCCESS; \
         } \
      } \
  } \
  joe_Object_assign(retval, \
                    joe_Exception_New (#f ": invalid argument(s)")); \
  return JOE_FAILURE; \
}

FUNC1(acos)
FUNC1(asin)
FUNC1(atan)
FUNC2(atan2)
FUNC1(ceil)
FUNC1(cos)
FUNC1(cosh)
FUNC1(exp)
FUNC1(fabs)
FUNC1(floor)
FUNC2(fmod)
FUNC1(log)
FUNC1(log10)
FUNC2(pow)
FUNC1(sin)
FUNC1(sinh)
FUNC1(sqrt)
FUNC1(tan)
FUNC1(tanh)

static char *variables[] = { 0 };

static joe_Method mthds[] = {
   {"acos", joe_acos },
   {"asin", joe_asin },
   {"atan", joe_atan },
   {"atan2", joe_atan2 },
   {"ceil", joe_ceil },
   {"cos", joe_cos },
   {"cosh", joe_cosh },
   {"exp", joe_exp },
   {"fabs", joe_fabs },
   {"abs", joe_fabs },
   {"floor", joe_floor },
   {"fmod", joe_fmod },
   {"log", joe_log },
   {"log10", joe_log10 },
   {"pow", joe_pow },
   {"sin", joe_sin },
   {"sinh", joe_sinh },
   {"sqrt", joe_sqrt },
   {"tan", joe_tan },
   {"tanh", joe_tanh },
   {(void *) 0, (void *) 0}
};

static joe_Class joe_Math_Class = {
   "joe_Math",
   0,
   0,
   mthds,
   variables,
   0,
   0
};

JOEOBJ_API void
joe_init ()
{
   joe_Math_Class.extends = joe_Class_getClass("joe_Object");
   joe_Class_registerClass (&joe_Math_Class);
   float_Class = joe_Class_getClass("joe_Float");
   integer_Class = joe_Class_getClass("joe_Integer");
}
