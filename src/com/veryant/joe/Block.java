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
   private String name;
   private String[] argName;

   Block (Executor exec, Block par) {
      executor = exec;
      parent = par;
      if (parent != null)
         constants = parent.constants;
      else
         constants = new HashMap<String,Object>();
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
   public Object sfExec (Object...argv) throws JOEException {
      if (variables == null)
         variables = new HashMap<String,Object>();
      return vExec (variables, argv);
   }
   
   public Object vExec (HashMap<String,Object> vars, Object...argv)
                                                          throws JOEException {
      HashMap<String,Object> saveVar = variables;
      variables = vars;

      if (argv != null && argName != null) {
         final int nArgs = Math.min (argv.length, argName.length);
         for (int i = 0; i < nArgs; i++)
            variables.put (argName[i], argv[i]);
      }
      Object Return = executor.run (this);
      variables = saveVar;
      return Return;
   }
   private HashMap<String,Object> getDataContaining (String name) {
      if (variables.containsKey (name))
         return variables;
      else if (parent != null)
         return parent.getDataContaining (name);
      else 
         return null;
   }
   public Object setConstant (String name, Object val) {
      if (constants.get (name) == null) {
         constants.put (name, val);
         return val;
      } else
         return null;
   }

   public Object setVariable (String name, Object val) {
      if (constants.get (name) == null) {
         HashMap<String,Object> var = getDataContaining (name);
         if (var != null)
            var.put (name, val);
         else
            variables.put (name, val);
         return val;
      } else
         return null;
   }
   public Object getVariable (WString name) {
      return getVariable (name.value);
   }
   public Object getVariable (String name) {
      Object Return = variables.get(name);
      if (Return == null && !variables.containsKey (name)) {
         Return = constants.get (name);
         if (Return == null && parent != null)
            Return = parent.getVariable (name);
      }
      return Return;
   }
   public HashMap<String,Object> getVariables () {
      return variables;
   }
   public Object execBlock (String name, Object...argv) throws JOEException {
      Object block = getVariable(name);
      if (block instanceof Block)
         return ((Block) block).exec (argv);
      else
         return null;
   }
   public String name() {
      return name;
   }
   void setName(String n) {
      name = n;
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
      return "{" + super.toString() + "}";
   }
}

