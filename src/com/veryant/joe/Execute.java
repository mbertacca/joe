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
import java.util.ArrayList;

public class Execute {
   private final CommandBase command;
   private final String fileName;
   private final ArrayList<String> pgmLines = new ArrayList<String>();
   private final Block block;
   private int lineNum;
   private FileInfo fileInfo = new FileInfo () {
      public String getName () {
         return fileName;
      }
      public int getLineNumber () {
         return lineNum;
      }
   };
   public Execute (Block block, CommandBase cb, String fname) {
      this.block = block;
      if (cb != null)
         this.command = cb;
      else
         this.command = new DefaultCommand();
      if (fname != null)
         this.fileName = fname;
      else
         this.fileName = "<execute>";
   }
   public Execute (Block block, String fname) {
      this (block, null, fname);
   }
   public Execute (Block block) {
      this (block, null, null);
   }
   public Execute add (String line) {
      pgmLines.add (line);
      return this;
   }
   public Execute clear () {
      lineNum = 0;
      pgmLines.clear ();
      return this;
   }
   public Object exec (Object argv[]) throws JOEException {
      ArrayDeque<Token> tokens = new ArrayDeque<Token>();
      Tokenizer tkzer = new Tokenizer();
      for (String line : pgmLines) {
         lineNum++;
         tkzer.tokenize (line.toCharArray(), tokens, fileInfo);
      }
      Parser prg = new Parser(command, new Block(block.executor,block), fileInfo);
      Block b = prg.compile (tokens);
      if (tokens.size() > 0) {
         Token tk = tokens.pop();
         throw new JOEException (
                "Token(s) external to any block `" + tk.word + "`", tk);
      }
      return b.init(new Object[] { argv });
   }
   public Object exec () throws JOEException {
      return exec (null);
   }
}
