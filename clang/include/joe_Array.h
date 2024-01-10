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

# ifndef joe_Array_h
# define joe_Array_h  "$Id$";
# include "joe_Object.h"

typedef joe_Object joe_Array;
extern joe_Class joe_Array_Class;

JOEOBJ_API joe_Object joe_Array_New (unsigned int length);
JOEOBJ_API unsigned int joe_Array_length (joe_Array self);
JOEOBJ_API joe_Array joe_Array_clean(joe_Array self);
JOEOBJ_API int joe_Array_add(joe_Object self, joe_Object obj, joe_Object *retval);

# endif
