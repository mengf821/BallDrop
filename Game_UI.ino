#include <stdbool.h>
#include <string.h>
#include <math.h>


static enum GamePages
{
  Welcome       = 0,
  Instruction   = 1,
  GamePage      = 2,
  GameOver      = 3,
  Score         = 4,
  Restart       = 5,
  ChoosePlayer  = 6,
} gameUiPage = Welcome;


const uint32_t ButtonCount = 2;
const uint32_t Buttons[2] = {PD_2, PE_0};
const int MaxX = 128;
const int MaxY = 32;
const int Potentiometer = PE_3;
const int TerminalSpeed = 35;
const int Radius = 5;
const int CliffWidth = 20;
const int TimePerUpdate = 20;
const int HeightBetween = 16;
const int AppleDimension = 7;
const int NumberOfCliffs = 2;


struct BallState
{
  int centerX;
  int centerY;
  int bottomHeight;
  int leftCornerX;
  int leftCornerY;
};

struct graphicsState
{
  int platformHeight[NumberOfCliffs];
  int cliff[NumberOfCliffs];
  bool obstacles[NumberOfCliffs];
  struct BallState ball;
  int applePosition[NumberOfCliffs];
}gameGraphics;


struct StateChange
{
  unsigned long currentTime;
  unsigned long previousUpdateTime;
  unsigned long previousShiftTime;
  int timePerShift;
  int score;
  int highScore;
  int player;
  int count;
}gameStateChange;

void GameUIInit()
{
  OrbitOledInit();
  OrbitOledClear();
  OrbitOledClearBuffer();
  OrbitOledSetFillPattern(OrbitOledGetStdPattern(iptnSolid));
  OrbitOledSetDrawMode(modOledSet);
 
  
  for(int i = 0; i < ButtonCount; i++)
    pinMode(Buttons[i], INPUT);
    
  pinMode(Potentiometer, INPUT);
  
  randomSeed(millis());
}

static void handlePageGame()
{
  gameStateChange.currentTime = millis();
  gameStateChange.previousShiftTime = millis();
  gameStateChange.previousUpdateTime = millis();
  gameStateChange.timePerShift = 150;
  gameStateChange.score = 0;
  gameStateChange.count = 0;
  gameGraphics.platformHeight[0] = 16;
  gameGraphics.platformHeight[1] = gameGraphics.platformHeight[0] + HeightBetween;
  gameGraphics.cliff[0] = 0;
  gameGraphics.cliff[1] = 0;
  gameGraphics.ball.centerX = 5;
  gameGraphics.ball.centerY = gameGraphics.platformHeight[0] - 6;
  updateBallState(gameGraphics.ball.centerX, gameGraphics.ball.centerY);

  gameGraphics.cliff[0] = generateCliff();
  do
  {
    gameGraphics.cliff[1] = generateCliff();
  }while(gameGraphics.cliff[1] >= gameGraphics.cliff[0] && 
         gameGraphics.cliff[1] <= gameGraphics.cliff[0] + CliffWidth || 
         gameGraphics.cliff[1] + CliffWidth > gameGraphics.cliff[0] && 
         gameGraphics.cliff[1] + CliffWidth < gameGraphics.cliff[0] + CliffWidth);

  gameGraphics.obstacles[0] = generateObstacle();
  gameGraphics.obstacles[1] = generateObstacle();

  for(int i = 0; i < NumberOfCliffs; i++)
  {
    if(generateApple())
    {
      gameGraphics.applePosition[i] = generateApplePosition(gameGraphics.cliff[i]);
    }
    else
    {
      gameGraphics.applePosition[i] = -1;
    }
  }

  while(gameGraphics.applePosition[0]!= -1 && gameGraphics.applePosition[0] < 11)
  {
     gameGraphics.applePosition[0] = generateApplePosition(gameGraphics.cliff[0]);
  }
  
  drawCliff(gameGraphics.platformHeight, CliffWidth, gameGraphics.obstacles, 
    gameGraphics.cliff, MaxX, gameGraphics.applePosition, AppleDimension);
  drawBall(gameGraphics.ball.leftCornerX,gameGraphics.ball.leftCornerY);
  OrbitOledUpdate();
  
  while(notLosing())
  {
    OrbitOledUpdate();
    
    TiltTick();
    updateObjectPositions();
    if(updateTimePassed())
    {
      OrbitOledClearBuffer();
      OrbitOledClear();
      drawCliff(gameGraphics.platformHeight, CliffWidth, gameGraphics.obstacles, 
        gameGraphics.cliff, MaxX, gameGraphics.applePosition, AppleDimension);
      drawBall(gameGraphics.ball.leftCornerX, gameGraphics.ball.leftCornerY);
      OrbitOledUpdate();
    }   
  }

  OrbitOledClear();
  OrbitOledClearBuffer();
  OrbitOledUpdate();
  
  gameUiPage = GameOver;
}



static void updateObjectPositions()
{
  gameStateChange.currentTime = millis();
  
  if(!isDropping())
  {
    if(gameStateChange.currentTime%4 == 0)
     {
      (gameGraphics.ball.centerX)+=tiltDirection();
      updateBallState(gameGraphics.ball.centerX, gameGraphics.ball.centerY);
     }

    if(getApple() >= 0)
    {
      gameGraphics.applePosition[getApple()] = -1;
      gameStateChange.score += 5;
    }
  }
  
  if(gameStateChange.currentTime - gameStateChange.previousShiftTime > gameStateChange.timePerShift)
  {
    switch (++gameStateChange.count % 2) {
      case 0:
        digitalWrite(RED_LED, HIGH);
        break;
      case 1:
        digitalWrite(RED_LED, LOW);
      }
     gameGraphics.platformHeight[0]--;
     gameGraphics.platformHeight[1]--;
     gameGraphics.ball.centerY--; 
     updateBallState(gameGraphics.ball.centerX, gameGraphics.ball.centerY);
    
     if(gameGraphics.platformHeight[0] < 0)
     {
       
       gameGraphics.platformHeight[0] = gameGraphics.platformHeight[1];
       gameGraphics.platformHeight[1] = gameGraphics.platformHeight[0] + HeightBetween;
       gameGraphics.cliff[0] = gameGraphics.cliff[1];
       gameGraphics.applePosition[0] = gameGraphics.applePosition[1];

    
       do
       {
        gameGraphics.cliff[1] = generateCliff();
       }while(gameGraphics.cliff[1] >= gameGraphics.cliff[0] && 
              gameGraphics.cliff[1] <= gameGraphics.cliff[0] + CliffWidth || 
              gameGraphics.cliff[1] + CliffWidth > gameGraphics.cliff[0] && 
              gameGraphics.cliff[1] + CliffWidth < gameGraphics.cliff[0] + CliffWidth);
              
        gameGraphics.obstacles[0] = gameGraphics.obstacles[1];
        gameGraphics.obstacles[1] = generateObstacle();
       if(gameStateChange.timePerShift >= TerminalSpeed)
          gameStateChange.timePerShift *=0.95;

      if(generateApple())
        gameGraphics.applePosition[1] = generateApplePosition(gameGraphics.cliff[1]);
      else
       gameGraphics.applePosition[1] = -1;
     }


    gameStateChange.previousShiftTime = gameStateChange.currentTime;
  }
 
  if(isDropping())
  {
    if(gameStateChange.currentTime%2 == 0)
    {
      gameGraphics.ball.centerY++;
      updateBallState(gameGraphics.ball.centerX, gameGraphics.ball.centerY);
      if(gameGraphics.ball.bottomHeight == gameGraphics.platformHeight[1])
      {
        gameStateChange.score++;
      }
    }
  }
  
  updateBallState(gameGraphics.ball.centerX, gameGraphics.ball.centerY);
}

void updateBallState(int centerX, int centerY)
{
  //sets center of the ball so the ball does not go outside the range of the screen
  if(gameGraphics.ball.centerX < Radius)
    gameGraphics.ball.centerX = Radius;
  if(gameGraphics.ball.centerX > MaxX - Radius)
    gameGraphics.ball.centerX = MaxX - Radius;
    
   gameGraphics.ball.bottomHeight = centerY + 6;
   gameGraphics.ball.leftCornerY = centerY - 5;
   gameGraphics.ball.leftCornerX = centerX - 5;
}

int generateCliff()//keep in mind the number generated here is pseudorandom thus the same sequence will always appear, this should be changed
{
  return random(0 + 10, MaxX - 10 - CliffWidth);
}

bool isDropping()
{
  bool inBetween[NumberOfCliffs] = {gameGraphics.ball.centerX - Radius >= gameGraphics.cliff[0] 
                       && gameGraphics.ball.centerX + Radius <= gameGraphics.cliff[0] + CliffWidth,
                       gameGraphics.ball.centerX - Radius >= gameGraphics.cliff[1] 
                       && gameGraphics.ball.centerX + Radius <= gameGraphics.cliff[1] + CliffWidth};
                        
  if(gameGraphics.ball.bottomHeight != gameGraphics.platformHeight[0] 
     && gameGraphics.ball.bottomHeight != gameGraphics.platformHeight[1])
    return true;

  if(analogRead(Buttons[1]) > 3000)//press button 2 to go through the cliff 
     return false;
          
  for(int i = 0; i < NumberOfCliffs; i++)
  {
    if(gameGraphics.ball.bottomHeight == gameGraphics.platformHeight[i])
    {
      if(inBetween[i])
      {   
         if(!gameGraphics.obstacles[i])
          return true;
         else
         {
            if(analogRead(Buttons[0]) > 3000)
              return true;
         }
      } 
    }
  }
  return false; 
}

bool updateTimePassed()
{
  gameStateChange.currentTime = millis();
  if(gameStateChange.currentTime - gameStateChange.previousUpdateTime > TimePerUpdate)
  {
    gameStateChange.previousUpdateTime = gameStateChange.currentTime;
    return true;
  }

  return false;
}

bool notLosing()
{
  if(gameGraphics.ball.bottomHeight <= 0)
    return false;
    
  if(gameGraphics.ball.centerY >= MaxY)
    return false;
  
  return true;
}


bool generateObstacle()
{
  if(random(1, 5) == 1)
    return true;
  return false;
}

bool generateApple()
{
  if(random(1, 5) == 1)
    return true;
  return false;
}

int generateApplePosition(int cliff)
{
  int position;
  do
  {
    position = random(1, MaxX - AppleDimension);
  }while(position >= cliff && position <= cliff + CliffWidth 
         || position + AppleDimension >=cliff && position + AppleDimension <= cliff + CliffWidth);
  return position;
}

int getApple()
{
  for(int i = 0; i < NumberOfCliffs; i++)
  {
    if(gameGraphics.ball.bottomHeight == gameGraphics.platformHeight[i])
    {
        if((gameGraphics.ball.leftCornerX + 11 > gameGraphics.applePosition[i] 
          && gameGraphics.ball.leftCornerX + 11 < gameGraphics.applePosition[i] +7
          ||gameGraphics.ball.leftCornerX > gameGraphics.applePosition[i] 
          && gameGraphics.ball.leftCornerX < gameGraphics.applePosition[i] +7)
          && gameGraphics.applePosition[i] != -1)
           return i;
    }
  }

  return -1;
}

static void handlePageWelcome() {
  gameStateChange.highScore = 0;
  gameStateChange.currentTime = millis();
  OrbitOledSetDrawMode(modOledSet);
  OrbitOledMoveTo(2,0);
  OrbitOledDrawString("Welcome!");
  OrbitOledMoveTo(2,10);
  OrbitOledDrawString("Press BTN1");
  OrbitOledMoveTo(2,20);
  OrbitOledDrawString("to continue!");
  OrbitOledUpdate();
  
  detectButtonRelease(Buttons[0]);
  
  OrbitOledClear();
  
  gameUiPage = Instruction;
}

static void handlePageInstruction() {
  gameStateChange.currentTime = millis();
  OrbitOledSetDrawMode(modOledSet);
  OrbitOledMoveTo(2,0);
  OrbitOledDrawString("Instruction");
  OrbitOledMoveTo(2,10);
  OrbitOledDrawString("1. Tilt to move");
  OrbitOledMoveTo(2,20);
  OrbitOledDrawString("the ball");
  OrbitOledUpdate();

  detectButtonRelease(Buttons[0]);
  
  OrbitOledMoveTo(2,5);
  OrbitOledDrawString("2. Press BTN1");
  OrbitOledMoveTo(2,15);
  OrbitOledDrawString("to go across");
  drawObstacle(100, 17);
  OrbitOledUpdate();
  
  detectButtonRelease(Buttons[0]);
  
  OrbitOledMoveTo(2,0);
  OrbitOledDrawString("3. Press BTN2");
  OrbitOledMoveTo(2,10);
  OrbitOledDrawString("to go acorss");
  OrbitOledMoveTo(2,20);
  OrbitOledDrawString("the gap");
  OrbitOledUpdate();
  
  detectButtonRelease(Buttons[0]);
  
  OrbitOledMoveTo(2,0);
  OrbitOledDrawString("4. Eat");
  drawApple(55, 7, AppleDimension);
  OrbitOledMoveTo(68, 0);
  OrbitOledDrawString("or drop");
  OrbitOledMoveTo(2, 10);
  OrbitOledDrawString("to get higher");
  OrbitOledMoveTo(2, 20);
  OrbitOledDrawString("score");
  OrbitOledUpdate();
  
  detectButtonRelease(Buttons[0]);
  
  OrbitOledMoveTo(10,12);
  OrbitOledDrawString("ARE YOU READY?");
  OrbitOledUpdate();
  detectButtonRelease(Buttons[0]);
   
  gameUiPage = ChoosePlayer;
}

void handlePageChoosePlayer()
{
  gameStateChange.player = 0;
  OrbitOledMoveTo(30, 0);
  OrbitOledDrawString("Player");
  OrbitOledMoveTo(30, 10);
  OrbitOledDrawString("Other");
  OrbitOledUpdate();

  bool previousState[ButtonCount] = {false, false};
  bool currentState[ButtonCount] = {false, false};
  bool decisionMade = false;
  
  while(true)
  {
    for(int i = 0; i < ButtonCount; i++)
    {
      currentState[i] = analogRead(Buttons[i]) > 3000;

      if(!currentState[i] && previousState[i])
      {
        if(i == 1)
        {
          gameStateChange.player = (gameStateChange.player+1)%3;
          OrbitOledClearBuffer();
          OrbitOledClear();
          OrbitOledMoveTo(30, 0);
          OrbitOledDrawString("Player");
          OrbitOledMoveTo(30, 10);
          switch(gameStateChange.player)
          {
            case(0):
              OrbitOledDrawString("Other");
              break;
            case(1):
              OrbitOledDrawString("Bo");
              break;
            case(2):
              OrbitOledDrawString("Mengfei");
              break;
          }
          OrbitOledUpdate();
        }
        if(i == 0)
        {
          decisionMade = true;
          break;
        }
      }
      
      if(previousState[i]  != currentState[i])
        previousState[i] = currentState[i];
     }

     if(decisionMade)
      break;
  }
  
  
  OrbitOledClearBuffer();
  OrbitOledClear();
  gameUiPage = GamePage;
}

void handlePageGameOver()
{
  digitalWrite(RED_LED, HIGH);
  OrbitOledMoveTo(25,12);
  OrbitOledDrawString("GAME OVER");
  OrbitOledUpdate();
  gameStateChange.previousUpdateTime = millis();
  while(true)
  {
    gameStateChange.currentTime = millis();
    if(gameStateChange.currentTime - gameStateChange.previousUpdateTime > 1000)
      break;
  }
  OrbitOledClearBuffer();
  OrbitOledClear();
  gameUiPage = Score;
}

void handlePageScore()
{
  
  
  bool positive = true;
  bool zero = false;
  switch(gameStateChange.player)
  {
   case(1):
    positive = false;
    break;
   case(2):
    gameStateChange.score = gameStateChange.score*100;
    break;
   case(0):
    gameStateChange.score = gameStateChange.score*1;
    break;
  }
  if (gameStateChange.score > gameStateChange.highScore) {
    gameStateChange.highScore = gameStateChange.score;  
  }
  if(gameStateChange.score == 0)
    zero = true;
    
  OrbitOledMoveTo(25, 10);
  OrbitOledDrawString("Score:");
  int numberOfDigits = log10(gameStateChange.score) + 1;
  
  if(gameStateChange.score == 0)
    numberOfDigits = 1;
      
  char scoreDigits[numberOfDigits];

  for(int i = numberOfDigits-1; i>=0; i--)
  {
    scoreDigits[i] = (gameStateChange.score%10 + '0');
    gameStateChange.score/=10;
  }
  if(!positive && !zero)
    OrbitOledDrawChar(45);

  OrbitOledDrawString(scoreDigits);
  
  if (gameStateChange.highScore > 0) {
    int tempHighScore = gameStateChange.highScore;
    numberOfDigits = log10(tempHighScore) + 1;
    char hScoreDigits[numberOfDigits];
    for (int i = 0; i < numberOfDigits; i++) {
      hScoreDigits[numberOfDigits - 1 - i] = (tempHighScore%10) + '0';
      tempHighScore/=10;
    }
    OrbitOledMoveTo(5, 20);
    OrbitOledDrawString("High Score:");
    OrbitOledDrawString(hScoreDigits);
  }

  OrbitOledUpdate();

  detectButtonRelease(Buttons[0]);
  gameUiPage = Restart;
}

void handlePageRestart()
{
  OrbitOledMoveTo(35, 12);
  OrbitOledDrawString("RESTART?");
   OrbitOledUpdate();
  detectButtonRelease(Buttons[0]);
  gameUiPage = ChoosePlayer;
}

void detectButtonRelease(int button)
{
  bool previousState = false;
  
  while(true)
  {
    bool currentState = analogRead(button) > 3000;

    if(!currentState && previousState)
      break;
      
    if(previousState  != currentState)
      previousState = currentState;
  }
  
  OrbitOledClearBuffer();
  OrbitOledClear();
}


void GameUITick()
{
  switch(gameUiPage)
  {
  case Welcome:
    handlePageWelcome();
    break;
    
  case Instruction:
    handlePageInstruction();
    break;

  case GamePage:
    handlePageGame();
    break;

  case GameOver:
    handlePageGameOver();
    break;

  case Score:
    handlePageScore();
    break;

  case Restart:
    handlePageRestart();
    break;

  case ChoosePlayer:
    handlePageChoosePlayer();
    break;
  }
}

