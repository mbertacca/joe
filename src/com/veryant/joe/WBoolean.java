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

public class WBoolean extends Wrapper {
   private final boolean pValue;
   private final Boolean oValue;

   public WBoolean (Boolean v) {
      oValue = v;
      pValue = v.booleanValue();
   }
   public WBoolean (boolean v) {
      oValue = new Boolean (v);
      pValue = v;
   }
   public Type type() {
      return Type.BOOLEAN;
   }
   public Object getWrapped () {
      return oValue;
   }
   public boolean booleanValue() {
      return pValue;
   }
   public boolean equals (WBoolean b) {
      return pValue == b.pValue;
   }
   public boolean ne (WBoolean b) {
      return pValue != b.pValue;
   }
   public WBoolean and (Boolean b) {
      return new WBoolean (pValue && b.booleanValue());
   }
   public WBoolean and (WBoolean b) {
      return new WBoolean (pValue && b.pValue);
   }
   public WBoolean and (Block m) throws JOEException {
      if (pValue) {
         Object b = m.exec();
         if (b instanceof WBoolean)
            return (WBoolean) b;
      }
      return this;
   }
   public WBoolean or (WBoolean b) {
      return new WBoolean (pValue || b.pValue);
   }
   public WBoolean or (Boolean b) {
      return new WBoolean (pValue || b.booleanValue());
   }
   public WBoolean or (Block m) throws JOEException {
      if (!pValue) {
         Object b = m.exec();
         if (b instanceof WBoolean)
            return (WBoolean) b;
      }
      return this;
   }
   public WBoolean not () {
      return new WBoolean (!pValue);
   }

   public Object iif (Object oTrue, Object oFalse) throws JOEException {
      if (pValue)
         return oTrue;
      else
         return oFalse;
   }
   public WBoolean ifTrue (Block m) throws JOEException {
      if (pValue)
         m.exec();

      return this;
   }
   public WBoolean ifFalse (Block m) throws JOEException {
      if (!pValue)
         m.exec();

      return this;
   }
}
