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

# ifndef joe_Integer_h
# define joe_Integer_h  "$Id$";
# include "joe_Object.h"
# include <stdint.h>

typedef joe_Object joe_Integer;
extern joe_Class joe_Integer_Class;
JOEOBJ_API joe_Object joe_Integer_New (long value);
JOEOBJ_API long joe_Integer_value(joe_Integer self);
JOEOBJ_API joe_Integer joe_Integer_addMe(joe_Object self, long incr);
JOEOBJ_API joe_Integer joe_Integer_addMe1(joe_Object self);
JOEOBJ_API joe_Integer joe_Integer_subMe1 (joe_Object self);
JOEOBJ_API void joe_Integer_toAscii (int64_t n, char *out);
JOEOBJ_API int64_t joe_Integer_fromAscii (char *asc);
# endif
