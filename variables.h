#include <pspdisplay.h>
#include <pspctrl.h>
#include <pspkernel.h>
#include <psppower.h>
#include <pspdebug.h>
#include <pspgu.h>
#include <png.h> 
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <psputils.h>
#include <psputility_sysparam.h>
#include <oslib/oslib.h>
#include "graphics.h"

#define printf pspDebugScreenPrintf
#define RGB2(r, g, b) ((r)|((g)<<8)|((b)<<16))
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
int  AICollision();
void CheckForAIBullet();
void CheckForAIMove();
void CheckForEnemyBullets();
void CheckForBossBullets();
void CheckForBossBulletsLeft();
void CheckForEnemeyBulletsLeft();
void CheckForHighScore();
void CheckForHitBoss();
void CheckForHitEnemy();
void CheckForPowerUp();
void CheckForSpaceChange();
void CheckForUserBullet();
void CheckForUserMove();
void CheckForUserPause();
void CreateDummyScores();
void DrawArrow();
void DrawBoss();
void DrawEnemies();
void DrawEnemyBullets();
void DrawEngine();
void DrawPowerUp();
void DrawSpace();
void DrawUserBullets();
void DrawUserShip();
void EndGameLoop();
void GetClosestUserBullet();
int  GetDigits();
int  GetHighestBossBullet();
int  GetHighestEnemyBullet();
void GetLastBossBulletShot();
void GetLastEnemyBulletShot();
void GetLastUserBulletShot();
void GetOptions();
void GetPersonalBest();
void GetPlayerBulletsLeft();
void GetPowerUpTimeLeft();
void IncreaseScore();
int  intersects();
void LevelBegin();
void MoveBoss();
void MoveEnemies();
void MoveEnemiesBonusLevel();
void PauseGame();
void PlayGame();
void ReadHighScores();
void ResetAllBullets();
void ResetGame();
void SetUpImages();
void ShowBonusStats();
void ShowExplosion();
void ShowGameOver();
void ShowHighScoreScreen();
void ShowHighScoreScreenRegular();
void ShowHighScoreScreenSurvival();
void ShowLevelScreen();
void ShowLoadingScreen();
void ShowOptionsScreen();
void ShowPracticeScreen();
void ShowTitleBar();
void ShowTitleBarForBoss();
void ShowTitleScreen();
void SwitchLevel();
void TeleportEnemy();
void TurnCommaIntoNumberArray();
void TurnFloatIntoNumberArray();
void TurnIntoNumberArray();
void UpdateBossBullets();
void UpdateEnemyBullets();
void UpdateUserBullets();
void UserIsHit();
void Wait();
void WriteNewHighScores();
void WriteOptionsToFile();

char buffer[200];
char leveltext[200];

Image* explosion;
Image* levelPic; 
Image* powerup[2];
Image* titlebar;
Image* smallship;
Image* gover;
Image* overnums;
Image* percent;
Image* period;
Image* wl;
Image* wr;
Image* rr;
Image* rl;
Image* meter;
Image* meter1;
Image* bstats;
Image* of;
Image* plus;
Image* ops;
Image* ltext;
Image* btext;
Image* paused;
Image* smallnums;
Image* comma;
Image* scomma;
Image* psphacks;
Image* border;
Image* boss;
Image* bosstext;
Image* arrow;
Image* practice;
Image* backship;
Image* back;
Image* highBack;
Image* options;

int bonusLevel = 0;
int numOfSpace = 10;
int myshipcount = 11;
int speedAmount = 11;

Image* space[10];
Image* myship[11];
Image* bullet[6];
Image* enemyImg[6];
Image* speedImg[11];

Color ColorWhite = RGB2(255,255,255);
Color ColorBlack = RGB2(0,0,0);
Color ColorGreen =	RGB2(0,255,0);
Color ColorBlue = RGB2(0,0,255);

//each item is xor with the key for the specific index so high[3] is xor with KEY[3] asnd so on
const int KEYS[10] = {973621, 2321746, 675321, 2432578, 12345231, 565443, 232356, 344433, 76995, 347529};

SceCtrlData pad;

const int maxX = 480,
		  maxY = 272,

		  //yellow = 7,
		  blue = 2,
		  green = 1,
		  red = 0,
		  teal = 3,
		  //orange = 6,
		  black = 4,
		  purple = 5,

		  timeLimit = 30, //15

		  numWidth = 20,
		  numHeight = 25,
		  snumWidth = 15,
		  snumHeight = 19,

		  letWidth = 27,
		  letHeight = 33,

		  bossWidth = 37,
		  bossHeight = 44,

		  defaultEnemySpeed = 2,
		  defaultEnemyBulletSpeed = 4,

		  whiteHeight = 11,
		  whiteWidth = 7,

		  bottomBarHeight = 12,
		  myshipWidth = 20,
		  myshipHeight = 20,
		  enemyWidth = 20,
		  enemyHeight = 24,
		  bulletHeight = 12,
		  bulletWidth = 4,
		  explWidth = 17,
		  enemyPadding = 10,
		  explHeight = 19;

int i = 0,
	j = 0,
	k = 0,
	f = 0,
	z = 0,
	hsindex = 0,
	menuHigh = 1,
	survival = 0,
	bossOnly = 0,
	infinite = 0,
	bulletSpeed = 6, // 6
	practiceLevel = 0,
	mainIndex = 0,
	drawBoss = 0,
	bossBulletsLeft = 0,
	bossHealth = 250,
	bossLevel = 0,
	bossSpeed = 1,
	bossBulletSpeed = 2,
	bossBulletsMax = 6,
	lastBossBullet = 0,
	bossX = 230,
	bossY = 20,
	survivalpersonalBest = 0,
	personalBest = 0,
	playerBulletsLeft = 0,
	lastUserBullet = 0,
	lastEnemyBullet = 0,
	closestBullet = 0,
	frozen = 0,
	survivalhightop = 0,
	survivalhighlow = 0,
	hightop = 0,
	highlow = 0,
	saveBulletSpeed = 0,
	playerSpeedX = 3, //3
	justStarted = 0,
	count = 0,
	restart = 0,
	playerX = 100,
	playerY,
	defaultPlayerY,
	score = 0,
	shotsFired = 0,
	shotsHit = 0,
	bonusFired = 0,
	bonusHit = 0,
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
	enemyBulletSpeed = 2, //3
	dualShoot = 0,
	generalEnemySpeed = 2, //1
	nextStage = 0,
	notOkayToMove = 1,
	tempLevel = 1,
	powerUpSpeed = 1,
	userColor = 6,
	bulletColor = 2,
	spacePic = 0,
	firsttime = 1,
	enemyBulletsLeft = 10,
	whoShotLast = 99999, // garbage number
	sameShotTimes = 0;   // keeps track of how many time same enemy tries to shoot in a row

float bonusAccuracy = 0;
float accuracy = 0;
float accDecimal = 0;

int practiceSelection[4] = {0,2,1,2};
int highScores[3];
int phaseTwo[50];
int enemyX[50];
int	enemyY[50];
int enemySpeed[50];
int	enemyBoolDead[50];
int drawEnemyBullet[15];
int enemyBulletY[15];
int enemyBulletX[15];
int drawBossBullet[15];
int bossBulletY[15];
int bossBulletX[15];
int shooter[15];
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
int saveSpeed[50];

int	drawPowerUp[2] = {0,0};
int	powerUpY[2] = {0,0};
int	powerUpX[2] = {0,0};

int newHigh = 0;
int newHighLoc = 0;
int h1 = 0;
int high[10] = {0,0,0,0,0,0,0,0,0,0};
int highLevel[10] = {0,0,0,0,0,0,0,0,0,0};
int highAccuracy[10] = {0,0,0,0,0,0,0,0,0,0};
int survivalHigh[10] = {0,0,0,0,0,0,0,0,0,0};
int survivalHighLevel[10] = {0,0,0,0,0,0,0,0,0,0};
int survivalHighAccuracy[10] = {0,0,0,0,0,0,0,0,0,0};

int enemyBeginX = 100;

time_t TimeNow_;
time_t TimeStart_;

FILE * pFile;
char nickname[25];
int temps[2] = {0,0};

OSL_SOUND *music, *explSound, *music2;

void SetUpImages()
{ 
	int num = 1;
	ShowLoadingScreen(num);

	sprintf(buffer,"./images/main/backship.png");
	backship = loadImage(buffer);
	num++;
	ShowLoadingScreen(num);

	sprintf(buffer,"./images/high/highscores.png");
	highBack = loadImage(buffer);
	num++;
	ShowLoadingScreen(num);

	sprintf(buffer,"./images/main/back0.png");
	back = loadImage(buffer);
	num++;
	ShowLoadingScreen(num);

	sprintf(buffer,"./images/options/ops0.png");
	options = loadImage(buffer);
	num++;
	ShowLoadingScreen(num);

	sprintf(buffer,"./images/player/explosion.png");
	explosion = loadImage(buffer);
	num++;
	ShowLoadingScreen(num);

	sprintf(buffer,"./images/level/level.png");
	levelPic = loadImage(buffer);
	num++;
	ShowLoadingScreen(num);

	sprintf(buffer,"./images/level/titlebar.png");
	titlebar = loadImage(buffer);
	num++;
	ShowLoadingScreen(num);

	sprintf(buffer,"./images/numbers/comma.png");
	comma = loadImage(buffer);
	num++;
	ShowLoadingScreen(num);

	sprintf(buffer,"./images/numbers/scomma.png");
	scomma = loadImage(buffer);
	num++;
	ShowLoadingScreen(num);

	sprintf(buffer,"./images/level/smallship.png");
	smallship = loadImage(buffer);
	num++;
	ShowLoadingScreen(num);

	sprintf(buffer,"./images/level/paused.png");
	paused = loadImage(buffer);
	num++;
	ShowLoadingScreen(num);

	sprintf(buffer,"./images/level/ltext.png");
	ltext = loadImage(buffer);
	num++;
	ShowLoadingScreen(num);

	sprintf(buffer,"./images/bonus/of.png");
	of = loadImage(buffer);
	num++;
	ShowLoadingScreen(num);

	sprintf(buffer,"./images/level/gameover.png");
	gover = loadImage(buffer);
	num++;
	ShowLoadingScreen(num);

	sprintf(buffer,"./images/numbers/overnums.png");
	overnums = loadImage(buffer);
	num++;
	ShowLoadingScreen(num);

	sprintf(buffer,"./images/numbers/percent.png");
	percent = loadImage(buffer);
	num++;
	ShowLoadingScreen(num);

	sprintf(buffer,"./images/numbers/period.png");
	period = loadImage(buffer);
	num++;
	ShowLoadingScreen(num);

	sprintf(buffer,"./images/options/wl.png");
	wl = loadImage(buffer);
	num++;
	ShowLoadingScreen(num);

	sprintf(buffer,"./images/options/wr.png");
	wr = loadImage(buffer);
	num++;
	ShowLoadingScreen(num);

	sprintf(buffer,"./images/numbers/plus.png");
	plus = loadImage(buffer);
	num++;
	ShowLoadingScreen(num);

	sprintf(buffer,"./images/options/practice.png");
	practice = loadImage(buffer);
	num++;
	ShowLoadingScreen(num);

	sprintf(buffer,"./images/options/rr.png");
	rr = loadImage(buffer);
	num++;
	ShowLoadingScreen(num);

	sprintf(buffer,"./images/options/rl.png");
	rl = loadImage(buffer);
	num++;
	ShowLoadingScreen(num);

	sprintf(buffer,"./images/bonus/bstats.png");
	bstats = loadImage(buffer);
	num++;
	ShowLoadingScreen(num);

	sprintf(buffer,"./images/level/btext.png");
	btext = loadImage(buffer);
	num++;
	ShowLoadingScreen(num); 

	sprintf(buffer,"./images/level/bosstext.png");
	bosstext = loadImage(buffer);
	num++;
	ShowLoadingScreen(num); 

	sprintf(buffer,"./images/enemies/boss.png");
	boss = loadImage(buffer);
	num++;
	ShowLoadingScreen(num);

	sprintf(buffer,"./images/main/arrow.png");
	arrow = loadImage(buffer);
	num++;
	ShowLoadingScreen(num);

	sprintf(buffer,"./images/numbers/smallnums.png");
	smallnums = loadImage(buffer);
	num++;
	ShowLoadingScreen(num);

	for (i = 0; i < myshipcount; i++)
	{
		sprintf(buffer,"./images/player/myship%i.png",i);
		myship[i] = loadImage(buffer);
		num++;
		ShowLoadingScreen(num);
	}

	for (i = 0; i < 6; i++)
	{
		sprintf(buffer,"./images/bullets/bullet%i.png",i);
		bullet[i] = loadImage(buffer);
		num++;
		ShowLoadingScreen(num);
	}

	for (i = 0; i < 2; i++)
	{
		sprintf(buffer,"./images/powerups/powerup%i.png",i);
		powerup[i] = loadImage(buffer);
		num++;
		ShowLoadingScreen(num);
	}

	for (i = 0; i < 6; i++)
	{
		sprintf(buffer,"./images/enemies/enemy%i.png",i);
		enemyImg[i] = loadImage(buffer);
		num++;
		ShowLoadingScreen(num);
	}

	for (i = 0; i < speedAmount; i++)
	{
		sprintf(buffer,"./images/options/s%i.png",i);
		speedImg[i] = loadImage(buffer);
		num++;
		ShowLoadingScreen(num);
	}

}

void ResetGame()
{
	infinite = 0;
	bonusLevel = 0;
	i = 0;
	j = 0;
	k = 0;
	f = 0;
	restart = 0;
	defaultPlayerY = maxY - myshipHeight - 3,
	playerX = 200;
	score = 0;
	shotsFired = 0;
	shotsHit = 0;
	lives = 3; // 3
	if (survival == 1){
		lives = 1;
	}
	currentLevel = -1;//-1 is default, 6 for bonus level testing 7 for boss level
	enemyCountTotal = 7;
	playerBulletsMax = 2;
	whoShot = 0;
	gameover = 0;
	enemyBulletsMax = 7;
	lastLoopX = 0;
	explode = 0;
	levelX = 0;
	enemyBulletSpeed = defaultEnemyBulletSpeed;
	drawPowerUp[0] = 0; 
	powerUpX[0] = -10;
	powerUpY[0] = -10;
	drawPowerUp[1] = 0; 
	powerUpX[1] = -10;
	powerUpY[1] = -10;
	powerUpSpeed = 2; //2
	justStarted = 0;
	dualShoot = 0; //0 if you set this to one you also have to double the max bullets
	//playerBulletsMax *= 2;
	generalEnemySpeed = defaultEnemySpeed;
	nextStage = 0;
	notOkayToMove = 1;
	tempLevel = 1;
	whoShotLast = 99999; // garbage number
	sameShotTimes = 0; // keeps track of how many time same enemy tries to shoot in a row
	accuracy = 0;
	aliveCount = enemyCountTotal;
	playerY = defaultPlayerY;
	h1 = 0;
	explode = 0;
	enemyBulletsLeft = enemyBulletsMax;
	bonusFired = 0;
	bossLevel = 0;
	bonusHit = 0;
	bonusAccuracy = 0;
	frozen = 0;
}

