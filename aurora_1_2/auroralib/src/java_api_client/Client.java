import java.lang.*;
import java.net.*;
import java.io.*;



//To use this javaAPI first start the c++ server (javaapi_server).  Then every program must first call
//aurora_init with a valid filename.  Then the program may use these functions as they please.
//Shutdown should be called last and only once because it shuts down the api server as well.

public class Client{
  
    private String callString = "";
    private String inLine = "";
    private static String hostIP = "";
    private static int portNum = 0;
    private boolean initCalled = false;
    // these are set after enq and deq are called
    private byte[] enqTupleAddr = null;
    private byte[] deqTupleAddr = null;

    public Client(){
	



    }
    public boolean aurora_init(String x) throws Exception {
	boolean ret = false;
	int meth = 1;
	callString =  meth+" "+ x;
	int len = callString.getBytes().length;
	callString = len + " " + callString;
	try {
	    run_client(false, false);
	}
	catch (Exception e) {
	    throw e;
	}
	if (inLine.substring(0,1).equals("1")) {
	    initCalled = true;
	    ret  = true;
	    System.out.println("aurora initialized");
	}
	else {
	    throw new Exception(inLine.substring(2));
	}
	return ret;
	    
    }
    public boolean aurora_shutdown() throws Exception{
	boolean ret = false;
	if (initCalled) {
	    callString = "2 ";
	    int len = callString.getBytes().length;
	    callString = len + " " + callString;
	    try {
		run_client(false, false);
	    }
	    catch (Exception e) {
		throw e;
	    }
	    if (inLine.substring(0,1).equals("1")) {
		System.out.println("shutdown successful");
		ret = true;
	    }
	    else {
		throw new Exception(inLine.substring(2));
	    }
	}
	else {
	    throw new Exception("Exception: No Aurora Initialized");
	}
	
	return ret;
    }
    public int aurora_getNumInputPorts() throws Exception {
	int numI = 0;
	if (initCalled) {
	    callString = "3 ";
	    int len = callString.getBytes().length;
	    callString = len + " " + callString;
	    try {
		run_client(false, false);
	    }
	    catch (Exception e) {
		throw e;
	    }
	    if (inLine.substring(0,1).equals("1")) {
		numI = Integer.parseInt(inLine.substring(2));
		System.out.println("recieved numInputPorts: "+numI);
	    }
	    else {
		throw new Exception(inLine.substring(2));
	    }
	}
	else {
	    throw new Exception("Exception: No Aurora Initialized");
	}
	return numI;
    }

    public int aurora_getInputPortNum(int portIndex) throws Exception {
	int iNum = 0;
	if (initCalled) {
	    callString = "4 " + portIndex;
	    int len = callString.getBytes().length;
	    callString = len + " " + callString;
	    try {
		run_client(false, false);
	    }
	    catch (Exception e) {
		throw e;
	    }
	    if (inLine.substring(0,1).equals("1")) {
		iNum = Integer.parseInt(inLine.substring(2));
		System.out.println("recieved InputPortNum: "+ iNum);
	    }
	    else {
		throw new Exception(inLine.substring(2));
	    }
	}
	else {
	    throw new Exception("Exception: No Aurora Initialized");
	}
	return iNum;
    }

    public int aurora_getNumOutputPorts() throws Exception {
	int numO = 0;
	if (initCalled) {
	    callString = "5 ";
	    int len = callString.getBytes().length;
	    callString = len + " " + callString;
	    try {
		run_client(false, false);
	    }
	    catch (Exception e) {
		throw e;
	    }
	    if (inLine.substring(0,1).equals("1")) {
		numO = Integer.parseInt(inLine.substring(2));
		System.out.println("recieved numOutputPorts: "+ numO);
	    }
	    else {
		throw new Exception(inLine.substring(2));
	    }
	}
	else {
	    throw new Exception("Exception: No Aurora Initialized");
	}
	return numO;
    }
    public int aurora_getOutputPortNum(int portIndex) throws Exception {
	int Onum = 0;
	if (initCalled) {
	    callString = "6 " + portIndex;
	    int len = callString.getBytes().length;
	    callString = len + " " + callString;
	    try {
		run_client(false, false);
	    }
	    catch (Exception e) {
		throw e;
	    }
	    if (inLine.substring(0,1).equals("1")) {
		Onum = Integer.parseInt(inLine.substring(2));
		System.out.println("recieved OutputPortNum: "+ Onum);
	    }
	    else {
		throw new Exception(inLine.substring(2));
	    }
	}
	else {
	    throw new Exception("Exception: No Aurora Initialized");
	}
	return Onum;
    }


    //enqrecord fills int tupleAddr if successful
    // tuplesize must be the size of the tupleAddr and first 12 bytes will be overwritten
    // with time value and stream ID  so your fields should start at tupleAddr[12].
    public void aurora_enqRecord(int inputPortId, 
				 int streamId,
				 byte tupleAddr[],
				 int tupleSize) throws Exception {	
	if (initCalled) {
	    enqTupleAddr = tupleAddr;
	    callString = "7 " + inputPortId + " " +streamId + " " + tupleSize;
	    int len = callString.getBytes().length;
	    callString = len + " " + callString;
	    try {
		run_client(true, false);
	    }
	    catch (Exception e) {
		throw e;
	    }
	    if (inLine.substring(0,1).equals("1")) {
		
		tupleAddr = enqTupleAddr;
		String prntOut = "enqueued record: ";
		for (int i =0; i<tupleAddr.length; i++) {
		    prntOut = prntOut + "[" + (int)tupleAddr[i]+"]";
		}

		System.out.println(prntOut);
	    }
	    
	    else {
		throw new Exception(inLine.substring(2));
	    }
	    
	}
	else {
	    throw new Exception("Exception: No Aurora Initialized");
	}
    }			 
    //deqrecord:
    /* wasDequeued must be of at least size 1 and after this is called, if it was successful,
       wasDequeued[0]=1. if it was unsuccessfull, wasDequeued[0]=0;
       outputPortID must also be of at least size 1. After this is called, if it was successful,
       outputPortID[0] will equal the port ID.  If it was unsuccessful, this is not changed.
       tupleAddr must be of size buffSize and that size must be big enough to get the tuple being 
       dequeued
    */
    public boolean aurora_deqRecord(int wasDequeued[], 
				 int outputPortID[],
				 byte tupleAddr[],
				 int buffSize) throws Exception {
	boolean ret = false;
	if (initCalled) {
	    deqTupleAddr = tupleAddr;
	    callString = "8 " + buffSize;
	    int len = callString.getBytes().length;
	    callString = len + " " + callString;
	    try {
		run_client(false, true);
	    }
	    catch (Exception e) {
		throw e;
	    }
	    if (inLine.substring(0,1).equals("1")) {
		inLine = inLine.substring(2);
		wasDequeued[0] = Integer.parseInt(inLine.substring(0,1));
		if (wasDequeued[0]==1) {
		    ret = true;
		    inLine = inLine.substring(2);
		    outputPortID[0] = Integer.parseInt(inLine);
			    
		    tupleAddr = deqTupleAddr;
		    String prntOut = "outputPortID: " + outputPortID[0] + "\ndequeued record: ";
		    for (int i=0; i<tupleAddr.length;i++) {
			prntOut = prntOut+ "["+(int)tupleAddr[i]+"]";
			
		    }
		    System.out.println(prntOut);
		    
		}
		else {
		    System.out.println("No Tuple Was Dequeued");
		}
	    }
	    else {
		throw new Exception(inLine.substring(2));
	    }
	}
	else {
	    throw new Exception("Exception: No Aurora Initialized");
	}
	return ret;
    }


    public boolean aurora_xStartLoadGenerator() throws Exception {
	boolean ret = false;
	if (initCalled) {
	    callString = "9 ";
	    int len = callString.getBytes().length;
	    callString = len + " " + callString;
	    try {
		run_client(false, false);
	    }
	    catch (Exception e) {
		throw e;
	    }
	    if (inLine.substring(0,1).equals("1")) {
		System.out.println("xStartLoadGenerator successful");
		ret = true;
	    }
	    else {
		throw new Exception(inLine.substring(2));
	    }
	}
	else {
	    throw new Exception("Exception: No Aurora Initialized");
	}
	return ret;
    }
    
    public boolean aurora_xStopLoadGenerator() throws Exception {
	boolean ret = false;
	if (initCalled) {
	    callString = "10 ";
	    int len = callString.getBytes().length;
	    callString = len + " " + callString;
	    try {
		run_client(false, false);
	    }
	    catch (Exception e) {
		throw e;
	    }
	    if (inLine.substring(0,1).equals("1")) {
		System.out.println("xStopLoadGenerator successful");
		ret = true;
	    }
	    else {
		throw new Exception(inLine.substring(2));
	    }
	}
	else {
	    throw new Exception("Exception: No Aurora Initialized");
	}
	return ret;
    }


    public void run_client(boolean enq, boolean deq) throws Exception {

	Socket csock = null;
	DataInputStream inst = null;
	PrintStream out = null;
	boolean connected = false;
	//create socket
	try {
	    csock = new Socket(InetAddress.getByName(hostIP).getHostName(), portNum);
	    
	    out = new PrintStream(csock.getOutputStream());
	    inst = new DataInputStream(csock.getInputStream());
	    connected = true;
	}
	catch (IOException e) {
	    throw e;
	}
	
	//send (and recieve info);
	try {
	    out.println(callString);
	    out.flush();
	    
	    if (!enq)
		inLine = inst.readLine();
	    
	    inst.close();
	    csock.close();
	    out.close();
	}
	catch (Exception e){
	    throw e;
	}
	
	DataOutputStream out2 = null;
	if (enq) {
	    try {
		csock = new Socket(InetAddress.getByName(hostIP).getHostName(),portNum);			
		inst = new DataInputStream(csock.getInputStream());
		out2 = new DataOutputStream(csock.getOutputStream());
		
	    }
	    catch(Exception e){
		throw e;
	    }
	    
	    try {
		out2.write(enqTupleAddr);
		out2.flush();
		
	    }
	    catch (Exception e) {
		throw e;
	    }	    
	    try {
		
		inLine = inst.readLine();
		out2.close();
		inst.close();
		csock.close();
	    }	 
	    catch (Exception e){
		throw e;
	    }
	    try {
		csock = new Socket(InetAddress.getByName(hostIP).getHostName(),portNum);
		inst = new DataInputStream(csock.getInputStream());
	    }
	    catch(Exception e){
		throw e;
	    }
	    try {		
		inst.readFully(enqTupleAddr);
		inst.close();
		csock.close();
	    }
	    catch(Exception e) {
		throw e;
	    }
	}
	if (deq) {
	    try {
		csock = new Socket(InetAddress.getByName(hostIP).getHostName(),portNum);			
		inst = new DataInputStream(csock.getInputStream());
	    }
	    catch(Exception e){
		throw e;
	    }
	    try {		
		inst.readFully(deqTupleAddr);
		inst.close();
		csock.close();
	    }
	    catch(Exception e) {
		throw e;
	    }
	}
    }


    public static void main(String args[]) {
	if (args.length<2) {
	    System.out.println("Usage: Client <serverIP> <PortNumber>");
	}
	else {
	    hostIP = args[0];
	    portNum = Integer.parseInt(args[1]);
	    Client x = new Client();
	    try {
		x.aurora_init("/u/cjc/aurora_trunk/auroralib/src/test_capi1/dataset1_props.xml");
		
		x.aurora_xStartLoadGenerator();
		x.aurora_xStopLoadGenerator();
		/*
		String bs = "I love making bytes from strings";
		byte[] bsA = bs.getBytes();
		int bsAlen = bsA.length;
		*/
		
		byte[] a = new byte[20];//12+bsAlen];
		for (int i=0; i<20; i++) {
		    Integer j = new Integer(i);
		    a[i] = j.byteValue();
		}
		/*
		for (int i=12; i<(a.length); i++) {
		    a[i] = bsA[i-12];
		}
		*/
		
		//System.out.println(a.length);
		String as = "";
		for (int i=0; i<a.length;i++) {
		    as = as+"["+a[i]+"]";
		}
		//System.out.print(as);
		
		int l = x.aurora_getNumInputPorts();
		int m = x.aurora_getInputPortNum(l-1);

		x.aurora_enqRecord(m,0,a,a.length);
		
		
		int[] b = new int[3];
		int[] c = new int[3];
		for (int i=0;i<3;i++) {
		    b[i] = c[i] = 0;
		}
		int w = x.aurora_getNumOutputPorts();
		int y = x.aurora_getOutputPortNum(w-1);	
		x.aurora_deqRecord(b,c,a,20);
		x.aurora_shutdown();
		
	    }
	    catch (Exception e) {
		System.out.println(e.toString());
	    }
	}
    }
}
