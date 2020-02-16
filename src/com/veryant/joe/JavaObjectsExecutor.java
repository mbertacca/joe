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
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.io.StringReader;
import java.util.ArrayDeque;
import java.util.HashMap;

public class JavaObjectsExecutor {
   public static final String rcsid = "1.6";
   private Block block;

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
         DefaultCommand defCmd = new DefaultCommand();
         ScriptManager sm;
         try {
            final File f = new File (argv[0]).getCanonicalFile();
            sm = new ScriptManager(f.getParentFile(), exec, defCmd);
            Object[] jarg = new Object [] {new WArray (argv)};
            Block blk = sm.load (f.getName(), jarg);
            blk.init (jarg);
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
         File cwd = new java.io.File (System.getProperty("user.dir"));
         ScriptManager sm = new ScriptManager(cwd, exec, defCmd);

         Object cmd = defCmd;
         line = "";
         defCmd.println (
             "JOE interactive ver. " + rcsid +
             " ready, type 'exit' to exit the session");
         defCmd.println ();
         Parser prg = new Parser(cmd, exec,"<stdin>");
         Block b = null;

         while (line != null && !"exit".equals (line)) {
            defCmd.print ("joe> ");
            try {
               line = defCmd.readLine();
               if (line == null)
                  break;
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
}

