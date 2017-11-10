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

# ifndef joe_BreakException_h
# define joe_BreakException_h  "$Id$";
# include "joe_Exception.h"

typedef joe_Object joe_BreakException;
extern joe_Class joe_BreakException_Class;
joe_Object joe_BreakException_New (char *c);
void joe_BreakException_setReturnObj (joe_Object self, joe_Object obj);
joe_Object joe_BreakException_getReturnObj (joe_Object self);
int joe_BreakException_hasReturnObj (joe_Object self);
# endif
