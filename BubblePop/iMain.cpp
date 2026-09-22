

#include "iGraphics.h"
#include "menu.h"
#include "obstacle.h"
#include "character.h"
#include "bullet.h"
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

// -------------------------------------------------------
// GAME STATE
// -------------------------------------------------------
bool inMenu = true;
int currentLevel = 1;

float remainingDistance = 700.0f;
bool isWin = false;
bool isLose = false;

bool isBonusPhase = false;
float bonusDistance = 60.0f;

int score = 0;
float maxFuel = 100.0f;
float currentFuel = 100.0f;

bool DEBUG_HITBOXES = false;

// -------------------------------------------------------
// AUDIO
// -------------------------------------------------------
void startMenuMusic()
{
	mciSendString("open \"menu.mp3\" type mpegvideo alias menuMusic", NULL, 0, NULL);
	mciSendString("play menuMusic from 0", NULL, 0, NULL);
}

void restartMenuMusicIfLooping()
{
	if (inMenu) mciSendString("play menuMusic from 0", NULL, 0, NULL);
}

void resumeMenuMusic()
{
	mciSendString("play menuMusic from 0", NULL, 0, NULL);
}

void stopMenuMusic()
{
	mciSendString("stop menuMusic", NULL, 0, NULL);
}

int currentGroundSound = 0;

void loadGameplayAudio()
{
	mciSendString("open \"run.mp3\" type mpegvideo alias runMusic", NULL, 0, NULL);
	mciSendString("open \"jetpack.mp3\" type mpegvideo alias jetpackMusic", NULL, 0, NULL);
}

void startRunMusic()      { mciSendString("play runMusic from 0 repeat", NULL, 0, NULL); }
void stopRunMusic()       { mciSendString("stop runMusic", NULL, 0, NULL); }
void startJetpackMusic()  { mciSendString("play jetpackMusic from 0 repeat", NULL, 0, NULL); }
void stopJetpackMusic()   { mciSendString("stop jetpackMusic", NULL, 0, NULL); }

void updateCharacterAudio()
{
	if (isWin || isLose)
	{
		if (currentGroundSound != 0)
		{
			stopRunMusic();
			stopJetpackMusic();
			currentGroundSound = 0;
		}
		return;
	}

	int desired;
	if (isAirborne() && isJumping)      desired = 2;
	else if (!isAirborne())             desired = 1;
	else                                desired = 0;

	if (desired != currentGroundSound)
	{
		if (currentGroundSound == 1) stopRunMusic();
		if (currentGroundSound == 2) stopJetpackMusic();

		if (desired == 1) startRunMusic();
		if (desired == 2) startJetpackMusic();

		currentGroundSound = desired;
	}
}

// -------------------------------------------------------
// BACKGROUND & FLOOR
// -------------------------------------------------------
int images[4];
int level2Images[4];
int bonusBackgroundImage;
int currentFrame = 0;

void advanceFrame()
{
	currentFrame = (currentFrame + 1) % 4;
}

int floorImage;
float floorX = 0.0f;
float floorSpeed = 220.0f;
int floorWidth = 1280;
int floorHeight = 124;

// -------------------------------------------------------
// RESET GAME
// -------------------------------------------------------
void resetGame()
{
	if (currentLevel == 1)      remainingDistance = 700.0f;
	else if (currentLevel == 2) remainingDistance = 500.0f;

	isWin = false;
	isLose = false;
	isBonusPhase = false;
	bonusDistance = 60.0f;

	score = 0;
	currentFuel = 100.0f;

	resetCharacter();
	resetObstacles(currentLevel);
	resetBullets();

	currentGroundSound = 0;
}

void startLevel1()
{
	currentLevel = 1;
	resetGame();
	inMenu = false;
}

void startLevel2()
{
	currentLevel = 2;
	resetGame();
	inMenu = false;
}

// -------------------------------------------------------
// UPDATE LOGIC (16 ms)
// -------------------------------------------------------
void updateFloor()
{
	if (inMenu || isWin || isLose) return;

	floorX -= floorSpeed * 0.016f;
	if (floorX <= -floorWidth) floorX += floorWidth;

	float charLeft, charRight, charBottom, charTop;
	getCharacterAABB(charLeft, charRight, charBottom, charTop);

	if (isBonusPhase)
	{
		bonusDistance -= 5.33f * 0.016f;
		if (bonusDistance <= 0.0f)
		{
			bonusDistance = 0.0f;
			isWin = true;
			isBonusPhase = false;
		}

		updateBonusCoins(floorSpeed, charLeft, charRight, charBottom, charTop, score);
		updateCharacterAudio();
		return;
	}

	float metersTraveled = 5.33f * 0.016f;

	if (remainingDistance > 0)
	{
		remainingDistance -= metersTraveled;
		if (remainingDistance <= 0)
		{
			remainingDistance = 0;
			if (currentLevel == 2)
			{
				isBonusPhase = true;
				bonusDistance = 60.0f;
				initBonusCoins();
			}
			else
			{
				isWin = true;
			}
		}
	}

	if (isAirborne() && isJumping)
	{
		currentFuel -= metersTraveled * 1.25f;
		if (currentFuel <= 0.0f)
		{
			currentFuel = 0.0f;
			isJumping = false;
		}
	}

	float currentSpeed = floorSpeed;
	if (remainingDistance <= 300) currentSpeed += 100.0f;

	updateObstacles(currentSpeed, charLeft, charRight, charBottom, charTop, score, currentFuel, maxFuel, isLose, currentLevel, remainingDistance);
	updateBullets(inMenu, isWin, isLose, characterX, characterY, currentLevel, remainingDistance);
	updateCharacterAudio();
}

void updateJump()
{
	updateCharacter(inMenu, isWin, isLose);
}

// -------------------------------------------------------
// DRAW
// -------------------------------------------------------
void iDraw()
{
	iClear();

	if (inMenu)
	{
		drawMenu();
		return;
	}

	if (isBonusPhase)
	{
		iShowImage(0, 0, 1280, 720, bonusBackgroundImage);
	}
	else if (currentLevel == 2)
	{
		iShowImage(0, 0, 1280, 720, level2Images[currentFrame]);
	}
	else
	{
		iShowImage(0, 0, 1280, 720, images[currentFrame]);
	}

	iShowImage((int)floorX, 0, floorWidth, floorHeight, floorImage);
	iShowImage((int)floorX + floorWidth, 0, floorWidth, floorHeight, floorImage);

	if (isBonusPhase)
	{
		drawBonusCoins();
	}
	else
	{
		drawObstacles(currentLevel);
	}

	drawCharacter();

	if (currentLevel == 2 && !isBonusPhase)
	{
		drawBullets();
		drawBulletPickup(); // Draw bullet pickup item
	}

	if (DEBUG_HITBOXES && !isBonusPhase)
	{
		drawObstacleHitboxes();
		drawCharacterHitbox();
	}

	// -----------------------------------------------
	// HUD DISPLAY
	// -----------------------------------------------
	iSetColor(255, 255, 255);

	char distStr[50];
	if (isBonusPhase)
		sprintf_s(distStr, "BONUS: %d m", (int)bonusDistance);
	else
		sprintf_s(distStr, "Distance: %d m", (int)remainingDistance);

	iText(1050, 670, distStr, GLUT_BITMAP_TIMES_ROMAN_24);

	char scoreStr[50];
	sprintf_s(scoreStr, "Score: %d", score);
	iText(1100, 45, scoreStr, GLUT_BITMAP_TIMES_ROMAN_24);

	// FUEL BAR
	iSetColor(100, 100, 100);
	iFilledRectangle(1050, 605, 180, 18);

	if (currentFuel > 40.0f) iSetColor(0, 230, 0);
	else                     iSetColor(255, 50, 50);
	iFilledRectangle(1050, 605, (int)((currentFuel / maxFuel) * 180), 18);

	iSetColor(255, 255, 255);
	iRectangle(1050, 605, 180, 18);
	iText(980, 608, "Fuel:", GLUT_BITMAP_HELVETICA_12);

	// AMMO HUD BOX (Level 2)
	if (currentLevel == 2 && !isBonusPhase)
	{
		iSetColor(20, 30, 50);
		iFilledRectangle(1050, 560, 180, 28);
		iSetColor(0, 200, 255);
		iRectangle(1050, 560, 180, 28);

		iSetColor(255, 215, 0);
		char ammoStr[50];
		sprintf_s(ammoStr, "AMMO: %d / %d", currentAmmo, maxAmmo);
		iText(1075, 568, ammoStr, GLUT_BITMAP_HELVETICA_18);
	}

	// -----------------------------------------------
	// WIN / LOSE OVERLAY
	// -----------------------------------------------
	if (isWin)
	{
		iSetColor(15, 35, 80);
		iFilledRectangle(340, 160, 600, 400);
		iSetColor(0, 210, 255);
		iRectangle(340, 160, 600, 400);

		iSetColor(255, 255, 255);
		if (currentLevel == 2)
			iText(440, 480, "500m COMPLETE! YOU WIN!", GLUT_BITMAP_TIMES_ROMAN_24);
		else
			iText(440, 480, "700m COMPLETE! EXCELLENT!", GLUT_BITMAP_TIMES_ROMAN_24);

		iSetColor(0, 120, 200);
		iFilledRectangle(400, 240, 200, 50);
		iSetColor(255, 255, 255);
		iText(460, 258, "RESTART", GLUT_BITMAP_HELVETICA_18);

		iSetColor(20, 60, 130);
		iFilledRectangle(680, 240, 200, 50);
		iSetColor(255, 255, 255);
		iText(750, 258, "MENU", GLUT_BITMAP_HELVETICA_18);
	}

	if (isLose)
	{
		iSetColor(15, 35, 80);
		iFilledRectangle(340, 160, 600, 400);
		iSetColor(0, 210, 255);
		iRectangle(340, 160, 600, 400);

		iSetColor(255, 255, 255);
		char loseMsg[60];
		sprintf_s(loseMsg, "YOU LOSE! Distance Left: %d m", (int)remainingDistance);
		iText(420, 480, loseMsg, GLUT_BITMAP_TIMES_ROMAN_24);

		iSetColor(0, 120, 200);
		iFilledRectangle(400, 240, 200, 50);
		iSetColor(255, 255, 255);
		iText(460, 258, "RESTART", GLUT_BITMAP_HELVETICA_18);

		iSetColor(20, 60, 130);
		iFilledRectangle(680, 240, 200, 50);
		iSetColor(255, 255, 255);
		iText(750, 258, "MENU", GLUT_BITMAP_HELVETICA_18);
	}
}

// -------------------------------------------------------
// INPUT HANDLERS
// -------------------------------------------------------
void iMouseMove(int mx, int my) {}

void iPassiveMouseMove(int mx, int my)
{
	handleMenuMouseMove(mx, my);
}

void iKeyboard(unsigned char key) {}
void iSpecialKeyboard(int key) {}

void iMouse(int button, int state, int mx, int my)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		if (inMenu)
		{
			if (handleMenuClick(mx, my)) return;
		}

		if (isWin || isLose)
		{
			if (mx >= 400 && mx <= 600 && my >= 240 && my <= 290)
			{
				resetGame();
				return;
			}
			if (mx >= 680 && mx <= 880 && my >= 240 && my <= 290)
			{
				stopRunMusic();
				stopJetpackMusic();
				resetGame();
				inMenu = true;
				resumeMenuMusic();
				return;
			}
		}

		if (!inMenu && !isWin && !isLose)
		{
			if (currentFuel > 0.0f)
			{
				isJumping = true;
			}

			if (currentLevel == 2 && !isBonusPhase && currentAmmo > 0)
			{
				isShooting = true;
				spawnBullet(characterX + 120, characterY + 110);
			}
		}
	}

	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		if (!inMenu && !isWin && !isLose)
		{
			isJumping = false;
			isShooting = false;
			bulletShootTimer = 0;
		}
	}
}

void fixedUpdate()
{
	if (inMenu)
	{
		static bool prevUp = false, prevDown = false, prevEnter = false, prevEsc = false;
		bool curUp = isSpecialKeyPressed(GLUT_KEY_UP);
		bool curDown = isSpecialKeyPressed(GLUT_KEY_DOWN);
		bool curEnter = isKeyPressed(13);
		bool curEsc = isKeyPressed(27);

		if (curUp    && !prevUp)    handleMenuSpecialKey(GLUT_KEY_UP);
		if (curDown  && !prevDown)  handleMenuSpecialKey(GLUT_KEY_DOWN);
		if (curEnter && !prevEnter) handleMenuKey(13);
		if (curEsc   && !prevEsc)   handleMenuKey(27);

		prevUp = curUp; prevDown = curDown;
		prevEnter = curEnter; prevEsc = curEsc;
		return;
	}

	static bool prevEscGame = false;
	bool curEsc = isKeyPressed(27);
	if (curEsc && !prevEscGame)
	{
		stopRunMusic();
		stopJetpackMusic();
		inMenu = true;
		resumeMenuMusic();
	}
	prevEscGame = curEsc;

	if (!isWin && !isLose)
	{
		bool curSpace = isKeyPressed(' ');
		if (curSpace && currentFuel > 0.0f)
		{
			isJumping = true;
		}
		else if (!curSpace)
		{
			isJumping = false;
		}
	}
}

// -------------------------------------------------------
// LOAD IMAGES
// -------------------------------------------------------
void loadImages()
{
	loadMenuImages();
	loadObstacleImages();
	loadCharacterImages();

	for (int i = 0; i < 4; i++)
	{
		char path[100];
		sprintf_s(path, "Images//level1_bg%d.png", i + 1);
		images[i] = iLoadImage(path);
	}

	for (int i = 0; i < 4; i++)
	{
		char path[100];
		sprintf_s(path, "Images//level2_bg%d.png", i + 1);
		level2Images[i] = iLoadImage(path);
	}

	bonusBackgroundImage = iLoadImage("Images//BonusBackground.png");
	floorImage = iLoadImage("Images//floor.png");
}

// -------------------------------------------------------
// MAIN
// -------------------------------------------------------
int main()
{
	iSetTimer(500, advanceMenuFrame);
	iSetTimer(83, advanceFrame);
	iSetTimer(83, updatePlayerAnimation);
	iSetTimer(16, updateFloor);
	iSetTimer(16, updateJump);
	iSetTimer(16, fixedUpdate);
	iSetTimer(12000, restartMenuMusicIfLooping);

	iInitialize(1280, 720, "Escape Protocol");
	loadImages();
	loadGameplayAudio();
	startMenuMusic();
	resetGame();
	iStart();
	return 0;
}