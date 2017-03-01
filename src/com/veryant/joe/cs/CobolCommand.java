/*
          ooo
   oo    ooo
    oo  ooo
v e r y a n t
      ooo

http://www.veryant.com

 (C) 2016
 ________________
/  _   _   ____  \
| | | | | |  __| |
| | |_| | | |__  |
| |     | |____| |
\_|     |________/
Marco   Bertacca
*/

package com.veryant.joe.cs;

import com.iscobol.gui.server.ScrFactory;
import com.iscobol.java.CobolVarHelper;
import com.iscobol.java.IsCobol;
import com.iscobol.rts.ICobolVar;
import com.iscobol.rts.Factory;

import com.veryant.joe.Block;
import com.veryant.joe.BreakCmdException;
import com.veryant.joe.BreakLoopException;
import com.veryant.joe.ClassReference;
import com.veryant.joe.CommandBase;
import com.veryant.joe.ExecException;
import com.veryant.joe.JOEException;
import com.veryant.joe.WBoolean;

import java.io.BufferedReader;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.IOException;
import java.io.PrintWriter;
import java.io.StringWriter;
import java.util.HashMap;
import java.lang.reflect.Constructor;
import java.lang.reflect.Field;

/**
 * This class contains methods that makes the flavor of JOE
 * similar to the COBOL language
 */

public class CobolCommand extends CommandBase {
   public static final String rcsid = "$Id$";
   public Object args;

   /**
    * Read a string from the standard isCOBOL input.
    */
   public String accept () {
   // this doesn't work well for Unicode, only for ASCII chars
      CobolVarHelper cvh = new CobolVarHelper ("cvh",CobolVarHelper._DCA)
         .picXAnyLength ("inp");
      ICobolVar inp = cvh.get("inp");
    
      ScrFactory.getGUIEnviroment().accept ((ICobolVar)null,
                            (ICobolVar)null, (ICobolVar)null, null, null, inp);
      return inp.toString();
   }
   /**
    * This is a convenience method for
    * <code>display(true, b)</code>.
    */
   public Object display (Object... b) {
      return display (true, b);
   }
   /**
    * This is a convenience method for
    * <code>display(false, b)</code>.
    */
   public Object displayNoAdv (Object... b) {
      return display (false, b);
   }
   /**
    * Shows the arguments on the isCOBOL standard output.
    * If the first argument is true then a newline character
    * is appended at the end.
    * 
    * @return         the object itself
    */
   public Object display (boolean advancing, Object... b) {
      String s = "";
      for (int i = 0; i < b.length; i++) {
         if (b[i] == null)
            s += "(null)";
         else
            s += b[i].toString();
      }
      ScrFactory.getGUIEnviroment().display (s, advancing);
      return this;
   }
   /**
    * Shows the arguments on a isCOBOL message box.
    * 
    * @return         the object itself
    */
   public Object displayMessage (Object... b) {
      String s = "";
      for (int i = 0; i < b.length; i++) {
         if (b[i] == null)
            s += "(null)";
         else
            s += b[i].toString();
      }
      ScrFactory.getGUIMessageBox().setText (s).show(null);
      return this;
   }
   public Object display () {
      display ("");
      return this;
   }
   /**
    * Terminates the currently running isCOBOL session.
    */
   public void stopRun (int ec) {
      Factory.stopRun (ec);
   }
   /**
    * This is a convenience method for
    * <code>stopRun(0)</code>.
    */
   public void stopRun () {
      stopRun (0);
   }
   /**
    * Sets an environment variable.
    */
   public Object setEnvironment (String key, String value) {
      Factory.setEnv(key, value);
      return this;
   }
   /**
    * Gets an environment variable.
    */
   public Object getEnvironment (String key) {
      Object Return = Factory.acceptFromEnv(key, false);
      return Return;
   }
   /**
    * Makes an integer of the string representation of the passed object.
    */
   public Integer toInt (Object obj) {
      return Integer.parseInt (obj.toString());
   }
   /**
    * Calls an isCOBOL subroutine supplying the passed arguments when present.
    * If an arguments is an isCOBOL variable (i.e. implements ICobolVar) then
    * it is passed as it is.
    * If an arguments is an integer then its value is put in a PIC S9(9) COMP,
    * otherwise its string representation is put in a PIC X(n).
    *
    * @param          name the name of the isCOBOL subroutine to call
    * @param          args the parameters of the call
    *
    * @return         the CALL return code
    */
   public Integer call (String name, Object... args) {
      int Return;
      if (args != null && args.length > 0) {
         ICobolVar cobArgs[] = new ICobolVar[args.length];

         CobolVarHelper cvh = new CobolVarHelper ("cvh",CobolVarHelper._DCA);
         for (int i = 0; i < args.length; i++) {
            if (args[i] instanceof ICobolVar)
               cobArgs[i] = (ICobolVar) args[i];
            else if (args[i] instanceof Integer)
               cvh.picS9Comp ("arg"+i, 9, 9);
            else {
               cvh.picX ("arg"+i, args[i].toString().length());
            }
         }
         for (int i = 0; i < args.length; i++)
            if (cobArgs[i] == null) {
               cobArgs[i] = cvh.get ("arg" + i);
               if (args[i] instanceof Integer)
                  cobArgs[i].set (((Integer) args[i]).intValue());
               else
                  cobArgs[i].set (args[i].toString());
            }
         Return = IsCobol.call (name, cobArgs, false);
      } else {
         Return = IsCobol.call (name, null, false);
      }
      return Return;
   }
   /**
    * This is a convenience method for
    * <code>call (name, (Object[]) null);</code>.
    */
   public Integer call (String name) {
      return call (name, (Object[]) null);
   }
   /**
    * Cancel the name subroutine from memory
    */
   public void cancel (String name) {
      IsCobol.cancel (name);
   }
   /**
    * if implementation without 'else'.
    */
   public Object $if (Boolean cond, Block ifTrue) throws Exception {
      Object Return = cond;
      if (cond.booleanValue()) {
         Return = ifTrue.exec();
      }
      return Return;
   }
   /**
    * if implementation with 'else'.
    */
   public Object $if (Boolean cond, Block ifTrue, Block ifFalse)
                                                             throws Exception {
      Object Return;
      if (cond.booleanValue()) {
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
    * This class is used to implement a multiway branch.
    */
   public static class Evaluate {
      final Object cfrt1;
      boolean alreadyDone;
      boolean prevCondition;
      Object Return;

      Evaluate (Object cfrt) {
         this.cfrt1 = cfrt;
      }
      public Evaluate when (Object cfrt2) {
         prevCondition |= cfrt1.equals (cfrt2);
         return this;
      }
      public Evaluate when (Object cfrt2, Block block) throws Exception {
         if (!alreadyDone && (prevCondition || cfrt1.equals (cfrt2))) {
            alreadyDone = true;
            Return = block.exec();
         } else {
            prevCondition = false;
         }
         return this;
      }
      public Evaluate when_other (Block block) throws Exception {
         if (!alreadyDone) {
            alreadyDone = true;
            Return = block.exec();
         }
         return this;
      }
      public Object end_evaluate () {
         return Return;
      }
   }
   /**
    * Start of a multiway branch.
    */
   public Evaluate evaluate (Object cfrt) {
      return new Evaluate (cfrt);
   }

   /**
    * Executes a loop on the second argument until the first argument is true.
    */
   public Object until (Block cond, Block code) throws Exception {
      Object cnd;
      Object Return = null;
      while ((cnd = cond.exec()) != null && cnd instanceof WBoolean &&
             !((WBoolean) cnd).booleanValue())
         try {
            Return = code.exec();
         } catch (BreakLoopException _ex) {
            break;
         }

      return Return;
   }
   /**
    * Causes the exit from a loop
    */
   public void exit_loop() throws BreakLoopException {
      throw new BreakLoopException();
   }

   /**
    * Checks if the label named as arguemnt exists or not.
    */
   public LabelExists labelExists (String lbl) {
      return new LabelExists (lbl);
   }
   /**
    * Jumps to the specified label.
    */
   public GoTo $goto (String lbl) throws Exception {
      return new GoTo (lbl);
   }
   /**
    * Perform the code after the specified label.
    */
   public GoSub perform (String lbl) throws Exception {
      return new GoSub (lbl);
   }
   /**
    * Exits from the last perform executed.
    */
   public FlowControl exit () throws Exception {
      return new FlowControl ();
   }
   /**
    * Convenience method for exit_bloc ("").
    */
   public Object exit_block () throws Exception {
      throw new BreakCmdException ("");
   }
   /**
    * Exits from the block whose name is blkName;
    * if blkName = "" then exits from the current block;
    * the block returns the object result of the last invocation.
    */
   public Object exit_block (String blkName) throws Exception {
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

   private static String OS = System.getProperty("os.name");
   private Process myExec (String cmd) throws IOException {
      Process Return;
      Runtime rt = Runtime.getRuntime();
      if (OS.startsWith ("Windows")) {
         Return = rt.exec(new String[] {"cmd", "/c", cmd} );
      } if (OS.matches (".*n.*x.*")) {
         Return = rt.exec(new String[] {"sh", "-c", cmd} );
      } else {
         Return = rt.exec(cmd);
      }
      return Return;
   }
   /**
    * Executes the specified OS command and returns its return code.
    */
   public int exec (String cmd) throws Exception {
/*
      class ResultReader extends Thread {
         final InputStream is;
         ResultReader (InputStream is) {
            this.is = is;
         }
         public void run () {
            BufferedReader br = new BufferedReader(new InputStreamReader (is));
            String line;
            try {
               while ((line = br.readLine()) != null)
                  display (line);
            } catch (IOException ex) {
               StringWriter errors = new StringWriter();
               ex.printStackTrace(new PrintWriter(errors));
               display (errors.toString());
            }
         }
      };
*/
      final Process proc = myExec (cmd);
      BufferedReader bro = new BufferedReader(
                              new InputStreamReader (proc.getInputStream()));
      BufferedReader bre = new BufferedReader(
                              new InputStreamReader (proc.getErrorStream()));
       String line;
       while ((line = bro.readLine()) != null ||
              (line = bre.readLine()) != null)
          display (line);
/*
      Thread out = new ResultReader(proc.getInputStream());
      Thread err = new ResultReader(proc.getErrorStream());
      out.start();
      err.start();
      out.join();
      err.join();
*/
      return proc.waitFor();
   }
   /**
    * Executes the specified OS command and returns its standard output
    * as a string.
    */
   public String execGetOut (String cmd) throws Exception {
      final Process proc = myExec (cmd);
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
            Return.append (" ");
            Return.append (line);
         } else if ((line = bre.readLine()) != null)
            display (true, line);
         else
            break;
      }
      return Return.toString();
   }
   /**
    * Executes the specified JOE script and returns its return code.
    */
   public int runJoe (String...cmds) throws Exception {
      return CobShell.imain (cmds);
   }
   /** Stops the execution of the current script and executes the
    * script specified as argument.
    */
   public int execJoe (String...cmds) throws ExecException {
      throw new ExecException (cmds);
   }
}
