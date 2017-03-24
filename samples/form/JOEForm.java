/*
 *
 * This source file is part of the "Java Objects Executor" open source project
 *
 * Copyright 2017 Marco Bertacca
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

import java.awt.Dimension;
import java.awt.image.BufferedImage;
import java.net.URL;
import java.net.URLDecoder;
import javax.swing.JEditorPane;
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.SwingUtilities;
import javax.swing.event.HyperlinkEvent;
import javax.swing.event.HyperlinkListener;
import javax.swing.text.AttributeSet;
import javax.swing.text.Document;
import javax.swing.text.Element;
import javax.swing.text.ElementIterator;
import javax.swing.text.StyleConstants;
import javax.swing.text.html.HTML;
import javax.swing.text.html.FormSubmitEvent;
import javax.swing.text.html.HTMLDocument;
import javax.swing.text.html.HTMLEditorKit;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import javax.imageio.ImageIO;
import java.util.Enumeration;
import java.util.HashMap;

public class JOEForm extends JFrame {
   private static final long serialVersionUID = 789198678271434l;

   private JEditorPane pane;
   private HTMLEditorKit kit;
   private HTMLDocument doc;
   private final int height;
   private final int width;
   private final HashMap<String,String>formContent=new HashMap<String,String>();

   public JOEForm () {
      this (800, 600);
   }
   public JOEForm (final int width, final int height) {
      this.width = width;
      this.height = height;
      SwingUtilities.invokeLater(new Runnable() {
         public void run() {
            try {
               InputStream iconStream=getClass().getResourceAsStream("joe.png");
               if (iconStream != null) {
                  BufferedImage icon = ImageIO.read (iconStream);
                  setIconImage (icon);
               }
            } catch (IOException _ex) {
            }
            pane = new JEditorPane();
            pane.setEditable (false);
            pane.setFocusCycleRoot(false);
            pane.setFocusable(false);
            pane.setPreferredSize(new Dimension(width, height));
            pane.addHyperlinkListener(new HyperlinkListener () {
               public void hyperlinkUpdate(HyperlinkEvent e) {
                  if (e instanceof FormSubmitEvent) {
                     synchronized (formContent) {
                        formContent.clear();
                        final FormSubmitEvent fse = (FormSubmitEvent) e;
                        String field[] =  fse.getData().split ("&");
                        for (int i = 0; i < field.length; i++) {
                           String part[] = field[i].split ("=");
                           if (part.length > 1)
                              formContent.put (part[0], urlDecode(part[1]));
                           else
                              formContent.put (part[0], "");
                        }
                        formContent.notify();
                     }
                  } else {
                     //System.out.println ("=[" + e + "]=");
                  }
               }
            });
            pane.setEditorKit(kit = new HTMLEditorKit());
            kit.setAutoFormSubmission (false);

            setContentPane(new JScrollPane(pane));
            setDefaultCloseOperation(EXIT_ON_CLOSE);
            pack();
            setVisible(true);
         }
      });
      setTitle("Joe Form handler");
   }
   private static String urlDecode (String in) {
      try {
         return URLDecoder.decode(in, "UTF-8");
      } catch (java.io.UnsupportedEncodingException _ex) {
         return in;
      }
   }

   public HashMap input() throws Exception {
      synchronized (formContent) {
         formContent.wait();
      }
      return formContent;
   }

   public void loadFile (final String fileName) throws Exception {
      final URL url = new File (fileName).toURI().toURL();
      SwingUtilities.invokeAndWait(new Runnable() {
         public void run() {
            try {
               pane.setDocument(doc=(HTMLDocument)kit.createDefaultDocument());
               doc.setAsynchronousLoadPriority (-1);
               //pane.setText (TXT);
               pane.setPage (url);
               doc = (HTMLDocument) pane.getDocument();
            } catch (IOException _ex) {
               _ex.printStackTrace();
            }
         }
      });
   }
   public JOEForm exit () {
      setVisible (false);
      dispose ();
      return this;
   }
   private String debug(Element element) {
      Object key, val;
      AttributeSet as = element.getAttributes();
      StringBuilder sb = new StringBuilder();
      sb.append (element.getName());
      for (Enumeration en = as.getAttributeNames(); en.hasMoreElements(); ) {
         key = en.nextElement();
         val = as.getAttribute(key);
         if (val instanceof String) {
            sb.append (" ");
            sb.append (key.toString());
            sb.append ("=\"");
            sb.append (val.toString());
            sb.append ("\"");
         }
      }
      return sb.toString();
   }
   private Element getById (final String id) {
      final Element[] Return = new Element[1];
      try {
         SwingUtilities.invokeAndWait(new Runnable() {
            public void run() {
               ElementIterator iterator = new ElementIterator(doc);
               Element element;
               while ((element = iterator.next()) != null) {
                  AttributeSet as = element.getAttributes();
                  if (id.equals(as.getAttribute (HTML.Attribute.ID))) {
                     Return[0] = element;
                     return;
                  }
               }
            }
         });
      } catch (Exception _ex) {
      }
      return Return[0];
   }
   private void removeTag (Element element, String id) {
      try {
         doc.setOuterHTML (element, "<DIV id=\"" + id + "\"/>");
      } catch (javax.swing.text.BadLocationException _ex) {
         System.out.println (_ex);
      } catch (IOException _ex) {
         System.out.println (_ex);
      }
   }
   private void replaceValue (Element element, String value) {
      boolean valueFound = false;
      Object key, val;
      AttributeSet as = element.getAttributes();

      StringBuilder sb = new StringBuilder();
      sb.append ("<");
      sb.append (element.getName());
      for (Enumeration en = as.getAttributeNames(); en.hasMoreElements(); ) {
         key = en.nextElement();
         val = as.getAttribute(key);
         if (val instanceof String) {
            sb.append (" ");
            sb.append (key.toString());
            sb.append ("=\"");
            if (valueFound = (HTML.Attribute.VALUE == key))
               sb.append (value);
            else
               sb.append (val.toString());
            sb.append ("\"");
         }
      }
      if (!valueFound) {
         sb.append (" value=\"");
         sb.append (value);
         sb.append ("\"");
      }
      sb.append ("/>");
      try {
         doc.setOuterHTML (element, sb.toString());
      } catch (javax.swing.text.BadLocationException _ex) {
      } catch (IOException _ex) {
      }
   }
   private void replaceText (Element element, String text) {
      Object key, val;
      AttributeSet as = element.getAttributes();

      StringBuilder sb = new StringBuilder();
      sb.append ("<");
      sb.append (element.getName());
      for (Enumeration en = as.getAttributeNames(); en.hasMoreElements(); ) {
         key = en.nextElement();
         val = as.getAttribute(key);
         if (val instanceof String) {
            sb.append (" ");
            sb.append (key.toString());
            sb.append ("=\"");
            sb.append (val.toString());
            sb.append ("\"");
         }
      }
      sb.append (">");
      sb.append (text);
      sb.append ("</");
      sb.append (element.getName());
      sb.append (">");
      try {
         doc.setInnerHTML (element, sb.toString());
      } catch (javax.swing.text.BadLocationException _ex) {
      } catch (IOException _ex) {
      }
   }
   public JOEForm setText (String id, String text) {
      Element element = getById (id);
      if (element != null) {
         replaceText (element, text);
      }
      return this;
   }
   public JOEForm setValue (String id, String value) {
      Element element = getById (id);
      if (element != null) {
         replaceValue (element, value);
      }
      return this;
   }
   public JOEForm remove (String id) {
      Element element = getById (id);
      if (element != null) {
         removeTag (element, id);
      }
      return this;
   }
}
