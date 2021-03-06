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

import java.io.IOException;
import java.util.ArrayDeque;


public class OuterBlock extends Block {
   public static Block get (String name, LineReader src, Object[] argv,
                            Object cmd, Executor exec)
                                          throws IOException, JOEException {
      String line;
      final Tokenizer tkzer = new Tokenizer();

      final ArrayDeque<Token> tokens = new ArrayDeque<Token>();
      while ((line = src.readLine()) != null) {
         tkzer.tokenize (line.toCharArray(), tokens, src);
      }
      src.close();
      final Parser parser = new Parser(cmd, exec, src);
      final Block Return = parser.compile (tokens);
      if (argv != null) {
         Object wobj;
         for (int i = 0; i < argv.length; i++) 
            if ((wobj = Wrapper.newInstance (argv[i])) != null)
               argv[i] = wobj;
      }
      if (tokens.size() > 0) {
         Token tk = tokens.pop();
         throw new JOEException (
                "Token(s) external to any block `" + tk.word + "`", tk);
      }
      return Return;
   }
   public OuterBlock (Executor exec) {
      super (exec, null);
   }
}

