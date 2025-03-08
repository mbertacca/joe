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

# include "joe_String.h"
# include "joe_ByteArray.h"
# include "joe_Integer.h"
# include "joe_Pointer.h"
# include "joe_Exception.h"

# include <stdlib.h>
# include <string.h>
/**
# Class joe_BangSO
### extends joe_Object

This class allows to call C function in a shared library loaded using the
`!loadSO` method.
*/
# ifdef WIN32
# include "windows.h"

# define DLOPEN LoadLibraryA
# define DLOPEN_RETURN_TYPE HMODULE
# define DLSYM GetProcAddress
# define DLSYM_TYPE(x) FARPROC x
# define DLCLOSE FreeLibrary
# define STDCALL_FUNC(fPnt) ((long(__stdcall*)())fPnt)
# define CDECL_FUNC(fPnt) ((long(__cdecl*)())fPnt)
# define STRLEN _strlen

# else /* WIN32 */

# include <dlfcn.h>
# ifndef __FreeBSD__
# include <alloca.h>
# endif
# ifdef AIX
# define DLOPEN(x) dlopen(x,RTLD_MEMBER|RTLD_LAZY)
# else /* OTHER UNIX FLAVORS */
# define DLOPEN(x) dlopen(x,RTLD_LAZY)
# endif /* ! AIX */
# define DLOPEN_RETURN_TYPE void *
# define DLSYM dlsym
# define DLSYM_TYPE(x) void* (*x)()
# define DLCLOSE dlclose
# define STDCALL_FUNC(fPnt) ((long(*)())fPnt)
# define CDECL_FUNC(fPnt) ((long(*)())fPnt)
# define STRLEN strlen

# endif /* ! WIN32 */


struct LibData {
   DLOPEN_RETURN_TYPE pnt;
   int dllConv;
};
/**
## call _aString_ [,arg1 ... ,argN ] 

This method calls a C function in this shared object. It returns a
Pointer object that can be interpreted either a C string or an integer
or an address. The arguments can be Strings, Integers or an instance
of ByteArray in order to pass complex data structures.

Example
```
lib := !loadSO"".
lib call "printf",("%s length =%d" + (!nl)),"string",("string" length).
```
**Be careful in using this methods** since an incorrect use can cause
a memory corruption.
*/
static int
call (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   joe_Class* stringClass = joe_Class_getClass("joe_String");
   joe_Class* byteArrayClass = joe_Class_getClass("joe_ByteArray");
   joe_Class* integerClass = joe_Class_getClass("joe_Integer");
   joe_Class* pointerClass = joe_Class_getClass("joe_Pointer");
   struct LibData *lib = (struct LibData *) *joe_Object_getMem (self);

   if (argc > 0 && joe_Object_instanceOf(argv[0], stringClass)) {
      char *funcName = joe_String_getCharStar (argv[0]);
      void *funcArg[8];
      DLSYM_TYPE(pnt) = DLSYM(lib->pnt,funcName);

      if (pnt) {
         int i = 0;

         for (i = 1; i < argc; i++) {
            if (joe_Object_instanceOf(argv[i], stringClass)) {
               funcArg[i - 1] = joe_String_getCharStar (argv[i]);
            } else if (joe_Object_instanceOf(argv[i], byteArrayClass)) {
               funcArg[i - 1] = joe_ByteArray_getCharStar (argv[i]);
            } else if (joe_Object_instanceOf(argv[i], integerClass)) {
               funcArg[i - 1] = (void*) joe_Integer_value (argv[i]);
            } else if (joe_Object_instanceOf(argv[i], pointerClass)) {
               funcArg[i - 1] = (void*) joe_Pointer_value (argv[i]);
            } else  {
               funcArg[i - 1] = joe_Object_getMem (argv[i]);
            }
         }
         switch (argc) {
         case 1:
            joe_Object_assign(retval, joe_Pointer_New ((void*)pnt ()));
            break;
         case 2:
            joe_Object_assign(retval, joe_Pointer_New ((void*)pnt (funcArg[0])));
            break;
         case 3:
            joe_Object_assign(retval,joe_Pointer_New ((void*)pnt (funcArg[0],
                                                           funcArg[1])));
            break;
         case 4:
            joe_Object_assign(retval,joe_Pointer_New ((void*)pnt (funcArg[0],
                                                           funcArg[1],
                                                           funcArg[2])));
            break;
         case 5:
            joe_Object_assign(retval,joe_Pointer_New ((void*)pnt (funcArg[0],
                                                           funcArg[1],
                                                           funcArg[2],
                                                           funcArg[3])));
            break;
         case 6:
            joe_Object_assign(retval,joe_Pointer_New ((void*)pnt (funcArg[0],
                                                           funcArg[1],
                                                           funcArg[2],
                                                           funcArg[3],
                                                           funcArg[4])));
            break;
         case 7:
            joe_Object_assign(retval,joe_Pointer_New ((void*)pnt (funcArg[0],
                                                           funcArg[1],
                                                           funcArg[2],
                                                           funcArg[3],
                                                           funcArg[4],
                                                           funcArg[5])));
            break;
         case 8:
            joe_Object_assign(retval,joe_Pointer_New ((void*)pnt (funcArg[0],
                                                           funcArg[1],
                                                           funcArg[2],
                                                           funcArg[3],
                                                           funcArg[4],
                                                           funcArg[5],
                                                           funcArg[6])));
            break;
         case 9:
            joe_Object_assign(retval,joe_Pointer_New ((void*)pnt (funcArg[0],
                                                           funcArg[1],
                                                           funcArg[2],
                                                           funcArg[3],
                                                           funcArg[4],
                                                           funcArg[5],
                                                           funcArg[6],
                                                           funcArg[7])));
            break;
         default:
            joe_Object_assign(retval,
                              joe_Exception_New ("call :too many arguments"));
            return JOE_FAILURE;
         }
         return JOE_SUCCESS;
      } else {
        joe_Object_assign(retval,
                          joe_Exception_New ("call :function not found"));
         return JOE_FAILURE;
      }
   } else {
     joe_Object_assign(retval, joe_Exception_New ("call :invalid argument"));
      return JOE_FAILURE;
   }
}

static joe_Method mthds[] = {
  {"call", call},
  {(void *) 0, (void *) 0}
};

static joe_Class joe_BangSO_Class = {
   "joe_BangSO",
   0,
   0,
   mthds,
   0,
   &joe_Object_Class,
   0
};

int
joe_BangSO_New (joe_String soName, joe_Object *retval)
{
   DLSYM_TYPE(pnt) = 0;
   const char* libName = joe_String_getCharStar (soName);
   struct LibData lib;

   if (libName == 0 || *libName == 0) {
# ifdef WIN32
      joe_Object_assign (retval,
                         joe_Exception_New ("loadSO: null argument"));
      return JOE_FAILURE;
# else
      libName = 0;
# endif
   }
   lib.pnt = DLOPEN(libName);
   lib.dllConv = 0;
   if (lib.pnt == 0) {
      joe_Object_assign(retval,
                        joe_Exception_New ("loadSO: object not found"));
      return JOE_FAILURE;
   }
   pnt = DLSYM(lib.pnt,"joe_init");
   if (pnt != 0) {
      pnt();
   }

   joe_Object_assign(retval,
                     joe_Object_New (&joe_BangSO_Class, sizeof(struct LibData)));
   memcpy (*joe_Object_getMem (*retval), &lib, sizeof(struct LibData));

   return JOE_SUCCESS;
}
