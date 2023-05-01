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

# ifndef joe_Variable_h
# define joe_Variable_h  "$Id$";
# include "joe_String.h"

typedef joe_Object joe_Variable;
extern joe_Class joe_Variable_Class;
/* joe_Object joe_Variable_New (char *c); */
joe_Object joe_Variable_New_String (joe_String name, int depth, int index);
joe_String joe_Variable_name (joe_Variable self);
char * joe_Variable_nameCharStar (joe_Variable self);
int joe_Variable_getDepth (joe_Variable self);
int joe_Variable_getIndex (joe_Variable self);
unsigned int joe_Variable_hash (joe_Variable self);

# endif
