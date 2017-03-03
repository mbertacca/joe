In this directory you can find some program examples showing the
JOE capabilities, in particular the recursion.

In order make things easier to see (and funnier), a class Logo.java
has been written: this class contains some of the Logo programming language 
commands implemented as API. Note that even 'repeat' and 'stop' have been
implemented.

A jar file containing both JOE and the Logo class is supplied,
so that you can execute a script with the following command:

    java -jar logojoe.jar <script-name>

Two OS scripts (viewall.sh for *n*x and viewall.cmd for Windows) are
also provided in order to perform all the scripts in sequence.

A JOE script is supplied as well, You can run it using the following command:

java -jar logojoe.jar viewall.jsh
