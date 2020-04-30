/*
 * @(#)  ArcEnd.java	1.0 05/15/2002
 *
 * Authors Jeong-Hyon Hwang and Robin Yan (jhhwangcs.brown.edu, rly@cs.brown.edu)
 * Box# 1910, Dept. of Computer Science, Brown University, RI 02912, USA.
 * All Rights Reserved. 
 */
package edu.brown.aurora.gui;

import javax.swing.*;
import javax.swing.*;
import javax.swing.border.*;
import java.awt.event.*;
import java.awt.geom.*;
import java.awt.*;

import java.util.*;

import edu.brown.bloom.petal.*;

/**
 * Describes the end of an arc and draws the specified endpoints
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu) and Robin Yan (rly@cs.brown.edu)
 * @version 1.0 04/10/2002
 */
public class ArcEnd implements PetalArcEnd, java.io.Serializable
{


    static private final double	DEFAULT_SIZE = 6.0;

    private int	end_type;
    private Color	end_color;
    private double	end_size;

    /**
     * Constructs a new ArcEnd object
     *
     * @param type Specifies the type of end for this ArcEnd object
     */
    public ArcEnd(int type)
    {
        this(type,DEFAULT_SIZE,Color.black);
    }

    /**
     * Constructs a new ArcEnd object
     *
     * @param type Specifies the type of end for this ArcEnd object
     * @param size Specifies the size of the end for this ArcEnd object
     */
    public ArcEnd(int type,double size)
    {
        this(type,size,Color.black);
    }


    /**
     * Constructs a new ArcEnd object
     *
     * @param type Specifies the type of end for this ArcEnd object
     * @param size Specifies the size of the end for this ArcEnd object
     * @param c Specifies the color for the end of this ArcEnd object
     */
    public ArcEnd(int type,double size,Color c)
    {
        end_type = type;
        end_color = c;
        end_size = size;
    }

    /**
     * Redraws the arcend object
     *
     * @param g The graphics context to use for drawing
     * @param tp The destination point of this ArcEnd object
     * @param fp The source point of this ArcEnd object
     */     
    public void draw(Graphics g,Point2D tp,Point2D fp)
    {
        this.draw(g,tp,fp,end_color);
    }

    /**
     * Redraws the arcend object with the specified color
     *
     * @param g The graphics context to use for drawing
     * @param tp The destination point of this ArcEnd object
     * @param fp The source point of this ArcEnd object
     * @param col The color with which to redraw the ArcEnd
     */     
    public void draw(Graphics g,Point2D tp,Point2D fp,java.awt.Color col)
    {
        int dtyp = end_type & PETAL_ARC_END_TYPE_MASK;

        end_color = col;
        g.setColor(end_color);
        
        switch (dtyp) {
        case PETAL_ARC_END_CIRCLE :
            drawCircle(g,tp,fp);
            break;
        case PETAL_ARC_END_TRIANGLE :
            drawTriangle(g,tp,fp);
            break;
        case PETAL_ARC_END_ARROW :
            if ((end_type & PETAL_ARC_END_CLOSED) != 0) drawTriangle(g,tp,fp);
            drawArrow(g,tp,fp);
            break;
        case PETAL_ARC_END_SQUARE :
            drawSquare(g,tp,fp);
            break;
        default :
            break;
        }
    }

    private void drawCircle(Graphics g,Point2D tp,Point2D fp)
    {
        double d = fp.distance(tp);
        
        if (d == 0) return;
        
        double t = end_size/d;
        double cx0 = tp.getX() + t*(fp.getX() - tp.getX());
        double cy0 = tp.getY() + t*(fp.getY() - tp.getY());
        
        int x0 = (int)(cx0 - end_size+1);
        int y0 = (int)(cy0 - end_size+1);
        int wd = (int)(2 * end_size - 1);
        int ht = (int)(2 * end_size - 1);
        
        if ((end_type & PETAL_ARC_END_CLOSED) != 0) g.fillOval(x0,y0,wd,ht);
        else g.drawOval(x0,y0,wd,ht);
    }
    
    
    
    
    
    private void drawTriangle(Graphics g,Point2D tp,Point2D fp)
    {
        double d = fp.distance(tp);
        
        if (d == 0) return;
        
        double t = end_size/d;
        double cx0 = tp.getX() + 2*t*(fp.getX() - tp.getX());
        double cy0 = tp.getY() + 2*t*(fp.getY() - tp.getY());
        double cx1 = cx0 + t*(fp.getY() - tp.getY());
        double cy1 = cy0 - t*(fp.getX() - tp.getX());
        double cx2 = cx0 - t*(fp.getY() - tp.getY());
        double cy2 = cy0 + t*(fp.getX() - tp.getX());
        
        Polygon p = new Polygon();
        p.addPoint((int) tp.getX(),(int) tp.getY());
        p.addPoint((int) cx1,(int) cy1);
        p.addPoint((int) cx2,(int) cy2);
        
        if ((end_type & PETAL_ARC_END_CLOSED) != 0) g.fillPolygon(p);
        else g.drawPolygon(p);
    }
    
    
    
    
    private void drawArrow(Graphics g,Point2D tp,Point2D fp)
    {
        double d = fp.distance(tp);
        
        if (d == 0) return;
        
        double t = end_size/d;
        double cx0 = tp.getX() + 2*t*(fp.getX() - tp.getX());
        double cy0 = tp.getY() + 2*t*(fp.getY() - tp.getY());
        double cx1 = cx0 + t*(fp.getY() - tp.getY());
        double cy1 = cy0 - t*(fp.getX() - tp.getX());
        double cx2 = cx0 - t*(fp.getY() - tp.getY());
        double cy2 = cy0 + t*(fp.getX() - tp.getX());
        
        g.drawLine((int) cx1,(int) cy1,(int) tp.getX(),(int) tp.getY());
        g.drawLine((int) cx2,(int) cy2,(int) tp.getX(),(int) tp.getY());
    }
    
    
    
    private void drawSquare(Graphics g,Point2D tp,Point2D fp)
    {
        double d = fp.distance(tp);
        
        if (d == 0) return;
        
        double dx = (tp.getX() - fp.getX())/d*end_size*0.5;
        double dy = (fp.getY() - tp.getY())/d*end_size*0.5;
        
        Polygon p = new Polygon();
        p.addPoint((int)(tp.getX()-dy),(int)(tp.getY()-dx));
        p.addPoint((int)(tp.getX()+dy),(int)(tp.getY()+dx));
        p.addPoint((int)(tp.getX()-2*dx-dy),(int)(tp.getY()-2*dy-dx));
        p.addPoint((int)(tp.getX()-2*dx+dy),(int)(tp.getY()-2*dy+dx));
        
        if ((end_type & PETAL_ARC_END_CLOSED) != 0) g.fillPolygon(p);
        else g.drawPolygon(p);
    }
}
