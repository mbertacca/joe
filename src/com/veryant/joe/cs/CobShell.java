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

import com.veryant.joe.Block;
import com.veryant.joe.ExecException;
import com.veryant.joe.Executor;
import com.veryant.joe.JOEException;
import com.veryant.joe.Parser;
import com.veryant.joe.Token;
import com.veryant.joe.Tokenizer;
import com.veryant.joe.WArray;
import com.veryant.joe.Wrapper;

import java.io.Console;
import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayDeque;
import java.util.HashMap;

public class CobShell {
   public static final String rcsid = "$Id$";
   public static final String usage = " [-tron] program";

   private Block block;

   public CobShell (String fileName) throws Exception {
      String line;
      BufferedReader fr;
      Tokenizer tkzer = new Tokenizer();
      CobolCommand cobcmd = new CobolCommand();
      fr = new BufferedReader(new FileReader(fileName));
      ArrayDeque<Token> tokens = new ArrayDeque<Token>();
      while ((line = fr.readLine()) != null) {
         tkzer.tokenize (line.toCharArray(), tokens);
      }
      fr.close();
      Executor exec = new VSExecutor(false);
      Parser parser = new Parser(cobcmd, exec);
      block = parser.compile (tokens);
   }
   public Object exec () throws Exception {
      return exec ((Object[]) null);
   }
   public Object exec (Object...a1) throws Exception {
      Object Return = block.exec (a1);
      if (Return instanceof Wrapper)
         Return = ((Wrapper) Return).getWrapped();
      return Return;
   }
   public Object sfExec () throws Exception {
      return sfExec ((Object[]) null);
   }
   public Object sfExec (Object...a1) throws Exception {
      Object Return = block.sfExec (a1);
      if (Return instanceof Wrapper)
         Return = ((Wrapper) Return).getWrapped();
      return Return;
   }
   public Object execBlock (String name) throws Exception {
      return execBlock (name, (Object[]) null);
   }
   public Object execBlock (String name, Object...a1) throws Exception {
      Object Return = block.execBlock (name, a1);
      if (Return instanceof Wrapper)
         Return = ((Wrapper) Return).getWrapped();
      return Return;
   }

   public static void showException (String fileName,
                                     CobolCommand cmd, Throwable ex) {
      cmd.display(fileName + ": " + ex.getMessage());
      Throwable cause = ex;
      while ((cause = cause.getCause()) != null) {
         cmd.display ("Caused by: " + cause.toString());
      }
   }

   public static void main (String argv[]) {
      CobolCommand cobcmd = new CobolCommand();
      int rc = imain (cobcmd, argv);
      if (rc != 0)
         cobcmd.accept();
      cobcmd.stopRun(rc);
   }

   public static int imain (String argv[]) {
      return imain (new CobolCommand(), argv);
   }
   public static int imain (CobolCommand cobcmd, String argv[]) {
      for ( ; ; )
          try {
             return _main (cobcmd, argv);
         } catch (ExecException newScript) {
            argv = newScript.cmds;
         }
   }
   private static int _main (CobolCommand cobcmd, String argv[])
                     throws ExecException {
      int Return;
      boolean tron = false;
      Console cons;
      String line;
      int argIdx = 0;

      if (argIdx < argv.length && "-tron".equals(argv[argIdx])) {
         tron = true;
         argIdx++;
      }
      
      if (argIdx < argv.length) {
         final String fileName = argv[argIdx];
         BufferedReader fr;
         try {
            Tokenizer tkzer = new Tokenizer();
            fr = new BufferedReader(new FileReader(fileName));
            ArrayDeque<Token> tokens = new ArrayDeque<Token>();
            while ((line = fr.readLine()) != null) {
               tkzer.tokenize (line.toCharArray(), tokens);
/*
               for (Token tk : tokens)
                  System.out.display (tk.toString());
*/
            }
            fr.close();
            Executor exec = new VSExecutor(tron);
            Parser prg = new Parser(cobcmd, exec);
            prg.compile (tokens).exec(new WArray(argv));
            Return = 0;
         } catch (ExecException ex) {
            throw ex;
         } catch (JOEException ex) {
            showException (fileName, cobcmd, ex);
            Return = 2;
         } catch (IOException ex) {
            showException (fileName, cobcmd, ex);
            Return = 3;
         }
      } else {
         Executor exec = new VSExecutor(tron);
         Object cmd = cobcmd;

         line = "";
         cobcmd.display (true,
             "CobShell interactive ready, type 'exit' to exit the session");
         cobcmd.display ();
         Parser prg = new Parser(cmd, exec);
         Block b = null;
         Return = 0;

         while (!"exit".equals (line)) {
            cobcmd.display (false, "cs> ");
            line = cobcmd.accept().toString();
            //cobcmd.display (true, line);
            ArrayDeque<Token> tokens = new ArrayDeque<Token>();
            Tokenizer tkzer = new Tokenizer();
            tkzer.tokenize (line.toCharArray(), tokens);
            try {
               b = prg.compile (tokens);
               try {
                  b.sfExec();
                  cmd = prg.getCommand();
               } catch (Exception ex) {
                  showException ("<stdin>", cobcmd, ex);
                  Return = 2;
                  break;
               } finally {
                  b.clear();
               }
            } catch (Exception ex) {
               showException ("<stdin>", cobcmd, ex);
               Return = 3;
               break;
            }
         }
      }
      return Return;
   }
}
