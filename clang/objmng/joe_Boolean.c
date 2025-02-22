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

# include "joe_Boolean.h"
# include "joe_String.h"

# include <stdio.h>

/**
# Class joe_Boolean
### extends joe_Object

This class implements a boolean type.
*/

/* the implementation of the following methods are in joe_Object.c */
/**
## and _aBoolean_

AND boolean operation
*/

/**
## or _aBoolean_

OR boolean operation
*/

/**
## xor _aBoolean_

XOR boolean operation
*/

/**
## not

NOT operation
*/

/**
## iif _aObject1_, _aObject2_

If this boolean is <1> then this method return _aObject1_ else _aObject2_.
*/

/**
## ifTrue _aBlock1_ [, _aBlock2_]

If this boolean is <1> then this method executes _aBlock1_
else _aBlock2_ when supplied.
*/

/**
## ifFalse  _aBlock1_ [, _aBlock2_]

If this boolean is <0> then this method executes _aBlock1_
else _aBlock2_ when supplied.
*/

/**
## toString
Returns the string representation of this boolean.
*/

joe_Object
joe_Boolean_New_true ()
{
   return joe_Boolean_True;
}

joe_Object
joe_Boolean_New_false ()
{
   return joe_Boolean_False;
}

int
joe_Boolean_isTrue (joe_Object self)
{
   return  self == joe_Boolean_True;
}

int
joe_Boolean_isFalse (joe_Object self)
{
   return  self == joe_Boolean_False;
}
