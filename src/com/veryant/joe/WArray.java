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
import java.util.Arrays;

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
   public Object set(int idx, Object obj) {
      if (!value.getClass().equals(Object[].class) && obj instanceof Wrapper)
         return value[idx] = ((Wrapper)obj).getWrapped();
      else 
         return value[idx] = obj;
   }
   public Object set(WNumber idx, Object obj) {
      return set (idx.intValue(), obj);
   }
   /* The method DO NOT modify the object since 1.14 */
   public Object add(Object obj) {
      final int i = value.length;
      WArray Return = new WArray(Arrays.copyOf (value, i + 1));
      Return.set (i, obj);
      return Return;
   }
   /* The method DO NOT modify the object since 1.14 */
   public Object shift(final int shft) {
      return new WArray(Arrays.copyOfRange (value, shft, value.length));
   }
   public Object shift(WNumber shiftValue) {
      return shift (shiftValue.intValue());
   }
   public Object shift() {
      return shift (1);
   }
}

