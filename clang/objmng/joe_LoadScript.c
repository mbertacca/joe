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

# include "joe_LoadScript.h"
# include "joe_StringBuilder.h"
# include "joe_String.h"
# include "joe_Exception.h"
# include "joetoken.h"
# include "joearray.h"
# include "joeparser.h"
# include <errno.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>

# define isFileSeparator(c) (c=='/' || c=='\\')

static char *variables[] = { 0 };
static char *dirname = 0;

static int
ctor (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   switch (argc) {
   case 1:
      if (!joe_Object_instanceOf (argv[0], &joe_String_Class)) {
         joe_Object_assign(retval, joe_Exception_New ("LoadScript: invalid 1st argument"));
         return JOE_FAILURE;
      }
      break;
   default:
      joe_Object_assign(retval, joe_Exception_New ("LoadScript: invalid argument number"));
      return JOE_FAILURE;
   }

   joe_Object_assign(retval, joe_LoadScript_New (self, joe_String_getCharStar(argv[0])));

   if (joe_Object_instanceOf (*retval, &joe_Exception_Class))
      return JOE_FAILURE;
   else
      return JOE_SUCCESS;
}

static joe_Method mthds[] = {
  {(void *) 0, (void *) 0}
};

joe_Class joe_LoadScript_Class = {
   "joe_LoadScript",
   ctor,
   0,
   mthds,
   variables,
   &joe_Block_Class,
   0
};

# define MAXLINELEN 256

joe_Block
joe_LoadScript_New (joe_Block self, char *scriptName)
{
   joe_Object Return = self;
   char *path;
   FILE *scriptFile;
   JoeArray tokens = JoeArray_new (sizeof (struct t_Token), 256);
   Tokenizer tokenizer = Tokenizer_new (tokens);
   char *line = calloc (1, MAXLINELEN);
   JoeParser parser = JoeParser_new (scriptName);

   if (dirname == 0 || *dirname == 0) {
      path = scriptName;
   } else {
      path = calloc (1, strlen (dirname) + strlen (scriptName) + 1);
      strcat (path, dirname);
      strcat (path, scriptName);
   }

   scriptFile = fopen (path, "r");

   if (scriptFile == NULL) {
      joe_StringBuilder msg = joe_StringBuilder_New ();
      joe_StringBuilder_appendCharStar (msg, "cannot access ");
      joe_StringBuilder_appendCharStar (msg, scriptName);
      joe_StringBuilder_appendCharStar (msg, " (errno=");
      joe_StringBuilder_appendInt (msg, errno);
      joe_StringBuilder_appendChar (msg, ')');
      Return = joe_Exception_New_string (joe_StringBuilder_toString (msg));
      joe_Object_delIfUnassigned (&msg);
   } else {
      if (dirname == 0) {
         char *c;
         int fs = -1;
         dirname = strdup (scriptName);
         for (c = dirname; *c; c++) {
            if (isFileSeparator (*c))
               fs = (c - dirname);
         }
         if (fs >= 0)
            dirname[fs+1] = 0;
         else
            dirname[0] = 0;
      }
      while (fgets (line, MAXLINELEN, scriptFile) != NULL) {
         Tokenizer_tokenize (tokenizer, line);
      }
      fclose (scriptFile);
      joe_Block_Init (self, 0);
      if (JoeParser_compile (parser, self, tokens) != JOE_SUCCESS)
         Return = JoeParser_getException (parser);
   }
   JoeParser_delete (parser);
   JoeArray_delete (tokens);
   Tokenizer_delete (tokenizer);

   if (path != scriptName)
      free (path);
   free (line);
   return Return;
}
