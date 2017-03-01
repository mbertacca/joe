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

public class BreakCmdException extends JOEException {
   private Object returnObject;

   public BreakCmdException (String name) {
      super(name);
      returnObject = this;
   }
   public boolean hasReturnObject () {
      return returnObject != this;
   }
   public void setReturnObject (Object obj) {
      returnObject = obj;
   }
   public Object getReturnObject () {
      return returnObject;
   }
}
