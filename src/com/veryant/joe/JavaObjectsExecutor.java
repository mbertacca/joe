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
   static protected DefaultCommand command = new DefaultCommand();
   static protected Executor executor = new StandardExecutor();
   static protected LineReader lineReader = new BasicLineReader();
   static protected String version = "JOE " + Revision.id 
              + ", type 'exit' to exit the session";

   public static void showException (DefaultCommand cmd, Throwable ex) {
      cmd.eprintln(ex.getMessage());
      Throwable cause = ex;
      while ((cause = cause.getCause()) != null) {
         cmd.eprintln ("Caused by: " + cause.toString());
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
      int Return = 0;
      Console cons;
      String line;
      Executor exec = executor;
      DefaultCommand defCmd = command;
      LineReader lr = lineReader;
      if (argv.length > 0) {
         ScriptManager sm;
         try {
            final File f = new File (argv[0]).getCanonicalFile();
            sm = new ScriptManager(f.getParentFile(), exec, defCmd, lr);
            Object[] jarg = new Object [] {new WArray (argv)};
            Block blk = sm.load (f.getName(), jarg, new String[0]);
            Object rc  = blk.init (jarg);
            if (rc instanceof WNumber) {
               Return = ((WNumber) rc).intValue();
            }
         } catch (BreakEndException ex) {
            Return = ex.rc;
         } catch (ExecException ex) {
            throw ex;
         } catch (BreakCmdException ex) {
            showException(defCmd, new BreakCmdException (
                          "Block name not found: " + ex.getMessage()));
            Return = 65;
         } catch (JOEException ex) {
            showException(defCmd, ex);
            Return = 66;
         } catch (FileNotFoundException ex) {
            showException(defCmd, ex);
            Return = 67;
         } catch (IOException ex) {
            showException(defCmd, ex);
            Return = 68;
         }
      } else {
         Return = 0;
         File cwd = new java.io.File (System.getProperty("user.dir"));
         ScriptManager sm = new ScriptManager(cwd, exec, defCmd, lr);

         Object cmd = defCmd;
         line = "";
         defCmd.println (version);
         defCmd.println ();
         Parser prg = new Parser(cmd, exec, lr);
         Block b = null;

         for ( ; ; ) {
            try {
               line = readLine(defCmd);
            } catch (IOException ex) {
               showException(defCmd, ex);
               Return = 69;
               break;
            }
            if (line == null || "exit".equals (line))
               break;
            ArrayDeque<Token> tokens = new ArrayDeque<Token>();
            Tokenizer tkzer = new Tokenizer();
            tkzer.tokenize (line.toCharArray(), tokens, lineReader);
            try {
               b = prg.compile (tokens);
               try {
                  defCmd.println ("---> ",executor.run(b));
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
   static String readLine (DefaultCommand defCmd) throws IOException {
      boolean isConsole = defCmd.isConsole();
      StringBuilder buffer = new StringBuilder();
      String line = null;
      int braces = 0;
      boolean missingDot = false;
      do {
         if (defCmd.isConsole()) {
            if (braces > 0) {
               if (braces < 10)
                  defCmd.print ("00" + braces + "> ");
               else if (braces < 100)
                  defCmd.print ("0" + braces + "> ");
               else
                  defCmd.print (braces + "> ");
            } else if (missingDot) {
               defCmd.print (" . > ");
            } else {
               defCmd.print ("joe> ");
            }
         }
         line = defCmd.readLine();
         if (line == null || (line = line.trim()).equals ("exit")) {
            line = "exit";
            buffer.delete (0, buffer.length());
            buffer.append (line);
            braces = 0;
         } else if ("".equals (line)) {
            if (missingDot || braces != 0) {
               braces = 0;
               missingDot = false;
            }
         } else {
            line = line.trim();
            buffer.append (line);
            buffer.append (' ');
            braces = countBraces (line, braces);
            missingDot = line.charAt (line.length() - 1) != '.';
         }
      } while ((braces > 0 || missingDot) && !"exit".equals (line));
      return buffer.toString();
   }
   static int countBraces (String line, int braces) {
      boolean inquote = false;
      for (int i = 0; i < line.length(); i++) {
         switch (line.charAt (i)) {
         case '{':
            if (!inquote)
               braces++;
            break;
         case '}':
            if (!inquote)
               braces--;
            break;
         case '"':
            inquote = !inquote;
            break;
         default:
            break;
         }
      }
      return braces;
   }
}

