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

# include "joe_Glob.h"
# include "joe_String.h"
# include "joe_StringBuilder.h"
# include "joe_Boolean.h"
# include "joe_Array.h"
# include "joe_Exception.h"
# include "joestrct.h"

#define REGEX 0

static char *variables[] = { "regex", 0 };

static int
ctor (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if  (argc > 0 && joe_Object_instanceOf (argv[0], &joe_String_Class)) {
      if (argc == 1 || 
          (argc == 2 && joe_Object_instanceOf (argv[1], &joe_Boolean_Class))) {
         int ci = argc == 2 ? joe_Boolean_isTrue (argv[1]) : 0;
         joe_StringBuilder re = 0;
         char *glob = joe_String_getCharStar (argv[0]);
         joe_Object_assign (&re, joe_StringBuilder_New());

         if (ci) {
           joe_StringBuilder_appendCharStar (re, "(?i)");
         }
         for ( ;*glob; glob++) {
            switch (*glob) {
            case '*':
               joe_StringBuilder_appendChar (re, '.');
               joe_StringBuilder_appendChar (re, *glob);
               break;
            case '?':
               joe_StringBuilder_appendChar (re, '.');
               break;
            case '.':
               joe_StringBuilder_appendChar (re, '\\');
               joe_StringBuilder_appendChar (re, *glob);
               break;
            case '\\':
               joe_StringBuilder_appendChar (re, *glob);
               glob++;
               if (!*glob) {
                  glob--;
                  break;
               }
               /* ELSE PASSTHRU */
            default:
               joe_StringBuilder_appendChar (re, *glob);
               break;
            }
         }

         joe_Object_assign(JOE_AT (self, REGEX),
                           joe_StringBuilder_toString(re));

         joe_Object_assign (&re, 0);
         return JOE_SUCCESS;
      }
   }
   joe_Object_assign(retval,
        joe_Exception_New ("Glob ctor: invalid argument"));
   return JOE_FAILURE;
}

static int
matches (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1 && joe_Object_instanceOf (argv[0], &joe_String_Class)) {
      return joe_Object_invoke(argv[0],"matches",
                                      1, JOE_AT (self, REGEX), retval);
   } else {
      joe_Object_assign(retval,
                  joe_Exception_New ("Glob ctor: invalid argument"));
      return JOE_FAILURE;
   }
}


static joe_Method mthds[] = {
  {"matches", matches },
  {(void *) 0, (void *) 0}
};

joe_Class joe_Glob_Class = {
   "joe_Glob",
   ctor,
   0,
   mthds,
   variables,
   &joe_Object_Class,
   0
};
