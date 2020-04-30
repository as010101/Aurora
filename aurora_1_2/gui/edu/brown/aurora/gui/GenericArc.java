package edu.brown.aurora.gui;

import edu.brown.bloom.petal.*;

import edu.brown.aurora.gui.dbts.*;
import edu.brown.aurora.gui.types.*;

import javax.swing.*;
import javax.swing.border.*;
import java.awt.event.*;
import java.awt.geom.*;
import java.awt.*;

import java.util.*;

import com.sleepycat.db.*;

/**
 * A method that implements the methods for PetalArc.
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu) and Robin Yan (rly@cs.brown.edu)
 * @version 1.0 04/10/2002
 */

public class GenericArc implements java.io.Serializable, PetalArc 
{
    protected class InnerStroke extends BasicStroke implements java.io.Serializable
    {
        public InnerStroke()
        {
            super(2, BasicStroke.CAP_BUTT, BasicStroke.JOIN_ROUND);
        }
    }
    
    static int idCounter = 0;
    
    private GenericNode	source_node;
    private GenericNode	target_node;
    
    private ArcEnd	source_end;
    private ArcEnd	target_end;
    
    private Vector		arc_pivots;
    private Point []	arc_points;
    
    private InnerStroke	arc_stroke;
    transient private GeneralPath	arc_path;
    
    private boolean 	spline_arc;
    
    protected BoxNodeInputPort inputPort;
    protected BoxNodeOutputPort outputPort;
    
    protected int arcId = 0;
    
    protected int parentId;

    public GenericArc(GenericNode src, BoxNodeOutputPort opt,
                      GenericNode tgt, BoxNodeInputPort ipt,
                      int parentId)
    {
        inputPort = ipt;
        outputPort = opt;      

        this.parentId = parentId;

        inputPort.addArc(this);
        outputPort.addArc(this);  
        
        arcId = idCounter;
        idCounter++;
        
        source_node = src;
        target_node = tgt;
        
        arc_pivots = null;
        
        arc_stroke = new InnerStroke();
        arc_path = null;				// not drawn
        arc_points = null;
        source_end = null;
        target_end = null;
        
        spline_arc = true;
        
    }

    public void setArcId(int arcId)
    {
        this.arcId = arcId;
    }

    public boolean equals(Object other)
    {
        if(other == null || !(other instanceof GenericArc)) {
            return false;
        }
        GenericArc otherArc = (GenericArc)other;

        return 
            otherArc.parentId == this.parentId &&
            otherArc.arcId == this.arcId;
    }

    public String toString()
    {
        return
            "Arc:  [" + source_node.getBoxId() + ", " + outputPort.getIndex() + 
            "] --> [" + target_node.getBoxId() + ", " + inputPort.getIndex() + "]";
    }
    
    /**
     * Saves this generic arc object into the arcTable.
     *
     * @param arcTable The sleepycat database table to store the arc object in.
     */
    public void saveRecord(Db arcTable) throws Throwable
    {
        IntegerArrayDbt key = new IntegerArrayDbt(new int[] {getArcId(), parentId});
        
        GenericNode s = (GenericNode)getSource();
        GenericNode n = (GenericNode)getTarget();
        
        int targetPortId = 0;
        int sourcePortId = 0;
        if (n.getComponent() instanceof BoxComponent) {
            BoxComponent boxComponent = (BoxComponent)n.getComponent();
            targetPortId = boxComponent.getInputPortIndex(getInputPort());
        }
        
        if (s.getComponent() instanceof BoxComponent) {
            BoxComponent boxComponent = (BoxComponent)s.getComponent();
            sourcePortId = boxComponent.getOutputPortIndex(getOutputPort());
        }
        
        int typeId = RootFrame.typeManager.findCompositeTypeIndex(getOutputPort().getType());
        
        ArcRecord arcRecord = new ArcRecord(getArcId() /* id */, 
                                            1 /* rate */,
                                            typeId /* typeId */,
                                            s.getBoxId() /* sourceNodeId */,
                                            n.getBoxId() /*targetNodeId */,
                                            sourcePortId /*sourcePortId */,
                                            targetPortId /* targetPortId */,
                                            1 /* cpFlag */,
                                            parentId);
        if (arcTable.put(null, key, arcRecord, 0) == Db.DB_KEYEXIST) {
            System.out.println("GenericArc.saveRecordDbt() - The key already exists.");
        }
        
    }

    
    public PaletteRecord getPaletteRecord(int rootBoxId)
    {
        GenericNode s = (GenericNode)getSource();
        GenericNode n = (GenericNode)getTarget();
        
        int targetPortId = 0;
        int sourcePortId = 0;
        if (n.getComponent() instanceof BoxComponent) {
            BoxComponent boxComponent = (BoxComponent)n.getComponent();
            targetPortId = boxComponent.getInputPortIndex(getInputPort());
        }
        
        if (s.getComponent() instanceof BoxComponent) {
            BoxComponent boxComponent = (BoxComponent)s.getComponent();
            sourcePortId = boxComponent.getOutputPortIndex(getOutputPort());
        }
        
        int typeId = RootFrame.typeManager.findCompositeTypeIndex(getOutputPort().getType());
        
        int sourceBoxId = s.getBoxId();
        int targetBoxId = n.getBoxId();

        ArcPaletteRecord record = new ArcPaletteRecord(arcId, 
                                                       1,            // rate
                                                       typeId,
                                                       sourceBoxId,
                                                       targetBoxId,
                                                       sourcePortId, 
                                                       targetPortId,
                                                       1,            // cpFlag
                                                       parentId,
                                                       rootBoxId);
        return record;
    }
    


    public int getParentId()
    {
        return parentId;
    }


    public void setParentId(int parentId)
    {
        this.parentId = parentId;
    }

    /**
     * Returns a GenericArc object given from the record.
     *
     * @param arcRecord The record object representing an arc tuple in the sleepycat
     * database.
     * @param model The model that contains this generic arc object.
     */
    public static GenericArc loadRecord(ArcRecord arcRecord, Model model)
    {
        arcRecord.parse();

        GenericNode n1 = model.getBoxNode(arcRecord.getSourceNodeId());
        GenericNode n2 = model.getBoxNode(arcRecord.getTargetNodeId());

        int sourcePortIndex = arcRecord.getSourcePortIndex();
        int targetPortIndex = arcRecord.getTargetPortIndex();            
        
        GenericArc pa = null;
        if (n1 != null && n2 != null && model.getSuperBox().getBoxId() == n1.getParentId()) {
            BoxComponent n1Component = (BoxComponent)n1.getComponent();
            BoxNodeOutputPort boxNodeOutputPort = n1Component.getOutputPort(sourcePortIndex);
            
            BoxComponent n2Component = (BoxComponent)n2.getComponent();
            BoxNodeInputPort boxNodeInputPort = n2Component.getInputPort(targetPortIndex);
            
            pa = new GenericArc(n1, boxNodeOutputPort, n2, boxNodeInputPort, arcRecord.getParentId());
        } else if (n1 == null && n2 != null && 
                   model.getSuperBox().getBoxId() == n2.getParentId()) {
            BoxComponent n2Component = (BoxComponent)n2.getComponent();
            BoxNodeInputPort boxNodeInputPort = n2Component.getInputPort(targetPortIndex);

            InputPortNode n11 = model.getInputPortNode(arcRecord.getSourceNodeId());
            BoxNodeOutputPort opt = n11.getOutputPortNode();
            
            pa = new GenericArc(n11, opt, n2, boxNodeInputPort, arcRecord.getParentId());
        } else if (n1 != null && n2 == null && 
                   model.getSuperBox().getBoxId() == n1.getParentId()) {
            BoxComponent n1Component = (BoxComponent)n1.getComponent();
            BoxNodeOutputPort boxNodeOutputPort = n1Component.getOutputPort(sourcePortIndex);
            OutputPortNode n22 = model.getOutputPortNode(arcRecord.getTargetNodeId());
            BoxNodeInputPort ipt = n22.getInputPortNode();
            
            pa = new GenericArc(n1, boxNodeOutputPort, n22, ipt, arcRecord.getParentId());
        } else if (n1 == null && n2 == null) { 
            // kinda a dumb situation, an inputportnode is connected to an output portnode 
            // directly.
            /*
            InputPortNode n11 = model.getInputPortNode(arcRecord.getSourceNodeId());
            if(n11 != null && n11.getParentId() == model.getSuperBox().getBoxId()) {
                BoxNodeOutputPort opt = n11.getOutputPortNode();
                
                OutputPortNode n22 = model.getOutputPortNode(arcRecord.getTargetNodeId());
                BoxNodeInputPort ipt = n22.getInputPortNode();
                
                pa = new GenericArc(n11, opt, n22, ipt, arcRecord.getParentId());
            } 
            */           
        }

        return pa;
    }

    public static void setCurrentIdCounter(int id)
    {
        idCounter = id;
    }
    
    public static int getCurrentIdCounter()
    {
        return idCounter;
    }
    
    
    public int getArcId()
    {
        return arcId;
    }
    
    public BoxNodeInputPort getInputPort()
    {
        return inputPort;
    }
    
    public BoxNodeOutputPort getOutputPort()
    {
        return outputPort;
    }
    
    /********************************************************************************/
    /*										*/
    /*	Constants								*/
    /*										*/
    /********************************************************************************/
    
    static final int	CORRELATE_SIZE = 4;
    static final int	PIVOT_CORRELATE_SIZE = 8;
    
    
    /********************************************************************************/
    /*										*/
    /*	Private Storage 							*/
    /*										*/
    /********************************************************************************/
    
    
    /********************************************************************************/
    /*										*/
    /*	Access methods								*/
    /*										*/
    /********************************************************************************/

    public PetalNode getSource()		{ return source_node; }
    
    public PetalNode getTarget()		{ return target_node; }
    
    public BoxPortConnectionPoint getSourcePort()	{ return outputPort.getPort(); }
    
    public BoxPortConnectionPoint getTargetPort()	{ return inputPort.getPort(); }
    
    
    public void setSourceEnd(ArcEnd e) 	{ source_end = e; }
    
    public void setTargetEnd(ArcEnd e) 	{ target_end = e; }
    
    
    public void setSource(GenericNode pn)
    {
        if (source_node == pn) return;
        
        source_node = pn;
        clearPivots();
        arc_path = null;
    }
    
    
    
    public void setTarget(GenericNode pn)
    {
        if (target_node == pn) return;
        
        target_node = pn;
        clearPivots();
        arc_path = null;
    }
    
    
    /*
      public GenericArc cloneArc(PetalClipSet pcs)
      {
      GenericArc pa = null;
      
      try {
      pa = (GenericArc) clone();
      }
      catch (CloneNotSupportedException e) { }
      
      if (pa != null && pcs != null) {
      pa.source_node = pcs.getMapping(getSource());
      pa.target_node = pcs.getMapping(getTarget());
      }
      
      return pa;
      }
      
      
    */
    public void setSplineArc(boolean fg)			{ spline_arc = fg; }
    public boolean getSplineArc()				{ return spline_arc; }
    
    
    
    
    public void getPropertiesFrom(GenericArc h)
    {
        source_end = h.source_end;
        target_end = h.target_end;
        
        if (h.arc_pivots != null) arc_pivots = new Vector(h.arc_pivots);
        
        spline_arc = h.spline_arc;
    }
    
    
    
    /********************************************************************************/
    /*										*/
    /*	Methods to get the points for the arc					*/
    /*										*/
    /********************************************************************************/
    
    public Point [] getPoints()
    {
        return arc_points;
    }
    
    
    
    /********************************************************************************/
    /*										*/
    /*	Method to draw the arc							*/
    /*										*/
    /********************************************************************************/
    
    public void layout()
    {
        arc_path = new GeneralPath();
        
        int ct = 2;
        if (arc_pivots != null) ct += arc_pivots.size();
        arc_points = new Point[ct];
        ct = 0;
        
        Component sc = source_node.getComponent();
        Component tc = target_node.getComponent();
        
        arc_points[ct++] = outputPort.getPort().getConnectionPoint(sc.getBounds());
        
        if (arc_pivots != null) {
            for (Enumeration e = arc_pivots.elements(); e.hasMoreElements(); ) {
                arc_points[ct++] = (Point) e.nextElement();
            }
        }
        
        arc_points[ct] = inputPort.getPort().getConnectionPoint(tc.getBounds());
        
        arc_points[0] = source_node.findPortPoint(arc_points[0],arc_points[1]);
        arc_points[ct] = target_node.findPortPoint(arc_points[ct],arc_points[ct-1]);
        
        if (!spline_arc || arc_points.length == 2) {
            for (int i = 1; i < arc_points.length; ++i) {
                Line2D line = new Line2D.Double(arc_points[i-1],arc_points[i]);
                arc_path.append(line,true);
            }
        }
        else {
            if (arc_points.length == 2) {
                Point p = new Point();
                arc_path.moveTo(arc_points[0].x,arc_points[0].y);
                for (int i = 1; i < arc_points.length; ++i) {
                    if (Math.abs(arc_points[i-1].x - arc_points[i].x) <
                        Math.abs(arc_points[i-1].y - arc_points[i].y)) {
                        p.setLocation(arc_points[i-1].x,arc_points[i].y);
                    }
                    else {
                        p.setLocation(arc_points[i].x,arc_points[i-1].y);
                    }
                    arc_path.quadTo(p.x,p.y,arc_points[i].x,arc_points[i].y);
                }
            }
            else {
                generateSpline();
            }
        }
    }
    
    
    
    private void generateSpline()
    {
        int ptct = arc_points.length;
        Point2D [] npts = new Point2D[ptct];
        double [] g = new double[ptct];
        
        // first find control points that put the original points on the curve
        
        for (int i = 0; i < ptct; ++i) {
            npts[i] = new Point2D.Double(6.0*arc_points[i].x,6.0*arc_points[i].y);
        }
        npts[0].setLocation(arc_points[0].x,arc_points[0].y);
        g[0] = 0;
        
        for (int i = 1; i < ptct-1; ++i) {
            double c = 4.0 - g[i-1];
            npts[i].setLocation((npts[i].getX() - npts[i-1].getX())/c,
                                (npts[i].getY() - npts[i-1].getY())/c);
            g[i] = 1.0/c;
        }
        
        npts[ptct-1].setLocation(arc_points[ptct-1].x,arc_points[ptct-1].y);
        
        for (int i = ptct-2; i > 0; --i) {
            npts[i].setLocation(npts[i].getX() - g[i]*npts[i+1].getX(),
                                npts[i].getY() - g[i]*npts[i+1].getY());
        }
        
        // then we can draw the spline
        
        arc_path.moveTo(arc_points[0].x,arc_points[0].y);
        calcBSpline(npts[0],npts[0],npts[0],npts[1]);
        calcBSpline(npts[0],npts[0],npts[1],npts[2]);
        for (int i = 1; i < ptct-2; ++i) {
            calcBSpline(npts[i-1],npts[i],npts[i+1],npts[i+2]);
        }
        calcBSpline(npts[ptct-3],npts[ptct-2],npts[ptct-1],npts[ptct-1]);
        calcBSpline(npts[ptct-2],npts[ptct-1],npts[ptct-1],npts[ptct-1]);
    }
    
    
    
    private void calcBSpline(Point2D cm1,Point2D c,Point2D cp1,Point2D cp2)
    {
        Point2D p1 = new Point2D.Double();
        Point2D p2 = new Point2D.Double();
        Point2D t = new Point2D.Double();
        Point2D p0 = new Point2D.Double();
        Point2D p3 = new Point2D.Double();
        
        thirdPoint(c,cp1,p1);
        thirdPoint(cp1,c,p2);
        thirdPoint(cp1,cp2,t);
        midPoint(t,p2,p3);
        
        arc_path.curveTo((float) p1.getX(),(float) p1.getY(),
                         (float) p2.getX(),(float) p2.getY(),
                         (float) p3.getX(),(float) p3.getY());
    }
    
    
    
    private void thirdPoint(Point2D p,Point2D q,Point2D r)
    {
        r.setLocation((2*p.getX()+q.getX())/3.0 , (2*p.getY()+q.getY())/3.0);
    }
    
    
    private void midPoint(Point2D p,Point2D q,Point2D r)
    {
        r.setLocation((p.getX()+q.getX())/2.0, (p.getY()+q.getY())/2.0);
    }
    
    
    
    public void draw(Graphics g)
    {
        if (arc_path == null) layout();
        
        Graphics2D g2 = (Graphics2D) g;
        
        //******NOTE******
        //the following is temporary test code to demonstrate the ability of 
        //the GUI arrows to change colors. it will be replaced with an actual
        //formula, or perhaps a mutator method, depending on the evenual
        //implementation details.

        java.awt.Color load = new java.awt.Color((float)0.4,(float)0.4,(float)0.9);
        //java.awt.Color load = new java.awt.Color((float)java.lang.Math.random(),(float)java.lang.Math.random(),(float)java.lang.Math.random());
        
        g2.setPaint(load);
        g2.setStroke(arc_stroke);
        g2.draw(arc_path);
        
        if (source_end != null) {
            source_end.draw(g,arc_points[0],getArrowPoint(true),load);
        }
        if (target_end != null) {
            int ct = arc_points.length-1;
            target_end.draw(g,arc_points[ct],getArrowPoint(false),load);
        }
    }
    
    
    
    /********************************************************************************/
    /*										*/
    /*	Correlation methods							*/
    /*										*/
    /********************************************************************************/
    
    public boolean contains(Point p)
    {
        if (arc_path == null) return false;
        
        Point2D cpt = new Point2D.Double();
        
        double x = getClosestPoint(p,cpt);
        
        return p.distance(cpt) <= CORRELATE_SIZE;
        
	/*****
              Rectangle r = new Rectangle(p.x-CORRELATE_SIZE/2,p.y-CORRELATE_SIZE/2,
              CORRELATE_SIZE,CORRELATE_SIZE);
              return arc_path.intersects(r);
	*****/
    }
    
    
    
    /********************************************************************************/
    /*										*/
    /*	Relative point location methods 					*/
    /*										*/
    /********************************************************************************/
    
    public Point2D getRelativePoint(double pos)
    {
        if (arc_path == null) layout();
        
        AffineTransform at = new AffineTransform();
        double [] pts = new double[6];
        Point2D.Double cpt = new Point2D.Double();
        Point2D.Double npt = new Point2D.Double();
        
        double len = 0;
        for (PathIterator pi = arc_path.getPathIterator(at,1.0); !pi.isDone(); pi.next()) {
            int pt = pi.currentSegment(pts);
            int j = -1;
            switch (pt) {
            case PathIterator.SEG_MOVETO :
                cpt.setLocation(pts[0],pts[1]);
                break;
            case PathIterator.SEG_LINETO :
                j = 0;
                break;
            case PathIterator.SEG_QUADTO :
                j = 2;
                break;
            case PathIterator.SEG_CUBICTO :
                j = 4;
                break;
            default :
                break;
            }
            if (j >= 0) {
                len += cpt.distance(pts[j],pts[j+1]);
                cpt.setLocation(pts[j],pts[j+1]);
            }
        }
        
        double apos = len * pos;
        double nlen = 0;
        for (PathIterator pi = arc_path.getPathIterator(at,1.0); !pi.isDone(); pi.next()) {
            int pt = pi.currentSegment(pts);
            int j = -1;
            nlen = 0;
            switch (pt) {
            case PathIterator.SEG_MOVETO :
                cpt.setLocation(pts[0],pts[1]);
                break;
            case PathIterator.SEG_LINETO :
                j = 0;
                break;
            case PathIterator.SEG_QUADTO :
                j = 2;
                break;
            case PathIterator.SEG_CUBICTO :
                j = 4;
                break;
            default :
                break;
            }
            if (j >= 0) {
                npt.setLocation(pts[j],pts[j+1]);
                nlen = cpt.distance(npt);
                if (nlen >= apos) break;
                apos -= nlen;
                cpt.setLocation(npt);
            }
        }
        
        if (nlen > 0) {
            double ax = cpt.x + apos/nlen * (npt.x - cpt.x);
            double ay = cpt.y + apos/nlen * (npt.y - cpt.y);
            cpt.setLocation(ax,ay);
        }
        
        return cpt;
    }
    
    
    
    
    public double getClosestPoint(Point2D loc,Point2D rslt)
    {
        AffineTransform at = new AffineTransform();
        double [] pts = new double[6];
        Point2D.Double cpt = new Point2D.Double();
        Point2D.Double npt = new Point2D.Double();
        Point2D.Double pt0 = null;
        Point2D.Double pt1 = null;
        double dmin = 0;
        double lmin = 0;
        
        if (arc_path == null) layout();
        
        double len = 0;
        for (PathIterator pi = arc_path.getPathIterator(at,1.0); !pi.isDone(); pi.next()) {
            int pt = pi.currentSegment(pts);
            int j = -1;
            switch (pt) {
            case PathIterator.SEG_MOVETO :
                cpt.setLocation(pts[0],pts[1]);
                break;
            case PathIterator.SEG_LINETO :
                j = 0;
                break;
            case PathIterator.SEG_QUADTO :
                j = 2;
                break;
            case PathIterator.SEG_CUBICTO :
                j = 4;
                break;
            default :
                break;
            }
            if (j >= 0) {
                npt.setLocation(pts[j],pts[j+1]);
                double d = Line2D.ptSegDistSq(cpt.x,cpt.y,npt.x,npt.y,loc.getX(),loc.getY());
                if (pt0 == null || d < dmin) {
                    dmin = d;
                    lmin = len;
                    pt0 = new Point2D.Double(cpt.x,cpt.y);
                    pt1 = new Point2D.Double(npt.x,npt.y);
                }
                len += cpt.distance(npt);
                cpt.setLocation(npt);
            }
        }
        
        if (pt0 == null) {
            if (rslt != null) rslt.setLocation(cpt.x,cpt.y);
            return 0;
        }
        
        double x1,y1;
        
        double f = pt1.x - pt0.x;
        double g = pt1.y - pt0.y;
        double fsq = f*f;
        double gsq = g*g;
        double fgsq = fsq+gsq;
        if (fgsq < 0.0000001) {
            x1 = pt1.x;
            y1 = pt1.y;
        }
        else {
            double xj0 = loc.getX() - pt0.x;
            double yj0 = loc.getY() - pt0.y;
            double tj = (f * xj0 + g * yj0)/fgsq;
            if (tj < 0) tj = 0;
            if (tj > 1) tj = 1;
            x1 = pt0.x + tj*(pt1.x - pt0.x);
            y1 = pt0.y + tj*(pt1.y - pt0.y);
        }
        
        double apos = 0;
        
        if (len > 0) {
            double len1 = Point2D.distance(pt0.x,pt0.y,x1,y1);
            apos = (lmin + len1)/len;
        }
        
        if (rslt != null) rslt.setLocation(x1,y1);
        
        return apos;
    }
    
    
    
    public Point2D getArrowPoint(boolean start)
    {
        AffineTransform at = new AffineTransform();
        double [] pts = new double[6];
        Point2D.Double cpt = new Point2D.Double();
        Point2D.Double npt = new Point2D.Double();
        
        if (arc_path == null) layout();
        
        int ct = 0;
        
        for (PathIterator pi = arc_path.getPathIterator(at,1.0); !pi.isDone(); pi.next()) {
            int pt = pi.currentSegment(pts);
            int j = -1;
            switch (pt) {
            case PathIterator.SEG_MOVETO :
                npt.setLocation(pts[0],pts[1]);
                break;
            case PathIterator.SEG_LINETO :
                j = 0;
                break;
            case PathIterator.SEG_QUADTO :
                j = 2;
                break;
            case PathIterator.SEG_CUBICTO :
                j = 4;
                break;
            default :
                break;
            }
            if (j >= 0) {
                cpt.setLocation(npt);
                npt.setLocation(pts[j],pts[j+1]);
                if (start) return npt;
            }
        }
        
        return cpt;
    }
    
    
    
    
    private int getPreviousPivot(Point p)
    {
        Rectangle2D r = new Rectangle(p.x-1,p.y-1,3,3);
        AffineTransform at = new AffineTransform();
        double [] pts = new double[6];
        Point2D.Double npt = new Point2D.Double();
        Line2D.Double ln = new Line2D.Double();
        QuadCurve2D.Double qc = new QuadCurve2D.Double();
        CubicCurve2D.Double cc = new CubicCurve2D.Double();
        
        if (arc_path == null) layout();
        
        int ct = 0;
        int rslt = -1;
        for (PathIterator pi = arc_path.getPathIterator(at); rslt < 0 && !pi.isDone(); pi.next()) {
            int pt = pi.currentSegment(pts);
            int j = -1;
            switch (pt) {
            case PathIterator.SEG_MOVETO :
                npt.setLocation(pts[0],pts[1]);
                break;
            case PathIterator.SEG_LINETO :
                ln.setLine(npt.x,npt.y,pts[0],pts[1]);
                if (ln.intersects(r)) rslt = ct;
                j = 0;
                break;
            case PathIterator.SEG_QUADTO :
                qc.setCurve(npt.x,npt.y,pts[0],pts[1],pts[2],pts[3]);
                if (qc.intersects(r)) rslt = ct;
                j = 2;
                break;
            case PathIterator.SEG_CUBICTO :
                cc.setCurve(npt.x,npt.y,pts[0],pts[1],pts[2],pts[3],pts[4],pts[5]);
                if (cc.intersects(r)) rslt = ct;
                j = 4;
                break;
            default :
                break;
            }
            if (j >= 0) {
                npt.setLocation(pts[j],pts[j+1]);
                ++ct;
            }
        }
        
        if (rslt >= 0) {
            if (rslt > 0) --rslt;			// eliminate dummy first segment
            if (rslt > arc_pivots.size()) --rslt;	// eliminate dummy last segment
        }
        
        return rslt;
    }
    
    
    
    
    /********************************************************************************/
    /*										*/
    /*	Pivot methods								*/
    /*										*/
    /********************************************************************************/
    
    public void clearPivots()
    {
        if (arc_pivots == null) return;
        
        arc_pivots = null;
    }
    
    
    
    public Point [] getPivots()
    {
        if (arc_pivots == null) return null;
        
        int ct = arc_pivots.size();
        if (ct == 0) return null;
        
        Point [] pvts = new Point[ct];
        arc_pivots.copyInto(pvts);
        
        return pvts;
    }
    
    
    
    public boolean overPivot(Point p)
    {
        if (arc_pivots == null) return false;
        
        for (int i = 0; i < arc_pivots.size(); ++i) {
            Point pv = (Point) arc_pivots.elementAt(i);
            double d = pv.distance(p);
            if (d <= PIVOT_CORRELATE_SIZE/2) {
                return true;
            }
        }
        
        return false;
    }
    
    
    
    
    public int createPivot(Point p)
    {
        int rslt = -1;
        
        if (arc_pivots == null) {
            arc_pivots = new Vector();
            arc_pivots.addElement(p);
            rslt = 0;
        }
        else {
            for (int i = 0; i < arc_pivots.size(); ++i) {
                Point pv = (Point) arc_pivots.elementAt(i);
                double d = pv.distance(p);
                if (d <= PIVOT_CORRELATE_SIZE/2) {
                    rslt = i;
                    break;
                }
            }
            if (rslt < 0) {
                rslt = getPreviousPivot(p);
                if (rslt >= 0) arc_pivots.insertElementAt(p,rslt);
            }
        }
        
        layout();
        
        return rslt;
    }
    
    
    
    public boolean movePivot(int idx,Point p)
    {
        if (arc_pivots == null) arc_pivots = new Vector();
        if (arc_pivots.size() == idx) arc_pivots.addElement(p);
        else if (arc_pivots.size() < idx) return false;
        else arc_pivots.setElementAt(p,idx);
        
        layout();
        
        if (!spline_arc) {
            Point p0 = arc_points[idx];
            Point p1 = arc_points[idx+2];
            double d = Line2D.ptSegDist(p0.x,p0.y,p1.x,p1.y,p.x,p.y);
            if (d <= PIVOT_CORRELATE_SIZE/2) return false;
        }
        else {
            for (int i = 0; i < arc_points.length; ++i) {
                double d = p.distance(arc_points[i]);
                if (i != idx+1 && d <= PIVOT_CORRELATE_SIZE/2) return false;
            }
        }
        
        return true;
    }
    
    
    
    public void removePivot(int idx)
    {
        if (arc_pivots.size() <= 1) arc_pivots = null;
        else arc_pivots.removeElementAt(idx);
        
        layout();
    }
}

