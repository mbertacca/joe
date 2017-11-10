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

# ifndef joe_ArrayScan_h
# define joe_ArrayScan_h  "$Id$";
# include "joe_Array.h"

typedef joe_Object joe_ArrayScan;
extern joe_Class joe_ArrayScan_Class;

joe_Object joe_ArrayScan_New (joe_Array array);
joe_Object joe_ArrayScan_next (joe_ArrayScan self);
void joe_ArrayScan_back (joe_ArrayScan self);
joe_Object joe_ArrayScan_peek (joe_ArrayScan self);
int joe_ArrayScan_hasNext (joe_ArrayScan self);
int joe_ArrayScan_hasPeek (joe_ArrayScan self);

# endif

