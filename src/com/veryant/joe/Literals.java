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

import java.math.BigDecimal;

public class Literals {
   public static Object getInteger (String val) {
      if (val.startsWith("x"))
         return Wrapper.newInstance (Integer.parseInt(val.substring(1),16));
      else
         return Wrapper.newInstance (Integer.parseInt(val));
   }
   public static Object getLong (String val) {
      if (val.startsWith("x"))
         return Wrapper.newInstance (Long.parseLong(val.substring(1),16));
      else
         return Wrapper.newInstance (Long.parseLong(val));
   }
   public static Object getDouble (String val) {
      return Wrapper.newInstance (Double.parseDouble(val));
   }
   public static Object getDecimal (String val) {
      return Wrapper.newInstance (new BigDecimal(val));
   }
   public static Object getString (String val) {
      return Wrapper.newInstance (val);
   }
   public static Object getBoolean (boolean val) {
      return Wrapper.newInstance (val);
   }
   public static Object getNull () {
      return Wrapper.newInstance (null);
   }
}
