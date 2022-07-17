/*
 *
 * This source file is part of the "Java Objects Executor" open source project
 *
 * Copyright 2022 by Marco Bertacca
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

import java.io.IOException;

public class Debugger {
   private final DefaultCommand cmd = JavaObjectsExecutor.command;
   static private boolean firstTime = true;
   private boolean isOn = true;
   public Object breakpoint (Message msg, Block blk) throws JOEException {
      Object Return;
      String line;
      if (firstTime) {
         cmd.println ("* JOE debugger *");
         cmd.println ("<enter>   execute next message;");
         cmd.println (".         execute without entering in blocks;");
         cmd.println ("..        exit debug mode;");
         cmd.println ("otherwise execute the command you enter.");
         firstTime = false;
      }
      for ( ; ; ) {
         Return = null;
         try {
            cmd.println ("***  [", msg,"]");
            line =  JavaObjectsExecutor.readLine (cmd).trim();
         } catch (IOException ex) {
            JavaObjectsExecutor.showException (cmd, ex);
            line = ".";
         }
         switch (line) {
         case "":
            try {
               Return = msg.exec (blk);
               cmd.println ("---> ",Return);
            } catch (DoDebugException ignore) {
            }
            return Return;
         case ".":
            isOn = false;
            try {
               Return = msg.exec (blk);
               cmd.println ("---> ",Return);
            } catch (DoDebugException ignore) {
            }
            isOn = true;
            return Return;
         case "..":
            isOn = false;
            try {
               Return = msg.exec (blk);
            } catch (DoDebugException ignore) {
            }
            return Return;
         default:
            isOn = false;
            try {
               Execute exec = new Execute(blk);
               exec.add (line);
               Return = exec.exec();
               cmd.println ("---> ",Return);
            } catch (Exception ex) {
               JavaObjectsExecutor.showException (cmd, ex);
            }
            isOn = true;
         }
      }
   }
   public void on() {
      isOn = true;
   }
   public boolean isOn() {
      return isOn;
   }
}
