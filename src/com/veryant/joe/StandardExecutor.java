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

public class StandardExecutor implements Executor {
   private static Debugger debug = null;
   public Object run (Block blk) throws JOEException {
      Object Return = null;
      int i = 0;
      int len = blk.size();
      try {
         try {
            if (debug != null && debug.isOn()) {
               i--;
               throw new DoDebugException();
            }
            for ( ; i < len; i++)
               Return = blk.get(i).exec(blk);
         } catch (DoDebugException _ex) {
            if (debug == null)
               debug = _ex.getDebugger();
            debug.on();
            i++;
            for ( ; i < len; i++) {
               if (debug.isOn()) {
                  Return = debug.breakpoint (blk.get(i), blk);
               } else {
                  try {
                     Return = blk.get(i).exec(blk);
                  } catch (DoDebugException _ex1) {
                     if (debug == null)
                        debug = _ex1.getDebugger();
                     debug.on();
                  }
               }
            }
         }
      } catch (BreakLoopException ex) {
         if (!ex.hasReturnObject())
            ex.setReturnObject(Return);
          throw ex;
      } catch (BreakCmdException ex) {
         final String blkName = ex.getMessage();
         if (blkName.length() > 0) {
            if (blkName.equals (blk.name())) {
               if (ex.hasReturnObject())
                  Return = ex.getReturnObject();
            } else {
               if (!ex.hasReturnObject())
                   ex.setReturnObject(Return);
                throw ex;
            }
         } else {
            if (ex.hasReturnObject())
               Return = ex.getReturnObject();
         }
      } catch (JOEException ex) {
         Message msg = blk.get(i); 
         ex.addStack (msg.getFileName(), msg.getRow(), msg.getCol());
         throw ex;
      }
      return Return;
   }
}
