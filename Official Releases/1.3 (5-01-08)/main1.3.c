//***************************************************************
// Birdman's Shooter PSP
//
// Anthony Casagrande
//
// Project Start Date: 10/23/07
// Version 1.0: submitted to psp-hacks.com on 10/28/07
// Version 1.2: submitted to psp-hacks.com on 11/02/07
// Latest Revision: 4/30/08
//
// Change Log From 1.0 to 1.2:
// + Current Level diplayed on HUD and when you lose.
// + 3,2,1 Go! before each level and after each time you die.
// + Ability to move before enemies (for positioning)
// + Fixed enemy positions at beginning of levels
// + Powerup moves from top to bottom
// + Fixed explosion animation
// + Pausing is a little smoother
// + Enemies no longer get stuck to each other
// + New HUD background
// + Lives are displayed as mini-images in HUD  
// + New title screen splash image
// + New gameover text and sequence
// + New enemy colors
//
// Change Log From 1.2 to 1.3:
// + A sleek new interface
// + Gameover screen changed
// + New options screen
// + Options and high scores are saved
// + 10 backgrounds, 5 bullet colors, and 11 player's ship colors
// + Fixed some glitches with the powerups
// + Next level doesn't come until all bullets are gone
// + Ability to end game when paused
// + Ability to quit from main menu
// + Best high score is displayed on titlebar (instead of level)
// + Personal best (ignores preset scores) is shown on main menu
// + New high score table that displays: score, level, and accuracy for top 10
// + 10 preset high scores to try to beat
// + New bonus level with stats screen
// + Increased enemy speeds for beginning levels
// + Slightly decreased enemy bullet speed
// + Increased player's bullet speed
// + Changed powerup texture
// + Powerup falls a little less often
// + Completly revamped scoring system
// + Scores are slightly based off of accuracy
// + New loading screen with loading bar
// + Paused text, level intro, and 3,2,1 are now images
// + Level intro is now displayed in realtime
// + No more 3,2,1 at level begin (only when you die)
// + Spare lives are displayed on titlebar instead of total
// + Scores now use commas for thousands
// + Fixed a glitch with 3,2,1 when dying on last enemy bullet
// + Enemy bullets come directly out of the tip of the enemy
// + Bullets from top row enemies are shown underneath the front row enemies
// + You can now fire using the O button or R trigger
// + New powerup that slows enemies for a few seconds
// + Fixed a glitch when having no score or no accuracy at game over
// + Fixed some glitches with the player's bullets
// + Enemies are less likely to fire multiple rounds in a row
//***************************************************************

#include "variables.h"

int  main()
{
	//scePowerSetClockFrequency(333, 333, 111);

	pspDebugScreenInit();
	SetupCallbacks();
	initGraphics();

	//pspAudioInit();
	//MP3_Init(1);


	// get psp nickname
	//sceUtilityGetSystemParamString(1, nickname, 25);

	sprintf(buffer,"./images/psphacks.png");
	psphacks = loadImage(buffer);
	sprintf(buffer,"./images/border.png");
	border = loadImage(buffer);
	sprintf(buffer,"./images/meter.png");
	meter = loadImage(buffer);

	//CreateDummyScores();

	SetUpImages();
	ResetGame(); 
	GetOptions(1);

	//MP3_Load("music.mp3");
	//MP3_Play();

	//scePowerSetClockFrequency(222, 222, 111);
	//scePowerSetClockFrequency(20, 20, 10);

	if(0){
		//Image load failed
		printf("Image load failed!\n\nMake sure all of the images are in the folder named \"images\"\n");
	}
	else
	{
		SwitchLevel();
		ShowTitleScreen();

		//these set up the ability to use the analog stick
		sceCtrlSetSamplingCycle(0);
		sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);

		while (1)
		{
			sceCtrlReadBufferPositive(&pad, 1);

			enemyBulletsLeft = 0;
			CheckForEnemeyBulletsLeft();
			//includes code to find how many user bullets are left
			GetPlayerBulletsLeft();

			//makes sure all enemy bullets and all player bullets have left the screen
			if (!aliveCount && !enemyBulletsLeft && !playerBulletsLeft)
			{
				SwitchLevel(); 
				ShowLevelScreen();
				ResetAllBullets();
			}
			if (justStarted)
			{
				LevelBegin();
				ResetAllBullets();
			}
			else
			{
				CheckForUserPause();
				CheckForUserMove();
				
				//will only let you fire if there are enemies on screen
				if (aliveCount){
					CheckForUserBullet();
				}

				//CheckForAIBullet();
				//CheckForAIMove();

				UpdateUserBullets();

				if (!bonusLevel){
					MoveEnemies();
					CheckForEnemyBullets();
					UpdateEnemyBullets();
					CheckForPowerUp();
					if (drawPowerUp[0] || drawPowerUp[1]){
						DrawPowerUp();
					}
					if (frozen){
						GetPowerUpTimeLeft();
					}
				}else{
					MoveEnemiesBonusLevel();
				}
				DrawEngine();
			}
		}
	}
	sceKernelSleepThread();
	return 0;
}

int  AICollision(){
	for (k = 0; k < enemyBulletsMax; k++){
		if (drawEnemyBullet[k]){
			if (intersects(playerX, playerY, myshipWidth, myshipHeight, enemyBulletX[k], enemyBulletY[k], bulletWidth * 2, bulletHeight * 2))
				return 1;
			if(intersects(playerX + playerSpeedX, playerY, myshipWidth, myshipHeight, enemyBulletX[k], enemyBulletY[k], bulletWidth * 2, bulletHeight * 2))
				return 1;
			if(intersects(playerX - playerSpeedX, playerY, myshipWidth, myshipHeight, enemyBulletX[k], enemyBulletY[k], bulletWidth * 2, bulletHeight * 2))
				return 1;	
		}
	}
	return 0;
}

void CheckForAIBullet()
{
	GetLastUserBulletShot();
	GetClosestUserBullet();
	//still check for firebullet[last] here because if nothing turns up
	//the value is defaulted to zero, so this makes sure it is a good value and not just default value
	if (fireBullet[lastUserBullet])
	{
		// this is not the first bullet so check if it is okay to fire again
		if (lastUserBullet > 0)
		{
			// makes sure last bullet was not "just shot"
			if (bulletY[closestBullet] < 150) 
			{
				// make sure there are two bullets available to shoot
				int nextUserBullet = lastUserBullet + 1;
				if (nextUserBullet > playerBulletsMax){
					//wraps around to zero if lastUserBullet is the same as maximum bullets so it 
					//will use the first bullet and lastbullet for dual shoot
					nextUserBullet = 0;
				}
				//makes sure that the next bullet is fireable, otherwise you have to wait until you have two bullets again
				if (dualShoot && fireBullet[nextUserBullet])
				{
					// bullet on left side (-5 from center of ship)  
					bulletY[lastUserBullet] = playerY - bulletHeight;
					bulletX[lastUserBullet] = (int) (playerX + (myshipWidth / 2) - (bulletWidth / 2)) - 5;
					fireBullet[lastUserBullet] = 0;
					drawBullet[lastUserBullet] = 1;

					// bullet on right side (+5 from center of ship)
					bulletY[nextUserBullet] = playerY - bulletHeight;
					bulletX[nextUserBullet] = (int) (playerX + (myshipWidth / 2) - (bulletWidth / 2)) + 5;
					fireBullet[nextUserBullet] = 0;
					drawBullet[nextUserBullet] = 1;
					// cause you shot twice
					shotsFired ++;
				}
				else if (!dualShoot)
				{
					bulletY[lastUserBullet] = playerY - bulletHeight;
					bulletX[lastUserBullet] = (int) (playerX + (myshipWidth / 2) - (bulletWidth / 2));
					fireBullet[lastUserBullet] = 0;
					drawBullet[lastUserBullet] = 1;
				}

				shotsFired ++;
				// this is so you can't hold down the "X" button
				lastLoopX = 1;
				// leaves loop so we don't fire twice
			}
		}
		else
		{
			if (dualShoot)
			{
				// bullet on left side (-5 from center of ship)
				bulletY[lastUserBullet] = playerY - bulletHeight;
				bulletX[lastUserBullet] = (int) (playerX + (myshipWidth / 2) - (bulletWidth / 2)) - 5;
				fireBullet[lastUserBullet] = 0;
				drawBullet[lastUserBullet] = 1;

				// bullet on right side (+5 from center of ship)
				bulletY[lastUserBullet + 1] = playerY - bulletHeight;
				bulletX[lastUserBullet + 1] = (int) (playerX + (myshipWidth / 2) - (bulletWidth / 2)) + 5;
				fireBullet[lastUserBullet + 1] = 0;
				drawBullet[lastUserBullet + 1] = 1;
				// cause you shot twice
				shotsFired ++;
			}
			else
			{
				bulletY[lastUserBullet] = playerY - bulletHeight;
				bulletX[lastUserBullet] = (int) (playerX + (myshipWidth / 2) - (bulletWidth / 2));
				fireBullet[lastUserBullet] = 0;
				drawBullet[lastUserBullet] = 1;
			}

			shotsFired ++;
			// this is so you can't hold down the "X" button
		}
	}
}

void CheckForAIMove()
{
	//only move if no collision (for smart move only)
	if (!AICollision()){
		// Move Left
		if (pad.Buttons & PSP_CTRL_LEFT || pad.Lx <= 25)
		{
			// Check For Hit Left Wall
			if (playerX - playerSpeedX <= 0)
			{
				playerX = 0;
			}
			else
			{
				playerX -= playerSpeedX;
			}
		}

		// Move Right
		if (pad.Buttons & PSP_CTRL_RIGHT || pad.Lx >= 225)
		{
			// Check For Hit Right Wall
			if (playerX + playerSpeedX >= (maxX - myshipWidth))
			{
				playerX = maxX - myshipWidth;
			}
			else
			{
				playerX += playerSpeedX;
			}
		}
	}

	//AI
	for (i = 0; i < enemyBulletsMax; i++){
		if (enemyBulletY[i] > (playerY - 50) && drawEnemyBullet[i] && intersects(playerX, playerY, myshipWidth, myshipHeight, enemyBulletX[i], playerY, bulletWidth * 2, bulletHeight)){
			if (enemyBulletX[i] + (bulletWidth / 2) < (playerX + (myshipWidth / 2))){
				playerX += playerSpeedX;
			}else{
				playerX -= playerSpeedX;
			}
		}else{
			if (!enemyBoolDead[i] && enemyX[i] > playerX && !AICollision()){
				playerX += (int)(playerSpeedX / 2);
			}else if(!enemyBoolDead[i] && enemyX[i] < playerX && !AICollision()){
				playerX -= (int)(playerSpeedX / 2);
			}
		}
	}

	// Check For Hit Left Wall
	if (playerX < 0)
	{
		playerX = 0;
	}
	// Check For Hit Right Wall
	if (playerX > (maxX - myshipWidth))
	{
		playerX = maxX - myshipWidth;
	}
}

void CheckForEnemyBullets()
{
	GetLastEnemyBulletShot();
	//its okay to fire
	if (!drawEnemyBullet[lastEnemyBullet])
	{
		// this is not the first bullet shot
		if (lastEnemyBullet > 0)
		{
			// makes sure the last bullet is at least ?? away before shooting again
			if (GetHighestEnemyBullet() > enemyY[whoShotLast] + enemyHeight + 30)
			{
				whoShot = rand() / (int)(((unsigned)RAND_MAX + 1) / enemyCountTotal);

				if (!enemyBoolDead[whoShot])
				{
					if (whoShotLast != whoShot)
					{
						drawEnemyBullet[lastEnemyBullet] = 1;
						shooter[lastEnemyBullet] = whoShot;
						enemyBulletY[lastEnemyBullet] = enemyY[whoShot] + enemyHeight;
						enemyBulletX[lastEnemyBullet] = (int) (enemyX[whoShot] + (enemyWidth / 2) - (bulletWidth / 2));
						whoShotLast = whoShot;
					}
					else
					{
						sameShotTimes ++;
						if (sameShotTimes == 3)
						{
							// okay to shoot again
							whoShotLast = 99999; // garbage number
							sameShotTimes = 0;
						}
					}
				}
			}
		}
		// this is the first bullet to be shot
		else
		{
			whoShot = rand() / (int)(((unsigned)RAND_MAX + 1) / enemyCountTotal);

			if (!enemyBoolDead[whoShot])
			{
				drawEnemyBullet[lastEnemyBullet] = 1;
				enemyBulletY[lastEnemyBullet] = enemyY[whoShot] + enemyWidth + bulletHeight;
				enemyBulletX[lastEnemyBullet] = (int) (enemyX[whoShot] + (enemyWidth / 2) - (bulletWidth / 2));
			}
		}
	}
}

void CheckForEnemeyBulletsLeft()
{
	enemyBulletsLeft = 0;
	for (i = 0; i < enemyBulletsMax; i++)
	{
		if (drawEnemyBullet[i])
			enemyBulletsLeft++;
	}
}

void CheckForHighScore()
{
	ReadHighScores();

	newHigh = 0;
	// this checks to see if it beat any high scores
	for (i = 0; i <10; i ++)
	{
		if (score >= high[i])
		{
			// sorts same scores by accuracy
			if ((score > high[i]) || (score == high[i] && (int)(accuracy * 100) > highAccuracy[i]))
			{
				newHigh = 1;
				newHighLoc = i;
				// if it did, then push all the scores down one to make room for new high score
				// it is done backwards so that you aren't reading what you wrote
				// since you don't need score 9, you save score 8 to it
				// then it opens room for 7 to move into where 8 was without losing 8 (since it is now in 9)
				for (j = 8; j >i-1; j--)
				{
					high[j+1] = high[j];
					highLevel[j+1] = highLevel[j];
					highAccuracy[j+1] = highAccuracy[j];
				}
				high[i] = score;
				highLevel[i] = currentLevel + 1;
				// accuray is * 100 to save decimal so 43.71 is now 4371
				highAccuracy[i] = (int)(accuracy * 100);
				break;
			}
		}
	}
	if (newHigh)
	{
		WriteNewHighScores();
		//EnterNewHigh();
	}
}
void CheckForPowerUp()
{
	//only shows power up when random is 7 and user does not already have this power up and more than 3 enemies are alive
	if (!dualShoot && aliveCount > 3 && !drawPowerUp[0] && rand() / (int)(((unsigned)RAND_MAX + 1) / 1500) == 3)//1500?
	{
		drawPowerUp[0] = 1;
		powerUpX[0] = rand() / (int)(((unsigned)RAND_MAX + 1) / (maxX - 10));
	}
	if (!drawPowerUp[0] && !frozen && aliveCount > 3 && !drawPowerUp[1] && rand() / (int)(((unsigned)RAND_MAX + 1) / 1500) == 7)//1500?
	{
		drawPowerUp[1] = 1;
		powerUpX[1] = rand() / (int)(((unsigned)RAND_MAX + 1) / (maxX - 10));
	}
}

void CheckForUserBullet()
{
	GetLastUserBulletShot();
	GetClosestUserBullet();

	if (pad.Buttons & PSP_CTRL_CROSS || pad.Buttons & PSP_CTRL_RTRIGGER || pad.Buttons & PSP_CTRL_CIRCLE)
	{
		//still check for firebullet[last] here because if nothing turns up
		//the value is defaulted to zero, so this makes sure it is a good value and not just default value
		if (fireBullet[lastUserBullet] && !lastLoopX)
		{
			// this is not the first bullet so check if it is okay to fire again
			if (lastUserBullet > 0)
			{
				// makes sure last bullet was not "just shot"
				if (bulletY[closestBullet] < 150) 
				{
					// make sure there are two bullets available to shoot
					int nextUserBullet = lastUserBullet + 1;
					if (nextUserBullet > playerBulletsMax){
						//wraps around to zero if lastUserBullet is the same as maximum bullets so it 
						//will use the first bullet and lastbullet for dual shoot
						nextUserBullet = 0;
					}
					//makes sure that the next bullet is fireable, otherwise you have to wait until you have two bullets again
					if (dualShoot && fireBullet[nextUserBullet])
					{
						// bullet on left side (-5 from center of ship)  
						bulletY[lastUserBullet] = playerY - bulletHeight;
						bulletX[lastUserBullet] = (int) (playerX + (myshipWidth / 2) - (bulletWidth / 2)) - 5;
						fireBullet[lastUserBullet] = 0;
						drawBullet[lastUserBullet] = 1;

						// bullet on right side (+5 from center of ship)
						bulletY[nextUserBullet] = playerY - bulletHeight;
						bulletX[nextUserBullet] = (int) (playerX + (myshipWidth / 2) - (bulletWidth / 2)) + 5;
						fireBullet[nextUserBullet] = 0;
						drawBullet[nextUserBullet] = 1;
						// cause you shot twice
						shotsFired ++;
					}
					else if (!dualShoot)
					{
						bulletY[lastUserBullet] = playerY - bulletHeight;
						bulletX[lastUserBullet] = (int) (playerX + (myshipWidth / 2) - (bulletWidth / 2));
						fireBullet[lastUserBullet] = 0;
						drawBullet[lastUserBullet] = 1;
					}

					shotsFired ++;
					// this is so you can't hold down the "X" button
					lastLoopX = 1;
					// leaves loop so we don't fire twice
				}
			}
			else
			{
				if (dualShoot)
				{
					// bullet on left side (-5 from center of ship)
					bulletY[lastUserBullet] = playerY - bulletHeight;
					bulletX[lastUserBullet] = (int) (playerX + (myshipWidth / 2) - (bulletWidth / 2)) - 5;
					fireBullet[lastUserBullet] = 0;
					drawBullet[lastUserBullet] = 1;

					// bullet on right side (+5 from center of ship)
					bulletY[lastUserBullet + 1] = playerY - bulletHeight;
					bulletX[lastUserBullet + 1] = (int) (playerX + (myshipWidth / 2) - (bulletWidth / 2)) + 5;
					fireBullet[lastUserBullet + 1] = 0;
					drawBullet[lastUserBullet + 1] = 1;
					// cause you shot twice
					shotsFired ++;
				}
				else
				{
					bulletY[lastUserBullet] = playerY - bulletHeight;
					bulletX[lastUserBullet] = (int) (playerX + (myshipWidth / 2) - (bulletWidth / 2));
					fireBullet[lastUserBullet] = 0;
					drawBullet[lastUserBullet] = 1;
				}

				shotsFired ++;
				// this is so you can't hold down the "X" button
				lastLoopX = 1;
				// leaves loop so we don't fire twice
			}
		}
	}
	else
	{
		lastLoopX = 0;
	}
}

void CheckForUserMove(){
	// Move Left
	if (pad.Buttons & PSP_CTRL_LEFT || pad.Lx <= 25)
	{
		// Check For Hit Left Wall
		if (playerX - playerSpeedX <= 0)
		{
			playerX = 0;
		}
		else
		{
			playerX -= playerSpeedX;
		}
	}

	// Move Right
	if (pad.Buttons & PSP_CTRL_RIGHT || pad.Lx >= 225)
	{
		// Check For Hit Right Wall
		if (playerX + playerSpeedX >= (maxX - myshipWidth))
		{
			playerX = maxX - myshipWidth;
		}
		else
		{
			playerX += playerSpeedX;
		}
	}
}




void CheckForUserPause()
{
	if (pad.Buttons & PSP_CTRL_START)
	{
		PauseGame();
	}
}
void CreateDummyScores()
{
	int highS[10] = {300,275,250,200,150,120,70,45,30,20};
	int highL[10] = {20,18,16,14,12,10,8,6,5,4};
	for (i = 0; i < 10; i++){
		high[i] = highS[i] * 1000;
		highLevel[i] = highL[i];
		highAccuracy[i] = 4000;
	}
	WriteNewHighScores();
}
void DrawEnemies()
{
	// for each enemy up to the total enemies for that particular level,
	// first check if they are dead, if not, then:
	// draw each enemy based on color
	for (k = 0; k < enemyCountTotal; k++)
	{
		//make sure enemy is alive and in boundaries
		if (!enemyBoolDead[k] && enemyX[k] >= 0 && enemyX[k] <= maxX && enemyY[k] >= 0)
		{
			blitAlphaImageToScreen(0,0,enemyWidth,enemyHeight,enemyImg[enemyColor[k]],enemyX[k],enemyY[k]);
		}
	}
}


void DrawEnemyBullets()
{
	// Draws Enemy Bullets
	for (i = 0; i < enemyBulletsMax; i ++)
	{
		if (drawEnemyBullet[i])
		{
			blitAlphaImageToScreen(0,0,bulletWidth,bulletHeight,bullet[5],enemyBulletX[i],enemyBulletY[i]);
		}
	}
}

void DrawEngine()
{
	clearScreen(ColorBlack);
	//Draws Background Image before anything else
	DrawSpace();
	if (drawPowerUp[0] || drawPowerUp[1]){
		DrawPowerUp();
	}
	DrawUserShip();
	DrawUserBullets();
	DrawEnemyBullets();
	DrawEnemies();
	ShowTitleBar();
	flipScreen();
}

void DrawPowerUp()
{
	//drawPowerUp[1] tells which powerup to draw because if drawPowerUp[1] == 1 then it
	//draws the 2nd one (index 1). but if it is a zero, then the 1st one should be drawn
	int whichPowerUp = drawPowerUp[1];

	if (count == 3)
	{
		powerUpY[whichPowerUp] += powerUpSpeed;
		count = 0;
	}
	else
	{
		count ++;
	}
	//resets power up
	if (powerUpY[whichPowerUp] >= maxY)
	{
		drawPowerUp[whichPowerUp] = 0;
		powerUpX[whichPowerUp] = 0;
		powerUpY[whichPowerUp] = 0;
	}
	// powerup is 10 x 10
	blitAlphaImageToScreen(0,0,10,10,powerup[whichPowerUp],powerUpX[whichPowerUp],powerUpY[whichPowerUp]);

	// checks to see if player shoots the powerup icon
	for (z = 0; z < playerBulletsMax; z++)
	{
		//make sure powerup is visible and not behind titlebar
		//check to see if you shoot or hit it with your ship
		if ((powerUpY[whichPowerUp] >= 10) && (intersects(bulletX[z],bulletY[z],bulletWidth,bulletHeight,powerUpX[whichPowerUp],powerUpY[whichPowerUp],10,10) || intersects(playerX, playerY, myshipWidth, myshipHeight, powerUpX[whichPowerUp],powerUpY[whichPowerUp],10,10)))
		{
			if (whichPowerUp == 0){
				dualShoot = 1;
				// this is so they can shoot the same amount of double bullets
				// as they could with single bullets
				playerBulletsMax *= 2;
				//resets all bullets
				for (k = z; k < playerBulletsMax; k++)
				{
					drawBullet[k] = 0;
					fireBullet[k] = 1;
					bulletY[k] = playerY - bulletHeight;
				}
			}else{
				for (k = 0; k < enemyCountTotal; k++){
					saveSpeed[k] = enemySpeed[k];
					if (enemySpeed[k] > 0){
						enemySpeed[k] = 1;
					}else{
						enemySpeed[k] = -1;
					}
				}
				for (k = z; k < playerBulletsMax; k++){
					drawBullet[k] = 0;
					fireBullet[k] = 1;
					bulletY[k] = playerY - bulletHeight;
				}
				saveBulletSpeed = enemyBulletSpeed;
				enemyBulletSpeed /= 2;
				frozen = 1;
				sceKernelLibcTime(&TimeStart_);
			}

			drawPowerUp[whichPowerUp] = 0;
			powerUpX[whichPowerUp] = -10;
			powerUpY[whichPowerUp] = -10;
		}
	}
}

void DrawSpace()
{
	blitImageToScreen(0,0,maxX,maxY,space[spacePic],0,0);
}
void DrawUserBullets()
{
	// Draws User bullets if they have been fired
	for (i = 0; i < playerBulletsMax; i++)
	{
		if (drawBullet[i])
		{
			blitAlphaImageToScreen(0,0,bulletWidth,bulletHeight,bullet[bulletColor],bulletX[i],bulletY[i]);
		}
	}
}



void DrawUserShip()
{
	blitAlphaImageToScreen(0,0,myshipWidth,myshipHeight,myship[userColor],playerX,playerY);
}

void EndGameLoop()
{
	do
	{
		sceCtrlReadBufferPositive(&pad, 1);

	} while (!(pad.Buttons & PSP_CTRL_START));

	if (newHigh){
		ShowHighScoreScreen(newHighLoc);
	}else{
		ShowHighScoreScreen(-1);
	}

	// restarts the game if START is pressed
	ResetGame();
	SwitchLevel();
	ShowTitleScreen();
}

void GetClosestUserBullet(){
	int bulY = maxY + 25;
	closestBullet = 0;
	for (z = 0; z < playerBulletsMax; z++){
		//if the bullet exists, then you will compare this to new bullet
		if (drawBullet[closestBullet]){
			bulY = bulletY[closestBullet];
		}
		if (drawBullet[z] && bulletY[z] < bulY){
			closestBullet = z;
		}
	}
}

int  GetDigits(int number)
{
	return ((int)log10(number) + 1);
}

int  GetHighestEnemyBullet(){
	int top = maxY;
	for (k = 0; k < enemyBulletsMax; k++){
		if (drawEnemyBullet[k] && enemyBulletY[k] < top){
			top = enemyBulletY[k];
		}
	}
	return top;
}

void GetLastEnemyBulletShot(){
	lastEnemyBullet = 0;
	int first = 1;
	for (z = 0; z < enemyBulletsMax; z++){
		if (!drawEnemyBullet[z]){
			//the last bullet will be overwritten with each larger z and the last one is the largest
			lastEnemyBullet = z;
			first = 1;
			for (j = z; j < enemyBulletsMax; j++){
				if (drawEnemyBullet[z]){
					first = 0;
					break;
				}
			}
			if (first){
				//this will get the first fireable bullet that the rest of the bullets after it will be fireable also
				break;
			}
		}
	}
}

void GetLastUserBulletShot(){
	lastUserBullet = 0;
	int first = 1;
	for (z = 0; z < playerBulletsMax; z++){
		if (fireBullet[z]){
			//the last bullet will be overwritten with each larger z and the last one is the largest
			lastUserBullet = z;
			first = 1;
			for (j = z; j < playerBulletsMax; j++){
				if (!fireBullet[z]){
					first = 0;
					break;
				}
			}
			if (first){
				//this will get the first fireable bullet that the rest of the bullets after it will be fireable also
				break;
			}
		}
	}
}

void GetOptions(int readFile)
{
	if (readFile)
	{
		pFile = fopen("data2 v13.dat","r");
		if (pFile != NULL)
		{
			int temp = 0;
			fread(&temp,sizeof(spacePic),1,pFile);
			spacePic = temp;
			fread(&temp,sizeof(bulletColor),1,pFile);
			bulletColor = temp;
			fread(&temp,sizeof(userColor),1,pFile);
			userColor = temp;
		}
		fclose(pFile);
	}
	temps[0] = bulletColor;
	temps[1] = userColor;
}


void GetPersonalBest(){
	personalBest = 0;
	//counts down top 10 high scores
	for (z = 0; z < 10; z++){
		if (high[z]%1000 != 0 && highAccuracy[z] != 4000){
			//this is not a preset score
			personalBest = high[z];
			//break to only get highest score
			break;
		}
	}
}

void GetPlayerBulletsLeft(){
	playerBulletsLeft = 0;
	for (z = 0; z < playerBulletsMax; z++){
		if (drawBullet[z]){
			playerBulletsLeft++;
		}
	}
}

void GetPowerUpTimeLeft(){
	sceKernelLibcTime(&TimeNow_);
	// the !frozen is so you can clear the powerup whenever you want
	if ((int)(TimeNow_) - (int)(TimeStart_) >= 5 || !frozen){
		for (k = 0; k < enemyCountTotal; k++){
			//determines if they are both negative or both positive
			if ((enemySpeed[k] > 0 && saveSpeed[k] > 0) || (enemySpeed[k] < 0 && saveSpeed[k] < 0)){
				enemySpeed[k] = saveSpeed[k];
			}else{
				enemySpeed[k] = saveSpeed[k] * -1;
			}
		}
		enemyBulletSpeed = saveBulletSpeed;
		frozen = 0;
	}
}

void IncreaseScore(int enemyIndex)
{
	int temp = 0;
	int color = enemyColor[enemyIndex];
	int value1 = 0;

	if (color == purple){
		value1 = 235;
	}else if (color == blue){
		value1 = 245;
	}else if (color == teal){
		value1 = 255;
	}else if (color == red){
		value1 = 265;
	}else if (color == black){
		value1 = 315;
	}
	temp = (int)((currentLevel + 9) * (currentLevel + 7) + value1 + (1.5 * accuracy));
	score += temp;  
}

int  intersects(int x1, int y1, int width1, int height1, int x2, int y2, int width2, int height2)
{
	// (rect1.Right > rect2.Left) && (rect1.Left < rect2.Right) && (rect1.Bottom > rect2.Top) && (rect1.Top < rect2.Bottom)
	return ( (x1 + width1 >= x2) && (x1 <= x2 + width2) && (y1 + height1 >= y2) && (y1 <= y2 + height2) );
}



void LevelBegin()
{
	for (i = 60; i >=0; i--)
	{
		sceCtrlReadBufferPositive(&pad, 1);
		CheckForUserMove();

		clearScreen(ColorBlack);
		DrawSpace();
		ShowTitleBar();

		//if (nextStage)
		//{
		//	sprintf(buffer, "Bonus Life!");
		//	printTextScreen ((maxX / 2) - 40, (maxY / 2) - 25, buffer, ColorWhite);
		//}

		if (i > 40)
		{
			//display "3"
			blitAlphaImageToScreen(3 * snumWidth,0,snumWidth,snumHeight,smallnums,((maxX - snumWidth) / 2),((maxY - snumHeight) / 2));
		}
		else if (i > 20)
		{
			//display "2"
			blitAlphaImageToScreen(2 * snumWidth,0,snumWidth,snumHeight,smallnums,((maxX - snumWidth) / 2),((maxY - snumHeight) / 2));
		}
		else if (i > 0)
		{
			//display "1"
			blitAlphaImageToScreen(1 * snumWidth,0,snumWidth,snumHeight,smallnums,((maxX - snumWidth) / 2),((maxY - snumHeight) / 2));
		}

		DrawEnemies();
		DrawUserShip();
		flipScreen();
	}
	justStarted = 0;
	nextStage = 0;
}
void MoveEnemies()
{
	// number of enemies still alive in the level, starts at zero
	// then incremented each time an alive one is found
	// if this stays at zero, we know to go to the next level
	aliveCount = 0;

	for (i = 0; i < enemyCountTotal; i++)
	{
		if (!enemyBoolDead[i])
		{
			aliveCount ++;

			if (enemyX[i] + enemySpeed[i] <= 0)
			{
				enemyX[i] = 0;
				// *-1 will switch directions!!!
				enemySpeed[i] *= -1;
			}
			else if (enemyX[i] + enemyWidth >= maxX - enemySpeed[i])
			{
				enemyX[i] = maxX - enemyWidth;
				// *-1 will switch directions!!!
				enemySpeed[i] *= -1;
			}
			else
			{
				for (j = 0; j < enemyCountTotal; j++)
				{
					// j != i makes sure it is not checking to see if it is intersecting with itself
					// the following two code segments make sure the enemies are going different directions
					// i is on left going right, j is on right going left (enemyX[i] < enemyX[j] && enemySpeed[j] < 0 && enemySpeed[i] > 0)
					// i is on right going left, j is on left going right (enemyX[i] > enemyX[j] && enemySpeed[j] >0 && enemySpeed[i] < 0)
					if (((enemyX[i] < enemyX[j] && enemySpeed[j] < 0 && enemySpeed[i] > 0) || (enemyX[i] > enemyX[j] && enemySpeed[j] > 0 && enemySpeed[i] < 0)) && j != i && !enemyBoolDead[j] && intersects(enemyX[i],enemyY[i],enemyWidth,enemyHeight,enemyX[j],enemyY[j],enemyWidth,enemyHeight))
					{
						// make them both bounce off each other
						enemySpeed[i] *= -1;
						enemySpeed[j] *= -1;
					}
					else
					{
						//if the rand is 3 randomly switch directions
						if (rand() / (int)(((unsigned)RAND_MAX + 1) / 1000) == 3)
						{
							if (j != i && !enemyBoolDead[j] && !intersects(enemyX[i] + (enemySpeed[i] * - 1),enemyY[i],enemyWidth,enemyHeight,enemyX[j],enemyY[j],enemyWidth,enemyHeight))
							{
								enemySpeed[i] *= -1;
							}
						}
					}
				}
			}//end else
			enemyX[i] += enemySpeed[i];
		}//end if booldead
	}//end for
}

void MoveEnemiesBonusLevel(){
	aliveCount = 0;

	for (i = 0; i < enemyCountTotal; i++)
	{
		//enemy dies if goes off screen the Y >= 100 is to make sure the have completed their rounds
		if (enemyY[i] < -10 || (phaseTwo[i] && (enemyX[i] < (0 - enemyWidth) || enemyX[i] > maxX))){
			enemyBoolDead[i] = 1;
		}
		if (!enemyBoolDead[i])
		{
			aliveCount ++;
			
			//if ((enemySpeed[i] > 0 && enemyX[i] >= (maxX / 2) - 50) || (enemySpeed[i] < 0 && enemyX[i] <= (maxX / 2) + 50) ){
			if (!phaseTwo[i] && intersects(enemyX[i],enemyY[i],enemyWidth,enemyHeight,(maxX / 2) - 5, enemyY[i], 10, enemyHeight)){
				enemyY[i] += generalEnemySpeed;
				if (enemyY[i] >= 100){
					enemySpeed[i] *= -1;
					enemyX[i] += enemySpeed[i];
				}
			}else if (phaseTwo[i] == 0 && ((enemySpeed[i] < 0 && enemyX[i] <= 10) || (enemySpeed[i] > 0 && enemyX[i] >= (maxX - enemyWidth - 10)))){
				enemyY[i] -= generalEnemySpeed;
				if (enemyColor[i] == blue && enemyY[i] <= 20){
					enemySpeed[i] *= -1;
					enemyX[i] += enemySpeed[i];
					phaseTwo[i] = 1;
				}
				if (enemyColor[i] == red && enemyY[i] <= 20 + enemyHeight + 10){
					enemySpeed[i] *= -1;
					enemyX[i] += enemySpeed[i];
					phaseTwo[i] = 1;
				}
			}else{
				enemyX[i] += enemySpeed[i];
			}
		}
	}
}




void PauseGame()
{
	clearScreen(ColorBlack);
	DrawEngine();
	blitAlphaImageToScreen(0,0,maxX,maxY,paused,0,0);
	flipScreen();

	// makes sure it doesn't detect keypress twice in a row
	Wait(25);

	//PAUSE GAME
	do
	{
		sceCtrlReadBufferPositive(&pad, 1);

		if (pad.Buttons & PSP_CTRL_SELECT){
			ShowGameOver();
			break;
		}

	} while (!(pad.Buttons & PSP_CTRL_START));

	// makes sure it doesn't detect keypress twice in a row
	if (pad.Buttons & PSP_CTRL_START){
		Wait(25);
	}
}

void ReadHighScores(){
	// reads all of the high scores
    pFile = fopen("data v13.dat","r");
	if (pFile != NULL)
	{
		for (i = 0; i < 30; i++)
		{
			// read high scores
			if (i < 10){
				fread(&h1,sizeof(h1),1,pFile);
				high[i] = h1;
				high[i] ^= KEYS[i];
			}
			//read level
			else if (i < 20)
			{
				fread(&h1,sizeof(h1),1,pFile);
				highLevel[i - 10] = h1;
				highLevel[i - 10] ^= KEYS[i - 10];
			}
			// read accuracy
			else if (i < 30)
			{
				fread(&h1,sizeof(h1),1,pFile);
				highAccuracy[i - 20] = h1;
				highAccuracy[i - 20] ^= KEYS[i - 20];
			}
		}

		for (i = 0; i < 10; i++){
			if (highAccuracy[i] < 100){
				highAccuracy[i] *= 100;
			}
		}
	}
    fclose(pFile);
	hightop = high[0]/1000;
	highlow = high[0]%1000;
}

void ResetAllBullets()
{
	// resets user and enemy bullets to "not been shot yet"
	for (i = 0; i < playerBulletsMax; i++)
	{
		drawBullet[i] = 0;
		bulletY[i] = playerY - bulletHeight;
		bulletX[i] = 0;
		fireBullet[i] = 1;
	}
	for (i = 0; i < enemyBulletsMax; i++)
	{
		drawEnemyBullet[i] = 0;
		enemyBulletY[i] = enemyY[0];
	}

	for (i = 0; i < enemyBulletsMax; i++)
	{
		drawPowerUp[i] = 0;
		powerUpX[i] = -10;
		powerUpY[i] = -10;
	}
}

void ShowBonusStats(){
	int perfectBonus = 10000;
	int bonus = (int)(bonusHit * accuracy * 5);
	//2nd bonus level gets more
	if (currentLevel > 9){
		perfectBonus *= 2.5;
		bonus *= 2.5;
		if (currentLevel > 14){
			//3rd bonus level
			perfectBonus *= 2;
			bonus *= 2;
		}
	}
	int xc = 230;
	int yc = 97;
	int padding = 33; 
	int newTemp = 0;

	TurnIntoNumberArray(bonusHit);
	for (i = 0; i < 10; i++)
	{
		scoreNums[i] = theNums[i];
	}
	TurnIntoNumberArray(bonus);
	for (i = 0; i < 10; i++)
	{
		levelNums[i] = theNums[i];
	}

	clearScreen(ColorBlack);
	DrawSpace();
	blitAlphaImageToScreen(0,0,maxX,maxY,bstats,0,0);

	for (k = 0; k < GetDigits(bonusHit); k++)
	{
		newTemp = (scoreNums[k] * numWidth);
		blitAlphaImageToScreen(newTemp,0,numWidth,numHeight,overnums,xc + (k * numWidth),yc);
	}
	//display "of"
	blitAlphaImageToScreen(0,0,50,25,of,xc + (2 * numWidth),yc);
	//display "3"
	blitAlphaImageToScreen(3 * numWidth,0,numWidth,numHeight,overnums,xc + (2 * numWidth) + 50,yc);
	//display "0"
	blitAlphaImageToScreen(0,0,numWidth,numHeight,overnums,xc + (3 * numWidth) + 50,yc);

	for (k = 0; k < GetDigits(bonus); k++)
	{
		newTemp = (levelNums[k] * numWidth);
		blitAlphaImageToScreen(newTemp,0,numWidth,numHeight,overnums,xc + (k * numWidth),yc + padding);
	}

	if (currentLevel > 10){
		//display the "+" sign
		blitAlphaImageToScreen(0,0,26,25,plus,xc,yc + (2 * padding));
		//display the user bonus ship
		blitAlphaImageToScreen(0,0,myshipWidth,myshipHeight,myship[userColor],xc + 5 + (numWidth),yc + (2 * padding) + 3);
	}
	
	if (currentLevel > 10){
		//second bonus level so give life automatically
		lives ++;
	}
	if (bonusHit == 30){ 
		// bonus life!!!
		lives ++;
		score += perfectBonus;
	
		//if this is the second bonus level, it shows a second ship to the right
		int xc2 = xc + 5 + (numWidth);
		if (currentLevel > 10){
			xc2 += (5 + myshipWidth);
		}
		//display the "+" sign
		blitAlphaImageToScreen(0,0,26,25,plus,xc,yc + (2 * padding));
		//display the user bonus ship
		blitAlphaImageToScreen(0,0,myshipWidth,myshipHeight,myship[userColor],xc2,yc + (2 * padding) + 3);

		//shows the perfect bonus number 
		TurnIntoNumberArray(perfectBonus);
		for (i = 0; i < 10; i++)
		{
			accNums[i] = theNums[i];
		}

		//display the "+" sign
		blitAlphaImageToScreen(0,0,26,25,plus,xc,yc + (3 * padding));
		for (k = 0; k < GetDigits(perfectBonus); k++)
		{
			newTemp = (accNums[k] * numWidth);
			blitAlphaImageToScreen(newTemp,0,numWidth,numHeight,overnums,xc + 5 + ((k+1) * numWidth),yc + (3 * padding));
		}
	}

	score+= bonus;
	flipScreen();
	do 
	{
		sceCtrlReadBufferPositive(&pad, 1);

	} while (!(pad.Buttons & PSP_CTRL_START)); 
}

void ShowExplosion()
{
	//MP3_Load("./sounds/explosion.mp3");
	//MP3_Play();
	for (i = 0; i < 12; i++)
	{
		clearScreen(ColorBlack);
		DrawSpace();
		ShowTitleBar();
		DrawEnemies();
		blitAlphaImageToScreen((int)(i * explWidth),0,explWidth,explHeight,explosion,playerX,playerY);
		flipScreen();
		Wait(5);
	}
	justStarted = 1;
	playerY = defaultPlayerY;
	ResetAllBullets();
	//MP3_Stop();
}




void ShowGameOver()
{
	// Shows your end game final score, accuracy, and level
	int xc = 230;
	int yc = 97;
	int padding = 33;
	int newTemp = 0;

	TurnIntoNumberArray(currentLevel + 1);
	for (i = 0; i < 10; i++)
	{
		levelNums[i] = theNums[i];
	}
	TurnIntoNumberArray((int)accuracy);
	for (i = 0; i < 10; i++)
	{
		accNums[i] = theNums[i];
	}
	accDecimal = accuracy - (int)accuracy;
	accDecimal *= 100;
	TurnFloatIntoNumberArray((int)accDecimal);
	for (i = 0; i < 2; i++)
	{
		accDecNums[i] = theNums[i];
	}

	clearScreen(ColorBlack);
	DrawSpace();
	blitAlphaImageToScreen(0,0,maxX,maxY,gover,0,0);

	if (score > 1000){
		TurnIntoNumberArray(score/1000);
		for (i = 0; i < 10; i++)
		{
			scoreNums[i] = theNums[i];
		}
		for (k = 0; k < GetDigits(score/1000); k++)
		{
			newTemp = (scoreNums[k] * numWidth); 
			blitAlphaImageToScreen(newTemp,0,numWidth,numHeight,overnums,xc + (k * numWidth),yc);
			scoreNums[k] = 0;
		}
		blitAlphaImageToScreen(0,0,13,23,comma,xc + 4 + (GetDigits(score/1000) * numWidth),yc + 6);
		TurnCommaIntoNumberArray(score%1000);
		for (i = 0; i < 10; i++)
		{
			scoreNums[i] = theNums[i];
		}
		for (k = 0; k < 3; k++)
		{
			newTemp = (scoreNums[k] * numWidth); 
			blitAlphaImageToScreen(newTemp,0,numWidth,numHeight,overnums,xc + ((k + (GetDigits(score/1000))) * numWidth) + 18,yc);
		}
	}else{
		TurnIntoNumberArray(score%1000);
		for (i = 0; i < 10; i++)
		{
			scoreNums[i] = theNums[i];
		}
		for (k = 0; k < 3; k++)
		{
			if (score > 0){
				newTemp = (scoreNums[k] * numWidth); 
				blitAlphaImageToScreen(newTemp,0,numWidth,numHeight,overnums,xc + (k * numWidth),yc);
			}else{
				blitAlphaImageToScreen(0,0,numWidth,numHeight,overnums,xc,yc); 
			}
		}
	}

	for (k = 0; k < GetDigits(currentLevel + 1); k++) 
	{
		newTemp = (levelNums[k] * numWidth);
		blitAlphaImageToScreen(newTemp,0,numWidth,numHeight,overnums,xc + (k * numWidth),yc + padding);
	}

	int acclength = GetDigits((int)accuracy);
	if (accuracy > 0){
		for (k = 0; k < acclength; k++)
		{		
				newTemp = (accNums[k] * numWidth);
				blitAlphaImageToScreen(newTemp,0,numWidth,numHeight,overnums,xc + (k * numWidth),yc + (2 * padding));
		}
	}else{
		acclength = 1;
		blitAlphaImageToScreen(0,0,numWidth,numHeight,overnums,xc,yc + (2 * padding));
	}

	blitAlphaImageToScreen(0,0,13,23,period,xc + 8 + (acclength * numWidth),yc + (2 * padding) + 3);
	for (k = 0; k < 2; k++)
	{
		newTemp = (accDecNums[k] * numWidth);
		blitAlphaImageToScreen(newTemp,0,numWidth,numHeight,overnums,xc + 13 + 10 + (GetDigits((int)accuracy) * numWidth)+ (k * numWidth),yc + (2 * padding));
	}

	blitAlphaImageToScreen(0,0,26,25,percent,xc + 13 + 10 +(acclength * numWidth)+ (2 * numWidth),yc + (2 * padding));

	flipScreen();
	Wait(25);

	CheckForHighScore();
	EndGameLoop();
}


void ShowHighScoreScreen(int coloredRow)
{
	clearScreen(ColorBlack);
	blitAlphaImageToScreen(0,0,maxX,maxY,highBack,0,0);
	ReadHighScores();
	
	for (i = 0; i < 10; i++)
	{
		// shows coloredRow red
		if (i == coloredRow)// && count >= (i * 15))
		{
			if (high[i] >= 100000){
				if ((highAccuracy[i] / 100) < 100)
					sprintf(buffer, "High Score %02i: %-i,%03i  Level: %-2i    Accuracy: %2i.%02i%%", i + 1, high[i]/1000, high[i]%1000, highLevel[i], (highAccuracy[i] / 100), (highAccuracy[i] - ((int)(highAccuracy[i] / 100) * 100)));
				else if ((highAccuracy[i] / 100) == 100)
					sprintf(buffer, "High Score %02i: %-i,%03i  Level: %-2i    Accuracy: %i%%", i + 1, high[i]/1000, high[i]%1000, highLevel[i], (highAccuracy[i] / 100));
			}else{
				//extra space since it is shorter of a number
				if ((highAccuracy[i] / 100) < 100)
					sprintf(buffer, "High Score %02i: %-i,%03i   Level: %-2i    Accuracy: %2i.%02i%%", i + 1, high[i]/1000, high[i]%1000, highLevel[i], (highAccuracy[i] / 100), (highAccuracy[i] - ((int)(highAccuracy[i] / 100) * 100)));
				else if ((highAccuracy[i] / 100) == 100)
					sprintf(buffer, "High Score %02i: %-i,%03i   Level: %-2i    Accuracy: %i%%", i + 1, high[i]/1000, high[i]%1000, highLevel[i], (highAccuracy[i] / 100));
			}
			printTextScreen (30, 15 * i + 100, buffer, ColorBlue);
			blitAlphaImageToScreen(0,0,maxX,maxY,newhigh,0,0);
			//sprintf(buffer,"*");
			//printTextScreen(5, 15 * i + 100, buffer, colors[0]);
		}
		else if (high[i] && highLevel[i])// && count >= (i * 15))
		{
			if (high[i] >= 100000){
				if ((highAccuracy[i] / 100) < 100)
					sprintf(buffer, "High Score %02i: %-i,%03i  Level: %-2i    Accuracy: %2i.%02i%%", i + 1, high[i]/1000, high[i]%1000, highLevel[i], (highAccuracy[i] / 100), (highAccuracy[i] - ((int)(highAccuracy[i] / 100) * 100)));
				else if ((highAccuracy[i] / 100) == 100)
					sprintf(buffer, "High Score %02i: %-i,%03i  Level: %-2i    Accuracy: %i%%", i + 1, high[i]/1000, high[i]%1000, highLevel[i], (highAccuracy[i] / 100));
			}else{
				//extra space since it is shorter of a number
				if ((highAccuracy[i] / 100) < 100)
					sprintf(buffer, "High Score %02i: %-i,%03i   Level: %-2i    Accuracy: %2i.%02i%%", i + 1, high[i]/1000, high[i]%1000, highLevel[i], (highAccuracy[i] / 100), (highAccuracy[i] - ((int)(highAccuracy[i] / 100) * 100)));
				else if ((highAccuracy[i] / 100) == 100)
					sprintf(buffer, "High Score %02i: %-i,%03i   Level: %-2i    Accuracy: %i%%", i + 1, high[i]/1000, high[i]%1000, highLevel[i], (highAccuracy[i] / 100));
			}
			printTextScreen (30, 15 * i + 100, buffer, ColorBlack);
		}
		
	}
	flipScreen();

	Wait(15);
	do{
		sceCtrlReadBufferPositive(&pad, 1);
	} while (!(pad.Buttons & PSP_CTRL_START));
	//so you don't wait for main menu
	firsttime = 1;
}


void ShowLevelScreen()
{
	levelX = 0;
	int textX = ((maxX - snumWidth) / 2) - 40;
	int textY = ((maxY - snumHeight) / 2);

	TurnIntoNumberArray(currentLevel + 1);
	for (i = 0; i < 10; i++)
	{
		levelNums[i] = theNums[i];
	}
	
	while (levelX < maxX)
	{
		clearScreen(ColorBlack);
		DrawSpace();
		if (!bonusLevel){
			//will only show image "Level " if it has been passed
			if (levelX >= textX){
				blitAlphaImageToScreen(0,0,71,26,ltext,textX,textY-4);
			}
			for (k = 0; k < GetDigits(currentLevel + 1); k++)
			{
				blitAlphaImageToScreen((levelNums[k] * snumWidth),0,snumWidth,snumHeight,smallnums,textX + 80 + (k * snumWidth),textY);
			}			
		}else{
			if (levelX >= textX){
				blitAlphaImageToScreen(0,0,155,26,btext,textX - 40,textY-4);
			}			
		}
		//2nd space pic covers up the level text until it has passed over it
		blitImageToScreen(levelX,0,(maxX - levelX),maxY,space[spacePic],levelX,0);
		blitAlphaImageToScreen(0,0,44,37,levelPic,levelX,115);
		DrawEnemies();
		DrawUserShip();
		ShowTitleBar();
		flipScreen();
		levelX += 4;//8
		sceCtrlReadBufferPositive(&pad, 1);
		CheckForUserMove();
	}
	ResetAllBullets();
	//in case you died on the last bullet from last level
	justStarted = 0;
}

void ShowLoadingScreen(int num){
	int maxLength = 250;
	int maxImages = 60;
	clearScreen(ColorBlack);
	if ((int)((num * maxLength)/ maxImages) <= maxLength){
		blitImageToScreen(0,0,(int)((num * maxLength)/ maxImages),6,meter,(maxX / 2) - (maxLength / 2),(maxY / 2) - (8/2) + 1);
	}else{
		blitImageToScreen(0,0,maxLength,6,meter,(maxX / 2) - (maxLength / 2),(maxY / 2) - (8/2) + 1);
	}
	blitAlphaImageToScreen(0,0,maxX,maxY,psphacks,0,0);
	blitAlphaImageToScreen(0,0,maxLength,8,border,(maxX / 2) - (maxLength / 2),(maxY / 2) - (8/2));
	flipScreen();
}

void ShowOptionsScreen()
{
	GetOptions(0);
	int index = 0;
	int bX = (maxX / 2) - (bulletWidth / 2);
	int bY = playerY - 100;
	int flip = 1;
	do
	{
		sceCtrlReadBufferPositive(&pad, 1);
		clearScreen(ColorBlack);
		DrawSpace();
		blitAlphaImageToScreen(0,0,maxX,maxY,options,0,0);
		playerX = (maxX / 2) - (myshipWidth / 2);
		DrawUserShip();

		if (pad.Buttons & PSP_CTRL_DOWN)
		{
			index = 1;
			Wait(15);
		}
		if (pad.Buttons & PSP_CTRL_UP)
		{
			index = 0;
			Wait(15);
		}
		if (pad.Buttons & PSP_CTRL_RTRIGGER)
		{
			spacePic ++;
			if (spacePic >= numOfSpace)
			{
				spacePic = 0;
			}
			Wait(15);
		}
		if (pad.Buttons & PSP_CTRL_LTRIGGER)
		{
			spacePic --;
			if (spacePic < 0)
			{
				spacePic = (numOfSpace - 1);
			}
			Wait(15);
		}

		// do this earlier (here) to allow for rewriting over the green arrow with a red one
		if (index == 1)
		{
			blitAlphaImageToScreen(0,0,7,11,gl,playerX - 10 - 7, playerY + 4);
			blitAlphaImageToScreen(0,0,7,11,gr,playerX + myshipWidth + 10, playerY + 4);
			//blitAlphaImageToScreen(0,0,7,11,wl,bX - 10 - 7, bY + 1);
			//blitAlphaImageToScreen(0,0,7,11,wr,bX + bulletWidth + 10, bY + 1);
			blitAlphaImageToScreen(0,0,7,11,wl,playerX - 10 - 7, bY + 1);
			blitAlphaImageToScreen(0,0,7,11,wr,playerX + myshipWidth + 10, bY + 1);
			userColor = temps[1];


		}
		else
		{
			blitAlphaImageToScreen(0,0,7,11,wl,playerX - 10 - 7, playerY + 4);
			blitAlphaImageToScreen(0,0,7,11,wr,playerX + myshipWidth + 10, playerY + 4);
			//blitAlphaImageToScreen(0,0,7,11,gl,bX - 10 - 7, bY + 1);
			//blitAlphaImageToScreen(0,0,7,11,gr,bX + bulletWidth + 10, bY + 1);
			blitAlphaImageToScreen(0,0,7,11,gl,playerX - 10 - 7, bY + 1);
			blitAlphaImageToScreen(0,0,7,11,gr,playerX + myshipWidth + 10, bY + 1);
			bulletColor = temps[0];
		}

		blitAlphaImageToScreen(0,0,bulletWidth,bulletHeight,bullet[bulletColor],bX,bY);

		if (pad.Buttons & PSP_CTRL_RIGHT)
		{
			temps[index] ++;

			//my ship has 7 colors, bullet only has 5
			if (index == 1){
				if (temps[index] >= myshipcount)
				{
					temps[index] = 0;
				}
			}else{
				if (temps[index] >= 5)
				{
					temps[index] = 0;
				}
			}

			if (index == 1)
				blitAlphaImageToScreen(0,0,7,11,rr,playerX + myshipWidth + 10, playerY + 4);
			else
				//blitAlphaImageToScreen(0,0,7,11,rr,bX + bulletWidth + 10, bY + 1);
				blitAlphaImageToScreen(0,0,7,11,rr,playerX + myshipWidth + 10, bY + 1);
			flipScreen();
			flip = 0;

			Wait(15);
		}
		if (pad.Buttons & PSP_CTRL_LEFT)
		{
			temps[index] --;

			//my ship has 7 colors, bullet only has 5
			if (index == 1){
				if (temps[index] < 0)
				{
					temps[index] = myshipcount - 1;
				}
			}else{
				if (temps[index] < 0)
				{
					temps[index] = 4;
				}
			}

			if (index == 1)
				blitAlphaImageToScreen(0,0,7,11,rl,playerX - 10 - 7, playerY + 4);
			else
				//blitAlphaImageToScreen(0,0,7,11,rl,bX - 10 - 7, bY + 1);
				blitAlphaImageToScreen(0,0,7,11,rl,playerX - 10 - 7, bY + 1);
			flipScreen();
			flip = 0;

			Wait(15);
		}

		if (flip)
			flipScreen();
		else
			flip = 1;
	} while (!(pad.Buttons & PSP_CTRL_START));

	WriteOptionsToFile();

	Wait(25);
}


void ShowTitleBar()
{
	//-10 from everything for new format
	blitImageToScreen(0,0,maxX,10,titlebar,0,0);
	if (score >= 1000){
		// Shows current score in titlebar
		sprintf(buffer, "Score: %-i,%03i", score/1000,score%1000);
	}else{
		sprintf(buffer, "Score: %i", score);
	}
	printTextScreen (115 - 10, 1, buffer, ColorBlack);  
	// Shows accuracy in titlebar
	sprintf(buffer, "Accuracy: %0.2f%%", accuracy);
	printTextScreen (235 - 10, 1, buffer, ColorBlack);
	//Shows Current Level in titlebar
	//sprintf(buffer, "Level: %i", currentLevel + 1);
	if (score < (hightop * 1000) + (highlow)){
		sprintf(buffer, "High: %-i,%03i",hightop,highlow);
		printTextScreen (380 - 10, 1, buffer, ColorBlack);
	}else{
		sprintf(buffer, "High: %-i,%03i",score/1000,score%1000);
		printTextScreen (380 - 10, 1, buffer, ColorBlack);
	}

	// Shows current lives in titlebar
	sprintf(buffer, "Lives: ");
	printTextScreen (1, 1, buffer, ColorBlack);
	if (lives - 1 <= 5){
		for (f = 0; f < lives - 1; f++)
		{
			if ( f <= 3)
			{
				blitAlphaImageToScreen(0,0,9,9,smallship, 50 +(f * (9 + 3)),0);
			}
		}
	}else{
		// Shows current lives in titlebar
		sprintf(buffer, "Lives:%2i",lives - 1);
		printTextScreen (1, 1, buffer, ColorBlack);
		blitAlphaImageToScreen(0,0,9,9,smallship,64,0);
	}

}


void ShowTitleScreen()
{
	if (!firsttime)
	{
		Wait(20);
	}
	firsttime = 0;
	int loops = 0;
	int showX = 0;
	int newTemp = 0;
	ReadHighScores();
	GetPersonalBest();

	// flashes the x button on the title screen
	do
	{
		sceCtrlReadBufferPositive(&pad, 1);
		clearScreen(ColorBlack);
		blitImageToScreen(0,0,maxX,maxY,backship,0,0);
		blitAlphaImageToScreen(0,0,maxX,maxY,back,0,0);

		if (personalBest != 0){
			TurnIntoNumberArray(personalBest/1000);
			for (k = 0; k < GetDigits(personalBest/1000); k++)
			{
				newTemp = (theNums[k] * numWidth);
				blitAlphaImageToScreen(newTemp,0,numWidth,numHeight,overnums,65 + 2 + (k * numWidth),173);
			}
			blitAlphaImageToScreen(0,0,13,23,comma,65 + 3 + (GetDigits(personalBest/1000) * numWidth),178);
			TurnCommaIntoNumberArray(personalBest%1000);
			for (k = 0; k < 3; k++)
			{
				newTemp = (theNums[k] * numWidth);
				blitAlphaImageToScreen(newTemp,0,numWidth,numHeight,overnums,65 + 2 + 15 +((GetDigits(personalBest/1000) + k) * numWidth),173);
			}
		}else{
			blitAlphaImageToScreen(0,0,numWidth,numHeight,overnums,65 + 2 + (5 * numWidth),173);
		}

		if (loops % 30 == 0)
		{
			if (showX)
			{
				showX = 0;
			}
			else
			{
				showX = 1;
			}
		}
		if (showX)
		{
			blitAlphaImageToScreen(0,0,20,21,pspX,408,178);
		}
		loops ++;

		if (pad.Buttons & PSP_CTRL_CIRCLE)
			ShowOptionsScreen();

		if (pad.Buttons & PSP_CTRL_TRIANGLE)
			ShowHighScoreScreen(-1);

		if (pad.Buttons & PSP_CTRL_SELECT)
			sceKernelExitGame();

		flipScreen();


	} while (!(pad.Buttons & PSP_CTRL_CROSS));

	ShowLevelScreen();
	ResetAllBullets();
}

void SwitchLevel()
{
	if (frozen){
		//clears the powerup
		frozen = 0;
		GetPowerUpTimeLeft();
	}
	currentLevel ++;
	switch (currentLevel % 8) //7 + bonus
	{
		case 0:
			if (currentLevel != 0)
			{
				if (bonusLevel){
					bonusLevel = 0;
					Wait(30);
					ShowBonusStats();
					ResetAllBullets();
				}
				// new set of levels
				enemyCountTotal = 8;
				aliveCount = enemyCountTotal;
				// bonus points
				//score += 3000 * (int)(accuracy / 100);
				// enemies shoot faster!!!
				enemyBulletSpeed ++;
				// user gets extra bullets!!!
				//playerBulletsMax ++;
				// Enemies move faster!!!
				generalEnemySpeed ++;
				// nextStage = 1; to display ("bonus life") on level screen
				nextStage = 1;
			}

			for (i = 0; i < enemyCountTotal; i++)
			{
				enemyBeginX = 130;
				enemyX[i] = enemyBeginX + (i * (enemyWidth + enemyPadding));
				enemyBoolDead[i] = 0;
				enemyY[i] = 20;

				if (i == 0 || i == 1 || i == 6 || i == 7)
				{
					enemyColor[i] = purple;
				}
				else
				{
					enemyColor[i] = blue;
				}
			}

			break;
		case 1:
			enemyCountTotal = 14;
			aliveCount = enemyCountTotal;

			for (i = 0; i < enemyCountTotal; i++)
			{
				enemyBoolDead[i] = 0;

				if (i < 8)
				{
					enemyBeginX = 130;
					enemyX[i] = enemyBeginX + (i * (enemyWidth + enemyPadding));
					enemyY[i] = 20;
					if (i == 0 || i == 1 || i == 6 || i == 7)
					{
						enemyColor[i] = purple;
					}
					else
					{
						enemyColor[i] = blue;
					}
				}
				else
				{
					enemyBeginX = 160;
					enemyX[i] = enemyBeginX + ((i % 8)*(enemyWidth + enemyPadding));
					enemyY[i] = 20 + enemyHeight + 10;
					enemyColor[i] = teal;
				}
			}

			break;
		case 2:
			enemyCountTotal = 18;
			aliveCount = enemyCountTotal;

			for (i = 0; i < enemyCountTotal; i++)
			{
				enemyBoolDead[i] = 0;

				if (i < 8)
				{
					enemyBeginX = 130;
					enemyX[i] = enemyBeginX + (i * (enemyWidth + enemyPadding));
					enemyY[i] = 20;
					if (i == 0 || i == 1 || i == 6 || i == 7)
					{
						enemyColor[i] = purple;
					}
					else
					{
						enemyColor[i] = blue;
					}
				}
				else if (i < 14)
				{
					enemyBeginX = 160;
					enemyX[i] = enemyBeginX + ((i % 8)*(enemyWidth + enemyPadding));
					enemyY[i] = 20 + enemyHeight + 10;
					enemyColor[i] = teal;
				}
				else
				{
					enemyBeginX = 190;
					enemyX[i] = enemyBeginX + ((i % 14)*(enemyWidth + enemyPadding));
					enemyY[i] = 20 + enemyHeight + 10 + enemyHeight + 10;
					enemyColor[i] = red;
				}
			}
			break;
		case 3:
			enemyCountTotal = 24;
			aliveCount = enemyCountTotal;

			for (i = 0; i < enemyCountTotal; i++)
			{
				enemyBoolDead[i] = 0;

				if (i < 10)
				{
					enemyBeginX = 100;
					enemyX[i] = enemyBeginX + (i * (enemyWidth + enemyPadding));
					enemyY[i] = 20;
					if (i == 0 || i == 1 || i == 8 || i == 9)
					{
						enemyColor[i] = purple;
					}
					else
					{
						enemyColor[i] = blue;
					}
				}
				else if (i < 18)
				{
					enemyBeginX = 130;
					enemyX[i] = enemyBeginX + ((i % 10)*(enemyWidth + enemyPadding));
					enemyY[i] = 20 + enemyHeight + 10;
					enemyColor[i] = teal;
				}
				else
				{
					enemyBeginX = 160;
					enemyX[i] = enemyBeginX + ((i % 18)*(enemyWidth + enemyPadding));
					enemyY[i] = 20 + enemyHeight + 10 + enemyHeight + 10;
					enemyColor[i] = red;
				}
			}
			break;
		case 4:
			enemyCountTotal = 26;
			aliveCount = enemyCountTotal;

			for (i = 0; i < enemyCountTotal; i++)
			{
				enemyBoolDead[i] = 0;

				if (i < 12)
				{
					enemyBeginX = 70;
					enemyX[i] = enemyBeginX + (i * (enemyWidth + enemyPadding));
					enemyY[i] = 20;
					if (i == 0 || i == 1 || i == 10 || i == 11)
					{
						enemyColor[i] = purple;
					}
					else
					{
						enemyColor[i] = blue;
					}
				}
				else if (i < 20)
				{
					enemyBeginX = 130;
					enemyX[i] = enemyBeginX + ((i % 12)*(enemyWidth + enemyPadding));
					enemyY[i] = 20 + enemyHeight + 10;
					enemyColor[i] = teal;
				}
				else
				{
					enemyBeginX = 160;
					enemyX[i] = enemyBeginX + ((i % 20)*(enemyWidth + enemyPadding));
					enemyY[i] = 20 + enemyHeight + 10 + enemyHeight + 10;
					enemyColor[i] = red;
				}
			}
			break;
		case 5:
			enemyCountTotal = 30;
			aliveCount = enemyCountTotal;

			for (i = 0; i < enemyCountTotal; i++)
			{
				enemyBoolDead[i] = 0;

				if (i < 12)
				{
					enemyBeginX = 70;
					enemyX[i] = enemyBeginX + (i * (enemyWidth + enemyPadding));
					enemyY[i] = 20;
					if (i == 0 || i == 1 || i == 10 || i == 11)
					{
						enemyColor[i] = purple;
					}
					else
					{
						enemyColor[i] = blue;
					}
				}
				else if (i < 22)
				{
					enemyBeginX = 100;
					enemyX[i] = enemyBeginX + ((i % 12)*(enemyWidth + enemyPadding));
					enemyY[i] = 20 + enemyHeight + 10;
					enemyColor[i] = teal;
				}
				else
				{
					enemyBeginX = 130;
					enemyX[i] = enemyBeginX + ((i % 22)*(enemyWidth + enemyPadding));
					enemyY[i] = 20 + enemyHeight + 10 + enemyHeight + 10;
					enemyColor[i] = red;
				}
			}
			break;
		case 6:
			enemyCountTotal = 36;
			aliveCount = enemyCountTotal;

			for (i = 0; i < enemyCountTotal; i++)
			{
				enemyBoolDead[i] = 0;

				if (i < 14)
				{
					enemyBeginX = 40;
					enemyX[i] = enemyBeginX + (i * (enemyWidth + enemyPadding));
					enemyY[i] = 20;
					if (i == 0 || i == 1 || i == 12 || i == 13)
					{
						enemyColor[i] = purple;
					}
					else
					{
						enemyColor[i] = blue;
					} 
				}
				else if (i < 26)
				{
					enemyBeginX = 70;
					enemyX[i] = enemyBeginX + ((i % 14)*(enemyWidth + enemyPadding));
					enemyY[i] = 20 + enemyHeight + 10;
					enemyColor[i] = black;
				}
				else
				{
					enemyBeginX = 100;
					enemyX[i] = enemyBeginX + ((i % 26)*(enemyWidth + enemyPadding));
					enemyY[i] = 20 + enemyHeight + 10 + enemyHeight + 10;
					enemyColor[i] = red;
				}
			}
			break;
		case 7:
			bonusHit = 0;
			bonusLevel = 1;
			enemyCountTotal = 30;
			aliveCount = enemyCountTotal;
			enemyBeginX = 0 - enemyWidth;
			for (i = 0; i < enemyCountTotal; i++)
			{
				phaseTwo[i] = 0;
				enemyBoolDead[i] = 0;		
				if (i < (enemyCountTotal / 2)){
					enemySpeed[i] = generalEnemySpeed;
					enemyX[i] = enemyBeginX - (i * (enemyWidth + enemyPadding));
					enemyY[i] = 20;
					enemyColor[i] = blue;
				}else{
					enemySpeed[i] = generalEnemySpeed * -1;
					enemyX[i] = maxX + ((i - (enemyCountTotal / 2)) * (enemyWidth + enemyPadding));
					enemyY[i] = 20;
					enemyColor[i] = red;
				}
			}
			break;
		}
		if (!bonusLevel){
			for (i = 0; i < enemyCountTotal; i++)
			{
				if (rand() / (int)(((unsigned)RAND_MAX + 1) / 3))
				{
					enemySpeed[i] = generalEnemySpeed;
				}
				else
				{
					enemySpeed[i] = -1 * generalEnemySpeed;
				}
			}
		}
	playerY = defaultPlayerY;
}

void TeleportEnemy(){
	int notOkayToMove = 1;
	do
	{
		enemyX[i] = rand() / (int)(((unsigned)RAND_MAX + 1) / maxX);
		for (k = 0; k < enemyCountTotal; k++)
		{
			if (k != i && !intersects(enemyX[i] - 5,enemyY[i],enemyWidth + 10,enemyHeight,enemyX[k] - 5,enemyY[k],enemyWidth + 10,enemyHeight));
			{
				notOkayToMove = 0;
			}
		}
	} while (notOkayToMove);
}




void TurnCommaIntoNumberArray(int number){
	int divisor = 1;
	int scoreLeft = number;
	int digits = 3;
	for (i = 0; i < digits; i++)
	{
		divisor = 1;
		if (i == 0 && number < 10)
		{
			theNums[0] = 0;
			theNums[1] = 0;
			i += 2;
		}
		else if (i == 0 && number < 100)
		{
			theNums[0] = 0;
			i++;
		}
		else if (i == 1 && scoreLeft < 10)
		{
			theNums[1] = 0;
			i++;
		}

		for (j = 0; j < (int)log10(scoreLeft); j++)
		{
			divisor *= 10;
		}
		theNums[i] = (int)(scoreLeft / divisor);
		scoreLeft -= (divisor * theNums[i]);
	}
}

void TurnFloatIntoNumberArray(int number)
{
	int divisor = 1;
	int scoreLeft = number;
	int digits = 2;
	for (i = 0; i < digits; i++)
	{
		divisor = 1;
        if (number < 10)
        {
			theNums[0] = 0;
			i++;
		}

		for (j = 0; j < (int)log10(scoreLeft); j++)
		{
			divisor *= 10;
		}
		theNums[i] = (int)(scoreLeft / divisor);
		scoreLeft -= (divisor * theNums[i]);
	}
}
void TurnIntoNumberArray(int number)
{
	int divisor = 1;
	int left = number;
	int digits = (int)log10(number) + 1;
	int lastD = digits - 1;
    int newD;

	for (i = 0; i < digits; i++)
	{
		divisor = 1;
		newD = (int)log10(left);
        if (newD + 1 != lastD && i!=0)
        {
			for (k = 0; k < lastD - newD - 1; k++)
			{
				theNums[i] = 0;
				i++;
			}
		}
		lastD = newD;
		for (j = 0; j < newD; j++)
		{
			divisor *= 10;
		}
		theNums[i] = (int)(left / divisor);
		left -= (divisor * theNums[i]);
	}
}

void UpdateEnemyBullets(){
	for (i = 0; i < enemyBulletsMax; i++){
		if (drawEnemyBullet[i]){
			enemyBulletY[i] += enemyBulletSpeed;

			// hit the player's ship
			if (intersects(enemyBulletX[i], enemyBulletY[i], bulletWidth, bulletHeight, playerX, playerY, myshipWidth, myshipHeight))
			{
				UserIsHit();
			}
			// goes down the bottom of the screen
			else if (enemyBulletY[i] >= maxY)
			{
				drawEnemyBullet[i] = 0;
				enemyBulletY[i] = enemyY[0];
			}
		}
	}
}

void UpdateUserBullets()
{
	for (i = 0; i < playerBulletsMax; i++)
	{
		if (drawBullet[i])
		{
			// makes user's bullet go up
			bulletY[i] -= bulletSpeed;
			// Bullet has left the top of the screen
			if (bulletY[i] <= 0)
			{
				drawBullet[i] = 0;
				fireBullet[i] = 1;
				bulletY[i] = playerY - bulletHeight;
				accuracy = ((float)shotsHit / shotsFired) * 100;
			}
			// for each enemy that is alive, it checks to see whether they have been hit with the bullet
			for (j = 0; j < enemyCountTotal; j++)
			{
				if (!enemyBoolDead[j] && intersects(bulletX[i], bulletY[i], bulletWidth, bulletHeight, enemyX[j], enemyY[j], enemyWidth, enemyHeight))
				{
					enemyBoolDead[j] = 1;
					drawBullet[i] = 0;
					fireBullet[i] = 1;
					bulletY[i] = playerY - bulletHeight;
					if (bonusLevel){
						bonusHit++;
					}
					shotsHit ++;
					IncreaseScore(j);
					accuracy = ((float)shotsHit / shotsFired) * 100;
				}
			}
		}
	}
}

void UserIsHit()
{
	// show explode sequence next draw loop
	explode = 1;
	// get rid of the evidence
	drawEnemyBullet[i] = 0;
	enemyBulletY[i] = enemyY[0];

	ShowExplosion();

	lives --;
	if (dualShoot)
	{
		// lose powerups if you had them
		dualShoot = 0;
		// so the max bullets go back to normal (like it was before they got the power up)
		playerBulletsMax /= 2;
	}

	if (frozen){
		sceKernelLibcTime(&TimeNow_);
		//to clear the powerup
		frozen = 0;
		GetPowerUpTimeLeft();
	}

	// Game Over
	if (!lives)
	{
		ShowGameOver();
	}
}

void Wait(int time)
{
	for (z = 0; z < time; z++)
	{
		sceDisplayWaitVblankStart();
	}
}
void WriteNewHighScores()
{
    pFile = fopen("data v13.dat","w");

	if (pFile != NULL)
	{
		// write high scores
		for (i = 0; i < 30; i++)
		{
			if (i < 10)
			{
				high[i] ^= KEYS[i];
				fwrite(&high[i],sizeof(h1),1,pFile);
			}
			// write high levels
			else if (i < 20)
			{
				highLevel[i - 10] ^= KEYS[i - 10];
				fwrite(&highLevel[i - 10],sizeof(h1),1,pFile);
			}
			// write high accuracys
			else if (i < 30)
			{
				highAccuracy[i - 20] ^= KEYS[i - 20];
				fwrite(&highAccuracy[i - 20],sizeof(h1),1,pFile);
			}
		}
	}
    fclose(pFile);
}

void WriteOptionsToFile()
{
	pFile = fopen("data2 v13.dat","w");
	if (pFile != NULL)
	{
		fwrite(&spacePic,sizeof(spacePic),1,pFile);
		fwrite(&bulletColor,sizeof(bulletColor),1,pFile);
		fwrite(&userColor,sizeof(userColor),1,pFile);
	}
	fclose(pFile);
}

///////////TODO BEFORE 1.3//////////////////////////////////////////////
// should titlebar show currentlevel or high score?
// fix when you get the time powerup with dualShoot the bullets mess up
// change chances of getting new powerup
// set clock frequency to 222mhz?
// allow practice mode
//		- unlock this feature?
//		- be able to set enemy difficulty
//		- set lives (an option for infinite)
// use arrow and move up/down for main menu
// should the game end at level 24?
////////////////////////////////////////////////////////////////////////

//**********To Fix/Add ********************************
// + Make Thumbstick support smoother instead of jumpy
// + Difficulty levels?
// + create a boss level (lvl 24?)
//   -> let him teleport? (randomly?)
//	 -> let him have a health bar (like the one used for loading)
//   -> let him have slaves/pawns?
//	 -> heat seaking bullets? 
//*****************************************************
