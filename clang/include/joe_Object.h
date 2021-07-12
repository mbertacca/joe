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

#ifdef WIN32
#define JOEOBJ_API __declspec(dllexport)
#else
#define JOEOBJ_API
#endif /*WIN32*/

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
extern joe_Class joe_int_Class;

typedef joe_Object joe_int;
JOEOBJ_API joe_int joe_int_New0();
JOEOBJ_API joe_int joe_int_New(int initval);
JOEOBJ_API int joe_int_value(joe_int self);
JOEOBJ_API int* joe_int_starValue(joe_int self);

JOEOBJ_API void joe_Class_registerClass (joe_Class *c);
JOEOBJ_API joe_Class *joe_Class_getClass (char *name);
JOEOBJ_API int joe_Class_newInstance (joe_Class *cls,
                           int argc, joe_Object *args, joe_Object *retval);
JOEOBJ_API joe_Object joe_Object_New (joe_Class *clazz, unsigned int size);
JOEOBJ_API int joe_Object_getVarIdx (joe_Object self, char *name);
JOEOBJ_API joe_Object *joe_Object_getVar (joe_Object self, char *name);
JOEOBJ_API unsigned int joe_Object_length (joe_Object self);
JOEOBJ_API joe_Object* joe_Object_array(joe_Object self);
JOEOBJ_API joe_Object* joe_Object_at(joe_Object self, unsigned int idx);
JOEOBJ_API void joe_Object_assign(joe_Object* self, joe_Object value);
JOEOBJ_API void joe_Object_transfer(joe_Object* self, joe_Object *value);
JOEOBJ_API char* joe_Object_toString(joe_Object self);
JOEOBJ_API void joe_Object_delIfUnassigned (joe_Object *self);
JOEOBJ_API joe_Object joe_Object_setAcyclic (joe_Object self);
/*
JOEOBJ_API void joe_Object_incrReference (joe_Object *self);
JOEOBJ_API void joe_Object_decrReference (joe_Object *self);
*/
JOEOBJ_API unsigned int joe_Object_getLiveObjectsCount ();
JOEOBJ_API void joe_Object_showLiveObjects ();
JOEOBJ_API void joe_Object_show(joe_Object obj, int indent);
JOEOBJ_API void joe_Object_gc ();
JOEOBJ_API void ** joe_Object_getMem (joe_Object self);
JOEOBJ_API joe_Class * joe_Object_getClass (joe_Object self);
JOEOBJ_API char * joe_Object_getClassName (joe_Object self);
JOEOBJ_API int joe_Object_instanceOf (joe_Object self, joe_Class *obj);
JOEOBJ_API joe_Object joe_Object_clone (joe_Object self);
JOEOBJ_API joe_Method *joe_Object_getMethod (joe_Class *clazz, const char *name);
JOEOBJ_API int joe_Object_invoke (joe_Object self, const char *name,
                       int argc, joe_Object *argv, joe_Object *retval);
JOEOBJ_API int joe_Object_invokeSuper (joe_Object self, const char *name,
                       int argc, joe_Object *argv, joe_Object *retval);
JOEOBJ_API void joe_Object_extends(joe_Object self, joe_Class* childClass);

# endif
