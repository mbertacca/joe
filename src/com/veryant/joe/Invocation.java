/*
 *
 * This source file is part of the "Java Objects Executor" open source project
 *
 * Copyright 2026 Marco Bertacca
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

/**
 * Represents a reified message invocation within JOE.
 * <p>
 * Encapsulates an unhandled message call (when the target
 * object does not possess the requested method), containing
 * its selector, arguments, and execution block. Enables
 * dynamic inspection, forwarding, or re-execution via
 * fallback mechanisms such as {@code doesNotUnderstand}.
 * </p>
 *
 * @author Marco
 * @see JOEException
 */

public class Invocation {
   final Block block;
   final Token selector;
   final Object args[];

   Invocation (Block block, Token selector, Object args[]) {
      this.block = block;
      this.selector = selector;
      this.args = args;
   }
/*
 * Constructs an {@code Invocation} using a string selector.
 *
 * @param block   the {@link Block} execution context.
 * @param selname the selector name as a string.
 * @param args    the array of arguments.
 */
   Invocation (Block block, String selname, Object args[]) {
      this (block,new Token (selname, TokenType._WORD, 0, 0, ""),args);
   }
/*
 * Constructs an {@code Invocation} using a string selector.
 *
 * @param block   the {@link Block} execution context.
 * @param selname the selector name as a string.
 */
   Invocation (Block block, String selname) {
      this (block,new Token (selname, TokenType._WORD, 0, 0, ""), null);
   }

/**
  * Returns the name of the message selector.
  *
  * @return the String representing the invoked method or message name.
  */
   public String selector() {
      return selector.word;
   }
/**
 * Returns the arguments associated with this invocation.
 *
 * @return an array of objects containing the message arguments.
 */
   public Object[] args() {
      return args;
   }
/**
 * Forwards and re-sends the message to a target object.
 * <p>
 * The target object is wrapped using {@link Wrapper} if
 * necessary before executing the message within the original
 * block context.
 * </p>
 *
 * @param obj the recipient target object.
 * @return the result of executing the message.
 * @throws JOEException if execution of the message fails.
 */
   public Object sendTo (Object obj) throws JOEException {
      Object wrpd = Wrapper.newInstance (obj);
      ExecMessage msg = new ExecMessage(wrpd==null ? obj : wrpd,selector,args);
      return msg.exec(block);
   }
}
