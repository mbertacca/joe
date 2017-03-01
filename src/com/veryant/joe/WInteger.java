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
   public WInteger mod (WInteger n) {
      return new WInteger (value % n.intValue());
   }
}

