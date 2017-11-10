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

# include <stdlib.h>
# include <string.h>
# include "joestrbuild.h"

# define DEFMAXLENGTH 16

struct t_JoeStrBuild {
   int length;
   int maxLength;
   char *memory;
};

JoeStrBuild
JoeStrBuild_new ()
{
   JoeStrBuild self = calloc (1, sizeof (struct t_JoeStrBuild));
   self->length = 0;
   self->maxLength = DEFMAXLENGTH - 1;
   self->memory = calloc (DEFMAXLENGTH, sizeof(char));

   return self;
}

void
JoeStrBuild_delete (JoeStrBuild self)
{
   free (self->memory);
   free (self);
}

void
JoeStrBuild_clean (JoeStrBuild self)
{
   if (self->length > 0) {
      memset (self->memory, 0, self->length);
      self->length = 0;
   }
}

static void
JoeStrBuild_realloc (JoeStrBuild self, int size)
{
   int newMaxLen = self->maxLength + size;
   self->memory = realloc (self->memory, newMaxLen + 1);
   memset (self->memory + self->maxLength, 0, size + 1);
   self->maxLength = newMaxLen;
}


void
JoeStrBuild_appendChr (JoeStrBuild self, char chr)
{
   if (self->maxLength <= self->length + 1)
      JoeStrBuild_realloc (self, DEFMAXLENGTH - 1);
   self->memory[self->length] = chr;
   self->length++;
}

void
JoeStrBuild_appendStr (JoeStrBuild self, char *str)
{
   int len = strlen (str);
   if (self->maxLength <= self->length + len)
      JoeStrBuild_realloc (self, len);
   memcpy(&self->memory[self->length], str, len);
   self->length += len;
}

void
JoeStrBuild_appendInt (JoeStrBuild self, int num)
{
   char *str = calloc (1, sizeof (num) << 2);
   int i;

   if (num < 0) {
      JoeStrBuild_appendChr (self, '-');
      num = -num;
   }
   for (i = 0; num; i++) {
      str[i] = '0' + (num % 10);
      num /= 10;
   }
   for (--i; i >= 0; i--)
      JoeStrBuild_appendChr (self, str[i]);

   free (str);
}

int
JoeStrBuild_length (JoeStrBuild self)
{
   return self->length;
}

char *
JoeStrBuild_toString (JoeStrBuild self)
{
   return strdup (self->memory);
}

/* test
int
main (int argc, char **argv)
{
   char *str;
   JoeStrBuild sb = JoeStrBuild_new ();

   JoeStrBuild_appendChr (sb, 'a');
   JoeStrBuild_appendChr (sb, 'b');
   JoeStrBuild_appendChr (sb, 'c');
   JoeStrBuild_appendStr (sb, "defghi");
   str = JoeStrBuild_toString (sb);
   printf ("[%s]\n", str);
   JoeStrBuild_clean (sb);
   free (str);

   JoeStrBuild_appendStr (sb, "ABCDEF");
   JoeStrBuild_appendChr (sb, 'g');
   JoeStrBuild_appendChr (sb, 'h');
   JoeStrBuild_appendChr (sb, 'i');
   str = JoeStrBuild_toString (sb);
   printf ("[%s]\n", str);
   JoeStrBuild_delete (sb);
   free (str);

   return 0;
}
*/
