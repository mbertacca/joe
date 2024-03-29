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

# ifndef joe_List_h
# define joe_List_h  "$Id$";
# include "joe_Object.h"
# include "joe_Block.h"

typedef joe_Object joe_ListItem;
JOEOBJ_API joe_ListItem joe_ListItem_next(joe_ListItem self);
JOEOBJ_API joe_Object joe_ListItem_value(joe_ListItem self);

typedef joe_Object joe_List;
extern joe_Class joe_List_Class;

JOEOBJ_API joe_List joe_List_New();
JOEOBJ_API void joe_List_push(joe_List self, joe_Object obj);
JOEOBJ_API void joe_List_pop(joe_List self, joe_Object* retval);
JOEOBJ_API int joe_List_empty(joe_List self);
JOEOBJ_API joe_ListItem joe_List_getFirstItem(joe_List self);

# endif
