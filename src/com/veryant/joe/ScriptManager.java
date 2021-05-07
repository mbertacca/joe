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
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.MalformedURLException;
import java.net.URI;
import java.net.URISyntaxException;
import java.net.URL;
import java.security.CodeSource;
import java.util.HashMap;
import java.util.zip.ZipInputStream;
import java.util.zip.ZipEntry;

public class ScriptManager {
   private static final HashMap<Object,ScriptManager> scriptManagerMap =
                    new HashMap<Object,ScriptManager>();
   public static ScriptManager get (Object cmd) {
      return scriptManagerMap.get (cmd);
   }
   public static Block load (Object cmd, String name, Object[] argv)
                                             throws IOException,JOEException {
      ScriptManager sm = ScriptManager.get (cmd);
      if (sm == null)
         throw new RuntimeException ("ScriptManager not found!");
      return sm.load (name, argv);
   }
   static URL getURL (File f) {
      try {
         return f.toURI().toURL();
      } catch (MalformedURLException ex) {
         throw new RuntimeException (ex);
      }
   }

   protected final URL entryPoint;
   protected final Executor executor;
   protected final Object command;
   protected final LineReader lineReader;
   protected final HashMap<String,Block> blocks=new HashMap<String,Block>();

   public ScriptManager (URL ep, Executor exec, Object cmd, LineReader lr) {
      entryPoint = ep;
      executor = exec;
      command = cmd;
      lineReader = lr;
      scriptManagerMap.put (command, this);
   }
   public ScriptManager (File ep, Executor exec, Object cmd, LineReader lr) {
      this (getURL (ep), exec, cmd, lr);
   }
   private InputStream getStreamFromJar (String name) throws IOException {
      ZipInputStream zip = new ZipInputStream(entryPoint.openStream());
      for ( ; ; ) {
         ZipEntry e = zip.getNextEntry();
         if (e == null)
            break;
         String n = e.getName();
         if (n.equals (name))
            return zip;
      }
      return null;
   }
   public URL getEntryPoint () {
      return entryPoint;
   }
   public URL getURL (String spec) throws MalformedURLException {
      return new URL (entryPoint, spec);
   }
   public InputStream getInputStream (String name) throws IOException {
      InputStream Return;
      File f;

      try {
         f = new File (entryPoint.toURI());
      } catch (URISyntaxException _ex) {
         throw new FileNotFoundException (entryPoint.getFile());
      }
      if (f.isDirectory()) {
         f = new File (f, name);
         Return = new FileInputStream (f);
      } else {
         Return = getStreamFromJar (name);
         if (Return == null) 
            throw new FileNotFoundException (name);
      }
      return Return;
   }
   public Block load (String name, Object[] argv)
                               throws IOException,JOEException {
      Block Return = blocks.get (name);
      if (Return != null) {
         Return = (Block) Return.clone();
         if (argv != null) {
            Object wobj;
            for (int i = 0; i < argv.length; i++) 
               if ((wobj = Wrapper.newInstance (argv[i])) != null)
                  argv[i] = wobj;
         }
      } else {
         lineReader.open (name, new InputStreamReader(getInputStream(name)));
         Return = OuterBlock.get (name, lineReader, argv, command, executor);
         blocks.put (name, Return);
      }
      return Return;
   }
/*
      CodeSource src = clazz.getProtectionDomain().getCodeSource();
      URL jar = src.getLocation();
      try {
         System.out.println ("[" + jar + "][" + jar.openStream() + "]");
         File f = new File (jar.getFile());
         System.out.println ("[" + f + "][" + f.exists() + "]");
         System.out.println ("[" + f.isDirectory() + "][" + f.isFile() + "]");

    
      } catch (Exception _ex) {
         System.out.println (_ex);
      }
   }
*/
/*
   public static Block get (String name, BufferedReader src, Object[] argv,
                            Object cmd, Executor exec) throws Exception {
   }
*/
}

