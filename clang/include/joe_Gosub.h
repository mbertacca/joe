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

# ifndef joe_Gosub_h
# define joe_Gosub_h  "$Id$";
# include "joe_String.h"

typedef joe_Object joe_Gosub;
extern joe_Class joe_Gosub_Class;

joe_Object joe_Gosub_New (joe_String msg);
void joe_Gosub_setReturnObj (joe_Object self, joe_Object obj);
joe_String joe_Gosub_getLabel (joe_Object self);
joe_Object joe_Gosub_getReturnObj (joe_Object self);
int  joe_Gosub_hasReturnObj (joe_Object self);


# endif
