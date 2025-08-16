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

public class WInteger extends WLong {
   public WInteger (long n) {
      super (n);
   }
   public WInteger (String n) {
      super (n);
   }
   public Type type() {
      return Type.INT;
   }
   public Object getWrapped () {
      return new Integer ((int) value);
   }
   public WInteger add (WInteger n) {
      return new WInteger (value + n.intValue());
   }
   public WInteger subtract (WInteger n) {
      return new WInteger (value - n.intValue());
   }
   public WInteger multiply (WInteger n) {
      return new WInteger (value * n.intValue());
   }
   public WInteger divide (WInteger n) {
      return new WInteger (value / n.intValue());
   }
   public WInteger remainder (WInteger n) {
      return new WInteger (value % n.intValue());
   }
   public WLong and (WLong n) {
      return new WLong (longValue() & n.value);
   }
   public WInteger and (WInteger n) {
      return new WInteger (value & n.value);
   }
   public WLong or (WLong n) {
      return new WLong (longValue() | n.value);
   }
   public WLong or (WInteger n) {
      return new WInteger (value | n.value);
   }
   public WLong xor (WLong n) {
      return new WLong (longValue() ^ n.value);
   }
   public WLong xor (WInteger n) {
      return new WInteger (value ^ n.value);
   }
   public WInteger not () {
      return new WInteger (~value);
   }
   public WLong shiftl (WInteger n) {
      return new WInteger (value << n.value);
   }
   public WLong shiftr (WInteger n) {
      return new WInteger (value >>> n.value);
   }
   public WLong shifta (WInteger n) {
      return new WInteger (value >> n.value);
   }
   public String toHexString () {
      return Integer.toHexString ((int) value);
   }
   public String toBinaryString () {
      return Integer.toBinaryString ((int) value);
   }
   public int signum() {
      return Integer.signum ((int) value);
   }
   public WInteger abs() {
      return value < 0 ? new WInteger(-value) : this;
   }
   public WInteger negate() {
      return new WInteger(-value);
   }
   public WInteger pow(WInteger e) {
      if (e.value < 0)
         throw new ArithmeticException ("Invalid exponent " + e);
      else if (e.value == 0)
         return new WInteger(1);
      else {
         long Return = value;
         for  (int i = (int) e.value - 1; i > 0; i--)
            Return *= value;
         return new WInteger(Return);
      }
   }
}

