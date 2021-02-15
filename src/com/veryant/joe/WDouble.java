/*
 *
 * This source file is part of the "Java Objects Executor" open source project
 *
 * Copyright 2017 Veryant and Marco Bertacca
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

public class WDouble extends WBaseNumber implements WNumber {
   final double value;

   public WDouble (double n) {
      value = n;
   }
   public WDouble (String n) {
      value = Double.parseDouble (n);
   }
   public Type type() {
      return Type.DOUBLE;
   }
   public Object getWrapped () {
      return new Double (value);
   }
   public byte byteValue() {
      return (byte) value;
   }
   public char charValue() {
      return (char) value;
   }
   public short shortValue() {
      return (short) value;
   }
   public int intValue() {
      return (int) value;
   }
   public long longValue() {
      return (long) value;
   }
   public float floatValue() {
      return (float) value;
   }
   public double doubleValue() {
      return (double) value;
   }
   public BigDecimal bigDecimalValue() {
      return new BigDecimal (value);
   }

   public WDouble add (WDouble n) {
      return new WDouble (value + n.value);
   }
   public WDouble add (WInteger n) {
      return new WDouble (value + n.value);
   }
   public WDouble add (WLong n) {
      return new WDouble (value + n.value);
   }

   public WDouble subtract (WDouble n) {
      return new WDouble (value - n.value);
   }
   public WDouble subtract (WInteger n) {
      return new WDouble (value - n.value);
   }
   public WDouble subtract (WLong n) {
      return new WDouble (value - n.value);
   }

   public WDouble multiply (WDouble n) {
      return new WDouble (value * n.value);
   }
   public WDouble multiply (WInteger n) {
      return new WDouble (value * n.value);
   }
   public WDouble multiply (WLong n) {
      return new WDouble (value * n.value);
   }

   public WDouble divide (WDouble n) {
      return new WDouble (value / n.value);
   }
   public WDouble divide (WInteger n) {
      return new WDouble (value / n.value);
   }
   public WDouble divide (WLong n) {
      return new WDouble (value / n.value);
   }

   public WDouble remainder (WDouble n) {
      return new WDouble (value % n.value);
   }
   public WDouble remainder (WInteger n) {
      return new WDouble (value % n.value);
   }
   public WDouble remainder (WLong n) {
      return new WDouble (value % n.value);
   }

   public WBoolean equals (WDouble n) {
      return new WBoolean (value == n.value);
   }
   public WBoolean equals (WInteger n) {
      return new WBoolean (value == n.value);
   }
   public WBoolean equals (WLong n) {
      return new WBoolean (value == n.value);
   }

   public WBoolean lt (WDouble n) {
      return new WBoolean (value < n.value);
   }
   public WBoolean lt (WInteger n) {
      return new WBoolean (value < n.value);
   }
   public WBoolean lt (WLong n) {
      return new WBoolean (value < n.value);
   }

   public WBoolean gt (WDouble n) {
      return new WBoolean (value > n.value);
   }
   public WBoolean gt (WInteger n) {
      return new WBoolean (value > n.value);
   }
   public WBoolean gt (WLong n) {
      return new WBoolean (value > n.value);
   }

   public WBoolean ge (WDouble n) {
      return new WBoolean (value >= n.value);
   }
   public WBoolean ge (WInteger n) {
      return new WBoolean (value >= n.value);
   }
   public WBoolean ge (WLong n) {
      return new WBoolean (value >= n.value);
   }

   public WBoolean le (WDouble n) {
      return new WBoolean (value <= n.value);
   }
   public WBoolean le (WInteger n) {
      return new WBoolean (value <= n.value);
   }
   public WBoolean le (WLong n) {
      return new WBoolean (value <= n.value);
   }

   public WBoolean ne (WDouble n) {
      return new WBoolean (value != n.value);
   }
   public WBoolean ne (WInteger n) {
      return new WBoolean (value != n.value);
   }
   public WBoolean ne (WLong n) {
      return new WBoolean (value != n.value);
   }

   public String toString() {
      return Double.toString (value);
   }
}
