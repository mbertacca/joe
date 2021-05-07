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

public class Tokenizer {
   public static final String rcsid = "$Id$";

   private ArrayDeque<Token> tokens;
   private StringBuilder word;
   private TokenType status = TokenType._INIT;
   private int cCol = 0;
   private int cRow = 0;
   private String fname;

   private Token newToken (String w, TokenType t) {
      return new Token (w, t, cRow, cCol - w.length(), fname);
   }
   private Token newToken (String w, TokenType t, int col) {
      return new Token (w, t, cRow, col, fname);
   }
   private boolean breakChar (char c) {
      boolean Return = true;
      switch (status) {
      case _INIT:
         break;
      case _INTEGER:
      case _FLOAT:
      case _DECIMAL:
      case _WORD:
         tokens.add (newToken(word.toString(), status));
         word = null;
         status = TokenType._INIT;
         break;
      case _STRING:
         word.append (c);
         Return = false;
         break;
      case _ML_COMMENT:
         Return = false;
         break;
      }
      return Return;
   }

   public boolean tokenize (char line[], ArrayDeque<Token> t, FileInfo fi) {
      boolean Return = false;
      fname = fi.getName();
      cRow = fi.getLineNumber();

      int idx = 0;
      int ila = 0;

      tokens = t;
      word = null;
     
      for (idx = 0; idx < line.length; idx++) {
         ila = cCol = idx + 1;
         switch (line[idx]) {
         case ' ':
         case '\t':
         case '\r':
            breakChar(line[idx]);
            break;
         case '*':
            switch (status) {
            case _ML_COMMENT:
               if (++idx < line.length && line[idx] == '/')
                  status = TokenType._INIT;
               else
                  --idx;
               break;
            case _STRING:
               word.append (line[idx]);
               break;
            case _INIT:
            case _WORD:
            case _INTEGER:
            case _FLOAT:
               if (++idx < line.length && line[idx] == '>') {
                  idx = line.length;
                  break;
               } else {
                  --idx;
                  if (status == TokenType._WORD)
                     tokens.add (newToken(word.toString(),TokenType._WORD));
                  else if (status == TokenType._INTEGER)
                     tokens.add (newToken(word.toString(),TokenType._INTEGER));
                  else if (status == TokenType._FLOAT)
                     tokens.add (newToken(word.toString(),TokenType._FLOAT));
                  word = null;
                  status = TokenType._INIT;
                  tokens.add (newToken("multiply", TokenType._WORD, cCol));
               }
            }
            break;
         case '=':
            if (breakChar(line[idx]))
               tokens.add (newToken("equals", TokenType._WORD, cCol));
            break;
         case '<':
            if (breakChar(line[idx])) {
               ++idx;
               if (idx < line.length && line[idx] == '=') {
                  tokens.add (newToken("le", TokenType._WORD, cCol));
               } else if (idx < line.length && line[idx] == '>') {
                  tokens.add (newToken("ne", TokenType._WORD, cCol));
               } else if (idx + 1 < line.length && line[idx + 1] == '>' &&
                                    (line[idx] == '0' || line[idx] == '1')) {
                  if (line[idx] == '1')
                     tokens.add (newToken("<1>", TokenType._TRUE, cCol));
                  else
                     tokens.add (newToken("<0>", TokenType._FALSE, cCol));
                  idx++;
               } else {
                  --idx;
                  tokens.add (newToken("lt", TokenType._WORD, cCol));
               }
            }
            break;
         case '>':
            if (breakChar(line[idx])) {
               ++idx;
               if (idx < line.length && line[idx] == '=') {
                  tokens.add (newToken("ge", TokenType._WORD, cCol));
               } else {
                  --idx;
                  tokens.add (newToken("gt", TokenType._WORD, cCol));
               }
            }
            break;
         case ':':
            if (breakChar(line[idx])) {
               ++idx;
               if (idx < line.length && line[idx] == '=') {
                  tokens.add (newToken(":=", TokenType._ASSIGN, cCol));
               } else {
                  --idx;
                  tokens.add (newToken(":", TokenType._COLON_, cCol));
               }
            }
            break;
         case '-':
            if (breakChar(line[idx])) {
               ++idx;
               if (idx < line.length && line[idx] >= '0' && line[idx] <= '9') {
                  --idx;
                  status = TokenType._INTEGER;
                  word = new StringBuilder (Character.toString(line[idx]));
               } else {
                  --idx;
                  tokens.add (newToken("subtract", TokenType._WORD, cCol));
               }
            }
            break;
         case '+':
            if (breakChar(line[idx]))
               tokens.add (newToken("add", TokenType._WORD, cCol));
            break;
         case '/':
            if (breakChar(line[idx]))
               if (++idx < line.length && line[idx] == '*')
                  status = TokenType._ML_COMMENT;
               else {
                  idx--;
                  tokens.add (newToken("divide", TokenType._WORD, cCol));
               }
            break;
         case '%':
            if (breakChar(line[idx]))
               tokens.add (newToken("remainder", TokenType._WORD, cCol));
            break;
         case '(':
            if (breakChar(line[idx]))
               tokens.add (newToken("(", TokenType._PAR_OPEN_, cCol));
            break;
         case ')':
            if (breakChar(line[idx]))
               tokens.add (newToken(")", TokenType._PAR_CLOSE_, cCol));
            break;
         case '{':
            if (breakChar(line[idx]))
               tokens.add (newToken("{", TokenType._BRACE_OPEN_, cCol));
            break;
         case '}':
            if (breakChar(line[idx]))
               tokens.add (newToken("}", TokenType._BRACE_CLOSE_, cCol));
            break;
         case ',':
            if (breakChar(line[idx]))
               tokens.add (newToken(",", TokenType._COMMA_, cCol));
            break;
         case ';':
            if (breakChar(line[idx]))
               tokens.add (newToken(";", TokenType._SEMICOLON_, cCol));
            break;
         case '.':
            if (status == TokenType._INTEGER &&
                ila < line.length && line[ila] >= '0' && line[ila] <= '9') {
               word.append ('.');
               status = TokenType._FLOAT;
            } else {
               if (breakChar(line[idx]))
                  tokens.add (newToken(".", TokenType._DOT_, cCol));
            }
            break;
         case '!':
            if (breakChar(line[idx]))
               if (ila < line.length && line[ila] == '!') {
                  tokens.add (newToken("!!", TokenType._BANGBANG_, cCol));
                  idx++;
               } else
                  tokens.add (newToken("!", TokenType._BANG_, cCol));
            break;
         case '"':
            switch (status) {
            case _ML_COMMENT:
               break;
            case _INTEGER:
            case _FLOAT:
            case _WORD:
               tokens.add (newToken(word.toString(), status));
               // PASSTHRU
            case _INIT:
               status = TokenType._STRING;
               word = new StringBuilder();
               break;
            case _STRING:
               if (++idx <line.length && line[idx] == '"') {
                  word.append(line[idx]);
               } else {
                  --idx;
                  tokens.add (newToken(word.toString(), TokenType._STRING,
                                       cCol - word.length() - 1));
                  word = new StringBuilder();
                  status = TokenType._INIT;
               }
               break;
            }
            break;
         case '0':
         case '1':
         case '2':
         case '3':
         case '4':
         case '5':
         case '6':
         case '7':
         case '8':
         case '9':
            switch (status) {
            case _ML_COMMENT:
               break;
            case _INIT:
               status = TokenType._INTEGER;
               word = new StringBuilder (Character.toString(line[idx]));
               break;
            case _INTEGER:
            case _FLOAT:
            case _STRING:
            case _WORD:
               word.append (line[idx]);
               break;
            }
            break;
         case 'e':
         case 'E':
         case 'm':
         case 'M':
            if (status == TokenType._INTEGER || status == TokenType._FLOAT) {
               if (line[idx] == 'e' || line[idx] == 'E') {
                  if (ila < line.length && line[ila]>='0' && line[ila]<='9') {
                     word.append ('e');
                     status = TokenType._FLOAT;
                     break;
                  }
               } else {
                  status = TokenType._DECIMAL;
                  breakChar('\000');
                  break;
               }
            }
            // PASSTHRU
         default:
            switch (status) {
            case _ML_COMMENT:
               break;
            case _INIT:
               if (idx == 0 && line[0] == '#' && line.length > 1
                            && line[1] == '!') {
                  idx = line.length;
               } else {
                  word = new StringBuilder (Character.toString(line[idx]));
                  status = TokenType._WORD;
               }
               break;
            case _INTEGER:
            case _FLOAT:
               breakChar('\000');
               word = new StringBuilder (Character.toString(line[idx]));
               status = TokenType._WORD;
               break;
            case _STRING:
            case _WORD:
               word.append (line[idx]);
               break;
            }
            break;
         }
      }
      cCol++; // in case of errors
      switch (status) {
      case _ML_COMMENT:
      case _INIT:
         break;
      case _INTEGER:
      case _FLOAT:
      case _WORD:
         tokens.add (newToken(word.toString(), status));
         word = null;
         status = TokenType._INIT;
         break;
      case _STRING:
         tokens.add (newToken("\""+word.toString(),TokenType._UNCLOSED_STRING));
         status = TokenType._INIT;
         break;
      }
      
      return Return;
   }
}


