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

import java.io.Console;
import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.io.StringReader;
import java.util.ArrayDeque;
import java.util.HashMap;

public class JavaObjectsExecutor {
   public static final String rcsid = "$Id$";
   private Block block;

   public JavaObjectsExecutor (String fileName) throws Exception {
      String line;
      BufferedReader fr;
      Tokenizer tkzer = new Tokenizer();
      DefaultCommand defcmd = new DefaultCommand();
      fr = new BufferedReader(new FileReader(fileName));
      ArrayDeque<Token> tokens = new ArrayDeque<Token>();
      while ((line = fr.readLine()) != null) {
         tkzer.tokenize (line.toCharArray(), tokens);
      }
      fr.close();
      Executor exec = new StandardExecutor();
      Parser parser = new Parser(defcmd, exec, fileName);
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
   public Object init () throws Exception {
      return init ((Object[]) null);
   }
   public Object init (Object...a1) throws Exception {
      Object Return = block.init (a1);
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

   public static void showException (DefaultCommand cmd, Throwable ex) {
      cmd.println(ex.getMessage());
      Throwable cause = ex;
      while ((cause = cause.getCause()) != null) {
         cmd.println ("Caused by: " + cause.toString());
      }
   }
   public static void main (String argv[]) {
      int rc = imain (argv);
      if (rc != 0)
         System.exit (rc);
   }
   public static int imain (String argv[]) {
      for ( ; ; )
         try {
            return _main (argv);
         } catch (ExecException newScript) {
            argv = newScript.cmds;
         }
   }
   private static int _main (String argv[]) throws ExecException {
      int Return;
      Console cons;
      String line;
      Executor exec = new StandardExecutor();
      if (argv.length > 0) {
         final String fileName = argv[0];
         DefaultCommand defCmd = new DefaultCommand();
         Parser parser = new Parser(defCmd,exec,fileName);
         Tokenizer tkzer = new Tokenizer();
         BufferedReader fr;
         try {
            fr = new BufferedReader(new FileReader(fileName));
            ArrayDeque<Token> tokens = new ArrayDeque<Token>();
            while ((line = fr.readLine()) != null) {
               tkzer.tokenize (line.toCharArray(), tokens);
/*
               for (Token tk : tokens)
                  System.out.println (tk.toString());
*/
            }
            fr.close();
            parser.compile (tokens).exec(new WArray(argv));
         } catch (BreakEndException ex) {
            Return = 0;
         } catch (ExecException ex) {
            throw ex;
         } catch (BreakCmdException ex) {
            showException(defCmd, new BreakCmdException (
                          "Block name not found: " + ex.getMessage()));
            Return = 2;
         } catch (JOEException ex) {
            showException(defCmd, ex);
            Return = 2;
         } catch (FileNotFoundException ex) {
            showException(defCmd, ex);
            Return = 3;
         } catch (IOException ex) {
            showException(defCmd, ex);
            Return = 4;
         }
         Return = 0;
      } else {
         Return = 0;
         DefaultCommand defCmd = new DefaultCommand();
         Object cmd = defCmd;
         line = "";
         defCmd.println (
             "JOE interactive ready, type 'exit' to exit the session");
         defCmd.println ();
         Parser prg = new Parser(cmd, exec,"<stdin>");
         Block b = null;

         while (!"exit".equals (line)) {
            defCmd.print ("joe> ");
            try {
               line = defCmd.readLine().toString();
               //defCmd.print (line);
            } catch (IOException ex) {
               showException(defCmd, ex);
               Return = 4;
               break;
            }
            ArrayDeque<Token> tokens = new ArrayDeque<Token>();
            Tokenizer tkzer = new Tokenizer();
            tkzer.tokenize (line.toCharArray(), tokens);
            try {
               b = prg.compile (tokens);
               try {
                  b.init();
                  cmd = prg.getCommand();
               } catch (ExecException ex) {
                  throw ex;
               } catch (JOEException ex) {
                  showException(defCmd, ex);
               } finally {
                  b.clear();
               }
            } catch (JOEException ex) {
               showException(defCmd, ex);
            }
         }
      }
      return Return;
   }
   public static Block newJoe (String fileName, Object...argv)
                                                            throws Exception {
      return newJoe (fileName,
                     new BufferedReader(new FileReader(fileName)), argv);
   }
   public static Block newJoe (String name, String code, Object...argv)
                                                            throws Exception {
      return newJoe (name, new BufferedReader (new StringReader (code)), argv);
   }
   public static Block newJoe (String name, BufferedReader code, Object...argv)
                                                            throws Exception {
      return OuterBlock.get (name, code, argv,
                             new DefaultCommand(), new StandardExecutor());
   }
}

