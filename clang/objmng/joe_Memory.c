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

# include "joe_Memory.h"

static joe_Method mthds[] = {
  {(void *) 0, (void *) 0}
};

joe_Class joe_Memory_Class = {
   "joe_Memory",
   0,
   0,
   mthds,
   0,
   &joe_Object_Class,
   0
};

joe_Object
joe_Memory_New (unsigned int size) {
   joe_Object self;

   self = joe_Object_New (&joe_Memory_Class, size);
   return self;
}

typedef struct s_WeakReference {
   joe_Object obj;
} WeakReference;

joe_Class joe_WeakReference_Class = {
   "joe_WeakReference",
   0,
   0,
   mthds,
   0,
   &joe_Object_Class,
   0
};
joe_Object
joe_WeakReference_New(joe_Object obj)
{
   joe_WeakReference self;

   self = joe_Object_New(&joe_WeakReference_Class,
                        sizeof(struct s_WeakReference));
   ((WeakReference*)joe_Object_getMem(self))->obj = obj;
   return self;
}

joe_Object
joe_WeakReference_get(joe_Object self)
{
   WeakReference* wr = (void*)joe_Object_getMem(self);
   return wr->obj;
}