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
# include "joe_HashMap.h"
# include "joe_Variable.h"
# include "joe_BreakBlockException.h"
# include "joe_GotoException.h"
# include "joe_DoDebugException.h"
# include "joe_Gosub.h"
# include "joe_Memory.h"
# include "joe_Null.h"
# include "joe_Bang.h"
# include "joestrct.h"
# include "joearray.h"

# define MESSAGES 0
# define PARENT 1
# define NAME 2
# define ARGS_NAMES 3
# define SUPER 4
# define BANG 5
# define VARIABLES 6
# define VARCONTENT 7

static char *varvalues[] = { "messages", "parent", "name",
                             "argsNames", "super", "bang",
                             "variables", "varContent", 0};

void joe_Block_setVarValue(joe_Block self, joe_Variable var, joe_Object value);

static int
findLabel (joe_Block *blk, joe_String name) {
   joe_Object obj;
   joe_Block parent = 0;
   joe_Array messages;
   unsigned int len;
   int i;

   for (parent = *JOE_AT(*blk, PARENT);
        parent != NULL;
        parent = *JOE_AT(*blk, PARENT)) {
       *blk = parent;
   }

   messages = *JOE_AT(*blk, MESSAGES);
   len = joe_Array_length (messages);
   for (i = 0; i < len; i++) {
      obj = *JOE_AT (messages,i);
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

static int debug = 0;

static int
my_exec_sub (joe_Object self, int startMsg, joe_Object *retval)
{
   unsigned int i;
   joe_Object obj;
   joe_Object lretval;
   int rc = JOE_SUCCESS;
   joe_Array messages = *JOE_AT(self,MESSAGES);
   unsigned int len = joe_Array_length (messages);

   joe_Object_assign(retval, joe_Null_value);
   lretval = 0;
   for (i = startMsg; i < len; i++) {
      obj = *JOE_AT(messages,i);
      if (joe_Object_instanceOf (obj, &joe_Message_Class)) {
         lretval = 0;
         if (debug)
            rc = joe_Message_debug (obj, &debug, self, &lretval);
         else
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
            } else if (joe_Object_instanceOf (
                       lretval, &joe_DoDebugException_Class)) {
               debug = 1;
               joe_Object_assign (&lretval, 0);
               rc = JOE_SUCCESS;
               continue;
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
         rc = joe_Block_getVarValue(self, obj, retval);
      } else {
         joe_Object_assign (retval, obj);
      }
   }
   return rc;
}

static int
init (joe_Object self, int argc, joe_Object *args, joe_Object *retval)
{
   unsigned int i;
   int rc = JOE_SUCCESS;
   joe_Array argsNames = *JOE_AT(self, ARGS_NAMES);
   unsigned int argsNamesLen = joe_Array_length (argsNames);
   unsigned int validArgsLen = argsNamesLen < argc ? argsNamesLen : argc;

   if (argsNamesLen) {
      joe_Array varcontent = *JOE_AT(self, VARCONTENT);
      for (i = 0; i < validArgsLen; i++) {
         joe_Object_assign (JOE_AT(varcontent, i+1), args[i]);
      }
   }
   rc = my_exec_sub (self, 0, retval);
   return rc;
}

static int
my_exec (joe_Object self, int argc, joe_Object *args, joe_Object *retval)
{
   unsigned int i;
   int rc = JOE_SUCCESS;
   joe_Array varcontent = *JOE_AT(self, VARCONTENT);
   int varcontentlen = joe_Array_length(varcontent);
   joe_Array savevar =  0;

   if (varcontentlen) {
      joe_Array argsNames = *JOE_AT(self, ARGS_NAMES);
      unsigned int argsNamesLen = joe_Array_length (argsNames);
      unsigned int validArgsLen = argsNamesLen < argc ? argsNamesLen : argc;

      joe_Object_assign (&savevar, joe_Array_New(varcontentlen));
 
      for (i = 0; i < varcontentlen; i++) {
         joe_Object_assign (JOE_AT(savevar,i), *JOE_AT(varcontent, i));
      }
      if (argsNamesLen) {
         for (i = 0; i < validArgsLen; i++) {
            joe_Object_assign (JOE_AT (varcontent, i+1), args[i]);
         }
         for ( ; i < argsNamesLen; i++) {
            joe_Object_assign (JOE_AT(varcontent, i+1), joe_Null_value);
         }
      }
   }

   rc = my_exec_sub (self, 0, retval);

   if (varcontentlen) {
      for (i = 0; i < varcontentlen; i++) {
         joe_Object_transfer (JOE_AT(varcontent, i), JOE_AT(savevar,i));
      }
      joe_Object_assign (&savevar, 0);
   }

   return rc;
}

int
joe_Block_outer_exec (joe_Object self, int argc, joe_Object *args, joe_Object *retval)
{
   unsigned int i;
   int rc = JOE_SUCCESS;
   joe_Array argsNames = *JOE_AT(self, ARGS_NAMES);
   unsigned int argsNamesLen = joe_Array_length (argsNames);
   unsigned int validArgsLen = argsNamesLen < argc ? argsNamesLen : argc;

   if (argsNamesLen) {
      joe_Array varcontent = *JOE_AT(self, VARCONTENT);
      for (i = 0; i < validArgsLen; i++) {
         joe_Object_assign (JOE_AT(varcontent, i+1), args[i]);
      }
      for ( ; i < argsNamesLen; i++) {
         joe_Object_assign (JOE_AT(varcontent, i+1), joe_Null_value);
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
   return rc;
}

int
joe_Block_new (joe_Object self, int argc, joe_Object *args, joe_Object *retval)
{
   int rc;
   joe_JOEObject jobj = 0;

   joe_Object_assign(&jobj, joe_JOEObject_New(self, *JOE_AT(self, PARENT)));

   rc = init (jobj, argc, args, retval);

   if (rc == JOE_SUCCESS)
      joe_Object_transfer(retval, &jobj);
   else
      joe_Object_assign(&jobj, 0);

   return rc;
}

int
joe_Block_exec (joe_Object self, int argc, joe_Object *args, joe_Object *retval)
{
   int rc;
   rc = my_exec (self, argc, args, retval);
   return rc;
}

joe_Block
joe_Block_clone (joe_Block self, joe_Block parent)
{
   joe_Block Return;
   joe_Array messNew, messOld;
   unsigned int len;
   int i;
   joe_Array newcontent;
   int contentlen = joe_Array_length(*JOE_AT(self, VARCONTENT));
   if (parent == 0)
      parent = self;

   Return = joe_Object_clone (self);
   joe_Object_assign (JOE_AT(Return, PARENT),parent);
   newcontent = joe_Array_New(contentlen);
   joe_Object_assign(JOE_AT(Return, VARCONTENT), newcontent);

   messOld = *JOE_AT(self, MESSAGES);
   len = joe_Array_length (messOld);   
   joe_Object_assign (JOE_AT(Return, MESSAGES),
                       messNew = joe_Array_New(len));
   for (i = 0; i < len; i++) {
      joe_Object_assign (JOE_AT(messNew, i),
                joe_Message_clone (*JOE_AT(messOld,i), Return));
   }
   return Return;
}

static int
getName(joe_JOEObject self, int argc, joe_Object* argv, joe_Object* retval)
{
   joe_Object_assign(retval, joe_Block_getName (self));
   return JOE_SUCCESS;
}

static void
getAllVars (joe_JOEObject blk, joe_HashMap hashvars)
{
   joe_Object oldValue = 0;
   joe_HashMap vars = *JOE_AT(blk, VARIABLES);
   joe_Block parent = *JOE_AT(blk, PARENT);

   if (vars != NULL) {
      int i;
      int len;
      joe_String name;
      joe_Array varArray = 0;
      joe_Object_assign(&varArray, joe_HashMap_keys (vars));
      len = joe_Array_length(varArray);
      for (i = 0; i < len; i++) {
         name = *JOE_AT(varArray, i);
         if (strcmp(joe_String_getCharStar(name),"!!")) {
            joe_HashMap_put(hashvars, name, 0, &oldValue);
            joe_Object_assign (&oldValue, 0);
         }
      }
      joe_Object_assign(&varArray, 0);
   }
   if (parent)
      getAllVars(parent, hashvars);
}

static int
getVariablesNames(joe_JOEObject self, int argc, joe_Object* argv, joe_Object* retval)
{
   joe_HashMap hashvars = 0;

   joe_Object_assign(&hashvars, joe_HashMap_New(8));

   getAllVars (self, hashvars);

   joe_Object_assign(retval, joe_HashMap_keys(hashvars));
   joe_Object_assign(&hashvars, 0);
   return JOE_SUCCESS;
}

static int
getVariable(joe_JOEObject self, int argc, joe_Object* argv, joe_Object* retval)
{
   if (argc == 1 && joe_Object_instanceOf(argv[0], &joe_String_Class)) {
      int rc;
      joe_Variable var = 0;
      joe_Object_assign(&var,
                       joe_Block_getVar(self, joe_String_getCharStar(argv[0])));
      if (var == 0) {
         char *error = calloc (1, 32 + joe_String_length(argv[0]));
         strcpy (error, "variable not found: ");
         strcat (error, joe_String_getCharStar(argv[0]));
         joe_Object_assign(retval, joe_Exception_New(error));
         rc = JOE_FAILURE;
         free (error);
      } else {
         rc = joe_Block_getVarValue(self, var, retval);
         joe_Object_assign(&var, 0);
      }
      return rc;
   } else {
      joe_Object_assign(retval,
                        joe_Exception_New("getVariable: invalid argument(s)"));
      return JOE_FAILURE;
   }
   return JOE_SUCCESS;
}

static int
setVariable(joe_JOEObject self, int argc, joe_Object* argv, joe_Object* retval)
{
   if (argc == 2 && joe_Object_instanceOf(argv[0], &joe_String_Class)) {
      joe_Variable var = joe_Block_getSetVar(self,
                                             joe_String_getCharStar (argv[0]));
      joe_Block_setVarValue (self, var, argv[1]);
      joe_Object_assign(retval, argv[1]);
   } else {
      joe_Object_assign(retval,
                        joe_Exception_New("setVariable: invalid argument(s)"));
      return JOE_FAILURE;
   }
   return JOE_SUCCESS;
}

static joe_Method mthds[] = {
  {"init",     init },
  {"exec",     joe_Block_exec },
  {"multiply", joe_Block_exec },
  {"new", joe_Block_new },
  {"add", joe_Block_new },
  {"name", getName },
  {"getName", getName },
  {"setVariable", setVariable },
  {"getVariable", getVariable },
  {"getVariablesNames", getVariablesNames },
  {(void *) 0, (void *) 0}
};


joe_Class joe_Block_Class = {
   "joe_Block",
   0,
   0,
   mthds,
   varvalues,
   &joe_Object_Class,
   0
};

joe_Object
joe_Block_getBang (joe_Block self)
{
   return *JOE_AT(self, BANG);
}

void
joe_Block_setBang (joe_Block self, joe_Object bang)
{
   joe_Object_assign (JOE_AT(self, BANG), bang);
}

void
joe_Block_Init (joe_Block self, joe_Block parent)
{
   joe_Object* thisVars = joe_Object_array(self);
   joe_HashMap hashvars = joe_HashMap_New(8);
   joe_String bangBang = 0;

   joe_Object_assign(&thisVars[MESSAGES], joe_Array_New(0));

   joe_Object_assign(&thisVars[PARENT], parent);
   if (parent) {
      joe_Object_assign(&thisVars[BANG], joe_Block_getBang(parent));
   } else {
      joe_Object_assign(&thisVars[BANG], joe_Bang_New());
   }
   joe_Object_assign(&thisVars[NAME], 0);
   joe_Object_assign(&thisVars[ARGS_NAMES], joe_Array_New(0));

   joe_Object_assign(&thisVars[VARIABLES], hashvars);
   joe_Object_assign(&thisVars[VARCONTENT], joe_Array_New(0));

   joe_Object_assign(&bangBang, joe_String_New ("!!"));
   joe_Block_setVar (self,  bangBang);
   joe_Object_assign(&bangBang, 0);
}

joe_Block
joe_Block_New (joe_Block parent)
{
   joe_Object self;
   self = joe_Object_New (&joe_Block_Class, 0);
   joe_Block_Init (self, parent);
   return self;
}

joe_String
joe_Block_getName(joe_Block self)
{
   joe_String Return = *JOE_AT(self,NAME);
   if (!Return) {
      char buffer[32];
      snprintf (buffer, sizeof(buffer),"block-%p", (void *) self);
      joe_Block_setName (self, buffer);
      Return= *JOE_AT(self,NAME);
   }
   return Return;
}

void
joe_Block_addMessage (joe_Block self, joe_Message msg)
{
   joe_Array newArray = 0;
   joe_Array_add (*JOE_AT(self,MESSAGES), msg, &newArray);
   joe_Object_transfer (JOE_AT(self,MESSAGES), &newArray);
}

joe_Object
joe_Block_varValue(joe_Object self, joe_Variable var)
{
   int index = joe_Variable_getIndex(var);
   if (index == 0) {
      return self;
   } else {
      joe_Array varcontent;
      int depth = joe_Variable_getDepth(var);
      joe_Block block = self;

      for ( ;depth > 0; depth--) {
         block = *JOE_AT(block, PARENT);
      }
      varcontent = *JOE_AT(block, VARCONTENT);

      return *JOE_AT(varcontent, index);
   }
} 

int
joe_Block_getVarValue (joe_Object self, joe_Variable var, joe_Object* retval)
{
   joe_Object_assign (retval, joe_Block_varValue(self, var));
   return JOE_SUCCESS;
}

void
joe_Block_setVarValue (joe_Block self, joe_Variable var, joe_Object value)
{
   joe_Array varcontent;
   int index = joe_Variable_getIndex(var);
   int depth = joe_Variable_getDepth(var);
   joe_Block block = self;

   for ( ;depth > 0; depth--) {
      block = *JOE_AT(block, PARENT);
   }
   varcontent = *JOE_AT(block, VARCONTENT);

   joe_Object_assign (JOE_AT(varcontent, index),value);
}

static joe_Variable
recurGetVar (joe_Block self, joe_String name, int *depth)
{
   joe_Variable Return = 0;
   joe_HashMap hashvars = *JOE_AT(self,VARIABLES);
   if (!joe_HashMap_get (hashvars, name, &Return)) {
      joe_Block parent = *JOE_AT(self, PARENT);
      if (parent) {
         (*depth)++;
         Return = recurGetVar (parent, name, depth);
      }
   }
   return Return;
}


static joe_Variable
joe_Block_setVarDepthIndex (joe_Block self,joe_String name,int depth,int index)
{
   joe_Object oldValue = 0;
   joe_Variable Return;
   joe_HashMap hashvars = *JOE_AT(self,VARIABLES);
   Return = joe_Variable_New_String (name, depth, index);
   joe_HashMap_put (hashvars, name, Return, &oldValue);
   joe_Object_assign (&oldValue, 0);
   return Return;
}

joe_Variable
joe_Block_getVar (joe_Block self, char *c)
{
   joe_Variable Return;
   int depth = 0;
   joe_String name = 0;
   joe_Object_assign(&name, joe_String_New (c));
   joe_Variable var = recurGetVar (self, name, &depth);
   if (var) {
      int index = joe_Variable_getIndex(var);
      depth += joe_Variable_getDepth(var);
      if (depth == 0) {
         Return = var;
      } else {
         Return = joe_Block_setVarDepthIndex (self, name, depth, index);
      }
   } else {
      Return = 0;
   }

   joe_Object_assign(&name, 0);

   return Return;
}


joe_Variable
joe_Block_setVar (joe_Block self,  joe_String name)
{
   joe_Variable Return;
   joe_Array newArray = 0;
   unsigned int index = joe_Array_length(*JOE_AT(self,VARCONTENT));

   Return = joe_Block_setVarDepthIndex (self, name, 0, index);
   joe_Array_add (*JOE_AT(self,VARCONTENT), joe_Null_value, &newArray);
   joe_Object_transfer (JOE_AT(self,VARCONTENT), &newArray);

   return Return;
}

joe_Variable
joe_Block_getSetVar (joe_Block self, char *c)
{
   joe_Variable Return;
   int depth = 0;
   joe_String name = 0;
   joe_Object_assign(&name, joe_String_New (c));
   joe_Variable var = recurGetVar (self, name, &depth);
   if (var == 0) {
      Return = joe_Block_setVar (self, name);
   } else {
      int index = joe_Variable_getIndex(var);
      depth += joe_Variable_getDepth(var);
      if (depth == 0) {
         Return = var;
      } else {
         Return = joe_Block_setVarDepthIndex (self, name, depth, index);
      }
   }
   joe_Object_assign(&name, 0);
   return Return;
}

void
joe_Block_setName (joe_Block self, char *name)
{
   joe_Object_assign (JOE_AT(self,NAME), joe_String_New (name));
}

void
joe_Block_addArgName (joe_Block self, char *c)
{
   joe_String name = joe_String_New (c);
   joe_Array newArray = 0;
   joe_Array_add (*JOE_AT(self,ARGS_NAMES), name, &newArray);
   joe_Object_transfer (JOE_AT(self,ARGS_NAMES), &newArray);
   joe_Block_setVar (self, name);
}

void
joe_Block_removeMessages (joe_Block self)
{
   joe_Object_assign (JOE_AT(self, MESSAGES), joe_Array_New(0));
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
getClass (joe_JOEObject self, int argc, joe_Object *argv, joe_Object *retval)
{
   joe_Object_assign(retval, *JOE_AT(self,PARENT));
   return JOE_SUCCESS;
}

static joe_Method JOEObject_mthds[] = {
  {"getClass", getClass },
  {"extends", extends },
  {(void *) 0, (void *) 0}
};

joe_Class joe_JOEObject_Class = {
   "joe_JOEObject",
   0,
   0,
   JOEObject_mthds,
   JOEObject_variables,
   &joe_Object_Class,
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
   joe_JOEObject super;
   joe_String sel = 0;
   joe_Variable var = 0;
   joe_HashMap variables = *JOE_AT(self,VARIABLES);

   joe_Object_assign (&sel, joe_String_New(selector));
   joe_HashMap_get (variables, sel, &var);
   joe_Object_assign (&sel, 0);

   if (var != 0) {
      joe_Object receiver = joe_Block_varValue (self,var);
      if (joe_Object_instanceOf (receiver, &joe_Block_Class))
         return receiver;
   }

   super = *JOE_AT(self,SUPER);
   if (super)
      return joe_JOEObject_getReceiver (super, selector);

   return 0;
}
