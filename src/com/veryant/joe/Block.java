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
import java.util.HashMap;

public class Block extends ArrayList<Message>
                implements InternalObject {
   static final Object[] voidArgs = {};
   final Executor executor;
   private Block parent;
   private Block $super;
   private HashMap<String,Variable> varByName;
   private ArrayList<Object> varValues;
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
   public Object exec () throws JOEException {
      return vExec (null, voidArgs);
   }
   public Object multiply () throws JOEException {
      return exec ();
   }
   public Object exec (Object...argv) throws JOEException {
      return vExec (null, argv);
   }
   public Object multiply (Object...argv) throws JOEException {
      return exec (argv);
   }
   public Object init () throws JOEException {
      return init (voidArgs);
   }
   public Object init (Object...argv) throws JOEException {
      return vExec (varValues, argv);
   }
   
   public Object vExec (ArrayList<Object> vars, Object...argv)
                                                          throws JOEException {
      Object Return;
      lastArgs = argv;
      if (varValues != null) {
         ArrayList<Object> saveData = vars != null ?
                                  vars : (ArrayList<Object>) varValues.clone();
         int i;
         final int nVars = varValues.size();
         final int nArgs = argv == null ? 0 : Math.min (argv.length, nVars);

         for (i = 0; i < nArgs; i++) {
            varValues.set (i, argv[i]);
         }
         for (     ; i < nVars; i++) {
            varValues.set (i, WNull.value);
         }
         Return = executor.run (this);
         varValues = saveData;
      } else {
         Return = executor.run (this);
      }
      return Return;
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

      block.varValues.set(var.getIndex(), val);

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
      return block.varValues.get(var.getIndex());
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
      Variable var = lookForVariable (name);
      if (var != null) {
         Object obj = getVariable (var);
         if (obj instanceof Block) {
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
         Return.varValues = (ArrayList<Object>)varValues.clone();
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
            Return = new Variable (name, depth[0] + var.getDepth(),
                                         var.getIndex());
            Return.setConstant (var.isConstant());
         }
      } else {
         Return = null;
      }
      return Return;
   }
   public Variable getSetLocalVariable (String name) {
      Variable Return;
      if (varByName == null) {
         varByName = new HashMap<String,Variable>();
         varValues = new ArrayList<Object>();
      }
      Return = new Variable (name, 0, varValues.size());
      varByName.put (name, Return);
      varValues.add (WNull.value);
      return Return;
   }
   public Variable getSetVariable (String name) {
      Variable Return = lookForVariable (name);
      if (Return == null)
         Return = getSetLocalVariable (name);
      return Return;
   }
   void setLocalVariables (ArrayList<String> names) {
      varByName = new HashMap<String,Variable>();
      varValues = new ArrayList<Object>();
      for (String name : names) {
         varByName.put (name, new Variable (name, 0, varValues.size()));
         varValues.add (WNull.value);
      }
   }
}
