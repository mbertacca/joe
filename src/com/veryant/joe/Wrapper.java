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

import java.lang.reflect.Array;
import java.math.BigDecimal;
import java.util.HashMap;

public abstract class Wrapper implements InternalObject {
   enum Type {
      NULL,
      BOOLEAN,
      BYTE,
      CHAR,
      SHORT,
      INT,
      LONG,
      FLOAT,
      DOUBLE,
      DECIMAL,
      STRING,
      OBJECT_ARRAY
   }
   private final static HashMap<Class,Type> mapping;
   static {
      mapping = new HashMap<Class,Type>();
      mapping.put (Boolean.class, Type.BOOLEAN);
      mapping.put (Byte.class, Type.BYTE);
      mapping.put (Character.class, Type.CHAR);
      mapping.put (Short.class, Type.SHORT);
      mapping.put (Integer.class, Type.INT);
      mapping.put (Long.class, Type.LONG);
      mapping.put (Float.class, Type.FLOAT);
      mapping.put (Double.class, Type.DOUBLE);
      mapping.put (BigDecimal.class, Type.DECIMAL);
      mapping.put (String.class, Type.STRING);
      mapping.put (Object[].class, Type.OBJECT_ARRAY);
   }

   public static Wrapper newInstance (Object obj) {
      if (obj != null) {
         Type t;
         Class clazz = obj.getClass();
         if ((t = mapping.get (clazz)) != null) {
            switch (t) {
            case BOOLEAN:
               return ((Boolean) obj).booleanValue() ?
                                       WBoolean.TRUE : WBoolean.FALSE;
            case BYTE:
               return new WInteger (((Byte) obj).longValue());
            case CHAR:
               return new WCharacter (((Character) obj).charValue());
            case SHORT:
               return new WShort (((Short) obj).shortValue());
            case INT:
               return new WInteger (((Integer) obj).intValue());
            case LONG:
               return new WLong ((Long) obj);
            case FLOAT:
               return new WDouble (((Float) obj).doubleValue());
            case DOUBLE:
               return new WDouble ((Double) obj);
            case DECIMAL:
               return new WBigDecimal((BigDecimal) obj);
            case STRING:
               return new WString ((String) obj);
            case OBJECT_ARRAY:
               return new WArray ((Object[]) obj);
            }
         } else if (clazz.isArray()) {
            final Class compClazz = clazz.getComponentType();
            if (compClazz.isPrimitive()) {
               if (byte.class.equals (compClazz) || 
                   char.class.equals (compClazz)) {
                  return null;
               } else {
                  final int len = Array.getLength(obj);

                  final Object[] objArray = new Object[len];
                  if (int.class.equals (compClazz)) {
                     for(int i = 0; i < len; i++)
                        objArray[i] = Integer.valueOf (((int[]) obj)[i]);
                  } else if (double.class.equals (compClazz)) {
                     for(int i = 0; i < len; i++)
                        objArray[i] = Double.valueOf (((double[]) obj)[i]);
                  } else if (float.class.equals (compClazz)) {
                     for(int i = 0; i < len; i++)
                        objArray[i] = Float.valueOf (((float[]) obj)[i]);
                  } else if (long.class.equals (compClazz)) {
                     for(int i = 0; i < len; i++)
                        objArray[i] = Long.valueOf (((long[]) obj)[i]);
                  } else if (short.class.equals (compClazz)) {
                     for(int i = 0; i < len; i++)
                        objArray[i] = Short.valueOf (((short[]) obj)[i]);
                  } else { // boolean
                     for(int i = 0; i < len; i++)
                        objArray[i] = Boolean.valueOf (((boolean[]) obj)[i]);
                  }
                  obj = objArray;
               }
            }
            return new WArray ((Object[]) obj);
         }
      } else {
         return WNull.value;
      }
      return null;
   }
   abstract Type type();
   abstract public Object getWrapped();
}
