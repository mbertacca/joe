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
# include <errno.h>
# include <time.h>
# include <string.h>
# include <stdlib.h>
# include <unistd.h>
# include "joe_Bang.h"
# include "joe_Array.h"
# include "joe_List.h"
# include "joe_Block.h"
# include "joe_Boolean.h"
# include "joe_Execute.h"
# include "joe_Integer.h"
# include "joe_Null.h"
# include "joe_Files.h"
# include "joe_String.h"
# include "joe_StringBuilder.h"
# include "joe_BreakBlockException.h"
# include "joe_BreakLoopException.h"
# include "joe_GotoException.h"
# include "joe_Gosub.h"
# include "joe_Glob.h"
# include "joe_LoadScript.h"
# include "joe_DoDebugException.h"
# include "joestrct.h"

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
         joe_Object_assign (retval, self);
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
         else {
            joe_Object_assign (&or,*joe_Object_at (self,SW_PREV_COND));
            if (joe_Boolean_isFalse (or)) {
               if (joe_Object_instanceOf (argv[0], &joe_Block_Class)) {
                  rc = joe_Object_invoke (argv[0], "exec", 0, 0, &or);
                  if (rc != JOE_SUCCESS) {
                     joe_Object_assign(retval, or);
                     joe_Object_assign(&or, 0);
                     return JOE_FAILURE;
                  }
                  rc = joe_Object_invoke (cfrt1, "equals", 1, &or, &or);
               } else {
                  rc = joe_Object_invoke (cfrt1, "equals", 1, &cfrt2, &or);
               }
            }
         }
         if (rc == JOE_SUCCESS) {
            if (argc == 1) {
               if (joe_Boolean_isTrue (or)) {
                  joe_Object_assign (joe_Object_at (self,SW_PREV_COND), or);
               }
               joe_Object_assign(retval, self);
               return JOE_SUCCESS;
            } else {
               if (joe_Object_instanceOf (argv[1], &joe_Block_Class)) {
                  if (joe_Boolean_isTrue (or)) {
                     joe_Object Return = 0;
                     rc = joe_Object_invoke (argv[1], "exec", 0, 0, &Return);
                     joe_Object_assign (joe_Object_at (self,SW_ALREADY_DONE),
                                        joe_Boolean_New_true());
                     if (rc == JOE_SUCCESS) {
                        joe_Object_assign (joe_Object_at (self,SW_RETURN),
                                           Return);
                        joe_Object_assign(retval, self);
                        joe_Object_assign(&Return, 0);
                        return JOE_SUCCESS;
                     } else {
                        joe_Object_assign(retval, Return);
                        joe_Object_assign(&Return, 0);
                        return JOE_FAILURE;
                     }
                  } else {
                     joe_Object_assign(retval, self);
                     return JOE_SUCCESS;
                  }
               } else {
                  joe_Object_assign(retval, joe_Exception_New ("case: invalid block"));
                  return JOE_FAILURE;
               }
            }
         } else {
            joe_Object_assign(retval, or);
            joe_Object_assign(&or, 0);
            return JOE_FAILURE;
         }
      }
   } else {
      joe_Object_assign(retval, joe_Exception_New ("case: invalid argument number"));
      return JOE_FAILURE;
   }
}

static int
Switch_default (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   int rc;
   if (argc == 1 && joe_Object_instanceOf (argv[0], &joe_Block_Class)) {
      if (joe_Boolean_isTrue (*joe_Object_at (self,SW_ALREADY_DONE))) {
         joe_Object_assign(retval, self);
         return JOE_SUCCESS;
      } else {
        joe_Object Return = 0;
        rc = joe_Object_invoke (argv[0], "exec", 0, 0, &Return);
        joe_Object_assign (joe_Object_at (self,SW_ALREADY_DONE),
                           joe_Boolean_New_true());
        if (rc == JOE_SUCCESS) {
           joe_Object_transfer (joe_Object_at (self,SW_RETURN), &Return);
           joe_Object_assign(retval, self);
           return JOE_SUCCESS;
        } else {
           joe_Object_transfer(retval, &Return);
            return JOE_FAILURE;
        }
      }
   } else {
      joe_Object_assign(retval, joe_Exception_New ("default: invalid argument"));
      return JOE_FAILURE;
   }
}

static int
Switch_end (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   joe_Object_assign(retval, *joe_Object_at (self,SW_RETURN));
   return JOE_SUCCESS;
}

static void
args2String(int argc, joe_Object* argv, joe_Object* retval)
{
   int i;
   joe_StringBuilder msg = 0;
   joe_Object_assign(&msg, joe_StringBuilder_New());

   for (i = 0; i < argc; i++)
      joe_StringBuilder_append(msg, argv[i]);

   joe_Object_assign(retval, joe_StringBuilder_toString(msg));
   joe_Object_assign(&msg, 0);
}

static int
exec (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc > 0) {
      int i, rc;
      joe_StringBuilder msg = 0;
      joe_Object_assign(&msg, joe_StringBuilder_New());

      for (i = 0; i < argc; i++) {
         joe_StringBuilder_append(msg, argv[i]);
         joe_StringBuilder_appendChar(msg, ' ');
      }

      rc = system (joe_StringBuilder_getCharStar(msg));
      if (rc != 0 && (rc & 0x00FF) == 0)
         rc >>= 8;
      joe_Object_assign(retval, joe_Integer_New (rc));

      joe_Object_assign(&msg, 0);
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval, joe_Exception_New ("exec: invalid argument(s)"));
      return JOE_FAILURE;
   }
}

static int
execFromDir (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc > 1 && joe_Object_instanceOf (argv[0], &joe_String_Class)) {
      int rc;
      int pathLen = 64;
      char *currpath = calloc (1, pathLen);
      while (getcwd (currpath, pathLen) == NULL) {
         pathLen <<= 1;
         currpath = realloc (currpath, pathLen);
      }
      if (chdir (joe_String_getCharStar(argv[0])) == 0) {
         rc = exec (self, (argc - 1), &argv[1], retval);
         chdir (currpath);
      } else {
         joe_Object_assign(retval, joe_Exception_New ("execFromDir: cannot change directory"));
         rc = JOE_FAILURE;
      }
      free (currpath);
      return rc;
   } else {
      joe_Object_assign(retval, joe_Exception_New ("execFromDir: invalid argument(s)"));
      return JOE_FAILURE;
   }
}

static int
version (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   joe_StringBuilder msg = 0;
   joe_Object_assign (&msg, joe_StringBuilder_New ());
   joe_StringBuilder_appendCharStar (msg, "JOE Revision 0.9n ");
   joe_StringBuilder_appendCharStar (msg, __DATE__);
   joe_Object_assign(retval, joe_StringBuilder_toString (msg));
   joe_Object_assign (&msg, 0);
   return JOE_SUCCESS;
}

static int
getOSType (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   joe_StringBuilder msg = 0;
   joe_Object_assign (&msg, joe_StringBuilder_New ());
#ifdef WIN32
   joe_StringBuilder_appendCharStar (msg, "Windows");
#elif __unix__
   joe_StringBuilder_appendCharStar (msg, "Unix");
#else
   joe_StringBuilder_appendCharStar (msg, "Unknown");
# endif
   joe_Object_assign(retval, joe_StringBuilder_toString (msg));
   joe_Object_assign (&msg, 0);
   return JOE_SUCCESS;
}

static int
systemGetenv (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1 && joe_Object_instanceOf (argv[0], &joe_String_Class)) {
      char *var = joe_String_getCharStar (argv[0]);
      char *val = getenv(var);
      if (val) {
         joe_Object_assign (retval, joe_StringBuilder_New ());
         joe_StringBuilder_appendCharStar (*retval, val);
      } else {
         joe_Object_assign(retval, joe_Null_value);
      } 
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval, joe_Exception_New ("systemGetenv: invalid argument"));
      return JOE_FAILURE;
   }
}

static int
chr (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1 && joe_Object_instanceOf (argv[0], &joe_Integer_Class)) {
      int code = joe_Integer_value (argv[0]);
      joe_StringBuilder str = 0;
      joe_Object_assign (&str, joe_StringBuilder_New ());
      joe_StringBuilder_appendChar (str, (char) code);
      joe_Object_assign(retval, joe_StringBuilder_toString (str));
      joe_Object_assign (&str, 0);
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval, joe_Exception_New ("chr: invalid argument"));
      return JOE_FAILURE;
   }
}

static int
asc (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1 && joe_Object_instanceOf (argv[0], &joe_String_Class)) {
      char *chr = joe_String_getCharStar (argv[0]);
      joe_Object_assign(retval, joe_Integer_New ((unsigned char) chr[0]));
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval, joe_Exception_New ("asc: invalid argument"));
      return JOE_FAILURE;
   }
}

static int
isNull (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1) {
      if (argv[0] == joe_Null_value || argv[0] == 0)
         joe_Object_assign(retval, joe_Boolean_New_true());
      else
         joe_Object_assign(retval, joe_Boolean_New_false());
      return JOE_SUCCESS;
   }
   joe_Object_assign(retval, joe_Exception_New ("isNull: invalid argument number"));
   return JOE_FAILURE;
}

static int
print (joe_Object self, int argc, joe_Object *args, joe_Object *retval)
{
   joe_String msg = 0;
   args2String(argc, args, &msg);
   fputs (joe_String_getCharStar (msg), stdout);
   fflush (stdout);

   joe_Object_assign(&msg, 0);
   joe_Object_assign(retval, self);
   return JOE_SUCCESS;
}

static int
println (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   int rc;
   if ((rc = print (self, argc, argv, retval)) == JOE_SUCCESS) {
      fputs ("\n", stdout);
      fflush (stdout);
   }
   return rc;
}

static char* inbuff = 0;
static ssize_t inbufflen = 0;

static int
readLine (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   int i = joe_Files_getline (&inbuff, &inbufflen, stdin);

   for (i--; i >= 0 && inbuff[i] < ' '; i--)
     inbuff[i] = 0;
   joe_Object_assign(retval, joe_String_New (inbuff));
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
               joe_Object_assign(retval, bol);
               joe_Object_assign(&bol, 0);
               return JOE_FAILURE;
            }
            if (!joe_Object_instanceOf (bol, &joe_Boolean_Class)) {
               joe_String msg = joe_String_New2 ("while: not a condition -> ",
                                                  joe_Object_getClassName(bol));
               joe_Object_assign (retval, joe_Exception_New_string (msg));
               joe_Object_delIfUnassigned (&msg);
               joe_Object_assign(&bol, 0);
               /* joe_Object_decrReference (retval); */
               return JOE_FAILURE;
            }
            if (joe_Boolean_isTrue (bol)) {
               if ((rc=joe_Object_invoke(argv[1],"exec",0,0,&lretval))
                                                            !=JOE_SUCCESS){
                  if (joe_Object_instanceOf (lretval,
                                            &joe_BreakLoopException_Class)) {
                     joe_Object_assign(retval,
                                   joe_BreakException_getReturnObj(lretval));
                     goOn = 0;
                     joe_Object_assign (&lretval, 0);
                     return JOE_SUCCESS;
                  } else {
                     joe_Object_assign (retval, lretval);
                     joe_Object_assign(&lretval, 0);
                     return JOE_FAILURE;
                  }
               } else {
                  joe_Object_assign(retval, lretval);
                  joe_Object_assign(&lretval, 0);
               }
               joe_Object_assign (&bol, 0);
            } else {
               /* joe_Object_decrReference (retval); */
               return JOE_SUCCESS;
            }
         }
      } else {
         joe_String msg = joe_String_New4 ("while: invalid arguments -> ",
                                           joe_Object_getClassName(argv[0]),
                                           " ",
                                           joe_Object_getClassName(argv[1])
                                           );
         joe_Object_assign(retval, joe_Exception_New_string(msg));
         joe_Object_delIfUnassigned(&msg);;
         return JOE_FAILURE;
      }
      return JOE_SUCCESS;
   }

   joe_Object_assign(retval, joe_Exception_New ("while: invalid argument number"));
   return JOE_FAILURE;
}

static int
_for(joe_Object self, int argc, joe_Object* argv, joe_Object* retval)
{
   int step, start, end, i, rc;
   joe_Block blk;
   joe_Integer arg = 0;
   joe_Object lretval = 0;

   if (argc == 3 && joe_Object_instanceOf(argv[0], &joe_Integer_Class) &&
                    joe_Object_instanceOf(argv[1], &joe_Integer_Class) &&
                    joe_Object_instanceOf(argv[2], &joe_Block_Class)) {
      step = 1;
      blk = argv[2];
   } else if (argc == 4 && joe_Object_instanceOf(argv[0], &joe_Integer_Class) &&
                           joe_Object_instanceOf(argv[1], &joe_Integer_Class) &&
                           joe_Object_instanceOf(argv[2], &joe_Integer_Class) &&
                           joe_Object_instanceOf(argv[3], &joe_Block_Class)) {
      step = joe_Integer_value (argv[2]);
      blk = argv[3];
   } else {
      joe_Object_assign(retval, joe_Exception_New("for: invalid arguments"));
      return JOE_FAILURE;
   }
   start = joe_Integer_value(argv[0]);
   end = joe_Integer_value(argv[1]);

   joe_Object_assign(&arg, joe_Integer_New(start));
   if (step < 0) {
      i = (start - end) / -step + 1;
   } else if (step > 0) {
      i = (end - start) / step + 1;
   } else
      i = 1;
   if (i < 1)
      i = 1;
   for ( ; i > 0; i--) {
      /* rc = joe_Object_invoke(blk, "exec", 1, &arg, &lretval); */
      rc = joe_Block_exec(blk, 1, &arg, &lretval);
      if (rc != JOE_SUCCESS) {
         joe_Object_assign(&arg, 0);
         if (joe_Object_instanceOf(lretval, &joe_BreakLoopException_Class)) {
            joe_Object retobj = joe_BreakException_getReturnObj(lretval);
            if (retobj)
               joe_Object_assign(retval, retobj);
            joe_Object_assign(&lretval, 0);
            return JOE_SUCCESS;
         } else {
            joe_Object_assign(retval, lretval);
            joe_Object_assign(&lretval, 0);
            return JOE_FAILURE;
         }
      } else {
         joe_Integer_addMe(arg, step);
      }
   }
   joe_Object_assign(&arg, 0);
   joe_Object_assign(retval, 0);
   *retval = lretval;
/*
   joe_Object_assign(retval, lretval);
   joe_Object_assign(&lretval, 0);
*/
   return JOE_SUCCESS;
}

static int
systemExit (joe_Object self, int argc, joe_Object* argv, joe_Object* retval)
{
   int exitCode = 0;

   if (argc == 1 && joe_Object_instanceOf (argv[0], &joe_Integer_Class)) {
      exitCode = (int) joe_Integer_value (argv[0]);
   } else if (argc != 0) {
      joe_Object_assign(retval,
              joe_Exception_New("systemExit: invalid argument"));
      return JOE_FAILURE;
   }
   exit (exitCode);
   return JOE_SUCCESS;
}

static int
unixTime (joe_Object self, int argc, joe_Object* argv, joe_Object* retval){
   if (argc == 0) {
      joe_Object_assign (retval, joe_Integer_New((long) time(NULL)));
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval, joe_Exception_New("unixTime: unhandled argument"));
      return JOE_FAILURE;
   }
}

static int
foreach (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc > 1) {
      return joe_Object_invoke(argv[0],"foreach", argc-1, &argv[1], retval);
   } else {
      joe_Object_assign(retval, joe_Exception_New (
                                    "foreach: invalid argument number"));
      return JOE_FAILURE;
   }
}

static int
_if (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   int rc;
   if (argc > 1) {
      joe_Object bol = 0;
      int isTrue;
      if (joe_Object_instanceOf (argv[0], &joe_Block_Class)) {
         if ((rc=joe_Object_invoke (argv[0],"exec",0,0,&bol))!=JOE_SUCCESS) {
            joe_Object_assign(retval, bol);
            joe_Object_assign(&bol, 0);
            return JOE_FAILURE;
         }
      } else {
         joe_Object_assign(&bol, argv[0]);
      }
      if (!joe_Object_instanceOf (bol, &joe_Boolean_Class)) {
         joe_Object_assign(retval, joe_Exception_New ("if: invalid condition"));
         joe_Object_assign(&bol, 0);
         return JOE_FAILURE;
      }
      isTrue = joe_Boolean_isTrue(bol);
      joe_Object_assign(&bol, 0);

      if (isTrue) {
         if (joe_Object_instanceOf (argv[1], &joe_Block_Class)) {
            if ((rc=joe_Object_invoke(argv[1],"exec",0,0,retval))!=JOE_SUCCESS){
               return JOE_FAILURE;
            }
          } else {
            joe_Object_assign(retval, joe_Exception_New ("if: invalid 2nd argument"));
            return JOE_FAILURE;
          }
      } else if (argc == 3) {
         if (joe_Object_instanceOf (argv[2], &joe_Block_Class)) {
            if ((rc=joe_Object_invoke(argv[2],"exec",0,0,retval))!=JOE_SUCCESS){
               return JOE_FAILURE;
            }
          } else {
            joe_Object_assign(retval, joe_Exception_New ("if: invalid 3rd argument"));
             return JOE_FAILURE;
          }
      }
      return JOE_SUCCESS;
   }

   joe_Object_assign(retval, joe_Exception_New ("if: missing argument"));
   return JOE_FAILURE;
}

static int
TRY(joe_Object self, int argc, joe_Object* argv, joe_Object* retval)
{
   if (argc >= 1 && joe_Object_instanceOf(argv[0], &joe_Block_Class)) {
      int rc;
      if (argc == 1 || 
          (argc == 2 && joe_Object_instanceOf(argv[0], &joe_Block_Class))) {
         if ((rc = joe_Object_invoke(argv[0], "exec", 0, 0, retval)) != JOE_SUCCESS) {
            if (argc == 2) {
               joe_Object largs[1] = { 0 };
               joe_Object_assign(&largs[0], *retval);
               joe_Object_assign(retval, 0);
               rc = joe_Object_invoke(argv[1], "exec", 1, largs, retval);
               joe_Object_assign(&largs[0], 0);
            } else {
               joe_Object_assign (retval, 0);
               rc = JOE_SUCCESS;
            }
            return rc;
         } else {
            return rc;
         }
      }
   }
   joe_Object_assign(retval, joe_Exception_New("try: invalid or missing argument(s)"));
   return JOE_FAILURE;
}

static int
_switch (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1) {
      joe_Object_assign(retval, Switch_New (argv[0]));
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval, joe_Exception_New ("switch: invalid argument number"));
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
         joe_Object lretval = 0;
         rc = joe_Class_newInstance (clazz, argc -1, newArgs, &lretval);
         joe_Object_assign(retval, lretval);
         joe_Object_assign(&lretval, 0);
      } else {
         joe_Object_assign(retval, joe_Exception_New ("newInstance: class not found"));
         rc = JOE_FAILURE;
      }
      return rc;
   } else {
      joe_Object_assign(retval, joe_Exception_New ("newInstance: invalid argument"));
      return JOE_FAILURE;
   }
}
static int
_throw (joe_Object self, int argc, joe_Object *args, joe_Object *retval)
{
   joe_String descr = 0;
   args2String (argc, args, &descr);

   joe_Object_assign(retval, joe_Exception_New (joe_String_getCharStar(descr)));
   joe_Object_assign (&descr, 0);

   return JOE_FAILURE;
}

static int
_break (joe_Object self, int argc, joe_Object *args, joe_Object *retval)
{
   if (argc == 0) {
      joe_Object_assign(retval, joe_BreakBlockException_New(""));
   } else if (argc == 1 && joe_Object_instanceOf (args[0], &joe_String_Class)) {
      joe_Object_assign(retval, joe_BreakBlockException_New (
                                          joe_String_getCharStar(args[0])));
   } else {
      joe_Object_assign(retval, joe_Exception_New("break: Invalid argument(s)"));
   }
   return JOE_FAILURE;
}

static int
breakLoop (joe_Object self, int argc, joe_Object *args, joe_Object *retval)
{
   joe_Object_assign (retval, joe_BreakLoopException_New ("breakLoop"));
   return JOE_FAILURE;
}

static int
_goto (joe_Object self, int argc, joe_Object *args, joe_Object *retval)
{
   if (argc == 1 && joe_Object_instanceOf (args[0], &joe_String_Class)) {
      joe_Object_assign(retval, joe_GotoException_New (
                                          joe_String_getCharStar(args[0])));
   } else {
      joe_Object_assign(retval, joe_Exception_New("goto: Invalid argument(s)"));
   }
   return JOE_FAILURE;
}

static int
debug (joe_Object self, int argc, joe_Object *args, joe_Object *retval)
{
   joe_Object_assign(retval, joe_DoDebugException_New ());
   return JOE_FAILURE;
}

static int
gosub (joe_Object self, int argc, joe_Object *args, joe_Object *retval)
{
   if (argc == 1 && joe_Object_instanceOf (args[0], &joe_String_Class)) {
      joe_Object_assign(retval, joe_Gosub_New (args[0]));
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval, joe_Exception_New("gosub: Invalid argument(s)"));
      return JOE_FAILURE;
   }
}

static int
_return (joe_Object self, int argc, joe_Object *args, joe_Object *retval)
{
   joe_Object_assign(retval, joe_BreakException_New ("return without gosub"));
   return JOE_FAILURE;
}

static int
loadScript (joe_String scriptName, joe_Object *retval)
{
   joe_Class* loadScriptClass = joe_Class_getClass("joe_LoadScript");
   return joe_Class_newInstance(loadScriptClass, 1, &scriptName, retval);
}

static int
_new (joe_Object self, int argc, joe_Object *args, joe_Object *retval)
{
   if (argc > 0 && joe_Object_instanceOf (args[0], &joe_String_Class)) {
      int rc;
      rc =loadScript (args[0], retval);
      if (rc == JOE_SUCCESS) {
         joe_LoadScript block = 0;
         joe_Object_transfer(&block, retval);
         rc = joe_Block_new (block, (argc - 1), &args[1], retval);
         joe_Object_assign(&block, 0);
         return rc;
      }
   } else {
      joe_Object_assign(retval, joe_Exception_New("new: Invalid argument(s)"));
   }
   return JOE_FAILURE;
}

static int
runJoe (joe_Object self, int argc, joe_Object *args, joe_Object *retval)
{
   if (argc > 0 && joe_Object_instanceOf (args[0], &joe_String_Class)) {
      int rc;
      rc =loadScript (args[0], retval);
      if (rc == JOE_SUCCESS) {
         int i;
         joe_LoadScript block = 0;
         joe_Array argArray = 0;
         joe_Object_transfer(&block, retval);

         joe_Object_assign(&argArray, joe_Array_New (argc));
         for (i = 0; i < argc; i++) {
            joe_Object_assign (joe_Object_at (argArray, i), args[i]);
         }

         rc = joe_Block_outer_exec (block, 1, &argArray, retval);
         joe_Object_assign(&block, 0);
         joe_Object_assign(&argArray, 0);
         return rc;
      }
   } else {
      joe_Object_assign(retval, joe_Exception_New("new: Invalid argument(s)"));
   }
   return JOE_FAILURE;
}

static int
runAsBlock (joe_Object self, int argc, joe_Object *args, joe_Object *retval)
{
   if (argc > 1 && joe_Object_instanceOf (args[0], &joe_Block_Class)
                && joe_Object_instanceOf (args[1], &joe_String_Class)) {
      char *scriptName = joe_String_getCharStar(args[1]);
      FILE *scriptFile = joe_LoadScript_getFile (scriptName);
      if (scriptFile) {
         int Return;
         char *line = 0;
         ssize_t lineLen = 0;
         joe_Execute exec = 0;
         joe_Array argv = 0;
         joe_Object_assign (&exec, joe_Execute_New (args[0], scriptName));
         while (joe_Files_getline (&line, &lineLen, scriptFile) >= 0) {
            joe_Execute_add (exec, line);
         }
         fclose (scriptFile);
         if (argc > 2 && joe_Object_instanceOf (args[2], &joe_Array_Class)) {
            int i;
            int len = joe_Array_length(args[2]);
            joe_Object_assign (&argv, joe_Array_New (len));
            for (i = 0; i < len; i++)
               joe_Object_assign (JOE_AT(argv, i), *JOE_AT(args[2], i));
         } else {
            joe_Object_assign (&argv, joe_Array_New (1));
            joe_Object_assign (JOE_AT(argv, 0), args[1]);
         }
         Return = joe_Execute_exec (exec, 1, &argv, retval);

         joe_Object_assign(&argv, 0);
         joe_Object_assign(&exec, 0);
         if (line)
            free (line);
         return Return;
      } else {
         joe_StringBuilder msg = joe_StringBuilder_New ();
         joe_StringBuilder_appendCharStar (msg, "cannot access ");
         joe_StringBuilder_appendCharStar (msg, scriptName);
         joe_StringBuilder_appendCharStar (msg, " (errno=");
         joe_StringBuilder_appendInt (msg, errno);
         joe_StringBuilder_appendChar (msg, ')');
         joe_Object_assign(retval,
             joe_Exception_New_string (joe_StringBuilder_toString (msg)));
         joe_Object_delIfUnassigned (&msg);
         return JOE_FAILURE;
      }
   } else {
      joe_Object_assign(retval,
                        joe_Exception_New("runAsBlock: Invalid argument(s)"));
      return JOE_FAILURE;
   }
}

static int
newArray (joe_Object self, int argc, joe_Object *args, joe_Object *retval)
{
   if (argc == 1 && joe_Object_instanceOf (args[0], &joe_Integer_Class)) {
      joe_Object_assign(retval, joe_Array_New (joe_Integer_value (args[0])));
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval, joe_Exception_New ("newArray: invaliid argument"));
      return JOE_FAILURE;
   }
}

static int
array (joe_Object self, int argc, joe_Object *args, joe_Object *retval)
{
   int i;
   joe_Object_assign (retval, joe_Array_New (argc));
   for (i = 0; i < argc; i++)
      joe_Object_assign (joe_Object_at (*retval, i), args[i]);
   return JOE_SUCCESS;
}


static int
getGlob (joe_Object self, int argc, joe_Object *args, joe_Object *retval)
{
   return joe_Class_newInstance (&joe_Glob_Class, argc, args, retval);
}

extern int joe_BangSO_New(joe_String soName, joe_Object* obj);

static int
loadSO(joe_Object self, int argc, joe_Object* args, joe_Object* retval)
{
   if (argc == 1 && joe_Object_instanceOf(args[0], &joe_String_Class)) {
      return joe_BangSO_New(args[0], retval);
   } else {
      joe_Object_assign (retval,
                         joe_Exception_New("loadSO: invalid argument"));
      return JOE_FAILURE;
   }
}

static int
toString (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   joe_Object_assign(retval, joe_String_New ("!"));
   return JOE_SUCCESS;
}
static joe_Method mthds[] = {
  {"if", _if},
  {"try", TRY},
  {"while", _while},
  {"foreach", foreach},
  {"for", _for},
  {"switch", _switch},
  {"isNull", isNull},
  {"println", println},
  {"print", print},
  {"readLine", readLine},
  {"newInstance", newInstance},
  {"throw", _throw},
  {"break", _break},
  {"breakLoop", breakLoop},
  {"goto", _goto},
  {"gosub", gosub},
  {"return", _return},
  {"array", array},
  {"new", _new},
  {"runJoe", runJoe},
  {"runAsBlock", runAsBlock},
  {"newArray", newArray},
  {"version", version},
  {"getOSType", getOSType},
  {"asc", asc},
  {"chr", chr},
  {"systemGetenv", systemGetenv},
  {"exec", exec},
  {"execFromDir", execFromDir},
  {"unixTime", unixTime},
  {"systemExit", systemExit},
  {"getGlob", getGlob},
  {"loadSO", loadSO},
  {"toString", toString},
  {"debug", debug},
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
