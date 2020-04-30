
package edu.brown.aurora.gui;

import edu.brown.bloom.petal.*;

import javax.swing.JOptionPane;
import java.net.*;
import java.io.*;
import edu.brown.bloom.petal.*;
import edu.brown.aurora.gui.types.*;

import java.util.*;

/**
 * A class to send data to the Workload generator in a nice format.
 *
 * @author Adam Singer (arsinger@cs.brown.edu)
 * @version 1.0 08/16/2002
 */
public class WorkloadDataPackage
{
    private String _server;
    private int _port, _mode;
    private SuperBoxNode _root;

    /**
     * Creates a WorkloadDataPackage with no root node
     */
    public WorkloadDataPackage()
    {
        _server = "localhost";
        _port = 4999;
        _root = null;
        _mode = Constants.WORKLOAD_START;
    }

    /**
     * Creates a WorkloadDataPackage with the specified root superbox
     *
     * @param root The root superbox of the network
     */
    public WorkloadDataPackage(SuperBoxNode root)
    {
        _server = "localhost";
        _port = 4999;
        _root = root;
        _mode = Constants.WORKLOAD_START;
    }

    /**
     * Wrap and send all data from the root superbox's input ports
     * to the workload generator
     */
    public boolean sendData()
    {
        if (_root==null) {
            JOptionPane.showMessageDialog(null, "No root super box specified");
            return false;
        }
        boolean success = true;
        Model base = _root.getSuperBoxModel();
        Vector nodes = base.getAllInputPortNodes();
        
        InetAddress ip;
        try {
            ip = InetAddress.getByName(_server);
        } catch (UnknownHostException e) {
            showException(e);
            return false;
        }
        int loops = nodes.size();
        if (_mode == Constants.WORKLOAD_STOP) {
            loops = 1;
        }
        for (int i=0; i<loops; ++i) {
            if (((InputPortNode)nodes.elementAt(i)).usesWorkload()) {
                Socket socketOut = new Socket();
                try {
                    socketOut.connect(new InetSocketAddress(ip, _port));
                } catch (IOException e) {
                    showException(e);
                    return false;
                }
                //System.out.println("Sending node data " + i);
                try {
                    success = sendPacket((InputPortNode)nodes.elementAt(i), socketOut);
                    socketOut.close();
                } catch (Exception e) {
                    showException(e);
                    return false;
                }
            }
        }
        return success;
    }

    /**
     * Sends a single packet to the workload generator from one port.
     *
     * @param portNumber the inputPort to examine
     * @param socketOut the socket to send data out to
     */
    protected boolean sendPacket(InputPortNode node, Socket socketOut)
    {
        
        File tmpF= new File("./aurora_workload_data");
        DataOutputStream o;
        DataOutputStream ofstr;
        DataInputStream ifstr;

        try {
            o = new DataOutputStream(socketOut.getOutputStream());
        } catch (Exception e) {
            showException(e);
            return false;
        }
        try {
            ofstr=new DataOutputStream(new FileOutputStream(tmpF));
        } catch (Exception e) {
            showException(e);
            return false;
        }
        try {
            ifstr = new DataInputStream(new FileInputStream(tmpF));
        } catch (Exception e) {
            showException(e);
            return false;
        }
        
        double nextByte=0;
        int count = 0;
        BoxNodeOutputPort outputNode = node.getOutputPortNode();
        Vector arcs = outputNode.getArcs();

        FieldSet fields = node.getFieldSet();
        int numFields = fields.getNumFields();
        double[] fieldLengs = new double[numFields];
        
        //transfer mode
        nextByte = _mode;
        try {
            ofstr.writeInt((int)nextByte);
        } catch (Exception e) {
            showException(e);
            return false;
        }
        count += 4;

        //unique stream identifier
        nextByte = node.getBoxId();
        try {
            ofstr.writeInt((int)nextByte);
        } catch (Exception e) {
            showException(e);
            return false;
        }
        count += 4;

        //number of streams
        nextByte = node.getNumberOfStreams();
        try {
            ofstr.writeInt((int)nextByte);
        } catch (Exception e) {
            showException(e);
            return false;
        }
        count += 4;

        //number of tuples
        nextByte = node.getNumberOfTuples();
        try {
            if (nextByte<0) {
                ofstr.writeInt(0);
                nextByte = -nextByte;
            } else {
                ofstr.writeInt(1);
            }
        } catch (Exception e) {
            showException(e);
        }
        count += 4;

        try {
            ofstr.writeInt((int)nextByte);
        } catch (Exception e) {
            showException(e);
            return false;
        }
        count += 4;

        /**rate information**/
        //rate distribution
        nextByte = fields.getDistributionType();
        try {
            ofstr.writeInt((int)nextByte);
        } catch (Exception e) {
            showException(e);
            return false;
        }
        count += 4;

        //rate distribution num params
        int tmpStore = Constants.VAR_PARAM_NUM[fields.getDistributionType()];
        try {
            ofstr.writeInt((int)tmpStore);
        } catch (Exception e) {
            showException(e);
            return false;
        }
        count += 4;

        //rate distribution params' distributions
        for (int j=0; j<tmpStore; ++j) {
            int disttype = fields.getDistributionParam(j).getDistributionType();
            nextByte = disttype;
            try {
                ofstr.writeInt((int)nextByte);
            } catch (Exception e) {
                showException(e);
                return false;
            }
            count += 4;

            //rate distribution params' distribution num params
            int innerMax = Constants.VAR_PARAM_NUM[disttype];
            try {
                ofstr.writeInt(innerMax);
            } catch (Exception e) {
                showException(e);
                return false;
            }
            count += 4;

            //rate distribution params' distributions' params
            //
            //note- this comment is actually correct. see workload
            //required information for details
            for (int inner=0; inner<innerMax; ++inner){
                nextByte = fields.getDistributionParam(j).getParameter(inner);
                try {
                    if (nextByte<0) {
                        ofstr.writeInt(0);
                        nextByte = -nextByte;
                    } else {
                        ofstr.writeInt(1);
                    }
                } catch (Exception e) {
                    showException(e);
                    return false;
                }
                count += 4;
                
                try {
                    ofstr.writeInt((int)(nextByte*1000000));
                } catch (Exception e) {
                    showException(e);
                    return false;
                }
                count += 4;
            }
        }

        /**stream information**/

        //number of fields
        nextByte = numFields;
        try {
            ofstr.writeInt((int)nextByte);
        } catch (Exception e) {
            showException(e);
            return false;
        }
        count += 4;
        
        /**tuples information**/
        for (int k=0; k<fields.getNumFields(); ++k) {
            Field fieldTemp = fields.getField(k);

            //field type
            nextByte = (-(fieldTemp.getTypeId()))-1;
            try {
                ofstr.writeInt((int)nextByte);
            } catch (Exception e) {
                showException(e);
                return false;
            }
            count += 4;

            //field length
            nextByte = fieldTemp.getLength();
            try {
                ofstr.writeInt((int)nextByte);
            } catch (Exception e) {
                showException(e);
                return false;
            }
            count += 4;

            //field variance
            int variance = fieldTemp.getVariance();
            nextByte = variance;
            try {
                ofstr.writeInt((int)nextByte);
            } catch (Exception e) {
                showException(e);
                return false;
            }
            count += 4;

            //field variance num parameters
            tmpStore = Constants.VAR_PARAM_NUM[variance];
            try {
                ofstr.writeInt((int)tmpStore);
            } catch (Exception e) {
                showException(e);
                return false;
            }
            count += 4;

            //field variance parameters
            for (int m=0; m<tmpStore; ++m) {
                //distribution type per parameter
                DistributionSet tmpDistType = fieldTemp.getParameter(m);
                int fieldDistType = tmpDistType.getDistributionType();
                nextByte = fieldDistType;
                try {
                    ofstr.writeInt((int)nextByte);
                } catch (Exception e) {
                    showException(e);
                    return false;
                }
                count += 4;

                //field variance num parameters
                int tmpStore2 = Constants.VAR_PARAM_NUM[fieldDistType];
                try {
                    ofstr.writeInt((int)tmpStore2);
                } catch (Exception e) {
                    showException(e);
                    return false;
                }
                count += 4;

                //distribution parameters of the parameters
                for (int n=0; n<tmpStore2; ++n) {
                    nextByte = tmpDistType.getParameter(n);
                    try {
                        if (nextByte<0) {
                            ofstr.writeInt(0);
                            nextByte = -nextByte;
                        } else {
                            ofstr.writeInt(1);
                        }
                    } catch (Exception e) {
                        showException(e);
                        return false;
                    }
                    count += 4;
                    
                    try {
                        ofstr.writeInt((int)(nextByte*1000));
                    } catch (Exception e) {
                        showException(e);
                        return false;
                    }
                    count += 4;
                }
            }

        }


        byte[] all = new byte[count];
        try {
            ifstr.read(all, 0, count);
        } catch (Exception e) {
            showException(e);
            return false;
        }
        try {
            o.write(all);
        } catch (Exception e) {
            showException(e);
            return false;
        }
        tmpF.delete();

        return true;
    }

    /**
     * Set the root super box of the system.
     *
     * @param root the root superbox
     */
    public void setRootSuperBox(SuperBoxNode root)
    {
        _root = root;
    }

    /**
     * Set the workload generator's server.
     *
     * @param server the server on which the workload generator is running
     */
    public void setServer(String server)
    {
        _server = server;
    }

    /**
     * Set the port through which to contact the workload generator.
     *
     * @param port the port on which the workload generator is listening
     */
    public void setPort(int port)
    {
        _port = port;
    }

    /**
     * Sets the mode of data sending to perform.
     *
     * @param mode the mode to prepend to sent data
     */
    public void setMode(int mode)
    {
        _mode = mode;
    }

    /**
     * Get the name of the server to contact.
     *
     * @return server name
     */
    public String getServer()
    {
        return _server;
    }

    /**
     * Get the port on which we plan to contact the workload generator.
     *
     * @return port number
     */
    public int getPort()
    {
        return _port;
    }
    
    /**
     * Get the mode we are prepending to sent data.
     *
     * @return mode being used at present
     */
    public int getMode()
    {
        return _mode;
    }

    /**
     * Shows a pop-up with an exception.
     * @param e the exception to show
     */
    protected void showException(Exception e) {
        JOptionPane.showConfirmDialog(null, e.toString(),
                                      "Exception",
                                      JOptionPane.DEFAULT_OPTION,
                                      JOptionPane.ERROR_MESSAGE);
    }

}
