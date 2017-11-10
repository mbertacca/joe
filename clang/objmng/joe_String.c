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

# include <string.h>
# include <ctype.h>
# include "joe_Boolean.h"
# include "joe_Integer.h"
# include "joe_String.h"
# include "joe_Exception.h"

static int
add (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1) {
      char **str = 0;
      char *str1;
      char *str2;
      joe_String tmp = 0;
      if (!joe_Object_instanceOf(argv[0], &joe_String_Class)) {
         if (argv[0])
             joe_Object_assignInvoke (argv[0], "toString", 0, 0, &tmp);
          else
             joe_Object_assign (&tmp, joe_String_New ("(null)"));
          str2 = *((char **) joe_Object_getMem(tmp));
      } else {
          str2 = *((char **) joe_Object_getMem(argv[0]));
      }

      str1 = *((char **) joe_Object_getMem(self));
      *retval = joe_Object_New (&joe_String_Class, strlen(str1)+strlen(str2)+1);
      str = (char **) joe_Object_getMem(*retval);
      strcat (*str, str1);
      strcat (*str, str2);
      if (tmp)
          joe_Object_assign (&tmp, 0);
    
      return JOE_SUCCESS;
   } else {
      *retval = joe_Exception_New ("invalid argument");
      return JOE_FAILURE;
   }
}

static int
length (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 0) {
      *retval = joe_Integer_New (joe_String_length(self));
      return JOE_SUCCESS;
   } else {
      *retval = joe_Exception_New ("length: invalid argument");
      return JOE_FAILURE;
   }
}

static int
toLowerCase (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 0) {
      char *c;
      *retval = joe_String_New (joe_String_getCharStar (self));
      c = joe_String_getCharStar (*retval);
      for ( ; *c; c++)
         *c = tolower (*c);
      return JOE_SUCCESS;
   } else {
      *retval = joe_Exception_New ("toLowerCase: invalid argument");
      return JOE_FAILURE;
   }
}

static int
toUpperCase (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 0) {
      char *c;
      *retval = joe_String_New (joe_String_getCharStar (self));
      c = joe_String_getCharStar (*retval);
      for ( ; *c; c++)
         *c = toupper (*c);
      return JOE_SUCCESS;
   } else {
      *retval = joe_Exception_New ("toUpperCase: invalid argument");
      return JOE_FAILURE;
   }
}

static int
substring (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc>0 && argc<3 && joe_Object_instanceOf(argv[0], &joe_Integer_Class)) {
      unsigned int selfLen = joe_String_length (self);
      unsigned int beginIndex = joe_Integer_value (argv[0]);
      if (argc == 1) {
         if (beginIndex < selfLen) {
            *retval=joe_String_New(&joe_String_getCharStar (self)[beginIndex]);
         } else {
            *retval=joe_String_New("");
         }
         return JOE_SUCCESS;
      } else if (argc==2 && joe_Object_instanceOf(argv[1],&joe_Integer_Class)) {
         int newLen = joe_Integer_value (argv[1]) - beginIndex;
         if (beginIndex < selfLen && newLen > 0) {
            *retval = joe_String_New_len(
                          &joe_String_getCharStar (self)[beginIndex],newLen);
         } else {
            *retval=joe_String_New("");
         }
         return JOE_SUCCESS;
      }
   }

   *retval = joe_Exception_New ("substring: invalid argument");
   return JOE_FAILURE;
}

static int
toString (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   *retval = self;
   return JOE_SUCCESS;
}

static int
equals (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1 && joe_Object_instanceOf(argv[0], &joe_String_Class)
                 && joe_String_compare (self, argv[0]) == 0)
      *retval = joe_Boolean_New_true();
   else
      *retval = joe_Boolean_New_false();
   return JOE_SUCCESS;
}

static int
ne (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1 && joe_Object_instanceOf(argv[0], &joe_String_Class)) {
      if (joe_String_compare (self, argv[0]) != 0)
         *retval = joe_Boolean_New_true();
      else
         *retval = joe_Boolean_New_false();
      return JOE_SUCCESS;
   } else {
      *retval = joe_Exception_New ("String <>: invalid argument");
      return JOE_FAILURE;
   }
}

static int
ge (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1 && joe_Object_instanceOf(argv[0], &joe_String_Class)) {
      if (joe_String_compare (self, argv[0]) >= 0)
         *retval = joe_Boolean_New_true();
      else
         *retval = joe_Boolean_New_false();
      return JOE_SUCCESS;
   } else {
      *retval = joe_Exception_New ("String >=: invalid argument");
      return JOE_FAILURE;
   }
}

static int
gt (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1 && joe_Object_instanceOf(argv[0], &joe_String_Class)) {
      if (joe_String_compare (self, argv[0]) > 0)
         *retval = joe_Boolean_New_true();
      else
         *retval = joe_Boolean_New_false();
      return JOE_SUCCESS;
   } else {
      *retval = joe_Exception_New ("String >: invalid argument");
      return JOE_FAILURE;
   }
}

static int
le (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1 && joe_Object_instanceOf(argv[0], &joe_String_Class)) {
      if (joe_String_compare (self, argv[0]) <= 0)
         *retval = joe_Boolean_New_true();
      else
         *retval = joe_Boolean_New_false();
      return JOE_SUCCESS;
   } else {
      *retval = joe_Exception_New ("String <=: invalid argument");
      return JOE_FAILURE;
   }
}

static int
lt (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1 && joe_Object_instanceOf(argv[0], &joe_String_Class)) {
      if (joe_String_compare (self, argv[0]) < 0)
         *retval = joe_Boolean_New_true();
      else
         *retval = joe_Boolean_New_false();
      return JOE_SUCCESS;
   } else {
      *retval = joe_Exception_New ("String <: invalid argument");
      return JOE_FAILURE;
   }
}

static joe_Method mthds[] = {
  {"add", add},
  {"equals", equals},
  {"ne", ne},
  {"ge", ge},
  {"gt", gt},
  {"le", le},
  {"lt", lt},
  {"length", length},
  {"substring", substring},
  {"toLowerCase", toLowerCase},
  {"toUpperCase", toUpperCase},
  {"toString", toString},
  {(void *) 0, (void *) 0}
};

joe_Class joe_String_Class = {
   "joe_String",
   0,
   0,
   mthds,
   0,
   &joe_Object_Class,
   0
};

joe_Object
joe_String_New_len (char *c, unsigned int len) {
   joe_Object self;

   self = joe_Object_New (&joe_String_Class, len + 1);
   strncpy (*((char **) joe_Object_getMem(self)), c, len);
   return self;
}

joe_Object
joe_String_New (const char *c) {
   int len = strlen (c);
   joe_Object self;

   self = joe_Object_New (&joe_String_Class, len + 1);
   strcpy (*((char **) joe_Object_getMem(self)), c);
   return self;
}

joe_Object
joe_String_New2 (const char *c1, const char *c2) {
   int len = strlen (c1) + strlen (c2);
   joe_Object self;

   self = joe_Object_New (&joe_String_Class, len + 1);
   strcpy (*((char **) joe_Object_getMem(self)), c1);
   strcat (*((char **) joe_Object_getMem(self)), c2);
   return self;
}

joe_Object
joe_String_New3 (const char *c1, const char *c2, const char *c3) {
   int len = strlen (c1) + strlen (c2) + strlen(c3);
   joe_Object self;

   self = joe_Object_New (&joe_String_Class, len + 1);
   strcpy (*((char **) joe_Object_getMem(self)), c1);
   strcat (*((char **) joe_Object_getMem(self)), c2);
   strcat (*((char **) joe_Object_getMem(self)), c3);
   return self;
}

joe_Object
joe_String_New4(const char *c1,const char *c2,const char *c3,const char *c4) {
   int len = strlen (c1) + strlen (c2) + strlen(c3) + strlen (c4);
   joe_Object self;

   self = joe_Object_New (&joe_String_Class, len + 1);
   strcpy (*((char **) joe_Object_getMem(self)), c1);
   strcat (*((char **) joe_Object_getMem(self)), c2);
   strcat (*((char **) joe_Object_getMem(self)), c3);
   strcat (*((char **) joe_Object_getMem(self)), c4);
   return self;
}
 
char *
joe_String_getCharStar (joe_String self)
{
   return *((char **) joe_Object_getMem(self));
}

int
joe_String_compare (joe_String self, joe_String aStr)
{
   if (self)
      if (aStr)
         return strcmp (*((char **) joe_Object_getMem(self)),
                        *((char **) joe_Object_getMem(aStr)));
     else
        return 1;
   else
      if (aStr)
         return -1;
      else
         return 0;
}

unsigned int
joe_String_length (joe_String self)
{
   return strlen (*((char **) joe_Object_getMem(self)));
}
