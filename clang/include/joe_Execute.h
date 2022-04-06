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

# ifndef joe_Execute_h
# define joe_Execute_h  "$Id$";
# include "joe_Object.h"
# include "joe_Block.h"

typedef joe_Object joe_Execute;
extern joe_Class joe_Execute_Class;
JOEOBJ_API joe_Object joe_Execute_New (joe_Block parent);
JOEOBJ_API int joe_Execute_add (joe_Execute self, char *line);
JOEOBJ_API int joe_Execute_clear (joe_Execute self);
JOEOBJ_API int joe_Execute_exec (joe_Execute self, joe_Object *retval);

# endif
