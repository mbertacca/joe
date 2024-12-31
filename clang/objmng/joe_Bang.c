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
# include <errno.h>
# include <string.h>
# include <stdlib.h>
# include <unistd.h>
# include "joe_Bang.h"
# include "joe_Array.h"
# include "joe_ArrayList.h"
# include "joe_Block.h"
# include "joe_Boolean.h"
# include "joe_Execute.h"
# include "joe_Integer.h"
# include "joe_Float.h"
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

# ifdef WIN32
# include <windows.h>

static int
run (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   DWORD rc;
   int i;
   STARTUPINFO si;
   PROCESS_INFORMATION pi;
   joe_StringBuilder msg = 0;
   joe_Object_assign(&msg, joe_StringBuilder_New());

   ZeroMemory( &si, sizeof(si) );
   si.cb = sizeof(si);
   ZeroMemory( &pi, sizeof(pi) );

   for (i = 0; i < argc; i++) {
      joe_StringBuilder_append(msg, argv[i]);
      joe_StringBuilder_appendChar(msg, ' ');
   }
 
   if (!CreateProcess( NULL,   /* No module name (use command line) */
        joe_StringBuilder_getCharStar(msg), /* Command line */
        NULL,           /* Process handle not inheritable */
        NULL,           /* Thread handle not inheritable */
        FALSE,          /* Set handle inheritance to FALSE */
        0,              /* No creation flags */
        NULL,           /* Use parent's environment block */
        NULL,           /* Use parent's starting directory  */
        &si,            /* Pointer to STARTUPINFO structure */
        &pi )           /* Pointer to PROCESS_INFORMATION structure */
   ) {
      joe_Object_assign(&msg, 0);
      joe_Object_assign(retval,
                        joe_Exception_New ("exec: cannot create process"));
      return JOE_FAILURE;
   }
   joe_Object_assign(&msg, 0);

   /* Wait until child process exits. */
   WaitForSingleObject( pi.hProcess, INFINITE );
   GetExitCodeProcess ( pi.hProcess, &rc);

   /* Close process and thread handles.  */
   CloseHandle( pi.hProcess );
   CloseHandle( pi.hThread );
   joe_Object_assign(retval, joe_Integer_New (rc));
   return JOE_SUCCESS;
}
#else
#include <sys/wait.h>
static int
run (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   int i;
   int rc;
   int pid;
   char **args = calloc (argc + 1, sizeof(char*));
   for (i = 0; i < argc; i++)
      args[i] = joe_String_getCharStar(argv[i]);
   args[i] = NULL;

   switch (pid = fork()) {
   case -1: /* fork() failed */
      joe_Object_assign(retval, joe_Exception_New ("exec: fork failed"));
      free (args);
      return JOE_FAILURE;
   case 0: /* Child: exec command */
      execvp(args[0], args);
      exit(127); /* We could not exec the shell */
   default: /* Parent: wait for our child to terminate */
      waitpid(pid, &rc, 0);
      if (rc != 0 && (rc & 0x00FF) == 0)
         rc >>= 8;
      joe_Object_assign (retval, joe_Integer_New (rc));
      break;
   }

   free (args);
   return JOE_SUCCESS;
}
#endif

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

static char *
execgetout (char *cmd)
{
  char buffer[160];
  char *Return = calloc (1, 1);
  int totSize = 0;
  int size;
  int rc = -1;

  FILE *fout = popen (cmd, "r");
  if (fout != NULL) {
     while (fgets (buffer, sizeof(buffer), fout) != NULL) {
        size = strlen (buffer);
        Return = realloc (Return, totSize + size + 1);
        memcpy (&Return[totSize], buffer, size);
        totSize += size;
     }
     Return[totSize] = 0;
     rc = pclose (fout);
     if (rc != 0 && (rc & 0x00FF) == 0)
        rc >>= 8;
  }
  if (rc && totSize == 0) {
     snprintf (buffer, sizeof(buffer), "Error trying to execute %s (rc=%d)",
               cmd, rc);
     Return = realloc (Return, strlen(buffer) + 1);
     strcpy (Return, buffer);
  }
  return Return;
}


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
                  rc = joe_Block_exec (argv[0], 0, 0, &or);
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
                     rc = joe_Block_exec (argv[1], 0, 0, &Return);
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
        rc = joe_Block_exec (argv[0], 0, 0, &Return);
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

static int *qsort_rc;
static joe_Block qsort_blk;
static joe_Object *qsort_retval;

static int
qCmp(const void *p1, const void *p2)
{
   joe_Object vArg[2];
   if (*qsort_rc != JOE_SUCCESS)
      return 0;

   vArg[0] = *((joe_Object *) p1);
   vArg[1] = *((joe_Object *) p2);

   if ((joe_Block_exec (qsort_blk,2,vArg,qsort_retval))!=JOE_SUCCESS) {
       *qsort_rc = JOE_FAILURE;
   } else if (joe_Object_instanceOf (*qsort_retval, &joe_Integer_Class)) {
       return joe_Integer_value (*qsort_retval);
   } else {
       joe_Object_assign(qsort_retval, joe_Exception_New (
                 "arraySort: invalid block return value, must be an integer"));
       *qsort_rc = JOE_FAILURE;
   }
   return 0;
}

static int
mysort  (joe_Array v, joe_Block blk, int n, joe_Object *retval)
{
   int Return = JOE_SUCCESS;
   qsort_rc = &Return;
   qsort_blk = blk;
   qsort_retval = retval;

   qsort(JOE_AT(v, 0), n, sizeof(char *), qCmp);

   return Return;
}

#ifdef COMMENT

static int
shellsort (joe_Array v, joe_Block blk, int n, joe_Object *retval)
{
   int gap, i, j;
   joe_Object vArg[2];

   gap = 1; 
   while (gap < n) {
     gap = gap * 3 + 1;
   }

   for (gap = gap/3 ; gap > 0; gap /= 3)
      for (i = gap; i < n; i++)
         for (j=i-gap; j>=0 /* && v[j]>v[j+gap] */; j-=gap) {
            vArg[0] = *JOE_AT(v, j);
            vArg[1] = *JOE_AT(v, j+gap);
            if ((joe_Block_exec (blk,2,vArg,retval))!=JOE_SUCCESS) {
                return JOE_FAILURE;
            } else if (joe_Object_instanceOf (*retval, &joe_Integer_Class)) {
               if (joe_Integer_value (*retval) <= 0) {
                  break;
               } else {
                  *JOE_AT(v, j) = vArg[1];
                  *JOE_AT(v, j+gap) = vArg[0];
               }
            } else {
               joe_Object_assign(retval, joe_Exception_New (
                 "arraySort: invalid block return value, must be an integer"));
               return JOE_FAILURE;
            }
         }
   joe_Object_assign (retval, joe_Null_value);
   return JOE_SUCCESS;
}	

#endif

# define PATH 0
static char *variables[] = { "path", 0 };

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
   int i;
   for (i = 0; i < argc; i++)
      if (! joe_Object_instanceOf(argv[i], &joe_String_Class))
         break;
   if (argc > 0 && i == argc) {
      return run (self, argc, argv, retval);
   } else {
      joe_Object_assign(retval, joe_Exception_New ("exec: invalid argument(s)"));
      return JOE_FAILURE;
   }
}

static int
system_ (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
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
execGetOut (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc > 0) {
      int i;
      char *output;
      joe_StringBuilder msg = 0;
      joe_Object_assign(&msg, joe_StringBuilder_New());

      for (i = 0; i < argc; i++) {
         joe_StringBuilder_append(msg, argv[i]);
         joe_StringBuilder_appendChar(msg, ' ');
      }
      output = execgetout (joe_StringBuilder_getCharStar(msg));
      joe_Object_assign(retval, joe_String_New (output));
      free (output);
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
         if (chdir (currpath) != 0) {
            joe_Object_assign(retval, joe_Exception_New (
                       "execFromDir: cannot change directory back"));
            rc = JOE_FAILURE;
         }
      } else {
         joe_Object_assign(retval, joe_Exception_New (
                       "execFromDir: cannot change directory"));
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
   joe_StringBuilder_appendCharStar (msg, "JOE (native) Revision 1.49 ");
   joe_StringBuilder_appendCharStar (msg, __DATE__);
#ifdef WIN32
   joe_StringBuilder_appendCharStar (msg, " Windows");
#else
   joe_StringBuilder_appendCharStar (msg, " Unixish");
#endif
   joe_Object_assign(retval, joe_StringBuilder_toString (msg));
   joe_Object_assign (&msg, 0);
   return JOE_SUCCESS;
}

static int
nl (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
#ifdef WIN32
   joe_Object_assign (retval,joe_String_New ("\r\n"));
#else
   joe_Object_assign (retval,joe_String_New ("\n"));
#endif
   return JOE_SUCCESS;
}

static int
getOSType (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
#ifdef WIN32
  joe_Object_assign (retval,joe_String_New ("Windows"));
#elif __unix__
   joe_Object_assign (retval,joe_String_New ("Unixish"));
#elif __MACH__
   joe_Object_assign (retval,joe_String_New ("Mach"));
#else
   joe_Object_assign (retval,joe_String_New ("Unknown"));
# endif
   return JOE_SUCCESS;
}

static int
systemGetenv (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1 && joe_Object_instanceOf (argv[0], &joe_String_Class)) {
      char *var = joe_String_getCharStar (argv[0]);
      char *val = getenv(var);
      if (val) {
         joe_Object_assign (retval, joe_String_New (val));
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
getErrno (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 0) {
      joe_Object_assign(retval, joe_Integer_New (errno));
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval, joe_Exception_New ("getErrno: invalid argument"));
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

static int
eprint (joe_Object self, int argc, joe_Object *args, joe_Object *retval)
{
   joe_String msg = 0;
   args2String(argc, args, &msg);
   fputs (joe_String_getCharStar (msg), stderr);
   fflush (stderr);

   joe_Object_assign(&msg, 0);
   joe_Object_assign(retval, self);
   return JOE_SUCCESS;
}

static int
eprintln (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   int rc;
   if ((rc = eprint (self, argc, argv, retval)) == JOE_SUCCESS) {
      fputs ("\n", stderr);
      fflush (stderr);
   }
   return rc;
}

static char* inbuff = 0;
static ssize_t inbufflen = 0;

static int
readLine (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   int i = joe_Files_getline (&inbuff, &inbufflen, stdin);

   if (i < 0) {
      joe_Object_assign(retval, joe_Null_value);
   } else {
      for (i--; i >= 0 && inbuff[i] < ' '; i--)
         inbuff[i] = 0;
      joe_Object_assign(retval, joe_String_New (inbuff));
   }
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
            if ((rc=joe_Block_exec (argv[0],0,0,&bol))!=JOE_SUCCESS) {
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
               if ((rc=joe_Block_exec(argv[1],0,0,&lretval))
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
   int istep = 1;

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
   } else {
      i = end - start + 1;
      istep = 0;
   }
   joe_Object_assign(&lretval, joe_Null_value);
   for ( ; i > 0; i -= istep) {
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
         joe_Object_assign (&arg, joe_Integer_add(arg, step));
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
      if (argv[0])
         return joe_Object_invoke(argv[0],"foreach", argc-1, &argv[1], retval);
      else
         return JOE_SUCCESS;
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
         if ((rc = joe_Block_exec (argv[0],0,0,&bol))!=JOE_SUCCESS) {
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
            if ((rc = joe_Block_exec(argv[1],0,0,retval))!=JOE_SUCCESS){
               return JOE_FAILURE;
            }
          } else {
            joe_Object_assign(retval, joe_Exception_New ("if: invalid 2nd argument"));
            return JOE_FAILURE;
          }
      } else if (argc == 3) {
         if (joe_Object_instanceOf (argv[2], &joe_Block_Class)) {
            if ((rc = joe_Block_exec(argv[2],0,0,retval))!=JOE_SUCCESS){
               return JOE_FAILURE;
            }
          } else {
            joe_Object_assign(retval, joe_Exception_New ("if: invalid 3rd argument"));
             return JOE_FAILURE;
          }
      } else {
          joe_Object_assign(retval, joe_Boolean_New_false());
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
         if ((rc = joe_Block_exec(argv[0], 0, 0, retval)) != JOE_SUCCESS) {
            if (argc == 2) {
               joe_Object largs[1] = { 0 };
               joe_Object_assign(&largs[0], *retval);
               joe_Object_assign(retval, 0);
               rc = joe_Block_exec(argv[1], 1, largs, retval);
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
loadScript (joe_Bang self, joe_String scriptName, joe_Object *retval)
{
   joe_Object args[3];
   joe_Class* loadScriptClass = joe_Class_getClass("joe_LoadScript");
   args[0] = scriptName;
   args[1] = *JOE_AT(self,PATH);
   args[2] = self;
   return joe_Class_newInstance(loadScriptClass, 3, args, retval);
}

static int
_new (joe_Object self, int argc, joe_Object *args, joe_Object *retval)
{
   if (argc > 0 && joe_Object_instanceOf (args[0], &joe_String_Class)) {
      int rc;
      rc =loadScript (self, args[0], retval);
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
typename (joe_Object self, int argc, joe_Object *args, joe_Object *retval)
{
   if (argc == 1) {
      if (joe_Object_instanceOf (args[0], &joe_JOEObject_Class))
         joe_Object_assign(retval,joe_Block_getName (args[0]));
      else
         joe_Object_assign(retval,
                        joe_String_New (joe_Object_getClassName (args[0])));
      return JOE_SUCCESS;
   }
   joe_Object_assign(retval,
                        joe_Exception_New("typename: Invalid argument"));
   return JOE_FAILURE;
}


static int
joe (joe_Object self, int argc, joe_Object *args, joe_Object *retval)
{
   joe_String scriptName;
   joe_Array argArray = 0;
   int argLen;
   int rc;

   if (argc > 0 && joe_Object_instanceOf (args[0], &joe_String_Class)) {
      int i;
      scriptName = args[0];
      joe_Object_assign(&argArray, joe_Array_New (argc));
      for (i = 0; i < argc; i++) {
         joe_Object_assign (joe_Object_at (argArray, i), args[i]);
      }
   } else if (argc == 1 && joe_Object_instanceOf (args[0], &joe_Array_Class) &&
       (argLen = joe_Array_length(args[0])) > 0 && 
       joe_Object_instanceOf (*JOE_AT (args[0], 0), &joe_String_Class)) {
      scriptName = *JOE_AT (args[0], 0);
      argArray = args[0];
   } else {
      joe_Object_assign(retval, joe_Exception_New("joe: Invalid argument(s)"));
      return JOE_FAILURE;
   }

   rc =loadScript (self, scriptName, retval);
   if (rc == JOE_SUCCESS) {
      joe_LoadScript block = 0;
      joe_Object_transfer(&block, retval);
      rc = joe_Block_outer_exec (block, 1, &argArray, retval);
      joe_Object_assign(&block, 0);
   }
   if (argArray != args[0])
      joe_Object_assign(&argArray, 0);
   return rc;
}

static int
runJoe (joe_Object self, int argc, joe_Object *args, joe_Object *retval)
{
   if (argc > 0 && joe_Object_instanceOf (args[0], &joe_String_Class)) {
      return joe (self, argc, args, retval);
   } else {
      joe_Object_assign(retval, joe_Exception_New("runJoe: Invalid argument(s)"));
   }
   return JOE_FAILURE;
}

static int
runAsBlock (joe_Object self, int argc, joe_Object *args, joe_Object *retval)
{
   if (argc > 1 && joe_Object_instanceOf (args[0], &joe_Block_Class)
                && joe_Object_instanceOf (args[1], &joe_String_Class)) {
      char *scriptName = joe_String_getCharStar(args[1]);
      FILE *scriptFile = joe_LoadScript_getFile (scriptName,
                                                 *JOE_AT(self,PATH), self);
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
      int i;
      int dim = joe_Integer_value (args[0]);
      joe_Object_assign(retval, joe_Array_New (dim));
      for (i = 0; i < dim; i++)
         joe_Object_assign (joe_Object_at (*retval, i), joe_Null_value);
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval, joe_Exception_New ("newArray: invalid argument"));
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
arraySort (joe_Object self, int argc, joe_Object *args, joe_Object *retval)
{
   if (argc == 2 && joe_Object_instanceOf (args[0], &joe_Array_Class)
                 && joe_Object_instanceOf (args[1], &joe_Block_Class)) {
      return mysort (args[0],args[1],joe_Array_length(args[0]),retval);
   } else if (argc == 2 && joe_Object_instanceOf (args[0], &joe_ArrayList_Class)
                       && joe_Object_instanceOf (args[1], &joe_Block_Class)) {
      joe_Array v = joe_ArrayList_getArray(args[0]);
      int n = joe_ArrayList_length (args[0]);
      return mysort (v, args[1], n, retval);
   } else {
      joe_Object_assign(retval,
                        joe_Exception_New ("arraySort: invalid argument(s)"));
      return JOE_FAILURE;
   }
   return JOE_SUCCESS;
}

static int
bsearch_ (joe_Array v, int n, joe_Object cfrt,
          joe_Block blk, joe_Integer *retval)
{
   int i = n / 2;
   int min = 0;
   int max = n - 1;
   joe_Object arg[2];

   arg[1] = cfrt;
   for ( ; ; ) {
      if (max < min)
         break;
      i = ((max - min) / 2) + min;
      arg[0] = *JOE_AT (v,i);
      if ((joe_Block_exec (blk,2,arg,retval)) != JOE_SUCCESS) {
         return JOE_FAILURE;
      } else if (joe_Object_instanceOf (*retval, &joe_Integer_Class)) {
         int cmp = joe_Integer_value (*retval);
         if ( cmp < 0) {
            min = i + 1;
         } else if (cmp > 0) {
            max = i - 1;
         } else {
            joe_Object_assign (retval, joe_Integer_New (i));
            return JOE_SUCCESS;
         }
      } else {
        joe_Object_assign(retval, joe_Exception_New (
            "binarySearch: invalid block return value, must be an integer"));
      }
   }
   joe_Object_assign (retval, joe_Integer_New (-1));
   return JOE_SUCCESS;
}

static int
binarySearch (joe_Object self, int argc, joe_Object *args, joe_Object *retval)
{
   if (argc == 3 && joe_Object_instanceOf (args[0], &joe_Array_Class)
                 && joe_Object_instanceOf (args[2], &joe_Block_Class)) {
      return bsearch_(args[0], joe_Array_length(args[0]), args[1],
                       args[2], retval);
   } else if (argc == 3 && joe_Object_instanceOf (args[0], &joe_ArrayList_Class)
                        && joe_Object_instanceOf (args[2], &joe_Block_Class)) {
      joe_Array v = joe_ArrayList_getArray(args[0]);
      int n = joe_ArrayList_length (args[0]);
      return bsearch_(v, n, args[1], args[2], retval);
   } else {
      joe_Object_assign(retval,
                        joe_Exception_New ("binarySearch: invalid argument(s)"));
      return JOE_FAILURE;
   }
   return JOE_SUCCESS;
}

int initRand;
 
static int
random_ (joe_Object self, int argc, joe_Object *args, joe_Object *retval)
{
   if (argc == 0) {
      double Return;
      if (initRand == 0) {
         srand (time(0));
         initRand = 1;
      }
      Return = (double) rand() / ((double)(RAND_MAX) + 1);
      joe_Object_assign(retval, joe_Float_New (Return));
   } else {
      joe_Object_assign(retval,
                        joe_Exception_New ("random: no arguments allowed"));
      return JOE_FAILURE;
   }
   return JOE_SUCCESS;
}

static int
getGlob (joe_Object self, int argc, joe_Object *args, joe_Object *retval)
{
   return joe_Class_newInstance (&joe_Glob_Class, argc, args, retval);
}

static int
addPath (joe_Object self, int argc, joe_Object *args, joe_Object *retval)
{
   if (argc == 1 && joe_Object_instanceOf (args[0], &joe_String_Class)) {
      joe_Array old = 0;
      joe_Array neu = 0;
      joe_Object_assign (&old, *JOE_AT(self,PATH));
      joe_Array_add(old, args[0], &neu);
      joe_Object_transfer (JOE_AT(self,PATH), &neu);
      joe_Object_assign (&old, 0);
      joe_Object_assign (retval, self);
   } else {
      joe_Object_assign (retval,
                         joe_Exception_New("addPath: invalid argument"));
      return JOE_FAILURE;
   }
   return JOE_SUCCESS;
}

static int
getPath (joe_Object self, int argc, joe_Object *args, joe_Object *retval)
{
   if (argc == 0) {
      joe_Object_assign (retval, joe_Object_clone (*JOE_AT(self,PATH)));
   } else {
      joe_Object_assign (retval,
                         joe_Exception_New("getPath: invalid argument"));
      return JOE_FAILURE;
   }
   return JOE_SUCCESS;
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
  {"addPath", addPath },
  {"getPath", getPath },
  {"if", _if},
  {"try", TRY},
  {"while", _while},
  {"foreach", foreach},
  {"for", _for},
  {"switch", _switch},
  {"isNull", isNull},
  {"println", println},
  {"print", print},
  {"eprintln", eprintln},
  {"eprint", eprint},
  {"readLine", readLine},
  {"newInstance", newInstance},
  {"throw", _throw},
  {"break", _break},
  {"breakLoop", breakLoop},
  {"goto", _goto},
  {"gosub", gosub},
  {"return", _return},
  {"array", array},
  {"arraySort", arraySort},
  {"binarySearch", binarySearch},
  {"new", _new},
  {"typename", typename },
  {"joe", joe},
  {"runJoe", runJoe},
  {"runAsBlock", runAsBlock},
  {"newArray", newArray},
  {"version", version},
  {"nl", nl},
  {"getOSType", getOSType},
  {"asc", asc},
  {"chr", chr},
  {"getErrno", getErrno},
  {"systemGetenv", systemGetenv},
  {"exec", exec},
  {"system", system_},
  {"execGetOut", execGetOut},
  {"execFromDir", execFromDir},
  {"unixTime", unixTime},
  {"systemExit", systemExit},
  {"getGlob", getGlob},
  {"loadSO", loadSO},
  {"toString", toString},
  {"random", random_},
  {"debug", debug},
  {(void *) 0, (void *) 0}
};

joe_Class joe_Bang_Class = {
   "joe_Bang",
   0,
   0,
   mthds,
   variables,
   &joe_Object_Class,
   0
};

joe_Object
joe_Bang_New () {
   joe_Object self = joe_Object_New (&joe_Bang_Class, 0);
   joe_Object_assign (JOE_AT(self,PATH), joe_Array_New (0));
   return self;
}
