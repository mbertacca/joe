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

# ifndef joe_Exception_h
# define joe_Exception_h  "$Id$";
# include "joe_String.h"

typedef joe_Object joe_Exception;
extern joe_Class joe_Exception_Class;
void joe_Exception_Init (joe_Exception self, char *desc);
joe_Object joe_Exception_New (char *c);
joe_Object joe_Exception_New_string (joe_String msg);
void joe_Exception_addStack (joe_Exception self,
                             char *fileName, int row, int col);
int joe_Exception_toString (joe_Exception self,
                            int argc, joe_Object *argv, joe_Object *retval);
int joe_Exception_getMessage (joe_Object self,
                             int argc, joe_Object *argv, joe_Object *retval);

# endif
