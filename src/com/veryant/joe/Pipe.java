/*
 *
 * This source file is part of the "Java Objects Executor" open source project
 *
 * Copyright 2020 Veryant and Marco Bertacca
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
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.io.Reader;
import java.io.StringReader;
import java.io.StringWriter;
import java.io.Writer;
import java.util.ArrayList;

public class Pipe {
   static class ReadWrite extends Thread {
      final Reader in;
      final Writer out;
      ReadWrite (Reader in, Writer out) {
         this.in = in;
         this.out = out;
      }
      public void run () {
         String line;
         final BufferedReader bri = new BufferedReader(in);
         final PrintWriter pwo = new PrintWriter(out);
         try {
            while ((line = bri.readLine()) != null) {
               pwo.println (line);
            }
         } catch (Exception ignore) {
         }
         try {
            bri.close();
         } catch (Exception ignore) {
         }
         pwo.close();
      }
   }

   final static char eol[] =
             System.getProperty("line.separator","\n").toCharArray();
   private StringWriter writer;
   private int returnCode;
   private boolean errOnOut;

   /**
    *  Executes the specified command in a pipeline
    */
   public Pipe exec (String...cmds) throws Exception {
      ProcessBuilder pb = new ProcessBuilder (cmds);
      pb.redirectErrorStream(errOnOut);
      if (!errOnOut)
         pb.redirectError(ProcessBuilder.Redirect.INHERIT);
      StringReader reader;

      if (writer != null) {
         reader = new StringReader(writer.toString());
      } else {
         pb.redirectInput(ProcessBuilder.Redirect.INHERIT);
         reader = null;
      }
      Process p = pb.start();

      ReadWrite rw1 = new ReadWrite (new InputStreamReader (p.getInputStream()),
                                     writer = new StringWriter());
      rw1.setDaemon(true);
      ReadWrite rw2;
      if (reader != null) {
         rw2 = new ReadWrite  (reader,
                               new OutputStreamWriter (p.getOutputStream()));
         rw2.setDaemon(true);
         rw2.start();
      } else {
         rw2 = null;
      }
      rw1.start();
      returnCode = p.waitFor();
      rw1.join();
      if (rw2 != null)
         rw2.join();
      return this;
   }

   /**
    * Tells to the pipeline if the standard error must be redirected
    * in the output (true) or must go on stderr (false): default is false.
    */
   public Pipe errOnOut(boolean b) {
      errOnOut = b;
      return this;
   }

   /**
    * Put the argument as standard input of the next command in the pipeline.
    */
   public Pipe echo (String s) {
      writer = new StringWriter ();
      writer.append (s);
      return this;
   }

   /**
    * Returns the error code of the last command in the pipeline.
    */
   public int returnCode() {
      return returnCode;
   }

   /**
    * Returns a String array containing the output of the pipeline.
    */
   public String[] toStringArray() {
      if (writer == null)
         return null;
      StringBuffer sb = writer.getBuffer();
      ArrayList<String> array = new ArrayList<String>();
      char c = 0;
      int s = 0;
      final int len = sb.length();
      final int eoll =  eol.length;
      final char eolc =  eol[eoll - 1];

      if (eoll == 2) {
         char prev;
         for (int i = 0; i < len; i++) {
            prev = c;
            c = sb.charAt (i);
            if (c == eolc && prev == eol[0]) {
               array.add (sb.substring(s, i - eoll + 1));
               s = i + 1;
               prev = 0;
            }
         }
      } else {
         for (int i = 0; i < len; i++) {
            c = sb.charAt (i);
            if (c == eolc) {
               array.add (sb.substring(s, i - eoll + 1));
               s = i + 1;
            }
         }
      }
      if (s < len - eoll)
         array.add (sb.substring(s, len));
      String Return[] = new String[array.size()];
      return array.toArray(Return);
   }

   /**
    * Returns a String containing the output of the pipeline.
    */
   public String toString() {
      if (writer == null)
         return null;
      return writer.toString();
   }
}
