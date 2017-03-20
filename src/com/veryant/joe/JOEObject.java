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

public class JOEObject implements InternalObject {
   private final String name;
   public final Block block;
   public JOEObject (String name, BufferedReader src, Object[] argv,
                     Object cmd, Executor exec) throws Exception {
      String line;
      this.name = name;
      Tokenizer tkzer = new Tokenizer();

      ArrayDeque<Token> tokens = new ArrayDeque<Token>();
      while ((line = src.readLine()) != null) {
         tkzer.tokenize (line.toCharArray(), tokens);
      }
      src.close();
      Parser parser = new Parser(cmd, exec, name);
      block = parser.compile (tokens);
      block.sfExec (argv);
   }
   public Block getMethod (String name) {
      Object Return = block.getVariable (name);
      if (Return instanceof Block) {
          return (Block) Return;
      } else {
          return null;
      }
   }
   public String toString () {
      return name;
   }
}
