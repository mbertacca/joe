/*
 * This source file is part of the "Joe Objects Executor" open source project
 *
 * Copyright 2021 Marco Bertacca (www.bertacca.eu)
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

# ifndef joe_Selector_h
# define joe_Selector_h  "$Id$";
# include "joe_Object.h"

typedef joe_Object joe_Selector;
extern joe_Class joe_Selector_Class;
joe_Object joe_Selector_New (char *name, int argc);
int joe_Selector_invoke (joe_Object self, joe_Object receiver,
                         int argc, joe_Object *argv, joe_Object *retval);
char * joe_Selector_name (joe_Object self);
char * joe_Selector_toString (joe_Object self);
int joe_Selector_getArgc (joe_Selector self);


# endif
