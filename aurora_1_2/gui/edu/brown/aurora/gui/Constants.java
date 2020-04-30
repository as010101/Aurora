package edu.brown.aurora.gui;

import java.awt.Color;
import java.awt.Font;
import java.awt.Dimension;
import edu.brown.aurora.gui.types.TypeManager;
import javax.swing.*;
import java.net.URL;
import java.awt.Image;
import java.awt.Toolkit;

/**
 * A class that contains generic, public global variables that are accessible by all
 * of the other classes.
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu) and Robin Yan (rly@cs.brown.edu)
 * @version 1.0 04/10/2002
 */
public class Constants
{
    /**
     * The default delimiter that seperates parts of a modifier
     */
    public static final String MODIFIER_DELIM = "~";

    /**
     * The inner delimiter for the modifier
     */
    public static final String INNER_MODIFIER_DELIM = ",";

    /**
     * The default color of unary boxes.
     */
    public static final Color UNARYBOX_COLOR = Color.lightGray;

    /**
     * The default color of binary boxes.
     */
    public static final Color BINARYBOX_COLOR = Color.lightGray;

    public static final Color NOMODIFIERBOX_COLOR = new Color(240,240,150);


    /**
     * The default color of super boxes.
     */
    public static final Color SUPERBOX_COLOR = Color.white;

    /**
     * The default color of input port nodes of the editor.
     */
    public static final Color INPUTPORTNODE_COLOR = Color.darkGray;

    /**
     * The default color of output port nodes of the editor.
     */
    public static final Color OUTPUTPORTNODE_COLOR = Color.darkGray;

    /**
     * The default background color of portnodes of boxes.
     */
    public static final Color PORTNODE_COLOR = Color.darkGray;

    /**
     * The default background color of portnodes when the mouse is over.
     */
    public static final Color PORTNODE_COLOR_OVER = Color.RED;

    /**
     * The default color of portnode font colors of boxes.
     */
    public static final Color NOMODIFIERPORTNODE_FONT_COLOR = Color.black;

    public static final Color PORTNODE_FONT_COLOR = Color.white;

    /**
     * The default font of box labels.
     */
    public static final Font LABEL_FONT = new Font("Sans Serif", Font.PLAIN, 10);

    /**
     * The default font of the boxes list.
     */
    public static final Font BOXES_LIST_FONT = new Font("Sans Serif", Font.PLAIN, 10);

    /**
     * The default font of the palette list.
     */
    public static final Font PALETTE_LIST_FONT = new Font("Sans Serif", Font.PLAIN, 10);

    /**
     * The default font for buttons on the toolbox space.
     */
    public static final Font TOOLBOX_BUTTON_FONT = new Font("Sans Serif", Font.PLAIN, 10);

    /**
     * The font for dialog buttons.
     */
    public static final Font DIALOG_FONT = new Font("Dialog", Font.BOLD, 10);


    /**
     * The number of rows set by default on the textareas in the
     * messagespace.
     */
    public static final int TEXTROWS = 6;

    /**
     * The number of column set by default on the textareas in the
     * messagespace.
     */
    public static final int TEXTCOLS = 10;

    /**
     * A flag to print out debug messages to standard out.  If set to true,
     * all debug messages will be printed out.  Otherwise debug messages
     * will not be printed out.
     */
    public static boolean VERBOSE = false;


    /**
     * A flag to print out debug messages.
     */
    public static boolean DEBUG = false;

    /**
     * The default size of the editor upon creation.
     */
    public static Dimension EDITOR_SIZE = new Dimension(800, 600);

    /**
     * The look and feel globally set for all of the windows of the GUI.
     */
    public static String LOOK_AND_FEEL = "javax.swing.plaf.metal.MetalLookAndFeel";

    /**
     * For string tokenizing
     */
    public static String DELIM = ":";

    /**
     * Box values
     */
    private static int incre = 0;
    public static int FILTER = incre++;
    public static int MAP = incre++;
    public static int AGGREGATE = incre++;
    public static int UPDATE_RELATION = incre++;
    public static int READ_RELATION = incre++;
    public static int BSORT = incre++;
    public static int RESTREAM = incre++;
    public static int UNION = incre++;
    public static int RESAMPLE = incre++;
    public static int JOIN = incre++;
    public static int DROP = incre++;
    public static int SUPERBOX = incre++;
    public static int INPUTPORT = incre++;
    public static int OUTPUTPORT = incre++;


    public static int INPUTPORTTYPE = 0;
    public static int OUTPUTPORTTYPE = 1;

    public static int ROOTBOXID = -1;


    public static int MAX_STREAM_BOX_ID = 63;

    /**
     * Primitive type values
     */


    public static int UNDEFINED_TYPE = TypeManager.UNDEFINED_TYPE;
    public static int INTEGER = TypeManager.INTEGER;
    public static int FLOAT = TypeManager.FLOAT;
    public static int DOUBLE = TypeManager.DOUBLE;
    public static int STRING = TypeManager.STRING;
    public static int BOOL = TypeManager.BOOL;
    public static int TIMESTAMP = TypeManager.TIMESTAMP;

    public static String INTEGER_NAME = TypeManager.INTEGER_NAME;
    public static String FLOAT_NAME = TypeManager.FLOAT_NAME;
    public static String DOUBLE_NAME = TypeManager.DOUBLE_NAME;
    public static String STRING_NAME = TypeManager.STRING_NAME;
    public static String BOOL_NAME = TypeManager.BOOL_NAME;
    public static String TIMESTAMP_NAME = TypeManager.TIMESTAMP_NAME;

    public static int VARIABLE_SIZE = TypeManager.VARIABLE_SIZE;

    public static int INTEGER_SIZE = TypeManager.INTEGER_SIZE;
    public static int FLOAT_SIZE = TypeManager.FLOAT_SIZE;
    public static int DOUBLE_SIZE = TypeManager.DOUBLE_SIZE;
    public static int STRING_SIZE = TypeManager.STRING_SIZE;
    public static int BOOL_SIZE = TypeManager.BOOL_SIZE;
    public static int TIMESTAMP_SIZE = TypeManager.TIMESTAMP_SIZE;

    public static Icon ARC = new ImageIcon("icons/arc.gif");
    public static Icon EDIT = new ImageIcon("icons/edit.gif");
    public static Icon LOAD = new ImageIcon("icons/load.gif");
    public static Icon SAVE = new ImageIcon("icons/save.gif");

    public static Icon INPUT = new ImageIcon("icons/input.gif");
    public static Icon OUTPUT = new ImageIcon("icons/output.gif");

    public static String INFERREDTYPENAME = "InferredType";
    public static String DEFINEDTYPENAME = "New";


    /**
     * Workload values
     */
    public final static int WORKLOAD_START = 0;
    public final static int WORKLOAD_UPDATE = 1;
    public final static int WORKLOAD_STOP = 2;

    //sourcetype values
    public final static int VAR_TYPE_CONSTANT = 0;
    public final static int VAR_TYPE_UNIFORM = 1;
    public final static int VAR_TYPE_NORMAL = 2;
    public final static int VAR_TYPE_EXPONENTIAL = 3;
    public final static int VAR_TYPE_LINEAR = 4;
    public final static int VAR_TYPE_RANDOM_WALK = 5;
    public final static int VAR_TYPE_RANDOMDIR_WALK = 6;
    public final static int VAR_TYPE_TIMESTAMP = 7;

    public final static int [] VAR_PARAM_NUM = {1,2,2,1,4,5,5,0};

    static {
        if(ARC.getIconHeight() == -1) {

            URL urlSave = RootFrame.class.getResource("save.gif");
            Image imgSave = Toolkit.getDefaultToolkit().getImage(urlSave);
            Constants.SAVE = new ImageIcon(imgSave);

            URL urlEdit = RootFrame.class.getResource("edit.gif");
            Image imgEdit = Toolkit.getDefaultToolkit().getImage(urlEdit);
            Constants.EDIT = new ImageIcon(imgEdit);

            URL urlArc = RootFrame.class.getResource("arc.gif");
            Image imgArc = Toolkit.getDefaultToolkit().getImage(urlArc);
            Constants.ARC = new ImageIcon(imgArc);

            URL urlLoad = RootFrame.class.getResource("load.gif");
            Image imgLoad = Toolkit.getDefaultToolkit().getImage(urlLoad);
            Constants.LOAD = new ImageIcon(imgLoad);
        }
    }

}


