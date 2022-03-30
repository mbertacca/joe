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

# ifndef joe_Message_h
# define joe_Message_h  "$Id$";
# include "joe_String.h"

typedef joe_Object joe_Message;
typedef joe_Object joe_Block;

extern joe_Class joe_Message_Class;

joe_Object joe_Message_New (joe_Object assign2, int argc, joe_Object *argv,
                            char *fileName, int row, int col);
joe_Message joe_Message_clone (joe_Message self, joe_Block parent);
int joe_Message_exec (joe_Object self, joe_Block block, joe_Object *retval);
int joe_Message_isLabel (joe_Message self, joe_String name);
char* joe_Message_toString(joe_Message self);
# endif
