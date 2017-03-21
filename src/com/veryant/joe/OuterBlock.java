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
import java.util.ArrayDeque;


public class OuterBlock extends Block {
   public static Block get (String name, BufferedReader src, Object[] argv,
                            Object cmd, Executor exec) throws Exception {
      String line;
      final Tokenizer tkzer = new Tokenizer();

      final ArrayDeque<Token> tokens = new ArrayDeque<Token>();
      while ((line = src.readLine()) != null) {
         tkzer.tokenize (line.toCharArray(), tokens);
      }
      src.close();
      final Parser parser = new Parser(cmd, exec, name);
      final Block Return = parser.compile (tokens);
      if (argv != null) {
         Object wobj;
         for (int i = 0; i < argv.length; i++) 
            if ((wobj = Wrapper.newInstance (argv[i])) != null)
               argv[i] = wobj;
      }
      Return.sfExec (argv);
      return Return;
   }

   public OuterBlock (Executor exec) {
      super (exec, null);
   }
   Block getMethod (String name) {
      Object Return = getVariable (name);
      if (Return instanceof Block) {
          return (Block) Return;
      } else {
          return null;
      }
   }
   public String toString() {
      Block joeToString = getMethod("toString");
      if (joeToString != null)
         try {
            return joeToString.exec().toString();
         } catch (JOEException _ex) {
            return _ex.toString();
         }
      else
         return name();
   }
}

