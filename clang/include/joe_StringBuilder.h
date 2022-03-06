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

# ifndef joe_StringBuilder_h
# define joe_StringBuilder_h  "$Id$";
# include "joe_Object.h"

typedef joe_Object joe_StringBuilder;
extern joe_Class joe_StringBuilder_Class;
joe_Object joe_StringBuilder_New ();
void joe_StringBuilder_appendChar (joe_StringBuilder self, char c);
void joe_StringBuilder_appendCharStar (joe_StringBuilder self, char *s);
void joe_StringBuilder_appendInt (joe_StringBuilder self, int n);
void joe_StringBuilder_append (joe_StringBuilder self, joe_Object obj);
unsigned int joe_StringBuilder_length (joe_StringBuilder self);
joe_Object joe_StringBuilder_toString (joe_StringBuilder self);
char * joe_StringBuilder_getCharStar (joe_Object self);

# endif
