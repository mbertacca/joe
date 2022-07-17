/*
 *
 * This source file is part of the "Java Objects Executor" open source project
 *
 * Copyright 2022 Marco Bertacca
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
import java.util.ArrayDeque;

public class Execute {
   static final DefaultCommand command = new DefaultCommand();
   private final StringBuilder messageText = new StringBuilder();
   private final Block block;
   private int lineNum;
   private FileInfo fileInfo = new FileInfo () {
      public String getName () {
         return "<execute>";
      }
      public int getLineNumber () {
         return lineNum;
      }
   };
   public Execute (Block block) {
      this.block = block;
   }
   public Execute add (String line) {
      lineNum++;
      messageText.append (line);
      return this;
   }
   public Execute clear () {
      lineNum = 0;
      messageText.setLength (0);
      return this;
   }
   public Object exec () throws JOEException {
      ArrayDeque<Token> tokens = new ArrayDeque<Token>();
      Tokenizer tkzer = new Tokenizer();
      tkzer.tokenize (messageText.toString().toCharArray(), tokens, fileInfo);
      Parser prg = new Parser(command, new Block(block.executor,block), fileInfo);
      Block b = prg.compile (tokens);
      return b.init();
   }
}
