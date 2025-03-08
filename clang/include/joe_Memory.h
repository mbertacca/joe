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

# ifndef joe_Memory_h
# define joe_Memory_h  "$Id$";
# include "joe_Object.h"

typedef joe_Object joe_Memory;
extern joe_Class joe_Memory_Class;
typedef joe_Object joe_WeakReference;
extern joe_Class joe_WeakReference_Class;

joe_Object joe_Memory_New (size_t size);

JOEOBJ_API joe_Object joe_WeakReference_New(joe_Object obj);
JOEOBJ_API joe_Object joe_WeakReference_get(joe_Object obj);
# endif
