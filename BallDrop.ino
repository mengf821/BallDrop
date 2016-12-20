#include <delay.h>
#include <FillPat.h>
#include <LaunchPad.h>
#include <OrbitBoosterPackDefs.h>
#include <OrbitOled.h>
#include <OrbitOledChar.h>
#include <OrbitOledGrph.h>
#include <string.h>
#include <stdlib.h>

void WireInit();
void GameUIInit();
void TiltInit();

void TiltTick();
void GameUITick();

void setup() {
  
  Serial.begin(9600);
  pinMode(RED_LED, INPUT);
  
  WireInit();
  GameUIInit();
  TiltInit();
}

void loop() 
{
    GameUITick();
}




/*
 * Functions to be implemented:
 * Basic functions:
 * ✓1. Show graphics on the screen 
 * ✓2. Move the "ball" left and right with the buttons
 * ✓3.Being able to drop to the next level
 * ✓ Press button to go across obstacles4. Shake to go across the obstacles
 * ✓5. Make the platform move up
 * ✓6. Detect whether the ball is out of vision
 * 7. Have a score board
 * ✓8. Make a struct for the ball
 * ✓ 9. bugs to be fixed: ball shouldn't automatically drop after it disapperas from top
 * ✓10. disable left and right movement while ball is dropping otherwise the ball will overlap with the platform 
 * ✓11. make the image of the ball become smaller when disappearing 
 * ✓12. tidy up the code
 * ✓13. need to set an upper limit to the speed moving up when increasing speed
 * 14. tabs vs. space in coding (think Rollen mentioned in a document that space should be used)
 * ✓15. Platform should be moving with increasing speed
 * ✓ only horizontal ones16. generate obstacles
 * ✓solved because changed ways of implementing the game17. shake to move across obstacles
 * Half solved??18. Bug: ball drop acrosses the barrier when speed is too fast
 * ✓Bitmap is used 19. When using struct the graphics update weirdly
 * 20. Welcome page
 * 21. Page that explains how the game works
 * ✓22. Make it only update every 30ms
 * 23. set it so that buttons can't alway be pressed
 * 24. make a separate file only for graphics
 * 25. Calculate game score
 * 26. Speed proportional to amount tilt
 * 27. fix score related to apple
 * 
 * Make code more readable
 * 1. create a struct for ball and cliff and merge them in struct gamegraphics
 * 2. have a separate class for drawing functions 
 * 3. maybe a separate class for the game itself because it requires far more code than all other pages to be implemented
 * 
 * Other functions:
 * 1. Add temperature sensor feature somehow idk
 * 2. 
 */

