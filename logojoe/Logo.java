// import java.awt.*;       // Using AWT's Graphics and Color
// import java.awt.event.*; // Using AWT event classes and listener interfaces
// import javax.swing.*;    // Using Swing's components and containers

import java.awt.BasicStroke;
import java.awt.Color;
import java.awt.Container;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.RenderingHints;
import java.awt.geom.Point2D;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import javax.imageio.ImageIO;
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.SwingUtilities;

public class Logo extends JFrame {
   private static class Turtle extends Point2D.Double {
      private boolean show;
      Turtle (double x, double y) {
         super (x, y);
      }
   }
   private static class Canvas extends JPanel {
      private final double SQRT2 = Math.sqrt (2);
      private final int width, height;
      private BufferedImage image;
      private Graphics2D imgG2;
      private Turtle currTurtle;
      private boolean penDown;
      private Color currBg = Color.white;
      private Color currPen = Color.black;
      private double direction;
      private int delay = 0;
      private int penSize = 1;

      Canvas (int width, int height) {
         this.width = width;
         this.height = height;
         clearScreen();
         setPreferredSize(new Dimension(width, height));
      }
      Turtle newturtle () {
         return new Turtle (width / 2, height / 2);
      }
      void home () {
         Turtle t = newturtle();
         movePenTo (t);
         currTurtle = t;
         currTurtle.show = true;
         direction = 0;
      }
      void clearScreen () {
         image = new BufferedImage (width,height,BufferedImage.TYPE_INT_ARGB);
         imgG2 = image.createGraphics();
         imgG2.setColor (currPen);
         imgG2.setColor (currBg);
         imgG2.fillRect (0, 0, width, height);
         imgG2.setColor (currPen);
         imgG2.setRenderingHint(RenderingHints.KEY_ANTIALIASING,
                                RenderingHints.VALUE_ANTIALIAS_ON);
         currTurtle = newturtle();
         currTurtle.show = true;
         direction = 0;
         penDown = true;
         penSize = 1;
      }
      void drawTurtle (Graphics g) {
         if (currTurtle.show) {
            g.setColor (Color.black);
            final int x = (int) Math.round(currTurtle.getX());
            final int y = (int) Math.round(currTurtle.getY());
            switch (((int) direction + 22) / 45) {
            case 7:
               g.drawLine (x - 2, y - 2,
                           x + 2, y - 2);
               g.drawLine (x - 2, y + 2,
                           x - 2, y - 2);
               g.setColor (Color.white);
               g.drawLine (x - 1, y - 1,
                           x + 1, y - 1);
               g.drawLine (x - 1, y + 1,
                           x - 1, y - 1);
               break;
            case 6:
               g.drawLine (x - 3, y,
                           x    , y + 3);
               g.drawLine (x - 3, y,
                           x    , y - 3);
               g.setColor (Color.white);
               g.drawLine (x - 2, y,
                           x    , y + 2);
               g.drawLine (x - 2, y,
                           x    , y - 2);
               break;
            case 5:
               g.drawLine (x - 2, y + 2,
                           x + 2, y + 2);
               g.drawLine (x - 2, y + 2,
                           x - 2, y - 2);
               g.setColor (Color.white);
               g.drawLine (x - 1, y + 1,
                           x + 1, y + 1);
               g.drawLine (x - 1, y + 1,
                           x - 1, y - 1);
               break;
            case 4:
               g.drawLine (x - 3, y - 1,
                           x    , y + 2);
               g.drawLine (x + 3, y - 1,
                           x    , y + 2);
               g.setColor (Color.white);
               g.drawLine (x - 2, y - 1,
                           x    , y + 1);
               g.drawLine (x + 2, y - 1,
                           x    , y + 1);
               break;
            case 3:
               g.drawLine (x - 2, y + 2,
                           x + 2, y + 2);
               g.drawLine (x + 2, y + 2,
                           x + 2, y - 2);
               g.setColor (Color.white);
               g.drawLine (x - 1, y + 1,
                           x + 1, y + 1);
               g.drawLine (x + 1, y + 1,
                           x + 1, y - 1);
               break;
            case 2:
               g.drawLine (x + 3, y,
                           x    , y + 3);
               g.drawLine (x + 3, y,
                           x    , y - 3);
               g.setColor (Color.white);
               g.drawLine (x + 2, y,
                           x    , y + 2);
               g.drawLine (x + 2, y,
                           x    , y - 2);
               break;
            case 1:
               g.drawLine (x - 2, y - 2,
                           x + 2, y - 2);
               g.drawLine (x + 2, y + 2,
                           x + 2, y - 2);
               g.setColor (Color.white);
               g.drawLine (x - 1, y - 1,
                           x + 1, y - 1);
               g.drawLine (x + 1, y + 1,
                           x + 1, y - 1);
               break;
            case 8:
            case 0:
               g.drawLine (x - 3, y + 1,
                           x    , y - 2);
               g.drawLine (x + 3, y + 1,
                           x    , y - 2);
               g.setColor (Color.white);
               g.drawLine (x - 2, y + 1,
                           x    , y - 1);
               g.drawLine (x + 2, y + 1,
                           x    , y - 1);
               break;
            default:
               g.drawOval ((int) currTurtle.getX() - 3,
                           (int) currTurtle.getY() - 3, 6, 6);
               g.setColor (Color.white);
               g.drawOval ((int) currTurtle.getX() - 2,
                           (int) currTurtle.getY() - 2, 4, 4);
               break;
            }
         }
      }
      public void paintComponent(Graphics g) {
         super.paintComponent(g);
         g.drawImage (image, 0, 0, null);
         drawTurtle (g);
      }
      public void delay() {
         if (delay > 0)
            try {
               Thread.sleep (delay);
            } catch (InterruptedException _ex) {
            }
      }
      private Point2D newPoint (double px) {
         final Point2D Return;
         double radians = Math.toRadians (direction);
            Return = new Point2D.Double(currTurtle.getX()+px*Math.sin (radians),
                                       currTurtle.getY()-px*Math.cos (radians));
         return Return;
      }
      void movePenTo (Point2D end) {
         delay();
         if (penDown) {
            imgG2.drawLine ((int) Math.round (currTurtle.getX()),
                            (int) Math.round (currTurtle.getY()),
                            (int) Math.round (end.getX()),
                            (int) Math.round (end.getY()));
         }
         currTurtle.setLocation (end);
         repaint();
      }
      void arc (double angle, double radius) {
         delay();
         if (penDown) {
            final Point2D start = newPoint (radius);
            int x = (int) Math.round (currTurtle.getX() - radius);
            int y = (int) Math.round (currTurtle.getY() - radius);
            imgG2.drawArc (x, y,
                           (int) Math.round (radius * 2),
                           (int) Math.round (radius * 2),
                           (int) Math.round (-(direction - 90)),
                           (int) Math.round (-angle));
            repaint();
         }
      }
      void forward (double px) {
         movePenTo (newPoint(px));
      }
      void setpensize (double size) {
         imgG2.setStroke (new BasicStroke ((float)size,BasicStroke.CAP_ROUND,
                                                       BasicStroke.JOIN_ROUND));
      }
      void setxy (double x, double y) {
         movePenTo (new Point2D.Double(width / 2 + x, height / 2 - y));
      }
      Double[] pos () {
         Double Return[] = new Double[2];
         Return[0] = currTurtle.getX() - (width / 2);
         Return[1] = (height / 2) - currTurtle.getY();
         return Return;
      }
      void seth (double angle) {
         direction = angle;
         while (direction > 360)
            direction -= 360;
         while (direction < 0)
            direction += 360;
         repaint();
      }
      void rotate (double angle) {
         seth (direction + angle);
      }
      void setPenDown (boolean down) {
         penDown = down;
      }
      void setPenColor (Color c) {
         currPen = c;
         imgG2.setColor (currPen);
      }
      void setAntiAliasing (boolean aa) {
         if (aa)
            imgG2.setRenderingHint(RenderingHints.KEY_ANTIALIASING,
                                   RenderingHints.VALUE_ANTIALIAS_ON);
         else
            imgG2.setRenderingHint(RenderingHints.KEY_ANTIALIASING,
                                   RenderingHints.VALUE_ANTIALIAS_OFF);
         imgG2.setColor (currPen);
      }
      Color getPenColor () {
         return currPen;
      }
      void setDelay (int ms) {
         delay = ms;
      }
      Color colorunder () {
         return new Color (image.getRGB ((int) Math.round(currTurtle.getX()),
                                         (int) Math.round(currTurtle.getY())));
      }
      void saveImage (String name, String ext) throws IOException {
         final File out = new File (name + "." + ext);
         ImageIO.write (image, ext, out);
      }
   }


   private final Canvas canvas;

   public Logo () {
      this (640, 480);
   }
   public Logo (int width, int height) {
      canvas = new Canvas(width, height);
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
            setContentPane(canvas);
            setDefaultCloseOperation(EXIT_ON_CLOSE);
            pack();
            setVisible(true);
         }
      });
      setTitle("Logo Joe");
   }
   public Logo forward (double px) {
      canvas.forward (px);
      return this;
   }
   public Logo forward (int px) {
      canvas.forward (px);
      return this;
   }
   public Logo back (double px) {
      canvas.forward (-px);
      return this;
   }
   public Logo back (int px) {
      canvas.forward (-px);
      return this;
   }
   public Logo arc (int angle, int radius) {
      canvas.arc (angle, radius);
      return this;
   }
   public Logo arc (double angle, int radius) {
      canvas.arc (angle, radius);
      return this;
   }
   public Logo arc (int angle, double radius) {
      canvas.arc (angle, radius);
      return this;
   }
   public Logo arc (double angle, double radius) {
      canvas.arc (angle, radius);
      return this;
   }
   public Logo right (int angle) {
      canvas.rotate (angle);
      return this;
   }
   public Logo right (double angle) {
      canvas.rotate (angle);
      return this;
   }
   public Logo left (int angle) {
      canvas.rotate (-angle);
      return this;
   }
   public Logo left (double angle) {
      canvas.rotate (-angle);
      return this;
   }
   public Logo seth (int angle) {
      canvas.seth (angle);
      return this;
   }
   public Logo setxy (int x, int y) {
      canvas.setxy (x, y);
      return this;
   }
   public Logo setxy (double x, int y) {
      canvas.setxy (x, y);
      return this;
   }
   public Logo setxy (int x, double y) {
      canvas.setxy (x, y);
      return this;
   }
   public Logo setxy (double x, double y) {
      canvas.setxy (x, y);
      return this;
   }
   public Logo penup () {
      canvas.setPenDown (false);
      return this;
   }
   public Logo pendown () {
      canvas.setPenDown (true);
      return this;
   }
   public Logo setpencolor (int r, int g, int b) {
      canvas.setPenColor (new Color (r, g, b));
      return this;
   }
   public Logo setpencolor (Color c) {
      canvas.setPenColor (c);
      return this;
   }
   public Logo setantialiasingon () {
      canvas.setAntiAliasing (true);
      return this;
   }
   public Logo setantialiasingoff () {
      canvas.setAntiAliasing (false);
      return this;
   }
   public double random (double limit) {
      return Math.random() * limit;
   }
   public int random (int limit) {
      return (int) (Math.random() * limit);
   }
   public Color getpencolor () {
      return canvas.getPenColor ();
   }
   public Color colorunder () {
      return canvas.colorunder ();
   }
   public Turtle newturtle () {
      return canvas.newturtle ();
   }
   public Logo showturtle () {
      canvas.currTurtle.show = true;
      return this;
   }
   public Logo hideturtle () {
      canvas.currTurtle.show = false;
      canvas.repaint();
      return this;
   }
   public double power (int n, int e) {
      return Math.pow (n, e);
   }
   public double power (double n, int e) {
      return Math.pow (n, e);
   }
   public double power (int n, double e) {
      return Math.pow (n, e);
   }
   public double power (double n, double e) {
      return Math.pow (n, e);
   }
   public double sqrt (int n) {
      return Math.sqrt ((double) n);
   }
   public double sqrt (double n) {
      return Math.sqrt (n);
   }
   public Double[] pos () {
      return canvas.pos();
   }
   public double distance (Turtle t) {
      return canvas.currTurtle.distance (t);
   }
   public Logo clearscreen () {
      canvas.clearScreen ();
      return this;
   }
   public Logo home () {
      canvas.home ();
      return this;
   }
   public Logo setpensize (double size) {
      canvas.setpensize (size);
      return this;
   }
   public Logo setpensize (int size) {
      canvas.setpensize (size);
      return this;
   }
   public Logo setDelay (int ms) {
      canvas.setDelay (ms);
      return this;
   }
   public Logo saveImage (String name) throws IOException {
      canvas.saveImage (name, "png");
      return this;
   }
   public Logo saveImage (String name, String ext) throws IOException {
      canvas.saveImage (name, ext);
      return this;
   }
   public Logo exit () {
      setVisible (false);
      dispose ();
      return this;
   }
}
