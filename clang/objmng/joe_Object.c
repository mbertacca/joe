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

# include <stdio.h>
# include <stdlib.h>
# include <string.h>

# include "joe_Array.h"
# include "joe_ArrayScan.h"
# include "joe_Boolean.h"
# include "joe_String.h"
# include "joe_StringBuilder.h"
# include "joe_Integer.h"
# include "joe_Float.h"
# include "joe_Exception.h"
# include "joe_JOEObject.h"
# include "joe_LoadScript.h"

# define BLACK 0
# define GRAY 1
# define WHITE 2

static struct s_ClassList *registeredClasses = 0;
# define JOE_MEM_DEBUG
static struct s_ObjectList *allObjects = 0;
/* # define JOE_MEM_DEBUG_FAST */
static unsigned int liveObjCount = 0;

union ptr {
   joe_Object * obj;
   void * mem;
   long  lng;
   double dbl;
};

struct s_joe_Object {
   unsigned int color:2;
   unsigned int refcount:30;
   unsigned int nItems;
   joe_Class *clazz;
   union ptr data;
};

struct s_ClassList {
   joe_Class *clazz;
   struct s_ClassList * next;
};

struct s_ObjectList {
   joe_Object obj;
   struct s_ObjectList * next;
   struct s_ObjectList * prev;
};

void
traceMemory (joe_Object obj)
{
# ifdef JOE_MEM_DEBUG_FAST
   liveObjCount++;
# endif
# ifdef JOE_MEM_DEBUG
   if (allObjects == 0) {
      allObjects = calloc (1, sizeof (struct s_ObjectList));
      allObjects->prev = 0;
      allObjects->next = 0;
      allObjects->obj = obj;
   } else {
      allObjects->prev = calloc (1, sizeof (struct s_ObjectList));
      allObjects->prev->obj = obj;
      allObjects->prev->next = allObjects;
      allObjects->prev->prev = 0;
      allObjects = allObjects->prev;
   }
# endif
}

static void init ();

void
joe_Class_registerClass (joe_Class *c)
{
   struct s_ClassList *newClass;
   if (registeredClasses == 0) {
      init ();
   }
   newClass = calloc (1, sizeof (struct s_ClassList));
   newClass->clazz = c;
   newClass->next = registeredClasses;
   registeredClasses = newClass;
}

static void
init ()
{
   if (registeredClasses == 0) {
      registeredClasses = calloc (1, sizeof (struct s_ClassList));
      registeredClasses->clazz = &joe_Object_Class;
      registeredClasses->next = 0;
      joe_Class_registerClass (&joe_Array_Class);
      joe_Class_registerClass (&joe_ArrayScan_Class);
      joe_Class_registerClass (&joe_String_Class);
      joe_Class_registerClass (&joe_StringBuilder_Class);
      joe_Class_registerClass (&joe_Integer_Class);
      joe_Class_registerClass (&joe_Float_Class);
      joe_Class_registerClass (&joe_LoadScript_Class);
      joe_Class_registerClass (&joe_Exception_Class);
   }
}

joe_Class *
joe_Class_getClass (char *name)
{
   joe_Class *cls;
   struct s_ClassList *clst;
   init();
   for (clst = registeredClasses; clst; clst = clst->next) {
      cls = clst->clazz;
      if (!strcmp (name, cls->name)) {
         return cls;
      }
   }
   return 0;
}

int
joe_Class_newInstance (joe_Class *cls,
                       int argc, joe_Object *args, joe_Object *retval)
{
   joe_Object newObj = joe_Object_New (cls, 0);
   if (cls->ctor != 0) {
      if (cls->ctor (newObj, argc, args, retval) == JOE_SUCCESS) {
         *retval = newObj;
         return JOE_SUCCESS;
      } else {
         joe_Object_delIfUnassigned (&newObj);
         return JOE_FAILURE;
      }
   } else {
      *retval = newObj;
      return JOE_SUCCESS;
   }
}

joe_Object
joe_Object_New (joe_Class *clazz, unsigned int size)
{
   joe_Object Return = 0;
   if (clazz->varNames == 0) { /* primitive */
      Return = calloc (1, sizeof (struct s_joe_Object) + size);
      Return->data.mem = (char *) Return + sizeof (struct s_joe_Object);
      size = 0;
   } else {
      if (size == 0) {
         int i;
         joe_Class *cls = clazz;
         while (cls) {
            if (cls->varNames) {
               for (i = 0; cls->varNames[i] != 0; i++)
                  size++;
            }
            cls = cls->extends;
         }
      }
      Return = calloc (1,sizeof(struct s_joe_Object)+(size*(sizeof(void*))));
      Return->data.mem = (char *) Return + sizeof (struct s_joe_Object);
   }
   Return->nItems = size;
   Return->clazz = clazz;

   traceMemory (Return);

   return Return;
}

joe_Object
joe_Object_clone (joe_Object self)
{
   if (self->nItems) {
      unsigned int size = sizeof(struct s_joe_Object) +
                         (self->nItems * (sizeof(joe_Object)));
      joe_Object Return = malloc (size);
      memcpy (Return, self, size);
      Return->data.mem = (char *) Return + sizeof (struct s_joe_Object);
      Return->refcount = 0;

      for (size = 0; size < Return->nItems; size++)
         if (Return->data.obj[size])
            Return->data.obj[size]->refcount++;

      traceMemory (Return);
      return Return;
   } else {
      return 0;
   }
}

void
joe_Object_extends (joe_Object self, joe_Class *childClass)
{
   childClass->extends = self->clazz;
   self->clazz = childClass;
}

void
joe_Object_Delete (joe_Object self)
{
# ifdef JOE_MEM_DEBUG_FAST
   liveObjCount--;
# endif
# ifdef JOE_MEM_DEBUG
   struct s_ObjectList *list;
   for (list = allObjects; list; list = list->next)
      if (list->obj == self) {
         if (list->prev) {
            list->prev->next = list->next;
            if (list->next)
               list->next->prev = list->prev;
         } else {
            if (list->next) {
               list->next->prev = 0;
               allObjects = list->next;
            } else
               allObjects = 0;
         }
         free (list);
         break;
      }
   if (list == 0) {
      printf ("Memory not found! %p\n", (void*) self);
   }
# endif
   self->data.obj = 0;
   self->color = 3;
   self->refcount = 9;
   free (self);
}

void
joe_Object_showLiveObjects ()
{
   if (allObjects) {
      unsigned int Return;
      struct s_ObjectList *list;
      for (Return = 0, list = allObjects; list; Return++) {
         if (list->obj->clazz == &joe_String_Class) {
            printf ("[%s] %s\n", list->obj->clazz->name,
                                 joe_String_getCharStar(list->obj));
         } else if (list->obj->clazz == &joe_Integer_Class) {
            printf ("[%s] %ld\n", list->obj->clazz->name,
                                 joe_Integer_value(list->obj));
         } else if (list->obj->clazz == &joe_Float_Class) {
            printf ("[%s] %lf\n", list->obj->clazz->name,
                                 joe_Float_value(list->obj));
         } else {
            printf ("[%s] %p\n", list->obj->clazz->name, (void*) list->obj);
         }
         list = list->next;
      }
   }
}

unsigned int
joe_Object_getLiveObjectsCount ()
{
   if (allObjects) {
      unsigned int Return;
      struct s_ObjectList *list;
      for (Return = 0, list = allObjects; list; Return++)
         list = list->next;
      return Return;
   } else {
      return liveObjCount;
   }
}

void **
joe_Object_getMem (joe_Object self)
{
   return &(self->data.mem);
}

unsigned int
joe_Object_length (joe_Object self)
{
   return self->nItems;
}

joe_Object *
joe_Object_at (joe_Object self, unsigned int idx)
{
   if (idx < self->nItems)
      return &self->data.obj[idx];
   else
      return 0;
}

static int
getVarIdx (joe_Class *cls, char *name, unsigned int *varIdx)
{
   int Return = JOE_FAILURE;
   if (cls->extends) 
      Return = getVarIdx (cls->extends, name, varIdx);
   if (Return != JOE_SUCCESS && cls->varNames) {
      unsigned int idx;
      char *var;
      for (idx = 0; (var = cls->varNames[idx]); idx++, (*varIdx)++) {
         if (!strcmp (var, name)) {
             Return = JOE_SUCCESS;
             break;
         }
      }
   }
   return Return;
}

int
joe_Object_getVarIdx (joe_Object self, char *name)
{
   unsigned int varIdx = 0;
   if (getVarIdx (self->clazz, name, &varIdx) == JOE_SUCCESS)
      return varIdx;
   else
      return -1;
}

joe_Object *
joe_Object_getVar (joe_Object self, char *name)
{
   int idx = joe_Object_getVarIdx (self, name);
   if (idx >= 0)
      return joe_Object_at (self, idx);
   else
      return 0;
}


static void
markGray (joe_Object self)
{
   self->color = GRAY;
   if (self->nItems) {
      int idx;
      joe_Object obj;
      for (idx = 0; idx < self->nItems; idx++) {
         if ((obj = self->data.obj[idx])) {
            obj->refcount--;
            if (obj->color != GRAY)
               markGray (obj);
         }
      }
   }
}

static void
scanBlack (joe_Object self)
{
   self->color = BLACK;
   if (self->nItems) {
      int idx;
      joe_Object obj;
      for (idx = 0; idx < self->nItems; idx++) {
         if ((obj = self->data.obj[idx])) {
            obj->refcount++;
            if (obj->color != BLACK)
               scanBlack (obj);
         }
      }
   }
}

static void
scanGray (joe_Object self)
{
   if (self->refcount != 0) {
       scanBlack (self);
   } else {
      self->color = WHITE;
      if (self->nItems) {
         int idx;
         joe_Object obj;
         for (idx = 0; idx < self->nItems; idx++) {
            if ((obj = self->data.obj[idx])) {
               if (obj->color == GRAY)
                  scanGray (obj);
            }
         }
      }
   }
}

static void
collectWhite (joe_Object self)
{
   if (self->color == WHITE) {
      self->color = BLACK;
      if (self->nItems) {
         int idx;
         joe_Object obj;
         for (idx = 0; idx < self->nItems; idx++) {
            if ((obj = self->data.obj[idx])) {
               collectWhite (obj);
            }
         }
      }
      joe_Object_Delete (self);
   }
}

void
joe_Object_unassign (joe_Object self)
{
   if (--self->refcount == 0) {
      if (self->nItems) {
         joe_Object obj;
         int idx;
         for (idx = 0; idx < self->nItems; idx++) {
            if ((obj = self->data.obj[idx])) {
               joe_Object_unassign (obj);
            }
         }
      }
      joe_Object_Delete (self);
   } else {
      markGray (self);
      scanGray (self);
      collectWhite (self);
   }
}

void
joe_Object_incrReference (joe_Object *self)
{
   if (*self)
      (*self)->refcount++;
}

void
joe_Object_decrReference (joe_Object *self)
{
   if (*self  && (*self)->refcount)
      (*self)->refcount--;
}

void
joe_Object_delIfUnassigned (joe_Object *self)
{
   if (*self) {
      (*self)->refcount++;
      joe_Object_unassign (*self);
   }
}

void
joe_Object_addReference (joe_Object self)
{
   self->refcount++;
}

void
joe_Object_assign (joe_Object *self, joe_Object value)
{
   if (self) {
      joe_Object obj = *self;
      *self = value;
      if (value)
         value->refcount++;
      if (obj != 0)
         joe_Object_unassign (obj);
   }
}

joe_Class *
joe_Object_getClass (joe_Object self)
{
  if (self)
     return self->clazz;
  else
     return 0;
}

char *
joe_Object_getClassName (joe_Object self)
{
  if (self)
     return self->clazz->name;
  else
     return "(null)";
}

int
joe_Object_instanceOf (joe_Object self, joe_Class *clazz)
{
   if (self) {
      joe_Class *selfclazz = self->clazz;
      while (selfclazz) {
         if (selfclazz == clazz)
            return JOE_TRUE;
         selfclazz = selfclazz->extends;
      }
   }
   return JOE_FALSE;
}

joe_Method *
joe_Object_getMethod (joe_Class *clazz, const char *name)
{
   joe_Method *mthd;

   while (clazz) {
      if (clazz->mthds)
         for (mthd = clazz->mthds; mthd->name != (void *) 0; mthd++) {
            if (!strcmp (name, mthd->name))
               return mthd;
         }
      clazz = clazz->extends;
   }
   return 0;
}

static char buffer[256];

static int
invoke (joe_Object self, joe_Class *clazz, const char *name,
                   int argc, joe_Object *argv, joe_Object *retval)
{
   int Return;
   joe_Method *mthd = 0;
   if (clazz == &joe_JOEObject_Class) {
      joe_Block receiver = joe_JOEObject_getReceiver (self, name);
      if (receiver != 0) {
         mthd = joe_Object_getMethod (&joe_Block_Class, "exec");
         return mthd->mthd (receiver, argc, argv, retval);
      }
   }

   mthd = joe_Object_getMethod (clazz, name);
   if (mthd) {
      Return = mthd->mthd (self, argc, argv, retval);
   } else {
      sprintf (buffer, "Method not found: %s in class %s",
                 name, clazz->name);
      *retval = joe_Exception_New (buffer);
      Return = JOE_FAILURE;
   }
   return Return;
}

int
joe_Object_invoke (joe_Object self, const char *name,
                   int argc, joe_Object *argv, joe_Object *retval)
{
   return invoke (self, self->clazz, name, argc, argv, retval);
}

int
joe_Object_invokeSuper (joe_Object self, const char *name,
                   int argc, joe_Object *argv, joe_Object *retval)
{
   return invoke (self, self->clazz->extends, name, argc, argv, retval);
}

int
joe_Object_assignInvoke (joe_Object self, const char *name,
                         int argc, joe_Object *argv, joe_Object *retval)
{
   joe_Object lRetval = 0;
   int rc = joe_Object_invoke (self, name, argc, argv, &lRetval);
   joe_Object_assign (retval, lRetval);
   return rc;
}

int
joe_Object_assignInvokeSuper (joe_Object self, const char *name,
                         int argc, joe_Object *argv, joe_Object *retval)
{
   joe_Object lRetval = 0;
   int rc = joe_Object_invokeSuper (self, name, argc, argv, &lRetval);
   joe_Object_assign (retval, lRetval);
   return rc;
}

static int
toString (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   sprintf (buffer, "[%s 0x%lx]", self->clazz->name, (unsigned long) self);
   *retval = joe_String_New (buffer);
   return JOE_SUCCESS;
}

static int
equals (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1 && argv[0] == self)
      *retval = joe_Boolean_New_true();
   else
      *retval = joe_Boolean_New_false();
   return JOE_SUCCESS;
}

int
clone (joe_Object self,int argc,joe_Object *argv,joe_Object *retval)
{
   if (argc > 0) {
      *retval = joe_Exception_New ("clone: invalid argument");
      return JOE_FAILURE;
   } else {
      *retval = joe_Object_clone (self);
      if (*retval) {
         return JOE_SUCCESS;
      } else {
         *retval = joe_Exception_New ("cannot clone primitive objects");
         return JOE_FAILURE;
      }
   }
}

static joe_Method mthds[] = {
  {"clone", clone},
  {"equals", equals},
  {"toString", toString},
  {(void *) 0, (void *) 0}
};

joe_Class joe_Object_Class = {
   "joe_Object",
   0,
   0,
   mthds,
   0,
   0,
   0
};

joe_Object joe_Boolean_True;
joe_Object joe_Boolean_False;

static int
boolToString (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (self == joe_Boolean_True)
      *retval = joe_String_New ("true");
   else
      *retval = joe_String_New ("false");
   return JOE_SUCCESS;
}

static int
and (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1) {
      if (argv[0] == joe_Boolean_True) {
         if (self == joe_Boolean_True)
            *retval = joe_Boolean_True;
         else
            *retval = joe_Boolean_False;
         return JOE_SUCCESS;
      } else if (argv[0] == joe_Boolean_False) {
         *retval = joe_Boolean_False;
         return JOE_SUCCESS;
      }
   }
   *retval = joe_Exception_New ("and: invalid argument");
   return JOE_FAILURE;
}

static int
or (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1) {
      if (argv[0] == joe_Boolean_False) {
         if (self == joe_Boolean_True)
            *retval = joe_Boolean_True;
         else
            *retval = joe_Boolean_False;
         return JOE_SUCCESS;
      } else if (argv[0] == joe_Boolean_True) {
         *retval = joe_Boolean_True;
         return JOE_SUCCESS;
      }
   }
   *retval = joe_Exception_New ("or: invalid argument");
   return JOE_FAILURE;
}

static int
not (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 0) {
      if (self == joe_Boolean_False)
         *retval = joe_Boolean_True;
      else
         *retval = joe_Boolean_False;
      return JOE_SUCCESS;
   }
   *retval = joe_Exception_New ("not: invalid argument");
   return JOE_FAILURE;
}


static joe_Method boolMthds[] = {
   {"and", and },
   {"or", or },
   {"not", not },
   {"toString", boolToString },
  {(void *) 0, (void *) 0}
};
joe_Class joe_Boolean_Class = {
   "joe_Boolean",
   0,
   0,
   boolMthds,
   0,
   &joe_Object_Class,
   0
};

static struct s_joe_Object obj_true = { 0, 1, 0, &joe_Boolean_Class, {0} };
joe_Object joe_Boolean_True = &obj_true;
static struct s_joe_Object obj_false = { 0, 1, 0, &joe_Boolean_Class, {0} };
joe_Object joe_Boolean_False = &obj_false;

/* test */

int
debug (joe_Object self)
{
   if (self)
      printf ("%p col=%d, cnt=%d\n",(void*)self, self->color, self->refcount);
   else
      printf ("0x0\n");
   return 0;
}
