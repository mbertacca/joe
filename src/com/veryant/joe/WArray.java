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

public class WArray extends Wrapper {
   private Object[] value;

   public WArray (Object[] array) {
      value = array;
   }
   public WArray (WNumber len) {
      value = new Object[len.intValue()];
   }
   public WArray (int len) {
      value = new Object[len];
   }
   public Type type() {
      return Type.OBJECT_ARRAY;
   }
   public Object getWrapped () {
      return value;
   }
   public WInteger length() {
      return new WInteger (value.length);
   }
   public Object get(WNumber idx) {
      return value[idx.intValue()];
   }
   public Object set(WNumber idx, Object obj) {
      return value[idx.intValue()] = obj;
   }
   public Object add(Object obj) {
      Object newValue[] = new Object[value.length + 1];
      int i;
      for (i = 0; i < value.length; i++)
         newValue[i] = value[i];
      value = newValue;
      return value[i] = obj;
   }
}
