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

import java.util.ArrayDeque;

enum TokenType {
   _COMMA_,
   _BANG_,
   _BANGBANG_,
   _SEMICOLON_,
   _COLON_,
   _DOT_,
   _PAR_OPEN_,
   _PAR_CLOSE_,
   _BRACE_OPEN_,
   _BRACE_CLOSE_,
   _INIT,
   _WORD,
   _STRING,
   _UNCLOSED_STRING,
   _INTEGER,
   _LONG,
   _FLOAT,
   _DECIMAL,
   _ASSIGN,
   _CONSTANT,
   _ML_COMMENT,
   _COMMENT,
   _TRUE,
   _FALSE,
   _NULL
}

