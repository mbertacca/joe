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

# include "joe_HashMap.h"
# include "joe_Array.h"
# include "joe_Integer.h"
# include "joe_String.h"

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
   joe_Object_assign (joe_Object_at (self, KEY), strKey);
   joe_Object_assign (joe_Object_at (self, VALUE), value);
   joe_Object_assign (joe_Object_at (self, SIBLING), joe_Integer_New (0));
   return self;
}

static joe_Method mthds[] = {
  /* {"length", length } */
  {(void *) 0, (void *) 0}
};

# define ARRAY 0
# define N_HASH 1
# define LAST_SIB 2
# define LEN 3
# define TOT_LEN 4
# define THRESHOLD 5
static char *variables[] = { "array", "nHash", "lastSibling",
                             "len",   "totLen","threshold"};

joe_Class joe_HashMap_Class = {
   "joe_HashMap",
   0,
   0,
   mthds,
   variables,
   &joe_Object_Class,
   0
};

static joe_Array
allocArray (joe_HashMap self, unsigned int size)
{
   unsigned int len = size >> 1;
   joe_Array Return = joe_Array_New (size);

   joe_Object_assign(joe_Object_at(self,ARRAY), Return);
   joe_Object_assign(joe_Object_at(self,N_HASH),joe_Integer_New (0));
   joe_Object_assign(joe_Object_at(self,LEN), joe_Integer_New (len));
   joe_Object_assign(joe_Object_at(self,THRESHOLD), joe_Integer_New (len));
   joe_Object_assign(joe_Object_at(self,LAST_SIB), joe_Integer_New (len));
   joe_Object_assign(joe_Object_at(self,TOT_LEN), joe_Integer_New (size));

   return Return;
}

joe_Object
joe_HashMap_New (unsigned int size)
{
   joe_Object self;
   self = joe_Object_New (&joe_HashMap_Class, 0);
   allocArray (self, size<<1);

   return self;
}

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

static joe_Object
checkKey (joe_Object item, joe_Object array, joe_String strKey)
{
   joe_String itmKey;
   unsigned int sibling;

   for ( ; ; ) {
      itmKey = *joe_Object_at (item, KEY);
      if (!joe_String_compare (itmKey, strKey))
         return item;
      sibling = joe_Integer_value(*joe_Object_at (item, SIBLING));
      if (sibling)
         item = *joe_Object_at (array, sibling);
      else
         return 0;
   }
}

static joe_Object
collision (joe_Object self, joe_Object strKey, joe_Object value,
           joe_Object *item, long *lastSib)
{
   joe_Object *pVal;
   joe_Object oldValue = 0;

   joe_String itmKey = *joe_Object_at (*item, KEY);
   if (!joe_String_compare (itmKey, strKey)) {
      pVal = joe_Object_at (*item, VALUE);
      oldValue = *pVal;
      joe_Object_decrReference (&oldValue);
      *pVal = 0;
      joe_Object_assign (pVal, value);
      joe_Object_delIfUnassigned (&strKey);
   } else {
      joe_Array array = *joe_Object_at (self, ARRAY);
      long sibling = joe_Integer_value (*joe_Object_at (*item, SIBLING));
      if (sibling == 0) {
          joe_Object_assign (joe_Object_at (*item, SIBLING),
                             joe_Integer_New (*lastSib));
          item = joe_Object_at (array, *lastSib);
          (*lastSib)++;
          joe_Integer_addMe1 (*joe_Object_at (self, LAST_SIB));
          joe_Object_assign (item, item_New (strKey, value));
      } else {
          item = joe_Object_at (array, sibling);
          oldValue = collision (self, strKey, value, item, lastSib);
      }
   }
   return oldValue;
}

static joe_Object
myput (joe_Object self, joe_Object array, joe_Object strKey, joe_Object value,
       int len, long *lastSib)
{
   joe_Object Return;
   joe_Object *item;
   int idx;

   idx = hash (strKey) % len;
   item = joe_Object_at (array, idx);
   if (*item) {
      Return = collision (self, strKey, value, item, lastSib);
   } else {
      joe_Object_assign (item, item_New (strKey, value));
      joe_Integer_addMe1 (*joe_Object_at (self, N_HASH));
      Return = 0;
   }

   return Return;
}

static void
rehash (joe_HashMap self)
{
   int i;
   joe_Object *item;
   joe_Object *strKey, *value;
   joe_Array oArray = 0;
   joe_Array array = 0;
   long newLen;
   long lastSib;
   long totLen = joe_Integer_value (*joe_Object_at (self, TOT_LEN));

   joe_Object_assign (&oArray, *joe_Object_at (self, ARRAY));
   array = allocArray (self, totLen << 1); 
   newLen = joe_Integer_value (*joe_Object_at (self, LEN));
   lastSib = joe_Integer_value (*joe_Object_at (self, LAST_SIB));

   for (i = 0; i < totLen; i++) {
      item = joe_Object_at (oArray, i);
      if (*item) {
         strKey = joe_Object_at (*item, KEY);
         value = joe_Object_at (*item, VALUE);
         myput (self, array, *strKey, *value, newLen, &lastSib);
      }
   }
   joe_Object_assign (&oArray, 0);
}

joe_Object
joe_HashMap_put (joe_HashMap self, joe_Object key, joe_Object value)
{
   joe_Object Return;
   joe_String strKey = 0;
   long nHash = joe_Integer_value (*joe_Object_at (self, N_HASH));
   long threshold = joe_Integer_value (*joe_Object_at (self, THRESHOLD));
   long totLen = joe_Integer_value (*joe_Object_at (self, TOT_LEN));
   long lastSib = joe_Integer_value (*joe_Object_at (self, LAST_SIB));
   long len = joe_Integer_value (*joe_Object_at (self, LEN));
   joe_Array array = 0;

   joe_Object_assignInvoke (key, "toString", 0, 0, &strKey);

   while (nHash >= threshold || lastSib >= totLen) {
      rehash (self);
      nHash = joe_Integer_value (*joe_Object_at (self, N_HASH));
      threshold = joe_Integer_value (*joe_Object_at (self, THRESHOLD));
      len = joe_Integer_value (*joe_Object_at (self, LEN));
      totLen = joe_Integer_value (*joe_Object_at (self, TOT_LEN));
      lastSib = joe_Integer_value (*joe_Object_at (self, LAST_SIB));
   }
   array = *joe_Object_at (self, ARRAY);
   Return = myput (self, array, strKey, value, len, &lastSib);
   joe_Object_assign (&strKey, 0);
   joe_Object_delIfUnassigned (&key);
   return Return;
}

joe_Object
joe_HashMap_get (joe_HashMap self, joe_Object key)
{
   joe_Object item;
   unsigned int idx;
   joe_Object Return = 0;
   joe_Object strKey = 0;
   long len = joe_Integer_value (*joe_Object_at (self, LEN));
   joe_Array array = *joe_Object_at (self, ARRAY);

   joe_Object_invoke (key, "toString", 0, 0, &strKey);

   idx = hash (strKey) % len;
   item = *joe_Object_at (array, idx);
   if (item) {
      item = checkKey (item, array, strKey);
      if (item)
         Return = *joe_Object_at (item, VALUE);
   }
   joe_Object_delIfUnassigned (&strKey);
   return Return;
}

unsigned int
joe_HashMap_length (joe_HashMap self)
{
   long nHash = joe_Integer_value (*joe_Object_at (self, N_HASH));
   long lastSib = joe_Integer_value (*joe_Object_at (self, LAST_SIB));
   long len = joe_Integer_value (*joe_Object_at (self, LEN));
   return nHash + lastSib - len;
}


joe_Array
getArray (joe_HashMap self, int keyOrValue)
{
   joe_Object item = 0;
   joe_String obj = 0;
   int i, j;
   long nItems = joe_HashMap_length (self);
   long totLen = joe_Integer_value (*joe_Object_at (self, TOT_LEN));
   joe_Array array = *joe_Object_at (self, ARRAY);
   joe_Array Return = joe_Array_New (nItems);

   for (i = 0, j = 0; i < totLen; i++) {
      item = *joe_Object_at (array, i);
      if (item) {
         obj = *joe_Object_at (item, keyOrValue);
         joe_Object_assign (joe_Object_at (Return, j), obj);
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
joe_HashMap_containsKey (joe_HashMap self, joe_Object key)
{
   joe_Object item;
   unsigned int idx;
   int Return = 0;
   joe_Object strKey = 0;
   long len = joe_Integer_value (*joe_Object_at (self, LEN));
   joe_Array array = *joe_Object_at (self, ARRAY);

   joe_Object_invoke (key, "toString", 0, 0, &strKey);

   idx = hash (strKey) % len;
   item = *joe_Object_at (array, idx);
   if (item) {
      item = checkKey (item, array, strKey);
      if (item)
         Return = 1;
   }
   joe_Object_delIfUnassigned (&strKey);
   return Return;
}

int
joe_HashMap_containsValue (joe_HashMap self, joe_Object value)
{
   long lastSib = joe_Integer_value (*joe_Object_at (self, LAST_SIB));
   joe_Array array = *joe_Object_at (self, ARRAY);
   joe_Object item = 0;
   joe_String itmVal;
   unsigned int i;

   for (i = 0; i < lastSib; i++) {
      if ((item = *joe_Object_at (array, i))) {
         itmVal = *joe_Object_at (item, VALUE);
         if (itmVal == value)
             return 1;
      }
   }
   return 0;
}
