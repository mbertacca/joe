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
# include <string.h>
# include "joe_HashMap.h"
# include "joe_Array.h"
# include "joe_Boolean.h"
# include "joe_Exception.h"
# include "joe_Integer.h"
# include "joe_String.h"
# include "joe_Null.h"
# include "joestrct.h"

/**
# Class joe_HashMap 
### extends joe_Object

This implements a an hash table.

An HashMap can be created using the following method:
```
aHashMap := !newInstance "joe_HashMap" [, aSize ]
```

_aSize_ is the initial size of the new hash table.

In this hash table you can use any kind of object as key, however its
String representation is stored, hence for example the key 1 is equivalent to the key "1"  
*/

# define KEY 0
# define VALUE 1
# define SIBLING 2
static char *item_vars[] = { "strKey", "value", "sibling", 0 };

static joe_Class item_clazz = {
   "joe_HashMap$Item",
   0,
   0,
   0,
   item_vars,
   &joe_Object_Class,
   0
};

static joe_Object
item_New (joe_Object strKey, joe_Object value)
{
   joe_Object self;
   self = joe_Object_New (&item_clazz, 0);
   joe_Object_assign (JOE_AT(self, KEY), strKey);
   joe_Object_assign (JOE_AT(self, VALUE), value);
   joe_Object_assign (JOE_AT(self, SIBLING), joe_int_New (0));
   return self;
}

# define ARRAY 0
# define N_HASH 1
# define LAST_SIB 2
# define LEN 3
# define TOT_LEN 4
# define THRESHOLD 5
static char *variables[] = { "array", "nHash", "lastSibling",
                             "len",   "totLen","threshold"};

struct s_cself {
   joe_Object* theArray;
   unsigned int* nHash;
   unsigned int* lastSibling;
   unsigned int* len;
   unsigned int* totLen;
   unsigned int* threshold;
};

static void
joe_HashMap_fillStruct(joe_HashMap self, struct s_cself *cself)
{
   joe_Object *selfVars = joe_Object_array (self);
   cself->theArray = &selfVars[ARRAY];
   cself->nHash = (unsigned int *) JOE_INT_STAR(selfVars[N_HASH]);
   cself->lastSibling = (unsigned int *) JOE_INT_STAR(selfVars[LAST_SIB]);
   cself->len = (unsigned int *) JOE_INT_STAR(selfVars[LEN]);
   cself->totLen = (unsigned int *) JOE_INT_STAR(selfVars[TOT_LEN]);
   cself->threshold = (unsigned int *) JOE_INT_STAR(selfVars[THRESHOLD]);
}

static void
joe_HashMap_init(joe_HashMap self, unsigned int size, struct s_cself* cself)
{
   unsigned int len = size >> 1;

   *cself->nHash = 0;
   *cself->lastSibling = len;
   *cself->len = len;
   *cself->totLen = size;
   *cself->threshold = len;
}

void
joe_HashMap_clean(joe_HashMap self)
{
   struct s_cself cself;
   joe_HashMap_fillStruct (self, &cself);
   joe_Array_clean (*cself.theArray);
   joe_HashMap_init (self, *(cself.totLen), &cself);
}

static joe_Array
allocArray (joe_HashMap self, unsigned int size)
{
   joe_Array Return = joe_Object_setAcyclic(joe_Array_New (size));
   struct s_cself cself;

   joe_Object_assign(JOE_AT(self, ARRAY), Return);
   joe_HashMap_fillStruct(self, &cself);
   *cself.totLen = size;
   joe_HashMap_init(self, size, &cself);

   return Return;
}

static void
init (joe_Object self, unsigned int size)
{
   joe_Object_assign(JOE_AT(self, N_HASH), joe_int_New0());
   joe_Object_assign(JOE_AT(self, LEN), joe_int_New0());
   joe_Object_assign(JOE_AT(self, THRESHOLD), joe_int_New0());
   joe_Object_assign(JOE_AT(self, LAST_SIB), joe_int_New0());
   joe_Object_assign(JOE_AT(self, TOT_LEN), joe_int_New0());
   allocArray(self, size << 1);
}

joe_Object
joe_HashMap_New_size(unsigned int size)
{
   joe_Object self;
   self = joe_Object_New(&joe_HashMap_Class, 0);
   init (self, size);

   return self;
}

joe_Object
joe_HashMap_New ()
{
   return joe_HashMap_New_size (12);
}

unsigned int
joe_HashMap_hashUCS(const unsigned char * str)
{
   unsigned long hash = 5381;
   int c;

   while ((c = *(str++)))
      hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

   return hash;
}

unsigned int
joe_HashMap_hash(joe_String strKey)
{
   return joe_HashMap_hashUCS((unsigned char *) joe_String_getCharStar(strKey));
}
/*
static unsigned int
hash (joe_String strKey)
{
   unsigned int hash = 0;
   char *c;

   for (c = joe_String_getCharStar (strKey); *c; ++c) {
      hash += *c;
      hash += (hash << 10);
      hash ^= (hash >> 6);
   }
   hash += (hash << 3);
   hash ^= (hash >> 11);
   hash += (hash << 15);

   return hash;
}
*/
static joe_Object
checkKeyCS (joe_Object item, joe_Object theArray, const char * csKey)
{
   joe_String itmKey;
   unsigned int sibling;

   for ( ; ; ) {
      itmKey = *JOE_AT(item, KEY);
      if (!strcmp(joe_String_getCharStar (itmKey), csKey))
         return item;
      sibling = JOE_INT(*JOE_AT(item, SIBLING));
      if (sibling)
         item = *JOE_AT(theArray, sibling);
      else
         return 0;
   }
}

static joe_Object
checkKey (joe_Object item, joe_Object theArray, joe_String strKey)
{
   return checkKeyCS (item, theArray,joe_String_getCharStar (strKey));
}

static void
collision (joe_Object self, joe_Object strKey, joe_Object value,
           joe_Object *item, struct s_cself *cself, joe_Object *oldValue)
{

   joe_String itmKey = *JOE_AT(*item, KEY);
   if (!joe_String_compare (itmKey, strKey)) {
      joe_Object_assign(oldValue, *JOE_AT(*item, VALUE));
      joe_Object_assign(JOE_AT(*item, VALUE), value);
      joe_Object_delIfUnassigned (&strKey);
   } else {
      int sibling = JOE_INT (*JOE_AT(*item, SIBLING));
      if (sibling == 0) {
          joe_Object_assign (JOE_AT(*item, SIBLING),
                             joe_int_New (*(cself->lastSibling)));
          item = JOE_AT(*(cself->theArray), *(cself->lastSibling));
          (*(cself->lastSibling))++;
          joe_Object_assign (item, item_New (strKey, value));
      } else {
          item = JOE_AT(*(cself->theArray), sibling);
          collision (self, strKey, value, item, cself, oldValue);
      }
   }
}

static void
myput (joe_Object self, struct s_cself *cself,
       unsigned int hash, joe_String strKey, joe_Object value,
       joe_Object *oldValue)
{
   joe_Object *item;
   unsigned int idx;

   idx = hash % *(cself->len);
   item = JOE_AT(*(cself->theArray), idx);
   if (*item) {
      collision (self, strKey, value, item, cself, oldValue);
   } else {
      joe_Object_assign (item, item_New (strKey, value));
      (*(cself->nHash))++;
   }
}

static void
rehash (joe_HashMap self, struct s_cself *cself)
{
   int i;
   joe_Object oldValue = 0;
   joe_Object *item;
   joe_Object *strKey, *value;
   joe_Array oArray = 0;
   int oldLen = *(cself->totLen);

   joe_Object_assign (&oArray, *JOE_AT(self, ARRAY));
   allocArray (self, oldLen << 1);

   for (i = 0; i < oldLen; i++) {
      item = JOE_AT(oArray, i);
      if (*item) {
         strKey = JOE_AT(*item, KEY);
         value = JOE_AT(*item, VALUE);
         myput (self, cself, joe_HashMap_hash (*strKey),
                *strKey, *value, &oldValue);
         joe_Object_assign (&oldValue, 0);
      }
   }
   joe_Object_assign (&oArray, 0);
}

static void
joe_HashMap_putHash (joe_HashMap self, unsigned int hash,
                     joe_String strKey, joe_Object value,
                     joe_Object *oldValue)
{
   struct s_cself cself;

   joe_HashMap_fillStruct(self, &cself);

   while (*(cself.nHash) >= *(cself.threshold) ||
          *(cself.lastSibling) >= *(cself.totLen)) {
      rehash (self, &cself);
   }
   myput (self, &cself, hash, strKey, value, oldValue);
}

void
joe_HashMap_put (joe_HashMap self, joe_Object key, joe_Object value,
                 joe_Object *oldValue)
{
   joe_String strKey = 0;
   joe_Object_invoke (key, "toString", 0, 0, &strKey);
   joe_HashMap_putHash (self, joe_HashMap_hash(strKey), strKey, value, oldValue);
   joe_Object_assign (&strKey, 0);
   joe_Object_delIfUnassigned (&key);
}

int
joe_HashMap_getHashCS (joe_HashMap self, unsigned int hash,
                       const char* csKey, joe_Object *retval)
{
   if (self) {
      int len = JOE_INT (*JOE_AT(self, LEN));
      joe_Array theArray = *JOE_AT(self, ARRAY);
      unsigned int idx = hash % len;
      joe_Object item = *JOE_AT(theArray, idx);
      if (item) {
         item = checkKeyCS (item, theArray, csKey);
         if (item) {
            *retval = *JOE_AT(item, VALUE);
            return JOE_TRUE;
         }
      }
   }
   return JOE_FALSE;
}

int
joe_HashMap_getHash (joe_HashMap self, unsigned int hash,
                     joe_String strKey, joe_Object *retval)
{
   return joe_HashMap_getHashCS(self,hash,joe_String_getCharStar(strKey),retval);
}

int
joe_HashMap_getCS (joe_HashMap self, const char * csKey, joe_Object *retval)
{
   int Return = JOE_FALSE;
   if (self) {
      Return = joe_HashMap_getHashCS (self,
                                 joe_HashMap_hashUCS ((unsigned char *)csKey),
                                 csKey, retval);
   }
   return Return;
}

int
joe_HashMap_get (joe_HashMap self, joe_Object key, joe_Object *retval)
{
   int Return = JOE_FALSE;
   if (self) {
      joe_Object strKey = 0;

      joe_Object_invoke (key, "toString", 0, 0, &strKey);

      Return = joe_HashMap_getHash (self, joe_HashMap_hash (strKey),
                                    strKey, retval);

      joe_Object_assign (&strKey, 0);
   }
   return Return;
}

unsigned int
joe_HashMap_length (joe_HashMap self)
{
   long nHash = JOE_INT (*JOE_AT(self, N_HASH));
   long lastSib = JOE_INT (*JOE_AT(self, LAST_SIB));
   long len = JOE_INT (*JOE_AT(self, LEN));
   return nHash + lastSib - len;
}


static joe_Array
getArray (joe_HashMap self, int keyOrValue)
{
   joe_Object item = 0;
   joe_String obj = 0;
   int i, j;
   long nItems = joe_HashMap_length (self);
   long totLen = JOE_INT (*JOE_AT(self, TOT_LEN));
   joe_Array theArray = *JOE_AT(self, ARRAY);
   joe_Array Return = joe_Array_New (nItems);

   for (i = 0, j = 0; i < totLen; i++) {
      item = *JOE_AT(theArray, i);
      if (item) {
         obj = *JOE_AT(item, keyOrValue);
         joe_Object_assign (JOE_AT(Return, j), obj);
         j++;
      }
   }
   return Return;
}

joe_Array
joe_HashMap_keys (joe_HashMap self)
{
   return getArray (self, KEY);
}

joe_Array
joe_HashMap_values (joe_HashMap self)
{
   return getArray (self, VALUE);
}

int
joe_HashMap_containsHashKey (joe_HashMap self, unsigned int hash, joe_String key)
{
   int Return = JOE_FALSE;
   if (self) {
      joe_Array theArray = *JOE_AT(self, ARRAY);
      int len = JOE_INT (*JOE_AT(self, LEN));
      unsigned int idx = hash % len;
      joe_Object item = *JOE_AT(theArray, idx);
      if (item) {
         item = checkKey (item, theArray, key); 
         if (item)
            Return = JOE_TRUE;
      }
   }
   return Return;
}

int
joe_HashMap_containsKey (joe_HashMap self, joe_Object key)
{
   int Return = JOE_FALSE;
   joe_Object strKey = 0;

   joe_Object_invoke (key, "toString", 0, 0, &strKey);

   Return = joe_HashMap_containsHashKey (self,joe_HashMap_hash(strKey),strKey);

   joe_Object_assign (&strKey, 0);
   return Return;
}

int
joe_HashMap_containsValue (joe_HashMap self, joe_Object value)
{
   joe_Object item = 0;
   joe_String obj = 0;
   int i;
   joe_Object res = 0;
   long totLen = JOE_INT (*JOE_AT(self, TOT_LEN));
   joe_Array theArray = *JOE_AT(self, ARRAY);

   for (i = 0; i < totLen; i++) {
      item = *JOE_AT(theArray, i);
      if (item) {
         obj = *JOE_AT(item, VALUE);
         joe_Object_invoke (value, "equals", 1, &obj, &res);
         if (joe_Boolean_isTrue (res)) {
            joe_Object_assign (&res, 0);
            return JOE_TRUE;
         }
      }
   }
   return JOE_FALSE;
}

static int
ctor (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 0) {
      init (self, 12);
   } else if (argc == 1 && joe_Object_instanceOf (argv[0], &joe_Integer_Class)) {
      init (self, joe_Integer_value (argv[0]));
   } else {
      joe_Object_assign(retval,
                   joe_Exception_New ("HashMap ctor: invalid argument(s)"));
      return JOE_FAILURE;
   }
   return JOE_SUCCESS;
}

/**
## length
## size

Returns the length of this hash table.

*/

static int
length (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 0) {
      joe_Object_assign(retval, joe_Integer_New (joe_HashMap_length(self)));
   } else {
      joe_Object_assign(retval,
                   joe_Exception_New ("HashMap length: invalid argument(s)"));
      return JOE_FAILURE;
   }
   return JOE_SUCCESS;
}

/**
## put _aKey_, _aValue_

Insert _aValue_ in this hash table with key _aKey_.

Both _aKey_ and _aValue_ can be any kind of object, however for _aKey_
its String representation is used.

If no item with the specified key is in the table then the couple is added to
the table and the method returns () otherwise _aValue_ is substituted
to the previous value and the method returns the previous value.
*/

static int
put (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 2) {
      joe_Object oldValue = 0;
      joe_HashMap_put (self, argv[0], argv[1], &oldValue);
      if (oldValue == NULL)
          joe_Object_assign (retval, joe_Null_value);
      else
          joe_Object_transfer (retval, &oldValue);
   } else {
      joe_Object_assign(retval,
                   joe_Exception_New ("HashMap put: invalid argument(s)"));
      return JOE_FAILURE;
   }
   return JOE_SUCCESS;
}

/**
## get _aKey_

Look in this hash table for _aKey_.

If the key is found then the associated value is returned otherwise
 () is returned

*/

static int
get (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1) {
      joe_Object retv = 0;
      if (joe_HashMap_get (self, argv[0], &retv))
         joe_Object_assign (retval, retv);
      else
         joe_Object_assign (retval, joe_Null_value);
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval,
                   joe_Exception_New ("HashMap get: invalid argument(s)"));
      return JOE_FAILURE;
   }
}

/**
## keys
## getKeys

Returns a String array containing all the keys in this hash table.
*/

static int
keys (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 0) {
      joe_Object_assign (retval, getArray (self, KEY));
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval,
                   joe_Exception_New ("HashMap keys: invalid argument(s)"));
      return JOE_FAILURE;
   }
}

/**
## values

Returns an array containing all the values in this hash table.
*/

static int
values (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 0) {
      joe_Object_assign (retval, getArray (self, VALUE));
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval,
                   joe_Exception_New ("HashMap keys: invalid argument(s)"));
      return JOE_FAILURE;
   }
}

/**
## containsKey _aKey_

Look in this hash table for _aKey_.

If the key is found then Boolean <1> is returned otherwise <0> is returned.

*/

static int
containsKey (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1) {
      if (joe_HashMap_containsKey (self, argv[0]))
         joe_Object_assign (retval, joe_Boolean_True);
      else
         joe_Object_assign (retval, joe_Boolean_False);
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval,
             joe_Exception_New ("HashMap containsKey: invalid argument(s)"));
      return JOE_FAILURE;
   }
}

/**
## containsValue _aValue_

Look in this hash table for _aValue_.

If the value is found then Boolean <1> is returned otherwise <0> is returned.

*/

static int
containsValue (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1) {
      if (joe_HashMap_containsValue (self, argv[0]))
         joe_Object_assign (retval, joe_Boolean_True);
      else
         joe_Object_assign (retval, joe_Boolean_False);
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval,
             joe_Exception_New ("HashMap containsValue: invalid argument(s)"));
      return JOE_FAILURE;
   }
}

static joe_Method mthds[] = {
  {"containsValue", containsValue },
  {"containsKey",   containsKey },
  {"keys",  keys },
  {"keySet",keys },
  {"values",values },
  {"put",   put },
  {"get",   get },
  {"length",length },
  {"size",  length },
  {(void *) 0, (void *) 0}
};

joe_Class joe_HashMap_Class = {
   "joe_HashMap",
   ctor,
   0,
   mthds,
   variables,
   &joe_Object_Class,
   0
};

