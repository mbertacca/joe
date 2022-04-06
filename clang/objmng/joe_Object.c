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
# include "joe_List.h"
# include "joe_Boolean.h"
# include "joe_String.h"
# include "joe_StringBuilder.h"
# include "joe_Integer.h"
# include "joe_Float.h"
# include "joe_BigDecimal.h"
# include "joe_Exception.h"
# include "joe_JOEObject.h"
# include "joe_LoadScript.h"
# include "joe_Memory.h"
# include "joe_Null.h"
# include "joe_Execute.h"
# include "joestrct.h"

/*
A Pure Reference Counting Garbage Collector
DAVID F. BACON, CLEMENT R. ATTANASIO, V.T. RAJAN, STEPHEN E. SMITH
*/
# define BLACK 0
# define GRAY 1
# define WHITE 2
# define PURPLE 3

static struct s_ClassList *registeredClasses = 0;

# define JOE_MEM_DEBUG
/* # define JOE_MEM_DEBUG_FAST */
static struct s_ObjectList *allObjects = 0;
#ifdef JOE_MEM_DEBUG
#define TRACEMEMORY(a) traceMemory(a)
#else
#ifdef JOE_MEM_DEBUG_FAST
#define TRACEMEMORY(a) liveObjCount++;
#else
#define TRACEMEMORY(a)
#endif /* JOE_MEM_DEBUG_FAST */
#endif /* JOE_MEM_DEBUG */
static unsigned int liveObjCount = 0;

struct s_ClassList {
   joe_Class *clazz;
   struct s_ClassList * next;
};

struct s_ObjectList {
   joe_Object obj;
   struct s_ObjectList * next;
   struct s_ObjectList * prev;
};


static joe_Object roots[128];
static int rootsCnt = 0;
static int maxRoots = sizeof(roots) / sizeof(joe_Object);

# ifdef JOE_MEM_DEBUG
void
traceMemory (joe_Object obj)
{
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
}
# endif

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
      joe_Class_registerClass (&joe_List_Class);
      joe_Class_registerClass (&joe_String_Class);
      joe_Class_registerClass (&joe_StringBuilder_Class);
      joe_Class_registerClass (&joe_Integer_Class);
      joe_Class_registerClass (&joe_Float_Class);
      joe_Class_registerClass (&joe_BigDecimal_Class);
      joe_Class_registerClass (&joe_LoadScript_Class);
      joe_Class_registerClass (&joe_Execute_Class);
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
   joe_Object_assign (retval, joe_Object_New (cls, 0));
   int Return = JOE_SUCCESS;
   if (cls->ctor != 0) {
      Return = cls->ctor (*retval, argc, args, retval);
   }
   return Return;
}

joe_Object
joe_Object_New (joe_Class *clazz, unsigned int size)
{
   joe_Object Return = 0;
   if (clazz->varNames == 0) { /* primitive */
      Return = calloc (1, sizeof (struct s_joe_Object) + size);
      Return->data.mem = (char *) Return + sizeof (struct s_joe_Object);
      Return->acyclic = 1;
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

   TRACEMEMORY (Return);

   return Return;
}

joe_int
joe_int_New0 ()
{
   joe_int Return = calloc(1, sizeof(struct s_joe_Object));
   Return->nItems = 0;
   Return->acyclic = 1;
   Return->clazz = &joe_int_Class;

   TRACEMEMORY(Return);

   return Return;
}

joe_Object
joe_Object_setAcyclic (joe_Object self)
{
   self->acyclic = 1;
   return self;
}

joe_int
joe_int_New(int initval)
{
   joe_int Return = joe_int_New0();
   Return->data.intg = initval;

   return Return;
}

int
joe_int_value(joe_int self)
{
   return self->data.intg;
}

int*
joe_int_starValue(joe_int self)
{
   return &self->data.intg;
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

      TRACEMEMORY (Return);
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
   free (self);
}

void
joe_Object_showLiveObject(joe_Object obj, int indent)
{
   while (indent-- > 0)
      fputs (" ", stdout);
   if (obj) {
      if (obj->clazz == &joe_String_Class) {
         printf("[%s] %p %s %d\n", obj->clazz->name, (void*)obj,
                joe_String_getCharStar(obj), obj->refcount);
      } else if (obj->clazz == &joe_Integer_Class) {
         printf("[%s] %p %ld %d\n", obj->clazz->name, (void*)obj,
                joe_Integer_value(obj), obj->refcount);
      } else if (obj->clazz == &joe_Float_Class) {
         printf("[%s] %p %lf %d\n", obj->clazz->name, (void*)obj,
                joe_Float_value(obj), obj->refcount);
      } else {
         printf("[%s] %p %d\n", obj->clazz->name, (void*)obj, obj->refcount);
      }
   } else {
      printf("[null]\n");
   }
}

void dbgShowClasses();

void
joe_Object_showLiveObjects ()
{
   if (allObjects) {
      unsigned int Return;
      struct s_ObjectList *list;
      for (Return = 0, list = allObjects; list; Return++) {
          joe_Object_showLiveObject (list->obj, 0);
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

joe_Object*
joe_Object_array(joe_Object self)
{
   return self->data.obj;
}

joe_Object *
joe_Object_at (joe_Object self, unsigned int idx)
{
   return &self->data.obj[idx];
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
      unsigned int idx;
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
      unsigned int idx;
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
         unsigned int idx;
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
collectWhite (joe_Object self, joe_Object *dellist)
{
   if (self->color == WHITE) {
      self->color = BLACK;
      if (self->nItems) {
         unsigned int idx;
         joe_Object obj;
         for (idx = 0; idx < self->nItems; idx++) {
            if ((obj = self->data.obj[idx])) {
               collectWhite (obj, dellist);
            }
         }
      }
      (*dellist)->data.mem = self;
      self->data.mem = 0;
      *dellist = self;
   }

}

void
joe_Object_gc ()
{
   int i;
   joe_Object *obj;
   struct s_joe_Object firstdel = { 0, 0, 0, 0, 0, 0, {0} };
   joe_Object scan = &firstdel;
   joe_Object next;

   for (i = 0, obj = roots; i < rootsCnt; i++, obj++) {
      if ((*obj)->color == PURPLE && (*obj)->refcount > 0) {
         markGray (*obj);
      } else {
         if ((*obj)->color == BLACK && (*obj)->refcount == 0) {
            joe_Object_Delete (*obj);
         } else {
            (*obj)->buffered = 0;
         }
         roots[i] = 0;
      }
   }

   for (i = 0, obj = roots; i < rootsCnt; i++, obj++)
      if (*obj)
         scanGray (*obj);

   for (i = 0, obj = roots; i < rootsCnt; i++, obj++) {
      if (*obj) {
         firstdel.data.mem = 0;
         scan = &firstdel;
         (*obj)->buffered = 0;
         collectWhite (*obj, &scan);
         scan = firstdel.data.mem;
         while (scan) {
            next = scan->data.mem;
            joe_Object_Delete(scan);
            scan = next;
         }
      }
   }
   rootsCnt = 0;
    
}

void
joe_Object_unassign (joe_Object self)
{
# ifdef JOE_MEM_DEBUG
   if (self->refcount <= 0)
      printf ("debug: unassign internal error\n");
# endif
   if (--self->refcount == 0) {
      if (self->nItems) {
         joe_Object obj;
         unsigned int idx;
         for (idx = 0; idx < self->nItems; idx++) {
            if ((obj = self->data.obj[idx])) {
               joe_Object_unassign (obj);
            }
         }
      }
      self->color = BLACK;
      if (!self->buffered)
         joe_Object_Delete (self);
   } else if (!self->acyclic) {
      if (self->color != PURPLE) {
         self->color = PURPLE;
         if (!self->buffered) {
            self->buffered = 1;
            roots[rootsCnt++] = self;
            if (rootsCnt == maxRoots)
               joe_Object_gc ();
         }
      }
   }
}

/*
      struct s_joe_Object firstdel = { 0, 0, 0, 0, 0, 0, {0} };
      joe_Object scan = &firstdel;
      joe_Object next;
      dbgCntClasses (self->clazz->name);

      markGray (self);
      scanGray (self);
      collectWhite (self, &scan);

      scan = firstdel.data.mem;
      while (scan) {
         next = scan->data.mem;
         joe_Object_Delete(scan);
         scan = next;
      }
   }
}
*/
void
joe_Object_delIfUnassigned (joe_Object *self)
{
   if (*self) {
      (*self)->refcount++;
      joe_Object_unassign (*self);
   }
}

void
joe_Object_assign (joe_Object *self, joe_Object value)
{
   joe_Object obj = *self;
   *self = value;
   if (value)
      value->refcount++;
   if (obj != 0)
      joe_Object_unassign (obj);
}

void
joe_Object_transfer(joe_Object* self, joe_Object* value)
{
   joe_Object obj = *self;
   *self = *value;
   *value = 0;
   if (obj != 0)
      joe_Object_unassign(obj);
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
joe_Object_isClass (joe_Object self, joe_Class *clazz)
{
   return self->clazz == clazz;
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

char *
joe_Object_toString(joe_Object self)
{
   if (self)
      snprintf(buffer, sizeof(buffer), "[%s %p]",
                       self->clazz->name, (void *) self);
   else
      snprintf(buffer, sizeof(buffer), "()");
   return buffer;
}

static int
invoke (joe_Object self, joe_Class *clazz, const char *name,
                   int argc, joe_Object *argv, joe_Object *retval)
{
   int Return;
   joe_Method *mthd = 0;

   if (clazz == &joe_WeakReference_Class) {
      self = joe_WeakReference_get(self);
      clazz = self->clazz;
   }
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
      joe_Object_assign (retval, joe_Exception_New (buffer));
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

static int
toString (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   joe_Object_assign (retval, joe_String_New (joe_Object_toString(self)));
   return JOE_SUCCESS;
}

static int
getClassName (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   joe_Object_assign (retval, joe_String_New (joe_Object_getClassName(self)));
   return JOE_SUCCESS;
}

static int
equals (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1 && argv[0] == self)
      joe_Object_assign(retval, joe_Boolean_New_true());
   else
      joe_Object_assign(retval, joe_Boolean_New_false());
   return JOE_SUCCESS;
}

int
clone (joe_Object self,int argc,joe_Object *argv,joe_Object *retval)
{
   if (argc > 0) {
      joe_Object_assign(retval, joe_Exception_New ("clone: invalid argument"));
      return JOE_FAILURE;
   } else {
      joe_Object_assign(retval, joe_Object_clone (self));
      if (*retval) {
         return JOE_SUCCESS;
      } else {
         joe_Object_assign(retval, joe_Exception_New ("cannot clone primitive objects"));
         return JOE_FAILURE;
      }
   }
}

static joe_Method mthds[] = {
  {"clone", clone},
  {"equals", equals},
  {"getClassName", getClassName},
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

joe_Class joe_int_Class = {
   "joe_int_Object",
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
      joe_Object_assign (retval, joe_String_New ("<1>"));
   else
      joe_Object_assign(retval, joe_String_New ("<0>"));
   return JOE_SUCCESS;
}

static int
and (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1) {
      if (argv[0] == joe_Boolean_True) {
         if (self == joe_Boolean_True)
            joe_Object_assign(retval, joe_Boolean_True);
         else
            joe_Object_assign(retval, joe_Boolean_False);
         return JOE_SUCCESS;
      } else if (argv[0] == joe_Boolean_False) {
         joe_Object_assign(retval, joe_Boolean_False);
         return JOE_SUCCESS;
      }
   }
   joe_Object_assign(retval, joe_Exception_New ("and: invalid argument"));
   return JOE_FAILURE;
}

static int
or (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1) {
      if (argv[0] == joe_Boolean_False) {
         if (self == joe_Boolean_True)
            joe_Object_assign(retval, joe_Boolean_True);
         else
            joe_Object_assign(retval, joe_Boolean_False);
         return JOE_SUCCESS;
      } else if (argv[0] == joe_Boolean_True) {
         joe_Object_assign (retval, joe_Boolean_True);
         return JOE_SUCCESS;
      }
   }
   joe_Object_assign(retval, joe_Exception_New ("or: invalid argument"));
   return JOE_FAILURE;
}

static int
xor (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1) {
      if ((argv[0] == joe_Boolean_True) ^ (self == joe_Boolean_True))
         joe_Object_assign(retval, joe_Boolean_True);
      else
         joe_Object_assign(retval, joe_Boolean_False);
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval, joe_Exception_New ("or: invalid argument"));
      return JOE_FAILURE;
   }
}

static int
not (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 0) {
      if (self == joe_Boolean_False)
         joe_Object_assign(retval, joe_Boolean_True);
      else
         joe_Object_assign(retval, joe_Boolean_False);
      return JOE_SUCCESS;
   }
   joe_Object_assign(retval, joe_Exception_New ("not: invalid argument"));
   return JOE_FAILURE;
}

static int
iif (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 2) {
      if (self == joe_Boolean_True)
         joe_Object_assign (retval, argv[0]);
      else
         joe_Object_assign (retval, argv[1]);
     return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval, joe_Exception_New ("iif: invalid arguments"));
      return JOE_FAILURE;
   }
}

static int
ifTrue (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if ((argc == 1 && argv[0]->clazz == &joe_Block_Class) ||
       (argc == 2 && argv[0]->clazz == &joe_Block_Class
                  && argv[1]->clazz == &joe_Block_Class)) {
      joe_Method *mthd = joe_Object_getMethod (&joe_Block_Class, "exec");
      if (self == joe_Boolean_True) {
         return mthd->mthd (argv[0], 0, NULL, retval);
      } else if (argc == 2) {
         return mthd->mthd (argv[1], 0, NULL, retval);
      } else {
         joe_Object_assign(retval, joe_Null_value);
         return JOE_SUCCESS;
      }
   } else {
      joe_Object_assign(retval, joe_Exception_New ("invalid arguments"));
      return JOE_FAILURE;
   }
}

static int
ifFalse (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (self == joe_Boolean_True)
      return ifTrue (joe_Boolean_False, argc, argv, retval);
   else
      return ifTrue (joe_Boolean_True, argc, argv, retval);
}

static joe_Method boolMthds[] = {
   {"and", and },
   {"or", or },
   {"xor", xor },
   {"not", not },
   {"iif", iif },
   {"ifTrue", ifTrue },
   {"ifFalse", ifFalse },
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

static struct s_joe_Object obj_true = { 0, 1, 0, 1, 0, &joe_Boolean_Class, {0} };
joe_Object joe_Boolean_True = &obj_true;
static struct s_joe_Object obj_false = { 0, 1, 0, 1, 0, &joe_Boolean_Class, {0} };
joe_Object joe_Boolean_False = &obj_false;

/* Null */
joe_Object joe_Null_value;

static int
nullToString (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   joe_Object_assign(retval, joe_String_New ("()"));
   return JOE_SUCCESS;
}

static int
nullEq (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1) {
      if (argv[0] == NULL || argv[0] == self)
         joe_Object_assign(retval, joe_Boolean_True);
      else
         joe_Object_assign(retval, joe_Boolean_False);
      return JOE_SUCCESS;
   }
   joe_Object_assign(retval, joe_Exception_New ("eq: missing argument"));
   return JOE_FAILURE;
}

static int
nullNe (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1) {
      if (argv[0] != self)
         joe_Object_assign(retval, joe_Boolean_True);
      else
         joe_Object_assign(retval, joe_Boolean_False);
      return JOE_SUCCESS;
   }
   joe_Object_assign(retval, joe_Exception_New ("ne: missing argument"));
   return JOE_FAILURE;
}

static joe_Method nullMthds[] = {
   {"equals", nullEq },
   {"ne", nullNe },
   {"toString", nullToString },
  {(void *) 0, (void *) 0}
};

joe_Class joe_Null_Class = {
   "joe_Null",
   0,
   0,
   nullMthds,
   0,
   &joe_Object_Class,
   0
};

static struct s_joe_Object obj_null = { 0, 1, 0, 1, 0, &joe_Null_Class, {0} };
joe_Null joe_Null_value = &obj_null;

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
