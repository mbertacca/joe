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
   final Executor executor;
   private Block parent;
   private HashMap<String,Object> variables;
   private final HashMap<String,Object> constants;
   private ArrayList<Block> children = new ArrayList<Block>();
   private String blockName;
   private String[] argName;
   private Object argArray[];
   private boolean execAsJoe;

   Block (Executor exec, Block par) {
      executor = exec;
      parent = par;
      if (parent != null)
         constants = parent.constants;
      else
         constants = new HashMap<String,Object>();
      if (parent != null)
         parent.children.add (this);
   }
   void setArguments (String argn[]) {
      argName = argn;
   }
   public Object exec () throws JOEException {
      return vExec (new HashMap<String,Object>(), (Object[]) null);
   }
   public Object exec (Object...argv) throws JOEException {
      return vExec (new HashMap<String,Object>(), argv);
   }
   public Object init () throws JOEException {
      return init ((Object[]) null);
   }
   public Object init (Object...argv) throws JOEException {
      if (variables == null)
         variables = new HashMap<String,Object>();
      return vExec (variables, argv);
   }
   @Deprecated
   public Object sfExec (Object...argv) throws JOEException {
      if (variables == null)
         variables = new HashMap<String,Object>();
      return vExec (variables, argv);
   }
   
   public Object vExec (HashMap<String,Object> vars, Object...argv)
                                                          throws JOEException {
      HashMap<String,Object> saveVar = variables;
      variables = vars;

      if (argv != null) {
         argArray = argv;
         if (argName != null) {
            final int nArgs = Math.min (argv.length, argName.length);
            for (int i = 0; i < nArgs; i++)
               variables.put (argName[i], argv[i]);
         }
      } else {
         argArray = new Object[] {};
      }
      Object Return = executor.run (this);
      variables = saveVar;
      return Return;
   }
   private HashMap<String,Object> getDataContaining (String name) {
      if (variables != null && variables.containsKey (name))
         return variables;
      else if (parent != null)
         return parent.getDataContaining (name);
      else 
         return null;
   }
   public Object setConstant (String name, Object val) {
      if (val == null)
         val = WNull.value;
      if (constants.get (name) == null) {
         constants.put (name, val);
         return val;
      } else
         return null;
   }

   public Object setVariable (String name, Object val) {
      if (val == null)
         val = WNull.value;
      if (constants.get (name) == null) {
         if (execAsJoe) {
            variables.put (name, val);
         } else {
            HashMap<String,Object> var = getDataContaining (name);
            if (var != null)
               var.put (name, val);
            else
               variables.put (name, val);
         }
         return val;
      } else
         return null;
   }
   public Object getVariable (WString name) throws JOEException {
      return getVariable (name.value);
   }
   public Object getVariable (String name) throws JOEException {
      Object Return = variables.get(name);
      if (Return == null && !variables.containsKey (name)) {
         Return = constants.get (name);
         if (Return == null) {
            if (parent != null)
               Return = parent.getVariable (name);
            else
               throw new JOEException ("Variable not found: `" + name + "`");
         }
      }
      return Return;
   }
   private void getVariablesNames(ArrayList<String> list) {
      for (String entry : variables.keySet())
         list.add (entry);
      if (parent != null)
         parent. getVariablesNames(list);
   }
   public String[] getVariablesNames() {
      ArrayList<String> list = new ArrayList<String>();
      getVariablesNames(list);
      String Return[] = new String[list.size()];
      return list.toArray (Return);
   }
   public HashMap<String,Object> getVariables () {
      return variables;
   }
   public Object[] getArgv() {
      return argArray;
   }
   protected boolean isExecAsJoe () {
     return execAsJoe;
   }
   Block getMethod (String name) throws JOEException {
      Object Return = getVariable (name);
      if (Return instanceof Block) {
          return (Block) Return;
      } else {
          return null;
      }
   }
   public Object execBlock (String name, Object...argv) throws JOEException {
      Object block = getVariable(name);
      if (block instanceof Block)
         return ((Block) block).exec (argv);
      else
         return null;
   }
   public Object clone() {
      Block Return = (Block) super.clone();
      Return.variables = null;
      Return.children = new ArrayList<Block>();
      final int size = children.size();
      for (int i = 0; i < size; i++)
         Return.children.add(((Block)children.get(i).clone()).$extends(Return));
      return Return;
   }
   public Block $new() throws JOEException {
      return $new ((Object[]) null);
   }
   public Block $new(Object...args) throws JOEException {
      Block Return = (Block) clone();
      Return.execAsJoe = true;
      Return.init (args);
      return Return;
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
         return "block-" + hashCode();
      else
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
   public Block $extends(Block b) {
      parent = b;
      return this;
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
}
