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

# ifndef joe_ArrayList_h
# define joe_ArrayList_h  "$Id$";
# include "joe_Object.h"

typedef joe_Object joe_ArrayList;
extern joe_Class joe_ArrayList_Class;

joe_Object joe_ArrayList_New (unsigned int maxLength);
unsigned int joe_ArrayList_length (joe_Object self);
unsigned int joe_ArrayList_maxLength (joe_Object self);
void joe_ArrayList_add (joe_Object self, joe_Object item);
joe_Object *joe_ArrayList_at (joe_Object self, unsigned int idx);
joe_Object joe_ArrayList_getArray (joe_Object self);

# endif
