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
# include "joe_Files.h"
# include "joe_Exception.h"
# include "joetoken.h"
# include "joearray.h"
# include "joeparser.h"
# include "joestrct.h"
# include <errno.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <limits.h>

# ifdef WIN32
# define PATH_SEPARATOR '\\'
# define isFileSeparator(c) (c=='/'||c=='\\')
# else
# define PATH_SEPARATOR '/'
# define isFileSeparator(c) (c=='/')
# endif

static char *variables[] = { 0 };
static char *dirname = 0;

static int
ctor (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   joe_Array path = 0;
   joe_Block bang = 0;
   switch (argc) {
   case 1:
      if (!joe_Object_instanceOf (argv[0], &joe_String_Class)) {
         joe_Object_assign(retval, 
                     joe_Exception_New ("LoadScript: invalid 1st argument"));
         return JOE_FAILURE;
      }
      break;
   case 3:
      if (joe_Object_instanceOf (argv[0], &joe_String_Class) &&
          joe_Object_instanceOf (argv[1], &joe_Array_Class)  &&
          joe_Object_instanceOf (argv[2], &joe_Bang_Class) ) {
         path = argv[1];
         bang = argv[2];
      } else {
         joe_Object_assign(retval,
                     joe_Exception_New ("LoadScript: invalid arguments"));
         return JOE_FAILURE;
      }
      break;
   default:
      joe_Object_assign(retval, joe_Exception_New ("LoadScript: invalid argument number"));
      return JOE_FAILURE;
   }

   joe_Object_assign(retval, joe_LoadScript_New (self, 
                      joe_String_getCharStar(argv[0]), path, bang));

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

void
joe_LoadScript_setCWD ()
{
   if (dirname == 0) {
      dirname = "./";
   }
}

char *
joe_LoadScript_getCWD ()
{
   return dirname;
}

FILE *
joe_LoadScript_getFile (char *scriptName, joe_Array path, joe_Bang bang)
{
   FILE *Return;

   if (dirname == 0) {
      Return = fopen (scriptName, "r");
      if (Return != NULL) {
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
   } else {
      char fullpath[PATH_MAX];
      int npath = path != NULL ? joe_Array_length(path) : 0;
      int i;
      char *dir;
      strcpy (fullpath, dirname);
      strcat (fullpath, scriptName);
      Return = fopen (fullpath, "r");
      for ( i = 0; Return == NULL && i < npath; i++) {
         dir = joe_String_getCharStar(*JOE_AT (path, i));
         if (joe_Files_isAbsolute(dir)) {
            snprintf (fullpath,sizeof(fullpath), "%s%c%s",
                                             dir,PATH_SEPARATOR,scriptName);
         } else {
            snprintf (fullpath,sizeof(fullpath),"%s%s%c%s",
                                     dirname,dir,PATH_SEPARATOR,scriptName);
         }
         Return = fopen (fullpath, "r");
      }
   }
   return Return;
}

joe_Block
joe_LoadScript_New (joe_Block self, char *scriptName, joe_Array path, joe_Bang bang)
{
   int i;
   joe_Object Return = self;
   FILE *scriptFile;
   char *line = 0;
   ssize_t lineLen = 0;
   JoeArray tokens = JoeArray_new (sizeof (struct t_Token), 256);
   Tokenizer tokenizer = Tokenizer_new (tokens);
   JoeParser parser = JoeParser_new (scriptName);

   scriptFile = joe_LoadScript_getFile (scriptName, path, bang);

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
      while (joe_Files_getline (&line, &lineLen, scriptFile) >= 0) {
         Tokenizer_tokenize (tokenizer, line);
      }
      fclose (scriptFile);
      joe_Block_Init (self, 0, bang);
      if (JoeParser_compile (parser, self, tokens) != JOE_SUCCESS)
         Return = JoeParser_getException (parser);
   }
   JoeParser_delete (parser);
   for (i = JoeArray_length(tokens) - 1; i >= 0; i--)
      free (((struct t_Token*) JoeArray_get(tokens, i))->word);
   JoeArray_delete (tokens);
   Tokenizer_delete (tokenizer);

   if (line) {
      free (line);
      line = 0;
      lineLen = 0;
   }
   return Return;
}
