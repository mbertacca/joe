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

# ifndef joe_Files_h
# define joe_Files_h  "$Id$";
# include <stdio.h>
# include "joe_Object.h"

typedef joe_Object joe_Files;
extern joe_Class joe_Files_Class;
JOEOBJ_API extern ssize_t joe_Files_getline (char **lineptr, ssize_t *n, FILE *fp);
JOEOBJ_API extern int joe_Files_isAbsolute (char *p);

# endif
