package edu.brown.aurora.gui;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.border.*;
import java.io.*;
import java.util.StringTokenizer;

import edu.brown.aurora.gui.types.*;


/**
 * A basic JDialog that allows the user to specify a file name
 * and a struct name for the creation of a C++ struct for the 
 * given GenericPortNode
 *
 * written by Matthew Hatoun (mhatoun@cs.brown.edu) on 6/23/03
 */

public class PortNodeStructDialog extends JDialog
{
    GenericPortNode node;

    //the field to specify the file name in
    private JTextField fileField;

    //the field to specify the struct name in
    private JTextField structField;

    //the file label
    private JLabel fileLabel;
    
    //the struct label
    private JLabel structLabel;

    //the okay button
    private JButton okayButton;
    
    //the cancel button 
    private JButton cancelButton;

    public PortNodeStructDialog(GenericPortNode node)
    {
        super();
	this.node = node;
        setTitle("Struct Specification");
        setResizable(false);
        setModal(true);
        setSize(285,134);
	getContentPane().setLayout (new BorderLayout());

	//bags is the layout used to set up all the layouts
	GridBagLayout bags = new GridBagLayout();

	//the JPanel used to store all the components 
	JPanel structDialogPanel = new JPanel (bags);

	//sets up the file components
	fileField = new JTextField ();
	fileLabel = new JLabel ("File name:");

	//sets up the struct components
	structField = new JTextField();
	structLabel = new JLabel ("Struct name:");

	//sets up the buttons
	okayButton = new JButton (" Okay ");
	cancelButton = new JButton ("Cancel");
        StructDialogButtonListener bListener = new StructDialogButtonListener();
        okayButton.addActionListener(bListener);
        cancelButton.addActionListener(bListener);

	//adds all the components to the name dialog panel
	JPanel buttonPanel = new JPanel (bags);
	JPanel filePanel = new JPanel (bags);
	JPanel structPanel = new JPanel (bags);
	addComponent (bags, buttonPanel, okayButton, 0,0, 1,1, .2,.2, 
		      new Insets (5,20,20,5));
	addComponent (bags, buttonPanel, cancelButton, 1,0, 1,1, .2,.2,
		      new Insets (5,5,20,20));
	addComponent (bags, filePanel, fileLabel, 0,0, 1,1, .2,.2, 
		      new Insets (0,0,0,0));
	addComponent (bags, filePanel, fileField, 1,0, 1,1, .6,.2, 
		      new Insets (0,0,0,0));
	addComponent (bags, structPanel, structLabel, 0,0, 1,1, .2,.2, 
		      new Insets (0,0,0,0));
	addComponent (bags, structPanel, structField, 1,0, 1,1, .6,.2, 
		      new Insets (0,0,0,0));
	addComponent (bags, structDialogPanel, filePanel, 0,0, 1,1, .0,.0,
		      new Insets (0,0,0,0));
	addComponent (bags, structDialogPanel, structPanel, 0,1, 1,1, .0,.0,
		      new Insets (0,0,0,0));
	addComponent (bags, structDialogPanel, buttonPanel, 0,2, 1,1, .0,.0,
		      new Insets (0,0,0,0));

	getContentPane().add (structDialogPanel, BorderLayout.CENTER);
	setLocationRelativeTo (node.getNodeComponent());
	show();
    }
  
    
    class StructDialogButtonListener implements ActionListener
    {
        public void actionPerformed(ActionEvent event)
        {
	    if (event.getActionCommand().equals (" Okay ")) {
                okayButtonActionPerformed();
	    }
            else if (event.getActionCommand().equals ("Cancel")) {
                cancelButtonActionPerformed();
	    }
        }
    }
    
  
    void okayButtonActionPerformed() {
        if (fileField.getText().equals("")) {
            JOptionPane.showConfirmDialog(null, 
                                          "The file name should not be null", 
                                          "Exception" , 
                                          JOptionPane.DEFAULT_OPTION, 
                                          JOptionPane.ERROR_MESSAGE);
        } 
	else if (structField.getText().equals("")) {
            JOptionPane.showConfirmDialog(null, 
                                          "The struct name should not be null", 
                                          "Exception" , 
                                          JOptionPane.DEFAULT_OPTION, 
                                          JOptionPane.ERROR_MESSAGE);
        } 
	else {
	    if (createStruct (node.getType())) {
		dispose();
	    }
	}
	   
    }
    
    void cancelButtonActionPerformed()
    {	
	dispose();
    }
    

    //generates a valid C++ name for the given attribute name
    String generateValidName (String name) 
    {
	StringTokenizer tokenizer = new StringTokenizer (name, ".", true);
	String returnName = "";
	while (tokenizer.hasMoreElements()) {
	    String token = tokenizer.nextToken();
	    if (token.equals (".")) {
		returnName += "_";
	    }
	    else {
		returnName += token;
	    }
	}

	if (returnName == "") {
	    return name;
	}
	else {
	    return returnName;
	}
	
    }


    boolean createStruct (Type type) 
    {
	CompositeType compType = (CompositeType) type;
	if (compType == null ) {
	    JOptionPane.showConfirmDialog(null, 
					  "No type specified", 
					  "Exception" , 
					  JOptionPane.DEFAULT_OPTION, 
					  JOptionPane.ERROR_MESSAGE);
	    dispose();
	}
	else {

        try {
	    boolean stringAttribute = false;
	    int maxStringLength = 0;
		
	    
	    //makes a list of all the attribute names in valid C++ format
	    String[] attributeNames = new String[compType.getNumberOfAttributes()];
	    for (int i = 0 ; i < compType.getNumberOfAttributes() ; i++) {
	 	attributeNames[i] = generateValidName (compType.getAttributeName(i));
	    }
	    
	    FileWriter fWriter = new FileWriter (new File (fileField.getText()));
	    fWriter.write ("#ifndef " + structField.getText().toUpperCase() + "_H\n");
	    fWriter.write ("#define "+ structField.getText().toUpperCase() + "_H\n\n\n");
	    fWriter.write ("#include <time.h>\n");
	    fWriter.write ("#include <sys/time.h>\n");
	    fWriter.write ("#include <string>\n");
	    fWriter.write ("#include <sstream>\n");
	    fWriter.write ("#include <util.H>\n");
	    fWriter.write ("using namespace std;\n");
	    fWriter.write ("struct " + structField.getText() + " { \n\n");
	    fWriter.write ("  timeval  _sys_timestamp  __attribute__ ((packed));\n");
	    fWriter.write ("  int  _sys_streamId  __attribute__ ((packed));\n\n");
	    
	    for  (int i = 0; i < compType.getNumberOfAttributes(); i++) {
		if (((PrimitiveType)compType.getAttributeType (i)).equals (TypeManager.INTEGER_TYPE)) {
		    fWriter.write ("  int  _" + attributeNames[i] 
				   + "  __attribute__ ((packed));\n");
		}
		else if (((PrimitiveType) compType.getAttributeType (i)).equals (TypeManager.FLOAT_TYPE)) {
		    fWriter.write ("  float  _" + attributeNames[i] 
				   + "  __attribute__ ((packed));\n");
		}	
		else if (((PrimitiveType) compType.getAttributeType (i)).equals (TypeManager.DOUBLE_TYPE)) {
		    fWriter.write ("  double  _" + attributeNames[i] + 
				   "  __attribute__ ((packed));\n");
		}
		else if (((PrimitiveType) compType.getAttributeType (i)).equals (TypeManager.TIMESTAMP_TYPE)) {
		    fWriter.write ("  timeval  _" + attributeNames[i] + 
				   "  __attribute__ ((packed));\n");
		}
		else if (((PrimitiveType) compType.getAttributeType (i)).equals (TypeManager.BOOL_TYPE)) {
		    fWriter.write ("  bool  _" + attributeNames[i] + 
				   "  __attribute__ ((packed));\n");
		}
		else if (((PrimitiveType) compType.getAttributeType (i)).equals (TypeManager.STRING_TYPE)) {
		    fWriter.write ("  char  _" + attributeNames[i] +
				   "[" + compType.getAttributeLength(i) 
				   + "]  __attribute__ ((packed));\n");
		    stringAttribute = true;
		    if (compType.getAttributeLength(i) > maxStringLength) {
			maxStringLength = compType.getAttributeLength(i);
		    }
		}
	    }
	    

	    //writes the toString method
	    fWriter.write ("\n\n  string toString() const\n{");
	    fWriter.write ("    char* pBuffer = new char[" + maxStringLength + "+1];\n");
	    fWriter.write ("     ostringstream os;\n");
	    fWriter.write ("     os << \"" +structField.getText() + " (this = \" << this << \")\" << endl\n");
	    fWriter.write ("        << \"   _sys_timestamp = \" << timevalToSimpleString(_sys_timestamp) << endl\n");
	    fWriter.write ("        << \"   _sys_streamId = \" << _sys_streamId << endl;\n");
	    
	    for  (int i = 0; i < compType.getNumberOfAttributes(); i++) {
		if (((PrimitiveType)compType.getAttributeType (i)).equals (TypeManager.INTEGER_TYPE) ||
		    ((PrimitiveType) compType.getAttributeType (i)).equals ( TypeManager.FLOAT_TYPE) ||
		    ((PrimitiveType) compType.getAttributeType (i)).equals ( TypeManager.DOUBLE_TYPE) ||
		    ((PrimitiveType) compType.getAttributeType (i)).equals (TypeManager.BOOL_TYPE) ) {
		    fWriter.write ("     os << \"   _" + attributeNames[i] + " = \" << _" +
				   attributeNames[i]);
		 
		}
		else if (((PrimitiveType) compType.getAttributeType (i)).equals ( TypeManager.TIMESTAMP_TYPE)) {
		    fWriter.write ("     os << \"   _" + attributeNames[i] 
				   + " = \" << timevalToSimpleString(_" + 
				   attributeNames[i] + ")");
		}
		else if (((PrimitiveType) compType.getAttributeType (i)).equals (TypeManager.STRING_TYPE)) {
		    fWriter.write ("     memcpy(pBuffer, _" + attributeNames[i] + ", " +
				   compType.getAttributeLength(i) + ");\n" +
				   "     pBuffer[" +compType.getAttributeLength(i) + "] = '\\0'; \n" +
				   "     os << \"   _" + attributeNames[i]+ 
				   " = \\\"\" << pBuffer << \"\\\"\" ");
		   
		}
		if ( i+1 < compType.getNumberOfAttributes()) {
		    fWriter.write (" << endl;\n");
		}
		else {
		    fWriter.write (";\n");
		}

	    }

	    fWriter.write ("     delete[] pBuffer;\n" +
			   "     return os.str();\n" +
			   "  }\n " + 
			   "\n};\n" +
			   "#endif\n");
	    fWriter.close();

	} catch (Exception e) {
	    JOptionPane.showConfirmDialog(null, 
					  "Could not write to file", 
					  "Exception" , 
					  JOptionPane.DEFAULT_OPTION, 
					  JOptionPane.ERROR_MESSAGE);
	    return false;
	}
	}
	
	return true;
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
    private void addComponent(GridBagLayout bags,
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

