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

# include "joe_BreakBlockException.h"

static char* variables[] = { 0 };

static joe_Method mthds[] = {
  {(void *) 0, (void *) 0}
};

joe_Class joe_BreakBlockException_Class = {
   "joe_BreakBlockException",
   0,
   0,
   mthds,
   variables,
   &joe_BreakException_Class,
   0
};


joe_Object
joe_BreakBlockException_New (char *desc) {
   joe_Object self;
   self = joe_Object_New (&joe_BreakBlockException_Class, 0);
   joe_Exception_Init (self, joe_String_New(desc));

   return self;
}
