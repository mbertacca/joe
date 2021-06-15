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
   joe_Object_assign (joe_Object_at (self, SIBLING), joe_int_New (0));
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

struct s_cself {
   joe_Object* theArray;
   int* nHash;
   int* lastSibling;
   int* len;
   int* totLen;
   int* threshold;
};

static void
joe_HashMap_fillStruct(joe_HashMap self, struct s_cself *cself)
{
   joe_Object *selfVars = joe_Object_array (self);
   cself->theArray = &selfVars[ARRAY];
   cself->nHash = joe_int_starValue(selfVars[N_HASH]);
   cself->lastSibling = joe_int_starValue(selfVars[LAST_SIB]);
   cself->len = joe_int_starValue(selfVars[LEN]);
   cself->totLen = joe_int_starValue(selfVars[TOT_LEN]);
   cself->threshold = joe_int_starValue(selfVars[THRESHOLD]);
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
   joe_Array Return = joe_Array_New (size);
   struct s_cself cself;

   joe_Object_assign(joe_Object_at(self, ARRAY), Return);
   joe_HashMap_fillStruct(self, &cself);
   *cself.totLen = size;
   joe_HashMap_init(self, size, &cself);

   return Return;
}

joe_Object
joe_HashMap_New_size(unsigned int size)
{
   joe_Object self;
   self = joe_Object_New(&joe_HashMap_Class, 0);
   joe_Object_assign(joe_Object_at(self, N_HASH), joe_int_New0());
   joe_Object_assign(joe_Object_at(self, LEN), joe_int_New0());
   joe_Object_assign(joe_Object_at(self, THRESHOLD), joe_int_New0());
   joe_Object_assign(joe_Object_at(self, LAST_SIB), joe_int_New0());
   joe_Object_assign(joe_Object_at(self, TOT_LEN), joe_int_New0());
   allocArray(self, size << 1);

   return self;
}

joe_Object
joe_HashMap_New ()
{
   return joe_HashMap_New_size (12);
}

static unsigned int
hash(joe_String strKey)
{
   unsigned long hash = 5381;
   unsigned char *str = joe_String_getCharStar(strKey);
   int c;

   while (c = *str++)
      hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

   return hash;
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
checkKey (joe_Object item, joe_Object theArray, joe_String strKey)
{
   joe_String itmKey;
   unsigned int sibling;

   for ( ; ; ) {
      itmKey = *joe_Object_at (item, KEY);
      if (!joe_String_compare (itmKey, strKey))
         return item;
      sibling = joe_int_value(*joe_Object_at (item, SIBLING));
      if (sibling)
         item = *joe_Object_at (theArray, sibling);
      else
         return 0;
   }
}

static joe_Object
collision (joe_Object self, joe_Object strKey, joe_Object value,
           joe_Object *item, struct s_cself *cself)
{
   joe_Object oldValue = 0;

   joe_String itmKey = *joe_Object_at (*item, KEY);
   if (!joe_String_compare (itmKey, strKey)) {
      joe_Object_assign(joe_Object_at(*item, VALUE), value);
      joe_Object_delIfUnassigned (&strKey);
   } else {
      int sibling = joe_int_value (*joe_Object_at (*item, SIBLING));
      if (sibling == 0) {
          joe_Object_assign (joe_Object_at (*item, SIBLING),
                             joe_int_New (*(cself->lastSibling)));
          item = joe_Object_at (*(cself->theArray), *(cself->lastSibling));
          (*(cself->lastSibling))++;
          joe_Object_assign (item, item_New (strKey, value));
      } else {
          item = joe_Object_at (*(cself->theArray), sibling);
          oldValue = collision (self, strKey, value, item, cself);
      }
   }
   return oldValue;
}

static joe_Object
myput (joe_Object self, struct s_cself *cself, joe_Object strKey, joe_Object value)
{
   joe_Object Return;
   joe_Object *item;
   int idx;

   idx = hash (strKey) % *(cself->len);
   item = joe_Object_at (*(cself->theArray), idx);
   if (*item) {
      Return = collision (self, strKey, value, item, cself);
   } else {
      joe_Object_assign (item, item_New (strKey, value));
      (*(cself->nHash))++;
      Return = 0;
   }

   return Return;
}

static void
rehash (joe_HashMap self, struct s_cself *cself)
{
   int i;
   joe_Object *item;
   joe_Object *strKey, *value;
   joe_Array oArray = 0;
   int oldLen = *(cself->totLen);

   joe_Object_assign (&oArray, *joe_Object_at (self, ARRAY));
   allocArray (self, oldLen << 1);

   for (i = 0; i < oldLen; i++) {
      item = joe_Object_at (oArray, i);
      if (*item) {
         strKey = joe_Object_at (*item, KEY);
         value = joe_Object_at (*item, VALUE);
         myput (self, cself, *strKey, *value);
      }
   }
   joe_Object_assign (&oArray, 0);
}

joe_Object
joe_HashMap_put (joe_HashMap self, joe_Object key, joe_Object value)
{
   joe_Object Return;
   joe_String strKey = 0;
   struct s_cself cself;

   joe_HashMap_fillStruct(self, &cself);

   joe_Object_invoke (key, "toString", 0, 0, &strKey);

   while (*(cself.nHash) >= *(cself.threshold) ||
          *(cself.lastSibling) >= *(cself.totLen)) {
      rehash (self, &cself);
   }
   Return = myput (self, &cself, strKey, value);
   joe_Object_assign (&strKey, 0);
   joe_Object_delIfUnassigned (&key);
   return Return;
}

int
joe_HashMap_get (joe_HashMap self, joe_Object key, joe_Object *retval)
{
   joe_Object item;
   unsigned int idx;
   int Return = 0; /* false */
   joe_Object strKey = 0;
   int len = joe_int_value (*joe_Object_at (self, LEN));
   joe_Array theArray = *joe_Object_at (self, ARRAY);

   joe_Object_invoke (key, "toString", 0, 0, &strKey);

   idx = hash (strKey) % len;
   item = *joe_Object_at (theArray, idx);
   if (item) {
      item = checkKey (item, theArray, strKey);
      if (item) {
         *retval = *joe_Object_at (item, VALUE);
         Return = 1;/* true */;
      }
   }
   joe_Object_assign (&strKey, 0);
   return Return;
}

unsigned int
joe_HashMap_length (joe_HashMap self)
{
   long nHash = joe_int_value (*joe_Object_at (self, N_HASH));
   long lastSib = joe_int_value (*joe_Object_at (self, LAST_SIB));
   long len = joe_int_value (*joe_Object_at (self, LEN));
   return nHash + lastSib - len;
}


static joe_Array
getArray (joe_HashMap self, int keyOrValue)
{
   joe_Object item = 0;
   joe_String obj = 0;
   int i, j;
   long nItems = joe_HashMap_length (self);
   long totLen = joe_int_value (*joe_Object_at (self, TOT_LEN));
   joe_Array theArray = *joe_Object_at (self, ARRAY);
   joe_Array Return = joe_Array_New (nItems);

   for (i = 0, j = 0; i < totLen; i++) {
      item = *joe_Object_at (theArray, i);
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
   int len = joe_int_value (*joe_Object_at (self, LEN));
   joe_Array theArray = *joe_Object_at (self, ARRAY);

   joe_Object_invoke (key, "toString", 0, 0, &strKey);

   idx = hash (strKey) % len;
   item = *joe_Object_at (theArray, idx);
   if (item) {
      item = checkKey (item, theArray, strKey);
      if (item)
         Return = 1;
   }
   joe_Object_assign (&strKey, 0);
   return Return;
}

int
joe_HashMap_containsValue (joe_HashMap self, joe_Object value)
{
   unsigned int lastSib = joe_int_value (*joe_Object_at (self, LAST_SIB));
   joe_Array theArray = *joe_Object_at (self, ARRAY);
   joe_Object item = 0;
   joe_String itmVal;
   unsigned int i;

   for (i = 0; i < lastSib; i++) {
      if ((item = *joe_Object_at (theArray, i))) {
         itmVal = *joe_Object_at (item, VALUE);
         if (itmVal == value)
             return 1;
      }
   }
   return 0;
}
