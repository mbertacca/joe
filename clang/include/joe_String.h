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

# ifndef joe_String_h
# define joe_String_h  "$Id$";
# include "joe_Object.h"

typedef joe_Object joe_String;
extern joe_Class joe_String_Class;
joe_Object joe_String_New (const char *c);
joe_Object joe_String_New2 (const char *c1, const char *c2);
joe_Object joe_String_New3 (const char *c1, const char *c2, const char *c3);
joe_Object joe_String_New4 (const char *c1, const char *c2, const char *c3,
                            const char *c4);
joe_Object joe_String_New_len (char *c, unsigned int len);
char * joe_String_getCharStar (joe_String self);
unsigned int joe_String_length (joe_String self);
int joe_String_compare (joe_String self, joe_String aStr);

# endif
