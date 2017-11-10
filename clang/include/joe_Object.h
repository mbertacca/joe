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

# ifndef joe_Object_h
# define joe_Object_h  "$Id$";

# define JOE_TRUE 1
# define JOE_FALSE 0

# define JOE_FAILURE 1
# define JOE_SUCCESS 0

typedef struct s_joe_Object *joe_Object;

# define JOE_CTOR_ARGS joe_Object self, \
                       int argc, \
                       joe_Object *argv, \
                       joe_Object *retval
# define JOE_METHOD_ARGS joe_Object self, \
                       int argc, \
                       joe_Object *argv, \
                       joe_Object *retval

typedef struct s_joe_Method joe_Method;
struct s_joe_Method {
   char *name;
   int (*mthd)(JOE_METHOD_ARGS);
};

typedef struct s_joe_Class {
   char  *name;
   int (*ctor)(JOE_CTOR_ARGS);
   void (*dtor)(joe_Object *_self);
   joe_Method *mthds;
   char **varNames;
   struct s_joe_Class *extends;
   void *reserved; /* must be 0 */
} joe_Class;

extern joe_Class joe_Object_Class;
extern joe_Object joe_Boolean_True;
extern joe_Object joe_Boolean_False;

void joe_Class_registerClass (joe_Class *c);
joe_Class *joe_Class_getClass (char *name);
int joe_Class_newInstance (joe_Class *cls,
                           int argc, joe_Object *args, joe_Object *retval);
joe_Object joe_Object_New (joe_Class *clazz, unsigned int size);
int joe_Object_getVarIdx (joe_Object self, char *name);
joe_Object *joe_Object_getVar (joe_Object self, char *name);
unsigned int joe_Object_length (joe_Object self);
joe_Object *joe_Object_at (joe_Object self, unsigned int idx);
void joe_Object_assign (joe_Object *self, joe_Object value);
void joe_Object_delIfUnassigned (joe_Object *self);
void joe_Object_incrReference (joe_Object *self);
void joe_Object_decrReference (joe_Object *self);
unsigned int joe_Object_getLiveObjectsCount ();
void joe_Object_showLiveObjects ();
void ** joe_Object_getMem (joe_Object self);
joe_Class * joe_Object_getClass (joe_Object self);
char * joe_Object_getClassName (joe_Object self);
int joe_Object_instanceOf (joe_Object self, joe_Class *obj);
joe_Object joe_Object_clone (joe_Object self);
joe_Method *joe_Object_getMethod (joe_Class *clazz, const char *name);
int joe_Object_invoke (joe_Object self, const char *name,
                       int argc, joe_Object *argv, joe_Object *retval);
int joe_Object_invokeSuper (joe_Object self, const char *name,
                       int argc, joe_Object *argv, joe_Object *retval);
int joe_Object_assignInvoke (joe_Object self, const char *name,
                             int argc, joe_Object *argv, joe_Object *retval);
int joe_Object_assignInvokeSuper (joe_Object self, const char *name,
                             int argc, joe_Object *argv, joe_Object *retval);
void joe_Object_extends (joe_Object self, joe_Class *childClass);

# endif
