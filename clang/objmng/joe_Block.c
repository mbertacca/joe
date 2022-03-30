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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
# include "joe_Block.h"
# include "joe_JOEObject.h"
# include "joe_ArrayList.h"
# include "joe_HashMap.h"
# include "joe_Variable.h"
# include "joe_BreakBlockException.h"
# include "joe_GotoException.h"
# include "joe_Gosub.h"
# include "joe_Memory.h"
# include "joe_Null.h"
# include "joestrct.h"
# include "joearray.h"

# define MESSAGES 0
# define VARIABLES 1
# define PARENT 2
# define NAME 3
# define ARGS_NAMES 4
# define SUPER 5

static char *variables[] = { "messages", "variables", "parent",
                             "name", "argsNames", "super", 0};

void joe_Block_setVariable(joe_Block self, joe_Variable var, joe_Object value);

int
joe_Block_assign (joe_Object self,int argc,joe_Object *args,joe_Object *retval)
{
   if (argc == 2 && joe_Object_instanceOf (args[0], &joe_Variable_Class)) {
      joe_Block_setVariable (self, args[0], args[1]);
      *retval = args[1];
      return JOE_SUCCESS;
   } else {
      *retval = joe_Exception_New ("assignment: invalid argument");
      return JOE_FAILURE;
   }
}

static int
findLabel (joe_Block *blk, joe_String name) {
   joe_Object obj;
   joe_Block parent = 0;
   joe_ArrayList messages;
   unsigned int len;
   int i;

   for (parent = joe_WeakReference_get(*JOE_AT(*blk, PARENT));
        parent != NULL;
        parent = joe_WeakReference_get(*JOE_AT(*blk, PARENT))) {
       *blk = parent;
   }

   messages = *JOE_AT(*blk, MESSAGES);
   len = joe_ArrayList_length (messages);
   for (i = 0; i < len; i++) {
      obj = *joe_ArrayList_at(messages,i);
      if (joe_Message_isLabel (obj, name)) {
/*
         printf ("[%d][%s] ? [%d]\n", i, joe_String_getCharStar(obj),
                   strcmp (joe_String_getCharStar(obj), name));
*/
         return i;
      }
   }
   return -1;
}


static int my_exec_sub (joe_Object self, int startMsg, joe_Object *retval);


static int transfer_flow (joe_Block blk, joe_String label, joe_Object *retval) {
   int Return;
   int newExecIdx;

   for ( ; ; ) {
      newExecIdx = findLabel (&blk, label);
      if (newExecIdx < 0) {
         char *error = calloc (1, 32 + joe_String_length(label));
         strcpy (error, "label not found: ");
         strcat (error, joe_String_getCharStar(label));
         joe_Object_assign(retval, joe_Exception_New(error));
         Return = JOE_FAILURE;
         free (error);
         break;
      } else {
         Return = my_exec_sub (blk, newExecIdx, retval);
         if (joe_Object_instanceOf (*retval, &joe_GotoException_Class)) {
            joe_String msg = 0;
            joe_Exception_getMessage (*retval, 0, 0, &msg);
            label = msg;
            joe_Object_assign (&msg, 0);
         } else {
            break;
         }
      }
   }
   return Return;
}

static int
gosub (joe_Block blk, joe_Gosub gsb, joe_Object *retval) {
   joe_String label = joe_Gosub_getLabel(gsb);
   int Return;

   joe_Object_assign (&gsb, 0);
   Return = transfer_flow (blk, label, retval);
   if (Return != JOE_SUCCESS &&
       JOE_ISCLASS((*retval),&joe_BreakException_Class)) {
      joe_Object retobj = joe_BreakException_getReturnObj(*retval);
      if (retobj)
         joe_Object_assign(retval, retobj);
      Return = JOE_SUCCESS;
   }

   return Return;
}

static int
my_exec_sub (joe_Object self, int startMsg, joe_Object *retval)
{
   unsigned int i;
   joe_Object obj;
   joe_Object lretval;
   int rc = JOE_SUCCESS;
   joe_ArrayList messages = *JOE_AT(self,MESSAGES);
   unsigned int len = joe_ArrayList_length (messages);

   joe_Object_assign(retval, 0);
   lretval = 0;
   for (i = startMsg; i < len; i++) {
      obj = *joe_ArrayList_at(messages,i);
      if (joe_Object_instanceOf (obj, &joe_Message_Class)) {
         lretval = 0;
         rc = joe_Message_exec (obj, self, &lretval);
         if (rc != JOE_SUCCESS) {
            if (joe_Object_instanceOf (lretval, &joe_BreakException_Class)) {
               joe_Object retobj = joe_BreakException_getReturnObj(lretval);
               if (retobj == 0) {
                  joe_BreakException_setReturnObj(lretval, *retval);
               }
               if (joe_Object_instanceOf (lretval,
                                          &joe_BreakBlockException_Class)) {
                  joe_String msg = 0;
                  joe_String blkName = *JOE_AT(self,NAME);
                  joe_Exception_getMessage (lretval, 0, 0, &msg);

                  if (joe_String_length(msg) == 0 ||
                      !joe_String_compare (blkName, msg)) {
                     rc = JOE_SUCCESS;
                     joe_Object_assign(&lretval, 0);
                     if (retobj)
                        joe_Object_assign (retval, retobj);
                  } else {
                     joe_Object_transfer (retval, &lretval);
                  }
                  joe_Object_assign (&msg, 0);
               } else {
                  joe_Object_transfer(retval, &lretval);
               }
            } else {
               joe_Object_transfer(retval, &lretval);
            }
            break;
         } else if (joe_Object_instanceOf (lretval, &joe_Gosub_Class)) {
             if ((rc = gosub (self, lretval, retval)) != JOE_SUCCESS)
                break;
         } else {
            joe_Object_transfer(retval, &lretval);
         }
      } else if (joe_Object_instanceOf (obj, &joe_Variable_Class)) {
         rc = joe_Block_getVariable(self, obj, retval);
      } else {
         joe_Object_assign (retval, obj);
      }
   }
   return rc;
}


static int
my_exec (joe_Object self, int argc, joe_Object *args, joe_Object *retval)
{
   unsigned int i;
   joe_Object obj;
   int rc = JOE_SUCCESS;
   joe_ArrayList argsNames = *JOE_AT(self, ARGS_NAMES);
   joe_HashMap argsVars = 0;
   unsigned int argsNamesLen = joe_ArrayList_length (argsNames);
   unsigned int validArgsLen = argsNamesLen < argc ? argsNamesLen : argc;

   if (argsNamesLen) {
      argsVars = *JOE_AT(self, VARIABLES);
      if (!argsVars) {
         argsVars = joe_HashMap_New_size (4);
         joe_Object_assign (JOE_AT(self, VARIABLES), argsVars);
      }
      for (i = 0; i < validArgsLen; i++) {
         obj=joe_HashMap_put(argsVars,*joe_ArrayList_at(argsNames, i),args[i]);
         joe_Object_delIfUnassigned (&obj);
      }
      for ( ; i < argsNamesLen; i++) {
         obj=joe_HashMap_put(argsVars,*joe_ArrayList_at(argsNames, i),
                             joe_Null_value);
         joe_Object_delIfUnassigned (&obj);
      }
   }
   rc = my_exec_sub (self, 0, retval);
   if (argsVars)
      for (i = 0; i < argsNamesLen; i++) {
         obj=joe_HashMap_put (argsVars, *joe_ArrayList_at(argsNames, i), 0);
         joe_Object_delIfUnassigned (&obj);
      }
   return rc;
}

int
joe_Block_outer_exec (joe_Object self, int argc, joe_Object *args, joe_Object *retval)
{
   unsigned int i;
   joe_Object obj;
   int rc = JOE_SUCCESS;
   joe_ArrayList argsNames = *JOE_AT(self, ARGS_NAMES);
   joe_HashMap argsVars = 0;
   unsigned int argsNamesLen = joe_ArrayList_length (argsNames);
   unsigned int validArgsLen = argsNamesLen < argc ? argsNamesLen : argc;

   if (argsNamesLen) {
      argsVars = *JOE_AT(self, VARIABLES);
      for (i = 0; i < validArgsLen; i++) {
         obj=joe_HashMap_put(argsVars,*joe_ArrayList_at(argsNames, i),args[i]);
         joe_Object_delIfUnassigned (&obj);
      }
      for ( ; i < argsNamesLen; i++) {
         obj=joe_HashMap_put(argsVars,*joe_ArrayList_at(argsNames, i),
                             joe_Null_value);
         joe_Object_delIfUnassigned (&obj);
      }
   }

   rc = my_exec_sub (self, 0, retval);
   if (rc != JOE_SUCCESS &&
       joe_Object_instanceOf (*retval, &joe_GotoException_Class)) {
      joe_String msg = 0;
      joe_String label;
      joe_Exception_getMessage (*retval, 0, 0, &msg);
      label = msg;
      joe_Object_assign (&msg, 0);
      rc = transfer_flow (self, label, retval);
   }

   if (argsVars)
      for (i = 0; i < argsNamesLen; i++) {
         obj=joe_HashMap_put (argsVars, *joe_ArrayList_at(argsNames, i), 0);
         joe_Object_delIfUnassigned (&obj);
      }
   return rc;
}

int
joe_Block_new (joe_Object self, int argc, joe_Object *args, joe_Object *retval)
{
   int rc;
   joe_JOEObject jobj = 0;

   joe_Object_assign(&jobj, joe_JOEObject_New(self, self));

   rc = my_exec (jobj, argc, args, retval);

   if (rc == JOE_SUCCESS) {
      joe_Object_assign(retval, jobj);
   }
   joe_Object_assign(&jobj, 0);

   return rc;
}

int
joe_Block_exec (joe_Object self, int argc, joe_Object *args, joe_Object *retval)
{
   int rc;
   joe_Object* thisVars = joe_Object_array(self);

   joe_HashMap oldVar = 0;

   joe_Object_assign (&oldVar, thisVars[VARIABLES]);
   joe_Object_assign (&thisVars[VARIABLES], 0);

   rc = my_exec (self, argc, args, retval);

   joe_Object_assign (&thisVars[VARIABLES], oldVar);
   joe_Object_assign (&oldVar, 0);
   return rc;
}

void
joe_Block_setParent (joe_Block self, joe_Block parent)
{
   joe_Block currParent = joe_WeakReference_get(*JOE_AT(self, PARENT));
   if (currParent != parent)
       joe_Object_assign (JOE_AT(self, PARENT), joe_WeakReference_New(parent));
}


joe_Block
joe_Block_clone (joe_Block self, joe_Block parent)
{
   joe_Block Return;
   joe_ArrayList messNew, messOld;
   unsigned int len;
   int i;
   if (parent == 0)
      parent = self;

   Return = joe_Object_clone (self);
   joe_HashMap newVars = joe_HashMap_New (8);

   joe_Object_assign (JOE_AT(Return, PARENT),
                                  joe_WeakReference_New(parent));
   joe_Object_assign (JOE_AT(Return, VARIABLES), newVars);

   messOld = *JOE_AT(self, MESSAGES);
   len = joe_ArrayList_length (messOld);   
   joe_Object_assign (JOE_AT(Return, MESSAGES),
                       messNew = joe_ArrayList_New(len));
   for (i = 0; i < len; i++) {
      joe_ArrayList_add (messNew,
                joe_Message_clone (*joe_ArrayList_at(messOld,i), Return));
   }
   return Return;
}

static int
_name(joe_JOEObject self, int argc, joe_Object* argv, joe_Object* retval)
{
   joe_Object_assign(retval, joe_String_New(joe_Block_getName(self)));
   return JOE_SUCCESS;
}

static void
getVarCount (joe_JOEObject blk, int *count)
{
   joe_HashMap vars = *JOE_AT(blk, VARIABLES);
   joe_Block parent = joe_WeakReference_get (*JOE_AT(blk, PARENT));

   *count += joe_HashMap_length(vars);
   if (parent)
      getVarCount (parent, count);
}

static void
varsToArray(joe_JOEObject blk, joe_Array array, int *count)
{
   int i;
   joe_HashMap vars = *JOE_AT(blk, VARIABLES);
   joe_Block parent = joe_WeakReference_get(*JOE_AT(blk, PARENT));
   joe_Array varArray = 0;

   joe_Object_assign(&varArray, joe_HashMap_keys (vars));
   int len = joe_Array_length(varArray);
   for (i = 0; i < len; i++) 
      joe_Object_assign(JOE_AT(array, (*count)++),
                        *JOE_AT(varArray, i));
   if (parent)
      varsToArray(parent, array, count);
   joe_Object_assign(&varArray, 0);
}

static int
getVariablesNames(joe_JOEObject self, int argc, joe_Object* argv, joe_Object* retval)
{
   int varCount = 0;
   joe_Array Return = 0;

   getVarCount (self, &varCount);
   Return = joe_Array_New (varCount);
   varCount = 0;
   varsToArray (self, Return, &varCount);
   joe_Object_assign(retval, Return);
   return JOE_SUCCESS;
}

static int
getVariable(joe_JOEObject self, int argc, joe_Object* argv, joe_Object* retval)
{
   if (argc == 1 && joe_Object_instanceOf(argv[0], &joe_String_Class)) {
      int rc;
      joe_Variable var = 0;
      joe_Object_assign(&var, joe_Variable_New (joe_String_getCharStar(argv[0])));
      rc = joe_Block_getVariable(self, var, retval);
      joe_Object_assign(&var, 0);
      return rc;
   } else {
      joe_Object_assign(retval,
                        joe_Exception_New("getVariable: invalid argument(s)"));
      return JOE_FAILURE;
   }
   return JOE_SUCCESS;
}


static joe_Method mthds[] = {
  {"exec", joe_Block_exec },
  {"assign", joe_Block_assign },
  {"new", joe_Block_new },
  {"name", _name },
  {"getVariable", getVariable },
  {"getVariablesNames", getVariablesNames },
  {(void *) 0, (void *) 0}
};


joe_Class joe_Block_Class = {
   "joe_Block",
   0,
   0,
   mthds,
   variables,
   &joe_Object_Class,
   0
};

void
joe_Block_Init (joe_Block self, joe_Block parent)
{
   joe_HashMap dbg;
   joe_Object* thisVars = joe_Object_array(self);
   joe_Object_assign(&thisVars[MESSAGES], joe_ArrayList_New(8));

   joe_Object_assign(&thisVars[VARIABLES], dbg = joe_HashMap_New(8));

   joe_Object_assign(&thisVars[PARENT],
                    joe_WeakReference_New(parent));
   joe_Object_assign(&thisVars[NAME], 0);
   joe_Object_assign(&thisVars[ARGS_NAMES], joe_ArrayList_New(8));
   joe_Object_setAcyclic (self);
}

joe_Block
joe_Block_New (joe_Block parent)
{
   joe_Object self;
   self = joe_Object_New (&joe_Block_Class, 0);
   joe_Block_Init (self, parent);
   return self;
}

void
joe_Block_addMessage (joe_Block self, joe_Message msg)
{
   joe_ArrayList_add (*JOE_AT(self,MESSAGES), msg);
}

static joe_HashMap
getVars (joe_Object self, joe_String varname, unsigned int hash)
{
   joe_HashMap Return = 0;

   Return = *JOE_AT(self,VARIABLES);
   if (!joe_HashMap_containsHashKey(Return, hash, varname)) {
      joe_Block parent = joe_WeakReference_get(*JOE_AT(self, PARENT));
      if (parent && !joe_Object_instanceOf (self, &joe_JOEObject_Class))
         Return = getVars (parent, varname, hash);
      else
         Return = 0;
   }

   return Return;
}

static int
getVar (joe_Object self, unsigned int hash, joe_String varname,
                   joe_Object* retval)
{
   joe_Object lretval;
   joe_HashMap hashmap = *JOE_AT(self,VARIABLES);

   if (!strcmp(joe_String_getCharStar (varname), "!!")) {
      joe_Object_assign(retval, joe_WeakReference_New(self));
      return JOE_SUCCESS;
   }

   if (joe_HashMap_getHash(hashmap, hash, varname, &lretval)) {
      joe_Object_assign (retval, lretval);
   } else {
      joe_Block parent = joe_WeakReference_get(*JOE_AT(self, PARENT));
      if (parent) {
         return getVar (parent, hash, varname, retval);
      } else {
         joe_String msg = 0;
         joe_Object_assign(&msg,
                          joe_String_New3("Variable `",
                                          joe_String_getCharStar(varname),
                                         "' not found"));
         joe_Object_assign(retval, joe_Exception_New_string(msg));
         joe_Object_assign(&msg, 0);
          return JOE_FAILURE;
      }
   }

   return JOE_SUCCESS;
}

int
joe_Block_getVariable (joe_Object self, joe_Variable var, joe_Object* retval)
{
   return getVar (self, joe_Variable_hash(var), joe_Variable_name(var),
                  retval);
}

void
joe_Block_setVariable (joe_Block self, joe_Variable var, joe_Object value)
{
   joe_Object oldValue;
   unsigned int hash = joe_Variable_hash (var);
   joe_String varname = joe_Variable_name (var);
   joe_HashMap vars = getVars (self, varname, hash);
   if (!vars) {
      vars = *JOE_AT(self, VARIABLES);
      if (!vars) {
         vars = joe_HashMap_New_size (4);
         joe_Object_assign (JOE_AT(self, VARIABLES), vars);
      }
   }
   if (value == NULL)
      value = joe_Null_value;
   oldValue = joe_HashMap_putHash (vars, hash, joe_Variable_name(var), value);
   joe_Object_delIfUnassigned (&oldValue);
}

void
joe_Block_setName (joe_Block self, char *name)
{
   joe_Object_assign (JOE_AT(self,NAME), joe_String_New (name));
}

char *
joe_Block_getName (joe_Block self)
{
   joe_String name = *JOE_AT(self,NAME);
   if (name)
      return joe_String_getCharStar (*JOE_AT(self,NAME));
   else
      return "(null)";
}

void
joe_Block_addArgName (joe_Block self, char *name)
{
   joe_ArrayList argsNames = *JOE_AT(self,ARGS_NAMES);
   joe_ArrayList_add (argsNames,  joe_String_New (name));
}


/* JOEObject */

static char *JOEObject_variables[] = { 0 };

static int extends (joe_JOEObject self,
                   int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1 && joe_Object_instanceOf(argv[0], &joe_JOEObject_Class)) {
      joe_Object_assign (JOE_AT(self,SUPER), argv[0]);
      joe_Object_assign(retval, self);
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval,
                        joe_Exception_New ("extends: invalid argument"));
      return JOE_FAILURE;
   }
}

static int
parent (joe_JOEObject self, int argc, joe_Object *argv, joe_Object *retval)
{
   joe_Object_assign(retval, joe_WeakReference_get(*JOE_AT(self,PARENT)));
   return JOE_SUCCESS;
}

static joe_Method JOEObject_mthds[] = {
  {"name", _name },
  {"parent", parent },
  {"extends", extends },
  {(void *) 0, (void *) 0}
};

joe_Class joe_JOEObject_Class = {
   "joe_JOEObject",
   0,
   0,
   JOEObject_mthds,
   JOEObject_variables,
   &joe_Block_Class,
   0
};


joe_JOEObject
joe_JOEObject_New (joe_Block block, joe_Block parent)
{
   joe_Object self;

   self = joe_Block_clone (block, parent);
   if (!joe_Object_instanceOf (block, &joe_JOEObject_Class))
      joe_Object_extends (self, &joe_JOEObject_Class);

   return self;
}

joe_Block
joe_JOEObject_getReceiver (joe_JOEObject self, const char *selector)
{
   joe_String sel = 0;
   joe_Object receiver = 0;
   joe_HashMap variables = *JOE_AT(self,VARIABLES);

   joe_Object_assign (&sel, joe_String_New(selector));
   joe_HashMap_get (variables, sel, &receiver);
   joe_Object_assign (&sel, 0);
   if (receiver != 0 && joe_Object_instanceOf (receiver, &joe_Block_Class)) {
      return receiver;
   } else {
      joe_JOEObject super = *JOE_AT(self,SUPER);
      if (super)
         return joe_JOEObject_getReceiver (super, selector);
      else
         return 0;
   }
}
