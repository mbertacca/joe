# Java Objects Executor: a polymorphic script environment.![alt tag](https://raw.githubusercontent.com/mbertacca/joe/master/samples/logojoe/joe.png)

Many applications in the real world are constituted by a mix of programs and scripts: programs communicate with the user and make the calculations while the scripts do the administrative tasks communicating mainly with the OS.

When you port a legacy application in a Java environment, you want only one environment, therefore you have to migrate the scripts to Java: this can be an hard and lengthy task and the result is in many cases less flexible than the starting point because interpreted procedures are now compiled and because administrative tasks are now handled using a general purpose language.

So you can find useful to have a script language whose features are:

- ability to access any Java resource: Java is operating system independent therefore the JVM is its virtual operating system;
- easy to change in order to get similar to any script language in terms of capability and readability;
- easy to customize in order to get frequently used operations at hand;
- easy to extend in order to be useful also for future applications’ enhancements, not only for the migration process;
- easy to understand and use;
- 100% compatible with Java.

The Java Objects Executor (JOE for short) can be the answer to the above requests.

JOE only task is to execute methods of Java objects in sequence on the fly, however it can be used to mimic any scripting language,
with encapsulation, inheritance, nested functions and closures.

The syntax can be summarized in the following 2 lines:

*message:* [var `:=` ] obj [ method1 { ; | obj-arg1-1 [,obj-arg1-2] ... } [ method2 { ; | obj-arg2-1 [,obj-arg2-2] ...}] ... ]`.`  
*block:* `{` [[name]:[arg1 [arg2] ...].] [*message*] ... `}`

*block* is an object itself therefore it can be put wherever an object can. Parenthesis can be used to alter the default left-to-right evaluation  order.

I developed also a version using C language that could be used in small devices (automation, robotics, Internet of things) in order to allow the use of an object oriented interpreter with garbage collection and a small memory footprint. Currently it is just a proof-of-concept developed on Linux, however it should be easy to compile it on any other platform.

