/*
          ooo
   oo    ooo
    oo  ooo
v e r y a n t
      ooo

http://www.veryant.com

 (C) 2016
 ________________
/  _   _   ____  \
| | | | | |  __| |
| | |_| | | |__  |
| |     | |____| |
\_|     |________/
Marco   Bertacca
*/

package com.veryant.joe.cs;

public class FlowControl {
   enum Type {
      LABEL_EXISTS,
      GOTO,
      GOSUB,
      RETURN
   };
   final Type type;
   final String label;
   public FlowControl () {
      type = Type.RETURN;
      label = null;
   }
   public FlowControl (Type t, String lbl) {
      type = t;
      label = lbl;
   }
}
