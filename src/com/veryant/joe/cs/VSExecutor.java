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

import com.veryant.joe.Block;
import com.veryant.joe.BreakCmdException;
import com.veryant.joe.BreakLoopException;
import com.veryant.joe.Executor;
import com.veryant.joe.JOEException;
import com.veryant.joe.Message;
import com.veryant.joe.SingleVariableMessage;
import com.veryant.joe.WBoolean;

public class VSExecutor implements Executor {
   private boolean tron;

   private Block block0;
   private String labels[];

   public VSExecutor (boolean tron) {
      this.tron = tron;
   }

   public Object run (Block blk) throws JOEException {
      if (block0 == null) {
         block0 = blk;
         labels = new String[blk.size()];
         Message msg;

         for (int i = 0; i < labels.length; i++) {
            if ((msg = blk.get(i)) instanceof SingleVariableMessage) {
               labels[i] = ((SingleVariableMessage) msg).getName();
               blk.setConstant (labels[i], labels[i]);
            }
         }
      }
      return goSub (blk, 0); 
   }

   private final int find (String lbl) throws JOEException {
      for (int Return = 0; Return < labels.length; Return++)
         if (lbl.equals (labels[Return]))
            return Return;
      throw new JOEException ("Missing label \"" + lbl + "\"");
   }

   private Object goSub (Block blk, int begin) throws JOEException {
      Message msg;
      Object obj, Return = null;
      final int end = blk.size();
      exitFor: for (int pc = begin; pc < end; pc++) {
         msg = blk.get(pc);
         if (tron)
            System.err.println ("+ " + msg.toString());
         try {
            obj = msg.exec();
         } catch (BreakLoopException ex) {
            if (!ex.hasReturnObject())
               ex.setReturnObject(Return);
             throw ex;
         } catch (BreakCmdException ex) {
            final String blkName = ex.getMessage();
            if (blkName.length() > 0) {
               if (blkName.equals (blk.name())) {
                  if (ex.hasReturnObject())
                     Return = ex.getReturnObject();
                  break exitFor;
               } else {
                  if (!ex.hasReturnObject())
                      ex.setReturnObject(Return);
                   throw ex;
               }
            } else {
               if (ex.hasReturnObject())
                  Return = ex.getReturnObject();
               break exitFor;
            }
         }

         if (obj instanceof FlowControl) {
            FlowControl fc = (FlowControl) obj;
            switch (fc.type) {
            case RETURN:
               if (blk != block0)
                  Return = fc;
               break exitFor;
            case GOTO:
               Return = obj;
               if (blk == block0)
                  pc = find (fc.label);
               else
                  break exitFor;
               break;
            case GOSUB:
               goSub (block0, find (fc.label));
               break;
            case LABEL_EXISTS:
               try {
                  find (fc.label);
                  Return = new WBoolean (true);
               } catch (JOEException _ex) {
                  Return = new WBoolean (false);
               }
               break;
            }
         } else {
            Return = obj;
         }

      }
      return Return;
   }
}

