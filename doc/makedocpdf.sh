export DOCLET=com.tarsec.javadoc.pdfdoclet.PDFDoclet
export JARS=pdfdoclet-1.0.3-all.jar
SRCDIR=../src/com/veryant/joe

javadoc -doclet $DOCLET -docletpath $JARS -pdf Joe_Reference.pdf -config config_html.properties -public $SRCDIR/W*.java $SRCDIR/*Command*.java $SRCDIR/Block.java
