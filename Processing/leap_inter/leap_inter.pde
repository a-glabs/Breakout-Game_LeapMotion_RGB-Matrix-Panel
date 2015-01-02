// leap motion library for Processing
// https://github.com/voidplus/leap-motion-processing
import de.voidplus.leapmotion.*;

LeapMotion leap;
boolean isLeapReady;
int fingerCount_pre;
boolean newGameReady;

void setup(){
  size(800, 500, P3D);
  background(255);
  noStroke(); fill(50);
  
  leap = new LeapMotion(this);
  ardu_setup();
  newGameReady = false;
}
int mappedX_pre;

int trigCount;
int trigCount_pre;

void draw(){
  background(255);
  
  if(isLeapReady){
    for(int i = 0; i< leap.getHands().size();i++)
    {
     Hand hand = leap.getHands().get(i); 
    
     // process only the fist hand
     if(i == 0){
       hand.draw();
        int     hand_id          = hand.getId();
        PVector hand_position    = hand.getPosition();
        PVector hand_stabilized  = hand.getStabilizedPosition();
        PVector hand_direction   = hand.getDirection();
        PVector hand_dynamics    = hand.getDynamics();
        float   hand_roll        = hand.getRoll();
        float   hand_pitch       = hand.getPitch();
        float   hand_yaw         = hand.getYaw();
        float   hand_time        = hand.getTimeVisible();
        PVector sphere_position  = hand.getSpherePosition();
        float   sphere_radius    = hand.getSphereRadius();
        
        float mappedX = map(hand_stabilized.x, 100,700, -2, 30 );
        float constX = constrain(mappedX, -1, 28);
        
        if(int(constX) != mappedX_pre ){
        
          if(!sending)sendMessage(TAGP, int(constX));
          mappedX_pre = int(constX);
        }
        
        // process finger count to begin the game
        if( hand.getFingers().size() != fingerCount_pre){
          
          if(hand.getFingers().size() > 3) {
            if(newGameReady && trigCount !=0) {
              println("paper");
              
              if(!sending)sendMessage(TAGM, 1);
              newGameReady = false;
            }
          }
          else if(hand.getFingers().size() == 1 || hand.getFingers().size() == 0) {//
            if(newGameReady){
               println("rock"); 
               trigCount++;
            }
           
          }
          fingerCount_pre = hand.getFingers().size();
        }
         
        // FINGERS
        for(Finger finger : hand.getFingers()){
        
        // Basics
        finger.draw();
       
        }
     }
    }
  }
}


void keyPressed(){
  if(!sending)sendMessage(TAGM, 1);
}

void leapOnConnect(){
  println("Leap Motion Connect");
  isLeapReady = true;
}

