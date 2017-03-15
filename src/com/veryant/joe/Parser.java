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
import java.util.ArrayList;

public class Parser {
   static class TkStack {
      private final ArrayDeque<Token> tokens;
      private int lRow;
      private int lCol;

      TkStack(ArrayDeque<Token> tks) {
         tokens = tks;
      }
      Token pop() {
        Token Return;
         try {
            Return = tokens.removeFirst();
            lRow = Return.row;
            lCol = Return.col;
         } catch (java.util.NoSuchElementException _ex) {
            Return = new Token("(end stream)", TokenType._DOT_, lRow, lCol);
         }
         return Return;
      }
      void push(Token tk) {
         tokens.addFirst(tk);
      }
      Token peek() {
        Token Return;
         try {
            Return = tokens.getFirst();
         } catch (java.util.NoSuchElementException _ex) {
            Return = new Token("(end stream)", TokenType._DOT_, lRow, lCol);
         }
         return Return;
      }
      int size() {
         return tokens.size();
      }
      public String toString() {
         return tokens.toString();
      }
   }

   private Object command;
   final Block block;
   final String fName;

   public Parser (Object cmds, Executor exec, String fn) {
      this (cmds, exec, null, fn);
   }
   public Parser (Object cmds, Executor exec, Block par, String fn) {
      command = cmds;
      block = new Block (exec, par);
      fName = fn;
   }
   public Object getCommand() {
      return command;
   }

   public Block compile (ArrayDeque<Token> tks) throws JOEException {
      return compile (new TkStack (tks));
   }
   private void setBlockArguments (TkStack tokens) throws JOEException {
      Token tk;
      ArrayList<String> argv = new ArrayList<String>();
      while ((tk = tokens.pop()).type == TokenType._WORD) {
         argv.add (tk.word);
         if ((tk = tokens.pop()).type != TokenType._COMMA_)
            break;
      }
      if (tk.type != TokenType._DOT_)
         unexpectedToken (tk);
      String args[] = new String[argv.size()];
      block.setArguments (argv.toArray(args));
   }
   public Block compile (TkStack tokens) throws JOEException {
      Object msg;
      if (tokens.peek().type == TokenType._WORD) {
         final Token name = tokens.pop();
         if (tokens.peek().type == TokenType._COLON_) {
            tokens.pop();
            setBlockArguments (tokens);
            block.setName (name.word);
         } else if (tokens.peek().type == TokenType._DOT_) {
            block.setName (name.word);
            tokens.push(name);
         } else {
            tokens.push(name);
         }
      } else if (tokens.peek().type == TokenType._COLON_) {
         tokens.pop();
         setBlockArguments (tokens);
      }
      while (tokens.size()>0  && tokens.peek().type!=TokenType._BRACE_CLOSE_) {
         block.add (assignment (tokens));
      }
      return block;
   }

   private Message assgnDyMsg (final String name, final Object val,
                                      final int row, final int col) {
      Message Return;
      if (val instanceof Message) {
         final Message msg = (Message) val;
         Return = new Message() {
            public Object exec () throws JOEException {
               return block.setVariable(name, msg.exec());
            }
            public int getRow() {
               return row;
            }
            public int getCol() {
               return col;
            }
            public String toString () {
               return name + ":={"+ val + "}";
            }
         };
      } else {
         Return = new Message() {
            public Object exec () throws JOEException {
               return block.setVariable(name, val);
            }
            public int getRow() {
               return row;
            }
            public int getCol() {
               return col;
            }
            public String toString () {
               return name + ":=" + val;
            }
         };
      }
      return Return;
   }
   private Message assignment (TkStack tokens) throws JOEException {
      Message Return;
      Token tk = tokens.pop();
      if (tokens.peek().type== TokenType._ASSIGN) {
         tokens.pop();
         final Object val = message (tokens, tokens.pop());
         final int row = tk.row, col = tk.col;
         if (tk.type==TokenType._WORD) {
            final String name = tk.word;
            Return = assgnDyMsg (name, val, row, col);
         } else {
            Return = null;
            unexpectedToken ( tk);
         }
      } else {
         final Object val = message (tokens, tk);
         if (val instanceof Message) {
            Return = (Message) val;
         } else {
            final int row = tk.row, col = tk.col;
            Return = new Message() {
               public Object exec () throws JOEException {
                  return val;
               }
               public int getRow() {
                  return row;
               }
               public int getCol() {
                  return col;
               }
               public String toString () {
                  return val.toString();
               }
            };
         }
      }
      return Return;
   }

   private Object message(TkStack tokens,Token tk) throws JOEException {
      Object receiver;
      final String name = tk.word;

      if ((receiver = getValue(tk)) == tk) {
         switch (tk.type) {
         case _PAR_OPEN_:
            receiver = message (tokens, tokens.pop());
            parClose(tokens);
            break;
         case _BRACE_OPEN_:
            receiver = block (tk, tokens);
            braceClose(tokens);
            break;
         case _DOT_:
            return null;
         default:
            unexpectedToken ( tk);
         }
      }

      tk = tokens.pop();
      switch (tk.type) {
      case _WORD:
         return arguments (tokens, receiver, tk);
      case _PAR_CLOSE_:
      case _BRACE_CLOSE_:
         tokens.push (tk);
         //PASSTHRU
      case _SEMICOLON_:
      case _DOT_:
         return receiver;
      default:
         unexpectedToken ( tk);
         return null;
      }
   }

   private Block block (Token tk, TkStack tokens) throws JOEException {
      Parser parser = new Parser(command, block.executor, block, fName);
      Block Return = parser.compile (tokens);
      return Return;
   }

   private Message arguments (TkStack tokens, Object receiver,
                                 Token selector) throws JOEException {
      Message Return;
      Object obj;
      ArrayList<Object> args = new ArrayList<Object>();
      Token tk;

      exitFor: for ( ; ; ) {
         if ((obj = getValue(tk = tokens.pop())) == tk) {
            switch (tk.type) {
            case _DOT_:
               tokens.push(tk);
               // PASSTHRU
            case _SEMICOLON_:
               break exitFor;
            case _PAR_CLOSE_:
            case _BRACE_CLOSE_:
               tokens.push(tk);
               break exitFor;
            case _PAR_OPEN_:
               if (tokens.peek().type == TokenType._PAR_CLOSE_) {
                  tokens.pop();
                  break exitFor;
               } else {
                  args.add (message (tokens, tokens.pop()));
                  parClose (tokens);
               }
               break;
            case _BRACE_OPEN_:
               args.add (block (tk, tokens));
               braceClose (tokens);
               break;
            default:
               unexpectedToken (tk);
            }
         } else {
            args.add (obj);
         }
         if ((tk = tokens.pop()).type != TokenType._COMMA_) {
            tokens.push(tk);
            break exitFor;
         }
      }
      if (args.size() == 0) {
         Return = new ExecMessage (receiver, selector, null, fName);
      } else {
         Object argsAr[] = args.toArray();
         Return = new ExecMessage (receiver, selector, argsAr, fName);
      }
      switch ((tk = tokens.pop()).type) {
      case _WORD:
         Return = arguments (tokens, Return, tk);
         break;
      case _DOT_:
      case _SEMICOLON_:
         break;
      case _BRACE_CLOSE_:
      case _PAR_CLOSE_:
         tokens.push(tk);
         break;
      default:
         unexpectedToken ( tk);
      }
      return Return;
   }

   private Object getValue (final Token tk) {
      switch (tk.type) {
      case _BANG_:
         return command;
      case _STRING:
         return Literals.getString(tk.word);
      case _INTEGER:
         return Literals.getInteger(tk.word);
      case _FLOAT:
         return Literals.getDecimal(tk.word);
      case _WORD:
         return new SingleVariableMessage()  {
            public Object exec () {
               return block.getVariable(tk.word);
            }
            public int getRow() {
               return tk.row;
            }
            public int getCol() {
               return tk.col;
            }
            public String getName () {
               return tk.word;
            }
            public String toString () {
               return tk.word;
            }
         };
      default:
         return tk;
      }
   }
   private void parClose (TkStack tokens) throws JOEException {
      Token tk = tokens.pop();
      if (tk.type != TokenType._PAR_CLOSE_)
         throw new JOEException (
                     "Expected close parenthesis, found " + tk.word, tk, fName);
   }
   private void braceClose (TkStack tokens) throws JOEException {
      Token tk = tokens.pop();
      if (tk.type != TokenType._BRACE_CLOSE_)
         throw new JOEException (
                      "Expected close brace, found " + tk.word, tk, fName);
   }
   private void unexpectedToken (Token tk) throws JOEException {
      throw new JOEException ("Unexpected token `" + tk.word + "`", tk, fName);
   }
}
