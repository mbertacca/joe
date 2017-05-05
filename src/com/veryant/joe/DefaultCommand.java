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

import java.io.BufferedReader;
import java.io.Console;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.IOException;
import java.io.PrintWriter;
import java.io.StringWriter;
import java.util.HashMap;
import java.lang.reflect.Constructor;
import java.lang.reflect.Field;

/**
 * This class contains methods that makes tha flavor of JOE
 * similar to the Java language
 */

public class DefaultCommand extends CommandBase {
   /**
    * Read a line from the console.
    */
   public String readLine () throws IOException {
      Console cons = System.console();
      if (cons != null)
         return cons.readLine();
      else
         throw new IOException ("console not available");
   }
   /**
    * Prints a newline.
    */
   public Object println () {
      System.out.println ("");
      return this;
   }
   /**
    * Prints the specified objects.
    */
   public Object print (Object... b) {
      String s = "";
      for (int i = 0; i < b.length; i++) {
         if (b[i] == null)
            s += "(null)";
         else
            s += b[i].toString();
      }
      System.out.print (s);
      return this;
   }
   /**
    * Prints the specified objects followed by a newline.
    */
   public Object println (Object... b) {
      String s = "";
      for (int i = 0; i < b.length; i++) {
         if (b[i] == null)
            s += "(null)";
         else
            s += b[i].toString();
      }
      System.out.println (s);
      return this;
   }
   /**
    * If cond is true then the block ifTrue is executed and its
    * return value returned, otherwise cond is returned.
    */
   public Object $if (Boolean cond, Block ifTrue) throws Exception {
      Object Return = cond;
      if (cond.booleanValue()) {
         Return = ifTrue.exec();
      }
      return Return;
   }
   /**
    * If cond is true then the block ifTrue is executed and its
    * return value returned, otherwise ifFalse is executed and its
    * return value returned.
    */
   public Object $if (boolean cond, Block ifTrue, Block ifFalse)
                                                             throws Exception {
      Object Return;
      if (cond) {
         Return = ifTrue.exec();
      } else {
         Return = ifFalse.exec();
      }
      return Return;
   }
   /**
    * if implementation without 'else'.
    */
   public Object $if (Block cond, Block ifTrue) throws Exception {
      Object cnd;
      if ((cnd = cond.exec()) != null && cnd instanceof WBoolean)
         return $if (((WBoolean) cnd).booleanValue(), ifTrue);
      else
         return null;
   }
   /**
    * if implementation with 'else'.
    */
   public Object $if (Block cond, Block ifTrue, Block ifFalse)
                                                           throws Exception {
      Object cnd;
      if ((cnd = cond.exec()) != null && cnd instanceof WBoolean)
         return $if (((WBoolean) cnd).booleanValue(), ifTrue, ifFalse);
      else
         return null;
   }

   /**
    * This method implements a loop. The code block is executed while the
    * cond block is executed and it returns a boolean value true or
    * until the breakLoop () invocation is encountered.
    */
   public Object $while (Block cond, Block code) throws JOEException {
      Object Return;
      while ((Return = cond.exec()) != null && Return instanceof WBoolean &&
             ((WBoolean) Return).booleanValue())
         try {
            Return = code.exec();
         } catch (BreakLoopException _ex) {
            if (_ex.hasReturnObject())
               Return = _ex.getReturnObject();
            break;
         }
      return Return;
   }
   /**
    * This method implements a 'for each'. The code block is executed for
    * each object passed in the array startint from the n-th,
    * each object is passed to the block as argument.
    */
   public Object foreach(Object list[], int n, Block code) throws JOEException {
      Object Return = null;
      for (int i = n; i < list.length; i++) {
         try {
            final Object o = Wrapper.newInstance(list[i]);
            if (o != null) {
               Return = code.exec(o);
            } else {
               Return = code.exec(list[i]);
            }
         } catch (BreakLoopException _ex) {
            if (_ex.hasReturnObject())
               Return = _ex.getReturnObject();
            break;
         }
      }
      return Return;
   }
   /**
    * This is a convenience method for foreach (list, 0, code).
    */
   public Object foreach (Object list[], Block code) throws JOEException {
      return foreach (list, 0, code);
   }
   /**
    * Causes the exit from a loop.
    */
   public void breakLoop() throws BreakLoopException {
      throw new BreakLoopException();
   }
   /**
    * This class is used in order to implement a behaviour similar
    * to the Java switch.
    */
   protected static class Switch {
      private final Object cfrt1;
      private boolean alreadyDone;
      private boolean prevCondition;
      private Object Return;

      /**
       * Constructor, cfrt is the object to whom all the objects specified
       * in the following 'case' methods will be compared.
       */
      protected Switch (Object cfrt) {
         this.cfrt1 = cfrt;
      }
      /**
       * This method compares cfrt2 with the object specified in the constructor
       * and if they are equal then the following $case (cfrt2, block), if
       * any, will be executed independently from its argument.
       * It returns this.
       */
      public Object $case (Object cfrt2) {
         prevCondition |= cfrt1.equals (cfrt2);
         return this;
      }
      /**
       * This method executes block under the following conditions:
       * - any previous invocation of this method on this object
       *   hasn't been executed yet;
       * - the argument of any previous $case(cftr2) invocation, or
       *   the argument of this invocation is equal to the object
       *   specified in the constructor.
       */

      public Object $case (Object cfrt2, Block block) throws Exception {
         if (!alreadyDone && (prevCondition || cfrt1.equals (cfrt2))) {
            alreadyDone = true;
            Return = block.exec();
         } else {
            prevCondition = false;
         }
         return this;
      }
      /**
       * This method executes block if any previous invocation
       * of $case (cfrt2, block) method on this object hasn't be
       * executed yet.
       */
      public Object $default (Block block) throws Exception {
         if (!alreadyDone) {
            alreadyDone = true;
            Return = block.exec();
         }
         return this;
      }
      /**
       * This method returns the return code of the block executed or null
       * if no block has been executed by this object.
       */
      public Object endSwitch () {
         return Return;
      }
   }
   /**
    * Return an instance of class Switch created using cfrt as parameter.
    */
   public Object $switch (Object cfrt) {
      return new Switch (cfrt);
   }
   /**
    * Causes the exit from the current block.
    */
   public Object $break () throws Exception {
      throw new BreakCmdException ("");
   }
   /**
    * Causes the exit from the block whose name is blkName.
    */
   public Object $break (String blkName) throws Exception {
      throw new BreakCmdException (blkName);
   }
   /**
    * Executes the block passed as argument: if the execution throws
    * an exception then the exception is returned instead of the
    * expected return object.
    */
   public Object $try (Block blk) throws Exception {
      Object Return;
      try {
         Return = blk.exec();
      } catch (JOEException ex) {
         if (ex.getCause() != null)
            Return = ex.getCause();
         else
            Return = ex;
      } catch (Throwable ex) {
         Return = ex;
      }
      return Return;
   }
   /**
    * Executes the block passed as 1st argument: if the execution throws
    * an exception then the block passed as 2nd argument is executed with
    * the exception as argument.
    */
   public Object $try (Block blk, Block excpt) throws Exception {
      Object Return;
      try {
         Return = blk.exec();
      } catch (JOEException ex) {
         if (ex.getCause() != null)
            Return = ex.getCause();
         else
            Return = ex;
         Return = excpt.exec (Return);
      } catch (Throwable ex) {
         Return = excpt.exec (ex);
      }
      return Return;
   }
   /**
    * Executes the specified command and returns its return code.
    */
   public int exec (String...cmds) throws Exception {
      final Runtime rt = Runtime.getRuntime();
      final Process proc = rt.exec(cmds);
      BufferedReader bro = new BufferedReader(
                              new InputStreamReader (proc.getInputStream()));
      BufferedReader bre = new BufferedReader(
                              new InputStreamReader (proc.getErrorStream()));
       String line;
       while ((line = bro.readLine()) != null ||
              (line = bre.readLine()) != null)
          println (line);
      return proc.waitFor();
   }
   /**
    * Executes the specified command and returns its standard output
    * as a string.
    */
   public String execGetOut (String...cmds) throws Exception {
      final Runtime rt = Runtime.getRuntime();
      final Process proc = rt.exec(cmds);
      BufferedReader bro = new BufferedReader(
                              new InputStreamReader (proc.getInputStream()));
      BufferedReader bre = new BufferedReader(
                              new InputStreamReader (proc.getErrorStream()));

      StringBuilder Return = new StringBuilder();
      String line;
      if ((line = bro.readLine()) != null)
         Return.append (line);
      for ( ; ; ) {
         if ((line = bro.readLine()) != null) {
            Return.append (' ');
            Return.append (line);
         } else if ((line = bre.readLine()) != null)
            println (line);
         else
            break;
      }
      return Return.toString();
   }
   /**
    * Executes the specified JOE script and returns its return code.
    */
   public int runJoe (String...cmds) throws Exception {
      return JavaObjectsExecutor.imain (cmds);
   }
   /** Stops the execution of the current script and executes the
    * script specified as argument.
    */
   public int execJoe (String...cmds) throws ExecException {
      throw new ExecException (cmds);
   }
   /**
    * Reads the value of the specified environment variable.
    */
   public String systemGetenv (String name) {
      return System.getenv (name);
   }

   /**
    * Shows the specified objects in a graphical window.
    */
   public void showMessageDialog(Object... message) {
      String msg = "";
      for (int i = 0; i < message.length; i++)
         msg += message[i].toString();
      javax.swing.JOptionPane.showMessageDialog(null, msg);
   }
   /**
    * Shows the specified objects in a graphical window and reads
    * an input that returns.
    */
   public String showInputDialog(Object... message) {
      String Return;
      String msg = "";
      for (int i = 0; i < message.length; i++)
         if (message[i] == null)
            msg += "(null)";
         else
            msg += message[i].toString();
      Return = javax.swing.JOptionPane.showInputDialog(null, msg);
      if (Return == null)
         Return = "";
      return Return;
   }
}
