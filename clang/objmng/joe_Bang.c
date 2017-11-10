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
# include <string.h>
# include "joe_Bang.h"
# include "joe_Array.h"
# include "joe_Block.h"
# include "joe_Boolean.h"
# include "joe_Integer.h"
# include "joe_String.h"
# include "joe_BreakBlockException.h"
# include "joe_BreakLoopException.h"

static int Switch_case (joe_Object self,
                        int argc, joe_Object *argv, joe_Object *retval);
static int Switch_end (joe_Object self,
                       int argc, joe_Object *argv, joe_Object *retval);
static int Switch_default (joe_Object self,
                           int argc, joe_Object *argv, joe_Object *retval);
static joe_Method switchMthds[] = {
  {"case", Switch_case},
  {"default", Switch_default},
  {"endSwitch", Switch_end},
  {(void *) 0, (void *) 0}
};

#define SW_CFRT1        0
#define SW_ALREADY_DONE 1
#define SW_PREV_COND    2
#define SW_RETURN       3
static char* switchVariables[] = {
   "cfrt1", "alreadyDone", "prevCond", "return", 0
};

static joe_Class switchClazz = {
   "joe_Bang$Switch",
   0,
   0,
   switchMthds,
   switchVariables,
   &joe_Object_Class,
   0
};

joe_Object
Switch_New (joe_Object cfrt)
{
   joe_Object self = joe_Object_New (&switchClazz, 0);

   joe_Object_assign (joe_Object_at (self,SW_CFRT1), cfrt);
   joe_Object_assign (joe_Object_at (self,SW_PREV_COND),
                                    joe_Boolean_New_false());
   joe_Object_assign (joe_Object_at (self,SW_ALREADY_DONE),
                                    joe_Boolean_New_false());
   return self;
}

static int
Switch_case (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   int rc;
   if (argc == 1 || argc == 2) {
      if (joe_Boolean_isTrue (*joe_Object_at (self,SW_ALREADY_DONE))) {
         *retval = self;
         return JOE_SUCCESS;
      } else {
         joe_Object cfrt1 = *joe_Object_at (self,SW_CFRT1);
         joe_Object cfrt2 = argv[0];
         joe_Object or = 0;
         rc = JOE_SUCCESS;
         if (cfrt1 == 0)
            if (cfrt2 == 0)
               or = joe_Boolean_New_true();
            else
               or = joe_Boolean_New_false();
         else
            rc = joe_Object_invoke (cfrt1, "equals", 1, &cfrt2, &or);
         if (rc == JOE_SUCCESS) {
            if (argc == 1) {
               if (joe_Boolean_isTrue (or))
                  joe_Object_assign (joe_Object_at (self,SW_PREV_COND), or);
               *retval = self;
               return JOE_SUCCESS;
            } else {
               if (joe_Object_instanceOf (argv[1], &joe_Block_Class)) {
                  if (joe_Boolean_isTrue (or) ||
                      joe_Boolean_isTrue (*joe_Object_at (self,SW_PREV_COND))){
                     joe_Object Return = 0;
                     rc = joe_Object_invoke (argv[1], "exec", 0, 0, &Return);
                     joe_Object_assign (joe_Object_at (self,SW_ALREADY_DONE),
                                        joe_Boolean_New_true());
                     if (rc == JOE_SUCCESS) {
                        joe_Object_assign (joe_Object_at (self,SW_RETURN),
                                           Return);
                        *retval = self;
                        return JOE_SUCCESS;
                     } else {
                        *retval = Return;
                        return JOE_FAILURE;
                     }
                  } else {
                     *retval = self;
                     return JOE_SUCCESS;
                  }
               } else {
                  *retval = joe_Exception_New ("case: invalid block");
                  return JOE_FAILURE;
               }
            }
         } else {
            *retval = or;
             return JOE_FAILURE;
         }
      }
   } else {
      *retval = joe_Exception_New ("case: invalid argument number");
      return JOE_FAILURE;
   }
}

static int
Switch_default (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   int rc;
   if (argc == 1 && joe_Object_instanceOf (argv[0], &joe_Block_Class)) {
      if (joe_Boolean_isTrue (*joe_Object_at (self,SW_ALREADY_DONE))) {
         *retval = self;
         return JOE_SUCCESS;
      } else {
        joe_Object Return = 0;
        rc = joe_Object_invoke (argv[0], "exec", 0, 0, &Return);
        joe_Object_assign (joe_Object_at (self,SW_ALREADY_DONE),
                           joe_Boolean_New_true());
        if (rc == JOE_SUCCESS) {
           joe_Object_assign (joe_Object_at (self,SW_RETURN), Return);
           *retval = self;
           return JOE_SUCCESS;
        } else {
           *retval = Return;
            return JOE_FAILURE;
        }
      }
   } else {
      *retval = joe_Exception_New ("default: invalid argument");
      return JOE_FAILURE;
   }
}

static int
Switch_end (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   *retval = *joe_Object_at (self,SW_RETURN);
   return JOE_SUCCESS;
}

static int
isNull (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1) {
      if (argv[0] == 0)
        *retval = joe_Boolean_New_true();
      else
        *retval = joe_Boolean_New_false();
      return JOE_SUCCESS;
   }
   *retval = joe_Exception_New ("isNull: invalid argument number");
   return JOE_FAILURE;
}

static int
print (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   int i;
   int rc;
   if (argc > 0 && argv) {
      joe_String str = 0;
      for (i = 0; i < argc; i++) {
         if (argv[i]) {
            rc = joe_Object_assignInvoke (argv[i], "toString", 0, 0, &str);
            if (rc == JOE_SUCCESS) {
               fputs (joe_String_getCharStar (str), stdout);
            } else {
               joe_Object_decrReference (&str);
               *retval = str;
               return rc;
            }
         } else {
            fputs ("(null)", stdout);
         }
      }
      joe_Object_assign (&str, 0);
   }
   *retval = self;
   return JOE_SUCCESS;
}

static int
println (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   int rc;
   if ((rc = print (self, argc, argv, retval)) == JOE_SUCCESS)
      fputs ("\r\n", stdout);

   return rc;
}

static char inbuff[255];
static int
readLine (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   int i;
   fgets (inbuff, sizeof(inbuff), stdin);
   i = strlen (inbuff);
   for (i--; inbuff[i] < ' '; i--)
     inbuff[i] = 0;
   *retval = joe_String_New (inbuff);
   return JOE_SUCCESS;
}

static int
_while (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   int rc;
   joe_Object lretval = 0;
   int goOn = 1;
   if (argc == 2) {
      joe_Object bol;
      if (joe_Object_instanceOf (argv[0], &joe_Block_Class) &&
          joe_Object_instanceOf (argv[1], &joe_Block_Class)) {
         while (goOn) {
            bol = 0;
            if ((rc=joe_Object_invoke (argv[0],"exec",0,0,&bol))!=JOE_SUCCESS) {
               joe_Object_assign (retval,  bol);
               joe_Object_decrReference (retval);
               return JOE_FAILURE;
            }
            if (!joe_Object_instanceOf (bol, &joe_Boolean_Class)) {
               joe_String msg = joe_String_New2 ("while: not a condition -> ",
                                                  joe_Object_getClassName(bol));
               joe_Object_assign (retval, joe_Exception_New_string (msg));
               joe_Object_delIfUnassigned (&msg);
               joe_Object_decrReference (retval);
               return JOE_FAILURE;
            }
            if (joe_Boolean_isTrue (bol)) {
               if ((rc=joe_Object_invoke(argv[1],"exec",0,0,&lretval))
                                                            !=JOE_SUCCESS){
                  if (joe_Object_instanceOf (lretval,
                                            &joe_BreakLoopException_Class)) {
                     joe_Object retobj =
                                  joe_BreakException_getReturnObj(lretval);
                     if (retobj)
                        *retval = retobj;
                     goOn = 0;
                     joe_Object_delIfUnassigned (&lretval);
                     return JOE_SUCCESS;
                  } else {
                     joe_Object_assign (retval, lretval);
                     *retval = lretval;
                     joe_Object_decrReference (retval);
                     return JOE_FAILURE;
                  }
               } else {
                  joe_Object_assign (retval, lretval);
               }
            } else {
               joe_Object_decrReference (retval);
               return JOE_SUCCESS;
            }
         }
      } else {
         joe_String msg = joe_String_New4 ("while: invalid arguments -> ",
                                           joe_Object_getClassName(argv[0]),
                                           " ",
                                           joe_Object_getClassName(argv[1])
                                           );
         *retval = joe_Exception_New_string (msg);
         joe_Object_delIfUnassigned (&msg);
         return JOE_FAILURE;
      }
      return JOE_SUCCESS;
   }

   *retval = joe_Exception_New ("while: invalid argument number");
   return JOE_FAILURE;
}

static int
foreach (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   int rc;
   joe_Object lretval = 0;
   if (argc == 2) {
      if (joe_Object_instanceOf (argv[0], &joe_Array_Class) &&
          joe_Object_instanceOf (argv[1], &joe_Block_Class)) {
         int len = joe_Array_length (argv[0]);
         int i;
         for (i = 0; i < len; i++) {
            if ((rc=joe_Object_invoke(argv[1],"exec",
                                      1, joe_Object_at(argv[0],i),
                                                   &lretval)) != JOE_SUCCESS) {
               if (joe_Object_instanceOf (lretval,
                                            &joe_BreakLoopException_Class)) {
                  joe_Object retobj = joe_BreakException_getReturnObj(lretval);
                  if (retobj)
                     *retval = retobj;
                  joe_Object_delIfUnassigned (&lretval);
                  return JOE_SUCCESS;
               } else {
                  *retval = lretval;
                  return JOE_FAILURE;
               }
            } else {
               *retval = lretval;
            }
         }
         return JOE_SUCCESS;
      } else {
         joe_String msg = joe_String_New4 ("foreach: invalid argument: ",
                                           joe_Object_getClassName(argv[0]),
                                           ",",
                                           joe_Object_getClassName(argv[1]));
         *retval = joe_Exception_New_string (msg);
         return JOE_FAILURE;
      }
   }
   *retval = joe_Exception_New ("forwach: invalid argument number");
   return JOE_FAILURE;
}

static int
_if (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   int rc;
   if (argc > 1) {
      joe_Object bol = 0;
      if (joe_Object_instanceOf (argv[0], &joe_Block_Class)) {
         if ((rc=joe_Object_invoke (argv[0],"exec",0,0,&bol))!=JOE_SUCCESS) {
            *retval = bol;
            return JOE_FAILURE;
         }
      } else {
         bol = argv[0];
      }
      if (!joe_Object_instanceOf (bol, &joe_Boolean_Class)) {
         *retval = joe_Exception_New ("if: invalid condition");
          return JOE_FAILURE;
      }
      if (joe_Boolean_isTrue (bol)) {
         if (joe_Object_instanceOf (argv[1], &joe_Block_Class)) {
            if ((rc=joe_Object_invoke(argv[1],"exec",0,0,retval))!=JOE_SUCCESS){
               return JOE_FAILURE;
            }
          } else {
             *retval = joe_Exception_New ("if: invalid 2nd argument");
             return JOE_FAILURE;
          }
      } else if (argc == 3) {
         if (joe_Object_instanceOf (argv[2], &joe_Block_Class)) {
            if ((rc=joe_Object_invoke(argv[2],"exec",0,0,retval))!=JOE_SUCCESS){
               return JOE_FAILURE;
            }
          } else {
             *retval = joe_Exception_New ("if: invalid 3rd argument");
             return JOE_FAILURE;
          }
      }
      return JOE_SUCCESS;
   }

   *retval = joe_Exception_New ("if: invalid argument");
   return JOE_FAILURE;
}
static int
_switch (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1) {
      *retval = Switch_New (argv[0]);
      return JOE_SUCCESS;
   } else {
      *retval = joe_Exception_New ("switch: invalid argument number");
      return JOE_FAILURE;
   }
}

static int
newInstance (joe_Object self, int argc, joe_Object *args, joe_Object *retval)
{
   if (argc > 0 && joe_Object_instanceOf (args[0], &joe_String_Class)) {
      int rc = 0;
      joe_Object *newArgs = argc == 1 ? 0 : &args[1];
      joe_Class *clazz = joe_Class_getClass (joe_String_getCharStar (args[0]));
      if (clazz) {
         rc = joe_Class_newInstance (clazz, argc -1, newArgs, retval);
      } else {
         *retval = joe_Exception_New ("newInstance: class not found");
         rc = JOE_FAILURE;
      }
      return rc;
   } else {
      *retval = joe_Exception_New ("newInstance: invalid argument");
      return JOE_FAILURE;
   }
}
static int
_throw (joe_Object self, int argc, joe_Object *args, joe_Object *retval)
{
   if (argc > 0 && joe_Object_instanceOf (args[0], &joe_String_Class)) {
      joe_String descr = 0;
      joe_Object_assignInvoke (args[0], "toString", 0, 0, &descr);
      *retval = joe_Exception_New (joe_String_getCharStar(descr));
      joe_Object_assign (&descr, 0);
      return JOE_FAILURE;
   } else {
      *retval = joe_Exception_New ("");
      return JOE_FAILURE;
   }
}

static int
_break (joe_Object self, int argc, joe_Object *args, joe_Object *retval)
{
   if (argc > 0 && joe_Object_instanceOf (args[0], &joe_String_Class)) {
      joe_String descr = 0;
      joe_Object_assignInvoke (args[0], "toString", 0, 0, &descr);
      *retval = joe_BreakBlockException_New (joe_String_getCharStar(descr));
      joe_Object_assign (&descr, 0);
      return JOE_FAILURE;
   } else {
      *retval = joe_BreakBlockException_New ("(null)");
      return JOE_FAILURE;
   }
}

static int
breakLoop (joe_Object self, int argc, joe_Object *args, joe_Object *retval)
{
   *retval = joe_BreakLoopException_New ("breakLoop");
   return JOE_FAILURE;
}

extern int joe_BangSO_New (joe_String soName, joe_Object *obj);

static int
loadSO (joe_Object self, int argc, joe_Object *args, joe_Object *retval)
{
   if (argc == 1 && joe_Object_instanceOf (args[0], &joe_String_Class)) {
      return joe_BangSO_New (args[0], retval);
   } else {
      *retval = joe_Exception_New ("loadSO: invaliid argument");
      return JOE_FAILURE;
   }
}

static int
newArray (joe_Object self, int argc, joe_Object *args, joe_Object *retval)
{
   if (argc == 1 && joe_Object_instanceOf (args[0], &joe_Integer_Class)) {
      *retval = joe_Array_New (joe_Integer_value (args[0]));
      return JOE_SUCCESS;
   } else {
      *retval = joe_Exception_New ("newArray: invaliid argument");
      return JOE_FAILURE;
   }
}

static int
array (joe_Object self, int argc, joe_Object *args, joe_Object *retval)
{
   int i;
   *retval = joe_Array_New (argc);
   for (i = 0; i < argc; i++)
      joe_Object_assign (joe_Object_at (*retval, i), args[i]);
   return JOE_SUCCESS;
}

static joe_Method mthds[] = {
  {"if", _if},
  {"while", _while},
  {"foreach", foreach},
  {"switch", _switch},
  {"isNull", isNull},
  {"println", println},
  {"print", print},
  {"readLine", readLine},
  {"newInstance", newInstance},
  {"throw", _throw},
  {"break", _break},
  {"breakLoop", breakLoop},
  {"array", array},
  {"newArray", newArray},
  {"loadSO", loadSO},
  {(void *) 0, (void *) 0}
};

joe_Class joe_Bang_Class = {
   "joe_Bang",
   0,
   0,
   mthds,
   0,
   &joe_Object_Class,
   0
};

joe_Object
joe_Bang_New () {
   joe_Object self = joe_Object_New (&joe_Bang_Class, 0);
   return self;
}
