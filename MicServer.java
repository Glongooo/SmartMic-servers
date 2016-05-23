import java.net.DatagramSocket;
import java.net.DatagramPacket;
import java.net.NetworkInterface;
import java.net.InetSocketAddress;
import java.net.InetAddress;
import java.net.SocketException;
import java.net.PortUnreachableException;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.FileInputStream;
import java.io.File;
import java.io.IOException;
import java.io.FileOutputStream;
import java.nio.channels.IllegalBlockingModeException;
import java.util.Enumeration;
import java.util.Properties;
import java.lang.Thread;
import javax.sound.sampled.AudioFormat;
import javax.sound.sampled.AudioSystem;
import javax.sound.sampled.DataLine;
import javax.sound.sampled.SourceDataLine;
import javax.sound.sampled.LineUnavailableException;
import org.xiph.speex.SpeexDecoder;

public class MicServer {
	private static String SS_IP;
	private static String SS_NAME;
	private static int SS_PORT;
	private static DatagramSocket socket;
	private static boolean playStarted = false;
	private static DatagramPacket packet;
	
	private byte[] buffer;
	private SpeexDecoder speexDecoder;
	private String ip = "";
	private final AudioFormat format;
	private final SourceDataLine srcLine;
	private final DataLine.Info info;
	private final int port = 4444;
	private final int sampleRate = 11025;
	private final int sampleSizeInBits = 16;
	private final int channels = 1; // mono
	private final boolean signed = true;
	private final boolean littleEndian = false;
	private final int packetSize = 62; // depends on client's encoding parameters
	private final int mode = 0; // narrowband mode
	private final String IPADDRESS_REGULAR = 
		"([01]?\\d\\d?|2[0-4]\\d|25[0-5])\\." +
		"([01]?\\d\\d?|2[0-4]\\d|25[0-5])\\." +
		"([01]?\\d\\d?|2[0-4]\\d|25[0-5])\\." +
		"([01]?\\d\\d?|2[0-4]\\d|25[0-5])";
	
	// Native methods
	public static native int connectSmartSpace(
	    String hostname, String ip,  int port);
	public static native int publishData(String ip, String port);
	public static native int deletePublishedData();
	
	// Loading SmartSlog library
	static {
	    final String dir = System.getProperty("user.dir");
	    System.load(dir + "/libsslog.so");
	}

	// Constructor
	MicServer() throws IOException, LineUnavailableException {
	    format = new AudioFormat(sampleRate, sampleSizeInBits, 
			channels, signed, littleEndian);
	    info = new DataLine.Info(SourceDataLine.class, format);
	    srcLine = (SourceDataLine) AudioSystem.getLine(info);
	    
	    speexDecoder = new SpeexDecoder();
	    if(!speexDecoder.init(mode, sampleRate, channels, false))
		System.out.println("decoder init failed");
	    
	    buffer = new byte[packetSize];
	    
	    // Taking IP of active wi-fi interface
	    try {
		ip = getWifiIP();
		
		if(ip.equals("")) {
		    System.out.println("Wifi is not active! \nService stops...\n");
		    System.exit(-1);
		}
	    } catch(NullPointerException e) {
		e.printStackTrace();
	    } catch(SocketException e) {
		e.printStackTrace();
	    }
	}

	public static void main(String[] args) {
		startExitThread();
		      
		if(!loadConfiguration())
		    System.exit(-1);
		    
		System.out.println(MicServer.SS_NAME + " " + MicServer.SS_IP + " " + 
		      MicServer.SS_PORT);
		
		if(connectSmartSpace(MicServer.SS_NAME, MicServer.SS_IP, 
		      MicServer.SS_PORT) != 0) {
		    System.out.println("Faild to connect Smart Space.");
		    System.exit(-1);
		}

		try {
		    MicServer server = new MicServer();
		    socket = new DatagramSocket(
			    new InetSocketAddress(server.ip, server.port));
		    packet = new DatagramPacket(server.buffer, server.packetSize);
		    
		    // Publishing service data to Smart Space (`ip` and `port`)
		    publishData(server.ip, String.valueOf(server.port));
		    
		    // Reading and playing data received from client
		    while(true) {
			byte[] decoded = null;
			int decNumber = 0;
			
			socket.receive(packet);
			
			// Decoding audio packet
			server.speexDecoder.processData(packet.getData(),
				0, server.packetSize);
			decNumber = server.speexDecoder.getProcessedDataByteSize();
			decoded = new byte[decNumber];
			server.speexDecoder.getProcessedData(decoded, 0);
			System.out.println("decoded data size: "  + decNumber);
		
			if(!playStarted) {
			    server.srcLine.open(server.format, decNumber);
			    server.srcLine.start();
			    playStarted = true;
			}
			
			// Playing decoded data
			server.srcLine.write(decoded, 0, decNumber);
		    }
		} catch(IOException e) {
		    e.printStackTrace();
		    System.exit(-1);
		} catch(IllegalArgumentException e) {
		    e.printStackTrace();
		} catch(ArrayIndexOutOfBoundsException e) {
		    e.printStackTrace();
		} catch(LineUnavailableException e) {
		    e.printStackTrace();
		    System.exit(-1);
		}
	}
	
	// Extracts active WiFi IP
	public String getWifiIP() throws NullPointerException, SocketException {
	      Enumeration<NetworkInterface> listInterfaces;
	      Enumeration<InetAddress> listInetAddr;
	      InetAddress inetAddr;
	      NetworkInterface netInterface;
	      String ipAddr;
	      
	      listInterfaces = NetworkInterface.getNetworkInterfaces();
	    
	      for(; listInterfaces.hasMoreElements();) {
		  netInterface = listInterfaces.nextElement();
		  listInetAddr = netInterface.getInetAddresses();
		    
		  if(netInterface.isLoopback() || !netInterface.isUp())
		      continue;
		    
		  for(; listInetAddr.hasMoreElements();) {
		      inetAddr = listInetAddr.nextElement();
		      ipAddr = inetAddr.getHostAddress();

			  // 
			  if(!checkConnectionIp(ipAddr))
				continue;
		  
			  if(ipAddr.matches(IPADDRESS_REGULAR)) {
			    
				  System.out.println(ipAddr);
				  return ipAddr;
		      }
		  }
	      }
	      
	      return "";
	}

	public boolean checkConnectionIp(String ip) {
						
		try {
			String str = String.valueOf(10);
			byte bufSize[] = str.getBytes();
			DatagramSocket socketBuffer = new DatagramSocket();
			packet = new DatagramPacket(bufSize, bufSize.length,
				new InetSocketAddress(ip, 4445));

			socketBuffer.send(packet);

		} catch (PortUnreachableException e) {
			e.printStackTrace();
			return false;
		} catch (IOException e) {
			e.printStackTrace();
			return false;
		} catch (SecurityException e) {
			e.printStackTrace();
			return false;
		} catch (IllegalBlockingModeException e) {
			e.printStackTrace();
			return false;
		}

		return true;
	}
	
	public static boolean loadConfiguration() {
	    Properties prop = new Properties();
	    String configPath = "/.config/SmartRoom/SmartRoom.ini";
	    String userHome = System.getProperty("user.home");
	    File file = new File(userHome + configPath);
	    
	    if(!file.exists())
		createConfigurationFile(file);
	    
	    try {
		prop.load(new FileInputStream(file));
		
		MicServer.SS_IP = prop.getProperty("ip", "194.85.173.9");
		MicServer.SS_NAME = prop.getProperty("name", "X");
		MicServer.SS_PORT = Integer.parseInt(prop.getProperty("port", "10011"));
	    
	    } catch(IOException e) {
		e.printStackTrace();
		return false;
	    }
	    
	    return true;
	}
	
	public static void startExitThread() {
		// Thread for correct stop of a server
		new Thread() {
		    @Override
		    public void run() {
			final BufferedReader reader = new BufferedReader(
				new InputStreamReader(System.in));
			System.out.println("Press 'q' to exit program.\n");
			while(true) {
			    try {
				if(reader.readLine().equals("q") ||
				    reader.readLine().equals("Q")) {
				    deletePublishedData();
					
					if(socket != null)
					    socket.close();

				    System.exit(0);
				}
			    } catch (IOException e) {
				e.printStackTrace();
			    }
			}
		    }
		}.start();
	}
	
	public static void createConfigurationFile(File file) {
	    Properties prop = new Properties();
    
	    try {
		file.createNewFile();
	
		if(!file.canWrite())
		    System.out.println("Can't write to config file");
		    
		prop.setProperty("ip", "194.85.173.9");
		prop.setProperty("port", "10011");
		prop.setProperty("name", "X");
	
		prop.store(new FileOutputStream(file), null);
    
	    } catch (IOException e) {
		e.printStackTrace();
	    } catch (SecurityException e) {
		e.printStackTrace();
	    }
	}
}
