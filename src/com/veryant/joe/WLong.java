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

public class WLong extends Wrapper implements WNumber {
   final long value;

   public WLong (long n) {
      value = n;
   }
   public WLong (String n) {
      value = Long.parseLong (n);
   }
   public Type type() {
      return Type.LONG;
   }
   public Object getWrapped () {
      return new Long (value);
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
   public WLong add (WLong n) {
      return new WLong (value + n.value);
   }
   public WLong add (WIntegers n) {
      return new WLong (value + n.longValue());
   }
   public WDouble add (WFloats n) {
      return new WDouble ((double) value + n.doubleValue());
   }
   public WLong subtract (WLong n) {
      return new WLong (value - n.value);
   }
   public WDouble subtract (WIntegers n) {
      return new WDouble (value - n.longValue());
   }
   public WDouble subtract (WFloats n) {
      return new WDouble ((double) value - n.doubleValue());
   }
   public WLong multiply (WLong n) {
      return new WLong (value * n.value);
   }
   public WLong multiply (WIntegers n) {
      return new WLong (value * n.longValue());
   }
   public WFloats multiply (WFloats n) {
      return new WDouble ((double) value * n.doubleValue());
   }
   public WLong divide (WLong n) {
      return new WLong (value / n.value);
   }
   public WLong divide (WIntegers n) {
      return new WLong (value / n.longValue());
   }
   public WDouble divide (WFloats n) {
      return new WDouble ((double) value / n.doubleValue());
   }
   public WLong mod (WLong n) {
      return new WLong (value % n.value);
   }
   public WLong mod (WIntegers n) {
      return new WLong (value % n.longValue());
   }
   public WDouble mod (WFloats n) {
      return new WDouble ((double) value % n.doubleValue());
   }

   public WBoolean equals (WLong n) {
      return new WBoolean (value == n.value);
   }
   public WBoolean equals (WNumber n) {
      return new WBoolean (value == n.longValue());
   }
   public WBoolean lt (WLong n) {
      return new WBoolean (value < n.value);
   }
   public WBoolean lt (WNumber n) {
      return new WBoolean (value < n.longValue());
   }
   public WBoolean gt (WLong n) {
      return new WBoolean (value > n.value);
   }
   public WBoolean gt (WNumber n) {
      return new WBoolean (value > n.longValue());
   }
   public WBoolean ge (WLong n) {
      return new WBoolean (value >= n.value);
   }
   public WBoolean ge (WNumber n) {
      return new WBoolean (value >= n.longValue());
   }
   public WBoolean le (WLong n) {
      return new WBoolean (value <= n.value);
   }
   public WBoolean le (WNumber n) {
      return new WBoolean (value <= n.longValue());
   }
   public WBoolean ne (WLong n) {
      return new WBoolean (value != n.value);
   }
   public WBoolean ne (WNumber n) {
      return new WBoolean (value != n.longValue());
   }
   public String toString() {
      return Long.toString (value);
   }
}
