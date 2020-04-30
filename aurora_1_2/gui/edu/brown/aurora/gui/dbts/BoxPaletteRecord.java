package edu.brown.aurora.gui.dbts;

import edu.brown.aurora.gui.*;
import com.sleepycat.db.*;
import java.io.*;
import java.util.Vector;

public class BoxPaletteRecord extends BoxRecord implements PaletteRecord
{
    public static String databaseFileName = "BoxPaletteTable.db";

    protected int rootBoxId;

    protected Vector internalPaletteRecords;

    public BoxPaletteRecord()
    {
        super(); // tell Db to allocate on retrieval
        internalPaletteRecords = new Vector();
    }

    public BoxPaletteRecord(int boxId, int boxType, String label, String description,
                     String modifier, int parentId,
                     float cost, float selectivity,
                     int x, int y, int width, int height, int rootBoxId)
    {
        this.boxId = boxId;
        this.boxType = boxType;
        this.label = label;
        this.description = description;
        this.modifier = modifier;
        this.parentId = parentId;
        this.cost = cost;
        this.selectivity = selectivity;
        this.x = x;
        this.y = y;
        this.width = width;
        this.height = height;
        this.rootBoxId = rootBoxId;

        internalPaletteRecords = new Vector();
    }

    public Object clone()
    {
        return new BoxPaletteRecord(boxId, boxType, label, description, modifier,
                                    parentId, cost, selectivity, x, y,
                                    width, height, rootBoxId);
    }

    protected void writeFields(DataOutputStream outStream)
        throws Exception
    {
        super.writeFields(outStream);
        outStream.writeInt(rootBoxId);
    }

    protected void readFields(DataInputStream inputStream) throws Exception
    {
        super.readFields(inputStream);
        rootBoxId = inputStream.readInt();
    }

    public int getId()
    {
        return boxId;
    }

    public int getRootBoxId()
    {
        return rootBoxId;
    }

    public void addPaletteRecord(PaletteRecord record)
    {
        internalPaletteRecords.addElement(record);
    }

    public Vector getPaletteRecords()
    {
        return internalPaletteRecords;
    }

    public String toString()
    {
        return label;
    }

    public GenericNode getNode(Model model) throws Throwable
    {
        if(boxType == Constants.FILTER) {
            return FilterBoxNode.loadRecord(this, model);
        } else if (boxType == Constants.MAP) {
            return MapBoxNode.loadRecord(this, model);
        } else if (boxType == Constants.AGGREGATE) {
            return AggregateBoxNode.loadRecord(this, model);
        } else if (boxType == Constants.UPDATE_RELATION) {
            return UpdateRelationBoxNode.loadRecord(this, model);
        } else if (boxType == Constants.READ_RELATION) {
            return ReadRelationBoxNode.loadRecord(this, model);
        } else if (boxType == Constants.BSORT) {
            return BsortBoxNode.loadRecord(this, model);
        } else if (boxType == Constants.UNION) {
            return UnionBoxNode.loadRecord(this, model);
        } else if (boxType == Constants.RESTREAM) {
            return RestreamBoxNode.loadRecord(this, model);
        } else if (boxType == Constants.RESAMPLE) {
            return ResampleBoxNode.loadRecord(this, model);
        } else if (boxType == Constants.JOIN) {
            return JoinBoxNode.loadRecord(this, model);
        } else if (boxType == Constants.DROP) {
            return DropBoxNode.loadRecord(this, model);
        } else if (boxType == Constants.SUPERBOX) {
            return SuperBoxNode.loadRecord(this, model);
        } else if (boxType == Constants.INPUTPORT) {
            return InputPortNode.loadRecord(this, model);
        } else if (boxType == Constants.OUTPUTPORT) {
            return OutputPortNode.loadRecord(this, model);
        } else {
          return UnknownBoxNode.loadRecord(this, model);
        }
    }

}
