
package edu.brown.aurora.gui;

import edu.brown.bloom.petal.*;

import javax.swing.*;
import javax.swing.border.*;
import java.awt.event.*;
import java.awt.*;

import java.util.*;
import javax.swing.event.*;
import edu.brown.aurora.gui.types.*;

/**
 * A properties dialog for generic nodes.  Implements basic OK, APPLY, and CANCEL
 * functionality of properties dialog boxes.  Meant to be subclassed.
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu) and Robin Yan (rly@cs.brown.edu)
 * @version 1.0 04/10/2002
 */

public abstract class GenericNodePropertiesDialog extends JDialog implements ActionListener
{
    /**
     * The generic node associated with this properties dialog box.
     */
    protected GenericNode node;

    /**
     * The tabbed pane that contains multiple panes for different functionality.
     */
    protected JTabbedPane tabbedPane;

    /**
     * Constructs a new GenericNodePropertiesDialog object.  Sets the layout for the
     * tabbedPane and the ok/apply/cancel buttons at the bottom.
     *
     * @param node The node to be described by this properties dialog object.
     */
    public GenericNodePropertiesDialog(GenericNode node) {

        this.node = node;
        setTitle("Properties");
        setSize(600, 500);
	setResizable(true);
        setModal(true);
        setLocationRelativeTo(node.getComponent());

        getContentPane().setLayout(new BorderLayout(50,50));

        JPanel tabPanel = new JPanel(new GridLayout(1,1));
        tabbedPane = new JTabbedPane();

        // Buttons section
        JPanel buttonsPanel = new JPanel();
        JButton apply = new JButton("Apply");
        apply.addActionListener(this);
        JButton ok = new JButton("OK");
        ok.addActionListener(this);
        JButton cancel = new JButton("Cancel");
        cancel.addActionListener(this);
        buttonsPanel.setLayout(new GridLayout(1,4,5,10));
        buttonsPanel.add(new Panel());
        buttonsPanel.add(apply);
        buttonsPanel.add(ok);
        buttonsPanel.add(cancel);

        tabPanel.add(tabbedPane);
        getContentPane().add(tabPanel, BorderLayout.CENTER);
        getContentPane().add(buttonsPanel, BorderLayout.SOUTH);
    }

    /**
     * A generic helper method for adding components in a GridBagLayout layout.
     * gridx, gridy, gridwidth, gridheight, weightx, and weighty are GridBagConstraints
     * arguments.
     * @see java.awt.GridBagConstraints
     *
     * @param bags The GridBagLayout object passed into the parent container object
     * @param parent The parent container object
     * @param child The child container object to add into the parent
     * @param gridx The gridx value.
     * @param gridy The gridy value.
     * @param gridwidth The gridwidth value.
     * @param gridheight The gridheight value.
     * @param weightx The weightx value.
     * @param weighty The weighty value.
     */
    protected void addComponent(GridBagLayout bags,
                                JComponent parent, JComponent child,
                                int gridx, int gridy,
                                int gridwidth, int gridheight,
                                double weightx, double weighty)
    {
        GridBagConstraints c = new GridBagConstraints(gridx, gridy,
                                                      gridwidth, gridheight,
                                                      weightx, weighty,
                                                      GridBagConstraints.CENTER,
                                                      GridBagConstraints.BOTH,
                                                      new Insets(0,0,0,0),
                                                      0,0);
        bags.setConstraints(child, c);
        parent.add(child);
    }

    /**
     * A generic helper method for adding components in a GridBagLayout layout.
     * gridx, gridy, gridwidth, gridheight, weightx, weighty, and inset are GridBagConstraints
     * arguments.
     * @see java.awt.GridBagConstraints
     *
     * @param bags The GridBagLayout object passed into the parent container object
     * @param parent The parent container object
     * @param child The child container object to add into the parent
     * @param gridx The gridx value.
     * @param gridy The gridy value.
     * @param gridwidth The gridwidth value.
     * @param gridheight The gridheight value.
     * @param weightx The weightx value.
     * @param weighty The weighty value.
     * @param inset The insets value
     */
    protected void addComponent(GridBagLayout bags,
                                JComponent parent, JComponent child,
                                int gridx, int gridy,
                                int gridwidth, int gridheight,
                                double weightx, double weighty,
				Insets inset )
    {
        GridBagConstraints c = new GridBagConstraints(gridx, gridy,
                                                      gridwidth, gridheight,
                                                      weightx, weighty,
                                                      GridBagConstraints.CENTER,
                                                      GridBagConstraints.BOTH,
                                                      inset,
                                                      0,0);
        bags.setConstraints(child, c);
        parent.add(child);
    }
}
