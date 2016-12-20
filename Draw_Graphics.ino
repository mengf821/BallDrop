#include "bitmap.h"

//Draw an obstacle with left corner coordinates (x,y)
void drawObstacle(int x, int y)
{
  OrbitOledMoveTo(x, y);
  OrbitOledPutBmp(20, 8, horizontalObstacle);
}

//draws a ball with left corner coordinates (leftCornerX, leftCornerY)
void drawBall(int leftCornerX, int leftCornerY)
{
  
  OrbitOledMoveTo(leftCornerX,leftCornerY);
  
  if(leftCornerY >=0)
    OrbitOledPutBmp(11,11,ball);
    
  else
    OrbitOledPutBmp(11,11+leftCornerY,halfBall[-leftCornerY-1]);
   
  OrbitOledUpdate();
}

//Draws two cliffs with apples and/or obstacles 
void drawCliff(int platformHeight[2], int cliffWidth, bool obstacles[2], int cliff[2], int maxX, int applePosition[2], int appleDimension)
{
  OrbitOledSetDrawMode(modOledOr);
  OrbitOledMoveTo(0, platformHeight[0]);
  OrbitOledLineTo(cliff[0], platformHeight[0]);
  OrbitOledMoveTo(cliff[0] + cliffWidth, platformHeight[0]);
  OrbitOledLineTo(maxX, platformHeight[0]);
  OrbitOledMoveTo(0, platformHeight[1]);
  OrbitOledLineTo(cliff[1], platformHeight[1]);
  OrbitOledMoveTo(cliff[1] + cliffWidth, platformHeight[1]);
  OrbitOledLineTo(maxX, platformHeight[1]);

  for(int i = 0; i < 2; i++)
  {
    if(obstacles[i])
      drawObstacle(cliff[i], platformHeight[i]);
      
    if(applePosition[i]!=-1)
      drawApple(applePosition[i], platformHeight[i], appleDimension); 
  }
}

//draws an apple given the height of the cliff it is on and the position of the apple
void drawApple(int position, int height, int appleDimension)
{
  int leftCorner = height - appleDimension;
  OrbitOledMoveTo(position, leftCorner);
  if(height >= appleDimension)
    OrbitOledPutBmp(appleDimension, appleDimension, apple);
  else
    OrbitOledPutBmp(appleDimension, appleDimension, halfApple[-leftCorner - 1]);
   
}

