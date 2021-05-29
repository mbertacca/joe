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
# include "joetoken.h"
# include "joeparser.h"
# include "joe_Array.h"
# include "joe_LoadScript.h"
# include "joe_String.h"
# include "joe_Integer.h"
# include "joe_Exception.h"

# define ERROR_CMDLINE 2
# define ERROR_GENERIC 3
# define MAXLINELEN 256
# define isFileSeparator(c) (c=='/')

static int
usage (char *prgname)
{
   fprintf (stderr, "usage: %s <script-name>\n", prgname);
   return ERROR_CMDLINE;
}

static void
showError (joe_Exception excpt)
{
   joe_String msg = 0;
   joe_Exception_toString (excpt, 0, 0, &msg);

   fprintf (stderr, "%s\n", joe_String_getCharStar(msg));
   joe_Object_delIfUnassigned (&msg);
}

int
main (int argc, char *argv[])
{
   int rc = 0;
   if (argc > 1) {
      int i;
      int lObjs;
      joe_Array argArray = 0;
      joe_LoadScript block = 0;
      joe_Object retval = 0;
      joe_String scriptName = 0;
      joe_Class* loadScriptClass = joe_Class_getClass("joe_LoadScript");
      joe_Class* integerClass = joe_Class_getClass("joe_Integer");

      joe_Object_assign (&scriptName, joe_String_New (argv[1]));
      rc=joe_Class_newInstance(loadScriptClass, 1, &scriptName, &retval);
      if (rc != JOE_SUCCESS) {
         showError (retval);
         return ERROR_GENERIC;
      } else {
         joe_Object_assign(&block, retval);
      }

      joe_Object_assign(&argArray, joe_Array_New (argc - 1));
      for (i = 1; i < argc; i++) {
         joe_Object_assign (joe_Object_at (argArray, i - 1),
                            joe_String_New(argv[i]));
      }
      if ((rc = joe_Block_exec (block, 1, &argArray, &retval)) != JOE_SUCCESS){
         showError (retval);
      } else {
         if (joe_Object_instanceOf (retval, integerClass))
            rc = joe_Integer_value (retval);
      }
      joe_Object_assign(&scriptName, 0);
      joe_Object_delIfUnassigned (&retval);
      joe_Object_assign (&block, 0);
      joe_Object_assign (&argArray, 0);
      lObjs =joe_Object_getLiveObjectsCount ();
      if (lObjs) {
         printf ("debug: unallocated objects=%d\n", lObjs);
         joe_Object_showLiveObjects ();
      }
   } else {
      return usage (argv[0]);
   }
}
