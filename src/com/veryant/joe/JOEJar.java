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

import java.io.File;
import java.io.BufferedReader;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.IOException;
import java.net.URL;
import java.security.CodeSource;
import java.util.ArrayList;
import java.util.zip.ZipInputStream;
import java.util.zip.ZipEntry;

public class JOEJar {
   public static void usage (URL jar, String[] sList) {
      System.err.println ("usage: java -jar " +
                          new File (jar.getFile()).getName() + 
                          " [<script> [arg ...]]");
      System.err.println ("scripts available are:");
      for (int i = 0; i < sList.length; i++)
         System.err.println ("\t" + sList[i]);
   }

   private static String [] getScriptList(URL jar) {
      ArrayList<String> Return = new ArrayList<String>();
      try {
         ZipInputStream zip = new ZipInputStream(jar.openStream());
         for ( ; ; ) {
            ZipEntry e = zip.getNextEntry();
            if (e == null)
               break;
            String n = e.getName();
            if (n.endsWith (".joe"))
               Return.add (n);
         }
      } catch (IOException _ex) {
         _ex.printStackTrace();
      }
      return Return.toArray (new String[Return.size()]);
   }

   public static void showException (DefaultCommand cmd, Throwable ex) {
      cmd.println(ex);
      Throwable cause = ex;
      while ((cause = cause.getCause()) != null) {
         cmd.println ("Caused by: " + cause.toString());
      }
   }

   public static void main (String argv[]) throws Exception {
      Class clazz = new JOEJar().getClass();
      CodeSource src = clazz.getProtectionDomain().getCodeSource();
      URL jar = src.getLocation();
      String[] sList = null;
      final String script;
      final Object[] jarg;

      if (argv.length < 1) {
         sList = getScriptList(jar);
         if (sList.length == 1) {
            script = sList[0];
            jarg = new Object [] {new String[] {script}};
         } else {
            script = "";
            jarg = new Object [] {new String[] {""}};
            usage (jar, sList);
         }
      } else {
         script = argv[0];
         jarg = new Object [] {new WArray (argv)};
      }

      Executor exec = new StandardExecutor();
      DefaultCommand defCmd = new DefaultCommand();
      ScriptManager sm;
      try {
         sm = new ScriptManager(jar, exec, defCmd);
         Block blk = sm.load (script, jarg);
         blk.init (jarg);
      } catch (BreakEndException ex) {
         System.exit (0);
      } catch (ExecException ex) {
         throw ex;
      } catch (BreakCmdException ex) {
         showException(defCmd, new BreakCmdException (
                          "Block name not found: " + ex.getMessage()));
         System.exit (3);
      } catch (JOEException ex) {
         showException(defCmd, ex);
         System.exit (3);
      } catch (IOException ex) {
         showException(defCmd, ex);
         if (sList == null)
            sList = getScriptList(jar);
         usage (jar, sList);
      }

/*
      InputStream is = clazz.getResourceAsStream (argv[0]);
      if (is == null) {
         System.err.println ("Error: " + argv[0] + " not found");
         usage (jar);
      }
      Object[] jarg = new Object [] {new WArray (argv)};
      BufferedReader code = new BufferedReader (new InputStreamReader(is));
      Block blk = OuterBlock.get (argv[0], code, jarg,
                             new DefaultCommand(), new StandardExecutor());
*/
   }
}
