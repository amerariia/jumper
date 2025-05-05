#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <string> 


U8G2_SSD1306_128X64_NONAME_F_SW_I2C
u8g2(U8G2_R0, 14, 12, U8X8_PIN_NONE);

void setup() {
  u8g2.begin();
  Serial.begin(74880);
  
  pinMode(5, INPUT);
  pinMode(4, OUTPUT);

  u8g2.setFont(u8g2_font_7x14B_tr);
}

int counter = 0;
int lastButtonState = 0;
int maxWidth = 128;
int minWidth = 0;

//---------------------------------------------------------------------------------------
class palka{
  int speed = 5;
  public:
  int x1, y1, w, h;
  palka(int x1, int y1, int w, int h) : x1(x1), y1(y1), w(w), h(h) {  };

  void move(){
    x1 -= speed;
    if (x1 + w < minWidth){
      x1 = maxWidth;
      counter++;
    }
  }

  void draw(){
    u8g2.drawBox(x1, y1, w, h);
  }
};

//---------------------------------------------------------------------------------------
class ball{
  
  bool isJumping = false;
  int jumpFrame = 0;
  const int deltaFrameTime = 166;
  int lastFrameTime = 0;

  public:
  bool isAlive = true;
  int  x1 = 20, y1, r;

  ball(int y1, int r) : y1(y1), r(r) {  };

  void jump (){
    if (isJumping){
      return;
    }
    isJumping = true;
    lastFrameTime = millis();
  }

  void draw(){
    if (isJumping){
      drawJump();
    }
    else{
      drawIdle();
    }
  }

  private:
  void drawJump (){

    long currentFrameTime = millis();
    if (isAlive && (currentFrameTime - lastFrameTime >= deltaFrameTime)){
      jumpFrame++;
      lastFrameTime = currentFrameTime;
    }

    switch (jumpFrame){
      case 0:{
        y1 = 50;
        u8g2.drawEllipse(x1, y1, r + 2, r - 2);
        break;
      }
      case 1:{
        y1 = 38;
        u8g2.drawEllipse(x1, y1, r - 2, r + 2);
        break;
      }
      case 2:{
        y1 = 28;
        u8g2.drawEllipse(x1, y1, r - 1, r + 1);
        break;
      }
      case 3:{
        y1 = 23;
        u8g2.drawEllipse(x1, y1, r - 1, r + 1);
        break;
      }
      case 4:{
        y1 = 18;
        u8g2.drawEllipse(x1, y1, r, r);
        break;
      }
      case 5:{
        y1 = 23;
        u8g2.drawEllipse(x1, y1, r - 1, r + 1);
        break;
      }
      case 6:{
        y1 = 28;
        u8g2.drawEllipse(x1, y1, r - 1, r + 1);
        break;
      }
      case 7:{
        y1 = 38;
        u8g2.drawEllipse(x1, y1, r - 2, r + 2);
        break;
      }
      case 8:{
        y1 = 50;
        u8g2.drawEllipse(x1, y1, r + 2, r - 2);
        break;
      }
      case 9:{
        y1 = 48;
        u8g2.drawEllipse(x1, y1, r, r);
        jumpFrame = 0;
        isJumping = false;
      }
    }
  }

  void drawIdle (){
    u8g2.drawCircle(x1, y1, r);
  }
};

palka p (maxWidth, 41, 5, 15);
ball b (48, 7);

bool isButtonPressed (){
  digitalWrite(4, HIGH);

  int currentButtonState = digitalRead(5);

  if (currentButtonState != lastButtonState && currentButtonState == 1){
    lastButtonState = currentButtonState;
    return true;
  }
  lastButtonState = currentButtonState;
  return false;
}

bool isCollide(){
  bool isLeftPalkasSideIsInside = b.x1 + b.r > p.x1;
  bool isRightPalkasSideIsInside = b.x1 - b.r < p.x1 + p.w;
  bool isUpperPalkasSideIsInside = b.y1 + b.r > p.y1;
  
  return isLeftPalkasSideIsInside && isRightPalkasSideIsInside && isUpperPalkasSideIsInside;
}

void gameOver(){
  std::string s = "GAME OVER: " + std::to_string(counter);
  
  if(counter < 10){
    u8g2.drawStr(24, 28, s.c_str());
  }
  else{
    u8g2.drawStr(21, 28, s.c_str());
  }
}

void drawCounter(){
  std::string s = std::to_string(counter);
  if(counter < 10){
    u8g2.drawStr(61, 28, s.c_str());
  }
  else{
    u8g2.drawStr(58, 28, s.c_str());
  }
}

void loop() {

  if (isCollide()){
    b.isAlive = false;
  }

  if (b.isAlive){
    if (isButtonPressed()){
      b.jump();
    }

    p.move();
  }
 
  u8g2.clearBuffer();

  if(b.isAlive){
    drawCounter();
  }
  else{
    gameOver();
  }

  u8g2.drawLine(minWidth, 56, maxWidth, 56);

  b.draw();
  p.draw();

  u8g2.sendBuffer();

}