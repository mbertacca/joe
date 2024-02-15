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

public class Variable {
   private final String name;
   private int depth = 0;
   private int index = -1;
   private Boolean isConst; 

   public Variable (String name, int depth, int index) {
      this.depth = depth;
      this.index = index;
      this.name = name;
      this.isConst = null;
   }
   Variable setLocal (int index) {
      this.depth = 0;
      this.index = index;
      return this;
   }
   String getName () {
      return name;
   }
   int getDepth () {
      return depth;
   }
   int getIndex () {
      return index;
   }
   void setConstant (boolean b) {
      isConst = new Boolean(b);
   }
   boolean isConstant () {
      return isConst != null && isConst.booleanValue();
   }
   boolean canBeConst () {
      return isConst == null;
   }
   @Override
   public int hashCode() {
      return name.hashCode();
   }
   @Override
   public String toString() {
      return name + "[" + depth + "][" + index + "]";
   }
}
