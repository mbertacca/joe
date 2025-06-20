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
import java.io.File;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.IOException;
import java.io.OutputStream;
import java.io.PrintStream;
import java.io.Writer;
import java.util.Collection;
import java.util.Comparator;
import java.util.Enumeration;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Iterator;
import java.lang.reflect.Constructor;
import java.lang.reflect.Field;

/**
 * This class contains methods that makes tha flavor of JOE
 * similar to the Java language
 */

public class DefaultCommand extends CommandBase {
   final static BufferedReader stdin;
   final static Console console;
   static {
      console = System.console();
      if (console == null)
         stdin = new BufferedReader (new InputStreamReader (System.in));
      else
         stdin = null;
   }
   public boolean isConsole() {
      return console != null;
   }
   /**
    * Read a line from the console.
    */
   public String readLine () throws IOException {
      if (console != null)
         return console.readLine();
      else
         return stdin.readLine();
   }
   private void intPrint (PrintStream pw, boolean nl, Object... b) {
      if (b == null)
         if (nl)
            pw.println ("()");
         else
            pw.print ("()");
      else {
         StringBuilder s = new StringBuilder();
         for (int i = 0; i < b.length; i++)
            if (b[i] == null)
               s.append ("()");
            else
               s.append (b[i].toString());
         if (nl)
            pw.println (s.toString());
         else
            pw.print (s.toString());
      }
   }
   /**
    * Prints a newline.
    */
   public Object println () {
      intPrint (System.out, true);
      return this;
   }
   /**
    * Prints a newline on stderr.
    */
   public Object eprintln () {
      intPrint (System.err, true);
      return this;
   }
   /**
    * Prints the specified objects.
    */
   public Object print (Object... b) {
      intPrint (System.out, false, b);
      return this;
   }
   /**
    * Prints the specified objects on stderr.
    */
   public Object eprint (Object... b) {
      intPrint (System.err, false, b);
      return this;
   }
   /**
    * Prints the specified objects followed by a newline.
    */
   public Object println (Object... b) {
      intPrint (System.out, true, b);
      return this;
   }
   /**
    * Prints the specified objects followed by a newline on stderr.
    */
   public Object eprintln (Object... b) {
      intPrint (System.err, true, b);
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
    * This method implements a loop. This loop will execute the code block once,
    * before checking if the cond block returns the boolean value true,
    * then it will repeat the loop as long as the condition is true or
    * until the breakLoop () invocation is encountered..
    */
   public Object doWhile (Block code, Block cond) throws JOEException {
      Object Return = WNull.value;
      Object bool;
      do {
         try {
            Return = code.exec();
         } catch (BreakLoopException _ex) {
            if (_ex.hasReturnObject())
               Return = _ex.getReturnObject();
            break;
         }
      } while ((bool = cond.exec()) != null && bool instanceof WBoolean &&
             ((WBoolean) bool).booleanValue());
      return Return;
   }
   /**
    * This method implements a 'for' cycle. The code block is executed for
    * passing the index as argument.
    */
   public Object $for(int start, int end, int step, Block code)
                                                       throws JOEException {
      Object Return = null;
      if (step >= 0) {
         for (int i = start; i <= end; i += step) {
            try {
               Return = code.exec(new WInteger(i));
            } catch (BreakLoopException _ex) {
               if (_ex.hasReturnObject())
                  Return = _ex.getReturnObject();
               break;
            }
         }
      } else {
         for (int i = start; i >= end; i += step) {
            try {
               Return = code.exec(new WInteger(i));
            } catch (BreakLoopException _ex) {
               if (_ex.hasReturnObject())
                  Return = _ex.getReturnObject();
               break;
            }
         }
      }
      return Return;
   }
   /**
    * This is a convenience method for for (start, end, 1).
    */
   public Object $for(int start, int end, Block code)
                                                       throws JOEException {
      return $for (start, end, 1, code);
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
    * This method implements a 'for each'. The code block is executed for
    * each object passed in the Collection startint from the n-th,
    * each object is passed to the block as argument.
    */
   public Object foreach(Collection list, int n, Block code) throws JOEException {
      Object Return = null;
      Iterator it = list.iterator();
      for ( ;it.hasNext() && n > 0; n--)
         it.next();
      while (it.hasNext()) {
         try {
            final Object o = Wrapper.newInstance(Return = it.next());
            if (o != null) {
               Return = code.exec(o);
            } else {
               Return = code.exec(Return);
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
   public Object foreach (Collection list, Block code) throws JOEException {
      return foreach (list, 0, code);
   }
   /**
    * This method implements a 'for each'. The code block is executed for
    * each object passed in the Iterator starting from n,
    * each object is passed to the block as argument.
    */
   public Object foreach(Iterator it, int n, Block code) throws JOEException {
      Object Return = null;
      for ( ;it.hasNext() && n > 0; n--)
         it.next();
      while (it.hasNext()) {
         try {
            final Object o = Wrapper.newInstance(Return = it.next());
            if (o != null) {
               Return = code.exec(o);
            } else {
               Return = code.exec(Return);
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
   public Object foreach (Iterator it, Block code) throws JOEException {
      return foreach (it, 0, code);
   }
   /**
    * This method implements a 'for each'. The code block is executed for
    * each object passed in the Enumeration starting from n,
    * each object is passed to the block as argument.
    */
   public Object foreach(Enumeration it, int n, Block code) throws JOEException {
      Object Return = null;
      for ( ;it.hasMoreElements() && n > 0; n--)
         it.nextElement();
      while (it.hasMoreElements()) {
         try {
            final Object o = Wrapper.newInstance(Return = it.nextElement());
            if (o != null) {
               Return = code.exec(o);
            } else {
               Return = code.exec(Return);
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
   public Object foreach (Enumeration it, Block code) throws JOEException {
      return foreach (it, 0, code);
   }
   /**
    * This method implements a 'for each'. The code block is executed for
    * each object passed in the Iterable.iterator starting from n,
    * each object is passed to the block as argument.
    */
   public Object foreach(Iterable it, int n, Block code) throws JOEException {
      return foreach (it.iterator(), n, code);
   }
   /**
    * This is a convenience method for foreach (list, 0, code).
    */
   public Object foreach (Iterable it, Block code) throws JOEException {
      return foreach (it, 0, code);
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
      public Object $case (Block blk2) throws JOEException {
         if (!(alreadyDone || prevCondition)) {
            Object cfrt2 =  blk2.exec();
            if (cfrt2 instanceof Wrapper)
               cfrt2 = ((Wrapper) cfrt2).getWrapped();
            return $case (cfrt2);
         } else
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

      public Object $case (Object cfrt2, Block block) throws JOEException {
         if (!alreadyDone && (prevCondition || cfrt1.equals (cfrt2))) {
            alreadyDone = true;
            Return = block.exec();
         } else {
            prevCondition = false;
         }
         return this;
      }
      public Object $case (Block blk2, Block block) throws JOEException {
         if (!(alreadyDone || prevCondition)) {
            Object cfrt2 =  blk2.exec();
            if (cfrt2 instanceof Wrapper)
               cfrt2 = ((Wrapper) cfrt2).getWrapped();
            return $case (cfrt2, block);
         } else
            return $case ((Object) null, block);
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
    * Returns an instance of class Switch created using cfrt as parameter.
    */
   public Object $switch (Object cfrt) {
      return new Switch (cfrt);
   }
   /**
    * Returns an instance of class Switch created using Boolean.TRUE as parameter.
    */
   public Object switchTrue () {
      return new Switch (Boolean.TRUE);
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

   private static class ThreadReader extends Thread {
      final InputStream in;
      final OutputStream out;
      ThreadReader (InputStream in, OutputStream out) {
         this.in = in;
         this.out = out;
      }
      public void run () {
         try {
            int c;
            while ((c = in.read()) >= 0) {
               out.write (c);
               out.flush();
            }
         } catch (IOException ignore) {
            System.err.println (ignore + "(" + in + ";" + out + ")");
         }
      }
   }
   /**
    * Executes the specified ProcessBuilder
    */
   public int exec (ProcessBuilder pb) throws Exception {
      pb.redirectInput(ProcessBuilder.Redirect.INHERIT);
      pb.redirectOutput(ProcessBuilder.Redirect.INHERIT);
      pb.redirectError(ProcessBuilder.Redirect.INHERIT);

      Process p = pb.start();
      return p.waitFor();
   }
   private static String[] toStringArray (Object[] a) {
      if (a instanceof String[])
          return (String[]) a;
      String Return[] = new String[a.length];
      for (int i = 0; i < a.length; i++)
          Return[i] = a[i].toString();
      return Return;
   }
   /**
    * Executes the specified command and returns its return code.
    */
   public int exec (Object...cmds) throws Exception {
      return exec (new ProcessBuilder (toStringArray(cmds)));
   }
   /**
    * Executes the specified command from the specified directory
    * and returns its return code.
    */
   public int execFromDir (File dir, Object...cmds) throws Exception {
      ProcessBuilder pb = new ProcessBuilder (toStringArray(cmds));
      pb.directory (dir);
      return exec (pb);
   }
   /**
    * Executes the specified command from the specified directory
    * and returns its return code.
    */
   public int execFromDir (String dir, Object...cmds) throws Exception {
      return execFromDir (new File (dir), cmds);
   }
   /**
    * Executes the specified command from the standard shell when possible
    */
   private String shell;
   private String opt;
   public int system (String...cmd) throws Exception {
      if (shell == null) {
         if (System.getProperty("os.name")
                   .toLowerCase()
                   .startsWith("windows")) {
            shell = System.getenv ("ComSpec");
            if (shell == null)
               shell = "cmd";
            opt = "/c";
         } else {
            shell = System.getenv ("SHELL");
            if (shell == null)
               shell = "sh";
            opt = "-c";
         }
      }
      String cmdline = "";
      for (String s : cmd)
         cmdline = cmdline + s + " ";
      return exec (new Object[] {shell, opt, cmdline});
   }
   /**
    * Executes the specified command and returns its standard output
    * as a string.
    */
   public String execGetOut (Object...cmds) throws Exception {
      final Runtime rt = Runtime.getRuntime();
      final Process proc = rt.exec(toStringArray(cmds));
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
            Return.append ('\n');
            Return.append (line);
         } else if ((line = bre.readLine()) != null)
            println (line);
         else
            break;
      }
      return Return.toString();
   }
   /**
    * Returns an object of type Pipe that allows the execution of programs
    * in a pipeline.
    */
   public Pipe pipe() {
      return new Pipe();
   }
   /**
    * Executes the specified JOE script and returns its return code.
    */
   public int runJoe (Object...cmds) throws Exception {
      int Return = 0;
      // return JavaObjectsExecutor.imain (toStringArray(cmds));

      Object args[] = toStringArray (cmds);
      Object[] jarg = new Object [] {new WArray (cmds)};
      Block blk = ScriptManager.load (this, args[0].toString(), jarg);
      Object rc  = blk.init (jarg);
      if (rc instanceof WNumber)
         Return = ((WNumber) rc).intValue();

      return Return;
   }
   /**
    * Executes the specified JOE script as it were an inner block
    * of the given argument.
    */
   public Object runAsBlock (Block blk,String name,Object...cmds)
                                                    throws Exception {
      Object Return;
      String line;
      ScriptManager sm =  ScriptManager.get(this);
      LineReader lr = sm.getLineReader();
      Execute exec = new Execute(blk, this, name);

      lr.open (name, new InputStreamReader(sm.getInputStream(name,path)));
      while ((line = lr.readLine()) != null) {
         exec.add (line);
      }
      lr.close();
      return Return = exec.exec(cmds);
   }


   /** Stops the execution of the current script and executes the
    * script specified as argument.
    */
   public int execJoe (Object...cmds) throws ExecException {
      throw new ExecException (toStringArray(cmds));
   }
   /**
    * Reads the value of the specified environment variable.
    */
   public String systemGetenv (String name) {
      return System.getenv (name);
   }
   private class Cmp implements Comparator {
      final Block blk;
      Cmp (Block blk) {
         this.blk = blk;
      }
      public int compare (Object o1, Object o2) {
         Object intObj1 = Wrapper.newInstance(o1);
         Object intObj2 = Wrapper.newInstance(o2);
         if (intObj1 == null)
            intObj1 = o1;
         if (intObj2 == null)
            intObj2 = o2;
         try {
            Object obj = blk.exec (intObj1, intObj2);
            return ((WInteger) obj).intValue();
         } catch (JOEException ex) {
            throw new RuntimeException (ex);
         }
      }
   }
   /**
    * Sort an array according to the block.
    */
   public Object arraySort (Object array[], Block blk) {
      Arrays.sort (array, new Cmp(blk));
      return array;
   }
   /**
    * Binary search on an array according to the block.
    */
   public Object binarySearch (Object array[], Object key, Block blk) {
      return Arrays.binarySearch (array, key, new Cmp(blk));
   }

   /**
    * Returns a random number in the range 0.0 - 1.0.
    */
   public Object random () {
      return Math.random();
   }
   /**
    * Suspends execution for the specified number of milliseconds
    */
   public void sleep (int millis) throws Exception {
      Thread.sleep (millis);
   }

   /**
    * Returns a Glob object that performs match operations on paths
    * thru the method 'matches'.
    */
   public Glob getGlob (String glob, boolean caseInsensitive) {
      return new Glob (glob, caseInsensitive);
   }
   /**edit 
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
            msg += "()";
         else
            msg += message[i].toString();
      Return = javax.swing.JOptionPane.showInputDialog(null, msg);
      if (Return == null)
         Return = "";
      return Return;
   }
}
