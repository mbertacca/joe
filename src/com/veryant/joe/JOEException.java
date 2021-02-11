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
   private String buildInfo(String fName, int r, int c) {
      StringBuilder sb = new StringBuilder ();
      sb.append (this.getClass().getName());
      sb.append (" in ");
      sb.append (fName);
      sb.append (", line ");
      sb.append (r);
      sb.append (", column ");
      sb.append (c);
      sb.append (": ");
      return sb.toString();
   }
   public JOEException (String msg, int r, int c, String fileName) {
      super (msg);
      if (r != 0)
         info = buildInfo(fileName, r, c);
      else
         info = "";
   }
   JOEException (String msg, Token tk, String fileName) {
      this (msg, tk.row, tk.col, fileName);
   }
   public JOEException (String msg) {
      this (msg, 0, 0,  null);
   }
   public JOEException (Throwable ex, int r, int c, String fileName) {
      super (ex);
      if (r != 0 && ! (ex instanceof JOEException))
         info = buildInfo(fileName, r, c);
      else
         info = "";
   }
   public String getMessage () {
      return info + super.getMessage();
   }
}
