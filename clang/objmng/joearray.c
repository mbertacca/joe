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
# include "joearray.h"

# define DEFMAXLENGTH 16

struct t_JoeArray {
   int itemSize;
   int length;
   int maxLength;
   void *memory;
};

JoeArray
JoeArray_new (int itemSize, int maxLength)
{
   JoeArray self = calloc (1, sizeof (struct t_JoeArray));
   self->itemSize = itemSize;
   self->length = 0;
   self->maxLength = maxLength;
   self->memory = calloc (self->maxLength, self->itemSize);

   return self;
}

void
JoeArray_delete (JoeArray self)
{
   free (self->memory);
   free (self);
}

void
JoeArray_realloc (JoeArray self, int newLength)
{
   if (newLength > self->maxLength) {
      int oldSize =  self->maxLength * self->itemSize;
      int newSize =  newLength * self->itemSize;
      self->memory = realloc (self->memory, newSize);
      memset (((char*)self->memory) + oldSize, 0, newSize - oldSize);
      self->maxLength = newLength;
   }
}

void *
JoeArray_getMem (JoeArray self)
{
   return self->memory;
}

void
JoeArray_add (JoeArray self, void *item)
{
   int idx = self->length++;
   if (self->maxLength < self->length)
      JoeArray_realloc (self, self->maxLength << 1);
   memcpy (((char*)self->memory)+(idx * self->itemSize), item, self->itemSize);
}

void *
JoeArray_get (JoeArray self, int idx)
{
   if (idx < self->maxLength) {
      return ((char*)self->memory) +  (idx * self->itemSize);
   } else {
      return (void *) 0;
   }
}

void *
JoeArray_set (JoeArray self, int idx, void *item)
{
   if (idx < self->maxLength) {
      memcpy (((char*)self->memory)+(idx*self->itemSize),item,self->itemSize);
      if (idx >= self->length)
         self->length = ++idx;
      return item;
   } else {
      return (void *) 0;
   }
}

int
JoeArray_length (JoeArray self)
{
   return self->length;
}

void
JoeArray_clear (JoeArray self)
{
   self->length = 0;
}

/* test
struct s {
   int a;
   int b;
} item;
void
show (JoeArray sArray)
{
   int i;
   for (i = JoeArray_length (sArray) - 1; i >= 0; i--) {
      printf ("[%d][%d]\n", ((struct s*) JoeArray_get (sArray, i))->a,
                            ((struct s*) JoeArray_get (sArray, i))->b);
   }
}

int
main (int argc, char **argv)
{
   int i;
   JoeArray sArray = JoeArray_new (sizeof (struct s), 1);

   item.a = 1;
   item.b = 9;
   JoeArray_add (sArray, &item);

   item.a = 2;
   item.b = 8;
   JoeArray_add (sArray, &item);

   item.a = 3;
   item.b = 7;
   JoeArray_add (sArray, &item);

   item.a = 4;
   item.b = 6;
   JoeArray_add (sArray, &item);

   item.a = 5;
   item.b = 5;
   JoeArray_add (sArray, &item);

   item.a = 8;
   item.b = 8;
   JoeArray_set (sArray, 7, &item);

   JoeArray_realloc (sArray, 32);
   item.a = 3;
   item.b = 1;
   JoeArray_set (sArray, 31, &item);
   show (sArray);

   JoeArray_delete (sArray);
   return 0;
}
*/
