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

# ifndef joe_LoadScript_h
# define joe_LoadScript_h  "$Id$";
# include <stdio.h>
# include "joe_Block.h"
# include "joe_Array.h"
# include "joe_Bang.h"

typedef joe_Object joe_LoadScript;
extern joe_Class joe_LoadScript_Class;
JOEOBJ_API void joe_LoadScript_setCWD ();
JOEOBJ_API char* joe_LoadScript_getCWD ();
JOEOBJ_API joe_Block joe_LoadScript_New (joe_Block self,
                       char *scriptName, joe_Array path, joe_Bang bang);
FILE * joe_LoadScript_getFile (char *scriptName, joe_Array path, joe_Bang bang);
# endif
