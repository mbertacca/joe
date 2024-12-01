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
# include <stdlib.h>
# include "joe_Boolean.h"
# include "joe_Float.h"
# include "joe_Integer.h"
# include "joe_BigDecimal.h"
# include "joe_String.h"
# include "joe_ArrayList.h"
# include "joe_StringBuilder.h"
# include "joe_Null.h"
# include "joe_Exception.h"
# include "joestrct.h"

/* Basic regex matcher
   supported character: * ? + [ ^ ] .
   regex starting with (?i) makes the matching case-insensitive
 */

# define DOT ((unsigned char)'\x00')
static char DOTSTAR[] = { DOT, 0 };
#define chrcmp(c1,c2,ic) (ic ? toupper(c1) == toupper(c2) : c1 == c2)

#define rngcmp(c,r1,r2,ic) (ic && isalpha(c) ? \
                            ((r1 <= toupper(c) && toupper(c) <= r2)||  \
                             (r1 <= tolower(c) && tolower(c) <= r2)  ) \
                             : r1 <= c && c <= r2)

static int
ismatch (char *s, char *m, int ic)
{
   switch (*m) {
   case DOT:
      return 1;
   case '[':
      {
         int negate = 0;
         int correct = 0;
         int result = 0;
         char r0 = 0;

         m++;
         if (*m == '^') {
            r0 = *m;
            m++;
            if (*m == ']') {
               correct = 1;
               result = chrcmp(*s,r0,ic);
            } else {
               negate = 1;
            }
         }
         while (*m && !correct) {
            if (*m == '\\')
               m++;
            r0 = *m;
            m++;
            if (*m == ']') {
               correct = 1;
               if (chrcmp(r0,*s,ic))
                  result = 1;
            } else if (*m == '-') {
               m++;
               if (*m == ']') {
                  if (*s == '-' || chrcmp(*s,r0,ic))
                     result = 1;
                  correct = 1;
               } else {
                  if (*m == '\\')
                     m++;
                  if (rngcmp(*s,r0,*m,ic))
                     result = 1;
               }
            } else {
               if (chrcmp(r0,*s,ic))
                  result = 1;
            }
         }
         return negate ^ result;
      }
   default:
      return chrcmp(*m,*s,ic);
   }
}

static int
r_matches (char **s, char*  m, int ic)
{
   char *bgn = *s;
   char *lm = 0;
   char *fx = 0;
   char *bx = 0;

   while (*m) {
      switch (*m) {
      case '?':
         m++;
         if (*m == '?')
            m++;
         if (*m == 0) {
            ++(*s);
            return (bgn != *s);
         }
         fx = *s;
         if (r_matches (s, m, ic) && **s == 0)
            return (bgn != *s);
          else
             *s = fx;
          if (lm && ismatch (*s, lm, ic)) {
             (*s)++;
         }
         break;
      case '+':
      case '*':
         for (m++; *m == '*' || *m == '+'; m++)
            ;
         if (*m == 0) {
            for ( ; **s != 0 && ismatch (*s, lm, ic); (*s)++)
               ;
            return (bgn != *s);
         }
         for (bx = 0; **s != 0; (*s)++) {
            fx = *s;
            if (r_matches (s, m, ic) && **s == 0) {
               if (!bx)
                  bx = *s;
               else if (*s - bx > 0)
                  bx = *s;
            } else {
               *s = fx;
            }
            if (!ismatch (*s, lm, ic)) {
               break;
            }
            if (!**s)
               break;
         }
         if (bx) {
            *s = bx;
            return (bgn != *s);
         } 
         break;
      case '.':
         if (**s) {
            lm = DOTSTAR;
            if (*(++m) != '*' && *(m) != '?') {
               (*s)++;
            }
         } else if (*(++m) != '*' && *(m) != '?'){
            return 0;
         } else {
            ++m;
         }
         break;
      case '[':
         lm = m;
         for (m++; *m != 0; m++)
            if (*m == '\\') {
               m++;
               if (*m == 0)
                  break;
            } else if (*m == ']') {
               break;
            }
         if (*m != ']') {
            return 0;
         } else {
            if (!ismatch (*s, lm, ic)) {
               if (*(++m) == '*' || *(m) == '?') {
                  ++m;
               } else {
                  return 0;
               }
            } else {
               if (*(++m) != '*' && *(m) != '?') {
                  (*s)++;
               }
            }
         }
         break;
      case '\\':
         m++;
         if (*m == 0)
            break;
         // PASSTHRU
      default:
         lm = m;
         if (!chrcmp(*m,**s,ic)) {
            if (*(++m) == '*' || *(m) == '?') {
               ++m;
            } else
               return 0;
         } else {
            if (*(++m) != '*' && *(m) != '?') {
               (*s)++;
            }
         }
         break;
      }
   }
   return (*m == 0);
}

/* */

static int
add (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1) {
      char *str;
      char *str1;
      char *str2;
      joe_String tmp = 0;
      if (!JOE_ISCLASS(argv[0], &joe_String_Class)) {
         if (argv[0])
             joe_Object_invoke (argv[0], "toString", 0, 0, &tmp);
          else
             joe_Object_assign (&tmp, joe_String_New ("(null)"));
          str2 = JOE_CHAR_STAR(tmp);
      } else {
          str2 = JOE_CHAR_STAR(argv[0]);
      }

      str1 = JOE_CHAR_STAR(self);
      joe_Object_assign (retval,
             joe_Object_New (&joe_String_Class, strlen(str1)+strlen(str2)+1));
      str = JOE_CHAR_STAR(*retval);
      strcat (str, str1);
      strcat (str, str2);
      if (tmp)
          joe_Object_assign (&tmp, 0);
    
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval, joe_Exception_New ("joe_String add: invalid argument"));
      return JOE_FAILURE;
   }
}

static int
length (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 0) {
      joe_Object_assign(retval, joe_Integer_New (joe_String_length(self)));
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval, joe_Exception_New ("length: invalid argument"));
      return JOE_FAILURE;
   }
}

static int
indexOf (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1 && JOE_ISCLASS(argv[0], &joe_String_Class)) {
      char *hs = joe_String_getCharStar (self);
      char *delta = strstr (hs, joe_String_getCharStar (argv[0]));
      if (delta) {
         joe_Object_assign(retval, joe_Integer_New (delta - hs));
      } else {
         joe_Object_assign(retval, joe_Integer_New (-1));
      }
      return JOE_SUCCESS;
   } else if (argc == 2 && JOE_ISCLASS(argv[0], &joe_String_Class) &&
                           JOE_ISCLASS(argv[1], &joe_Integer_Class)) {
      unsigned int beginIndex = joe_Integer_value (argv[1]);
      char *hs = joe_String_getCharStar (self);
      char *delta = strstr (hs  + beginIndex, joe_String_getCharStar (argv[0]));
      if (delta) {
         joe_Object_assign(retval, joe_Integer_New (delta - hs));
      } else {
         joe_Object_assign(retval, joe_Integer_New (-1));
      }
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval, joe_Exception_New ("indexOf: invalid argument"));
      return JOE_FAILURE;
   }
}

static char *
strrstr (char *haystack, const char *needle)
{
   char *Return;
   if (*needle) {
      int lenNeedle = strlen (needle);
      int lenHaystack = strlen (haystack);
      char *ptr = haystack + lenHaystack - lenNeedle;
      Return = NULL;
      while (ptr >= haystack) {
         if (strstr (ptr, needle)) {
            Return = ptr;
            break;
         }
         ptr--;
      } 
   } else {
      Return = haystack;
   }
   return Return;
}

static int
lastIndexOf (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1 && JOE_ISCLASS(argv[0], &joe_String_Class)) {
      char *hs = joe_String_getCharStar (self);
      char *delta = strrstr (hs, joe_String_getCharStar (argv[0]));
      if (delta) {
         joe_Object_assign(retval, joe_Integer_New (delta - hs));
      } else {
         joe_Object_assign(retval, joe_Integer_New (-1));
      }
      return JOE_SUCCESS;
   } else if (argc == 2 && JOE_ISCLASS(argv[0], &joe_String_Class) &&
                           JOE_ISCLASS(argv[1], &joe_Integer_Class)) {
      unsigned int beginIndex = joe_Integer_value (argv[1]);
      char *hs = joe_String_getCharStar (self);
      char *delta = strrstr (hs  + beginIndex, joe_String_getCharStar (argv[0]));
      if (delta) {
         joe_Object_assign(retval, joe_Integer_New (delta - hs));
      } else {
         joe_Object_assign(retval, joe_Integer_New (-1));
      }
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval, joe_Exception_New ("indexOf: invalid argument"));
      return JOE_FAILURE;
   }
}


static int
toLowerCase (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 0) {
      char *c;
      joe_Object_assign(retval, joe_String_New (joe_String_getCharStar (self)));
      c = joe_String_getCharStar (*retval);
      for ( ; *c; c++)
         *c = tolower (*c);
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval, joe_Exception_New (
                                "joe_String toLowerCase: invalid argument"));
      return JOE_FAILURE;
   }
}

static int
toUpperCase (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 0) {
      char *c;
      joe_Object_assign(retval, joe_String_New (joe_String_getCharStar (self)));
      c = joe_String_getCharStar (*retval);
      for ( ; *c; c++)
         *c = toupper (*c);
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval, joe_Exception_New (
                                "joe_String toUpperCase: invalid argument"));
      return JOE_FAILURE;
   }
}

static int
substring (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc>0 && argc<3 && JOE_ISCLASS(argv[0], &joe_Integer_Class)) {
      unsigned int selfLen = joe_String_length (self);
      unsigned int beginIndex = joe_Integer_value (argv[0]);
      if (argc == 1) {
         if (beginIndex < selfLen) {
            joe_Object_assign(retval, joe_String_New(
                              &joe_String_getCharStar (self)[beginIndex]));
         } else {
            joe_Object_assign(retval, joe_String_New(""));
         }
         return JOE_SUCCESS;
      } else if (argc==2 && JOE_ISCLASS(argv[1],&joe_Integer_Class)) {
         int newLen = joe_Integer_value (argv[1]) - beginIndex;
         if (beginIndex < selfLen && newLen > 0) {
            joe_Object_assign(retval, joe_String_New_len(
                              &joe_String_getCharStar (self)[beginIndex],newLen));
         } else {
            joe_Object_assign(retval, joe_String_New(""));
         }
         return JOE_SUCCESS;
      }
   }

   joe_Object_assign(retval, joe_Exception_New (
                             "joe_String substring: invalid argument"));
   return JOE_FAILURE;
}

static int
trim(joe_Object self, int argc, joe_Object* argv, joe_Object* retval)
{
   if (argc == 0) {
      unsigned int selfLen = joe_String_length(self);
      char *strt =  joe_String_getCharStar(self);
      char *end =  strt + selfLen - 1;
      unsigned int trimLen = selfLen;
      while (*strt <= ' ' && trimLen) {
          trimLen--;
          strt++;
      }
      while (*end <= ' ' && trimLen) {
          trimLen--;
          end--;
      }
      joe_Object_assign(retval, joe_String_New_len(strt, trimLen));
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval, joe_Exception_New(
                               "joe_String trim: invalid argument"));
      return JOE_FAILURE;
   }
}



static int
charAt(joe_Object self, int argc, joe_Object* argv, joe_Object* retval)
{
   if (argc == 1 && JOE_ISCLASS(argv[0], &joe_Integer_Class)) {
      unsigned int selfLen = joe_String_length(self);
      unsigned int index = joe_Integer_value(argv[0]);
      if (index < selfLen) {
         joe_Object_assign(retval, joe_String_New_len(
                                   &joe_String_getCharStar(self)[index], 1));
      } else {
         joe_Object_assign(retval, joe_String_New(""));
      }
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval, joe_Exception_New(
                               "joe_String charAt: invalid argument"));
      return JOE_FAILURE;
   }
}

static int
startsWith(joe_Object self, int argc, joe_Object* argv, joe_Object* retval)
{
   if (argc == 1 && JOE_ISCLASS(argv[0], &joe_String_Class)) {
      unsigned int selfLen = joe_String_length(self);
      unsigned int arglen = joe_String_length(argv[0]);
      if (arglen <= selfLen) {
          if (strncmp (joe_String_getCharStar(self),
                       joe_String_getCharStar(argv[0]), arglen) == 0)
             joe_Object_assign(retval, joe_Boolean_New_true());
          else
             joe_Object_assign(retval, joe_Boolean_New_false());
      } else {
         joe_Object_assign(retval, joe_Boolean_New_false());
      }
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval, joe_Exception_New(
                                "joe_String startsWith: invalid argument"));
         return JOE_FAILURE;
   }
}


static int
toString (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   joe_Object_assign (retval, self);
   return JOE_SUCCESS;
}

static int
equals (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1 && JOE_ISCLASS(argv[0], &joe_String_Class)
                 && joe_String_compare (self, argv[0]) == 0)
      joe_Object_assign(retval, joe_Boolean_New_true());
   else
      joe_Object_assign(retval, joe_Boolean_New_false());
   return JOE_SUCCESS;
}

static int
equalsIgnoreCase (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1 && JOE_ISCLASS(argv[0], &joe_String_Class)) {
      char *c1 = joe_String_getCharStar (self);
      char *c2 = joe_String_getCharStar (argv[0]);
      for ( ; *c1 && c2; c1++, c2++) {
         if (toupper(*c1) != toupper (*c2)) {
            break;
         }
      }
      if (*c1 == *c2) 
         joe_Object_assign(retval, joe_Boolean_New_true());
      else
         joe_Object_assign(retval, joe_Boolean_New_false());
   } else
      joe_Object_assign(retval, joe_Boolean_New_false());
   return JOE_SUCCESS;
}


static int
ne (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1 && JOE_ISCLASS(argv[0], &joe_String_Class)) {
      if (joe_String_compare (self, argv[0]) != 0)
         joe_Object_assign(retval, joe_Boolean_New_true());
      else
         joe_Object_assign(retval, joe_Boolean_New_false());
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval, joe_Exception_New ("joe_String <>: invalid argument"));
      return JOE_FAILURE;
   }
}

static int
ge (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1 && JOE_ISCLASS(argv[0], &joe_String_Class)) {
      if (joe_String_compare (self, argv[0]) >= 0)
         joe_Object_assign(retval, joe_Boolean_New_true());
      else
         joe_Object_assign(retval, joe_Boolean_New_false());
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval, joe_Exception_New ("joe_String >=: invalid argument"));
      return JOE_FAILURE;
   }
}

static int
gt (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1 && JOE_ISCLASS(argv[0], &joe_String_Class)) {
      if (joe_String_compare (self, argv[0]) > 0)
         joe_Object_assign(retval, joe_Boolean_New_true());
      else
         joe_Object_assign(retval, joe_Boolean_New_false());
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval, joe_Exception_New ("joe_String >: invalid argument"));
      return JOE_FAILURE;
   }
}

static int
le (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1 && JOE_ISCLASS(argv[0], &joe_String_Class)) {
      if (joe_String_compare (self, argv[0]) <= 0)
         joe_Object_assign(retval, joe_Boolean_New_true());
      else
         joe_Object_assign(retval, joe_Boolean_New_false());
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval, joe_Exception_New ("joe_String <=: invalid argument"));
      return JOE_FAILURE;
   }
}

static int
lt (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1 && JOE_ISCLASS(argv[0], &joe_String_Class)) {
      if (joe_String_compare (self, argv[0]) < 0)
         joe_Object_assign(retval, joe_Boolean_New_true());
      else
         joe_Object_assign(retval, joe_Boolean_New_false());
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval, joe_Exception_New ("joe_String <: invalid argument"));
      return JOE_FAILURE;
   }
}

static int
matches (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1 && JOE_ISCLASS(argv[0], &joe_String_Class)) {
      char * str = joe_String_getCharStar (self);
      char * re = joe_String_getCharStar (argv[0]);
      int ic = 0;
      if (re[0] == '(' && re[1] == '?' && re[2] == 'i' && re[3] == ')') {
         re+=4;
         ic = 1;
      }
      if (*re == '*' || *re == '+'|| *re == '?')
         joe_Object_assign(retval, joe_Boolean_New_false());
      else
         if (r_matches (&str, re, ic) && *str == 0)
            joe_Object_assign(retval, joe_Boolean_New_true());
         else
            joe_Object_assign(retval, joe_Boolean_New_false());
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval, joe_Exception_New (
                               "joe_String matches: invalid argument"));
      return JOE_FAILURE;
   }
}

static int
split (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1 && JOE_ISCLASS(argv[0], &joe_String_Class)) {
      char * const str = joe_String_getCharStar (self);
      char * re = joe_String_getCharStar (argv[0]);
      joe_ArrayList al = 0;
      int ic = 0;
      if (re[0] == '(' && re[1] == '?' && re[2] == 'i' && re[3] == ')') {
         re+=4;
         ic = 1;
      }

      joe_Object_assign(&al, joe_ArrayList_New(4));
      if (*re == '*' || *re == '+'|| *re == '?')
          joe_ArrayList_add(al, self);
      else if (!*re) {
         char *sx = str;
         while (*sx) {
            joe_ArrayList_add(al, joe_String_New_len (sx, 1));
            sx++;
         }
      } else {
         char *s0 = str;
         char *st = str;
         char *sx = str;
         int len;
         while (*sx) {
           if (r_matches (&sx, re, ic) && sx != st) {
              len = st - s0;
              if (len > 0 || *sx) /* do not add the final 0 length sep */
                 joe_ArrayList_add(al, joe_String_New_len (s0, len));
              s0 = st = sx;
            } else {
              sx = ++st;
            }
         }
         if (s0 != st) {
            len = st - s0;
            joe_ArrayList_add(al, joe_String_New_len (s0, len));
         }
      }
      joe_ArrayList_toArray (al, retval);
      joe_Object_assign (&al, 0);
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval, joe_Exception_New (
                               "joe_String split: invalid argument"));
      return JOE_FAILURE;
   }
}

static joe_String
replace (joe_String self, char *are, char *rplcmnt, int firstOnly)
{
   joe_String Return = self;
   int ic = 0;
   char * const str = joe_String_getCharStar (self);
   joe_StringBuilder sb = 0;
   char *re0 = strdup (are);
   char *re = re0;
   int anchStrt = 0;
   int anchEnd = 0;
   int reLen;
   if (re[0] == '(' && re[1] == '?' && re[2] == 'i' && re[3] == ')') {
      re+=4;
      ic = 1;
   }
   if (*re == '^') {
      re++;
      anchStrt = 1;
   }
   reLen = strlen (re);
   if (re[reLen-1] == '$' && reLen > 1 && re[reLen-2] != '\\') {
      anchEnd = 1;
      re[reLen-1] = 0;
      reLen--;
   }
   if (*re != '*' && *re != '+' && *re != '?') {
      char *s0 = str;
      char *st = str;
      char *sx = str;
      int len0;
      while (*sx) {
         if (r_matches (&sx, re, ic) && sx != st) {
            len0 = st - s0;
            if (sb == 0) {
               joe_Object_assign (&sb, joe_StringBuilder_New ());
            }
            if (len0 > 0)
               joe_StringBuilder_appendCharStar_len(sb, s0, len0);
            if ((!anchStrt || s0 == str) && (!anchEnd  || *sx == 0))
               joe_StringBuilder_appendCharStar(sb, rplcmnt);
            else
               joe_StringBuilder_appendCharStar_len(sb, st, (sx - st));
            s0 = st = sx;
            if (firstOnly) {
               while (*st)
                 st++;
               sx = st;
            }
         } else {
            sx = ++st;
         }
      }
      if (sb != 0) {
         len0 = st - s0;
         if (len0 > 0) {
            joe_StringBuilder_appendCharStar_len(sb, s0, len0);
         }
         Return = joe_StringBuilder_toString (sb);
         joe_Object_assign (&sb, 0);
      }
   }
   free (re0);
   return Return;
}

static int
replaceAll (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 2 && JOE_ISCLASS(argv[0], &joe_String_Class)
                 && JOE_ISCLASS(argv[1], &joe_String_Class)) {
      char * re = joe_String_getCharStar (argv[0]);
      char * rplcmnt = joe_String_getCharStar (argv[1]);
      
      joe_Object_assign(retval, replace (self, re, rplcmnt,0));
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval, joe_Exception_New (
                               "joe_String replaceAll: invalid argument"));
      return JOE_FAILURE;
   }
}

static int
replaceFirst (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 2 && JOE_ISCLASS(argv[0], &joe_String_Class)
                 && JOE_ISCLASS(argv[1], &joe_String_Class)) {
      char * re = joe_String_getCharStar (argv[0]);
      char * rplcmnt = joe_String_getCharStar (argv[1]);
      
      joe_Object_assign(retval, replace (self, re, rplcmnt, 1));
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval, joe_Exception_New (
                               "joe_String replaceFirst: invalid argument"));
      return JOE_FAILURE;
   }
}

static int
intValue (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 0) {
      char * const str = joe_String_getCharStar (self);
      int64_t val = joe_Integer_fromAscii(str);
      if (val == 0) {
         int len = joe_String_length (self);
         if (len == 0) {
             joe_Object_assign(retval, joe_Null_value);
         } else {
            if (str[0] != '0')
               joe_Object_assign(retval, joe_Null_value);
            else
               joe_Object_assign(retval, joe_Integer_New (val));
         }
      } else {
         joe_Object_assign(retval, joe_Integer_New (val));
      }
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval, joe_Exception_New ("intValue: invalid argument"));
      return JOE_FAILURE;
   }
}

static int
floatValue (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 0) {
      char * const str = joe_String_getCharStar (self);
      double val = atof(str);
      if (val == 0) {
         int len = joe_String_length (self);
         if (len == 0) {
             joe_Object_assign(retval, joe_Null_value);
         } else {
            if (str[0] != '0' && str[0] != '.')
               joe_Object_assign(retval, joe_Null_value);
            else
               joe_Object_assign(retval, joe_Float_New (val));
         }
      } else {
         joe_Object_assign(retval, joe_Float_New (val));
      }
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval,
                      joe_Exception_New ("floatValue: invalid argument"));
      return JOE_FAILURE;
   }
}

static int
bigDecimalValue (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 0) {
      char * const str = joe_String_getCharStar (self);
      joe_Object_assign(retval, joe_BigDecimal_New_str (str));
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval,
                     joe_Exception_New ("bigDecimalValue: invalid argument"));
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
  {"indexOf", indexOf},
  {"lastIndexOf", lastIndexOf},
  {"substring", substring},
  {"trim", trim},
  {"charAt", charAt},
  {"startsWith", startsWith},
  {"toLowerCase", toLowerCase},
  {"toUpperCase", toUpperCase},
  {"equalsIgnoreCase", equalsIgnoreCase},
  {"matches", matches},
  {"split", split},
  {"replaceAll", replaceAll},
  {"replaceFirst", replaceFirst},
  {"intValue", intValue},
  {"longValue", intValue},
  {"floatValue", floatValue},
  {"doubleValue", floatValue},
  {"bigDecimalValue", bigDecimalValue},
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
joe_String_New_len (const char *c, unsigned int len) {
   joe_Object self;

   self = joe_Object_New (&joe_String_Class, len + 1);
   memcpy (JOE_CHAR_STAR(self), c, len);
   return self;
}

joe_Object
joe_String_New (const char *c)
{
   if (c)
      return joe_String_New_len (c, strlen (c));
   else
      return joe_String_New_len ("", 0);
}

joe_Object
joe_String_New2 (const char *c1, const char *c2) {
   int len = strlen (c1) + strlen (c2);
   joe_Object self;

   self = joe_Object_New (&joe_String_Class, len + 1);
   strcpy (JOE_CHAR_STAR(self), c1);
   strcat (JOE_CHAR_STAR(self), c2);
   return self;
}

joe_Object
joe_String_New3 (const char *c1, const char *c2, const char *c3) {
   int len = strlen (c1) + strlen (c2) + strlen(c3);
   joe_Object self;

   self = joe_Object_New (&joe_String_Class, len + 1);
   strcpy (JOE_CHAR_STAR(self), c1);
   strcat (JOE_CHAR_STAR(self), c2);
   strcat (JOE_CHAR_STAR(self), c3);
   return self;
}

joe_Object
joe_String_New4(const char *c1,const char *c2,const char *c3,const char *c4) {
   int len = strlen (c1) + strlen (c2) + strlen(c3) + strlen (c4);
   joe_Object self;

   self = joe_Object_New (&joe_String_Class, len + 1);
   strcpy (JOE_CHAR_STAR(self), c1);
   strcat (JOE_CHAR_STAR(self), c2);
   strcat (JOE_CHAR_STAR(self), c3);
   strcat (JOE_CHAR_STAR(self), c4);
   return self;
}
 
char *
joe_String_getCharStar (joe_String self)
{
   return JOE_CHAR_STAR(self);
}

int
joe_String_compare (joe_String self, joe_String aStr)
{
   if (self)
      if (aStr)
         return strcmp (JOE_CHAR_STAR(self), JOE_CHAR_STAR(aStr));
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
   return strlen (JOE_CHAR_STAR(self));
}
