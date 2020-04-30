package edu.brown.aurora.gui.types;

import java.awt.*;
import java.awt.event.*;

public class MessageBox extends Dialog 
{
	String command = null;

	class Handler extends WindowAdapter implements ActionListener
	{
		public void windowClosing(WindowEvent e)
		{
			dispose();
		}

		public void actionPerformed(ActionEvent e)
		{
			command = e.getActionCommand();
			dispose();
		}
	}

	public MessageBox(String title, String[] text, String[] buttons) 
	{
		super(new Frame(), title, true);
		Handler h = new Handler();
		int textLines = text.length;
		int numButtons = buttons.length;
		Panel textPanel = new Panel();
		Panel buttonPanel = new Panel();
		textPanel.setLayout(new GridLayout(textLines, 1));
		for(int i = 0; i < textLines; ++i) 
			textPanel.add(new Label(text[i]));
		for(int i = 0; i < numButtons; ++i)
		{
			Button b = new Button(buttons[i]);
			b.addActionListener(h);
			buttonPanel.add(b);
		}
		add("North", textPanel);
		add("South", buttonPanel);
		setBackground(Color.lightGray);
		setForeground(Color.black);
		addWindowListener(h);
		pack();
		setLocation(300, 300);
		setResizable(false);
		show();
	}

	public String getCommand()
	{
		return command;
	}
}
