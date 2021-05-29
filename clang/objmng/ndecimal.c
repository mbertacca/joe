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

# include "ndecimal.h"

# include <ctype.h>
# include <math.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>

# define DECIMAL_POINT '.'

typedef struct s_nDecimal {
   short precision;
   short scale;
   short negative;
} * nDecimal;

# define GETDATA(a) (((char*) a) + sizeof (struct s_nDecimal))

static void
setZero (nDecimal self)
{
   memset (GETDATA(self), '0', self->precision);
   self->negative = 0;
}

static int
check (nDecimal self)
{
   char *data;
   for (data = GETDATA(self); *data; data++)
      if (!isdigit(*data))
         return 0;
   return 1;
}

int
nDecimal_isZero (nDecimal self)
{
   char *data;
   for (data = GETDATA(self); *data; data++)
      if (*data != '0')
         return 0;
   return 1;
}

int
nDecimal_isOne (nDecimal self)
{
   int i;
   char *data = GETDATA(self);
   char *one = &data[self->precision - self->scale - 1];
   char *end = &data[self->precision];

   if (one < data)
      return 0;
   for ( ; data < one; data++)
      if (*data != '0')
         return 0;
   if (*data != '1')
      return 0;
   for (++data; data < end; data++)
      if (*data != '0')
         return 0;
   return 1;
}

int
nDecimal_memsize (nDecimal self)
{
   return sizeof (struct s_nDecimal) + self->precision + 1;
}

int
nDecimal_wrongAssignment (nDecimal self)
{
   return self->negative && nDecimal_isZero (self);
}

nDecimal
nDecimal_new (int precision, int scale)
{
   nDecimal self = calloc (1, sizeof (struct s_nDecimal) + precision + 1);
   self->precision = precision;
   self->scale = scale;
   setZero (self);
   return self;
}

void
nDecimal_delete (nDecimal self)
{
   free (self);
}

nDecimal
nDecimal_clone (nDecimal self)
{
   int size = sizeof (struct s_nDecimal) + self->precision + 1;
   nDecimal clone = malloc (size);
   memcpy (clone, self, size);
   return clone;
}

nDecimal
nDecimal_assign (nDecimal self, nDecimal src)
{
   int i, j;
   int self_intPart = self->precision - self->scale;
   int src_intPart = src->precision - src->scale;
   int xs = self_intPart - src_intPart;
   char *self_data = GETDATA(self);
   char *src_data = GETDATA(src);

   if (xs < 0) {
      i = 0;
      j = -xs;
   } else {
      for (i = 0; i < xs; i++)
         self_data[i] = '0';
      j = 0;
   }
   for ( ;i < self->precision && j < src->precision; i++, j++)
      self_data[i] = src_data[j];
   for ( ;i < self->precision; i++)
      self_data[i] = '0';
   self->negative = src->negative;
   return self;
}

nDecimal
nDecimal_assign_str (nDecimal self, const char * str)
{
   int i, j, iDot, xs;
   int locLeft;
   char *data = GETDATA(self);

   setZero(self);
   if (str != 0 && *str != 0) {
      int str_len = strlen (str);
      int intLen = self->precision - self->scale;

      for (i = 0; str[i] <= ' '; i++)
         ;
      if (str[i] == '-') {
         self->negative = 1;
         i++;
      } else if (str[i] == '+')
         i++;
      while (str[i] == '0')
         i++;

      for (iDot = i; iDot < str_len; iDot++) 
         if (str[iDot] == DECIMAL_POINT) 
            break;

      int xs = intLen - iDot + i;
      if (xs < 0) {
         j = 0;
         i -= xs;
      } else {
         j = xs;
      }

      for ( ; i < str_len && j < self->precision; i++) 
         if (i != iDot)
            data[j++] = str[i];
      if (!check(self)) {
         setZero(self);
         self->negative = 1;
      } else if (self->negative && nDecimal_isZero (self)) {
         self->negative = 0;
      }
   }
   
   return self;
}

nDecimal
nDecimal_new_str (const char * str)
{
   nDecimal self;
   int i, j, iDot, xs;
   int locLeft;

   if (str != 0 && *str != 0) {
      int str_len = strlen (str);
      for (i = 0; str[i] <= ' '; i++)
         ;
      if (str[i] == '-') {
         i++;
      } else if (str[i] == '+')
         i++;
      while (str[i] == '0')
         i++;

      for (iDot = i; iDot < str_len; iDot++) {
         if (str[iDot] == DECIMAL_POINT) {
            iDot++;
            i++;
            break;
         }
      }
      if (str_len > i) {
         if (i == iDot)
            self = nDecimal_new (str_len - i + 1, str_len - iDot);
         else
            self = nDecimal_new (str_len - i, str_len - iDot);
         nDecimal_assign_str (self, str);
      } else {
         self = nDecimal_new (1, 0);
      }
   } else {
      self = nDecimal_new (1, 0);
   }
   return self;
}

nDecimal
nDecimal_new_dbl (double dbl)
{
   nDecimal self = 0;
   char *buffer = calloc (1, 320);
   char *c;

   snprintf (buffer, 320, "%.8f", dbl);
   for (c = &buffer[strlen (buffer) - 1]; *c != '.'; c--)
      if (*c == '0')
         *c = '\0';
      else
         break;
   if (*c == '.')
      *c = '\0';
               
   self = nDecimal_new_str (buffer);

   free (buffer);
   return self;
}

nDecimal
nDecimal_assign_dbl (nDecimal self, double dbl)
{
   char *buffer = calloc (1, 320);

   snprintf (buffer, 320, "%.8f", dbl);
               
   self = nDecimal_assign_str (self, buffer);

   free (buffer);
   return self;
}

nDecimal
nDecimal_new_lng (long lng)
{
   nDecimal self = 0;
   char *buffer = calloc (1, 32);
 
   snprintf (buffer, 32, "%ld", lng);

   self = nDecimal_new_str (buffer);

   free (buffer);
   return self;
}

nDecimal
nDecimal_assign_lng (nDecimal self, long lng)
{
   char *buffer = calloc (1, 32);
 
   snprintf (buffer, 32, "%ld", lng);
   self = nDecimal_assign_str (self, buffer);

   free (buffer);
   return self;
}

static nDecimal
_minimize (nDecimal aNum)
{
   int precision = aNum->precision;
   int scale = aNum->scale;
   int intPart = precision - scale;
   char *data = GETDATA(aNum);
   int i;

   for (i = precision - 1; scale > 0; i--, scale--, precision--)
      if (data[i] != '0')
         break;
   for (i = 0; intPart > 1; i++, intPart--, precision--)
      if (data[i] != '0')
         break;
   if (precision == aNum->precision) {
      return aNum;
   } else {
      nDecimal newNum = nDecimal_new (precision, scale);
      nDecimal_assign (newNum, aNum);
      nDecimal_delete (aNum);
      return newNum;
   }
}


static nDecimal
minimize (nDecimal aNum)
{
   char *data = GETDATA(aNum);
   int intPart = aNum->precision - aNum->scale;
   int izeros = 0;
   char *c;

   for (c = &data[aNum->precision - 1]; aNum->scale > 0 && *c == '0';
        c--, aNum->scale--, aNum->precision--)
      *c = '\0';
   for (c = data; intPart > 1 && *c == '0';
        c++, intPart--, aNum->precision--)
      izeros++;
   if (izeros > 0) {
      memcpy (data, data + izeros, aNum->precision);
      data[aNum->precision] = 0;
   }

   return aNum;
}

static int
unsignedCompare (nDecimal n1, nDecimal n2)
{
   char *c1, *c2;
   int int1Part = n1->precision - n1->scale;
   int int2Part = n2->precision - n2->scale;

   for (c1 = GETDATA(n1); int1Part > 1; c1++, int1Part--)
      if (*c1 != '0')
         break;
   for (c2 = GETDATA(n2); int2Part > 1; c2++, int2Part--)
      if (*c2 != '0')
         break;
   if (int1Part != int2Part) {
      return int1Part > int2Part ? 1 : -1;
   } else {
      for ( ; *c1 && *c2; c1++, c2++)
         if (*c1 != *c2)
            return *c1 > *c2 ? 1 : -1;;
      for ( ;*c1; c1++)
         if (*c1 != '0')
            return 1;
      for ( ;*c2 ; c2++)
         if (*c2 != '0')
            return -1; 
      return 0;
   }
}


int
nDecimal_compareTo (nDecimal n1, nDecimal n2)
{
   if (n1->negative)
      if (n2->negative)
         return unsignedCompare (n2, n1);
      else
         return -1;
   else if (n2->negative)
      return 1;
   else
      return unsignedCompare (n1, n2);
}

static nDecimal
unsignedSubtract (nDecimal add1, nDecimal add2)
{
   int i1, i2, is;
   int int1Part = add1->precision - add1->scale;
   int int2Part = add2->precision - add2->scale;
   int scl1Part = add1->scale;
   int scl2Part = add2->scale;
   int scale = scl1Part > scl2Part ? scl1Part : scl2Part;
   int precision = (int1Part > int2Part ? int1Part : int2Part) + scale;
   nDecimal self = nDecimal_new (precision, scale);
   char *add1_data = GETDATA(add1);
   char *add2_data = GETDATA(add2);
   char *self_data = GETDATA(self);
   int carry = 0;
   char c = 0;

   is = precision - 1;
   i1 = add1->precision - 1;
   i2 = add2->precision - 1;

   if (scl1Part > scl2Part) {
      for ( ; scl1Part > scl2Part; scl1Part--)
         self_data[is--] = add1_data[i1--];
   } else {
      for ( ; scl1Part < scl2Part; scl2Part--) {
         c = '0' - (add2_data[i2--] & 0x0F) - carry;
         if (c < '0') {
            c += 10;
            carry = 1;
         } else {
            carry = 0;
         }
         self_data[is--] = c;
     }
   }
   for ( ; i1 >= 0 && i2 >= 0; i1--, i2--, is--) {
      c = add1_data[i1] - (add2_data[i2] & 0x0F) - carry;
      if (c < '0') {
         c += 10;
         carry = 1;
      } else {
         carry = 0;
      }
      self_data[is] = c;
   }
   if (i1 >= 0) {
      for ( ; i1 >= 0; i1--, is--) {
         c = add1_data[i1] - carry;
         if (c < '0') {
            c += 10;
            carry = 1;
         } else {
           carry = 0;
         }
         self_data[is] = c;
      }
   } else {
      for ( ; i2 >= 0; i2--, is--) {
         c = add2_data[i2] - carry;
         if (c < '0') {
            c += 10;
            carry = 1;
         } else {
           carry = 0;
         }
         self_data[is] = c;
      }
   }
   return minimize(self);
}

static nDecimal
unsignedAdd (nDecimal add1, nDecimal add2)
{
   int i1, i2, is;
   int int1Part = add1->precision - add1->scale;
   int int2Part = add2->precision - add2->scale;
   int scl1Part = add1->scale;
   int scl2Part = add2->scale;
   int scale = scl1Part > scl2Part ? scl1Part : scl2Part;
   int precision = (int1Part > int2Part ? int1Part : int2Part) + scale + 1;
   nDecimal self = nDecimal_new (precision, scale);
   char *add1_data = GETDATA(add1);
   char *add2_data = GETDATA(add2);
   char *self_data = GETDATA(self);
   int carry = 0;
   char c = 0;

   is = precision - 1;
   i1 = add1->precision - 1;
   i2 = add2->precision - 1;

   if (scl1Part > scl2Part) {
      for ( ; scl1Part > scl2Part; scl1Part--)
         self_data[is--] = add1_data[i1--];
   } else {
      for ( ; scl1Part < scl2Part; scl2Part--)
         self_data[is--] = add2_data[i2--];
   }
   for ( ; i1 >= 0 && i2 >= 0; i1--, i2--, is--) {
      c = add1_data[i1] + (add2_data[i2] & 0x0F) + carry;
      if (c > '9') {
         c -= 10;
         carry = 1;
      } else {
         carry = 0;
      }
      self_data[is] = c;
   }
   if (i1 >= 0) {
      for ( ; i1 >= 0; i1--, is--) {
         c = add1_data[i1] + carry;
         if (c > '9') {
            c -= 10;
            carry = 1;
         } else {
           carry = 0;
         }
         self_data[is] = c;
      }
   } else {
      for ( ; i2 >= 0; i2--, is--) {
         c = add2_data[i2] + carry;
         if (c > '9') {
            c -= 10;
            carry = 1;
         } else {
           carry = 0;
         }
         self_data[is] = c;
      }
   }
   if (carry == 1)
     self_data[is] = '1';
   else
     self_data[is] = '0';
   return minimize(self);
}

static nDecimal
addSub (nDecimal op1, int isneg1, nDecimal op2, int isneg2)
{
   nDecimal Return;
   if (isneg1 == isneg2) {
      Return = unsignedAdd (op1, op2);
      Return->negative = isneg1;
   } else {
      int cmp = unsignedCompare (op1, op2);
      if (cmp > 0) {
         Return = unsignedSubtract (op1, op2);
         Return->negative = isneg1;
      } else if (cmp < 0) {
         Return =  unsignedSubtract (op2, op1);
         Return->negative = isneg2;
      } else {
         Return = nDecimal_new (1, 0);
      }
   }
   return Return;
}

nDecimal
nDecimal_add (nDecimal add1, nDecimal add2)
{
   return addSub (add1, add1->negative, add2, add2->negative);
}

nDecimal
nDecimal_subtract (nDecimal sub1, nDecimal sub2)
{
   return addSub (sub1, sub1->negative, sub2, !sub2->negative);
}

nDecimal
nDecimal_multiply (nDecimal op1, nDecimal op2)
{
   char *n1ptr, *n2ptr, *selfptr, *iptr;
   char *op1_data = GETDATA(op1);
   char *op2_data = GETDATA(op2);
   char *self_data;
   int fact = 0;

   nDecimal self = nDecimal_new (op1->precision + op2->precision,
                                 op1->scale + op2->scale);
   self_data = GETDATA(self);

   selfptr = self_data + self->precision;

   for (n1ptr =  op1_data + op1->precision - 1; n1ptr >= op1_data; n1ptr--) {
      iptr = --selfptr;
      for (n2ptr = op2_data + op2->precision - 1; n2ptr >= op2_data; n2ptr--) {
         fact = (*n2ptr & 0x0F) * (*n1ptr & 0x0F) + fact + (*iptr & 0x0F);
         *iptr-- = (char) ('0' + fact % 10);
         fact /= 10;
      }
      *iptr = '0' + fact;
      fact = 0;
   }
   self->negative = op1->negative != op2->negative;
   minimize (self);
   return self;
}

static void
zonedMemcpy (char *dst, char *src, int len)
{
   for ( ; len > 0; len--)
      *dst++ = (char) (*src++ & 0x0F);
}

static void
_one_mult (char *num, int size, int digit, char *result)
{
   int carry, value;
   unsigned char *nptr, *rptr;

   if (digit == 0)
      memset (result, 0, size);
   else {
      if (digit == 1)
         memcpy (result, num, size);
      else {
         /* Initialize */
         nptr = (unsigned char *) (num+size-1);
         rptr = (unsigned char *) (result+size-1);
         carry = 0;
   
         while (size-- > 0) {
            value = *nptr-- * digit + carry;
            *rptr-- = value % 10;
            carry = value / 10;
         }
   
         if (carry != 0)
            *rptr = carry;
      }
   }
}
nDecimal
nDecimal_divide (nDecimal op1, nDecimal op2, int scale)
{
   nDecimal Return;
   char *num1, *num2, *mval;
   char *qptr, *n1ptr, *n2ptr, *ptr1, *ptr2;
   int prec1, prec2;
   int intPart1, scale1, intPart2, scale2;
   int extra, qdigits, norm, qdig, qguess, borrow, carry, val, count;
   char *op1_data = GETDATA(op1);
   char *op2_data = GETDATA(op2);
   char *Return_data;

   if (nDecimal_isZero (op2)) {
      Return = nDecimal_new (1, 0);
      Return->negative = 1;
      return Return;
   }
   if (nDecimal_isOne (op2)) {
      Return = nDecimal_clone (op1);
      Return->negative = op1->negative != op2->negative;
      return Return;
   }
   intPart1 = op1->precision - op1->scale;
   intPart2 = op2->precision - op2->scale;
   scale2 = op2->scale;
   n2ptr = (char *) op2_data + op2->precision;
   for (--n2ptr; (scale2 > 0) && (*n2ptr == '0'); --n2ptr)
      scale2--;

   prec1 = intPart1 + scale2;
   scale1 = op1->scale - scale2;
   if (scale1 < scale)
      extra = scale - scale1;
   else
      extra = 0;

   prec2 = intPart2 + scale2;

   for (n2ptr = op2_data; *n2ptr == '0'; n2ptr++)
      prec2--;

   if (prec2 > prec1 + scale) {
      Return = nDecimal_new (1, 0); /* divisor is too big */
      return Return;
   }

   num1 = calloc (1, op1->precision + extra + 2);
   zonedMemcpy (num1 + 1, op1_data, op1->precision);

   num2 = calloc (1, prec2 + 1);
   zonedMemcpy (num2, op2_data, op2->precision);

   n2ptr = num2 + (n2ptr - op2_data);

   if (prec2 > prec1)
      qdigits = scale + 1;
   else
      qdigits = prec1 - prec2 + scale + 1;

   Return = nDecimal_new (qdigits, scale);
   Return_data = GETDATA(Return);
   memset (Return_data, 0, qdigits);
   mval = calloc (1, prec2 + 1);

   /* Normalize */
   norm =  10 / ((int)*n2ptr + 1);
   if (norm != 1) {
      _one_mult (num1, prec1+scale1+extra+1, norm, num1);
      _one_mult (n2ptr, prec2, norm, n2ptr);
   }

   qdig = 0;
   if (prec2 > prec1)
      qptr = Return_data + prec2 - prec1;
   else
      qptr = Return_data;

   while (qdig <= prec1 + scale - prec2) {
      if (*n2ptr == num1[qdig])
         qguess = 9;
      else
         qguess = (num1[qdig]*10 + num1[qdig+1]) / *n2ptr;

      if (n2ptr[1]*qguess > (num1[qdig] * 10 + num1[qdig+1] - *n2ptr * qguess)
                            * 10 + num1[qdig+2]) {
         qguess--;

         if (n2ptr[1]*qguess > (num1[qdig] * 10 + num1[qdig+1] - *n2ptr * qguess)
                                * 10 + num1[qdig+2])
            qguess--;
      }

      borrow = 0;
      if (qguess != 0) {
         *mval = 0;
         _one_mult (n2ptr, prec2, qguess, mval+1);
         ptr1 = num1 + qdig + prec2;
         ptr2 = mval + prec2;
         for (count = 0; count < prec2+1; count++) {
            val = (int) *ptr1 - (int) *ptr2-- - borrow;
            if (val < 0) {
               val += 10;
               borrow = 1;
            } else
               borrow = 0;
            *ptr1-- = val;
         }
      }

      if (borrow == 1) {
         qguess--;
         ptr1 = num1 + qdig + prec2;
         ptr2 = n2ptr + prec2 - 1;
         carry = 0;
         for (count = 0; count < prec2; count++) {
            val = (int) *ptr1 + (int) *ptr2-- + carry;
            if (val > 9) {
               val -= 10;
               carry = 1;
            } else
               carry = 0;
            *ptr1-- = val;
         }
         if (carry == 1)
            *ptr1 = (*ptr1 + 1) % 10;
      }
       
      *qptr++ =  qguess;
      qdig++;
   }
    for (qptr = Return_data, count = 0; count < Return->precision;
         qptr++,count++)
       *qptr += '0'; 
   if (!nDecimal_isZero (Return))
      Return->negative = op1->negative != op2->negative;
    minimize (Return);

   free (mval);
   free (num1);
   free (num2);

   return Return;   
}

nDecimal
nDecimal_divideToInteger (nDecimal op1, nDecimal op2)
{
   nDecimal Return;
   int scale = op1->scale - op2->scale;
   char *ptr;

   if (scale < 0)
      scale = 0;
   Return =  nDecimal_divide (op1, op2, scale);
   ptr = GETDATA(Return) + Return->precision;
   for (--ptr; Return->scale > 0; --ptr,  Return->scale--,  Return->precision--)
      *ptr=0;
   if (nDecimal_isZero(Return))
     Return->negative = 0;
   return Return;
}

nDecimal
nDecimal_remainder (nDecimal op1, nDecimal op2)
{
   nDecimal div = nDecimal_divideToInteger (op1, op2);
   nDecimal mul = nDecimal_multiply (div, op2);
   nDecimal Return = nDecimal_subtract (op1, mul);

   nDecimal_delete (div);
   nDecimal_delete (mul);

   return Return;
}

char *
nDecimal_fillCharStar (nDecimal self, char *str)
{
   char *self_data = GETDATA(self);
   *str = 0;
   if (self->negative)
      strcat (str, "-");

   if (self->scale > 0) {
      int intLen = self->precision - self->scale;
      strncat (str, self_data, intLen);
      strcat (str, ".");
      strncat (str, &self_data[intLen], self->scale);
   } else {
      strcat (str, self_data);
   }
   return str;
}

char *
nDecimal_toString (nDecimal self)
{
   char *Return = calloc (1, self->precision + 3);
   nDecimal_fillCharStar (self, Return);
   return Return;
}

/*
int
main (int argc, char *argv[])
{
   char *str;
   nDecimal num = nDecimal_new (10, 2);
   nDecimal num2 = nDecimal_new_str (argv[1]);
   nDecimal num3;

   nDecimal_assign_str (num, "9999999999.999999999");

   nDecimal_assign (num, num2);
   str = nDecimal_toString (num);
   printf ("%s\n", str);
   free (num);
   free (num2);
   free (str);

   num = nDecimal_new_dbl (atof (argv[1]));
   str = nDecimal_toString (num);
   printf ("%s\n", str);
   free (num);
   free (str);


   num = nDecimal_new_lng (atol (argv[1]));
   str = nDecimal_toString (num);
   printf ("%s\n", str);
   free (num);
   free (str);

   num2 = nDecimal_new_dbl (1230.0000001);
   num = nDecimal_new_dbl (  0.00000001);
   num3 =nDecimal_subtract (num2, num);
   str = nDecimal_toString (num3);
   printf ("%s\n", str);
   free (num);
   free (num2);
   free (num3);
   free (str);
   return 0;
}
*/