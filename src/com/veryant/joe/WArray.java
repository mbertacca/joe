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

/**
 * This class implements a JOE array ans it is a wrapper of a Java array.
 * In order to create an array from JOE you can use the following instructions:
 * <br>
 * <code>! array <i>obj0</i> [, <i>obj1</i> ... , <i>objn</i> ]</code>
 * <br>
 * which creates an array containing all the objects specified or:
 * <br>
 * <code>! newArray <i>aInteger</i></code>
 * <br>
 * which creates an array of <i>aInteger</i> elements all containing null.
*/

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
/**
 * This is for internal use only 
 */
   public Object getWrapped () {
      return value;
   }
/**
 * Returns the length of this array.
 */
   public WInteger length() {
      return new WInteger (value.length);
   }
/**
 * Returns the length of this array.
 * @see #length()
 */
   public WInteger size() {
      return new WInteger (value.length);
   }
/**
 * Returns the object at position <i>idx</i>.
 */
   public Object get(WNumber idx) {
      return value[idx.intValue()];
   }
/**
 * Sets the object <i>obj</i> at position <i>idx</i>.
 */
   public Object set(int idx, Object obj) {
      if (!value.getClass().equals(Object[].class) && obj instanceof Wrapper)
         return value[idx] = ((Wrapper)obj).getWrapped();
      else 
         return value[idx] = obj;
   }
/**
 * @see #set(int idx, Object obj)
 */
   public Object set(WNumber idx, Object obj) {
      return set (idx.intValue(), obj);
   }
/**
 * Returns a new array longer by one of this array
 * with <i>obj</i> in the last position.
 */
   public Object add(Object obj) {
      final int i = value.length;
      WArray Return = new WArray(Arrays.copyOf (value, i + 1));
      Return.set (i, obj);
      return Return;
   }
/**
 * Returns a new array shorter by <i>shft</i> of this array
 * without <i>shft</i> objects at the beginning.
 */
   public Object shift(final int shft) {
      return new WArray(Arrays.copyOfRange (value, shft, value.length));
   }
/**
 * @see #shift(int shft)
 */
   public Object shift(WNumber shiftValue) {
      return shift (shiftValue.intValue());
   }
/**
 * Returns a new array shorter by one of this array
 * without the object in the first position.
 */
   public Object shift() {
      return shift (1);
   }
/**
 * Returns a new array longer by one of this array
 * with <i>obj</i> in the first position.
 */
   public Object unshift (Object obj) {
      Object Return[] = new Object[value.length + 1];
      Return[0] = obj;
      System.arraycopy(value, 0, Return, 1, value.length);
      return new WArray(Return);
   }
/**
 * Returns the specified range of this array as a new array.
 * The value at <i>from</i> is placed at the first position in the new array.
 * The length of the returned array will be <i>to</i> - <i>from</i>.
 */
   public Object slice (WNumber from, WNumber to) {
      return new WArray(Arrays.copyOfRange (value,
                                          from.intValue(), to.intValue()));
   }
/**
 * Returns a copy of this array
 */
   public Object clone() {
      return value.clone();
   }
}

