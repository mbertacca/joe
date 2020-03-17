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

import java.io.Console;
import java.util.HashMap;
import java.lang.reflect.Array;
import java.lang.reflect.Constructor;
import java.lang.reflect.Field;
import java.net.MalformedURLException;
import java.net.URL;

/**
 * This class contains some methods that are useful for any flavor of
 * JOE implementation.
 */

public class CommandBase {
   static HashMap<Class,Class> primitiveClasses;
   static {
      primitiveClasses = new HashMap<Class,Class>();
      primitiveClasses.put (Boolean.class, boolean.class);
      primitiveClasses.put (Byte.class, byte.class);
      primitiveClasses.put (Character.class, char.class);
      primitiveClasses.put (Short.class, short.class);
      primitiveClasses.put (Integer.class, int.class);
      primitiveClasses.put (Long.class, long.class);
      primitiveClasses.put (Float.class, float.class);
      primitiveClasses.put (Double.class, double.class);
   }
   static final String eol = System.getProperty("line.separator", "\n");
   /**
    * Returns a new instance of the specified class.
    */
   public Object newInstance (Class clazz) throws Exception {
      return clazz.newInstance();
   }
   /**
    * This is a convenience method for newInstance (getClass (className)).
    */
   public Object newInstance (String className) throws Exception {
      return newInstance(getClass (className));
   }
   /**
    * Returns a new instance of the specified class passing the arguments
    * args to its constructor.
    */
   public Object newInstance (Class clazz, Object... args) throws Exception {
      Class objType[] = new Class[args.length];
      Object obj;
      for (int i = 0; i < args.length; i++)
         if (args[i] != null)
            objType[i] = args[i].getClass();
         else
            objType[i] = null;
      Constructor ctor;
      try {
         ctor = clazz.getConstructor (objType);
      } catch (NoSuchMethodException _ex) {
         Class c;
         for (int i = 0; i < objType.length; i++)
            if ((c = primitiveClasses.get(objType[i])) != null)
               objType[i] = c;
         try {
            ctor = clazz.getConstructor (objType);
         } catch (NoSuchMethodException ignore) {
            return clazz.getConstructor (new Class[] { args.getClass() } )
                        .newInstance (new Object[]{args});
         }
      }
      return ctor.newInstance (args);
   }
   /**
    * This is a convenience method for newInstance (getClass (clsName), args).
    */
   public Object newInstance (String clsName, Object... args) throws Exception {
      return newInstance (getClass (clsName), args);
   }
   /**
    * Returns an Object array whose length is n.
    */
   public Object[] newArray (int n) throws Exception {
      return new Object[n];
   }
   /**
    * Returns an Object array of the specified class whose length is n.
    */
   public Object newArray (Class clazz, int n) throws Exception {
      return Array.newInstance (clazz, n);
   }
   /**
    * Returns an Object array of the specified class whose length is n.
    */
   public Object newArray (String clazz, int n) throws Exception {
      return newArray(getClass(clazz), n);
   }
   /**
    * Returns an Object array whose items are the parameters
    */
   public Object[] array (Object...items) throws Exception {
      Object[] Return = new Object[items.length];
      for (int i = 0; i < items.length; i++)
         Return[i] = items[i];
      return Return;
   }
   /**
    * Returns a field whose name is name of the object obj.
    */
   public Object getField(Object obj,String name) throws Exception {
      Field fld = obj.getClass().getDeclaredField(name);
      return fld.get(obj);
   }
   /**
    * Returns a static field whose name is name of the class clazz.
    */
   public Object getStaticField(String clazz,String name) throws Exception {
      Class c = Class.forName (clazz);
      Field fld = c.getDeclaredField(name);
      return fld.get(null);
   }
   /**
    * Returns the Class instance of clazz.
    */
   public Class getClass (String clazz) throws Exception {
      return Class.forName (clazz);
   }
   /**
    * Returns the ClassReference instance of clazz. The ClassReference
    * object can be used to invoke a static method  of a class, e.g.
    * the ClassReference of java.lang.System can be used to invoke
    * System.getProperty(String prop).
    */
   public ClassReference getClassRef (String clazz) throws Exception {
      return new ClassReference (Class.forName (clazz));
   }
   /**
    * Invoke System.exit (ec).
    */
   public void systemExit (int ec) {
      System.exit (ec);
   }
   /**
    * Invoke System.exit (0).
    */
   public void systemExit () {
      systemExit (0);
   }
   /**
    * Throws an Exception whose message is the argument.
    */
   public void $throw (String msg) throws Exception {
      throw new Exception (msg);
   }
   /**
    * Throws a throwable passed as argument.
    */
   public void $throw (Throwable ex) throws Throwable {
      throw ex;
   }
   /**
    * Returns true if the passed object is null, false otherwise.
    */
   public boolean isNull (Object obj) {
      return obj == null;
   }
   /**
    * Returns true if the passed objects are the same object, false otherwise.
    */
   public boolean areSameObject (Object o1, Object o2) {
      return o1 == o2;
   }
   /**
    * Returns the result of obj instanceof Throwable
    */
   public boolean isThrowable (Object obj) {
      return obj instanceof Throwable;
   }
   /**
    * Similar to the java operator instanceof.
    */
   public boolean $instanceof (Object obj, Class clazz) {
      if (obj != null && clazz != null)
         return clazz.isAssignableFrom (obj.getClass());
      return false;
   }
   /**
    * Executes the specified JOE script and returns it as an object.
    */
   public Object $new (String fname) throws Exception {
      Block blk = ScriptManager.load (this, fname, null);
      blk = blk.$new ();
      return blk;
   }
   /**
    * Executes the specified JOE script and returns it as an object.
    */
   public Object $new (String fname, Object...argv) throws Exception {
      Block blk = ScriptManager.load (this, fname, argv);
      blk = blk.$new (argv);
      return blk;
   }
   /**
    * Convenience method for instanceof (obj, getClass(clazz)).
    */
   public boolean $instanceof (Object obj, String clazz) {
      try {
         return $instanceof (obj, getClass(clazz));
      } catch (Exception ex) {
         return false;
      }
   }
   /**
    * Returns a string containing the line terminator.
    */
   public String nl () {
      return eol;
   }
   /**
    * Returns a string one character long containing the character whose
    * codepoint is specified as argument.
    */
   public String chr (Integer n) {
      return Character.toString ((char) n.intValue());
   }
   /**
    * Returns a char representing the codepoint corresponding
    * to the first letter in a string.
    */
   public char asc (String a) {
      return a.charAt (0);
   }
   /**
    * Makes the child inheriting from parent.
    */
   public void $extends (Block child, Block parent) {
      child.$extends (parent);
   }
   /**
    * Returns an URL based on the base URL of the first script run.
    */
   public URL getURL (String spec) throws MalformedURLException {
      return ScriptManager.get (this).getURL (spec);
   }
   /**
    * Returns the version.
    */
   public String version () {
      return Revision.id;
   }
}
