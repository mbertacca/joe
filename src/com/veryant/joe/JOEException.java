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

public class JOEException extends Exception {
   private final String info;
   JOEException (String msg, int r, int c) {
      super (msg);
      if (r != 0)
         info = " line " + r + ", column " + c;
      else
         info = "";
   }
   JOEException (String msg, Token tk) {
      this (msg, tk.row, tk.col);
   }
   public JOEException (String msg) {
      this (msg, 0, 0);
   }
   JOEException (Throwable ex, int r, int c) {
      super (ex);
      if (r != 0 && ! (ex instanceof JOEException))
         info = " line " + r + ", column " + c;
      else
         info = "";
   }
   public String getMessage () {
      return super.getMessage() + info;
   }
}
