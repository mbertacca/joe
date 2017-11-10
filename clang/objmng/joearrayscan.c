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

# include "joearrayscan.h"
# include <stdlib.h>

struct t_JoeArrayScan {
   JoeArray array;
   unsigned int len;
   unsigned int idx;
};

JoeArrayScan
JoeArrayScan_new (JoeArray array)
{
   JoeArrayScan self = calloc (1, sizeof (struct t_JoeArrayScan));
   self->array = array;
   self->len = JoeArray_length (array);
   self->idx = 0;

   return self;
}

void
JoeArrayScan_delete (JoeArrayScan self)
{
   free (self);
}

void
JoeArrayScan_clean (JoeArrayScan self)
{
   self->array = 0;
   self->len = 0;
   self->idx = 0;
}

void *
JoeArrayScan_next (JoeArrayScan self)
{
   if (self->idx < self->len)
      return JoeArray_get (self->array, self->idx++);
   else
      return 0;
}

void *
JoeArrayScan_prev (JoeArrayScan self)
{
   if (self->idx > 0)
      return JoeArray_get (self->array, --self->idx);
   else
      return 0;
}

void *
JoeArrayScan_peek (JoeArrayScan self)
{
   if (self->idx < self->len)
      return JoeArray_get (self->array, self->idx);
   else
      return 0;
}

