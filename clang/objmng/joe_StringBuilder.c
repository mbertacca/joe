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

# include <string.h>
# include <stdlib.h>
# include <ctype.h>
# include "joe_StringBuilder.h"
# include "joe_Boolean.h"
# include "joe_Integer.h"
# include "joe_Exception.h"
# include "joe_Memory.h"
# include "joe_String.h"
# include "joestrct.h"

# define DEFMAXLENGTH 16

typedef struct s_StrBuild {
   unsigned int length;
   unsigned int maxLength;
} StrBuild;

static char *variables[] = { "data", 0 };

static int
init (joe_Object self)
{
   joe_Memory mem = joe_Memory_New (sizeof (StrBuild) + DEFMAXLENGTH);
   StrBuild *sb = (void *) joe_Object_getMem(mem);

   sb->length = 0;
   sb->maxLength = DEFMAXLENGTH;

   joe_Object_assign (JOE_AT(self, 0), mem);
   return JOE_SUCCESS;
}

static int
ctor (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   init (self);
   return JOE_SUCCESS;
}

static int
add (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1) {
      joe_StringBuilder_append (self, argv[0]);
      joe_Object_assign(retval, self);
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval,
                        joe_Exception_New ("StringBuilder add: invalid argument"));
      return JOE_FAILURE;
   }
}

static int
length (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   joe_Object_assign(retval, joe_Integer_New (joe_StringBuilder_length(self)));
   return JOE_SUCCESS;
}

static int
toString (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   joe_Object_assign(retval, joe_StringBuilder_toString (self));
   return JOE_SUCCESS;
}

static joe_Method mthds[] = {
  {"add", add},
  {"length", length},
  {"toString", toString},
  {(void *) 0, (void *) 0}
};

joe_Class joe_StringBuilder_Class = {
   "joe_StringBuilder",
   ctor,
   0,
   mthds,
   variables,
   &joe_Object_Class,
   0
};


joe_Object
joe_StringBuilder_New ()
{
   joe_Object self;

   self = joe_Object_New (&joe_StringBuilder_Class, 0);
   init (self);

   return self;
}

unsigned int
joe_StringBuilder_length (joe_StringBuilder self)
{
   joe_Memory mem = *JOE_AT(self, 0);
   StrBuild *sb = (void *) joe_Object_getMem (mem) ;
   return sb->length;
}

static void
appendCharStar (joe_StringBuilder self, char *s, int len)
{
   joe_Memory mem = *JOE_AT(self, 0);
   StrBuild *sb = (void *) joe_Object_getMem (mem) ;
   char *str = ((char*) sb) + sizeof (StrBuild);
   unsigned int newLen = sb->length + len;
   if (sb->maxLength <= newLen) {
      char *newstr;
      StrBuild *newsb;
      int newMaxLen = newLen + DEFMAXLENGTH - (newLen % DEFMAXLENGTH);
      joe_Memory newmem = joe_Memory_New (newMaxLen + sizeof (StrBuild));
      newsb = (void *) joe_Object_getMem (newmem);
      newstr = ((char*) newsb) + sizeof (StrBuild);
      memcpy(newstr, str, sb->length);
      newsb->length = sb->length;
      newsb->maxLength = newMaxLen;
      sb = newsb;
      str = newstr;
      joe_Object_assign (JOE_AT(self, 0), newmem);
   }
   memcpy(&str[sb->length], s, len);
   sb->length += len;
}

void
joe_StringBuilder_appendCharStar (joe_StringBuilder self, char *s)
{
   appendCharStar (self, s, strlen (s));
}

void
joe_StringBuilder_appendChar (joe_StringBuilder self, char c)
{
   appendCharStar (self, &c, 1);
}

void
joe_StringBuilder_appendInt (joe_StringBuilder self, int num)
{
   char *str = calloc (1, sizeof (num) << 2);
   int i;

   if (num < 0) {
      joe_StringBuilder_appendChar (self, '-');
      num = -num;
   }
   for (i = 0; num; i++) {
      str[i] = '0' + (num % 10);
      num /= 10;
   }
   for (--i; i >= 0; i--)
      joe_StringBuilder_appendChar (self, str[i]);

   free (str);
}

void
joe_StringBuilder_append (joe_StringBuilder self, joe_Object obj)
{
   if (joe_Object_instanceOf(obj, &joe_String_Class)) {
      joe_StringBuilder_appendCharStar(self, joe_String_getCharStar(obj));
   } else {
      if (obj) {
         joe_String str = 0;
         joe_Object_invoke (obj, "toString", 0, 0, &str);
         joe_StringBuilder_appendCharStar(self, joe_String_getCharStar(str));
         joe_Object_assign(&str, 0);
      } else {
         joe_StringBuilder_appendCharStar(self, "(null)");
      }
   }
}

joe_Object
joe_StringBuilder_toString (joe_Object self)
{
   joe_Memory mem = *JOE_AT(self, 0);
   char *str = ((char*) joe_Object_getMem (mem) + sizeof (StrBuild));
   return joe_String_New (str);
}
