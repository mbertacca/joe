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
import java.util.Arrays;
import java.util.HashMap;

public class Block extends ArrayList<Message>
                implements InternalObject {
   static final Object[] voidArgs = {};
   final Executor executor;
   private Block parent;
   private Block $super;
   private HashMap<String,Variable> varByName;
   private Object[] varValues;
   private ArrayList<Block> children = new ArrayList<Block>();
   private Object[] lastArgs;
   private String blockName;
   private boolean execAsJoe;

   Block (Executor exec, Block par) {
      executor = exec;
      parent = par;
      if (parent != null)
         parent.children.add (this);
   }
   public final Object exec () throws JOEException {
      return exec (null, voidArgs);
   }
   public final Object multiply () throws JOEException {
      return exec ();
   }
   public final Object exec (Object...argv) throws JOEException {
      return exec (null, argv);
   }
   public final Object multiply (Object...argv) throws JOEException {
      return exec (argv);
   }
   public final Object init () throws JOEException {
      return init (voidArgs);
   }
   public final Object init (Object...argv) throws JOEException {
      return exec (varValues, argv);
   }
   
   public final Object exec (Object vars[], Object...argv)
                                                          throws JOEException {
      Object Return;
      Object[] saveArgs = lastArgs;

      lastArgs = argv;
      if (varValues != null) {
         Object saveData[] = vars != null ? vars : varValues.clone();
         int i;
         final int nVars = varValues.length;
         final int nArgs = argv == null ? 0 : Math.min (argv.length, nVars);

         for (i = 0; i < nArgs; i++) {
            varValues[i] = argv[i];
         }
         for (     ; i < nVars; i++) {
            varValues[i] = WNull.value;
         }
         Return = executor.run (this);
         varValues = saveData;
      } else {
         Return = executor.run (this);
      }
      lastArgs = saveArgs;
      return Return;
   }
   private final Object whileTF (Block b, WBoolean tf, boolean checkBefore)
                                                        throws JOEException {
      Object Return = null;
      Object bol;
      Block cond;
      Block code;
      if (checkBefore) {
         cond = this;
         code = b;
         bol = cond.exec ();
      } else {
         cond = b;
         code = this;
         bol = tf;
      }
      for ( ; ; ) {
         if (bol.equals (tf)) {
            try {
               Return = code.exec ();
            } catch (BreakLoopException _ex) {
               if (_ex.hasReturnObject())
                  Return = _ex.getReturnObject();
               break;
            }
         } else {
            break;
         }
         bol = cond.exec ();
      }
      return Return;
   }
   public final Object whileTrue (Block b) throws JOEException {
      return whileTF (b, WBoolean.TRUE, true);
   }
   public final Object whileFalse (Block b) throws JOEException {
      return whileTF (b, WBoolean.FALSE, true);
   }
   public final Object doWhileTrue (Block b) throws JOEException {
      return whileTF (b, WBoolean.TRUE, false);
   }
   public final Object doWhileFalse (Block b) throws JOEException {
      return whileTF (b, WBoolean.FALSE, false);
   }

   public Object setVariable (WString name, Object val)throws JOEException {
      Variable var = getSetVariable (name.value);
      return setVariable (var, val);
   }
   public Object setVariable (Variable var, Object val) {
      Block block = this;
      int depth = var.getDepth();

      for ( ; depth != 0; depth--)
         block = block.parent;

      block.varValues[var.getIndex()] = val;

      return val;
   }

   public Object getVariable (WString name) {
      Variable var = lookForVariable (name.value);
      if (var == null) {
         return WNull.value;
      } else {
         return getVariable (var);
      }
   }

   public Object getVariable (Variable var) {
      Block block = this;
      int depth = var.getDepth();

      for ( ; depth != 0; depth--)
         block = block.parent;
      return block.varValues[var.getIndex()];
   }
   private void getVariablesNames(ArrayList<String> list) {
      if (varByName != null)
         for (String name : varByName.keySet())
            list.add (name);
      if (parent != null)
         parent. getVariablesNames(list);
   }
   public String[] getVariablesNames() {
      ArrayList<String> list = new ArrayList<String>();
      getVariablesNames(list);
      String Return[] = new String[list.size()];
      return list.toArray (Return);
   }
   public Object[] getArgv() {
      Object[] Return = lastArgs;
      if (Return == null) {
         Return = new Object[0];
      }
      return Return;
   }
   protected boolean isExecAsJoe () {
     return execAsJoe;
   }
   Block getMethod (String name) throws JOEException {
      Block Return;

      Variable var = varByName != null ? varByName.get(name) : null;
      if (var != null) {
         Object obj = varValues[var.getIndex()];
         if (obj instanceof Block && !((Block)obj).execAsJoe) {
            Return = (Block) obj;
         } else {
            Return = null;
         }
      } else {
         if ($super != null)
            Return = $super.getMethod (name);
         else
            Return = null;
      }
      return Return;
   }
   public Object clone() {
      Block Return = (Block) super.clone();
      Return.blockName = name();
      if (varValues != null)
         Return.varValues = varValues.clone();
      Return.children = new ArrayList<Block>();
      final int size = children.size();
      for (int i = 0; i < size; i++)
         Return.children.add(((Block)children.get(i).clone()).setParent(Return));
      return Return;
   }
   public Block $new() throws JOEException {
      return $new (voidArgs);
   }
   public Block add() throws JOEException {
      return $new ();
   }
   public Block $new(Object...args) throws JOEException {
      Block Return = (Block) clone();
      Return.execAsJoe = true;
      Return.init (args);
      return Return;
   }
   public Block add(Object...args) throws JOEException {
      return $new(args);
   }
   int getLastChild() {
      return children.size() - 1;
   }
   Block getChild(int n) {
      final Block Return = children.get(n);
      return Return;
   }
   public String name() {
      if (blockName == null)
         blockName = "block-" + System.identityHashCode(this);
      return blockName;
   }
   void setName(String n) {
      blockName = n;
   }
   public int getRow() {
      return -1;
   }
   public int getCol() {
      return -1;
   }
   public Block setParent(Block b) {
      parent = b;
      return this;
   }
   public Block $extends(Block b) {
      $super = b;
      return this;
   }
   public Object getJoeClass () {
      return new Object () {
         public String getName() {
            return name();
         }
      };
   }
   public String toString() {
      String Return;

      if (isExecAsJoe()) {
         Block joeToString;
         try {
            joeToString = getMethod("toString");
            if (joeToString  != null)
               try {
                  Return = joeToString.exec().toString();
               } catch (NullPointerException _ex) {
                  Return = "()";
               } catch (JOEException _ex) {
                  Return = _ex.toString();
               }
            else
               Return = "{" + super.toString() + "}";
         } catch (JOEException _ex) {
            Return = "{" + super.toString() + "}";
         }
      } else {
         if ((Return = blockName) == null)
            Return = "{" + name() + "}";
      }
      return Return;
   }
   public Variable lookForVariable (String name, int depth[]) {
      Variable Return = varByName != null ? varByName.get(name) : null;
      if (Return == null && parent != null) {
         depth[0]++;
         Return = parent.lookForVariable (name, depth);
      }
      return Return;
   }
   public Variable lookForVariable (String name) {
      Variable Return;
      int depth[] =  { 0 };
      Variable var = lookForVariable (name, depth);
      if (var != null) {
         if (depth[0] == 0) {
            Return = var;
         } else {
            Return = new Variable (var, depth[0] + var.getDepth());
         }
      } else {
         Return = null;
      }
      return Return;
   }
   public Variable getSetLocalVariable (String name) {
      Variable Return;
      final int len;
      if (varByName == null) {
         len = 0;
         varByName = new HashMap<String,Variable>();
         varValues = new Object[]  { WNull.value };
      } else {
         len = varValues.length;
         varValues = Arrays.copyOf (varValues, len + 1);
         varValues[len] = WNull.value;
      }
      Return = new Variable (name, 0, len);
      varByName.put (name, Return);
      return Return;
   }
   public Variable getSetVariable (String name) {
      Variable Return = lookForVariable (name);
      if (Return == null)
         Return = getSetLocalVariable (name);
      return Return;
   }
   void setLocalVariables (ArrayList<String> names) {
      String name;
      final int len = names.size();
      varByName = new HashMap<String,Variable>();
      varValues = new Object[len];
      for (int i = 0; i < len; i++) {
         name = names.get(i);
         varByName.put (name, new Variable (name, 0, i));
         varValues[i] = WNull.value;
      }
   }
}
