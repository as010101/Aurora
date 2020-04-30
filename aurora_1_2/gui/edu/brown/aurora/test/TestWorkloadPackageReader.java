import java.net.Socket;
import java.net.ServerSocket;
import java.net.InetSocketAddress;
import java.net.InetAddress;
import java.io.*;

public class TestWorkloadPackageReader {

    //create a tester and run it
    public TestWorkloadPackageReader(){
        try {
            //opens the listening connection
            ServerSocket serverin = new ServerSocket(4999,10);
            System.out.println("Starting Server on port 4999.");
            Socket socket_In = serverin.accept();
            //stream to read from
            DataInputStream i = new DataInputStream(socket_In.getInputStream());

            //while there is data to read from the stream, print it out
            double nextByte=0;
            while(nextByte!=-1) {
                try {
                    nextByte=(double)i.readDouble();
                    System.out.print(nextByte+"\n");
                } catch (EOFException e) {
                    System.err.println(e + ": End of data... exiting");
                    i.close();
                    return;
                }
            }
        } catch (Exception e) {
            System.err.println(e);
        }
    }

    public static void main(String[] args) throws Exception {
        TestWorkloadPackageReader m = new TestWorkloadPackageReader();
    }
}
