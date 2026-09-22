#include "iGraphics.h"
#include "menu.h"

// -------------------------------------------------------
// inMenu DEFINED HERE (extern declared in menu.h)
// -------------------------------------------------------
bool inMenu = true;


//----------new------

// -------------------------------------------------------
// DISTANCE & WIN STATE
// -------------------------------------------------------
float remainingDistance = 700.0f; 
bool isWin = false;


//new

// -------------------------------------------------------
// OBSTACLE & LOSE STATE
// -------------------------------------------------------
bool isLose = false;




int score = 0;
int fuelMissedCount = 0;
float maxFuel = 100.0f;
float currentFuel = 100.0f;

struct Collectible {
	float x, y;
	int width, height;
	bool active;
};

Collectible coinItem;
Collectible fuelItem;

int coinImage;
int fuelImage;
// ================================

struct Obstacle {
	float x, y;
	int width, height;
	int type;      // 0 to 4 (obstacle 1 to 5)
	bool active;
};


int nextObstacleType = 0;// new last


#define MAX_OBSTACLES 4
Obstacle obstacles[MAX_OBSTACLES];
int obstacleImages[5]; // 5 terget image

// -------------------------------------------------------
// BACKGROUND ANIMATION
// -------------------------------------------------------
int images[4];
int currentFrame = 0;

void advanceFrame()
{
	currentFrame = (currentFrame + 1) % 4;
}
// -------------------------------------------------------
// PLAYER ANIMATION
// -------------------------------------------------------
int playerFrames[4];
int currentPlayerFrame = 0;
int characterJumpImage;

void updatePlayerAnimation()
{
	currentPlayerFrame++;
	if (currentPlayerFrame >= 4)
		currentPlayerFrame = 0;
}

// -------------------------------------------------------
// CHARACTER
// -------------------------------------------------------
float characterX = 200;
float characterY = 15;
bool  isJumping = false;
// game reset


void resetGame()
{
	remainingDistance = 700.0f;
	isWin = false;
	isLose = false;
	characterY = 15;
	isJumping = false;
	nextObstacleType = 0; // counter reset 

	// --- SCORE and FUEL RESET ---
	score = 0;
	fuelMissedCount = 0;
	currentFuel = 100.0f;

	// --- OBSTACLE RESET ---
	for (int i = 0; i < MAX_OBSTACLES; i++)
	{
		obstacles[i].x = 1280 + (i * 800);
		obstacles[i].active = true;

		// sequence maintain (0, 1, 2, 3, 4)
		obstacles[i].type = nextObstacleType;
		nextObstacleType = (nextObstacleType + 1) % 5;

		// type size position
		if (obstacles[i].type == 0) // obstacle 1
		{
			obstacles[i].width = 200;
			obstacles[i].height = 200;
			obstacles[i].y = 124;
		}
		else if (obstacles[i].type == 1) // obstacle 2
		{
			obstacles[i].width = 270;
			obstacles[i].height = 147;
			if (rand() % 2 == 0)
				obstacles[i].y = 90;
			else
				obstacles[i].y = 530;
		}
		else if (obstacles[i].type == 2) // obstacle 3
		{
			obstacles[i].width = 180;
			obstacles[i].height = 180;
			obstacles[i].y = 124 + (rand() % 150);
		}
		else if (obstacles[i].type == 3) // obstacle 4
		{
			obstacles[i].width = 160;
			obstacles[i].height = 220;
			obstacles[i].y = 124;
		}
		else // obstacle 5
		{
			obstacles[i].width = 270;
			obstacles[i].height = 147;
			obstacles[i].y = 124 + (rand() % 200);
		}
	}

	// --- INITIALIZE COIN 
	coinItem.width = 40;
	coinItem.height = 40;
	coinItem.x = obstacles[0].x + 400;
	coinItem.y = 180 + (rand() % 150);
	coinItem.active = true;

	// --- INITIALIZE FUEL 
	fuelItem.width = 50;
	fuelItem.height = 50;
	fuelItem.x = obstacles[1].x + 400;
	fuelItem.y = 180 + (rand() % 150);
	fuelItem.active = true;
}





// -------------------------------------------------------
// FLOOR
// -------------------------------------------------------
int   floorImage;
float floorX      = 0;
float floorSpeed  = 200.0f;
int   floorWidth  = 1280;
int   floorHeight = 124;
//--------new-----
void updateFloor()
{
	if (inMenu || isWin || isLose) return;

	floorX -= floorSpeed * 0.016f;
	if (floorX <= -floorWidth)
		floorX += floorWidth;

	// Meter decrease
	if (remainingDistance > 0)
	{
		remainingDistance -= 5.33f * 0.016f;
		if (remainingDistance <= 0)
		{
			remainingDistance = 0;
			isWin = true;
		}
	}

	// --- FUEL SYSTEM DECREASE & LOSE CHECK ---
	currentFuel -= 2.5f * 0.016f; //fuel dicrese
	if (currentFuel <= 0)
	{
		currentFuel = 0;
		isLose = true; //fuel end game over
	}

	float currentSpeed = floorSpeed;
	if (remainingDistance <= 300) currentSpeed += 100.0f;

	// -------------------------------------------------------
	// OBSTACLES MOVEMENT & COLLISION
	// -------------------------------------------------------
	for (int i = 0; i < MAX_OBSTACLES; i++)
	{
		obstacles[i].x -= currentSpeed * 0.016f;

		if (obstacles[i].x < -300)
		{
			int gap = 600 + (rand() % 300);
			if (remainingDistance <= 300) gap = 450 + (rand() % 200);

			int prevIndex = (i == 0) ? MAX_OBSTACLES - 1 : i - 1;
			obstacles[i].x = obstacles[prevIndex].x + gap;
			if (obstacles[i].x < 1280) obstacles[i].x = 1280 + gap;

			obstacles[i].type = nextObstacleType;
			nextObstacleType = (nextObstacleType + 1) % 5;

			if (obstacles[i].type == 0) { obstacles[i].width = 200; obstacles[i].height = 200; obstacles[i].y = 124; }
			else if (obstacles[i].type == 1) { obstacles[i].width = 270; obstacles[i].height = 147; obstacles[i].y = (rand() % 2 == 0) ? 90 : 530; }
			else if (obstacles[i].type == 2) { obstacles[i].width = 180; obstacles[i].height = 180; obstacles[i].y = 124 + (rand() % 150); }
			else if (obstacles[i].type == 3) { obstacles[i].width = 160; obstacles[i].height = 220; obstacles[i].y = 124; }
			else { obstacles[i].width = 270; obstacles[i].height = 147; obstacles[i].y = 124 + (rand() % 200); }
		}

		// Collision with Obstacle
		float ox = obstacles[i].x, oy = obstacles[i].y;
		float ow = (float)obstacles[i].width, oh = (float)obstacles[i].height;
		float padX = ow * 0.15f, padY = oh * 0.15f;

		if (characterX + 40 < ox + ow - padX &&
			characterX + 140 > ox + padX &&
			characterY + 25 < oy + oh - padY &&
			characterY + 160 > oy + padY)
		{
			isLose = true;
		}
	}

	// -------------------------------------------------------
	// COIN MOVEMENT & PICKUP LOGIC
	// -------------------------------------------------------
	coinItem.x -= currentSpeed * 0.016f;
	if (coinItem.x < -50)
	{
		// 0 
		coinItem.x = obstacles[0].x + 350 + (rand() % 200);
		coinItem.y = 150 + (rand() % 250);
		coinItem.active = true;
	}

	if (coinItem.active &&
		characterX < coinItem.x + coinItem.width &&
		characterX + 140 > coinItem.x &&
		characterY < coinItem.y + coinItem.height &&
		characterY + 160 > coinItem.y)
	{
		score += 1; // Score aumento
		coinItem.active = false;
	}

	// -------------------------------------------------------
	// FUEL MOVEMENT, PICKUP & MISS LOGIC
	// -------------------------------------------------------
	fuelItem.x -= currentSpeed * 0.016f;
	if (fuelItem.x < -50)
	{
		if (fuelItem.active) // miss count
		{
			fuelMissedCount++;
			if (fuelMissedCount >= 3)
			{
				isLose = true; // 3 miss over
			}
		}

		// obstacle 2 
		fuelItem.x = obstacles[2].x + 400 + (rand() % 200);
		fuelItem.y = 150 + (rand() % 250);
		fuelItem.active = true;
	}

	if (fuelItem.active &&
		characterX < fuelItem.x + fuelItem.width &&
		characterX + 140 > fuelItem.x &&
		characterY < fuelItem.y + fuelItem.height &&
		characterY + 160 > fuelItem.y)
	{
		currentFuel += 35.0f; // Fuel refill
		if (currentFuel > maxFuel) currentFuel = maxFuel;
		fuelItem.active = false;
	}
}








void updateJump()
{
	if (inMenu || isWin || isLose) return;  // don't update while in menu
    if (isJumping)
    {
        characterY += 500.0f * 0.016f;
        if (characterY > 520) characterY = 520;
    }
    else
    {
        characterY -= 500.0f * 0.016f;
        if (characterY < 15) characterY = 15;
    }
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

	// 1. Background Rendering
	iShowImage(0, 0, 1280, 720, images[currentFrame]);

	// 2. Floor Rendering
	iShowImage((int)floorX, 0, floorWidth, floorHeight, floorImage);
	iShowImage((int)floorX + floorWidth, 0, floorWidth, floorHeight, floorImage);

	// 3. Obstacles Rendering
	for (int i = 0; i < MAX_OBSTACLES; i++)
	{
		iShowImage((int)obstacles[i].x, (int)obstacles[i].y, obstacles[i].width, obstacles[i].height, obstacleImages[obstacles[i].type]);
	}

	// 4. Collectibles Rendering (Coin & Fuel)
	
	
	if (coinItem.active)
	{
		iShowImage((int)coinItem.x, (int)coinItem.y, (int)(coinItem.width * 1.5f), (int)(coinItem.height * 1.5f), coinImage);
	}

	if (fuelItem.active)
	{
		iShowImage((int)fuelItem.x, (int)fuelItem.y, (int)(fuelItem.width * 1.5f), (int)(fuelItem.height * 1.5f), fuelImage);
	}
	// 5. Player Animation Rendering
	if (isJumping)
		iShowImage((int)characterX, (int)characterY, 180, 200, characterJumpImage);
	else
		iShowImage((int)characterX, (int)characterY, 180, 200, playerFrames[currentPlayerFrame]);

	// -------------------------------------------------------
	// 6. HUD / UI DISPLAY (Distance, Score & Fuel Bar)
	// -------------------------------------------------------

	// Top Right: Remaining Distance
	iSetColor(255, 255, 255);
	char distStr[50];
	sprintf_s(distStr, "Distance: %d m", (int)remainingDistance);
	iText(1050, 670, distStr, GLUT_BITMAP_TIMES_ROMAN_24);

	// Bottom Right: Score Count
	char scoreStr[50];
	sprintf_s(scoreStr, "Score: %d", score);
	iText(1100, 565, scoreStr, GLUT_BITMAP_TIMES_ROMAN_24);

	// Bottom Right: Fuel Meter Progress Bar
	iSetColor(100, 100, 100);
	iFilledRectangle(1050, 605, 180, 18); // Background Bar

	// Fuel Level Color 
	if (currentFuel > 40.0f)
		iSetColor(0, 230, 0);
	else
		iSetColor(255, 50, 50);

	iFilledRectangle(1050, 605, (int)((currentFuel / maxFuel) * 180), 18); // Dynamic Fill Bar

	// Fuel Meter Border & Text
	iSetColor(255, 255, 255);
	iRectangle(1050, 605, 180, 18);
	iText(980, 608, "Fuel:", GLUT_BITMAP_HELVETICA_12);

	// Fuel Missed Counter Text
	char missStr[50];
	sprintf_s(missStr, "Fuel Missed: %d/3", fuelMissedCount);
	iText(1050, 640, missStr, GLUT_BITMAP_HELVETICA_12);

	// -------------------------------------------------------
	// 7. GAME OVER & WIN POP-UP SCREENS
	// -------------------------------------------------------

	// WIN SCREEN
	if (isWin)
	{
		// Blue Pop-up Box
		iSetColor(15, 35, 80);
		iFilledRectangle(340, 160, 600, 400);

		// Border Light Blue
		iSetColor(0, 210, 255);
		iRectangle(340, 160, 600, 400);
		iRectangle(342, 162, 596, 396);

		// Text
		iSetColor(255, 255, 255);
		iText(440, 480, "700m COMPLETE! EXCELLENT!", GLUT_BITMAP_TIMES_ROMAN_24);

		// Button 1: Restart
		iSetColor(0, 120, 200);
		iFilledRectangle(400, 240, 200, 50);
		iSetColor(255, 255, 255);
		iText(460, 258, "RESTART", GLUT_BITMAP_HELVETICA_18);

		// Button 2: Menu
		iSetColor(20, 60, 130);
		iFilledRectangle(680, 240, 200, 50);
		iSetColor(255, 255, 255);
		iText(750, 258, "MENU", GLUT_BITMAP_HELVETICA_18);
	}

	// LOSE SCREEN
	if (isLose)
	{
		// Blue Pop-up Box
		iSetColor(15, 35, 80);
		iFilledRectangle(340, 160, 600, 400);

		// Border Light Blue
		iSetColor(0, 210, 255);
		iRectangle(340, 160, 600, 400);
		iRectangle(342, 162, 596, 396);

		// Text
		iSetColor(255, 255, 255);
		char loseMsg[60];
		sprintf_s(loseMsg, "YOU LOSE! Distance Left: %d m", (int)remainingDistance);
		iText(420, 480, loseMsg, GLUT_BITMAP_TIMES_ROMAN_24);

		// Button 1: Restart
		iSetColor(0, 120, 200);
		iFilledRectangle(400, 240, 200, 50);
		iSetColor(255, 255, 255);
		iText(460, 258, "RESTART", GLUT_BITMAP_HELVETICA_18);

		// Button 2: Menu
		iSetColor(20, 60, 130);
		iFilledRectangle(680, 240, 200, 50);
		iSetColor(255, 255, 255);
		iText(750, 258, "MENU", GLUT_BITMAP_HELVETICA_18);
	}
}

//---------Input
void iMouseMove(int mx, int my) {}
void iPassiveMouseMove(int mx, int my) {}

//new

void iMouse(int button, int state, int mx, int my)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		//menu screen handl click
		if (inMenu)
		{
			if (handleMenuClick(mx, my)) return;
		}

		//win screen buton click
		if (isWin || isLose)
		{
			//restart buton
			if (mx >= 400 && mx <= 600 && my >= 240 && my <= 290)
			{
				resetGame();
				return;
			}
			//menu buton
			if (mx >= 680 && mx <= 880 && my >= 240 && my <= 290)
			{
				resetGame();
				inMenu = true;
				return;
			}
		}

		//juming click
		if (!inMenu && !isWin)
		{
			isJumping = true;
		}
	}

	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		if (!inMenu && !isWin)
			isJumping = false;
	}
}



void fixedUpdate() {}

//load image 
void loadImages()
{
    loadMenuImages();  //menu background

    for (int i = 0; i < 4; i++)
    {
        char path[100];
        sprintf_s(path, "Images//balloon%d.png", i + 1);
        images[i] = iLoadImage(path);
        printf("Loaded: %s\n", path);
    }

    floorImage = iLoadImage("Images//floor.png");
    printf("Loaded: Images//floor.png\n");

    playerFrames[0] = iLoadImage("Images//player 2.png");
    playerFrames[1] = iLoadImage("Images//player 3.png");
    playerFrames[2] = iLoadImage("Images//player 4.png");
    playerFrames[3] = iLoadImage("Images//player 5.png");
    printf("Loaded player running frames\n");

    characterJumpImage = iLoadImage("Images//player 1.png");
    printf("Loaded: Images//player 1.png\n");
	// new---
	for (int i = 0; i < 5; i++)
	{
		char path[100];
		sprintf_s(path, "Images//obstacle %d.png", i + 1);
		obstacleImages[i] = iLoadImage(path);
		printf("Loaded: %s\n", path);
	}

	// --- COIN & FUEL IMAGES (NEW STEP 3) ---
	coinImage = iLoadImage("Images//coin.png");
	printf("Loaded: Images//coin.png\n");

	fuelImage = iLoadImage("Images//fuel.png");
	printf("Loaded: Images//fuel.png\n");
}

//--------------MAIN---------------
int main()
{
    iSetTimer(500, advanceMenuFrame);   //menu bg swap
    iSetTimer(83,  advanceFrame);       //game by animation
    iSetTimer(83,  updatePlayerAnimation);
    iSetTimer(16,  updateFloor);
    iSetTimer(16,  updateJump);

    iInitialize(1280, 720, "Escape Protocol");
    loadImages();
	resetGame();
    iStart();
    return 0;
}