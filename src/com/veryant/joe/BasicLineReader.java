/*
 *
 * This source file is part of the "Java Objects Executor" open source project
 *
 * Copyright 2021 Marco Bertacca
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
import java.io.IOException;
import java.io.Reader;

public class BasicLineReader implements LineReader {
   protected BufferedReader reader;

   public void open (Reader r) {
      reader = new BufferedReader (r);
   }
   public String readLine () throws IOException {
      return reader.readLine();
   }
   public void close () throws IOException {
      reader.close();
   }
}
