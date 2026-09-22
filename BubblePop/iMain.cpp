
#include "iGraphics.h"
#include "menu.h"
#include "obstacle.h"
#include "character.h"
#include "bullet.h"
#include <mmsystem.h>
#include <cstring>
#pragma comment(lib, "winmm.lib")

// -------------------------------------------------------
// GAME STATE
// -------------------------------------------------------
bool inMenu = true;
int currentLevel = 1;
bool isPaused = false;
bool showLevel2Intro = false;

float remainingDistance = 700.0f;
bool isWin = false;
bool isLose = false;

// -------------------------------------------------------
// DISTANCE / TIMING
// -------------------------------------------------------
// distanceRate controls how fast "remainingDistance" (the meters shown on
// the HUD) counts down. It is set per-level in resetGame() below.
//   Level 1 : 700 m,  rate 5.33 m/s  (unchanged, original feel)
//   Level 2 : 500 m,  rate 5.33 m/s  (unchanged, original feel)
//   Level 3 : 400 m,  rate 4.00 m/s  -> 400 / 4.00 = 100s = 1 min 40 sec
// NOTE: fuel-drain and every other "meters" based rule (shield spawn every
// 100m, ammo reload every 100m, +100 speed in final 300m, etc.) still use
// the original fixed 5.33 constant, so gameplay difficulty/feel for
// Level 1 & Level 2 is 100% unchanged. Only the Level 3 HUD-distance clock
// runs at the special rate needed to hit exactly 1:40 for 400m.
float distanceRate = 5.33f;

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
int images[4];           // Level 1 animated background frames
int level2Images[4];     // Level 2 animated background frames
int bonusBackgroundImage;      // Bonus-phase background (all levels)
int level3BackgroundImage;     // Level 3 main background (static, "BonusBackground3")
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
	// ---------------------------------------------------
	// Per-level distance + distance-clock rate
	// ---------------------------------------------------
	if (currentLevel == 1)
	{
		remainingDistance = 700.0f;
		distanceRate = 5.33f;
	}
	else if (currentLevel == 2)
	{
		remainingDistance = 500.0f;
		distanceRate = 5.33f;
	}
	else if (currentLevel == 3)
	{
		remainingDistance = 400.0f;
		distanceRate = 4.00f; // 400m / 4.00 m/s = 100s = 1 min 40 sec
	}

	isWin = false;
	isLose = false;
	isBonusPhase = false;
	bonusDistance = 60.0f;
	showLevel2Intro = false;

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
	showLevel2Intro = true;
}

void startLevel3()
{
	// Level 3 reuses every Level-2 mechanic (gun/ammo, rocket, shield,
	// destructible obstacle, bonus phase) -- only distance/time and the
	// main background are different. See resetGame() and iDraw().
	currentLevel = 3;
	resetGame();
	inMenu = false;
}

// -------------------------------------------------------
// UPDATE LOGIC (16 ms)
// -------------------------------------------------------
void updateFloor()
{
	if (inMenu || isWin || isLose || isPaused || showLevel2Intro) return;

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
			isBonusPhase = false;

			// Level 3 drops into the final "Security" shootout instead of
			// winning immediately. Level 2 still wins immediately here.
			if (currentLevel == 3)
			{
				startFinalFight();
			}
			else
			{
				isWin = true;
			}
		}

		updateBonusCoins(floorSpeed, charLeft, charRight, charBottom, charTop, score);
		updateCharacterAudio();
		return;
	}

	// -----------------------------------------------
	// LEVEL 3 FINAL FIGHT (boss shootout vs. Security)
	// Replaces normal obstacle/rocket/shield logic; only bullets still run.
	// -----------------------------------------------
	if (isFinalFight)
	{
		updateFinalFight(isWin);
		updateBullets(inMenu, isWin, isLose, characterX, characterY, currentLevel, remainingDistance);
		updateCharacterAudio();
		return;
	}

	// metersTraveled: FIXED rate, used for fuel-drain math only, so fuel
	// balance stays identical across Level 1 / 2 / 3.
	float metersTraveled = 5.33f * 0.016f;

	// distanceTraveled: per-level rate, used ONLY to count down the
	// on-screen "remainingDistance" clock (so Level 3 finishes in 1:40).
	float distanceTraveled = distanceRate * 0.016f;

	if (remainingDistance > 0)
	{
		remainingDistance -= distanceTraveled;
		if (remainingDistance <= 0)
		{
			remainingDistance = 0;

			if (currentLevel == 2 || currentLevel == 3)
			{
				// Level 2 & Level 3 both drop into the bonus coin phase.
				isBonusPhase = true;
				bonusDistance = 60.0f;
				initBonusCoins();
			}
			else
			{
				// Level 1 has no bonus phase.
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
	if (isPaused || showLevel2Intro) return;
	updateCharacter(inMenu, isWin, isLose);
}

// -------------------------------------------------------
// TEXT WIDTH HELPER (for centering GLUT bitmap text exactly)
// -------------------------------------------------------
int getBitmapTextWidth(const char* text, void* font)
{
	int width = 0;
	for (const char* c = text; *c != '\0'; c++)
	{
		width += glutBitmapWidth(font, *c);
	}
	return width;
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

	if (isPaused)
	{
		iSetColor(0, 0, 0);
		iFilledRectangle(0, 0, 1280, 720);

		iSetColor(255, 255, 255);

		const char* line1 = "GAME PAUSED";
		const char* line2 = "Press ENTER to resume";
		const char* line3 = "Press ESC again to go back to the level selection menu";

		void* font = GLUT_BITMAP_TIMES_ROMAN_24;
		int w1 = getBitmapTextWidth(line1, font);
		int w2 = getBitmapTextWidth(line2, font);
		int w3 = getBitmapTextWidth(line3, font);

		iText(640 - w1 / 2, 440, line1, font);
		iText(640 - w2 / 2, 380, line2, font);
		iText(640 - w3 / 2, 330, line3, font);
		return;
	}

	// -----------------------------------------------
	// MAIN BACKGROUND -- kept explicitly per-level
	// -----------------------------------------------
	if (isBonusPhase)
	{
		// Bonus phase background is shared by every level (unchanged).
		iShowImage(0, 0, 1280, 720, bonusBackgroundImage);
	}
	else if (currentLevel == 1)
	{
		iShowImage(0, 0, 1280, 720, images[currentFrame]);
	}
	else if (currentLevel == 2)
	{
		iShowImage(0, 0, 1280, 720, level2Images[currentFrame]);
	}
	else if (currentLevel == 3)
	{
		// Level 3 main background is a single static image (BonusBackground3),
		// not an animated 4-frame set. Also reused, unchanged, for the final
		// Security shootout -- only what's drawn ON TOP of it changes.
		iShowImage(0, 0, 1280, 720, level3BackgroundImage);
	}

	iShowImage((int)floorX, 0, floorWidth, floorHeight, floorImage);
	iShowImage((int)floorX + floorWidth, 0, floorWidth, floorHeight, floorImage);

	if (isBonusPhase)
	{
		drawBonusCoins();
	}
	else if (isFinalFight)
	{
		// No obstacles/coins during the final fight -- just the Security enemies.
		drawSecurities();
	}
	else
	{
		drawObstacles(currentLevel);
	}

	drawCharacter();

	// Gun / bullets / shields: identical for Level 2 and Level 3 (final fight included).
	if ((currentLevel == 2 || currentLevel == 3) && !isBonusPhase)
	{
		drawObstacleShields(currentLevel);
		drawBullets();
		drawBulletPickup(); // Draw bullet pickup item
	}

	if (DEBUG_HITBOXES && !isBonusPhase)
	{
		drawObstacleHitboxes(currentLevel);
		drawCharacterHitbox();
	}

	// -----------------------------------------------
	// HUD DISPLAY
	// -----------------------------------------------
	iSetColor(255, 255, 255);

	char distStr[50];
	if (isBonusPhase)
		sprintf_s(distStr, "BONUS: %d m", (int)bonusDistance);
	else if (isFinalFight)
		sprintf_s(distStr, "SECURITY: %d", securitiesAlive);
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

	// AMMO HUD BOX (Level 2 & Level 3)
	if ((currentLevel == 2 || currentLevel == 3) && !isBonusPhase)
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
		else if (currentLevel == 3)
			iText(400, 480, "SECURITY DEFEATED! YOU WIN!", GLUT_BITMAP_TIMES_ROMAN_24);
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

	// -----------------------------------------------
	// LEVEL 2 INTRO OVERLAY
	// -----------------------------------------------
	if (showLevel2Intro)
	{
		iSetColor(15, 35, 80);
		iFilledRectangle(340, 160, 600, 400);
		iSetColor(0, 210, 255);
		iRectangle(340, 160, 600, 400);

		iSetColor(255, 255, 255);

		const char* heading = "LEVEL 2";
		const char* line1 = "In this level you have a gun and you can use it";
		const char* line2 = "to shoot down certain obstacles.";
		const char* line3 = "Use the left button of your mouse to shoot the gun.";

		int wHeading = getBitmapTextWidth(heading, GLUT_BITMAP_TIMES_ROMAN_24);
		int w1 = getBitmapTextWidth(line1, GLUT_BITMAP_HELVETICA_18);
		int w2 = getBitmapTextWidth(line2, GLUT_BITMAP_HELVETICA_18);
		int w3 = getBitmapTextWidth(line3, GLUT_BITMAP_HELVETICA_18);

		iText(640 - wHeading / 2, 490, heading, GLUT_BITMAP_TIMES_ROMAN_24);
		iText(640 - w1 / 2, 430, line1, GLUT_BITMAP_HELVETICA_18);
		iText(640 - w2 / 2, 400, line2, GLUT_BITMAP_HELVETICA_18);
		iText(640 - w3 / 2, 370, line3, GLUT_BITMAP_HELVETICA_18);

		iSetColor(0, 120, 200);
		iFilledRectangle(540, 240, 200, 50);
		iSetColor(255, 255, 255);
		iText(605, 258, "OKAY", GLUT_BITMAP_HELVETICA_18);
	}

	// -----------------------------------------------
	// LEVEL 3 "FINAL ROUND" BANNER
	// -----------------------------------------------
	if (showFinalRoundText)
	{
		iSetColor(0, 0, 0);
		iFilledRectangle(0, 0, 1280, 720);

		iSetColor(255, 40, 40);
		const char* bigText = "FINAL ROUND";
		void* bigFont = GLUT_BITMAP_TIMES_ROMAN_24;
		int wBig = getBitmapTextWidth(bigText, bigFont);
		iText(640 - wBig / 2, 390, bigText, bigFont);

		iSetColor(255, 255, 255);
		const char* subText = "Security is inbound -- shoot them down!";
		int wSub = getBitmapTextWidth(subText, GLUT_BITMAP_HELVETICA_18);
		iText(640 - wSub / 2, 340, subText, GLUT_BITMAP_HELVETICA_18);
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

void iKeyboard(unsigned char key)
{
	if (key == 'h' || key == 'H') DEBUG_HITBOXES = !DEBUG_HITBOXES;
}

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

		if (showLevel2Intro)
		{
			if (mx >= 540 && mx <= 740 && my >= 240 && my <= 290)
			{
				showLevel2Intro = false;
				return;
			}
			return; // swallow other clicks while the intro popup is up
		}

		if (!inMenu && !isWin && !isLose && !isPaused)
		{
			if (currentFuel > 0.0f)
			{
				isJumping = true;
			}

			// Gun / shooting: identical for Level 2 and Level 3 (final fight
			// included) -- but not while the "FINAL ROUND" banner is up.
			if ((currentLevel == 2 || currentLevel == 3) && !isBonusPhase && !showFinalRoundText && currentAmmo > 0)
			{
				isShooting = true;
				// Calibrated spawn offset matching higher gun barrel location
				spawnBullet(characterX + 148, characterY + 145);
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

// Shared key-edge state for fixedUpdate. Kept at file scope (rather than as
// separate `static` locals inside each branch of fixedUpdate) so a key that's
// still physically held down across an inMenu/gameplay mode switch (e.g. the
// ESC that both pauses and then exits pause) isn't misread as a brand new
// press by the other branch.
bool prevUpKey = false;
bool prevDownKey = false;
bool prevEnterKey = false;
bool prevEscKey = false;

void fixedUpdate()
{
	bool curUp = isSpecialKeyPressed(GLUT_KEY_UP);
	bool curDown = isSpecialKeyPressed(GLUT_KEY_DOWN);
	bool curEnter = isKeyPressed(13);
	bool curEsc = isKeyPressed(27);

	if (inMenu)
	{
		if (curUp    && !prevUpKey)    handleMenuSpecialKey(GLUT_KEY_UP);
		if (curDown  && !prevDownKey)  handleMenuSpecialKey(GLUT_KEY_DOWN);
		if (curEnter && !prevEnterKey) handleMenuKey(13);
		if (curEsc   && !prevEscKey)   handleMenuKey(27);

		prevUpKey = curUp; prevDownKey = curDown;
		prevEnterKey = curEnter; prevEscKey = curEsc;
		return;
	}

	if (showLevel2Intro)
	{
		// Freeze pause/jump handling while the intro popup is up; keep the
		// key-edge trackers in sync so a held key isn't misread once it closes.
		prevUpKey = curUp; prevDownKey = curDown;
		prevEnterKey = curEnter; prevEscKey = curEsc;
		return;
	}

	if (curEsc && !prevEscKey)
	{
		if (isWin || isLose)
		{
			// Win/Lose overlay already has its own buttons; ESC exits straight to menu.
			isPaused = false;
			stopRunMusic();
			stopJetpackMusic();
			inMenu = true;
			resumeMenuMusic();
		}
		else if (!isPaused)
		{
			// First ESC: pause the game.
			isPaused = true;
		}
		else
		{
			// Second ESC while paused: quit out to the level selection screen.
			isPaused = false;
			stopRunMusic();
			stopJetpackMusic();
			inMenu = true;
			inLevelSelect = true;
			levelSelected = -1;
			levelUsingKeyboard = false;
			resumeMenuMusic();
		}
	}

	if (curEnter && !prevEnterKey && isPaused && !isWin && !isLose)
	{
		// ENTER while paused: resume gameplay.
		isPaused = false;
	}

	prevUpKey = curUp; prevDownKey = curDown;
	prevEnterKey = curEnter; prevEscKey = curEsc;

	if (!isWin && !isLose && !isPaused)
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

	// Level 1 animated background frames
	for (int i = 0; i < 4; i++)
	{
		char path[100];
		sprintf_s(path, "Images//level1_bg%d.png", i + 1);
		images[i] = iLoadImage(path);
	}

	// Level 2 animated background frames
	for (int i = 0; i < 4; i++)
	{
		char path[100];
		sprintf_s(path, "Images//level2_bg%d.png", i + 1);
		level2Images[i] = iLoadImage(path);
	}

	// Shared bonus-phase background (used by Level 2 & Level 3 bonus phase)
	bonusBackgroundImage = iLoadImage("Images//BonusBackground.png");

	// Level 3 main background -- single static image, as requested.
	level3BackgroundImage = iLoadImage("Images//BonusBackground3.png");

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