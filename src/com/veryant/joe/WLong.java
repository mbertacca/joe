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

public class WLong extends WBaseNumber {
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
   public java.math.BigDecimal bigDecimalValue() {
      return new java.math.BigDecimal (value);
   }
   public int signum() {
      return Long.signum (value);
   }

   public WLong add (WLong n) {
      return new WLong (value + n.value);
   }
   public WLong add (WInteger n) {
      return new WLong (value + n.longValue());
   }
   public WDouble add (WDouble n) {
      return new WDouble ((double) value + n.doubleValue());
   }

   public WLong subtract (WLong n) {
      return new WLong (value - n.value);
   }
   public WLong subtract (WInteger n) {
      return new WLong (value - n.longValue());
   }
   public WDouble subtract (WDouble n) {
      return new WDouble ((double) value - n.doubleValue());
   }

   public WLong multiply (WLong n) {
      return new WLong (value * n.value);
   }
   public WLong multiply (WInteger n) {
      return new WLong (value * n.longValue());
   }
   public WDouble multiply (WDouble n) {
      return new WDouble ((double) value * n.doubleValue());
   }

   public WLong divide (WLong n) {
      return new WLong (value / n.value);
   }
   public WLong divide (WInteger n) {
      return new WLong (value / n.longValue());
   }
   public WDouble divide (WDouble n) {
      return new WDouble ((double) value / n.doubleValue());
   }

   public WLong remainder (WLong n) {
      return new WLong (value % n.value);
   }
   public WLong remainder (WInteger n) {
      return new WLong (value % n.longValue());
   }
   public WDouble remainder (WDouble n) {
      return new WDouble ((double) value % n.doubleValue());
   }

   public WBoolean equals (WLong n) {
      return value == n.value ? WBoolean.TRUE : WBoolean.FALSE;
   }
   public WBoolean equals (WInteger n) {
      return value == n.value ? WBoolean.TRUE : WBoolean.FALSE;
   }
   public WBoolean equals (WDouble n) {
      return value == n.value ? WBoolean.TRUE : WBoolean.FALSE;
   }

   public WBoolean lt (WLong n) {
      return value < n.value ? WBoolean.TRUE : WBoolean.FALSE;
   }
   public WBoolean lt (WInteger n) {
      return value < n.value ? WBoolean.TRUE : WBoolean.FALSE;
   }
   public WBoolean lt (WDouble n) {
      return value < n.value ? WBoolean.TRUE : WBoolean.FALSE;
   }

   public WBoolean gt (WLong n) {
      return value > n.value ? WBoolean.TRUE : WBoolean.FALSE;
   }
   public WBoolean gt (WInteger n) {
      return value > n.value ? WBoolean.TRUE : WBoolean.FALSE;
   }
   public WBoolean gt (WDouble n) {
      return value > n.longValue() ? WBoolean.TRUE : WBoolean.FALSE;
   }

   public WBoolean ge (WLong n) {
      return value >= n.value ? WBoolean.TRUE : WBoolean.FALSE;
   }
   public WBoolean ge (WInteger n) {
      return value >= n.value ? WBoolean.TRUE : WBoolean.FALSE;
   }
   public WBoolean ge (WDouble n) {
      return value >= n.value ? WBoolean.TRUE : WBoolean.FALSE;
   }

   public WBoolean le (WLong n) {
      return value <= n.value ? WBoolean.TRUE : WBoolean.FALSE;
   }
   public WBoolean le (WInteger n) {
      return value <= n.value ? WBoolean.TRUE : WBoolean.FALSE;
   }
   public WBoolean le (WDouble n) {
      return value <= n.value ? WBoolean.TRUE : WBoolean.FALSE;
   }

   public WBoolean ne (WLong n) {
      return value != n.value ? WBoolean.TRUE : WBoolean.FALSE;
   }
   public WBoolean ne (WInteger n) {
      return value != n.value ? WBoolean.TRUE : WBoolean.FALSE;
   }
   public WBoolean ne (WDouble n) {
      return value != n.value ? WBoolean.TRUE : WBoolean.FALSE;
   }

   public WLong and (WLong n) {
      return new WLong (value & n.value);
   }
   public WLong and (WInteger n) {
      return new WLong (value & n.longValue());
   }
   public WLong or (WLong n) {
      return new WLong (value | n.value);
   }
   public WLong or (WInteger n) {
      return new WLong (value | n.longValue());
   }
   public WLong xor (WLong n) {
      return new WLong (value ^ n.value);
   }
   public WLong xor (WInteger n) {
      return new WLong (value ^ n.longValue());
   }
   public WLong not () {
      return new WLong (~value);
   }
   public WLong shiftl (WInteger n) {
      return new WLong (value << n.value);
   }
   public WLong shiftr (WInteger n) {
      return new WLong (value >>> n.value);
   }
   public WLong shifta (WInteger n) {
      return new WLong (value >> n.value);
   }
   public String toString() {
      return Long.toString (value);
   }
   public String toHexString () {
      return Long.toHexString (value);
   }
   public String toBinaryString () {
      return Long.toBinaryString (value);
   }
}
