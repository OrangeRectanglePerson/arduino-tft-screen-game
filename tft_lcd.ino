
#include <SPI.h>
#include <PDQ_GFX.h>    // Core graphics library
#include "PDQ_ST7735_config.h"
#include <PDQ_ST7735.h> // Hardware-specific library for ST7735
#include "shape.hh"
#include "sprite.hh"


#include "PDQ_wrapper.hh"

void drawShapeRespectToCenter(PDQ_ST7735* screen, Shape* s, double centerPtX, double centerPtY, bool clearScreen){
  if(clearScreen) screen->fillScreen(ST7735_BLACK); //clear screen

  if(s->connectAllPoints){
    for(byte i = 0; i < s->numOfPoints-1; i++){
      for(byte j = i+1; j < s->numOfPoints; j++){
        byte startPointIndex = i;
        byte endPointIndex = j;
  
        int16_t x0 = centerPtX + s->centerX + s->points[startPointIndex]->x;
        int16_t y0 = centerPtY + s->centerY + s->points[startPointIndex]->y;
        int16_t x1 = centerPtX + s->centerX + s->points[endPointIndex]->x;
        int16_t y1 = centerPtY + s->centerY + s->points[endPointIndex]->y;
  
        screen->drawLine(x0, y0, x1, y1, s->color); 

      } 
    } 
  }
  else {    
    for(byte i = 0; i < s->numOfPoints; i++){
      byte startPointIndex = i;
      byte endPointIndex = (i+1 != s->numOfPoints) ? i+1 : 0;

      int16_t x0 = centerPtX + s->centerX + s->points[startPointIndex]->x;
      int16_t y0 = centerPtY + s->centerY + s->points[startPointIndex]->y;
      int16_t x1 = centerPtX + s->centerX + s->points[endPointIndex]->x;
      int16_t y1 = centerPtY + s->centerY + s->points[endPointIndex]->y;

      screen->drawLine(x0, y0, x1, y1, s->color); 

    } 
  }
}

void drawShape(PDQ_ST7735* screen, Shape* s, bool clearScreen){  
  drawShapeRespectToCenter(screen, s, 0, 0, clearScreen);
}


void drawSprite(PDQ_ST7735* screen, Sprite* s, bool clearScreen){

  drawShapeRespectToCenter(screen, s->shapes[0], s->centerX, s->centerY, clearScreen);

  
  for(byte i = 1; i < s->numOfShapes; i++){
    drawShapeRespectToCenter(screen, s->shapes[i], s->centerX, s->centerY, false);
  }
 
}

#define RESET 0

#define FPS 10
#define BCPS 25  //how many times to check for button pressed events in a second
#define PLAYERWIDTH 15

#define PI 3.1415926535897932384626433832795

#define MAX_SIDES 100


struct ScriptSentences {
  //remember to add 1 to string len for null terminator
  char a[16+1] = "The Bus is here!";
  char placeholdertext1[31+1] = "I have arrived at the bus door!";
};

const ScriptSentences scriptSentences PROGMEM ;

PDQ_ST7735* tft = new PDQ_ST7735();
unsigned long lastrefresh;  //store millis() of last time screen refreshed

byte playerCoords[2] = {65,0};
struct ButtonsPressed {
  bool up; bool down; bool left; bool right; 
  bool A; bool B;
  unsigned long lastButtonCheck; //store millis() of last time a check of what buttons are pressed ocurred
} buttonsPressed;
unsigned long counter = 0;

Sprite* sprite;

Sprite* player;

Shape* arrow;

enum InteractableAreas{
  NONE,
  BUS
} lastInteractableEncountered;
struct WhereIsPlayer {
  bool bus; 
  bool anotherArea;
} isPlayerIn;
bool isPlayerSomewhere() {
   return(isPlayerIn.bus || isPlayerIn.anotherArea);
}
char* textboxText = NULL;
bool playerMovementAllowed = true;

void setup() {
  // put your setup code here, to run once:

  lastrefresh = millis();

  arrow = newShape(7, 90, 90, false, ST7735_GREEN);
  editPoint(arrow->points[0], 0, 20)   ;
  editPoint(arrow->points[1], -20, 0)  ;
  editPoint(arrow->points[2], -8, 0)   ;
  editPoint(arrow->points[3], -8, -20) ;
  editPoint(arrow->points[4], 8, -20)  ;
  editPoint(arrow->points[5], 8, 0)    ;
  editPoint(arrow->points[6], 20, 0)   ;  

  translateShape(arrow, 0, 1, -1, 0);

  sprite = newSprite(7, 64, 50);
  Shape* &s1 = sprite->shapes[0]; // car
  Shape* &s2 = sprite->shapes[1]; // front wheel
  Shape* &s3 = sprite->shapes[2]; // back wheel
  Shape* &s4 = sprite->shapes[3]; // door
  Shape* &s5 = sprite->shapes[4]; // front window
  Shape* &s6 = sprite->shapes[5]; // mid window
  Shape* &s7 = sprite->shapes[6]; // back window

  s1 = newShape(6, 0, 0, false, ST7735_RED);
  editPoint(s1->points[0], 50, 0)    ;
  editPoint(s1->points[1], 50, -20)  ;
  editPoint(s1->points[2], 40, -20)  ;
  editPoint(s1->points[3], 20, -40)  ;
  editPoint(s1->points[4], -60, -40) ;
  editPoint(s1->points[5], -60, 0)   ;

  s2 = newShape(6, 30, 0, true, ST7735_WHITE);

  for(int i = 0; i < s2->numOfPoints; i++){
    editPoint(s2->points[i], 10*cos((2*PI/s2->numOfPoints)*i), 10*sin((2*PI/s2->numOfPoints)*i));
  }

  s3 = newShape(6, -30, 0, true, ST7735_WHITE);

  for(int i = 0; i < s3->numOfPoints; i++){
    editPoint(s3->points[i], 10*cos((2*PI/s3->numOfPoints)*i), 10*sin((2*PI/s3->numOfPoints)*i));
  }

  s4 = newShape(4, 10, -20, false, ST7735_BLUE);
  editPoint(s4->points[0], 8, 16)    ;
  editPoint(s4->points[1], -8, 16)  ;
  editPoint(s4->points[2], -8, -16)  ;
  editPoint(s4->points[3], 8, -16)  ;

  s5 = newShape(4, -10, -30, false, ST7735_BLUE);

  for(int i = 0; i < s5->numOfPoints; i++){
    editPoint(s5->points[i], 8*cos((2*PI/s5->numOfPoints)*i+PI/4), 8*sin((2*PI/s5->numOfPoints)*i+PI/4));
  }

  s6 = newShape(4, -30, -30, false, ST7735_BLUE);

  for(int i = 0; i < s6->numOfPoints; i++){
    editPoint(s6->points[i], 8*cos((2*PI/s6->numOfPoints)*i+PI/4), 8*sin((2*PI/s6->numOfPoints)*i+PI/4));
  }

  s7 = newShape(4, -50, -30, false, ST7735_BLUE);

  for(int i = 0; i < s7->numOfPoints; i++){
    editPoint(s7->points[i], 8*cos((2*PI/s7->numOfPoints)*i+PI/4), 8*sin((2*PI/s7->numOfPoints)*i+PI/4));
  }

  player = newSprite(2, 64, 50);
  Shape* &ps1 = player->shapes[0]; // head
  Shape* &ps2 = player->shapes[1]; // body

  ps1 = newShape(6, 0, -8, false, 0b1111100000011111);
  for(int i = 0; i < ps1->numOfPoints; i++){
    editPoint(ps1->points[i], 8*cos((2*PI/ps1->numOfPoints)*i), 8*sin((2*PI/ps1->numOfPoints)*i));
  }
  ps2 = newShape(4, 5, 0, false, ST7735_CYAN);
  editPoint(ps2->points[0], 0, 0)     ;
  editPoint(ps2->points[1], 5, 20)    ;
  editPoint(ps2->points[2], -15, 20)  ;
  editPoint(ps2->points[3], -10, 0)   ;

  tft->begin();

  pinMode(A0, INPUT_PULLUP); //left
  pinMode(A1, INPUT_PULLUP); //right
  pinMode(A2, INPUT_PULLUP); //up
  pinMode(A3, INPUT_PULLUP); //down
  pinMode(A4, INPUT_PULLUP); //A


  //Serial.begin(9600);

  firstDraw();  
}

void loop() {
  

  if(millis() - buttonsPressed.lastButtonCheck >= 1000/FPS){

    //detect buttons pressed
    buttonsPressed.left = digitalRead(A0) == LOW;
    buttonsPressed.right = digitalRead(A1) == LOW;
    buttonsPressed.up = digitalRead(A2) == LOW;
    buttonsPressed.down = digitalRead(A3) == LOW;
    buttonsPressed.A = digitalRead(A4) == LOW;

    if(playerMovementAllowed){
      if(buttonsPressed.left) player->centerX -= (player->centerX <= 0) ? 0 : 3; 
      if(buttonsPressed.right) player->centerX += (player->centerX >= 128) ? 0 : 3;
      if(buttonsPressed.up) player->centerY -= (player->centerY <= 0) ? 0 : 3; 
      if(buttonsPressed.down) player->centerY += (player->centerY >= 128) ? 0 : 3;
    } else{
      if(textboxText != NULL){ //if there is a textbox on screen
        if(buttonsPressed.A){
                    
          //delete textbox          
          free(textboxText);
          textboxText = NULL;
          //free player movement
          playerMovementAllowed = true;
          
        }  
      } else {
        //catchall to prevent softlock
        playerMovementAllowed = true;
      }
    }
  
    buttonsPressed.lastButtonCheck = millis();
  }

  if(millis() - lastrefresh >= 1000/FPS){

    draw();
  
    lastrefresh = millis();
  }
}

void firstDraw(){
  // method to call to setup the LCD
  // place static backdrops here, etc

  /*
  tft->setCursor(0, 0);
  tft->setTextColor(ST7735_WHITE);
  tft->setTextSize(2);
  tft->print(shape->numOfPoints);
  */

  drawShape(tft, arrow, true);

  drawSprite(tft, sprite, false);

  counter++;  
                    
}

void draw(){  
  //repeatedly call this method to update the screen (roughly) every 1000/FPS ms
  //blank out current player position and redraw the player triangle after updating coordinates

  //if(counter%12 == 0) sprite->shapes[0]->connectAllPoints = !sprite->shapes[0]->connectAllPoints;
  if(counter%6 == 0) arrow->color = ~arrow->color;   
    
  if (sprite->centerX > 200)  {
    sprite->centerX = -60;
  }
  
  translateShape(sprite->shapes[1], cos(PI/20), -sin(PI/20), sin(PI/20), cos(PI/20));
  translateShape(sprite->shapes[2], cos(PI/20), -sin(PI/20), sin(PI/20), cos(PI/20));

  //translateSprite(sprite, cos(PI/50), -sin(PI/50), sin(PI/50), cos(PI/50));

  drawShape(tft, arrow, true);
  
  
  drawText_P(0,0,ST7735_WHITE,1,scriptSentences.a);
  tft->print(((String) counter)); 

  Point* playerCenter =  malloc(sizeof(Point)); Point* busCenter =  malloc(sizeof(Point)); 
  editPoint(playerCenter, player->centerX, player->centerY);  
  //take door as center
  editPoint(busCenter, sprite->centerX + sprite->shapes[3]->centerX, sprite->centerY + sprite->shapes[3]->centerY);  
  isPlayerIn.bus = distanceBetweenPoints(playerCenter, busCenter) <= 20;
  //tft->print((String)distanceBetweenPoints(playerCenter, busCenter));
  free(playerCenter); free(busCenter);

  tft->print(isPlayerIn.bus ? F("true") : F("false")); 
  if(isPlayerIn.bus && textboxText == NULL && lastInteractableEncountered != BUS) {
    //create text for textbox only when player is in bus && there is no text in the textbox && last intereated object isnt the bus
    textboxText = malloc(strlen_P(scriptSentences.placeholdertext1)+1); //add 1 since strlen does not count null terminator
    strcpy_P(textboxText, scriptSentences.placeholdertext1);
    //set last interacted item as InteractableAreas.bus
    lastInteractableEncountered = BUS;
  } 

  drawSprite(tft, sprite, false);
  drawSprite(tft, player, false);

  if(!isPlayerSomewhere()) lastInteractableEncountered = NONE;
 
  if(textboxText != NULL)  {
    playerMovementAllowed = false;
    drawTextBox(textboxText);
  }

  //Serial.println(counter);

  counter++;
 
}

void drawText_P(int16_t x, int16_t y, uint16_t textColor, uint8_t textSize, char* text){
  
  tft->setCursor(x, y);
  tft->setTextColor(textColor);
  tft->setTextSize(textSize); // 1, 2 or 3
  
  for (byte k = 0; k < strlen_P(text); k++) {
    char myChar = pgm_read_byte_near(text + k);
    tft->print(myChar); 
    
  }
}

void drawText(int16_t x, int16_t y, uint16_t textColor, uint8_t textSize, char* text, byte paddingPx){
  byte widthAvailable = 128-2*paddingPx;
  tft->setTextColor(textColor);
  tft->setTextSize(textSize); // 1, 2 or 3
  byte charWidth = 6*textSize;
  byte charHeight = 8*textSize;
  byte numCharPerLine = widthAvailable/charWidth;
  for (byte k = 0; k < strlen(text); k++) {
    tft->setCursor(paddingPx+x+charWidth*(k%numCharPerLine), paddingPx+y+charHeight*(k/numCharPerLine));
    char myChar = text[k];
    tft->print(myChar);
  }
}

void drawTextBox(char* text){
  //make the textbox
  /*
  Shape* textbox = newShape(8, 5, 90, false, ST7735_WHITE);
  editPoint(textbox->points[0], 0, 5)     ;
  editPoint(textbox->points[1], 0, 25)  ;
  editPoint(textbox->points[2], 5, 30)  ;
  editPoint(textbox->points[3], 113, 30) ;  
  editPoint(textbox->points[4], 118, 25) ; 
  editPoint(textbox->points[5], 118, 5)    ; 
  editPoint(textbox->points[6], 113, 0)    ;
  editPoint(textbox->points[7], 5, 0)    ;
*/  
  
  tft->fillRoundRect(5, 90, 118, 33, 7, ST7735_BLACK);
  tft->drawRoundRect(5, 90, 118, 33, 7, ST7735_WHITE);

  //create the text
  drawText(0,95-10,ST7735_WHITE,1,text,10);

  //destory shape object used to create the text box
  //destroyShape(textbox);
}
