package edu.brown.aurora.gui;

import edu.brown.bloom.petal.*;
import edu.brown.bloom.swing.*;


import javax.swing.*;
import javax.swing.border.*;
import javax.swing.tree.*;
import java.awt.event.*;
import java.awt.*;

import java.util.*;

public class HierarchyNode extends DefaultMutableTreeNode
{
    BoxNode node;

    public HierarchyNode(BoxNode node)
    {
        super(node.getLabel());
        this.node = node;
    }

    public BoxNode getNode()
    {
        return node;
    }
}
