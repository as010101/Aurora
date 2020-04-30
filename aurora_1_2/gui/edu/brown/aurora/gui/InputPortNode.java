
package edu.brown.aurora.gui;

import edu.brown.bloom.petal.*;
import edu.brown.aurora.gui.types.*;
import edu.brown.aurora.gui.dbts.*;

import javax.swing.*;
import javax.swing.border.*;
import java.awt.event.*;
import java.awt.*;

import java.io.*;

import java.util.*;

import com.sleepycat.db.*;

/**
 * InputPortNode implements the input port on the workspace of the GUI.  
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu) and Robin Yan (rly@cs.brown.edu)
 * @version 1.0 04/10/2002
 */
public class InputPortNode extends GenericPortNode
{
    /**
     * The superbox's output port node associated with this InputPortNode.
     */
    protected BoxNodeOutputPort outputPortNode;

    /**
     * Whether this is periodic or not.
     */
    protected boolean isPeriodic;

    /**
     * Whether this is adjustable or not.
     */
    protected boolean isAdjustable;

    /**
     * The number of tuples of this port.
     */
    protected int numberOfTuples;
    
    /**
     * The number of streams of this port.
     */
    protected int numberOfStreams;
    

    protected boolean useDefinedTypes;

    /**
     * Whether this InputPortNode uses the Workload generator
     */
    protected boolean useWorkload;

    /**
     * The FieldSet for this InputPortNode if uses Workload Generator
     */
    protected FieldSet fields;


    protected float rate;

    /**
     * Constructs a new InputPortNode object.
     *
     * @param boxId The unique int identifier of this InputPortNode.
     * @param m The containing model of that this InputPortNode belongs to.
     * @param index The index of this InputPortNode within its container.
     * @param outputPortNode The outputPortNode of the containing superbox associated
     * with this InputPortNode.
     * @param rate The rate for this InputPortNode.
     * @param isPeriodic Sets whether this PortNode is periodic or not.
     * @param isAdjustable Sets whether this PortNode is  adjustable or not.
     * @param numberOfTuplesSets the number of tuples for this InputPortNode.
     * @param useDefinedTypes Sets whether this inputport node is using defined types or not.
     */
    public InputPortNode(int boxId, int typeId, Model m, int parentId,
                         int index, BoxNodeOutputPort outputPortNode, 
                         float rate, boolean isPeriodic, 
                         boolean isAdjustable, int numberOfTuples, boolean useDefinedTypes)
    {
        super(boxId, m, parentId);
        setIndex(index);
        setType(RootFrame.typeManager.findCompositeType(index));
        this.outputPortNode = outputPortNode;
	this.rate = rate;
        this.isPeriodic = isPeriodic;
        this.isAdjustable = isAdjustable;
        this.numberOfTuples = numberOfTuples;
    	this.useDefinedTypes = useDefinedTypes;
        //read these in eventually
        this.numberOfStreams = 1;
    	this.useWorkload = false;
	outputPortNode.setGenericNode(this);
	setColor();

    }

    /**
     * Returns true if this node gets its box Id set in the lower section of the box id
     * range.
     */
    public boolean isInLowerBoxIds()
    {
        return parentId == Constants.ROOTBOXID;
    }


    public String getToolTip(Point at) 
    {
	if(getType() == null) {
	    return "Undefined input type";
	}
	return getType().toStringInDetail();
    }

    /**
     * Gets the modifier for the inputportnode
     */
    public String getModifier()
    {
        String modifier = 
            getTypeId() + Constants.DELIM + 
            getRate() + Constants.DELIM +
            (isPeriodic()?1:0) + Constants.DELIM +
            (isAdjustable()?1:0) + Constants.DELIM +
	    (isUseDefinedTypes()?1:0) + Constants.DELIM +
            getNumberOfTuples();
            
        return modifier;
    }




    /**
     * Returns an InputPortNode object that is loaded using the information from
     * a database table in sleepycat.
     *
     * @param record the record containing the generic port node information to load.
     * @param sourceStreamTable the table containing the input port node-specific
     * source stream information.
     * @model The model that contains this input port node.
     * @return A new input port node object corresponding to the information in
     * the database.
     */
     
    public static InputPortNode loadRecord(BoxRecord record,
                                           Model model) throws Throwable
    {
        StringTokenizer tokens = new StringTokenizer(record.getModifier(), Constants.DELIM);

        int typeId = Integer.parseInt(tokens.nextToken());
        float rate = Float.parseFloat(tokens.nextToken());
        int periodicity = Integer.parseInt(tokens.nextToken());
        int adjustability = Integer.parseInt(tokens.nextToken());
	boolean isUseDefinedTypes = Integer.parseInt(tokens.nextToken()) == 1;
        int numberOfTuples = Integer.parseInt(tokens.nextToken());

        BoxNodeOutputPort boxNodeOutputPort = new BoxNodeOutputPort(PetalPort.EAST,
                                                                    new JPanel(), 0,
                                                                    null);

        InputPortNode node = new InputPortNode(record.getBoxId(),
                                               typeId,
                                               model,
                                               record.getParentId(),
                                               model.getNumberOfInputPorts(),
                                               boxNodeOutputPort,
                                               rate,
                                               periodicity != 0,
                                               adjustability != 0,
                                               numberOfTuples,
					       record.isUseDefinedTypes());
        
        int index = typeId;
        if (index >= 0) {
            try {
                CompositeType types = RootFrame.typeManager.getAllCompositeTypes()[index];
                node.setType(types);
            } catch (Exception e) {
                System.out.println(e.toString());
                e.printStackTrace();
            }
        }
        return node;
    }
    /**
     * Returns an InputPortNode object that is loaded using the information from
     * a database table in sleepycat.
     *
     * @param record the record containing the generic port node information to load.
     * @param sourceStreamTable the table containing the input port node-specific
     * source stream information.
     * @model The model that contains this input port node.
     * @return A new input port node object corresponding to the information in
     * the database.
     */
     
    public static InputPortNode loadRecord(BoxPaletteRecord record,
                                           Model model) throws Throwable
    {
        int boxId = record.getBoxId();	

	// After some thought, it's pretty obvious that any input port
	// going into the palette must not be in the root superbox.  So
	// we should never be setting the box id to the lowerId sets. But
	// I'm putting it in here just to be complete.

        if(boxId <= Constants.MAX_STREAM_BOX_ID) {
            boxId = setNextLowerIdCounter();
        } else {
            boxId = setNextUpperIdCounter();
        }

        float rate = 0;
        int typeId = -1;
        int periodicity = 0;
        int adjustability = 0;
        int numberOfTuples = 0;

        if(!record.getModifier().equals("")) {
            StringTokenizer tokens = new StringTokenizer(record.getModifier(), 
                                                         Constants.DELIM);
            
            typeId = Integer.parseInt(tokens.nextToken());
            rate = Float.parseFloat(tokens.nextToken());
            periodicity = Integer.parseInt(tokens.nextToken());
            adjustability = Integer.parseInt(tokens.nextToken());
            numberOfTuples = Integer.parseInt(tokens.nextToken());
        }

        BoxNodeOutputPort boxNodeOutputPort = new BoxNodeOutputPort(PetalPort.EAST,
                                                                    new JPanel(), 0,
                                                                    null);
	
        InputPortNode node = new InputPortNode(boxId,
                                               typeId,
                                               model,
                                               record.getParentId(),
                                               model.getNumberOfInputPorts(),
                                               boxNodeOutputPort,
                                               rate,
                                               periodicity != 0,
                                               adjustability != 0,
                                               numberOfTuples,
					       record.isUseDefinedTypes());
        
        int index = typeId;
        if (index >= 0) {
            try {
                CompositeType types = RootFrame.typeManager.getAllCompositeTypes()[index];
                node.setType(types);
            } catch (Exception e) {
                System.out.println(e.toString());
                e.printStackTrace();
            }
        }
        return node;
    }

    public void loadWGData(String catalogDir)
    {
        File wgData = new File(catalogDir+"/wg/wg_"+boxId+".wgf");
        if (wgData.exists()) {
            useWorkload = true;
            fields = new FieldSet();
            try {
                FileInputStream wgIn = new FileInputStream(wgData);
                InputStreamReader wgRead = new InputStreamReader(wgIn);
                BufferedReader wgBReader = new BufferedReader(wgRead);

                String temp;
                int j, k, maxJ, maxK, numAttrs;
                //id
                temp = wgBReader.readLine();
                //string id
                temp = wgBReader.readLine();
                //number of streams
                temp = wgBReader.readLine();
                numberOfStreams = Integer.parseInt(temp);
                //number of tuples
                temp = wgBReader.readLine();
                numberOfTuples = Integer.parseInt(temp);

                /*rate information*/
                //type
                temp = wgBReader.readLine();
                fields.setDistributionType(Integer.parseInt(temp));
                //num parameters
                temp = wgBReader.readLine();
                maxJ = Integer.parseInt(temp);

                //loop over params
                for (j=0; j<maxJ; ++j) {
                    DistributionSet ds = new DistributionSet();
                    //dist type
                    temp = wgBReader.readLine();
                    ds.setDistributionType(Integer.parseInt(temp));
                    //num params
                    temp = wgBReader.readLine();
                    maxK = Integer.parseInt(temp);
                    //loop over params
                    for (k=0; k<maxK; ++k) {
                        //param value
                        temp = wgBReader.readLine();
                        ds.setParameter(k, Double.parseDouble(temp));
                    }
                    fields.setDistributionParam(j,ds);
                }

                /*attributes*/
                //num attributes
                temp = wgBReader.readLine();
                numAttrs = Integer.parseInt(temp);
                fields.setNumFields(numAttrs);
                //loop over attrs
                for (int i=0; i<numAttrs; ++i) {
                    String name;
                    int type, length;
                    //data type
                    temp = wgBReader.readLine();
                    type = Integer.parseInt(temp);
                    //data name
                    temp = wgBReader.readLine();
                    name = temp.substring(1,temp.length());
                    //data length
                    temp = wgBReader.readLine();
                    length = Integer.parseInt(temp);
                    Field f = new Field(-(type+1),name,length);
                    //dist type
                    temp = wgBReader.readLine();
                    f.setVariance(Integer.parseInt(temp));
                    //num params
                    temp = wgBReader.readLine();
                    maxJ = Integer.parseInt(temp);
                    //loop over params
                    for (j=0; j<maxJ; ++j) {
                        DistributionSet fds = new DistributionSet();
                        //dist type
                        temp = wgBReader.readLine();
                        fds.setDistributionType(Integer.parseInt(temp));
                        //num params
                        temp = wgBReader.readLine();
                        maxK = Integer.parseInt(temp);
                        //loop over params
                        for (k=0; k<maxK; ++k) {
                            //param value
                            temp = wgBReader.readLine();
                            fds.setParameter(k, Double.parseDouble(temp));
                        }
                        f.setParameter(j,fds);
                    }
                    fields.setField(i,f);
                }

                wgIn.close();
                wgRead.close();
                wgBReader.close();
            } catch (Exception e) {
                System.err.println("Error: " + e);
            }
        }
    }

    public void saveWGData(String catalogDir)
    {
        if (useWorkload) {
            try {
                File wgFolder = new File(catalogDir+"/wg/");
                wgFolder.mkdir();
            } catch (Exception e) {
                System.err.println("Error: " + e);
            }
            try {
                File wgData = new File(catalogDir+"/wg/wg_"+boxId+".wgf");
                FileOutputStream wgDataOut = new FileOutputStream(wgData);
                OutputStreamWriter wgWrite = new OutputStreamWriter(wgDataOut);
                BufferedWriter dosWgData = new BufferedWriter(wgWrite);

                int j, k, maxJ, maxK;
                double temp;
                String sOut;

                //id
                sOut = "" + boxId + "\n";
                dosWgData.write(sOut,0,sOut.length());
                //string id
                sOut = ("input" + boxId + "\n");
                dosWgData.write(sOut,0,sOut.length());
                //number of streams
                sOut = ("" + numberOfStreams + "\n");
                dosWgData.write(sOut,0,sOut.length());
                //number of tuples
                sOut = ("" + numberOfTuples + "\n");
                dosWgData.write(sOut,0,sOut.length());

                /*rate information*/
                //type
                int dtype = fields.getDistributionType();
                sOut = ("" + dtype + "\n");
                dosWgData.write(sOut,0,sOut.length());
                //num parameters
                maxJ = Constants.VAR_PARAM_NUM[dtype];
                sOut = ("" + maxJ + "\n");
                dosWgData.write(sOut,0,sOut.length());

                //loop over params
                for (j=0; j<maxJ; ++j) {
                    DistributionSet dist = fields.getDistributionParam(j);
                    //dist type
                    temp = dist.getDistributionType();
                    sOut = ("" + (int)(temp+0.5) + "\n");
                    dosWgData.write(sOut,0,sOut.length());
                    //num params
                    maxK = Constants.VAR_PARAM_NUM[(int)(temp+0.5)];
                    sOut = ("" + maxK + "\n");
                    dosWgData.write(sOut,0,sOut.length());
                    //loop over params
                    for (k=0; k<maxK; ++k) {
                        //param value
                        temp = dist.getParameter(k);
                        sOut = ("" + temp + "\n");
                        dosWgData.write(sOut,0,sOut.length());
                    }
                }

                /*attributes*/
                //num attributes
                int numAttrs = fields.getNumFields();
                sOut = ("" + numAttrs + "\n");
                dosWgData.write(sOut,0,sOut.length());
                //loop over attrs
                for (int i=0; i<numAttrs; ++i) {
                    Field f = fields.getField(i);
                    //data type
                    temp = (-(f.getTypeId()))-1;
                    sOut = ("" + (int)(temp+0.5) + "\n");
                    dosWgData.write(sOut,0,sOut.length());
                    //data name
                    sOut = ("%" + f.getID() + "\n");
                    dosWgData.write(sOut,0,sOut.length());
                    //data length
                    temp = f.getLength();
                    sOut = ("" + (int)(temp+0.5) + "\n");
                    dosWgData.write(sOut,0,sOut.length());
                    //dist type
                    temp = f.getVariance();
                    sOut = ("" + (int)(temp+0.5) + "\n");
                    dosWgData.write(sOut,0,sOut.length());
                    //num params
                    maxJ = Constants.VAR_PARAM_NUM[(int)(temp+0.5)];
                    sOut = ("" + maxJ + "\n");
                    dosWgData.write(sOut,0,sOut.length());
                    //loop over params
                    for (j=0; j<maxJ; ++j) {
                        DistributionSet fds = f.getParameter(j);
                        //dist type
                        temp = fds.getDistributionType();
                        sOut = ("" + (int)(temp+0.5) + "\n");
                        dosWgData.write(sOut,0,sOut.length());
                        //num params
                        maxK = Constants.VAR_PARAM_NUM[(int)(temp+0.5)];
                        sOut = ("" + maxK + "\n");
                        dosWgData.write(sOut,0,sOut.length());
                        //loop over params
                        for (k=0; k<maxK; ++k) {
                            //param value
                            temp = fds.getParameter(k);
                            sOut = ("" + temp + "\n");
                            dosWgData.write(sOut,0,sOut.length());
                        }
                    }
                }

                //wgDataOut.close();
                //wgWrite.close();
                dosWgData.flush();
                dosWgData.close();
            } catch (Exception e) {
                System.err.println("Error: " + e);
            }
        }
    }

    /**
     * Returns a String representation of this InputPortNode.
     *
     * @return a String representation of this InputPortNode.
     */
    public String toString()
    {
        return 
            "InputPortNode : " +
            " boxId=" + boxId +
            " parent=" + parentId +
            " typeId="+getTypeId()+
            " rate="+getRate()+
            " isPeriodic="+ (isPeriodic()?1:0)+
            " isAdjustable=" + (isAdjustable()?1:0) +
            " #tuples=" + numberOfTuples;
    }


    
    /**
     * Sets the index of the this InputPortNode, as well as the title
     * accordingly.
     *
     * @param index The index to set this InputPortNode to.
     */
    public void setIndex(int index)
    {
        super.setIndex(index);
        setLabel("i"+index);
    }

    /**
     * Returns whether this port uses defined types.  
     * InputPortNodes should <b>always</b> specify
     * their incoming tuple types.
     */
    public boolean isUseDefinedTypes()
    {
        return useDefinedTypes;
    }

    /**
     * Set whether this InputPortNode uses definded types
     * @param useDefinedTypes whether to use defined types
     */
    public void setUseDefinedTypes(boolean useDefinedTypes)
    {
        this.useDefinedTypes = useDefinedTypes;
    }
    
    /**
     * Returns whether this InputPortNode uses the Workload Generator
     * @return usesWorkload 
     */
    public boolean usesWorkload()
    {
        return useWorkload;
    }

    /**
     * Set whether this InputPortNode uses the Workload Generator
     * @param usesWorkload whether to use defined types
     */
    public void setUsesWorkload(boolean usesWorkload)
    {
        if (useWorkload!=usesWorkload) {
            useWorkload = usesWorkload;
            if (useWorkload) {
                fields = new FieldSet(outputPortNode.getType());
                /*TODO*/
                numberOfStreams = 1;
                numberOfTuples = -1;
            }
            else {
                fields = null;
            }
        }
    }
    
    /**
     * Returns the current FieldSet, if using the Workload Generator.
     * Else, returns null.
     */
    public FieldSet getFieldSet()
    {
        return fields;
    }
    /**
     * Returns the type defined in this InputPortNode.
     *
     * @return the type defined in this InputPortNode.
     */
    public CompositeType getType()
    {
        if(outputPortNode == null) {
            return null;
        }
        return outputPortNode.getType();
    }

    /**
     * Returns the int value for the type of this box.  Refer to the box type values
     * @see edu.brown.aurora.gui.Constants
     *
     * @return An int value for the type of this box.
     */
    public int getBoxType()
    {
        return Constants.INPUTPORT;
    }

    
    public void setColor()
    {
	if(getType() == null) {
	    lbl.setBackground(Constants.NOMODIFIERBOX_COLOR);
	    lbl.setForeground(Constants.NOMODIFIERPORTNODE_FONT_COLOR);
	} else {
	    lbl.setBackground(Constants.PORTNODE_COLOR);
	    lbl.setForeground(Constants.PORTNODE_FONT_COLOR);
	}
    }

    /**
     * Sets the type of this InputPortNode.
     *
     * @param type The type to set this InputPortNode to.
     */
    public void setType(CompositeType type)
    {
        if(type != null && outputPortNode != null) {
            if (!type.equals(outputPortNode.getType())) {
                fields = new FieldSet(type);
            }
            outputPortNode.setType(type);
        }
        setColor();
    }
    
    /**
     * Sets the outputPortNode of the containing Superclass for this 
     * InputPortNode.
     *
     * @param ipt The outputPortNode.
     */
    public void setOutputPortNode(BoxNodeOutputPort ipt)
    {
        outputPortNode = ipt;
    }

    /**
     * Sets the cardinality of this InputPortNode.
     *
     * @param cardinality The cardinality to set for this InputPortNode.
     */
    public void setCardinality(int cardinality)
    {
        outputPortNode.setCardinality(cardinality);
    }

    /**
     * Returns the BoxNodeOutputPort associated with this InputPortNode.
     *
     * @return The BoxNodeOutputPortNode associated with this InputPortNode.
     */
    public BoxNodeOutputPort getOutputPortNode()
    {
        return outputPortNode;
    }

    /**
     * Returns the cardinality of this InputPortNode.
     *
     * @return The cardinality of this InputPortNode.
     */
    public int getCardinality()
    {
        return outputPortNode.getCardinality();
    }

    /** 
     * Returns the rate of this InputPortNode.
     *
     * @return the rate of this InputPortNode.
     */
    public int getRate()
    {
        if(fields != null) {
            return fields.getDistributionType();
        } else {
            return (int)rate;
        }
    }
    
    /**
     * Returns the type Id
     *
     * @return The type id
     */
    public int getTypeId()
    { 
        return RootFrame.typeManager.findCompositeTypeIndex(getType());
    }

    /**
     * Returns whether this InputPortNode is periodic or not.
     *
     * @return a boolean representing whether this InputPortNode is 
     * periodic or not.
     */
    public boolean isPeriodic()
    {
        return isPeriodic;
    }
    
    /**
     * Returns whether this InputPortNode is adjustable or not.
     * 
     * @return a boolean representing whether this InputPortNode is
     * adjustable or not.
     */
    public boolean isAdjustable()
    {
        return isAdjustable;
    }
    
    /**
     * Returns the number of tuples for this InputPortNode.
     *
     * @return The number of tuples for this InputPortNode.
     */
    public int getNumberOfTuples()
    {
        return numberOfTuples;
    }
    
    /**
     * Returns the number of streams for this InputPortNode.
     *
     * @return The number of streams for this InputPortNode.
     */
    public int getNumberOfStreams()
    {
        return numberOfStreams;
    }
    
    /**
     * Sets the rate.
     *
     * @param the rate to set
     */
    public void setRate(int type)
    {
        rate = type;
        if(fields != null) {
            fields.setDistributionType(type);
        }
    }
    
    /**
     * Sets whether this InputPortNode is periodic or not.
     *
     * @param periodicity A boolean representing whether this 
     * InputPortNode is periodic or not.
     */
    public void setPeriodicity(boolean periodicity)
    {
        isPeriodic = periodicity;
    }
    
    /**
     * Sets whether this InputPortNode is adjustable or not.
     *
     * @param adjustability A boolean representing whether this
     * InputPortNode is periodic or not.
     */
    public void setAdjustability(boolean adjustability)
    {
        isAdjustable = adjustability;
    }
    
    /**
     * Sets the number of tuples for this InputPortNode.
     *
     * @param numberOfTuples An int to seet for the number of tuples.
     */
    public void setNumberOfTuples(int numberOfTuples)
    {
        this.numberOfTuples = numberOfTuples;
    }
    
    /**
     * Sets the number of streams for this InputPortNode.
     *
     * @param numberOfStreams An int to seet for the number of streams.
     */
    public void setNumberOfStreams(int numberOfStreams)
    {
        this.numberOfStreams = numberOfStreams;
    }
    
    /**
     * Opens up the properties dialog box for this GenericPortNode  object.
     *
     */
    public void showProperties()
    {
        if (parentId==Constants.ROOTBOXID) {
            new InputPortPropertiesDialog(this);        
        }
        else {
            new PortNodePropertiesDialog(this);
        }
    }    

}
