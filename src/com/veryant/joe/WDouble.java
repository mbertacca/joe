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

public class WDouble extends Wrapper implements WFloats {
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

   public WDouble add (WDouble n) {
      return new WDouble (value + n.value);
   }
   public WDouble add (WNumber n) {
      return new WDouble (value + n.doubleValue());
   }
   public WDouble subtract (WDouble n) {
      return new WDouble (value - n.value);
   }
   public WDouble subtract (WNumber n) {
      return new WDouble (value - n.doubleValue());
   }
   public WDouble multiply (WDouble n) {
      return new WDouble (value * n.value);
   }
   public WDouble multiply (WNumber n) {
      return new WDouble (value * n.doubleValue());
   }
   public WDouble divide (WDouble n) {
      return new WDouble (value / n.value);
   }
   public WDouble divide (WNumber n) {
      return new WDouble (value / n.doubleValue());
   }
   public WDouble mod (WDouble n) {
      return new WDouble (value % n.value);
   }
   public WDouble mod (WNumber n) {
      return new WDouble (value % n.doubleValue());
   }
   public WBoolean equals (WDouble n) {
      return new WBoolean (value == n.value);
   }
   public WBoolean equals (WNumber n) {
      return new WBoolean (value == n.doubleValue());
   }
   public WBoolean lt (WDouble n) {
      return new WBoolean (value < n.value);
   }
   public WBoolean lt (WNumber n) {
      return new WBoolean (value < n.doubleValue());
   }
   public WBoolean gt (WDouble n) {
      return new WBoolean (value > n.value);
   }
   public WBoolean gt (WNumber n) {
      return new WBoolean (value > n.doubleValue());
   }
   public WBoolean ge (WDouble n) {
      return new WBoolean (value >= n.value);
   }
   public WBoolean ge (WNumber n) {
      return new WBoolean (value >= n.doubleValue());
   }
   public WBoolean le (WDouble n) {
      return new WBoolean (value <= n.value);
   }
   public WBoolean le (WNumber n) {
      return new WBoolean (value <= n.doubleValue());
   }
   public WBoolean ne (WDouble n) {
      return new WBoolean (value != n.value);
   }
   public WBoolean ne (WNumber n) {
      return new WBoolean (value != n.doubleValue());
   }
   public String toString() {
      return Double.toString (value);
   }
}
