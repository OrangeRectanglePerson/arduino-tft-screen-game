#ifndef SPRITE_H
#define SPRITE_H

typedef struct Sprite {
  byte numOfShapes;
  double centerX;
  double centerY;
  Shape* shapes[];
};

Sprite* newSprite(byte numShapes, double x, double y );

void destroySprite(Sprite* s);

void translateSprite(Sprite* s, double x0, double y0, double x1, double y1);

void moveSprite(Sprite*s, double x, double y);



#endif