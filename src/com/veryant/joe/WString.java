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

public class WString extends Wrapper {
   public final String value;

   public WString (String s) {
      value = s;
   }
   private WString (char c) {
      value = Character.toString(c);
   }
   public Type type() {
      return Type.STRING;
   }
   public Object getWrapped() {
      return value;
   }
   public WBoolean equals (WString s) {
      return value.equals(s.value) ? WBoolean.TRUE : WBoolean.FALSE;
   }
   public WBoolean lt (WString s) {
      return value.compareTo(s.value) < 0 ? WBoolean.TRUE : WBoolean.FALSE;
   }
   public WBoolean gt (WString s) {
      return value.compareTo(s.value) > 0 ? WBoolean.TRUE : WBoolean.FALSE;
   }
   public WBoolean ge (WString s) {
      return value.compareTo(s.value) >= 0 ? WBoolean.TRUE : WBoolean.FALSE;
   }
   public WBoolean le (WString s) {
      return value.compareTo(s.value) <= 0 ? WBoolean.TRUE : WBoolean.FALSE;
   }
   public WBoolean ne (WString s) {
      return value.compareTo(s.value) != 0 ? WBoolean.TRUE : WBoolean.FALSE;
   }
   public WString concat (WString str) {
      return new WString (value.concat(str.value));
   }
   public WString add (WString str) {
      return new WString (value.concat(str.value));
   }
   public WString add (Object obj) {
      return new WString (value.concat(obj.toString()));
   }
   public WBoolean startsWith (WString s) {
      return value.startsWith(s.value) ? WBoolean.TRUE : WBoolean.FALSE;
   }
   public WBoolean startsWith (WString s, WNumber toffset) {
      return value.startsWith(s.value, toffset.intValue()) ?
             WBoolean.TRUE : WBoolean.FALSE;
   }
   public WString substring (WNumber beginIndex) {
      int idx = beginIndex.intValue();
      if (idx < 0)
         return new WString (value.substring(value.length() + idx));
      else
         return new WString (value.substring(idx));
   }
   public WString substring (WNumber beginIndex, WNumber endIndex) {
      int idx = endIndex.intValue();
      if (idx < 0)
         return new WString (value.substring(beginIndex.intValue(),
                                             value.length() + idx));
      else
         return new WString (value.substring(beginIndex.intValue(),
                                             idx));
   }
   public WCharacter charAt (WNumber index) {
      return new WCharacter (value.charAt(index.intValue()));
   }
   public WString toLowerCase () {
      return new WString (value.toLowerCase());
   }
   public WString toUpperCase () {
      return new WString (value.toUpperCase());
   }
   public WInteger length () {
      return new WInteger (value.length());
   }
   public WInteger compareTo(String anotherString) {
      return new WInteger (value.compareTo (anotherString));
   }
   public WInteger compareToIgnoreCase(String anotherString) {
      return new WInteger (value.compareToIgnoreCase (anotherString));
   }
   public WInteger compareTo(WString anotherString) {
      return new WInteger (value.compareTo (anotherString.value));
   }
   public WInteger compareToIgnoreCase(WString anotherString) {
      return new WInteger (value.compareToIgnoreCase (anotherString.value));
   }
   public WBoolean contains (WString s) {
      return value.contains(s.value) ? WBoolean.TRUE : WBoolean.FALSE;
   }
   public WBoolean endsWith (WString s) {
      return value.endsWith(s.value) ? WBoolean.TRUE : WBoolean.FALSE;
   }
   public WBoolean equalsIgnoreCase (WString s) {
      return value.equalsIgnoreCase(s.value) ? WBoolean.TRUE : WBoolean.FALSE;
   }
   public WInteger indexOf(WString str) {
      return new WInteger (value.indexOf (str.value));
   }
   public WInteger indexOf (WString str, WNumber fromIndex) {
      return new WInteger (value.indexOf(str.value, fromIndex.intValue()));
   }
   public WBoolean isEmpty () {
      return value.isEmpty() ? WBoolean.TRUE : WBoolean.FALSE;
   }
   public WInteger lastIndexOf(WString str) {
      return new WInteger (value.lastIndexOf (str.value));
   }
   public WInteger lastIndexOf (WString str, WNumber fromIndex) {
      return new WInteger (value.lastIndexOf(str.value, fromIndex.intValue()));
   }
   public WBoolean matches (WString regex) {
      return value.matches(regex.value) ? WBoolean.TRUE : WBoolean.FALSE;
   }
   public WString replace(WString target, WString replacement) {
      return new WString (value.replace (target.value, replacement.value));
   }
   public WString replaceAll(WString regex, WString replacement) {
      return new WString (value.replaceAll (regex.value, replacement.value));
   }
   public WString replaceFirst(WString regex, WString replacement) {
      return new WString (value.replaceFirst (regex.value, replacement.value));
   }
   public String[] split(WString regex) {
      return value.split (regex.value);
   }
   public WString  trim() {
      return new WString (value.trim());
   }
   public String toJava() {
      return value;
   }
   public String toString() {
      return value;
   }
   public Integer intValue() {
      try {
         return new Integer (value);
      } catch (NumberFormatException _ex) {
         return null;
      }
   }
   public Long longValue() {
      try {
         return new Long (value);
      } catch (NumberFormatException _ex) {
         return null;
      }
   }
   public Double doubleValue() {
      try {
         return new Double (value);
      } catch (NumberFormatException _ex) {
         return null;
      }
   }
   public String at (WNumber index) {
      int idx = index.intValue();
      return value.substring (idx, idx + 1);
   }
   public WInteger charCodeAt (WNumber index) {
      int idx = index.intValue();
      return new WInteger (value.charAt(idx));
   }
   public WInteger charCodeAt () {
      return new WInteger (value.charAt(0));
   }
}
