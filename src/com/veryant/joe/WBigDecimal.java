/*
 *
 * This source file is part of the "Java Objects Executor" open source project
 *
 * Copyright 2021 Marco Bertacca
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

public class WBigDecimal extends WBaseNumber implements WNumber {
   final BigDecimal value;

   public WBigDecimal (BigDecimal n) {
      value = n;
   }
   public WBigDecimal (Integer n) {
      value = new BigDecimal (n);
   }
   public WBigDecimal (Double n) {
      value = new BigDecimal (n);
   }
   public WBigDecimal (String n) {
      value = new BigDecimal (n);
   }
   public Type type() {
      return Type.DECIMAL;
   }
   public Object getWrapped () {
      return value;
   }
   public BigDecimal bigDecimalValue() {
      return value;
   }
   public byte byteValue() {
      return value.byteValue();
   }
   public char charValue() {
      return (char) value.intValue();
   }
   public short shortValue() {
      return value.shortValue();
   }
   public int intValue() {
      return value.intValue();
   }
   public long longValue() {
      return value.longValue();
   }
   public float floatValue() {
      return value.floatValue();
   }
   public double doubleValue() {
      return value.doubleValue();
   }
   public String toString() {
      return value.toPlainString ();
   }
   public BigDecimal abs() {
      return value.abs ();
   }
   public BigDecimal negate() {
      return value.negate ();
   }
   public BigDecimal pow(WInteger n) {
      return value.pow (n.intValue());
   }
   public int precision() {
      return value.precision ();
   }
   public int scale() {
      return value.scale ();
   }
   public BigDecimal movePointLeft (WInteger newScale) {
      return value.movePointLeft (newScale.intValue());
   }
   public BigDecimal movePointRight (WInteger newScale) {
      return value.movePointRight (newScale.intValue());
   }
   public BigDecimal setScale(WInteger newScale) {
      return value.setScale (newScale.intValue(), BigDecimal.ROUND_HALF_EVEN);
   }
   public BigDecimal setScale(WInteger newScale, WInteger roundingMode) {
      return value.setScale (newScale.intValue(), roundingMode.intValue());
   }
   public int signum() {
      return value.signum ();
   }
   public BigDecimal stripTrailingZeros() {
      return value.stripTrailingZeros ();
   }
}
