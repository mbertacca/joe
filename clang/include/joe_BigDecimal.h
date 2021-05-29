/*
 * This source file is part of the "Joe Objects Executor" open source project
 *
 * Copyright 2020 Marco Bertacca (www.bertacca.eu)
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

# ifndef joe_BigDecimal_h
# define joe_BigDecima_h  "$Id$";
# include "joe_Object.h"
# include "ndecimal.h"

typedef joe_Object joe_BigDecimal;
extern joe_Class joe_BigDecimal_Class;
JOEOBJ_API long joe_BigDecimal_value(joe_BigDecimal self);

joe_Object joe_BigDecimal_New (nDecimal bd);
int joe_BigDecimal_bigDecimalValue (joe_Object self, joe_Object *retval);
nDecimal get_nDecimal(joe_BigDecimal self);
joe_Object joe_BigDecimal_New_str(char* num);
joe_Object joe_BigDecimal_New_lng(long num);
joe_Object joe_BigDecimal_New_dbl(double num);
joe_Object joe_BigDecimal_New_dbl(double num);
enum joe_BigDecimal_ops {
   ADD,
   SUBTRACT,
   MULTIPLY,
   DIVIDE,
   REMAINDER,
   EQUALS,
   LT,
   GT,
   LE,
   GE,
   NE
};

int joe_BigDecimal_oper(joe_Object self, int argc, joe_Object* argv,
                        joe_Object* retval, enum joe_BigDecimal_ops op);

# endif
