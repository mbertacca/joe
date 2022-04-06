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
# include <unistd.h>
# include <string.h>
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

   fflush (stdout);
   fprintf (stderr, "%s\n", joe_String_getCharStar(msg));
   joe_Object_assign (&msg, 0);
   fflush (stderr);
}

static void
showValue (joe_Object value)
{
   fflush (stdout);
   if (value) {
      joe_String str = 0;
      joe_Object_invoke (value, "toString", 0, 0, &str);

      fprintf (stderr, "---> %s\n", joe_String_getCharStar(str));
      joe_Object_assign (&str, 0);
   }
   fflush (stderr);
}

void
showPrompt (char *prompt)
{
   fputs (prompt, stdout);
}

void
showNoPrompt (char *prompt)
{
}

static int
countBraces (char *line, int braces)
{
   int inquote = 0;
   int i;
   int line_length = strlen (line);

   for (i = 0; i < line_length; i++) {
      switch (line[i]) {
      case '{':
         if (!inquote)
            braces++;
         break;
      case '}':
         if (!inquote)
            braces--;
         break;
      case '"':
         inquote = !inquote;
         break;
      default:
         break;
      }
   }
   return braces;
}

int
main (int argc, char *argv[])
{
   int rc = 0;
   int lObjs;
   joe_Block block = 0;
   joe_Object retval = 0;
   int i;

   if (argc > 1) {
      joe_Array argArray = 0;
      joe_String scriptName = 0;
      joe_Class* loadScriptClass = joe_Class_getClass("joe_LoadScript");
      joe_Class* integerClass = joe_Class_getClass("joe_Integer");

      joe_Object_assign (&scriptName, joe_String_New (argv[1]));
      rc=joe_Class_newInstance(loadScriptClass, 1, &scriptName, &retval);
      if (rc != JOE_SUCCESS) {
         showError (retval);
         usage(argv[0]);
         joe_Object_assign(&retval, 0);
         return ERROR_GENERIC;
      } else {
         joe_Object_assign(&block, retval);
      }
      joe_Object_assign(&retval, 0);

      joe_Object_assign(&argArray, joe_Array_New (argc - 1));
      for (i = 1; i < argc; i++) {
         joe_Object_assign (joe_Object_at (argArray, i - 1),
                            joe_String_New(argv[i]));
      }

      if ((rc = joe_Block_outer_exec (block, 1, &argArray, &retval)) != JOE_SUCCESS){
         showError (retval);
         rc = 66;
      } else {
         if (joe_Object_instanceOf (retval, integerClass))
            rc = joe_Integer_value (retval);
      }

      joe_Object_assign(&scriptName, 0);

      joe_Object_assign(&block, 0);
      joe_Object_assign (&argArray, 0);
      joe_Object_assign (&retval, 0);
   } else {
      static char line[MAXLINELEN];
      static char *ps1 = "joe> ";
      char ps2[6] = { ' ', '.', ' ', '>', ' ', '\0'};
      char *prompt = ps1;
      JoeParser parser = JoeParser_new ("<stdin>");
      JoeArray tokens = JoeArray_new (sizeof (struct t_Token), 8);
      Tokenizer tokenizer = Tokenizer_new (tokens);
      int braces = 0;
      void (*putPrompt)(char*) = isatty(0) ? showPrompt : showNoPrompt;

      joe_LoadScript_setCWD();
      joe_Object_assign (&block, joe_Block_New (0));
      strcpy (line,"!println (!version),\"; type 'exit' to exit the session\".");
      do {
         i = strlen (line);
         while (i > 0 && line[--i] <= ' ')
            line[i] = 0;
         if (!strcmp (line,"exit"))
            break;
         if (prompt == ps1) {
            JoeArray_clear (tokens);
            joe_Block_removeMessages(block);
         }
         Tokenizer_tokenize (tokenizer, line);
         i = JoeArray_length (tokens);
         braces = countBraces (line, braces);
         if (braces == 0 &&
             (!line[0] || ((Token) JoeArray_get(tokens, --i))->type == _DOT_)) {
            if (JoeParser_compile (parser, block, tokens) != JOE_SUCCESS) {
               joe_Object_assign (&retval, JoeParser_getException (parser));
               showError (retval);
               prompt = ps1;
            } else {
               if (joe_Block_outer_exec (block, 0, 0, &retval) != JOE_SUCCESS)
                  showError (retval);
               else
                  showValue (retval);
               prompt = ps1;
            }
          } else {
            if (braces > 0) {
               ps2[0] = (braces / 100) + '0';
               ps2[1] = ((braces % 100) / 10) + '0';
               ps2[2] = (braces % 10) + '0';
            } else {
               ps2[0] = ' ';
               ps2[1] = '.';
               ps2[2] = ' ';
            }
            prompt = ps2;
         }
         putPrompt (prompt);
      } while (fgets (line, MAXLINELEN, stdin) != NULL);
      joe_Object_assign(&retval, 0);
      joe_Object_assign(&block, 0);
      JoeParser_delete (parser);
   }
   joe_Object_gc ();
   lObjs =joe_Object_getLiveObjectsCount ();
   if (lObjs) {
      printf ("debug: unallocated objects=%d\n", lObjs);
      joe_Object_showLiveObjects ();
   }
   return rc;
}
