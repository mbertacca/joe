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

# include <stdlib.h>
# include <string.h>
# include <dirent.h>
# include <sys/stat.h>
# include <errno.h>
# include <unistd.h>
# include "joe_Integer.h"
# include "joe_String.h"
# include "joe_StringBuilder.h"
# include "joe_ArrayList.h"
# include "joe_Date.h"
# include "joe_Files.h"
# include "joe_Boolean.h"
# include "joe_Exception.h"
# include "joe_Null.h"

static void
getErrno (char *filename, joe_Object *retval)
{
   joe_StringBuilder msg = 0;
   joe_Object_assign(&msg, joe_StringBuilder_New());
   joe_StringBuilder_appendCharStar(msg, "Error accessing `");
   joe_StringBuilder_appendCharStar(msg, filename);
   joe_StringBuilder_appendCharStar(msg, "' #");
   joe_StringBuilder_appendInt(msg, errno);
   joe_StringBuilder_appendCharStar(msg, ": ");
   joe_StringBuilder_appendCharStar(msg, strerror(errno));
   joe_Object_assign(retval,
                 joe_Exception_New_string (joe_StringBuilder_toString(msg)));
   joe_Object_assign(&msg, 0);
}

static int
readAllLines (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1 && joe_Object_instanceOf (argv[0], &joe_String_Class)) {
      int llen;
      char *line = 0;
      ssize_t lineLen = 0;
      char *filename = joe_String_getCharStar (argv[0]);
      FILE *fd = fopen (filename, "r");
      if (fd != NULL) {
         joe_Object_assign (retval, joe_ArrayList_New(64));
         while ((llen = joe_Files_getline (&line, &lineLen, fd)) >= 0) {
            joe_ArrayList_add(*retval, joe_String_New_len(line, llen));
         }
         fclose (fd);
         if (line) {
            free (line);
            line = 0;
            lineLen = 0;
         }
         return JOE_SUCCESS;
      } else {
         getErrno (filename, retval);
         return JOE_FAILURE;
      }
   } else {
      joe_Object_assign(retval, joe_Exception_New ("readAllLines: invalid argument"));
      return JOE_FAILURE;
   }
}

static int
exists (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1 && joe_Object_instanceOf (argv[0], &joe_String_Class)) {
      if (access(joe_String_getCharStar (argv[0]),F_OK))
         joe_Object_assign (retval, joe_Boolean_New_false());
      else
         joe_Object_assign (retval, joe_Boolean_New_true());
   } else {
      joe_Object_assign(retval, joe_Exception_New ("exists: invalid argument"));
      return JOE_FAILURE;
   }
   return JOE_SUCCESS;
}

static int
deleteIfExists (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1 && joe_Object_instanceOf (argv[0], &joe_String_Class)) {
      if (unlink(joe_String_getCharStar (argv[0])))
         joe_Object_assign (retval, joe_Boolean_New_false());
      else
         joe_Object_assign (retval, joe_Boolean_New_true());
   } else {
      joe_Object_assign(retval, joe_Exception_New ("deleteIfExists: invalid argument"));
      return JOE_FAILURE;
   }
   return JOE_SUCCESS;
}

static int
files_write (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   char *op = "w";

   if (argc == 3 && joe_Object_instanceOf (argv[2], &joe_String_Class)) {
      op = joe_String_getCharStar (argv[2]);
      argc--;
   }
   if (argc == 2 && joe_Object_instanceOf (argv[0], &joe_String_Class)
                 && joe_Object_instanceOf (argv[1], &joe_ArrayList_Class)) {
      char *filename = joe_String_getCharStar (argv[0]);
      joe_ArrayList list = argv[1];
      FILE *fd = fopen (filename, op);
      if (fd != NULL) {
         int rc;
         int llen;
         char *chst = 0;
         joe_String str = 0;
         joe_Object value;
         int idx = 0;
         int maxidx = joe_ArrayList_length(list);
         for (idx = 0; idx < maxidx; idx++) {
            value = *joe_ArrayList_at (list, idx);
            joe_Object_invoke (value, "toString", 0, 0, &str);
            chst = joe_String_getCharStar(str);
            llen = joe_String_length (str);
            if (llen) {
               rc = fwrite (chst, llen, 1, fd);
               joe_Object_assign(&str, 0);
               if (rc == 0) {
                  getErrno (filename, retval);
                  fclose (fd);
                  return JOE_FAILURE;
               }
            } else {
               joe_Object_assign(&str, 0);
            }
            fwrite ("\n", 1, 1, fd);
         }
         fclose (fd);
         joe_Object_assign (retval, argv[0]);
         return JOE_SUCCESS;
      } else {
         getErrno (filename, retval);
         return JOE_FAILURE;
      }
   } else {
      joe_Object_assign(retval, joe_Exception_New ("write: invalid argument"));
      return JOE_FAILURE;
   }
}

static int
listDirectory (joe_Object self,
                    int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1 && joe_Object_instanceOf (argv[0], &joe_String_Class)) {
      char *filename = joe_String_getCharStar (argv[0]);
      DIR *dir = opendir (filename);
      if (dir != NULL) {
         struct dirent *entry;
         joe_Object_assign (retval, joe_ArrayList_New(64));
         while ((entry = readdir (dir))	) {
            if (entry->d_name[0] == '.' &&
                 (entry->d_name[1] == 0 ||
                   (entry->d_name[1] == '.' && entry->d_name[2] == 0)))
               continue;
            joe_ArrayList_add(*retval, joe_String_New(entry->d_name));
         }
         closedir (dir);
         return JOE_SUCCESS;
      } else {
         getErrno (filename, retval);
         return JOE_FAILURE;
      }
   } else {
      joe_Object_assign(retval, joe_Exception_New ("listDirectory: invalid argument"));
      return JOE_FAILURE;
   }
}

static int
isDirectory (joe_Object self,
                    int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1 && joe_Object_instanceOf (argv[0], &joe_String_Class)) {
      char *filename = joe_String_getCharStar (argv[0]);
      struct stat sb;
      if (stat (filename, &sb) == 0) {
         if ((sb.st_mode & S_IFMT) == S_IFDIR) {
            joe_Object_assign (retval, joe_Boolean_New_true());
         } else {
            joe_Object_assign (retval, joe_Boolean_New_false());
         }
         return JOE_SUCCESS;
      } else {
         getErrno (filename, retval);
         return JOE_FAILURE;
      }
   } else {
      joe_Object_assign(retval, joe_Exception_New ("isDirectory: invalid argument"));
      return JOE_FAILURE;
   }
}

static int
getAttribute (joe_Object self,
                    int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc >= 2 && joe_Object_instanceOf (argv[0], &joe_String_Class)
                 && joe_Object_instanceOf (argv[1], &joe_String_Class)) {
      char *filename = joe_String_getCharStar (argv[0]);
      char *attr = joe_String_getCharStar (argv[1]);
      struct stat sb;
      int rc = stat(filename, &sb);
      if (rc != 0) {
         joe_Object_assign(retval,
            joe_Exception_New_string (
                  joe_String_New2("getAttribute: cannot access file ",filename)));
         return JOE_FAILURE;
      }
      if (!strcmp("size",attr)) {
         joe_Object_assign(retval, joe_Integer_New (sb.st_size));
      } else if (!strcmp("isRegularFile",attr)) {
         if ((sb.st_mode & S_IFMT) == S_IFREG) {
            joe_Object_assign (retval, joe_Boolean_New_true());
         } else {
            joe_Object_assign (retval, joe_Boolean_New_false());
         }
      } else if (!strcmp("isDirectory",attr)) {
         if ((sb.st_mode & S_IFMT) == S_IFDIR) {
            joe_Object_assign (retval, joe_Boolean_New_true());
         } else {
            joe_Object_assign (retval, joe_Boolean_New_false());
         }
      } else if (!strcmp("isOther",attr)) {
         if ((sb.st_mode & S_IFMT) != S_IFDIR &&
             (sb.st_mode & S_IFMT) != S_IFREG) {
            joe_Object_assign (retval, joe_Boolean_New_true());
         } else {
            joe_Object_assign (retval, joe_Boolean_New_false());
         }
      } else if (!strcmp("fileKey",attr)) {
         if (sb.st_ino == 0) {
            joe_Object_assign (retval, joe_Null_value);
         } else {
            char str[64];
            snprintf (str,sizeof(str),"(dev=%ld,ino=%ld)",
                        (long) sb.st_dev, (long) sb.st_ino);
            joe_Object_assign (retval, joe_String_New(str));
         }
      } else if (!strcmp("lastModifiedTime",attr)) {
         joe_Integer millis = 0;
         joe_Object_assign(&millis,
            joe_Integer_New (((int64_t)sb.st_mtime * 1000) +
                              joe_Date_getEpochMillis()));
         joe_Class_newInstance (&joe_Date_Class, 1, &millis, retval);
         joe_Object_assign(&millis, 0);
      } else if (!strcmp("lastAccessTime",attr)) {
         joe_Integer millis = 0;
         joe_Object_assign(&millis,
            joe_Integer_New (((int64_t)sb.st_atime * 1000) +
                              joe_Date_getEpochMillis()));
         joe_Class_newInstance (&joe_Date_Class, 1, &millis, retval);
         joe_Object_assign(&millis, 0);
      } else if (!strcmp("creationTime",attr)) {
         joe_Integer millis = 0;
         joe_Object_assign(&millis,
            joe_Integer_New (((int64_t)sb.st_mtime * 1000) +
                              joe_Date_getEpochMillis()));
         joe_Class_newInstance (&joe_Date_Class, 1, &millis, retval);
         joe_Object_assign(&millis, 0);
      } else {
         joe_Object_assign(retval,
            joe_Exception_New_string (
                  joe_String_New2("getAttribute: invalid argument ",attr)));
         return JOE_FAILURE;
      }
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval,
         joe_Exception_New ("getAttribute: invalid argument"));
      return JOE_FAILURE;
   }
}

static int
isAbsolute (joe_Object self,
                    int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1 && joe_Object_instanceOf (argv[0], &joe_String_Class)) {
      if (joe_Files_isAbsolute(joe_String_getCharStar (argv[0])))
         joe_Object_assign (retval, joe_Boolean_New_true());
      else
         joe_Object_assign (retval, joe_Boolean_New_false());
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval, joe_Exception_New ("isAbsolute: invalid argument"));
      return JOE_FAILURE;
   }
}

static joe_Method filesMthds[] = {
   {"readAllLines", readAllLines },
   {"write", files_write },
   {"listDirectory", listDirectory },
   {"newDirectoryStream", listDirectory },
   {"isDirectory", isDirectory },
   {"getAttribute", getAttribute },
   {"exists", exists },
   {"deleteIfExists", deleteIfExists },
   {"isAbsolute", isAbsolute },
   {(void *) 0, (void *) 0}
};

joe_Class joe_Files_Class = {
   "joe_Files",
   0,
   0,
   filesMthds,
   0,
   &joe_Object_Class,
   0
};

ssize_t
joe_Files_getline (char **lineptr, ssize_t *n, FILE *fp)
{
   int c;
   ssize_t cnt = 0;
   char *pnt;

   if (lineptr == NULL || n == NULL) {
      return -1;
   }

   if (*lineptr == NULL || *n == 0) {
      *n = 1;
      *lineptr = malloc (*n + 1);
   }
   if (*lineptr == NULL) {
      return -1;
   }
   pnt = *lineptr;
   c = 0;
   for ( ; ; ) {
      if (cnt + 2 >= *n ) {
         *n *= 2;
         *lineptr = realloc (*lineptr, *n + 1);
         pnt = *lineptr + cnt;
      }
      c = fgetc (fp);
      if (c != EOF) {
         cnt++;
         *(pnt) = c;
         if (c == '\n') {
            *pnt = 0;
            cnt--;
            if (pnt > *lineptr && *(--pnt) == '\r') {
               *pnt = 0;
               cnt--;
            }
            break;
         } else {
            pnt++;
         }
      } else {
         *pnt = 0;
         if (cnt == 0)
            cnt = -1;
         break;
      }
   }
   return cnt; 
}

int
joe_Files_isAbsolute (char *p)
{
   if (*p == '/')
      return 1;
# ifdef WIN32
   if (*p == '\\')
      return 1;
   if (strlen(p) > 2 && p[1] == ':' && p[2] == '\\')
      return 1;
# endif
   return 0;
}
