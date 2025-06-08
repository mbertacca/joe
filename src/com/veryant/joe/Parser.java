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

      TkStack(ArrayDeque<Token> tks) {
         tokens = tks;
      }
      Token pop() {
        Token Return;
         try {
            Return = tokens.removeFirst();
         } catch (java.util.NoSuchElementException _ex) {
            Return = new Token("(end stream)", TokenType._DOT_, 0, 0, "");
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
            Return = new Token("(end stream)", TokenType._DOT_, 0, 0, "");
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
   final FileInfo info;

   public Parser (Object cmds, Block blk, FileInfo fi) {
      command = cmds;
      block = blk;
      info = fi;
      block.setName (info.getName());
   }
   public Parser (Object cmds, Executor exec, FileInfo fi) {
      command = cmds;
      block = new OuterBlock (exec);
      info = fi;
      block.setName (info.getName());
   }
   private Parser (Parser parent) {
      command = parent.command;
      block = new Block (parent.block.executor, parent.block);
      info = parent.info;
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
      block.setLocalVariables (argv);
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

   private Message assgnDyMsg (final Variable var, final Object val,
                                      final int row, final int col)
                                               throws JOEException  {
      Message Return;
      if (val instanceof Message) {
         final Message msg = (Message) val;
         Return = new Message() {
            public Object exec (Block blk) throws JOEException {
               return blk.setVariable(var, msg.exec(blk));
            }
            public int getRow() {
               return row;
            }
            public int getCol() {
               return col;
            }
            public String getFileName() {
               return info.getName();
            }
            public String toString () {
               return var.getName() + ":={"+ val + "}";
            }
         };
      } else {
         Return = new Message() {
            public Object exec (Block blk) throws JOEException {
               return blk.setVariable(var, val);
            }
            public int getRow() {
               return row;
            }
            public int getCol() {
               return col;
            }
            public String getFileName() {
               return info.getName();
            }
            public String toString () {
               return var.getName() + ":=" + val;
            }
         };
      }
      return Return;
   }
   private Message assignment (TkStack tokens) throws JOEException {
      Message Return;
      Token tk = tokens.pop();
      Token peek = tokens.peek();
      if (peek.type == TokenType._ASSIGN || peek.type == TokenType._CONSTANT) {
         tokens.pop();
         if (tk.type==TokenType._WORD) {
            final Variable var = block.getSetVariable (tk.word);
            if (peek.type == TokenType._CONSTANT) {
               if (var.canBeConst()) {
                  if (tokens.peek().type == TokenType._DOT_)
                     var.makeConstUnassigned ();
                  else
                     var.makeConstAssigned ();
               } else {
                  throw new JOEException (
                     "Variable already declared: `" + tk.word + "`", tk);
               }
            } else {
               if (var.isConstant()) {
                  throw new JOEException (
                     "Cannot change a constant: `" + tk.word + "`", tk);
               } else {
                  var.makeVarAssigned ();
               }
            }
            final Object val = message (tokens, tokens.pop());
            Return = assgnDyMsg (var, val, tk.row, tk.col);
         } else {
            Return = null;
            unexpectedToken ( tk);
         }
      } else if (peek.type == TokenType._DOT_ &&
                   tk.type == TokenType._STRING) {
         tokens.pop();
         final int row = tk.row, col = tk.col;
         final Object val = Literals.getString(tk.word);
         Return = new SingleStringMessage() {
            public Object exec (Block blk) throws JOEException {
               return val;
            }
            public int getRow() {
               return row;
            }
            public int getCol() {
               return col;
            }
            public String getFileName() {
               return info.getName();
            }
            public String getName () {
               return val.toString();
            }
            public String toString () {
               return val.toString();
            }
         };
      } else {
         final Object val = message (tokens, tk);
         if (val instanceof Message) {
            Return = (Message) val;
         } else {
            final int row = tk.row, col = tk.col;
            Return = new Message() {
               public Object exec (Block blk) throws JOEException {
                  return val;
               }
               public int getRow() {
                  return row;
               }
               public int getCol() {
                  return col;
               }
               public String getFileName() {
                  return info.getName();
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
            return Literals.getNull();
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

   private Message block (final Token tk, TkStack tokens) throws JOEException {
      Parser parser = new Parser(this);
      final Block newBlock = parser.compile (tokens);
      final int blkIdx = block.getLastChild();
      Message Return = new Message() {
         public Object exec (Block blk) throws JOEException {
            return blk.getChild(blkIdx);
         }
         public int getRow() {
            return tk.row;
         }
         public int getCol() {
            return tk.col;
         }
         public String getFileName() {
            return info.getName();
         }
         public String toString () {
            return newBlock.toString();
         }
      };
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
         Return = new ExecMessage (receiver, selector, null);
      } else {
         Object argsAr[] = args.toArray();
         Return = new ExecMessage (receiver, selector, argsAr);
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

   private Object getValue (final Token tk) throws JOEException {
      final Variable var;
      try {
         switch (tk.type) {
         case _BANG_:
            return command;
         case _BANGBANG_:
            return new Message ()  {
               public Object exec (Block blk) {
                  return blk;
               }
               public int getRow() {
                  return tk.row;
               }
               public int getCol() {
                  return tk.col;
               }
               public String getFileName() {
                  return info.getName();
               }
               public String getName () {
                  return tk.word;
               }
               public String toString () {
                  return tk.word;
               }
            };
         case _STRING:
            return Literals.getString(tk.word);
         case _INTEGER:
            return Literals.getInteger(tk.word);
         case _LONG:
            return Literals.getLong(tk.word);
         case _FLOAT:
            return Literals.getDouble(tk.word);
         case _DECIMAL:
            return Literals.getDecimal(tk.word);
         case _TRUE:
            return Literals.getBoolean(true);
         case _FALSE:
            return Literals.getBoolean(false);
         case _NULL:
            return Literals.getNull();
         case _WORD:
            var = block.lookForVariable (tk.word);
            if (var == null)
               throw new JOEException (
                         "Variable not found: `" + tk.word+ "`", tk);
            return new Message()  {
               public Object exec (Block blk) throws JOEException {
                  return blk.getVariable(var);
               }
               public int getRow() {
                  return tk.row;
               }
               public int getCol() {
                  return tk.col;
               }
               public String getFileName() {
                  return info.getName();
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
      } catch (Exception ex) {
         throw new JOEException (ex.toString(), tk);
      }
   }
   private void parClose (TkStack tokens) throws JOEException {
      Token tk = tokens.pop();
      if (tk.type != TokenType._PAR_CLOSE_)
         throw new JOEException (
           "Expected close parenthesis, found " + tk.word, tk);
   }
   private void braceClose (TkStack tokens) throws JOEException {
      Token tk = tokens.pop();
      if (tk.type != TokenType._BRACE_CLOSE_)
         throw new JOEException (
               "Expected close brace, found " + tk.word, tk);
   }
   private void unexpectedToken (Token tk) throws JOEException {
      throw new JOEException ("Unexpected token " + tk.type +
                              " `" + tk.word + "`", tk);
   }
}
