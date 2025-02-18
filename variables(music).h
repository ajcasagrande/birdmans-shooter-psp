#include <pspdisplay.h>
#include <pspctrl.h>
#include <pspkernel.h>
//#include <psppower.h>
#include <pspdebug.h>
#include <pspgu.h>
#include <png.h> 
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <psputils.h>
#include <psputility_sysparam.h>
#include <pspaudio.h>
#include <pspaudiolib.h>
#include "graphics.h"
#include "mp3player.h"

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
void ReadHighScores();
void ResetAllBullets();
void ResetGame();
void SetUpImages();
void ShowBonusStats();
void ShowExplosion();
void ShowGameOver();
void ShowHighScoreScreen();
void ShowLevelScreen();
void ShowLoadingScreen();
void ShowOptionsScreen();
void ShowPracticeScreen();
void ShowTitleBar();
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

Image* greenbullet;
Image* pspX;
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
Image* gr;
Image* gl;
Image* rr;
Image* rl;
Image* meter;
Image* bstats;
Image* of;
Image* plus;
Image* ops;
Image* ltext;
Image* btext;
Image* paused;
Image* newhigh;
Image* smallnums;
Image* comma;
Image* psphacks;
Image* border;
Image* boss;
Image* bosstext;
Image* arrow;
Image* practice;

int bonusLevel = 0;
int numOfSpace = 10;
int myshipcount = 11;

Image* backship;
Image* back;
Image* highBack;
Image* options;
Image* space[10];
Image* myship[11];
Image* bullet[6];
Image* enemyImg[6];

Color ColorWhite = RGB(255,255,255);
Color ColorBlack = RGB(0,0,0);
Color ColorGreen =	RGB(0,255,0);
Color ColorBlue = RGB(0,0,255);

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
	personalBest = 0,
	playerBulletsLeft = 0,
	lastUserBullet = 0,
	lastEnemyBullet = 0,
	closestBullet = 0,
	frozen = 0,
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
	sameShotTimes = 0; // keeps track of how many time same enemy tries to shoot in a row

float bonusAccuracy = 0;
float accuracy = 0;
float accDecimal = 0;

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

int enemyBeginX = 100;

time_t TimeNow_;
time_t TimeStart_;

FILE * pFile;
char nickname[25];
int temps[2] = {0,0};

void SetUpImages()
{
	int num = 1;
	ShowLoadingScreen(num);
	for (i = 0; i < numOfSpace; i++)
	{
		sprintf(buffer,"./images/space%i.png",i);
		space[i] = loadImage(buffer);
		num++;
		ShowLoadingScreen(num);
	}

	sprintf(buffer,"./images/highscores0.png");
	highBack = loadImage(buffer);
	num++;
	ShowLoadingScreen(num);

	sprintf(buffer,"./images/back0.png");
	back = loadImage(buffer);
	num++;
	ShowLoadingScreen(num);

	sprintf(buffer,"./images/ops0.png");
	options = loadImage(buffer);
	num++;
	ShowLoadingScreen(num);

	sprintf(buffer,"./images/newpspX.png");
	pspX = loadImage(buffer);
	num++;
	ShowLoadingScreen(num);

	sprintf(buffer,"./images/explosion.png");
	explosion = loadImage(buffer);
	num++;
	ShowLoadingScreen(num);

	sprintf(buffer,"./images/level.png");
	levelPic = loadImage(buffer);
	num++;
	ShowLoadingScreen(num);

	sprintf(buffer,"./images/titlebar.png");
	titlebar = loadImage(buffer);
	num++;
	ShowLoadingScreen(num);

	sprintf(buffer,"./images/comma.png");
	comma = loadImage(buffer);
	num++;
	ShowLoadingScreen(num);

	sprintf(buffer,"./images/smallship.png");
	smallship = loadImage(buffer);
	num++;
	ShowLoadingScreen(num);

	sprintf(buffer,"./images/paused.png");
	paused = loadImage(buffer);
	num++;
	ShowLoadingScreen(num);

	sprintf(buffer,"./images/ltext.png");
	ltext = loadImage(buffer);
	num++;
	ShowLoadingScreen(num);

	sprintf(buffer,"./images/of.png");
	of = loadImage(buffer);
	num++;
	ShowLoadingScreen(num);

	sprintf(buffer,"./images/gameover.png");
	gover = loadImage(buffer);
	num++;
	ShowLoadingScreen(num);

	sprintf(buffer,"./images/overnums.png");
	overnums = loadImage(buffer);
	num++;
	ShowLoadingScreen(num);

	sprintf(buffer,"./images/percent.png");
	percent = loadImage(buffer);
	num++;
	ShowLoadingScreen(num);

	sprintf(buffer,"./images/period.png");
	period = loadImage(buffer);
	num++;
	ShowLoadingScreen(num);

	sprintf(buffer,"./images/wl.png");
	wl = loadImage(buffer);
	num++;
	ShowLoadingScreen(num);

	sprintf(buffer,"./images/gl.png");
	gl = loadImage(buffer);
	num++;
	ShowLoadingScreen(num);

	sprintf(buffer,"./images/wr.png");
	wr = loadImage(buffer);
	num++;
	ShowLoadingScreen(num);

	sprintf(buffer,"./images/plus.png");
	plus = loadImage(buffer);
	num++;
	ShowLoadingScreen(num);

	sprintf(buffer,"./images/practice.png");
	practice = loadImage(buffer);
	num++;
	ShowLoadingScreen(num);

	sprintf(buffer,"./images/gr.png");
	gr = loadImage(buffer);
	num++;
	ShowLoadingScreen(num);

	sprintf(buffer,"./images/rr.png");
	rr = loadImage(buffer);
	num++;
	ShowLoadingScreen(num);

	sprintf(buffer,"./images/rl.png");
	rl = loadImage(buffer);
	num++;
	ShowLoadingScreen(num);

	sprintf(buffer,"./images/bstats.png");
	bstats = loadImage(buffer);
	num++;
	ShowLoadingScreen(num);

	sprintf(buffer,"./images/btext.png");
	btext = loadImage(buffer);
	num++;
	ShowLoadingScreen(num); 

	sprintf(buffer,"./images/bosstext.png");
	bosstext = loadImage(buffer);
	num++;
	ShowLoadingScreen(num); 

	sprintf(buffer,"./images/newhigh.png");
	newhigh = loadImage(buffer);
	num++;
	ShowLoadingScreen(num);

	//initiate the audio
	pspAudioInit();
	MP3_Init(1);
	MP3_Load("music.mp3");

	sprintf(buffer,"./images/boss.png");
	boss = loadImage(buffer);
	num++;
	ShowLoadingScreen(num);

	sprintf(buffer,"./images/arrow.png");
	arrow = loadImage(buffer);
	num++;
	ShowLoadingScreen(num);

	sprintf(buffer,"./images/smallnums.png");
	smallnums = loadImage(buffer);
	num++;
	ShowLoadingScreen(num);

	for (i = 0; i < myshipcount; i++)
	{
		sprintf(buffer,"./images/myship%i.png",i);
		myship[i] = loadImage(buffer);
		num++;
		ShowLoadingScreen(num);
	}

	sprintf(buffer,"./images/backship.png");
	backship = loadImage(buffer);
	num++;
	ShowLoadingScreen(num);

	for (i = 0; i < 6; i++)
	{
		sprintf(buffer,"./images/bullet%i.png",i);
		bullet[i] = loadImage(buffer);
		num++;
		ShowLoadingScreen(num);
	}

	for (i = 0; i < 2; i++)
	{
		sprintf(buffer,"./images/powerup%i.png",i);
		powerup[i] = loadImage(buffer);
		num++;
		ShowLoadingScreen(num);
	}

	for (i = 0; i < 6; i++)
	{
		sprintf(buffer,"./images/enemy%i.png",i);
		enemyImg[i] = loadImage(buffer);
		num++;
		ShowLoadingScreen(num);
	}
}

void ResetGame()
{
	bonusLevel = 0;
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
	currentLevel = 7;//-1 is default, 6 for bonus level testing 7 for boss level
	enemyCountTotal = 7;
	playerBulletsMax = 2;
	whoShot = 0;
	gameover = 0;
	enemyBulletsMax = 7;
	lastLoopX = 0;
	explode = 0;
	levelX = 0;
	enemyBulletSpeed = 3; //3
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
	generalEnemySpeed = 2; //1
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

	playerSpeedX*=2;
	generalEnemySpeed*=2;
	enemyBulletSpeed*=2;
	bossSpeed*=2;
	bulletSpeed*=2;
}

