/*
 * This source file is part of the "Joe Objects Executor" open source project
 *
 * Copyright 2021 Marco Bertacca (www.bertacca.eu)
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

# ifndef ndecimal_h
# define ndecimal_h

typedef struct s_nDecimal * nDecimal;

extern nDecimal nDecimal_new (int precision, int scale);
extern void nDecimal_delete (nDecimal self);
extern nDecimal nDecimal_new_str (const char * str);
extern nDecimal nDecimal_new_lng (long lng);
extern nDecimal nDecimal_new_dbl (double dbl);
extern nDecimal nDecimal_assign_str (nDecimal self, const char * str);
extern nDecimal nDecimal_assign_lng (nDecimal self, long lng);
extern nDecimal nDecimal_assign_dbl (nDecimal self, double dbl);
extern char * nDecimal_fillCharStar (nDecimal self, char *str);
extern char * nDecimal_toString (nDecimal self);
extern int nDecimal_isZero (nDecimal self);
extern int nDecimal_isOne (nDecimal self);
extern int nDecimal_wrongAssignment (nDecimal self);
extern int nDecimal_compareTo (nDecimal add1, nDecimal add2);
extern nDecimal nDecimal_add (nDecimal add1, nDecimal add2);
extern nDecimal nDecimal_subtract (nDecimal sub1, nDecimal sub2);
extern nDecimal nDecimal_multiply (nDecimal op1, nDecimal op2);
extern nDecimal nDecimal_divide (nDecimal op1, nDecimal op2, int scale);
extern nDecimal nDecimal_divideToInteger (nDecimal op1, nDecimal op2);
extern nDecimal nDecimal_remainder (nDecimal op1, nDecimal op2);
extern int nDecimal_memsize (nDecimal num);

# endif