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

# ifndef joestrct_h
# define joestrct_h  "$Id$"

union ptr {
   joe_Object * obj;
   void * mem;
   long  lng;
   int   intg;
   double dbl;
   char* str;
};

struct s_joe_Object {
   unsigned char color:4;
   unsigned char acyclic:1;
   unsigned char buffered:1;
   unsigned int refcount;
   unsigned int nItems;
   joe_Class *clazz;
   union ptr data;
};


#define JOE_ISCLASS(self,clzz) ((self)->clazz==clzz)
#define JOE_AT(self,index) (&(self)->data.obj[index])
#define JOE_MEM(self) (&((self)->data.mem))
#define JOE_LEN(self) ((self)->nItems)
#define JOE_ARRAY(self) ((self)->data.obj)
#define JOE_INT(self) ((self)->data.intg)
#define JOE_INT_STAR(self) (&(self)->data.intg)

# endif