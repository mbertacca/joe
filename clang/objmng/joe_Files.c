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
# include <time.h>
# include "joe_Integer.h"
# include "joe_String.h"
# include "joe_StringBuilder.h"
# include "joe_ArrayList.h"
# include "joe_Date.h"
# include "joe_Files.h"
# include "joe_Boolean.h"
# include "joe_Exception.h"
# include "joe_Null.h"

#ifdef WIN32
#include <windows.h>

#ifndef PATH_MAX
#define PATH_MAX _MAX_PATH
#endif

static char*
realpath(const char* path, char* resolved_path) {
   HANDLE hFile;
   int len;
   char final_path[PATH_MAX + 5];

   if (path == NULL) {
      errno = EINVAL;
      return NULL;
   }

    hFile=CreateFileA(path,0,FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,
                      NULL,OPEN_EXISTING,FILE_FLAG_BACKUP_SEMANTICS,NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        errno = ENOENT;
        return NULL;
    }
    len = GetFinalPathNameByHandleA(hFile, final_path,
                                        PATH_MAX, FILE_NAME_NORMALIZED);
    CloseHandle(hFile);
    if (len == 0 || len >= PATH_MAX) {
        errno = EIO;
        return NULL;
    }
    strcpy (resolved_path,
       (strncmp(final_path, "\\\\?\\", 4) == 0) ? final_path + 4 : final_path);
    return resolved_path;
}

static int
isSymLink(char *path) {
   int attrs = GetFileAttributesA(path);
   if (attrs == INVALID_FILE_ATTRIBUTES)
      return 0;
   else
      return attrs & FILE_ATTRIBUTE_REPARSE_POINT;
}
#else
static int
isSymLink(char *path) {
   struct stat sb;
   if (lstat(path, &sb) == -1)
      return 0;
   else
      return S_ISLNK(sb.st_mode);
}
#endif /* WIN32 */

static time_t
gmt2Local(time_t t) {
   struct tm local;
   struct tm utc;
   struct tm *tm_pnt;
   time_t Return;

   tm_pnt = localtime(&t);
   if (!tm_pnt) return 0L;
   local = *tm_pnt;
   tm_pnt = gmtime(&t);
   if (!tm_pnt) return 0L;
   utc = *tm_pnt;

   Return = (local.tm_hour - utc.tm_hour) * 60 + (local.tm_min - utc.tm_min);

   if (local.tm_mday != utc.tm_mday) {
      int diffday = local.tm_mday - utc.tm_mday;
      if (diffday == 1 || diffday < -1)
         Return += 1440;
      else
         Return -= 1440;
   }
   Return *= 60;
   Return += t;
   return Return;
}


/**
# Class joe_Files
### extends joe_Object

An instance of this class makes available some useful methods for handling files.
You can get an instance with the following call:
```
   !newInstance "joe_Files"
```
*/

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

/**
## readAllLines _aString_

Returns a string array containing all the lines
in the file whose path is _aString_
*/

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

/**
## exists _aString_

Returns Boolean <1> if the file _aString_ exists, <0> otherwise
*/

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
/**
## deleteIfExists _aString_

Deletes the file _aString_ if exists:
Returns Boolean <1> if the file has been deleted, <0> otherwise
*/
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

/**
## write _aString_,_aArray_ [ , _aOpenMode_ ]

Writes the representation string of all the objects in _aArray_ in the
file _aString_. 
If _aOpenMode_ is specified it must be a String containing the open
mode as in the fopen C call ("w", "w+","a","a+").
*/

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

/**
## listDirectory _aString_

Returns a String ArrayList containing the list of the file in
the directory specified by _aString_
*/
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
/**
## isDirectory _aString_

Returns Boolean <1> if the file specified by _aString_ is a directory,
<0> otherwise.
*/

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
      } else {
         joe_Object_assign (retval, joe_Boolean_New_false());
      }
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval, joe_Exception_New ("isDirectory: invalid argument"));
      return JOE_FAILURE;
   }
}


/**
## isSymbolicLink _aString_

Returns Boolean <1> if the file specified by _aString_ is a symbolic link,
<0> otherwise.
*/

static int
isSymbolicLink (joe_Object self,
                int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1 && joe_Object_instanceOf (argv[0], &joe_String_Class)) {
      char *filename = joe_String_getCharStar (argv[0]);
      if (isSymLink(filename)) {
         joe_Object_assign (retval, joe_Boolean_New_true());
      } else {
         joe_Object_assign (retval, joe_Boolean_New_false());
      }
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval, joe_Exception_New ("isSymbolicLink: invalid argument"));
      return JOE_FAILURE;
   }
}

/**
## toRealPath _aString_

Returns a String with the canonicalized absolute pathname
*/
static int
toRealPath (joe_Object self,
                    int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1 && joe_Object_instanceOf (argv[0], &joe_String_Class)) {
      char *filename = joe_String_getCharStar (argv[0]);
      char resolved_path[PATH_MAX];
      if (realpath (filename, resolved_path)) {
         joe_Object_assign (retval, joe_String_New(resolved_path));
         return JOE_SUCCESS;
      } else {
         getErrno (filename, retval);
         return JOE_FAILURE;
      }
   } else {
      joe_Object_assign(retval, joe_Exception_New ("toRealPath: invalid argument"));
      return JOE_FAILURE;
   }
}


/**
## getAttribute _aString_,_aAttribute_

Gets information about the file _aString_.
This method returns different object depending on the attribute
specified in the string _aAttribute_, i.e.;

| _aAttribute_    | Object returned                                         |
| --------------- | -------------------------------------------------------- |
| "isRegularFile" | Boolean <1> if _aString_ is a regular file, <0> otherwise|
| "isDirectory"   | Boolean <1> if _aString_ is a directory, <0> otherwise|
| "isSymbolicLink"| Boolean <1> if _aString_ is a symbolic link, <0> otherwise|
| "isOther"   | Boolean <1> if _aString_ is neither a directory nor a file, <0> otherwise |
| "fileKey" | String that is unique for each file in the system (eg (dev=2049,ino=3282325)|
| "lastModifiedTime" | aDate object with the time of last modification |
| "lastAccessTime" | aDate object with the time of last access |
| "creationTime" |  aDate object with the time of creation  |
| "size"         |  aInteger object with the size of the file  |

These attributes depend from the underlying filesystem so the results can be inaccurate.
*/

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
         getErrno (filename, retval);
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
      } else if (!strcmp("isSymbolicLink",attr)) {
         if (isSymLink(filename)) {
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
            joe_Integer_New (((int64_t)gmt2Local(sb.st_mtime) * 1000) +
                              joe_Date_getEpochMillis()));
         joe_Class_newInstance (&joe_Date_Class, 1, &millis, retval);
         joe_Object_assign(&millis, 0);
      } else if (!strcmp("lastAccessTime",attr)) {
         joe_Integer millis = 0;
         joe_Object_assign(&millis,
            joe_Integer_New (((int64_t)gmt2Local(sb.st_atime) * 1000) +
                              joe_Date_getEpochMillis()));
         joe_Class_newInstance (&joe_Date_Class, 1, &millis, retval);
         joe_Object_assign(&millis, 0);
      } else if (!strcmp("creationTime",attr)) {
         joe_Integer millis = 0;
         joe_Object_assign(&millis,
            joe_Integer_New (((int64_t)gmt2Local(sb.st_mtime) * 1000) +
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
/**
## isAbsolute _aString_

Returns Boolean <1> if the path specified by _aString_ is absolute,
<0> otherwise.
*/
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
   {"isSymbolicLink", isSymbolicLink },
   {"getAttribute", getAttribute },
   {"exists", exists },
   {"deleteIfExists", deleteIfExists },
   {"isAbsolute", isAbsolute },
   {"toRealPath", toRealPath },
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
