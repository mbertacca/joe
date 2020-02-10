javac -g -cp src src/com/veryant/joe/*.java
jar cvfm jar/joe.jar jar/Manifest.txt -C src dummy src/com/veryant/joe/*.class

DIR=samples/logojoe
cp jar/joe.jar $DIR/logojoe.jar
jar uvf $DIR/logojoe.jar -C $DIR dummy $DIR/Logo*.class $DIR/joe.png
javac -g -cp $DIR:$DIR/logojoe.jar $DIR/*.java

DIR=samples/form
cp jar/joe.jar $DIR/formjoe.jar
jar uvf $DIR/formjoe.jar -C $DIR dummy $DIR/*.class $DIR/joe.png
javac -g -cp $DIR:$DIR/formjoe.jar $DIR/*.java

