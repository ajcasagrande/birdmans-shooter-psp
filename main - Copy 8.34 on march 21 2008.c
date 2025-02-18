//***************************************************************
// Birdman's Shooter PSP
//
// Anthony Casagrande
//
// Project Start Date: 10/23/07
// Version 1.0: submitted to psp-hacks.com on 10/28/07
// Version 1.2: submitted to psp-hacks.com on 11/02/07
// Latest Revision: 11/17/07
//
// Change Log From 1.0 to 1.2:
// + Current Level diplayed on HUD and when you lose.
// + 3,2,1 Go! before each level and after each time you die.
// + Ability to move before enemies (for positioning)
// + Fixed enemy positions at beginning of levels
// + Powerup moves from top to bottom
// + Fixed explosion animation
// + Pausing is a little smoother 
// + Enemies no longer get stuck to each other!!!!! (Finally!)
// + New HUD background
// + Lives are displayed as mini-images in HUD
// + New title screen splash image
// + New gameover text and sequence
// + New enemy colors
//
// Change Log From 1.2 to 1.3:
// + Ability to skip the gameover sequence.
// + Options Screen
// + Options and High Scores are saved
// + New Backgrounds, Bullet Colors, Player's Ship Colors
// + Fixed some glitches with the power up
// + Next level doesn't come until all enemy bullets are gone
// + Ability to quit from main menu and during game when paused
// + Best high score is displayed on main menu and titlebar
//***************************************************************

//**********To Fix/Add Before Release******************
// + show high score in titlebar
// + Create a high score table, but show first place high score on homescreen
// + Make Thumbstick support smoother instead of jumpy
// + fix first bullet glitch how it is off to the side
// + allow users to exit from main menu
// + PowerUp continues to fall as you die. || or it goes away completly
// + Reset powerup at level over.
// + enemies do not randomly change directions as often
// + make player dodge last bullets after he kills all enemies
// + fix dualShoot how it shoots one if you just killed an enemy
// + Difficulty levels
// + PowerUp falls less frequently
// + create a bonus level
//	 -> time based
//	 -> infinite enemies?
// + create a boss level
//   -> let him teleport (randomly?)
//	 -> let him have a health bar
//   -> let him have slaves/pawns?
//*****************************************************

#include <pspdisplay.h>
#include <pspctrl.h>
#include <pspkernel.h>
#include <pspdebug.h>
#include <pspgu.h>
#include <png.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pspaudio.h>
#include <pspaudiolib.h>
#include "graphics.h"
//#include "mp3player.h"

#define printf pspDebugScreenPrintf
#define RGB(r, g, b) ((r)|((g)<<8)|((b)<<16))
PSP_MODULE_INFO("BirdShooter",0,1,1);

//begin code to use home button
/* Exit callback */
int exit_callback(int arg1, int arg2, void *common) {
	sceKernelExitGame();
	return 0;
}

/* Callback thread */
int CallbackThread(SceSize args, void *argp) {
	int cbid;

	cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
	sceKernelRegisterExitCallback(cbid);

	sceKernelSleepThreadCB();

	return 0;
}

/* Sets up the callback thread and returns its thread id */
int SetupCallbacks(void) {
	int thid = 0;

	thid = sceKernelCreateThread("update_thread", CallbackThread, 0x11, 0xFA0, 0, 0);
	if(thid >= 0) {
		sceKernelStartThread(thid, 0, 0);
	}

	return thid;
}

//}end code to use home button

// Declares the functions
int intersects();
void LevelBegin();
void CheckForPowerUp();
void MoveEnemies();
void SwitchLevel();
void DrawEngine();
void UserIsHit();
void FireEnemyBullets();
void UpdateUserBullets();
void EndGameLoop();
void DrawPowerUp();
void DrawEnemyBullets();
void DrawUserBullets();
void ShowExplosion();
void ShowGameOver();
void ShowTitleBar();
void ResetAllBullets();
void ShowLevelScreen();
void ShowTitleScreen();
void PauseGame();
void CheckForUserPause();
void CheckForUserBullet();
void CheckForUserMove();
void DrawEnemies();
void ResetGame();
int GetDigits();
void TurnIntoNumberArray();
void TurnFloatIntoNumberArray();
void DrawUserShip();
void ShowOptionsScreen();
void DrawSpace();
void SetUpImages();
void CheckForHighScore();
void WriteNewHighScores();
void GetOptions();
void ShowHighScoreScreen();
void WriteOptionsToFile();
void CheckForEnemeyBulletsLeft();
void IncreaseScore();
void EnterNewHigh();

//--------------------Variables-----------------------
char buffer[200];
char leveltext[200];

Image* myshipblack;
Image* myshipgreen;
Image* myshipred;
Image* myshipteal;
Image* enemyblue;
Image* enemyred;
Image* enemygreen;
//Image* enemyyellow;
Image* enemyteal;
Image* enemyblack;
Image* enemypurple;
//Image* enemyorange;
Image* greenbullet;
Image* tealbullet;
Image* purplebullet;
Image* bluebullet;
Image* back;
Image* pspX;
Image* redbullet;
Image* explosion;
Image* levelPic;
Image* powerup;
Image* titlebar;
Image* smallship;
Image* gover;
Image* gtext;
Image* nums;
Image* stext;
Image* ltext;
Image* atext;
Image* restext;
Image* percent;
Image* period;
Image* space0;
Image* space1;
Image* space2;
Image* space3;
Image* space4;
Image* space5;
//Image* space6;
//Image* space7;
//Image* space8;
//Image* space9;
//Image* space10;
//Image* space11;
//Image* space12;
//Image* space13;
//Image* space14;
//Image* space15;
//Image* space16;
//Image* space17;
Image* ops;
Image* wl;
Image* wr;
Image* gr;
Image* gl;
Image* rr;
Image* rl;
Image* highBack;
Image* greennums;
Image* trophy;
Image* letters;

Color ColorBlack = RGB(0,0,0);
Color ColorGreen = RGB(0,255,0);
Color white = RGB(255,255,255);

SceCtrlData pad;

const int maxX = 480,
		  maxY = 272,

		  yellow = 0,
		  blue = 1,
		  green = 2,
		  red = 3,
		  teal = 4,
		  orange = 5,
		  black = 6,
		  purple = 7,

		  numWidth = 20,
		  numHeight = 23,
		  gnumWidth = 18,
		  gnumHeight = 21,
		  scoreWidth = 119,
		  scoreHeight = 23,
		  levelWidth = 110,
		  levelHeight = 23,
		  accWidth = 180,
		  accHeight = 23,

		  letWidth = 27,
		  letHeight = 33,

		  bottomBarHeight = 12,
		  myshipWidth = 20,
		  myshipHeight = 20,
		  enemyWidth = 20,
		  enemyHeight = 24,
		  bulletHeight = 12,
		  bulletWidth = 4,
		  playerSpeedX = 3, //3
		  playerSpeedY = 2, //2
		  bulletSpeed = 5, // 5
		  explWidth = 17,
		  enemyPadding = 10,
		  explHeight = 19;

int i = 0,
	j = 0,
	k = 0,
	f = 0,
	count = 0,
	restart = 0,
	playerX = 100,
	playerY,
	defaultPlayerY,
	score = 0,
	shotsFired = 0,
	shotsHit = 0,
	lives = 3,//3
	currentLevel = -1,
	enemyCountTotal = 7,
	aliveCount,
	playerBulletsMax = 2,
	whoShot = 0,
	gameover = 0,
	enemyBulletsMax = 7,
	lastLoopX = 0,
	explode = 0,
	levelX = 0,
	enemyBulletSpeed = 3, //3
	drawPowerUp = 0,
	powerUpY = 0,
	dualShoot = 0,
	generalEnemySpeed = 1, //1
	justStarted = 1,
	nextStage = 0,
	notOkayToMove = 1,
	powerUpX = 0,
	tempLevel = 1,
	powerUpSpeed = 1,
	userColor = 6,
	bulletColor = 2,
	spacePic = 0,
	firsttime = 1,
	enemyBulletsLeft = 10,
	whoShotLast = 99999, // garbage number
	sameShotTimes = 0; // keeps track of how many time same enemy tries to shoot in a row

float accuracy = 0;
float accDecimal = 0;

int highScores[3];
int enemyX[50];
int	enemyY[50];
int enemySpeed[50];
int	enemyBoolDead[50];
int drawEnemyBullet[15];
int enemyBulletY[15];
int enemyBulletX[15];
int bulletX[10];
int bulletY[10];
int drawBullet[10];
int fireBullet[10];
int enemyColor[50];// colors are defined as constant integers
int theNums[10];
int scoreNums[10];
int levelNums[10];
int accNums[10];
int accDecNums[2];

int h1 = 0;
int high[10] = {0,0,0,0,0,0,0,0,0,0};

int enemyBeginX = 100;

FILE * pFile;
int temps[2] = {0,0};
//-----------------------------------------------------

int main()
{
	pspDebugScreenInit();
	SetupCallbacks();
	initGraphics();
	//pspAudioInit();
	//MP3_Init(1);

	SetUpImages();
	ResetGame();
	GetOptions(1);

	if(!myshipblack || !enemyblue || !enemyred  || !enemygreen || !greenbullet || !back || !redbullet || !explosion || !levelPic || !powerup || !enemypurple || !enemyteal || !enemyblack || !titlebar || !smallship ||!gover || !gtext || !nums || !stext || !ltext || !atext || !restext || !percent || !period || !myshipteal || !myshipred || !myshipgreen || !tealbullet || !bluebullet || !purplebullet || !space0 || !space1 || !space2 || !space3 || !space4 || !space5 || !ops || !wl || !wr || !gr || !gl ||!rr || !rl || !greennums ||!trophy ||!letters) //|| !enemyyellow || !enemyorange
	{
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

			MoveEnemies();

			enemyBulletsLeft = 0;
			CheckForEnemeyBulletsLeft();

			if (!aliveCount && !enemyBulletsLeft)
			{
				SwitchLevel();
				ShowLevelScreen();
				ResetAllBullets();
			}

			CheckForUserPause();
			CheckForUserMove();
			CheckForUserBullet();
			FireEnemyBullets();
			UpdateUserBullets();

			if (justStarted)
			{
				LevelBegin();
			}
			else
			{
				CheckForPowerUp();
				if (drawPowerUp)
				{
					DrawPowerUp();
				}
				DrawEngine();
			}
		}
	}
	sceKernelSleepThread();
	return 0;
}

int intersects(int x1, int y1, int width1, int height1, int x2, int y2, int width2, int height2)
{
	// (rect1.Right > rect2.Left) && (rect1.Left < rect2.Right) && (rect1.Bottom > rect2.Top) && (rect1.Top < rect2.Bottom)
	return ( (x1 + width1 >= x2) && (x1 <= x2 + width2) && (y1 + height1 >= y2) && (y1 <= y2 + height2) );
}

void DrawEnemies()
{
	// for each enemy up to the total enemies for that particular level,
	// first check if they are dead, if not, then:
	// draw each enemy based on color
	for (k = 0; k < enemyCountTotal; k++)
	{
		if (!enemyBoolDead[k])
		{
			if (enemyColor[k] == blue)
				blitAlphaImageToScreen(0,0,enemyWidth,enemyHeight,enemyblue,enemyX[k],enemyY[k]);
			else if (enemyColor[k] == green)
				blitAlphaImageToScreen(0,0,enemyWidth,enemyHeight,enemygreen,enemyX[k],enemyY[k]);
			else if (enemyColor[k] == red)
				blitAlphaImageToScreen(0,0,enemyWidth,enemyHeight,enemyred,enemyX[k],enemyY[k]);
			//else if (enemyColor[k] == yellow)
				//blitAlphaImageToScreen(0,0,enemyWidth,enemyHeight,enemyyellow,enemyX[k],enemyY[k]);
			else if (enemyColor[k] == teal)
				blitAlphaImageToScreen(0,0,enemyWidth,enemyHeight,enemyteal,enemyX[k],enemyY[k]);
			else if (enemyColor[k] == black)
				blitAlphaImageToScreen(0,0,enemyWidth,enemyHeight,enemyblack,enemyX[k],enemyY[k]);
			//else if (enemyColor[k] == orange)
				//blitAlphaImageToScreen(0,0,enemyWidth,enemyHeight,enemyorange,enemyX[k],enemyY[k]);
			else if (enemyColor[k] == purple)
				blitAlphaImageToScreen(0,0,enemyWidth,enemyHeight,enemypurple,enemyX[k],enemyY[k]);
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
void CheckForUserMove()
{
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

	////Move Up
	//if (pad.Buttons & PSP_CTRL_UP || pad.Ly <= 25)
	//{
	//	// Check For Hit Upper Boundary
	//	if (playerY - playerSpeedY <= maxY - 100)
	//	{
	//		playerY = maxY - 100;
	//	}
	//	else
	//	{
	//		playerY -= playerSpeedY;
	//	}
	//}

	////Move Down
	//if (pad.Buttons & PSP_CTRL_DOWN || pad.Ly >= 225)
	//{
	//	// Check For Hit Bottom of Screen
	//	if (playerY + playerSpeedY >= maxY - myshipHeight - 3)
	//	{
	//		playerY = maxY - myshipHeight - 3;
	//	}
	//	else
	//	{
	//		playerY += playerSpeedY;
	//	}
	//}

}

void CheckForUserBullet()
{
	for (i = 0; i < playerBulletsMax; i++)
	{
		if (pad.Buttons & PSP_CTRL_CROSS)
		{
			if (fireBullet[i] && !lastLoopX)
			{
				// this is not the first bullet so check if it is okay to fire again
				if (i)
				{
					// makes sure last bullet was not "just shot"
					if (bulletY[i-1] < 150)
					{
						if (dualShoot)
						{
							// bullet on left side (-5 from center of ship)
							bulletY[i] = playerY - bulletHeight;
							bulletX[i] = (int) (playerX + (myshipWidth / 2) - (bulletWidth / 2)) - 5;
							fireBullet[i] = 0;
							drawBullet[i] = 1;

							// bullet on right side (+5 from center of ship)
							bulletY[i + 1] = playerY - bulletHeight;
							bulletX[i + 1] = (int) (playerX + (myshipWidth / 2) - (bulletWidth / 2)) + 5;
							fireBullet[i + 1] = 0;
							drawBullet[i + 1] = 1;
							// cause you shot twice
							shotsFired ++;
						}
						if (!dualShoot)
						{
							bulletY[i] = playerY - bulletHeight;
							bulletX[i] = (int) (playerX + (myshipWidth / 2) - (bulletWidth / 2));
							fireBullet[i] = 0;
							drawBullet[i] = 1;
						}

						shotsFired ++;
						// this is so you can't hold down the "X" button
						lastLoopX = 1;
						// leaves loop so we don't fire twice
						break;
					}
				}
				else
				{
					if (dualShoot)
					{
						// bullet on left side (-5 from center of ship)
						bulletY[i] = playerY - bulletHeight;
						bulletX[i] = (int) (playerX + (myshipWidth / 2) - (bulletWidth / 2)) - 5;
						fireBullet[i] = 0;
						drawBullet[i] = 1;

						// bullet on right side (+5 from center of ship)
						bulletY[i + 1] = playerY - bulletHeight;
						bulletX[i + 1] = (int) (playerX + (myshipWidth / 2) - (bulletWidth / 2)) + 5;
						fireBullet[i + 1] = 0;
						drawBullet[i + 1] = 1;
						// cause you shot twice
						shotsFired ++;
					}
					else
					{
						bulletY[i] = playerY - bulletHeight;
						bulletX[i] = (int) (playerX + (myshipWidth / 2) - (bulletWidth / 2));
						fireBullet[i] = 0;
						drawBullet[i] = 1;
					}

					shotsFired ++;
					// this is so you can't hold down the "X" button
					lastLoopX = 1;
					// leaves loop so we don't fire twice
					break;
				}
			}
		}
		else
		{
			lastLoopX = 0;
		}
	}
}

void PauseGame()
{
	sprintf(buffer, "Paused");
	printTextScreen ((maxX / 2) - 20, (maxY / 2) - 5, buffer, ColorGreen);

	flipScreen();

	// makes sure it doesn't detect keypress twice in a row
	for (i = 0; i < 25; i++)
	{
		sceDisplayWaitVblankStart();
	}

	//PAUSE GAME
	do
	{
		sceCtrlReadBufferPositive(&pad, 1);

		if (pad.Buttons & PSP_CTRL_SELECT)
			sceKernelExitGame();

	} while (!(pad.Buttons & PSP_CTRL_START));

	// makes sure it doesn't detect keypress twice in a row
	for (i = 0; i < 25; i++)
	{
		sceDisplayWaitVblankStart();
	}
}

void ShowTitleScreen()
{
	if (!firsttime)
	{
		for (j = 0; j < 20; j++)
		{
			sceDisplayWaitVblankStart();
		}
	}
	firsttime = 0;
	int loops = 0;
	int showX = 0;
	int newTemp = 0;

	// flashes the x button on the title screen
	do
	{
		sceCtrlReadBufferPositive(&pad, 1);
		clearScreen(black);
		blitImageToScreen(0,0,maxX,maxY,back,0,0);

		score = 0;
		CheckForHighScore();
		TurnIntoNumberArray(high[0]);
		for (k = 0; k < GetDigits(high[0]); k++)
		{
			newTemp = (theNums[k] * gnumWidth);
			blitAlphaImageToScreen(newTemp,0,gnumWidth,gnumHeight,greennums,76 + 2 + (k * gnumHeight),173);
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
			blitAlphaImageToScreen(0,0,20,21,pspX,409,242);
		}
		loops ++;

		if (pad.Buttons & PSP_CTRL_CIRCLE)
			ShowOptionsScreen();

		if (pad.Buttons & PSP_CTRL_TRIANGLE)
			ShowHighScoreScreen();

		if (pad.Buttons & PSP_CTRL_SELECT)
			sceKernelExitGame();

		flipScreen();


	} while (!(pad.Buttons & PSP_CTRL_CROSS));

	ShowLevelScreen();
	ResetAllBullets();
}

void ShowLevelScreen()
{
	levelX = 0;
	sprintf(leveltext,"Level %i",currentLevel + 1);
	while (levelX < maxX)
	{
		clearScreen(black);
		printTextScreen((maxX / 2) - 20, (maxY / 2) - 5, leveltext, ColorGreen);
		blitImageToScreen(0,0,(maxX - levelX),maxY,levelPic,levelX,0);
		flipScreen();
		levelX ++;
	}
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
	drawPowerUp = 0;
	powerUpX = -10;
	powerUpY = -10;
}

void ShowTitleBar()
{
	blitImageToScreen(0,0,maxX,10,titlebar,0,0);
	// Shows current lives in titlebar
	sprintf(buffer, "Lives: ");
	printTextScreen (1, 1, buffer, ColorBlack);
	// Shows current score in titlebar
	sprintf(buffer, "Score: %i", score);
	printTextScreen (130, 1, buffer, ColorBlack);
	// Shows accuracy in titlebar
	sprintf(buffer, "Accuracy: %0.2f%%", accuracy);
	printTextScreen (230, 1, buffer, ColorBlack);
	//Shows Current Level in titlebar
	//sprintf(buffer, "Level: %i", currentLevel + 1);
	sprintf(buffer, "High: %i",high[0]);
	printTextScreen (380, 1, buffer, ColorBlack);
	for (f = 0; f < lives; f++)
	{
		if ( f <= 5)
		{
			blitAlphaImageToScreen(0,0,9,9,smallship, 50 +(f * (9 + 3)),0);
		}
	}

}

void ShowGameOver()
{
	// Shows your end game final score, accuracy, and level
	int xc = 60;
	int yc;
	int newTemp = 0;
	TurnIntoNumberArray(score);
	for (i = 0; i < 10; i++)
	{
		scoreNums[i] = theNums[i];
	}
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

	for (i = maxY * 2; i >= 3; i--)
	{
		sceCtrlReadBufferPositive(&pad, 1);

		if (pad.Buttons & PSP_CTRL_START || pad.Buttons & PSP_CTRL_SELECT || pad.Buttons & PSP_CTRL_CROSS)
		{
			i = 3;
		}

		yc = i / 2;
		clearScreen(black);
		blitImageToScreen(0,0,maxX,maxY,gover,0,0);
		blitAlphaImageToScreen(0,0,360,45,gtext,xc,yc);
		for (k = 0; k < GetDigits(score); k++)
		{
			newTemp = (scoreNums[k] * numWidth);
			blitAlphaImageToScreen(newTemp,0,numWidth,numHeight,nums,xc + scoreWidth + 10 + (k * numHeight),yc + 75);
		}
		blitAlphaImageToScreen(0,0,scoreWidth,scoreHeight,stext,xc,yc + 75);

		for (k = 0; k < GetDigits(currentLevel + 1); k++)
		{
			newTemp = (levelNums[k] * numWidth);
			blitAlphaImageToScreen(newTemp,0,numWidth,numHeight,nums,xc + levelWidth + 10 + (k * numHeight),yc + 75 + 30);
		}
		blitAlphaImageToScreen(0,0,levelWidth,levelHeight,ltext,xc,yc + 75 + 30);

		for (k = 0; k < GetDigits((int)accuracy); k++)
		{
			newTemp = (accNums[k] * numWidth);
			blitAlphaImageToScreen(newTemp,0,numWidth,numHeight,nums,xc + accWidth + 10 + (k * numHeight),yc + 75 + 30 + 30);
		}
		blitAlphaImageToScreen(0,0,accWidth,accHeight,atext,xc,yc + 75 + 30 + 30);

		blitAlphaImageToScreen(0,0,13,23,period,xc + accWidth + 10 +5 + (GetDigits((int)accuracy) * numWidth),yc + 75 + 30 + 30);
		for (k = 0; k < 2; k++)
		{
			newTemp = (accDecNums[k] * numWidth);
			blitAlphaImageToScreen(newTemp,0,numWidth,numHeight,nums,xc + accWidth + 10 + 13 + 10 + (GetDigits((int)accuracy) * numWidth)+ (k * numHeight),yc + 75 + 30 + 30);
		}

		blitAlphaImageToScreen(0,0,26,23,percent,xc + accWidth + 10 + 13 + 10 + 5 + (GetDigits((int)accuracy) * numWidth)+ (2 * numHeight),yc + 75 + 30 + 30);

		blitAlphaImageToScreen(0,0,maxX,12,restext,0,yc + (maxY - 12));

		flipScreen();
	}

	for (j = 0; j < 25; j++)
	{
		sceDisplayWaitVblankStart();
	}

	CheckForHighScore();
	EndGameLoop();
}

int GetDigits(int number)
{
	return ((int)log10(number) + 1);
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
void ShowExplosion()
{
	//MP3_Load("./sounds/explosion.mp3");
	//MP3_Play();
	for (i = 0; i < 12; i++)
	{
		clearScreen(black);
		DrawSpace();
		ShowTitleBar();
		DrawEnemies();

		blitAlphaImageToScreen((int)(i * explWidth),0,explWidth,explHeight,explosion,playerX,playerY);
		flipScreen();

		for (j = 0; j < 5; j++)
		{
			sceDisplayWaitVblankStart();
		}
	}

	//for (i = 0; i < (12 * 50); i++)
	//{
	//	clearScreen(black);
	//	blitImageToScreen(0,0,maxX,maxY,space,0,0);
	//	ShowTitleBar();
	//	DrawEnemies();
	//	blitAlphaImageToScreen((int)((i / 50) * explWidth),0,explWidth,explHeight,explosion,playerX,playerY);
	//	flipScreen();
	//}

	justStarted = 1;
	playerY = defaultPlayerY;
	ResetAllBullets();
	//MP3_Stop();
}

void DrawUserBullets()
{
	// Draws User bullets if they have been fired
	for (i = 0; i < playerBulletsMax; i++)
	{
		if (drawBullet[i])
		{
			if (bulletColor == 0)
				blitAlphaImageToScreen(0,0,bulletWidth,bulletHeight,greenbullet,bulletX[i],bulletY[i]);
			else if (bulletColor == 1)
				blitAlphaImageToScreen(0,0,bulletWidth,bulletHeight,tealbullet,bulletX[i],bulletY[i]);
			else if (bulletColor == 2)
				blitAlphaImageToScreen(0,0,bulletWidth,bulletHeight,bluebullet,bulletX[i],bulletY[i]);
			else if (bulletColor == 3)
				blitAlphaImageToScreen(0,0,bulletWidth,bulletHeight,purplebullet,bulletX[i],bulletY[i]);
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
			blitAlphaImageToScreen(0,0,bulletWidth,bulletHeight,redbullet,enemyBulletX[i],enemyBulletY[i]);
		}
	}
}

void DrawPowerUp()
{
	if (count == 3)
	{
		powerUpY += powerUpSpeed;
		count = 0;
	}
	else
	{
		count ++;
	}
	//resets power up
	if (powerUpY >= maxY)
	{
		drawPowerUp = 0;
		powerUpX = 0;
		powerUpY = 0;
	}
	// powerup is 10 x 10
	blitAlphaImageToScreen(0,0,10,10,powerup,powerUpX,powerUpY);

	// checks to see if player shoots the powerup icon
	for (i = 0; i < playerBulletsMax; i++)
	{
		if (intersects(bulletX[i],bulletY[i],bulletWidth,bulletHeight,powerUpX,powerUpY,10,10) || intersects(playerX, playerY, myshipWidth, myshipHeight, powerUpX,powerUpY,10,10))
		{
			dualShoot = 1;
			// this is so they can shoot the same amount of double bullets
			// as they could with single bullets
			playerBulletsMax *= 2;
			drawPowerUp = 0;
			powerUpX = -10;
			powerUpY = -10;
			lastLoopX = 0;
			for (k = 0; k < playerBulletsMax; k++)
			{
				drawBullet[k] = 0;
				fireBullet[k] = 1;
				bulletY[k] = playerY - bulletHeight;
			}
		}
	}
}

void EndGameLoop()
{
	do
	{
		sceCtrlReadBufferPositive(&pad, 1);

	} while (!(pad.Buttons & PSP_CTRL_START || pad.Buttons & PSP_CTRL_SELECT));

	if (pad.Buttons & PSP_CTRL_START)
	{
		// restarts the game if START is pressed
		ResetGame();
		SwitchLevel();
		ShowTitleScreen();
	}
	else
	{
		// exits if you press SELECT
		sceKernelExitGame();
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
					shotsHit ++;
					IncreaseScore();
					accuracy = ((float)shotsHit / shotsFired) * 100;
				}
			}
		}
	}
}

void FireEnemyBullets()
{
	for (i = 0; i < enemyBulletsMax; i ++)
	{
		if (!drawEnemyBullet[i])
		{
			// this is not the first bullet shot
			if (i)
			{
				// makes sure the last bullet is at least 40px away before shooting again
				if (enemyBulletY[i-1] > enemyBulletY[i] + enemyHeight + 40 + bulletHeight)
				{
					whoShot = rand() / (int)(((unsigned)RAND_MAX + 1) / enemyCountTotal);

					if (!enemyBoolDead[whoShot])
					{
						if (whoShotLast != whoShot)
						{
							drawEnemyBullet[i] = 1;
							enemyBulletY[i] = enemyY[whoShot] + enemyHeight + bulletHeight;
							enemyBulletX[i] = (int) (enemyX[whoShot] + (enemyWidth / 2) - (bulletWidth / 2));
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
					drawEnemyBullet[i] = 1;
					enemyBulletY[i] = enemyY[whoShot] + enemyWidth + bulletHeight;
					enemyBulletX[i] = (int) (enemyX[whoShot] + (enemyWidth / 2) - (bulletWidth / 2));
				}
			}
		}
		// Update the bullet
		else
		{
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

void UserIsHit()
{
	lives --;
	if (dualShoot)
	{
		// lose powerups if you had them
		dualShoot = 0;
		// so the max bullets go back to normal (like it was before they got the power up)
		playerBulletsMax /= 2;
	}
	// show explode sequence next draw loop
	explode = 1;
	// get rid of the evidence
	drawEnemyBullet[i] = 0;
	enemyBulletY[i] = enemyY[0];

	ShowExplosion();
	// Game Over
	if (!lives)
	{
		ShowGameOver();
	}
}

void DrawEngine()
{
	clearScreen(black);
	//Draws Background Image before anything else
	DrawSpace();
	if (drawPowerUp)
	{
		DrawPowerUp();
	}
	DrawUserShip();
	DrawUserBullets();
	DrawEnemies();
	DrawEnemyBullets();
	ShowTitleBar();
	flipScreen();
}

void SwitchLevel()
{
	currentLevel ++;
	switch (currentLevel % 7)
	{
		case 0:
			if (currentLevel != 0)
			{
				// new set of levels
				enemyCountTotal = 8;
				aliveCount = enemyCountTotal;
				// bonus life!!!
				lives ++;
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
		}

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
	playerY = defaultPlayerY;
	justStarted = 1;
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

					//}
					}
				}
			}

			enemyX[i] += enemySpeed[i];
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

void CheckForPowerUp()
{
	//only shows power up when this is 7 and user does not already have this power up
	if (!dualShoot && !drawPowerUp && rand() / (int)(((unsigned)RAND_MAX + 1) / 500) == 3)
	{
		drawPowerUp = 1;
		powerUpX = rand() / (int)(((unsigned)RAND_MAX + 1) / (maxX - 10));
	}
}

void LevelBegin()
{
	for (i = 50; i >=0; i--)
	{
		clearScreen(black);
		DrawSpace();
		ShowTitleBar();

		if (nextStage)
		{
			sprintf(buffer, "Bonus Life!");
			printTextScreen ((maxX / 2) - 30, (maxY / 2) - 25, buffer, ColorGreen);
		}

		if (i > 35)
		{
			sprintf(buffer, "3");
			printTextScreen ((maxX / 2) - 2, (maxY / 2) - 5, buffer, ColorGreen);
		}
		else if (i > 20)
		{
			sprintf(buffer, "2");
			printTextScreen ((maxX / 2) - 2, (maxY / 2) - 5, buffer, ColorGreen);
		}
		else if (i > 5)
		{
			sprintf(buffer, "1");
			printTextScreen ((maxX / 2) - 2, (maxY / 2) - 5, buffer, ColorGreen);
		}
		else
		{
			sprintf(buffer, "Go!");
			printTextScreen ((maxX / 2) - 10, (maxY / 2) - 5, buffer, ColorGreen);
		}

		DrawEnemies();

		DrawUserShip();
		flipScreen();

		sceCtrlReadBufferPositive(&pad, 1);
		CheckForUserMove();
	}
	justStarted = 0;
	nextStage = 0;
}

void DrawUserShip()
{
	if (userColor == 0)
		blitAlphaImageToScreen(0,0,myshipWidth,myshipHeight,myshipblack,playerX,playerY);
	else if (userColor == 1)
		blitAlphaImageToScreen(0,0,myshipWidth,myshipHeight,myshipred,playerX,playerY);
	else if (userColor == 2)
		blitAlphaImageToScreen(0,0,myshipWidth,myshipHeight,myshipgreen,playerX,playerY);
	else if (userColor == 3)
		blitAlphaImageToScreen(0,0,myshipWidth,myshipHeight,myshipteal,playerX,playerY);
}
void DrawSpace()
{
	if (spacePic == 0)
		blitImageToScreen(0,0,maxX,maxY,space0,0,0);
	else if (spacePic == 1)
		blitImageToScreen(0,0,maxX,maxY,space1,0,0);
	else if (spacePic == 2)
		blitImageToScreen(0,0,maxX,maxY,space2,0,0);
	else if (spacePic == 3)
		blitImageToScreen(0,0,maxX,maxY,space3,0,0);
	else if (spacePic == 4)
		blitImageToScreen(0,0,maxX,maxY,space4,0,0);
	else if (spacePic == 5)
		blitImageToScreen(0,0,maxX,maxY,space5,0,0);
	//else if (spacePic == 6)
	//	blitImageToScreen(0,0,maxX,maxY,space6,0,0);
	//else if (spacePic == 7)
	//	blitImageToScreen(0,0,maxX,maxY,space7,0,0);
	//else if (spacePic == 8)
	//	blitImageToScreen(0,0,maxX,maxY,space8,0,0);
	//else if (spacePic == 9)
	//	blitImageToScreen(0,0,maxX,maxY,space9,0,0);
	//else if (spacePic == 10)
	//	blitImageToScreen(0,0,maxX,maxY,space10,0,0);
	//else if (spacePic == 11)
	//	blitImageToScreen(0,0,maxX,maxY,space11,0,0);
	//else if (spacePic == 12)
	//	blitImageToScreen(0,0,maxX,maxY,space12,0,0);
	//else if (spacePic == 13)
	//	blitImageToScreen(0,0,maxX,maxY,space13,0,0);
	//else if (spacePic == 14)
	//	blitImageToScreen(0,0,maxX,maxY,space14,0,0);
	//else if (spacePic == 15)
	//	blitImageToScreen(0,0,maxX,maxY,space15,0,0);
	//else if (spacePic == 16)
	//	blitImageToScreen(0,0,maxX,maxY,space16,0,0);
	//else if (spacePic == 17)
	//	blitImageToScreen(0,0,maxX,maxY,space17,0,0);
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
		clearScreen(black);
		DrawSpace();
		blitAlphaImageToScreen(0,0,maxX,maxY,ops,0,0);
		playerX = (maxX / 2) - (myshipWidth / 2);
		DrawUserShip();

		if (pad.Buttons & PSP_CTRL_DOWN)
		{
			index = 1;
			for (j = 0; j < 15; j++)
			{
				sceDisplayWaitVblankStart();
			}
		}
		if (pad.Buttons & PSP_CTRL_UP)
		{
			index = 0;
			for (j = 0; j < 15; j++)
			{
				sceDisplayWaitVblankStart();
			}
		}	
		if (pad.Buttons & PSP_CTRL_RTRIGGER)
		{
			spacePic ++;
			if (spacePic >= 6)
			{
				spacePic = 0;
			}
			for (j = 0; j < 15; j++)
			{
				sceDisplayWaitVblankStart();
			}
		}
		if (pad.Buttons & PSP_CTRL_LTRIGGER)
		{
			spacePic --;
			if (spacePic < 0)
			{
				spacePic = 5;
			}
			for (j = 0; j < 15; j++)
			{
				sceDisplayWaitVblankStart();
			}
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

		if (bulletColor == 0)
			blitAlphaImageToScreen(0,0,bulletWidth,bulletHeight,greenbullet,bX,bY);
		else if (bulletColor == 1)
			blitAlphaImageToScreen(0,0,bulletWidth,bulletHeight,tealbullet,bX,bY);
		else if (bulletColor == 2)
			blitAlphaImageToScreen(0,0,bulletWidth,bulletHeight,bluebullet,bX,bY);
		else if (bulletColor == 3)
			blitAlphaImageToScreen(0,0,bulletWidth,bulletHeight,purplebullet,bX,bY);

		if (pad.Buttons & PSP_CTRL_RIGHT)
		{
			temps[index] ++;
			if (temps[index] >= 4)
			{
				temps[index] = 0;
			}
	
			if (index == 1)
				blitAlphaImageToScreen(0,0,7,11,rr,playerX + myshipWidth + 10, playerY + 4);
			else
				//blitAlphaImageToScreen(0,0,7,11,rr,bX + bulletWidth + 10, bY + 1);
				blitAlphaImageToScreen(0,0,7,11,rr,playerX + myshipWidth + 10, bY + 1);
			flipScreen();
			flip = 0;

			for (j = 0; j < 15; j++)
			{
				sceDisplayWaitVblankStart();
			}
		} 
		if (pad.Buttons & PSP_CTRL_LEFT)
		{
			temps[index] --;
			if (temps[index] < 0)
			{
				temps[index] = 3;
			}

			if (index == 1)
				blitAlphaImageToScreen(0,0,7,11,rl,playerX - 10 - 7, playerY + 4);
			else
				//blitAlphaImageToScreen(0,0,7,11,rl,bX - 10 - 7, bY + 1);
				blitAlphaImageToScreen(0,0,7,11,rl,playerX - 10 - 7, bY + 1);
			flipScreen();
			flip = 0;

			for (j = 0; j < 15; j++)
			{
				sceDisplayWaitVblankStart();
			}
		}

		if (flip)
			flipScreen();
		else
			flip = 1;
	} while (!(pad.Buttons & PSP_CTRL_START));

	WriteOptionsToFile();

	for (j = 0; j < 25; j++)
	{
		sceDisplayWaitVblankStart();
	}
}
void CheckForHighScore()
{
	// reads all of the high scores
    pFile = fopen("data v13.dat","r");
	if (pFile != NULL)
	{
		for (i = 0; i < 10; i++)
		{
			fread(&h1,sizeof(h1),1,pFile);
			high[i] = h1;
		}
	}
    fclose(pFile);

	int newHigh = 0;
	// this checks to see if it beat any high scores
	for (i = 0; i <10; i ++)
	{
		if (score > high[i])
		{
			newHigh = 1;
			// if it did, then push all the scores down one to make room for new high score
			// it is done backwards so that you aren't reading what you wrote
			// since you don't need score 9, you save score 8 to it
			// then it opens room for 7 to move into where 8 was without losing 8 (since it is now in 9)
			for (j = 8; j >i-1; j--)
			{
				high[j+1] = high[j];
			}
			high[i] = score;
			break;
		}
	}
	if (newHigh)
	{
		WriteNewHighScores();
		EnterNewHigh();
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

void WriteNewHighScores()
{
    pFile = fopen("data v13.dat","w");

	if (pFile != NULL)
	{
		for (i=0;i<10; i++)
		{
			fwrite(&high[i],sizeof(h1),1,pFile);
		}
	}
    fclose(pFile);
}
void ShowHighScoreScreen()
{
	do
	{
		clearScreen(black);
		blitAlphaImageToScreen(0,0,maxX,maxY,highBack,0,0);
		flipScreen();
		sceCtrlReadBufferPositive(&pad, 1);
	} while (!(pad.Buttons & PSP_CTRL_START));
	for (j = 0; j < 25; j++)
	{
		sceDisplayWaitVblankStart();
	}
}

void EnterNewHigh()
{
	for (j = 0; j < 35; j++)
		sceDisplayWaitVblankStart();
	int hIndex = 0;
	int Vindex[3] = {0,0,0};
	int letX[3] = {200, 200 + 5 + letWidth,  200 + 5 + 5 + (2*letWidth)};
	int letY = 200;
	int maxLetters = 35; //35
	// TODO: code her to set chars and Vindex
	do
	{
		sceCtrlReadBufferPositive(&pad, 1);
		
		if (pad.Buttons & PSP_CTRL_RIGHT)
		{
			hIndex++;
			if (hIndex > 2)
				hIndex = 0;
			for (j = 0; j < 15; j++)
				sceDisplayWaitVblankStart();
		}
		if (pad.Buttons & PSP_CTRL_LEFT)
		{
			hIndex--;
			if (hIndex < 0)
				hIndex = 2;
			for (j = 0; j < 15; j++)
				sceDisplayWaitVblankStart();
		}

		if (pad.Buttons & PSP_CTRL_UP)
		{
			Vindex[hIndex]--;
			if (Vindex[hIndex] < 0)
				Vindex[hIndex] = maxLetters;
			for (j = 0; j < 7; j++)
				sceDisplayWaitVblankStart();
		}
		if (pad.Buttons & PSP_CTRL_DOWN)
		{
			Vindex[hIndex]++;
			if (Vindex[hIndex] > maxLetters)
				Vindex[hIndex] = 0;
			for (j = 0; j < 7; j++)
				sceDisplayWaitVblankStart();
		}

		clearScreen(ColorBlack);
		blitImageToScreen(0,0,maxX,maxY,trophy,0,0);

		for (i = 0; i < 3; i++)
			blitAlphaImageToScreen((Vindex[i] % 18) * letWidth,(Vindex[i] / 18) * letHeight,letWidth,letHeight,letters,letX[i],letY);
	
		flipScreen();


	} while (!(pad.Buttons & PSP_CTRL_START));
}
void IncreaseScore()
{
	//score += currentLevel + 1;
	score += 123;
}

void ResetGame()
{
	i = 0;
	j = 0;
	k = 0;
	f = 0;
	restart = 0;
	defaultPlayerY = maxY - myshipHeight - 3,
	playerX = 100;
	score = 0;
	shotsFired = 0;
	shotsHit = 0;
	lives = 3; // 3
	currentLevel = -1;
	enemyCountTotal = 7;
	playerBulletsMax = 2;
	whoShot = 0;
	gameover = 0;
	enemyBulletsMax = 7;
	lastLoopX = 0;
	explode = 0;
	levelX = 0;
	enemyBulletSpeed = 4; //3
	drawPowerUp = 0;
	powerUpX = -10;
	powerUpSpeed = 2; //2
	powerUpY = -10;
	dualShoot = 0; //0
	generalEnemySpeed = 1; //1
	justStarted = 1;
	nextStage = 0;
	notOkayToMove = 1;
	tempLevel = 1;
	whoShotLast = 99999; // garbage number
	sameShotTimes = 0; // keeps track of how many time same enemy tries to shoot in a row
	accuracy = 0;
	aliveCount = enemyCountTotal;
	playerY = defaultPlayerY;
	h1 = 0;
	enemyBulletsLeft = enemyBulletsMax;
 
    for (i = 0; i < 10; i++)
		high[i] = 0;
}
void SetUpImages()
{
	//---------------------Images--------------------------
	sprintf(buffer,"./images/myshipblack.png");
	myshipblack = loadImage(buffer);
	sprintf(buffer,"./images/myshipgreen.png");
	myshipgreen = loadImage(buffer);
	sprintf(buffer,"./images/myshipred.png");
	myshipred = loadImage(buffer);
	sprintf(buffer,"./images/myshipteal.png");
	myshipteal = loadImage(buffer);
	sprintf(buffer,"./images/blueenemy.png");
	enemyblue = loadImage(buffer);
	//sprintf(buffer,"./images/yellowenemy.png");
	//enemyyellow = loadImage(buffer);
	sprintf(buffer,"./images/redenemy.png");
	enemyred = loadImage(buffer);
	sprintf(buffer,"./images/greenenemy.png");
	enemygreen = loadImage(buffer);
	sprintf(buffer,"./images/tealenemy.png");
	enemyteal = loadImage(buffer);
	sprintf(buffer,"./images/blackenemy.png");
	enemyblack = loadImage(buffer);
	//sprintf(buffer,"./images/orangeenemy.png");
	//enemyorange = loadImage(buffer);
	sprintf(buffer,"./images/purpleenemy.png");
	enemypurple = loadImage(buffer);
	sprintf(buffer,"./images/greenbullet.png");
	greenbullet = loadImage(buffer);
	sprintf(buffer,"./images/tealbullet.png");
	tealbullet = loadImage(buffer);
	sprintf(buffer,"./images/purplebullet.png");
	purplebullet = loadImage(buffer);
	sprintf(buffer,"./images/bluebullet.png");
	bluebullet = loadImage(buffer);
	sprintf(buffer,"./images/redbullet.png");
	redbullet = loadImage(buffer);
	sprintf(buffer,"./images/newback3.png");
	back = loadImage(buffer);
	sprintf(buffer,"./images/space0.png");
	space0 = loadImage(buffer);
	sprintf(buffer,"./images/space1.png");
	space1 = loadImage(buffer);
	sprintf(buffer,"./images/space2.png");
	space2 = loadImage(buffer);
	sprintf(buffer,"./images/space3.png");
	space3 = loadImage(buffer);
	sprintf(buffer,"./images/space4.png");
	space4 = loadImage(buffer);
	sprintf(buffer,"./images/space5.png");
	space5 = loadImage(buffer);
	//sprintf(buffer,"./images/space/space6.png");
	//space6 = loadImage(buffer);
	//sprintf(buffer,"./images/space/space7.png");
	//space7 = loadImage(buffer);
	//sprintf(buffer,"./images/space/space8.png");
	//space8 = loadImage(buffer);
	//sprintf(buffer,"./images/space/space9.png");
	//space9 = loadImage(buffer);
	//sprintf(buffer,"./images/space/space10.png");
	//space10 = loadImage(buffer);
	//sprintf(buffer,"./images/space/space11.png");
	//space11 = loadImage(buffer);
	//sprintf(buffer,"./images/space/space12.png");
	//space12 = loadImage(buffer);
	//sprintf(buffer,"./images/space/space13.png");
	//space13 = loadImage(buffer);
	//sprintf(buffer,"./images/space/space14.png");
	//space14 = loadImage(buffer);
	//sprintf(buffer,"./images/space/space15.png");
	//space15 = loadImage(buffer);
	//sprintf(buffer,"./images/space/space16.png");
	//space16 = loadImage(buffer);
	//sprintf(buffer,"./images/space/space17.png");
	//space17 = loadImage(buffer);
	sprintf(buffer,"./images/newpspX.png");
	pspX = loadImage(buffer);
	sprintf(buffer,"./images/explsmall2.png");
	explosion = loadImage(buffer);
	sprintf(buffer,"./images/level.png");
	levelPic = loadImage(buffer);
	sprintf(buffer,"./images/powerup.png");
	powerup = loadImage(buffer);
	sprintf(buffer,"./images/titlebar.png");
	titlebar = loadImage(buffer);
	sprintf(buffer,"./images/smallship.png");
	smallship = loadImage(buffer);
	sprintf(buffer,"./images/gameover.png");
	gover = loadImage(buffer);
	sprintf(buffer,"./images/gtext.png");
	gtext = loadImage(buffer);
	sprintf(buffer,"./images/nums.png");
	nums = loadImage(buffer);
	sprintf(buffer,"./images/score.png");
	stext = loadImage(buffer);
	sprintf(buffer,"./images/ltext.png");
	ltext = loadImage(buffer);
	sprintf(buffer,"./images/atext.png");
	atext = loadImage(buffer);
	sprintf(buffer,"./images/restext.png");
	restext = loadImage(buffer);
	sprintf(buffer,"./images/percent.png");
	percent = loadImage(buffer);
	sprintf(buffer,"./images/period.png");
	period = loadImage(buffer);
	sprintf(buffer,"./images/ops.png");
	ops = loadImage(buffer);
	sprintf(buffer,"./images/wl.png");
	wl = loadImage(buffer);
	sprintf(buffer,"./images/gl.png");
	gl = loadImage(buffer);
	sprintf(buffer,"./images/wr.png");
	wr = loadImage(buffer);
	sprintf(buffer,"./images/gr.png");
	gr = loadImage(buffer);
	sprintf(buffer,"./images/rr.png");
	rr = loadImage(buffer);
	sprintf(buffer,"./images/rl.png");
	rl = loadImage(buffer);
	sprintf(buffer,"./images/highscores.png");
	highBack = loadImage(buffer);
	sprintf(buffer,"./images/greennums.png");
	greennums = loadImage(buffer);
	//sprintf(buffer,"./images/trophy.png");
	//trophy = loadImage(buffer);
	//sprintf(buffer,"./images/letters.png");
	//letters = loadImage(buffer);
	//-----------------------------------------------------
}



///////////TODO BEFORE 1.3////////////////////////////////
// get new scoring scheme
// finish high score table displaying
// on game over, say "New High Score!" 
//		- possibly what place or show high score table
// fix dualshoot glitch
//////////////////////////////////////////////////////////
