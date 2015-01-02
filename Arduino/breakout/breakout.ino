
#include <Adafruit_GFX.h>   // https://github.com/adafruit/Adafruit-GFX-Library
#include <RGBmatrixPanel.h> // https://github.com/adafruit/RGB-matrix-Panel

// RGB panel port numbers
#define CLK 11  // MUST be on PORTB! (Use pin 11 on Mega)
#define OE  9
#define LAT 10
#define A   A0
#define B   A1
#define C   A2
#define D   A3
#define blockNum 32

// Serial comm tags
#define TAGP         'P'
#define TAGM         'M'
#define MESSAGE_BYTES  5  // the total bytes in a message


// Last parameter = 'true' enables double-buffering, for flicker-free,
// buttery smooth animation.  Note that NOTHING WILL SHOW ON THE DISPLAY
// until the first call to swapBuffers().  This is normal.
RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE, false);

// static colors
uint16_t black = matrix.Color444(0, 0, 0);
uint16_t white = matrix.Color444(15, 15, 15);

uint16_t blue = matrix.Color444(0, 0, 15);
uint16_t red = matrix.Color444(15, 0, 0);
uint16_t green = matrix.Color444(0, 15, 0);

int paddleWidth=5;
int paddleHeight=2;

int ballDiameter=1;
int paddleX = 16;
int paddleY = 28;
int oldPaddleX, oldPaddleY;
int ballDirectionX = 1;
int ballDirectionY = 1;

int ballSpeed = 400; //lower number means faster speed of ball movement

int ballX, ballY, oldBallX, oldBallY;
boolean go = false;

int blockWidth=4;
int blockHeight=1;

uint16_t colors[blockNum];
boolean blockLive[blockNum];
int blockX[blockNum];
int blockY[blockNum];

boolean gameState_pre;

int textX = 5;


void showFeedBack(int beginX, String first, String second){
     
    
    for(int i = 0; i < 3; i++){
        int rt = random(400) + 500;
        matrix.setCursor(beginX, 5); 
        matrix.setTextColor(matrix.Color333(7,7,7));
        matrix.println(first);
        matrix.setCursor(beginX, 12); 
        matrix.println(second);
        delay(rt);
        clear();
        delay(rt);
      }
      
  // delay(1000);
  
    matrix.setCursor(beginX, 5); 
    matrix.setTextColor(matrix.Color333(7,7,7));
    matrix.println(first);
    matrix.setCursor(beginX, 12); 
    matrix.println(second);
 
   delay(3000);     
   clear();
   
}


void setup() {
  matrix.begin();
  Serial.begin(57600);
  delay(500);
  for(int i = 0; i < blockNum; i++){
    
    blockLive[i] = true;
    blockX[i] = (i%8)*4;
    if(i < 8) blockY[i] = 3;
    else if(i >= 8 && i < 16) blockY[i] =4;
    else if(i >= 16 && i < 24) blockY[i] = 5;
    else blockY[i] = 6;
    
    if(i/8 == 0) colors[i] = blue;
    else if(i/8 == 1) colors[i] = red;
    else if(i/8 ==2) colors[i] = green;
     else if(i/8 ==3) colors[i] = white;

  }
  
  clear();
  delay(500);
  drawBlocks();
  delay(500);
  initBallPaddle();
       
  Serial.println("done");
}

void initBallPaddle(){
  ballX = paddleX + 2;
  ballY = paddleY - 1;
  oldBallX = ballX;
  oldBallY = ballY;
  oldPaddleX = paddleX;
  oldPaddleY = paddleY;
  
  ballDirectionY = -1;
  // draw the ball's current position
  matrix.fillRect(ballX, ballY, ballDiameter, ballDiameter,white);
}

void clear() {
  matrix.fillScreen(black);
}


// get Leap motion input from Processing code
void getLeap(){

  if ( Serial.available() >= MESSAGE_BYTES)
  {

      char tag = Serial.read();
      if(tag == TAGP)
      {

        int val = Serial.read() * 256;
        val = val + Serial.read();
        paddleX = val;
      
        if(!go) {
          ballX = paddleX + 2;
          ballY = paddleY - 1;
        }
      }
      
      
       if(tag == TAGM)
      {
        int val = Serial.read() * 256;
        val = val + Serial.read();

        if(!go && val == 1){
         
          ballX = paddleX + 2;
          ballY = paddleY - 1 ;
          ballDirectionY = -1;
          go = true;
          
        }
      }
  }
}


void loop() {
  int myWidth = matrix.width();
  int myHeight = matrix.height();
 
  getLeap();
  movePaddle();
  
  if(go){

    if (millis() % (ballSpeed/2) < 2) {
      moveBall();
    }
  }else{
  
      moveBall_idle();
  }
 
  // check game status
  if(checkBlocks() != gameState_pre ){
  
    if(checkBlocks()){
      // win the game
      clear();
      delay(500);
      showFeedBack(6, "you", "win");
      
      resetBlocks();
      go = false;
      Serial.println("reset_done");
      gameState_pre = false;
     
    }
    
    gameState_pre;
  }
}



boolean checkBlocks(){
  
  for(int i = 0; i< blockNum; i++){
    if( blockLive[i] ) return false;
  }
  
  return true;
}

void resetBlocks(){
   for(int i = 0; i< blockNum; i++){
   blockLive[i] = true;
  }
  
  drawBlocks();
  initBallPaddle();
}
void drawBlocks(){
  for(int i = 0; i< blockNum; i++){
    if(blockLive[i]) matrix.fillRect(blockX[i], blockY[i], blockWidth, blockHeight, colors[i]);
  }
}

void moveBall_idle(){
  // erase the ball's previous position
  if (oldBallX != ballX || oldBallY != ballY) {
    matrix.fillRect(oldBallX, oldBallY, ballDiameter, ballDiameter,black);
  }
  // draw the ball's current position
  matrix.fillRect(ballX, ballY, ballDiameter, ballDiameter,white);
 
  oldBallX = ballX;
  oldBallY = ballY;
}

void movePaddle(){
 if (oldPaddleX != paddleX || oldPaddleY != paddleY) {
    matrix.fillRect(oldPaddleX, oldPaddleY, paddleWidth, paddleHeight,black);
  }
  matrix.fillRect(paddleX, paddleY, paddleWidth, paddleHeight,blue);
  
  oldPaddleX = paddleX;
  oldPaddleY = paddleY;
}

void quicker(){
  if (ballSpeed>20) ballSpeed--;
}


void moveBall() {

  if (ballX > matrix.width()-1 || ballX < 0) {
    ballDirectionX = -ballDirectionX;
   // quicker();
  }
 
  if (ballY < 0) {
    ballDirectionY = 1;
   // quicker();
  }  
  
  // if ball goes out of range, set game as lost
  if(ballY == matrix.height() && go){
    clear();
    
    showFeedBack(6, "you", "lost");
    delay(500);
    resetBlocks();
    go = false;
    Serial.println("reset_done");
  }
  
 
  // check if the ball and the paddle occupy the same space on screen
  if (inPaddle(ballX, ballY, paddleX, paddleY, paddleWidth, paddleHeight)) {
    if(go) ballDirectionY = -1;
   // quicker();
  }
  
  for(int i = 0; i< blockNum; i++){
     if (inBlock(ballX, ballY, blockX[i], blockY[i]) && blockLive[i]) {
        ballDirectionY = ballDirectionY*-1;
        matrix.fillRect( blockX[i], blockY[i], blockWidth, blockHeight, black);
        blockLive[i] = false;

    }
  }
 
  // update the ball's position
  ballX += ballDirectionX;
  ballY += ballDirectionY;
 
  // erase the ball's previous position
  if (oldBallX != ballX || oldBallY != ballY) {
    matrix.fillRect(oldBallX, oldBallY, ballDiameter, ballDiameter,black);
  }
  // draw the ball's current position
  matrix.fillRect(ballX, ballY, ballDiameter, ballDiameter,white);
 
  oldBallX = ballX;
  oldBallY = ballY;
}


boolean inPaddle(int x, int y, int rectX, int rectY, int rectWidth, int rectHeight) {
  boolean result = false;
   
  if ((x >= rectX && x <= (rectX + rectWidth)) &&
      (y == rectY)) {
       result = true;
  }
  return result;  
}

boolean inBlock(int x, int y, int rectX, int rectY) {
  boolean result = false;
   
  if ((x >= rectX && x <= (rectX + blockWidth)) &&
      (y == rectY)) {
       result = true;
  }
  return result;  
}

