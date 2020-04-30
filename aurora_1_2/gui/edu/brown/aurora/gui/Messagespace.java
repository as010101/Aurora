package edu.brown.aurora.gui;

import edu.brown.bloom.petal.*;
import edu.brown.bloom.swing.*;

import javax.swing.*;
import javax.swing.tree.*;
import javax.swing.border.*;
import java.awt.event.*;
import java.awt.*;
import java.awt.dnd.*;
import java.util.*;

import edu.brown.aurora.gui.types.*;

/**
 * A Swing panel containing a tabbedPane of multiple textareas for displaying user interface
 * messages to the user.
 *<br><br>
 * maybe I should make this a singleton class - rly
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu) and Robin Yan (rly@cs.brown.edu)
 * @version 1.0 04/10/2002
 */
public class Messagespace extends JPanel
{
    static JTextArea s_messagesTextArea;
    static JTextArea s_statisticsTextArea;
    static JTextArea s_errorsTextArea;

    transient private PetalEditor petal_editor;

    private Model petal_model;
    private Editor editor;

    /**
     * The label of the errors text area tab.
     */
    static final String ERRORS = "Errors";

    /**
     * The label of the statistics text area tab.
     */
    static final String STATISTICS = "Stats";

    /**
     * The label of the messages text area tab.
     */
    static final String MESSAGES = "Messages";

    /**
     * The unique id of the errors tab.
     */
    public static final int ERRORS_ID = 0;

    /**
     * The unique id of the statistics tab.
     */
    public static final int STATS_ID = 1;

    /**
     * The unique id of the messages tab.
     */
    public static final int MESSAGES_ID = 2;

    /**
     * Constructs a new Messagespace object.
     *
     * @param petal_editor The PetalEditor object to operate on the messagespace.
     * @param petal_model The Model object to operate on the messagespace.
     * @param editor The frame containing this Messagespace object.
     */
    public Messagespace(PetalEditor petal_editor,
                        Model petal_model) {
        super(new GridLayout(1,1));

        this.petal_editor = petal_editor;
        this.petal_model = petal_model;
        this.editor = editor;

        JTabbedPane tabbedPane = new JTabbedPane();
        tabbedPane.setTabPlacement(JTabbedPane.RIGHT);


        s_errorsTextArea = new JTextArea();
        s_errorsTextArea.setRows(Constants.TEXTROWS);
        JPanel errorsPanel = new JPanel(new GridLayout(1,1));
        errorsPanel.add(new JScrollPane(s_errorsTextArea));
        tabbedPane.addTab(ERRORS, errorsPanel);

        s_statisticsTextArea = new JTextArea();
        s_statisticsTextArea.setRows(Constants.TEXTROWS);
        JPanel statisticsPanel = new JPanel(new GridLayout(1,1));
        statisticsPanel.add(new JScrollPane(s_statisticsTextArea));
        tabbedPane.addTab(STATISTICS, statisticsPanel);

        s_messagesTextArea = new JTextArea();
        s_messagesTextArea.setRows(Constants.TEXTROWS);
        JPanel messagesPanel = new JPanel(new GridLayout(1,1));
        messagesPanel.add(new JScrollPane(s_messagesTextArea));
        tabbedPane.addTab(MESSAGES, messagesPanel);

        add(tabbedPane);

        setBorder(new LineBorder(Color.black,1));

        s_errorsTextArea.setEditable(false);
        s_statisticsTextArea.setEditable(false);
        s_messagesTextArea.setEditable(false);

        tabbedPane.setSelectedIndex(MESSAGES_ID);
        
        

    }

    /**
     * Sets the parent editor of this object.
     *
     * @param editor The Editor object to set.
     */
    public void setEditor(Editor editor)
    {
        this.editor = editor;
    }


}
