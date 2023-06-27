#include "Arduino.h"
#include "shape.hh"
#include "sprite.hh"

Sprite* newSprite(byte numShapes, double x, double y ){
  Shape* shapes[numShapes];
  
  Sprite* s = malloc(sizeof(byte) + 2*sizeof(double) + sizeof(shapes));

  s->numOfShapes = numShapes; //byte
  s->centerX = x; //double
  s->centerY = y; //double
  memmove(s->shapes, shapes, sizeof(shapes)); //shapes

  return s;
}

void destroySprite(Sprite* s){
  for(byte i = 0; i < s->numOfShapes; i++) free(s->shapes[i]);
  free(s);
}

void translateSprite(Sprite* s, double x0, double y0, double x1, double y1){
  // matrix multiplication of all shapes in the sprite s by 
  // x0 y0 v x
  // x1 y1 ^ y
  //
  // newX = x0*x + y0*y
  // newX = x1*x + y1*y
  for(byte i = 0; i < s->numOfShapes; i++){
    translateShape(s->shapes[i],x0,y0,x1,y1);
    double newX = x0*s->shapes[i]->centerX + y0*s->shapes[i]->centerY;
    double newY = x1*s->shapes[i]->centerX + y1*s->shapes[i]->centerY;
    s->shapes[i]->centerX = newX;
    s->shapes[i]->centerY = newY;    
  }
}

void moveSprite(Sprite*s, double x, double y){
  s->centerX += x; s->centerY += y;  
}



