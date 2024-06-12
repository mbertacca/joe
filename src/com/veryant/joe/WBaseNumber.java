/*
 *
 * This source file is part of the "Java Objects Executor" open source project
 *
 * Copyright 2017-2021 Marco Bertacca
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

package com.veryant.joe;

import java.math.BigDecimal;
import java.math.MathContext;

public abstract class WBaseNumber extends Wrapper implements WNumber {
   public BigDecimal add (WNumber n) {
      return bigDecimalValue().add(n.bigDecimalValue());
   }

   public BigDecimal subtract (WNumber n) {
      return bigDecimalValue().subtract(n.bigDecimalValue());
   }

   public BigDecimal multiply (WNumber n) {
      return bigDecimalValue().multiply(n.bigDecimalValue());
   }

   public BigDecimal divide (WNumber n) {
      return bigDecimalValue().divide(n.bigDecimalValue(),
                                      MathContext.DECIMAL128);
   }

   public BigDecimal remainder (WNumber n) {
      return bigDecimalValue().remainder(n.bigDecimalValue());
   }

   public boolean equals (Object n) {
      if (n instanceof WNumber)
         return bigDecimalValue().compareTo(((WNumber)n).bigDecimalValue()) == 0;
      else
         return false;
   }

   public WBoolean equals (WNumber n) {
      return bigDecimalValue().compareTo(n.bigDecimalValue()) == 0 ?
             WBoolean.TRUE : WBoolean.FALSE;
   }

   public WBoolean ne (WNumber n) {
      return bigDecimalValue().compareTo(n.bigDecimalValue()) != 0 ?
             WBoolean.TRUE : WBoolean.FALSE;
   }

   public WBoolean lt (WNumber n) {
      return bigDecimalValue().compareTo(n.bigDecimalValue()) < 0 ?
             WBoolean.TRUE : WBoolean.FALSE;
   }

   public WBoolean le (WNumber n) {
      return bigDecimalValue().compareTo(n.bigDecimalValue()) <= 0 ?
             WBoolean.TRUE : WBoolean.FALSE;
   }

   public WBoolean gt (WNumber n) {
      return bigDecimalValue().compareTo(n.bigDecimalValue()) > 0 ?
             WBoolean.TRUE : WBoolean.FALSE;
   }

   public WBoolean ge (WNumber n) {
      return bigDecimalValue().compareTo(n.bigDecimalValue()) >= 0 ?
             WBoolean.TRUE : WBoolean.FALSE;
   }
}