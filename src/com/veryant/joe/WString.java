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
      return new WBoolean (value.equals(s.value));
   }
   public WBoolean lt (WString s) {
      return new WBoolean (value.compareTo(s.value) < 0);
   }
   public WBoolean gt (WString s) {
      return new WBoolean (value.compareTo(s.value) > 0);
   }
   public WBoolean ge (WString s) {
      return new WBoolean (value.compareTo(s.value) >= 0);
   }
   public WBoolean le (WString s) {
      return new WBoolean (value.compareTo(s.value) <= 0);
   }
   public WBoolean ne (WString s) {
      return new WBoolean (value.compareTo(s.value) != 0);
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
      return new WBoolean (value.startsWith(s.value));
   }
   public WBoolean startsWith (WString s, WNumber toffset) {
      return new WBoolean (value.startsWith(s.value, toffset.intValue()));
   }
   public WString substring (WNumber beginIndex) {
      return new WString (value.substring(beginIndex.intValue()));
   }
   public WString substring (WNumber beginIndex, WNumber endIndex) {
      return new WString (value.substring(beginIndex.intValue(),
                                           endIndex.intValue()));
   }
   public WString charAt (WNumber index) {
      return new WString (value.charAt(index.intValue()));
   }
   public WString toLowerCase () {
      return new WString (value.toLowerCase());
   }
   public WString toUpperCase () {
      return new WString (value.toUpperCase());
   }
   public WLong length () {
      return new WLong (value.length());
   }
   public WLong compareTo(String anotherString) {
      return new WLong (value.compareTo (anotherString));
   }
   public WLong compareToIgnoreCase(String anotherString) {
      return new WLong (value.compareToIgnoreCase (anotherString));
   }
   public WBoolean contains (WString s) {
      return new WBoolean (value.contains(s.value));
   }
   public WBoolean endsWith (WString s) {
      return new WBoolean (value.endsWith(s.value));
   }
   public WBoolean equalsIgnoreCase (WString s) {
      return new WBoolean (value.equalsIgnoreCase(s.value));
   }
   public WLong indexOf(WString str) {
      return new WLong (value.indexOf (str.value));
   }
   public WLong indexOf (WString str, WNumber fromIndex) {
      return new WLong (value.indexOf(str.value, fromIndex.intValue()));
   }
   public WBoolean isEmpty () {
      return new WBoolean (value.isEmpty());
   }
   public WLong lastIndexOf(WString str) {
      return new WLong (value.lastIndexOf (str.value));
   }
   public WLong lastIndexOf (WString str, WNumber fromIndex) {
      return new WLong (value.lastIndexOf(str.value, fromIndex.intValue()));
   }
   public WBoolean matches (WString regex) {
      return new WBoolean (value.matches(regex.value));
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
}
