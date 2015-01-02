import processing.serial.*;

Serial port;  // Create object from Serial class

public static final char TAGP  = 'P';
public static final char TAGM  = 'M';

boolean sending;

void ardu_setup(){
   
  try{
     port = new Serial(this, "/dev/cu.usbmodem1421", 57600);
   }catch(Exception e){
     port = new Serial(this, port.list()[0], 57600);
   }
   port.bufferUntil('\n');
}

void sendMessage(char tag, int value){
    try{
      sending = true;
      port.write(tag);
      char c = (char)(value / 256); // msb
      port.write(c);
      c = (char)(value & 0xff);  // lsb
      port.write(c); 
      sending = false;
    }catch(Exception e){
    
    }
  
}

void serialEvent(Serial myPort) {
  String input = myPort.readString();
  input = input.replaceAll("\\s","");
  if(input != null) println(input);
  
   newGameReady = true;
   println("ready to play");
   
  if(input.indexOf("done") > 0 ){
  
  }

  if(input.indexOf("reset_done")> 0 ){
    newGameReady = true;
    trigCount = 0;
    println("ready to play");
  }
}
