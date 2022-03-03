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

# ifndef joe_Block_h
# define joe_Block_h  "$Id$";
# include "joe_Message.h"
# include "joe_Variable.h"

typedef joe_Object joe_Block;
extern joe_Class joe_Block_Class;
JOEOBJ_API void joe_Block_Init (joe_Block self, joe_Block parent);
JOEOBJ_API joe_Block joe_Block_New (joe_Block parent);
JOEOBJ_API void joe_Block_addMessage (joe_Block self, joe_Message msg);
JOEOBJ_API joe_Block joe_Block_clone (joe_Block self, joe_Block parent);
JOEOBJ_API int joe_Block_outer_exec (joe_Block self,
                    int argc, joe_Object *args, joe_Object *retval);
JOEOBJ_API int joe_Block_exec (joe_Block self,
                    int argc, joe_Object *args, joe_Object *retval);
JOEOBJ_API int joe_Block_new (joe_Block self,
                   int argc, joe_Object *args, joe_Object *retval);
JOEOBJ_API int joe_Block_getVariable (joe_Block self, joe_Variable var,
                                                 joe_Object* retval);

JOEOBJ_API void joe_Block_setName (joe_Block self, char *name);
JOEOBJ_API char * joe_Block_getName (joe_Block self);

void joe_Block_addArgName (joe_Block self, char *name);
void joe_Block_setParent (joe_Block self, joe_Block parent);

# endif
