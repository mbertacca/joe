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

# ifndef joe_HashMap_h
# define joe_HashMap_h  "$Id$";
# include "joe_Object.h"
# include "joe_Array.h"
# include "joe_String.h"

typedef joe_Object joe_HashMap;
extern joe_Class joe_HashMap_Class;

joe_Object joe_HashMap_New();
joe_Object joe_HashMap_New_size(unsigned int len);
void joe_HashMap_put (joe_HashMap self, joe_Object key,
                      joe_Object value,  joe_Object *oldValue);
int joe_HashMap_get (joe_HashMap self, joe_Object key, joe_Object *retval);
int joe_HashMap_getCS (joe_HashMap self, const char *csKey, joe_Object *retval);
int joe_HashMap_getHash (joe_HashMap self, unsigned int hash,
                         joe_Object key, joe_Object *retval);
int joe_HashMap_getHashCS (joe_HashMap self, unsigned int hash,
                         const char *csKey, joe_Object *retval);
joe_Array joe_HashMap_keys (joe_HashMap self);
joe_Array joe_HashMap_values (joe_HashMap self);
int joe_HashMap_containsHashKey (joe_HashMap self, unsigned int hash, joe_Object key);
int joe_HashMap_containsKey (joe_HashMap self, joe_Object key);
int joe_HashMap_containsValue (joe_HashMap self, joe_Object value);
unsigned int joe_HashMap_length(joe_HashMap self);
void joe_HashMap_clean (joe_HashMap self);
unsigned int joe_HashMap_hash(joe_String strKey);
# endif
