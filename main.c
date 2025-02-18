//***************************************************************
// Birdman's Shooter PSP
//
// Anthony Casagrande
//
// Project Start Date: 10/23/07
// Version 1.0: submitted to psp-hacks.com on 10/28/07
// Version 1.2: submitted to psp-hacks.com on 11/02/07
// Version 1.3: submitted to psp-hacks.com on 05/01/08
// Version 1.4: submitted to psp-hacks.com on ??/??/08
// Latest Revision: 5/3/08
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
//
// Change Log From 1.3 to 1.4:
// + Boss level with boss health meter
// + Bonus and boss level opening screens are now smoother
// + Changed loading screen splash image
// + Main menu is now navigated using an arrow
// + Some changes to the look of the options screen
// + Practice mode: choose lives, enemy speed, and enemy bullet speed
// + You can now change space image at practice screen using L and R
// + You can now cancel options and practice by using O
// + Practice options are now saved to file
// + Survival mode: one life and no powerups
// + Survival mode shows level instead of lives on titlebar
// + Survival mode has its own high score table
// + Main menu now shows personal best for survival mode
// + Fixed glitch how you can't use analog stick during first level screen
//
//
// Developer Change Notes:
// + only loads space pic into memory one at a time game loads faster, but options is slower, and no more crashing due to full vram
// + loading screen is freed when not needed
// + moved explosion to right by 2px
//***************************************************************
 
#include "variables.h"

int  main()
{
	//scePowerSetClockFrequency(333, 333, 111);
	pspDebugScreenInit();
	SetupCallbacks();
	initGraphics();

    //Initialization of the Oslib library
    oslInit(0);

	//load sound files and initiate the audio
	//music = oslLoadSoundFile("./sounds/campaign.bgm", OSL_FMT_STREAM);
	music2 = oslLoadSoundFile("./sounds/race.bgm", OSL_FMT_STREAM);
	explSound = oslLoadSoundFile("./sounds/explosion.bgm", OSL_FMT_NONE);
	oslInitAudio();

	// get psp nickname
	//sceUtilityGetSystemParamString(1, nickname, 25);

	sprintf(buffer,"./images/loading/psphacks.png"); 
	psphacks = loadImage(buffer);
	sprintf(buffer,"./images/loading/border.png");
	border = loadImage(buffer);
	sprintf(buffer,"./images/loading/meter0.png");
	meter = loadImage(buffer);
	sprintf(buffer,"./images/loading/meter1.png");
	meter1 = loadImage(buffer); 

	//CreateDummyScores();
	SetUpImages();
	//free up the loading image now that it is not needed
	freeImage(psphacks);

	ResetGame(); 
	GetOptions(1);
	
	
	
	//only loads your current space image (do after getting options from file)
	sprintf(buffer,"./images/space/space%i.png",spacePic);
	space[spacePic] = loadImage(buffer);

	//scePowerSetClockFrequency(222, 222, 111);

	if(0){
		//Image load failed
		printf("Image load failed!\n\nMake sure all of the images are in the folder named \"images\"\n"); 
	}
	else
	{
		//these set up the ability to use the analog stick
		sceCtrlSetSamplingCycle(0);
		sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);

		SwitchLevel();
		ShowTitleScreen();

		while (1)
		{
			sceCtrlReadBufferPositive(&pad, 1);
			
			

			enemyBulletsLeft = 0;
			CheckForEnemeyBulletsLeft();
			CheckForBossBulletsLeft();
			//includes code to find how many user bullets are left
			GetPlayerBulletsLeft();

			//makes sure all enemy bullets and all player bullets have left the screen
			if (!aliveCount && !enemyBulletsLeft && !playerBulletsLeft && !bossBulletsLeft && !drawBoss)
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
				
				//will only let you fire if there are enemies or a boss on screen
				if (aliveCount || drawBoss){
					CheckForUserBullet();
				}

				//CheckForAIBullet();
				//CheckForAIMove();

				UpdateUserBullets();

				if (!bonusLevel){
					if (drawBoss){
						MoveBoss();
						CheckForBossBullets();
					}
					//do this always because the boss can be dead, but his bullets still there
					UpdateBossBullets();

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
			//hopefully syncs the frames
			//Wait(1);
			 //Synchronizes the Audio
			//oslAudioVSync();
		}//end while(1)

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

void CheckForBossBullets(){ 
	GetLastBossBulletShot();
	//its okay to fire and last bullet is more than 50 away
	if (!drawBossBullet[lastBossBullet] && ((lastBossBullet != 0 && GetHighestEnemyBullet() > bossHeight + 100) || (lastBossBullet == 0)))
	{
		for (z = 0; z < 3; z++){
			drawBossBullet[lastBossBullet + z] = 1;
			bossBulletY[lastBossBullet + z] = bossY + bossHeight;
			bossBulletX[lastBossBullet + z] = (int) (bossX + (z * (bossWidth / 2)));
		}
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

void CheckForBossBulletsLeft(){
	bossBulletsLeft = 0;
	for (i = 0; i < bossBulletsMax; i++)
	{
		if (drawBossBullet[i])
			bossBulletsLeft++;
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

	if (!survival){
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
	}else{
		newHigh = 0;
		// this checks to see if it beat any high scores
		for (i = 0; i <10; i ++)
		{
			if (score >= survivalHigh[i])
			{
				// sorts same scores by accuracy
				if ((score > survivalHigh[i]) || (score == survivalHigh[i] && (int)(accuracy * 100) > survivalHighAccuracy[i]))
				{
					newHigh = 1;
					newHighLoc = i;
					// if it did, then push all the scores down one to make room for new high score
					// it is done backwards so that you aren't reading what you wrote
					// since you don't need score 9, you save score 8 to it
					// then it opens room for 7 to move into where 8 was without losing 8 (since it is now in 9)
					for (j = 8; j >i-1; j--)
					{
						survivalHigh[j+1] = survivalHigh[j];
						survivalHighLevel[j+1] = survivalHighLevel[j];
						survivalHighAccuracy[j+1] = survivalHighAccuracy[j];
					}
					survivalHigh[i] = score;
					survivalHighLevel[i] = currentLevel + 1;
					// accuray is * 100 to save decimal so 43.71 is now 4371
					survivalHighAccuracy[i] = (int)(accuracy * 100);
					break;
				}
			}
		}
	}

	if (newHigh){
		WriteNewHighScores();
	}
}
void CheckForHitBoss(){
	if (intersects(bulletX[i], bulletY[i], bulletWidth, bulletHeight, bossX, bossY, bossWidth, bossHeight)){
		drawBullet[i] = 0;
		fireBullet[i] = 1;
		bulletY[i] = playerY - bulletHeight;
		shotsHit ++;
		IncreaseScore(-1);
		bossHealth -= 10;
		//you killed the boss
		if (bossHealth <= 0){
			drawBoss = 0;
		}
	}
}

void CheckForHitEnemy(){
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

void CheckForPowerUp()
{
	//no powerups for survival
	//only shows power up when random is 7 and user does not already have this power up and more than 3 enemies are alive
	if (!survival && !dualShoot && aliveCount > 3 && !drawPowerUp[0] && rand() / (int)(((unsigned)RAND_MAX + 1) / 1250) == 3)//1500?
	{
		drawPowerUp[0] = 1;
		powerUpX[0] = rand() / (int)(((unsigned)RAND_MAX + 1) / (maxX - 10));
	}
	if (!survival && !drawPowerUp[0] && !frozen && aliveCount > 3 && !drawPowerUp[1] && rand() / (int)(((unsigned)RAND_MAX + 1) / 1500) == 7)//1500?
	{
		drawPowerUp[1] = 1;
		powerUpX[1] = rand() / (int)(((unsigned)RAND_MAX + 1) / (maxX - 10));
	}
}

void CheckForSpaceChange(){
	if (pad.Buttons & PSP_CTRL_RTRIGGER)
	{
		//clear current image from memory
		freeImage(space[spacePic]);

		spacePic ++;
		if (spacePic >= numOfSpace)
		{
			spacePic = 0;
		}
		Wait(15);

		//load the new space image to memory
		sprintf(buffer,"./images/space/space%i.png",spacePic);
		space[spacePic] = loadImage(buffer);
	}
	if (pad.Buttons & PSP_CTRL_LTRIGGER)
	{
		//clear current image from memory
		freeImage(space[spacePic]);

		spacePic --;
		if (spacePic < 0)
		{
			spacePic = (numOfSpace - 1);
		}
		Wait(15);

		//load the new space image to memory
		sprintf(buffer,"./images/space/space%i.png",spacePic);
		space[spacePic] = loadImage(buffer);
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
void DrawArrow(){
	int zeroY = 136;
	int Ypadding = 27;
	int arrowX = 409;
	//figures the Y based off of which index you are currently selecting
	int arrowY = zeroY + (mainIndex * Ypadding);
	//32x23 arrow
	blitAlphaImageToScreen(0,0,32,23,arrow,arrowX,arrowY);
}

void DrawBoss(){
	if (drawBoss){
		blitAlphaImageToScreen(0,0,bossWidth,bossHeight,boss,bossX,bossY);
	}
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


void DrawBossBullets(){
	// Draws Boss Bullets
	for (i = 0; i < bossBulletsMax; i ++)
	{
		if (drawBossBullet[i])
		{
			if (i%3 == 0){
				//bullet slanted to the left
				blitAlphaImageToScreen(0,0,bulletWidth,bulletHeight,bullet[5],bossBulletX[i],bossBulletY[i]);
			}else if (1%3 == 1){
				//bullet straight down
				blitAlphaImageToScreen(0,0,bulletWidth,bulletHeight,bullet[5],bossBulletX[i],bossBulletY[i]);
			}else if (i%3 == 2){
				//bullet slanted to the right
				blitAlphaImageToScreen(0,0,bulletWidth,bulletHeight,bullet[5],bossBulletX[i],bossBulletY[i]);
			}
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
	DrawBossBullets();
	DrawEnemies();
	//draw boss overtop of enemies
	DrawBoss();
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
		menuHigh = 0;
		ShowHighScoreScreen(newHighLoc);
	}else{
		if (!practiceLevel){
			menuHigh = 0;
			ShowHighScoreScreen(-1);
		}else{
			ShowTitleScreen();
		}
	}
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

int  GetHighestBossBullet(){
	int top = maxY;
	for (k = 0; k < bossBulletsMax; k++){
		if (drawBossBullet[k] && bossBulletY[k] < top){
			top = bossBulletY[k];
		}
	}
	return top;
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

void GetLastBossBulletShot(){
	lastBossBullet = 0;
	int first = 1;
	for (z = 0; z < bossBulletsMax; z++){
		if (!drawBossBullet[z]){
			//the last bullet will be overwritten with each larger z and the last one is the largest
			lastBossBullet = z;
			first = 1;
			for (j = z; j < bossBulletsMax; j++){
				if (drawBossBullet[z]){
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
		pFile = fopen("./data/data2 v14.dat","r");
		if (pFile != NULL)
		{
			int temp = 0;

			//color options
			fread(&temp,sizeof(spacePic),1,pFile);
			spacePic = temp;
			fread(&temp,sizeof(bulletColor),1,pFile);
			bulletColor = temp;
			fread(&temp,sizeof(userColor),1,pFile);
			userColor = temp;

			//these are the practice options
			fread(&temp,sizeof(practiceSelection[0]),1,pFile);
			practiceSelection[0] = temp;
			fread(&temp,sizeof(practiceSelection[1]),1,pFile);
			practiceSelection[1] = temp;
			fread(&temp,sizeof(practiceSelection[2]),1,pFile);
			practiceSelection[2] = temp;
			fread(&temp,sizeof(practiceSelection[3]),1,pFile);
			practiceSelection[3] = temp;
		}
		fclose(pFile);
	}
	temps[0] = bulletColor;
	temps[1] = userColor;
}


void GetPersonalBest(){
	//get the regular personal best
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

	//get the survival personal best
	survivalpersonalBest = 0;
	//counts down top 10 high scores
	for (z = 0; z < 10; z++){
		if (survivalHigh[z]%1000 != 0 && survivalHighAccuracy[z] != 4000){
			//this is not a preset score
			survivalpersonalBest = survivalHigh[z];
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
	if ((int)(TimeNow_) - (int)(TimeStart_) >= 10 || !frozen){
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
	int value1 = 0;

	if (enemyIndex != -1){
		int color = enemyColor[enemyIndex];
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
	}else{
		//-1 index is a boss
		value1 = 350;
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
		DrawBoss();
		DrawUserShip();
		flipScreen();
	}
	justStarted = 0;
	nextStage = 0;
}
void MoveBoss(){
	//move the boss
	bossX += bossSpeed;
	//check for hit left wall
	if (bossX <= 0){
		bossX = 0;
		bossSpeed *= -1;
	//check for hit right wall
	}else if (bossX + bossWidth>= maxX){
		bossX = maxX - bossWidth;
		bossSpeed *= -1;
	//check for random direction change
	}else if (rand() / (int)(((unsigned)RAND_MAX + 1) / 1000) == 3){
		bossSpeed *= -1;
	}
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
					//hit the boss
					else if (((enemyX[j] < bossX && bossSpeed < 0 && enemySpeed[j] > 0) || (enemyX[j] > bossX && bossSpeed > 0 && enemySpeed[j] < 0)) && intersects(enemyX[j],enemyY[j],enemyWidth,enemyHeight,bossX,bossY,bossWidth,bossHeight))
					{
						enemySpeed[j] *= -1;
						bossSpeed *= -1;
						//TeleportEnemy(j);
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
	oslPauseSound(music2, 0);
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
	oslPlaySound(music2, 0);
}

void PlayGame(){
	oslPlaySound(music2, 0);
	oslSetSoundLoop(music2, 1);
	bossOnly = 0;
	practiceLevel = 0;
	ResetAllBullets();
	ResetGame();
	SwitchLevel();
	ShowLevelScreen();
}
void ReadHighScores(){
	// reads all of the high scores for regular mode
    pFile = fopen("./data/data v13.dat","r");
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


	// reads all of the high scores for survival mode
    pFile = fopen("./data/data3 v14.dat","r");
	if (pFile != NULL)
	{
		for (i = 0; i < 30; i++)
		{
			// read high scores
			if (i < 10){
				fread(&h1,sizeof(h1),1,pFile);
				survivalHigh[i] = h1;
				survivalHigh[i] ^= KEYS[i];
			}
			//read level
			else if (i < 20)
			{
				fread(&h1,sizeof(h1),1,pFile);
				survivalHighLevel[i - 10] = h1;
				survivalHighLevel[i - 10] ^= KEYS[i - 10];
			}
			// read accuracy
			else if (i < 30)
			{
				fread(&h1,sizeof(h1),1,pFile);
				survivalHighAccuracy[i - 20] = h1;
				survivalHighAccuracy[i - 20] ^= KEYS[i - 20];
			}
		}

		for (i = 0; i < 10; i++){
			if (survivalHighAccuracy[i] < 100){
				survivalHighAccuracy[i] *= 100;
			}
		}
	}
    fclose(pFile);
	survivalhightop = survivalHigh[0]/1000;
	survivalhighlow = survivalHigh[0]%1000;
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
	for (i = 0; i < bossBulletsMax; i++)
	{
		drawBossBullet[i] = 0;
		bossBulletY[i] = 0;
	}
	for (i = 0; i < 2; i++)
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
	if (currentLevel > 12){
		perfectBonus *= 2.5;
		bonus *= 2.5;
		if (currentLevel > 22){
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

	if (currentLevel > 10 && !survival){
		//display the "+" sign
		int yc2 = yc + (3 * padding);
		if (bonusHit != 30){
			yc2 = yc + (2 * padding);
		}
		blitAlphaImageToScreen(0,0,26,25,plus,xc,yc2); 
		//display the user bonus ship
		blitAlphaImageToScreen(0,0,myshipWidth,myshipHeight,myship[userColor],xc + 5 + (numWidth),yc2 + 3);
	}
	
	if (currentLevel > 10 && !survival){
		//second bonus level so give life automatically
		lives ++;
	}
	if (bonusHit == 30){ 
		if (!survival){
			// bonus life!!!  
			lives ++;
		}
		score += perfectBonus;

		//shows the perfect bonus number 
		TurnIntoNumberArray(perfectBonus);
		for (i = 0; i < 10; i++)
		{
			accNums[i] = theNums[i];
		}
		//display the "+" sign
		blitAlphaImageToScreen(0,0,26,25,plus,xc,yc + (2 * padding));
		for (k = 0; k < GetDigits(perfectBonus); k++)
		{
			newTemp = (accNums[k] * numWidth);
			blitAlphaImageToScreen(newTemp,0,numWidth,numHeight,overnums,xc + 5 + ((k+1) * numWidth),yc + (2 * padding));
		}

		//if this is the second bonus level, it shows a second ship to the right
		int xc2 = xc + 5 + (numWidth);
		if (currentLevel > 12){
			xc2 += (5 + myshipWidth);
		}
		if (!survival){
			//display the "+" sign
			blitAlphaImageToScreen(0,0,26,25,plus,xc,yc + (3 * padding));
			//display the user bonus ship
			blitAlphaImageToScreen(0,0,myshipWidth,myshipHeight,myship[userColor],xc2,yc + (3 * padding) + 3);
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
	oslPlaySound(explSound, 1);
	for (i = 0; i < 12; i++)
	{
		clearScreen(ColorBlack);
		DrawSpace();
		ShowTitleBar();
		DrawEnemies();
		if (drawBoss){
			DrawBoss();
		}

		blitAlphaImageToScreen((int)(i * explWidth),0,explWidth,explHeight,explosion,playerX + 2,playerY);
		flipScreen();
		Wait(5);
	}
	justStarted = 1;
	playerY = defaultPlayerY; 
	ResetAllBullets();
	oslStopSound(explSound);
}




void ShowGameOver()
{
	oslStopSound(music2);

	//oslStopSound(music2);
    //oslPlaySound(music, 0);
    //oslSetSoundLoop(music, 1);

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

	if (!practiceLevel){
		CheckForHighScore();
	}
	EndGameLoop();
}


void ShowHighScoreScreen(int coloredRow)
{
	int sWidth = 147;
	int sHeight = 33;
	int beginY = 59;
	int beginX = 170;

	if (!menuHigh){
		ReadHighScores();
		clearScreen(ColorBlack);
		blitAlphaImageToScreen(0,0,maxX,maxY,highBack,0,0);
		if (!survival){
			ShowHighScoreScreenRegular(coloredRow);
			if (coloredRow == -1){
				blitAlphaImageToScreen(0,0,sWidth,sHeight,speedImg[8],beginX,beginY);
			}
		}else{
			ShowHighScoreScreenSurvival(coloredRow);
			if (coloredRow == -1){
				blitAlphaImageToScreen(0,0,sWidth,sHeight,speedImg[9],beginX,beginY);	
			}
		}

		flipScreen();
		Wait(20);
		do{
			sceCtrlReadBufferPositive(&pad, 1);
		} while (!(pad.Buttons & PSP_CTRL_START));

	}else{
		ReadHighScores();
		int flip = 0;
		int whiteX = beginX - 20;
		int whiteX2 = beginX + sWidth + 20;
		int whiteY = beginY + (sHeight / 2) - (whiteHeight / 2);

		do{
			sceCtrlReadBufferPositive(&pad, 1);

			clearScreen(ColorBlack);
			blitAlphaImageToScreen(0,0,maxX,maxY,highBack,0,0);
			//do this early to allow overwritting it with a red one
			blitAlphaImageToScreen(0,0,whiteWidth,whiteHeight,wl,whiteX, whiteY - 1);
			blitAlphaImageToScreen(0,0,whiteWidth,whiteHeight,wr,whiteX2, whiteY);

			if (hsindex == 0){
				//draws Standard then calls standard score drawer
				blitAlphaImageToScreen(0,0,sWidth,sHeight,speedImg[8],beginX,beginY);
				ShowHighScoreScreenRegular();
			}else if (hsindex == 1){
				//draws Survival then calls survival score drawer
				blitAlphaImageToScreen(0,0,sWidth,sHeight,speedImg[9],beginX,beginY);
				ShowHighScoreScreenSurvival();
			}

			if ((pad.Buttons & PSP_CTRL_LEFT) || (pad.Buttons & PSP_CTRL_LTRIGGER)){
				hsindex--;
				if (hsindex < 0){
					hsindex = 1;
				}
				//draws the red left arrow
				blitAlphaImageToScreen(0,0,whiteWidth,whiteHeight,rl,whiteX,whiteY - 1);
				//so it will show the red for longer
				flipScreen();
				flip = 0;
				Wait(15);
			}
			if ((pad.Buttons & PSP_CTRL_RIGHT) || (pad.Buttons & PSP_CTRL_RTRIGGER)){
				hsindex++;
				if (hsindex > 1){
					hsindex = 0;
				}
				//draws the red right arrow
				blitAlphaImageToScreen(0,0,whiteWidth,whiteHeight,rr,whiteX2,whiteY);
				//so it will show the red for longer
				flipScreen();
				flip = 0;
				Wait(15);
			}

			if (flip)
				flipScreen();
			else
				flip = 1;
		} while (!(pad.Buttons & PSP_CTRL_START));

	}//end if

	ShowTitleScreen();
}

void ShowHighScoreScreenRegular(int coloredRow){	
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
			blitAlphaImageToScreen(0,0,147,33,speedImg[10],170,59);
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
}

void ShowHighScoreScreenSurvival(int coloredRow){
	for (i = 0; i < 10; i++)
	{
		// shows coloredRow red
		if (i == coloredRow)// && count >= (i * 15))
		{
			if (survivalHigh[i] >= 100000){
				if ((survivalHighAccuracy[i] / 100) < 100)
					sprintf(buffer, "High Score %02i: %-i,%03i  Level: %-2i    Accuracy: %2i.%02i%%", i + 1, survivalHigh[i]/1000, survivalHigh[i]%1000, survivalHighLevel[i], (survivalHighAccuracy[i] / 100), (survivalHighAccuracy[i] - ((int)(survivalHighAccuracy[i] / 100) * 100)));
				else if ((survivalHighAccuracy[i] / 100) == 100)
					sprintf(buffer, "High Score %02i: %-i,%03i  Level: %-2i    Accuracy: %i%%", i + 1, survivalHigh[i]/1000, survivalHigh[i]%1000, survivalHighLevel[i], (survivalHighAccuracy[i] / 100));
			}else{
				//extra space since it is shorter of a number
				if ((survivalHighAccuracy[i] / 100) < 100)
					sprintf(buffer, "High Score %02i: %-i,%03i   Level: %-2i    Accuracy: %2i.%02i%%", i + 1, survivalHigh[i]/1000, survivalHigh[i]%1000, survivalHighLevel[i], (survivalHighAccuracy[i] / 100), (survivalHighAccuracy[i] - ((int)(survivalHighAccuracy[i] / 100) * 100)));
				else if ((survivalHighAccuracy[i] / 100) == 100)
					sprintf(buffer, "High Score %02i: %-i,%03i   Level: %-2i    Accuracy: %i%%", i + 1, survivalHigh[i]/1000, survivalHigh[i]%1000, survivalHighLevel[i], (survivalHighAccuracy[i] / 100));
			}
			printTextScreen (30, 15 * i + 100, buffer, ColorBlue);
			blitAlphaImageToScreen(0,0,147,33,speedImg[10],170,59);
		}
		else if (survivalHigh[i] && survivalHighLevel[i])// && count >= (i * 15))
		{
			if (survivalHigh[i] >= 100000){
				if ((survivalHighAccuracy[i] / 100) < 100)
					sprintf(buffer, "High Score %02i: %-i,%03i  Level: %-2i    Accuracy: %2i.%02i%%", i + 1, survivalHigh[i]/1000, survivalHigh[i]%1000, survivalHighLevel[i], (survivalHighAccuracy[i] / 100), (survivalHighAccuracy[i] - ((int)(survivalHighAccuracy[i] / 100) * 100)));
				else if ((survivalHighAccuracy[i] / 100) == 100)
					sprintf(buffer, "High Score %02i: %-i,%03i  Level: %-2i    Accuracy: %i%%", i + 1, survivalHigh[i]/1000, survivalHigh[i]%1000, survivalHighLevel[i], (survivalHighAccuracy[i] / 100));
			}else{
				//extra space since it is shorter of a number
				if ((survivalHighAccuracy[i] / 100) < 100)
					sprintf(buffer, "High Score %02i: %-i,%03i   Level: %-2i    Accuracy: %2i.%02i%%", i + 1, survivalHigh[i]/1000, survivalHigh[i]%1000, survivalHighLevel[i], (survivalHighAccuracy[i] / 100), (survivalHighAccuracy[i] - ((int)(survivalHighAccuracy[i] / 100) * 100)));
				else if ((survivalHighAccuracy[i] / 100) == 100)
					sprintf(buffer, "High Score %02i: %-i,%03i   Level: %-2i    Accuracy: %i%%", i + 1, survivalHigh[i]/1000, survivalHigh[i]%1000, survivalHighLevel[i], (survivalHighAccuracy[i] / 100));
			}
			printTextScreen (30, 15 * i + 100, buffer, ColorBlack);
		}
		
	}
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
		if (!bonusLevel && !bossLevel){
			//will only show image "Level " if it has been passed
			if (levelX >= textX){
				blitAlphaImageToScreen(0,0,71,26,ltext,textX,textY-4);
			}
			for (k = 0; k < GetDigits(currentLevel + 1); k++)
			{
				blitAlphaImageToScreen((levelNums[k] * snumWidth),0,snumWidth,snumHeight,smallnums,textX + 80 + (k * snumWidth),textY);
			}			
		}else if (bonusLevel){
			blitAlphaImageToScreen(0,0,155,26,btext,textX - 40,textY-4);		
		}else if (bossLevel){
			blitAlphaImageToScreen(0,0,155,26,bosstext,textX - 40,textY-4);			
		}
		//2nd space pic covers up the level text until it has passed over it
		blitImageToScreen(levelX,0,(maxX - levelX),maxY,space[spacePic],levelX,0);
		blitAlphaImageToScreen(0,0,44,37,levelPic,levelX,115);
		DrawEnemies();
		DrawBoss();
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
	blitAlphaImageToScreen(0,0,maxX,maxY,psphacks,0,0);
	if ((int)((num * maxLength)/ maxImages) <= maxLength){
		blitImageToScreen(0,0,(int)((num * maxLength)/ maxImages),6,meter,(maxX / 2) - (maxLength / 2),(maxY / 2) - (8/2) + 1);
	}else{
		blitImageToScreen(0,0,maxLength,6,meter,(maxX / 2) - (maxLength / 2),(maxY / 2) - (8/2) + 1);
	}
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

		CheckForSpaceChange();

		// do this earlier (here) to allow for rewriting over the green arrow with a red one
		if (index == 1)
		{
			//these used to be gr and gl, but were changed to white
			blitAlphaImageToScreen(0,0,whiteWidth,whiteHeight,wl,playerX - 10 - 7, playerY + 4);
			blitAlphaImageToScreen(0,0,whiteWidth,whiteHeight,wr,playerX + myshipWidth + 10, playerY + 4);
			userColor = temps[1];
		}
		else
		{
			//these used to be gr and gl, but were changed to white
			blitAlphaImageToScreen(0,0,whiteWidth,whiteHeight,wl,playerX - 10 - 7, bY + 1);
			blitAlphaImageToScreen(0,0,whiteWidth,whiteHeight,wr,playerX + myshipWidth + 10, bY + 1);
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
				blitAlphaImageToScreen(0,0,whiteWidth,whiteHeight,rr,playerX + myshipWidth + 10, playerY + 4);
			else
				blitAlphaImageToScreen(0,0,whiteWidth,whiteHeight,rr,playerX + myshipWidth + 10, bY + 1);
			flipScreen();
			flip = 0;

			Wait(15);
		}
		if (pad.Buttons & PSP_CTRL_LEFT)
		{
			temps[index] --;

			//my ship has more colors than bullet
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
				blitAlphaImageToScreen(0,0,whiteWidth,whiteHeight,rl,playerX - 10 - 7, playerY + 4);
			else
				blitAlphaImageToScreen(0,0,whiteWidth,whiteHeight,rl,playerX - 10 - 7, bY + 1);
			flipScreen();
			flip = 0;

			Wait(15);
		}

		if (flip)
			flipScreen();
		else
			flip = 1;
	} while (!(pad.Buttons & PSP_CTRL_START || pad.Buttons & PSP_CTRL_CIRCLE));

	if (pad.Buttons & PSP_CTRL_START){
		WriteOptionsToFile();
	}else{
		//clear current image from memory
		freeImage(space[spacePic]);

		//reads options from file so it revert back to what you had
		GetOptions(1);

		//load the new space image to memory
		sprintf(buffer,"./images/space/space%i.png",spacePic);
		space[spacePic] = loadImage(buffer);
	}
	ShowTitleScreen();
}


void ShowPracticeScreen(){
	practiceLevel = 1;
	int flip = 0;
	int beginX = 290;
	int sWidth = 147;
	int sHeight = 33;
	int paddingY = 33;
	int beginY = 87;
	int whiteX = 280;
	int whiteX2 = whiteX + sWidth + 10;
	int whiteY = beginY + (sHeight / 2) - (whiteHeight / 2);
	int index = 0;
	int maxSelections[4] = {2,10,4,5};
	int maxIndex = 3;
	//read practice options from file
	GetOptions(1);
	do {
		sceCtrlReadBufferPositive(&pad, 1);

		clearScreen(ColorBlack);
		CheckForSpaceChange();
		DrawSpace();
		blitAlphaImageToScreen(0,0,maxX,maxY,practice,0,0);

		//do this early to allow overwritting it with a red one
		blitAlphaImageToScreen(0,0,whiteWidth,whiteHeight,wl,whiteX, whiteY + (index * paddingY) - 1);
		blitAlphaImageToScreen(0,0,whiteWidth,whiteHeight,wr,whiteX2, whiteY + (index * paddingY));

		//the +6 is because the boss only / standard images are 6 and 7
		blitAlphaImageToScreen(0,0,sWidth,sHeight,speedImg[practiceSelection[0] + 6],beginX,beginY);
		if (practiceSelection[1] + 1 < 10){
			// the +7 is because the images have padding
			blitAlphaImageToScreen((practiceSelection[1] + 1) * snumWidth,0,snumWidth,snumHeight,smallnums,beginX + (sWidth / 2) - (snumWidth / 2),beginY + 7 + paddingY);
		}else{
			//show Infinite
			blitAlphaImageToScreen(0,0,sWidth,sHeight,speedImg[5],beginX,beginY + paddingY);
		}
		//the plus 1 is because enemy speed does not have a super slow
		blitAlphaImageToScreen(0,0,sWidth,sHeight,speedImg[practiceSelection[2] + 1],beginX,beginY + (2 * paddingY));
		blitAlphaImageToScreen(0,0,sWidth,sHeight,speedImg[practiceSelection[3]],beginX,beginY + (3 * paddingY));

		if (pad.Buttons & PSP_CTRL_DOWN){
			index++;
			if (index > maxIndex){
				index = maxIndex;
			}
			Wait(15);
		}
		if (pad.Buttons & PSP_CTRL_UP){
			index--;
			if (index < 0){
				index = 0;
			}
			Wait(15);
		}
		if (pad.Buttons & PSP_CTRL_RIGHT){
			practiceSelection[index]++;
			if (practiceSelection[index] > maxSelections[index] - 1){
				practiceSelection[index] = maxSelections[index] - 1;
			}
			//draws the red right arrow
			blitAlphaImageToScreen(0,0,whiteWidth,whiteHeight,rr,whiteX2, whiteY + (index * paddingY));
			//so it will show the red for longer
			flipScreen();
			flip = 0;
			Wait(15);
		}
		if (pad.Buttons & PSP_CTRL_LEFT){
			practiceSelection[index]--;
			if (practiceSelection[index] < 0){
				practiceSelection[index] = 0;
			}
			//draws the red left arrow
			blitAlphaImageToScreen(0,0,whiteWidth,whiteHeight,rl,whiteX, whiteY + (index * paddingY) - 1);
			//so it will show the red for longer
			flipScreen();
			flip = 0;
			Wait(15);
		}

		if (flip)
			flipScreen();
		else
			flip = 1;

	}while (!(pad.Buttons & PSP_CTRL_START || pad.Buttons & PSP_CTRL_CIRCLE));

	if (pad.Buttons & PSP_CTRL_START){
		oslPlaySound(music2, 0);
		oslSetSoundLoop(music2, 1);
		//save practice options for next time
		WriteOptionsToFile();
		ResetAllBullets();
		ResetGame();
		survival = 0;
		if (practiceSelection[1] + 1 < 10){
			lives = practiceSelection[1] + 1;
		}else{
			infinite = 1;
			lives = 100;
		}
		//boss only is 0 if standard or 1 if only boss
		bossOnly = practiceSelection[0];
		int beginEnemySpeed = defaultEnemySpeed - 1;
		int beginEnemyBulletSpeed = defaultEnemyBulletSpeed - 2;
		generalEnemySpeed = beginEnemySpeed + practiceSelection[2];
		enemyBulletSpeed = beginEnemyBulletSpeed + practiceSelection[3];
		SwitchLevel();
		ShowLevelScreen();
	}else{
		//cancel practice mode
		ShowTitleScreen();
	}
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

	int top = hightop;
	int low = highlow;
	if (survival){
		top = survivalhightop;
		low = survivalhighlow;
	}
	if (score < (top * 1000) + (low)){
		sprintf(buffer, "High: %-i,%03i",top,low);
		printTextScreen (380 - 10, 1, buffer, ColorBlack);
	}else{
		sprintf(buffer, "High: %-i,%03i",score/1000,score%1000);
		printTextScreen (380 - 10, 1, buffer, ColorBlack);
	}

	//survival mode show level instead of lives
	if (!survival){
		// Shows current lives in titlebar
		sprintf(buffer, "Lives: ");
		printTextScreen (1, 1, buffer, ColorBlack);
		if (lives - 1 <= 4){
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
	}else{
		//Shows Current Level in titlebar
		sprintf(buffer, "Level: %i", currentLevel + 1);
		printTextScreen (1, 1, buffer, ColorBlack);
	}

	//draws the boss health titlebar
	if (bossLevel){
		ShowTitleBarForBoss();
	}
}

void ShowTitleBarForBoss(){
	//draws the boss health titlebar

	//draws a second titlebar under the first
	blitImageToScreen(0,0,maxX,10,titlebar,0,10);
	if ((bossHealth/250.0) > 0.33){
		//draws the boss's health using a green meter
		blitAlphaImageToScreen(0,0,bossHealth,6,meter1,100,11 + 1);
	}else{
		//red meter because health is low
		blitAlphaImageToScreen(0,0,bossHealth,6,meter,100,11 + 1);
	}
	blitAlphaImageToScreen(0,0,250,8,border,100,11);
	//shows boss health text
	sprintf(buffer, "Boss Health: ");
	printTextScreen (1, 12, buffer, ColorBlack);
}
void ShowTitleScreen()
{
	int newTemp = 0;
	ReadHighScores();
	GetPersonalBest(); 
	int X = 65;
	int Y = 152;
	int Y2 = 211;

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
				newTemp = (theNums[k] * snumWidth);
				blitAlphaImageToScreen(newTemp,0,snumWidth,snumHeight,smallnums,X + 2 + (k * snumWidth),Y);
			}
			blitAlphaImageToScreen(0,0,11,13,scomma,X + 3 + (GetDigits(personalBest/1000) * snumWidth),Y+10);
			TurnCommaIntoNumberArray(personalBest%1000);
			for (k = 0; k < 3; k++)
			{
				newTemp = (theNums[k] * snumWidth);
				blitAlphaImageToScreen(newTemp,0,snumWidth,snumHeight,smallnums,X + 2 + 11 +((GetDigits(personalBest/1000) + k) * snumWidth),Y);
			}
		}else{
			blitAlphaImageToScreen(0,0,snumWidth,snumHeight,smallnums,X + 2 + (5 * snumWidth),Y);
		}
		//show the personal best for survival mode
		if (survivalpersonalBest != 0){
			TurnIntoNumberArray(survivalpersonalBest/1000);
			for (k = 0; k < GetDigits(survivalpersonalBest/1000); k++)
			{
				newTemp = (theNums[k] * snumWidth);
				blitAlphaImageToScreen(newTemp,0,snumWidth,snumHeight,smallnums,X + 2 + (k * snumWidth),Y2);
			}
			blitAlphaImageToScreen(0,0,11,13,scomma,X + 3 + (GetDigits(survivalpersonalBest/1000) * snumWidth),Y2+10);
			TurnCommaIntoNumberArray(survivalpersonalBest%1000);
			for (k = 0; k < 3; k++)
			{
				newTemp = (theNums[k] * snumWidth);
				blitAlphaImageToScreen(newTemp,0,snumWidth,snumHeight,smallnums,X + 2 + 11 +((GetDigits(survivalpersonalBest/1000) + k) * snumWidth),Y2);
			}
		}else{
			blitAlphaImageToScreen(0,0,snumWidth,snumHeight,smallnums,X + 2 + (5 * snumWidth),Y2);
		}

		if (pad.Buttons & PSP_CTRL_DOWN){
			mainIndex++; 
			if (mainIndex > 4){
				mainIndex = 4;
			}
			Wait(15);
		}else if (pad.Buttons & PSP_CTRL_UP){
			mainIndex--;
			if (mainIndex < 0){
				mainIndex = 0;
			}
			Wait(15);
		}
		DrawArrow();

		if (pad.Buttons & PSP_CTRL_SELECT)
			sceKernelExitGame();

		flipScreen();

	} while (!(pad.Buttons & PSP_CTRL_CROSS));

	switch (mainIndex){
		case 0:
			//Play
			survival = 0;
			PlayGame();
			break;
		case 1:
			//Options
			ShowOptionsScreen();
			break;
		case 2:
			//Practice
			ShowPracticeScreen();
			break;
		case 3:
			//Survival
			survival = 1;
			PlayGame();
			break;
		case 4:
			//High Scores
			menuHigh = 1;
			//-1 because you don't want to color any row
			ShowHighScoreScreen(-1);
			break;
	}//end switch
}

void SwitchLevel()
{
	//not boss level or bonus level unless otherwise stated
	drawBoss = 0;
	bossLevel = 0;
	bonusLevel = 0;
	//do this here so that enemies don't stick at beginning of level if you had the powerup
	if (frozen){
		//clears the powerup
		frozen = 0;
		GetPowerUpTimeLeft(); 
	}
	currentLevel ++;

	if (bossOnly == 1){
		//boss level always because level 8 (9)  is boss level
		currentLevel = 8;
	}

	switch (currentLevel % 9) //7 + bonus + boss
	{
		case 0:
			if (currentLevel != 0)
			{
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
		case 8:
			if (!bossOnly){
				//show the bonus level stuff
				bonusLevel = 0;
				Wait(10);
				ShowBonusStats();
				ResetAllBullets();
			}

			bossSpeed = generalEnemySpeed - 1;
			bossBulletSpeed = enemyBulletSpeed - 1;
			bossY = 20;
			bossX = (maxX / 2) - (bossWidth / 2);
			bossLevel = 1;
			bossHealth = 250;
			drawBoss = 1;
			enemyCountTotal = 18;
			aliveCount = enemyCountTotal;

			for (i = 0; i < enemyCountTotal; i++)
			{
				enemyBoolDead[i] = 0;

				if (i < 5)
				{
					enemyBeginX = bossX - (6 * (enemyWidth + enemyPadding));
					enemyX[i] = enemyBeginX + (i * (enemyWidth + enemyPadding));
					enemyY[i] = 30;
					enemyColor[i] = purple;
				}
				else if(i < 10)
				{
					enemyBeginX = bossX + bossWidth + enemyPadding;
					enemyX[i] = enemyBeginX + ((i - 5) * (enemyWidth + enemyPadding));
					enemyY[i] = 30;
					enemyColor[i] = blue;
				}
				else
				{
					enemyBeginX = 100;
					enemyX[i] = enemyBeginX + ((i % 8)*(enemyWidth + enemyPadding));
					enemyY[i] = 30 + bossHeight + 10;
					enemyColor[i] = teal;
				}
			}
			break;
		}//end switch


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

void TeleportEnemy(int which){
	int notOkayToMove = 1;
	do
	{
		enemyX[which] = rand() / (int)(((unsigned)RAND_MAX + 1) / maxX);
		for (k = 0; k < enemyCountTotal; k++)
		{
			if (k != i && !intersects(enemyX[which] - 5,enemyY[which],enemyWidth + 10,enemyHeight,enemyX[k] - 5,enemyY[k],enemyWidth + 10,enemyHeight));
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

void UpdateBossBullets(){
	for (i = 0; i < bossBulletsMax; i++){
		if (drawBossBullet[i]){
			bossBulletY[i] += bossBulletSpeed;
			//this makes the bullets spray out
			if (i%3 == 0){
				bossBulletX[i] -= bossBulletSpeed;
			}else if (i%3 == 2){
				bossBulletX[i] += bossBulletSpeed;
			}

			// hit the player's ship
			if (intersects(bossBulletX[i], bossBulletY[i], bulletWidth, bulletHeight, playerX, playerY, myshipWidth, myshipHeight))
			{
				UserIsHit();
			}
			// goes down the bottom of the screen
			else if (bossBulletY[i] >= maxY)
			{
				drawBossBullet[i] = 0;
				bossBulletY[i] = bossY;
			}
		}
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
		}
		CheckForHitEnemy();
		if (drawBoss){
			CheckForHitBoss();
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
	if (infinite){
		lives = 100;
	}

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
	if (!survival){
		pFile = fopen("./data/data v13.dat","w");
	}else{
		pFile = fopen("./data/data3 v14.dat","w");
	}

	if (pFile != NULL)
	{
		// write high scores
		for (i = 0; i < 30; i++)
		{
			if (i < 10)
			{
				if (!survival){
					high[i] ^= KEYS[i];
					fwrite(&high[i],sizeof(h1),1,pFile);
				}else{
					survivalHigh[i] ^= KEYS[i];
					fwrite(&survivalHigh[i],sizeof(h1),1,pFile);
				}
			}
			// write high levels
			else if (i < 20)
			{
				if (!survival){
					highLevel[i - 10] ^= KEYS[i - 10];
					fwrite(&highLevel[i - 10],sizeof(h1),1,pFile);
				}else{
					survivalHighLevel[i - 10] ^= KEYS[i - 10];
					fwrite(&survivalHighLevel[i - 10],sizeof(h1),1,pFile);
				}
			}
			// write high accuracys
			else if (i < 30)
			{
				if (!survival){
					highAccuracy[i - 20] ^= KEYS[i - 20];
					fwrite(&highAccuracy[i - 20],sizeof(h1),1,pFile);
				}else{
					survivalHighAccuracy[i - 20] ^= KEYS[i - 20];
					fwrite(&survivalHighAccuracy[i - 20],sizeof(h1),1,pFile);
				}
			}
		}
	}
    fclose(pFile);
}

void WriteOptionsToFile()
{
	pFile = fopen("./data/data2 v14.dat","w");
	if (pFile != NULL)
	{
		//color options
		fwrite(&spacePic,sizeof(spacePic),1,pFile);
		fwrite(&bulletColor,sizeof(bulletColor),1,pFile);
		fwrite(&userColor,sizeof(userColor),1,pFile);

		//practice options
		fwrite(&practiceSelection[0],sizeof(practiceSelection[0]),1,pFile);
		fwrite(&practiceSelection[1],sizeof(practiceSelection[1]),1,pFile);
		fwrite(&practiceSelection[2],sizeof(practiceSelection[2]),1,pFile);
		fwrite(&practiceSelection[3],sizeof(practiceSelection[3]),1,pFile);
	}
	fclose(pFile);
}

///////////TODO BEFORE 1.4//////////////////////////////////////////////
// convert v13 high scores to v14 by finding all of their scores and placing them into v14 one by one using CheckForNewHigh and only do this if they don't have a personal best for v14 (makes sure you don't do it every time the game loads, or you could then delete or rename the file to backup or something)
// have bonus stats screen use commas
// shrink main menu scores
// pause music while paused
// show a 0 if you don't get any on bonus level
// possibly for the psp-hacks comp splash have it be like the level intro and reveal the text by the ship?
// fix how enemies don't bounce off of boss correctly
// should the boss health meter have the white surrounding it?
// when you get the time powerup it needs to slow down boss also
// make angled boss bullets
// make thumbstick support smoother instead of jumpy
////////////////////////////////////////////////////////////////////////
