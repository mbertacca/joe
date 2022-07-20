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
         cmd.println ("<enter> run current message;");
         cmd.println (".       run current message stepping over blocks;");
         cmd.println (".s      skip current message;");
         cmd.println (".e      exit debug mode;");
         cmd.println (".q      quit program.");
         cmd.println ("");
         cmd.println ("otherwise run the command you enter.");
         cmd.println ("");
         firstTime = false;
      }
      for ( ; ; ) {
         Return = null;
         try {
            cmd.println ("***  <"+msg.getRow()+","+msg.getCol()+"> " + msg);
            cmd.print ("dbg: ");
            line =  cmd.readLine();
            if (line == null)
               line = ".q";
            else
               line = line.trim();
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
         case ".s":
            Return = WNull.value;
            return Return;
         case ".e":
            isOn = false;
            try {
               Return = msg.exec (blk);
            } catch (DoDebugException ignore) {
            }
            return Return;
         case ".q":
            System.exit (0);
            break;
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
