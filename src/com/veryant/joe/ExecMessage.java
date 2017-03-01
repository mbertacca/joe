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

import java.lang.reflect.Array;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.lang.reflect.Modifier;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;

public class ExecMessage implements Message {
   private static class MethodWArgs {
      final Method method;
      final Object[] args;
      MethodWArgs (Method m, Object[] a) {
         method = m;
         args = a;
      }
   };
   static final HashMap<Class,Class> primitiveClasses;
   private static HashSet<String> javaKeywords = new HashSet<String> ();

   private Object object;
   private Message receiver;
   private final String selector;
   private Class clazz;
   private final Object origArgs[];
   private final int row, col;

   static {
      primitiveClasses = new HashMap<Class,Class>();
      primitiveClasses.put (boolean.class, Boolean.class);
      primitiveClasses.put (byte.class, Byte.class);
      primitiveClasses.put (char.class, Character.class);
      primitiveClasses.put (short.class, Short.class);
      primitiveClasses.put (int.class, Integer.class);
      primitiveClasses.put (long.class, Long.class);
      primitiveClasses.put (float.class, Float.class);
      primitiveClasses.put (double.class, Double.class);

      javaKeywords.add ("abstract");
      javaKeywords.add ("assert");
      javaKeywords.add ("boolean");
      javaKeywords.add ("break");
      javaKeywords.add ("byte");
      javaKeywords.add ("case");
      javaKeywords.add ("catch");
      javaKeywords.add ("char");
      javaKeywords.add ("class");
      javaKeywords.add ("const");
      javaKeywords.add ("continue");
      javaKeywords.add ("default");
      javaKeywords.add ("do");
      javaKeywords.add ("double");
      javaKeywords.add ("else");
      javaKeywords.add ("enum");
      javaKeywords.add ("extends");
      javaKeywords.add ("final");
      javaKeywords.add ("finally");
      javaKeywords.add ("float");
      javaKeywords.add ("for");
      javaKeywords.add ("goto");
      javaKeywords.add ("if");
      javaKeywords.add ("implements");
      javaKeywords.add ("import");
      javaKeywords.add ("instanceof");
      javaKeywords.add ("int");
      javaKeywords.add ("interface");
      javaKeywords.add ("long");
      javaKeywords.add ("native");
      javaKeywords.add ("new");
      javaKeywords.add ("package");
      javaKeywords.add ("private");
      javaKeywords.add ("protected");
      javaKeywords.add ("public");
      javaKeywords.add ("return");
      javaKeywords.add ("short");
      javaKeywords.add ("static");
      javaKeywords.add ("strictfp");
      javaKeywords.add ("super");
      javaKeywords.add ("switch");
      javaKeywords.add ("synchronized");
      javaKeywords.add ("this");
      javaKeywords.add ("throw");
      javaKeywords.add ("throws");
      javaKeywords.add ("transient");
      javaKeywords.add ("try");
      javaKeywords.add ("void");
      javaKeywords.add ("volatile");
      javaKeywords.add ("while");
   }

   public static String getSelector(String s) {
      if (javaKeywords.contains (s))
         return "$" + s;
      else
         return s;
   }

   ExecMessage (Object obj, Token sel, Object args[])
                                              throws JOEException {
      selector = getSelector (sel.word);
      row = sel.row;
      col = sel.col;
      origArgs = args;
      if (obj instanceof Message) {
         receiver = (Message) obj;
      } else {
         object = obj;
      }
   }

   public int getRow() {
      return row;
   }
   public int getCol() {
      return col;
   }

   MethodWArgs check (Object actObj) throws JOEException {
      MethodWArgs Return;
      Object[] argArray = null;
      if (actObj instanceof ClassReference)
         clazz = ((ClassReference) actObj).get();
      else
         clazz = actObj.getClass();
      if (origArgs != null) {
         argArray = new Object[origArgs.length];
         for (int i = 0; i < origArgs.length; i++)
            if (origArgs[i] instanceof Message)
               argArray[i] = ((Message) origArgs[i]).exec();
            else
               argArray[i] = origArgs[i];
         if (! (actObj instanceof InternalObject))
            for (int i = 0; i < argArray.length; i++)
               if (argArray[i] instanceof Wrapper)
                  argArray[i] = ((Wrapper) argArray[i]).getWrapped();
         Class argClasses[] = new Class[argArray.length];
         for (int i = 0; i < argArray.length; i++)
            if (argArray[i] == null)
               argClasses[i] = Object.class;
            else
               argClasses[i] = argArray[i].getClass();
         try {
            Return = new MethodWArgs(clazz.getMethod (selector, argClasses),
                                     argArray);
         } catch (NoSuchMethodException ex) {
            Return = getMethod(selector, argClasses, argArray, ex);
         }
      }  else {
         try {
            Return = new MethodWArgs (clazz.getMethod (selector), argArray);
         } catch (NoSuchMethodException ex) {
            throw new JOEException (ex, row, col);
         }
      }
      return Return;
   }

   private int paramFits (Class methType, Class currParam, int currFit) {
      if (currParam != null) {
         if (methType.equals (currParam)) {
            currFit += 2;
         } else if (methType.isAssignableFrom (currParam)) {
            currFit += 1;
         } else if (methType.isPrimitive () &&
                    currParam.equals(primitiveClasses.get(methType))) {
            currFit += 2;
         } else {
            currFit = -1;
         }
      }
      return currFit;
   }

   private MethodWArgs getMethod(String selector, Class argClasses[],
           Object[] argArray, NoSuchMethodException ex) throws JOEException {
      Method method = null;
      MethodWArgs Return;
      final Method m[] = clazz.getMethods();
      int bestFit = -1;
      boolean bVarargs = false;
      Class types[], bTypes[] = null; 

      for (int i = 0; i < m.length; i++) {
         if (m[i].getName().equals(selector)) {
            types = m[i].getParameterTypes();
            int currFit = 0;
            boolean varargs = false;
            if (types.length > 0 && types.length <= argClasses.length) {
               final int fixed = types.length -1;
               for (int j = 0; j < fixed && currFit >= 0; j++) {
                  currFit = paramFits (types[j], argClasses[j], currFit);
               }
               if (currFit >= 0 && argClasses[fixed] != null) {
                  boolean allFixed = false;
                  if (types.length == argClasses.length) {
                     int fits = 0;
                     fits = paramFits(types[fixed],argClasses[fixed], fits);
                     if (fits >= 0) {
                        currFit += fits;
                        allFixed = true;
                     }
                  }
                  if (!allFixed && types[fixed].isArray()) {
                     allFixed = true;
                     varargs = true;
                     Class arCls = types[fixed].getComponentType();
                     final int nArgLen = argClasses.length - fixed;
                     int k;
                     for (k = 0; k < nArgLen; k++) {
                        if (argClasses[fixed+k] == null)
                           break;
                        else if (!arCls.isAssignableFrom (argClasses[fixed+k]))
                           break;
                     }
                     if (k == nArgLen)
                        currFit += 1;
                      else 
                        currFit = -1;
                  }
                  if (allFixed && currFit > bestFit) {
                     method = m[i];
                     bTypes = types;
                     bestFit = currFit;
                     bVarargs = varargs;
                  }
               }
            }
         }
      }
      if (method == null)
         throw new JOEException (ex, row, col);
      if (bVarargs) {
         Object newArgs[] = new Object[bTypes.length];
         Class arCls = bTypes[bTypes.length - 1].getComponentType();
         int i;
         for (i = 0; i < bTypes.length - 1; i++)
            newArgs[i] = argArray[i];
         Object lastArg = Array.newInstance (arCls, argArray.length - i);
         newArgs[i] = lastArg;
         for (int j = 0 ; i < argArray.length; i++, j++) {
            Array.set (lastArg, j, argArray[i]);
         }
         argArray = newArgs;
      }

      return new MethodWArgs (method, argArray);
   }

   public Object exec () throws JOEException {
      Object wobj;
      Object actObj;
      Object Return;
      try {
         if (receiver != null) {
            actObj = receiver.exec();
         } else {
            actObj = object;
         }
         if (actObj == null)
            throw new JOEException ("null receiver", row, col);
         MethodWArgs mwa = check(actObj);
         Return = mwa.method.invoke(actObj, mwa.args);
         if ((wobj = Wrapper.newInstance (Return)) != null)
            Return = wobj;
      } catch (IllegalAccessException ex) {
         throw new JOEException (ex, row, col);
      } catch (InvocationTargetException ex) {
         Throwable ilex = ex.getCause();
         if (ilex instanceof JOEException)
            throw (JOEException) ilex;
         else
            throw new JOEException (ilex, row, col);
      } catch (IllegalArgumentException ex) {
         throw new JOEException (ex, row, col);
      }
      return Return;
   }
   public String toString() {
      StringBuilder Return = new StringBuilder("<");
      Return.append (row);
      Return.append (",");
      Return.append (col);
      Return.append ("> ");

      if (object != null)
         Return.append(object.getClass().getName());
      else if (receiver != null)
         Return.append(receiver.toString());
      else 
         Return.append("(null)");

      Return.append (" ");
      Return.append (selector);

      return Return.toString();
   }
   public static void main (String argv[]) throws Exception {
      WLong a = new WLong ("7");
      WLong b = new WLong ("5");
      CommandBase c = new CommandBase();
      ExecMessage x, y;
      x = new ExecMessage (a, new Token("add",TokenType._WORD,0,0),
                                   new Object[] { b } );
      y = new ExecMessage (c, new Token("display",TokenType._WORD,0,0),
                                   new Object[] { "Hello ", x , " World!"} );
      y.exec();
   }
}