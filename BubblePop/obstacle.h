

#ifndef OBSTACLE_H
#define OBSTACLE_H

#include "iGraphics.h"
#include <cmath>
#include <cstdlib>

// -------------------------------------------------------
// OBSTACLE STRUCT
// -------------------------------------------------------
struct Obstacle
{
	float x, y;
	int width, height;
	int type;      // 0 to 3 (Level 1 & Level 2)
	int health;
	bool active;
};

#define MAX_OBSTACLES 6

Obstacle obstacles[MAX_OBSTACLES];

int obstacleImagesLvl1[4];
int obstacleImagesLvl2[4];
int nextObstacleType = 0;

// -------------------------------------------------------
// ROCKET
// -------------------------------------------------------
struct Rocket
{
	float x, y;
	int width, height;
	float speed;
	int health;
	bool active;
};

Rocket rocket;
int rocketImage;
float lastRocketDistance = 500.0f;

// -------------------------------------------------------
// COLLECTIBLES
// -------------------------------------------------------
struct Collectible
{
	float x, y;
	int width, height;
	bool active;
};

Collectible coinItem;
Collectible fuelItem;

int coinImage;
int fuelImage;

// -------------------------------------------------------
// BONUS COIN STREAM SYSTEM
// -------------------------------------------------------
#define MAX_BONUS_COINS 12

struct BonusCoin {
	float x, y;
	bool active;
};

BonusCoin bonusCoins[MAX_BONUS_COINS];

void initBonusCoins() {
	for (int i = 0; i < MAX_BONUS_COINS; i++) {
		bonusCoins[i].x = 1280.0f + (i % 6) * 70.0f;
		bonusCoins[i].y = (i < 6) ? 220.0f : 380.0f;
		bonusCoins[i].active = true;
	}
}

void updateBonusCoins(float speed, float charLeft, float charRight, float charBottom, float charTop, int& score) {
	for (int i = 0; i < MAX_BONUS_COINS; i++) {
		bonusCoins[i].x -= speed * 0.016f;

		if (bonusCoins[i].x < -50.0f) {
			bonusCoins[i].x = 1280.0f + (rand() % 150);
			bonusCoins[i].active = true;
		}

		if (bonusCoins[i].active &&
			charLeft < bonusCoins[i].x + 40 &&
			charRight > bonusCoins[i].x &&
			charBottom < bonusCoins[i].y + 40 &&
			charTop > bonusCoins[i].y)
		{
			score += 1;
			bonusCoins[i].active = false;
		}
	}
}

void drawBonusCoins() {
	for (int i = 0; i < MAX_BONUS_COINS; i++) {
		if (bonusCoins[i].active) {
			iShowImage((int)bonusCoins[i].x, (int)bonusCoins[i].y, 40, 40, coinImage);
		}
	}
}

// -------------------------------------------------------
// HITBOX INSETS
// -------------------------------------------------------
struct HitboxInset
{
	float left, right, top, bottom;
};

#define MAX_SUBBOXES 4

int obstacleBoxCount[4] = { 1, 1, 1, 1 };

HitboxInset obstacleInsets[4][MAX_SUBBOXES] =
{
	{ { 0.137f, 0.149f, 0.069f, 0.074f } },
	{ { 0.050f, 0.050f, 0.050f, 0.050f } },
	{ { 0.050f, 0.050f, 0.050f, 0.050f } },
	{ { 0.066f, 0.033f, 0.140f, 0.721f } }
};

int getRandomCollectibleY()
{
	int positions[] = { 150, 220, 300, 380, 450 };
	return positions[rand() % 5];
}

void applyObstacleType(Obstacle& obs, int currentLevel = 1)
{
	obs.active = true;

	if (obs.type == 0)
	{
		obs.width = (currentLevel == 1) ? 280 : 350;
		obs.height = (currentLevel == 1) ? 144 : 180;
		obs.health = 999;
		int positions[] = { 124, 220, 320 };
		obs.y = (float)positions[rand() % 3];
	}
	else if (obs.type == 1)
	{
		obs.width = (currentLevel == 1) ? 256 : 320;
		obs.height = (currentLevel == 1) ? 144 : 180;
		obs.health = (currentLevel == 2) ? 5 : 999;
		int positions[] = { 124, 230, 380, 480 };
		obs.y = (float)positions[rand() % 4];
	}
	else if (obs.type == 2)
	{
		obs.width = (currentLevel == 1) ? 288 : 360;
		obs.height = (currentLevel == 1) ? 144 : 180;
		obs.health = 999;
		int positions[] = { 124, 220, 320, 400 };
		obs.y = (float)positions[rand() % 4];
	}
	else if (obs.type == 3)
	{
		obs.width = (currentLevel == 1) ? 224 : 280;
		obs.height = (currentLevel == 1) ? 160 : 200;
		obs.health = 999;
		int positions[] = { 124, 200, 280, 350 };
		obs.y = (float)positions[rand() % 4];
	}
}

void resetRocket()
{
	rocket.width = 120;
	rocket.height = 45;
	rocket.x = 1300;
	rocket.y = 200;
	rocket.speed = 550.0f;
	rocket.health = 4;
	rocket.active = false;
	lastRocketDistance = 500.0f;
}

void resetObstacles(int currentLevel = 1)
{
	nextObstacleType = 0;

	for (int i = 0; i < MAX_OBSTACLES; i++)
	{
		obstacles[i].x = 1280.0f + (i * 550.0f);
		obstacles[i].type = nextObstacleType;
		nextObstacleType = (nextObstacleType + 1) % 4;
		applyObstacleType(obstacles[i], currentLevel);
	}

	coinItem.width = 40;
	coinItem.height = 40;
	coinItem.x = obstacles[0].x + 250 + (rand() % 200);
	coinItem.y = (float)getRandomCollectibleY();
	coinItem.active = true;

	fuelItem.width = 50;
	fuelItem.height = 50;
	fuelItem.x = obstacles[1].x + 250 + (rand() % 200);
	fuelItem.y = (float)getRandomCollectibleY();
	fuelItem.active = true;

	resetRocket();
}

bool checkCircleBoxCollision(float circleX, float circleY, float radius, float rectLeft, float rectRight, float rectBottom, float rectTop)
{
	float closestX = circleX;
	if (closestX < rectLeft) closestX = rectLeft;
	if (closestX > rectRight) closestX = rectRight;

	float closestY = circleY;
	if (closestY < rectBottom) closestY = rectBottom;
	if (closestY > rectTop) closestY = rectTop;

	float dx = circleX - closestX;
	float dy = circleY - closestY;

	return (dx * dx + dy * dy) <= (radius * radius);
}

bool checkObstacleCollision(const Obstacle& obs, float charLeft, float charRight, float charBottom, float charTop, int currentLevel = 1)
{
	if (!obs.active) return false;

	if (currentLevel == 2)
	{
		float centerX = obs.x + obs.width * 0.5f;
		float centerY = obs.y + obs.height * 0.5f;
		float minDimension = (obs.width < obs.height) ? (float)obs.width : (float)obs.height;
		float radius = (minDimension * 0.5f) * 0.80f;

		return checkCircleBoxCollision(centerX, centerY, radius, charLeft, charRight, charBottom, charTop);
	}

	float ox = obs.x;
	float oy = obs.y;
	float ow = (float)obs.width;
	float oh = (float)obs.height;

	int count = obstacleBoxCount[obs.type];

	for (int b = 0; b < count; b++)
	{
		const HitboxInset& in = obstacleInsets[obs.type][b];

		float obsLeft = ox + ow * in.left;
		float obsRight = ox + ow * (1.0f - in.right);
		float obsBottom = oy + oh * in.bottom;
		float obsTop = oy + oh * (1.0f - in.top);

		if (charLeft < obsRight && charRight > obsLeft && charBottom < obsTop && charTop > obsBottom)
		{
			return true;
		}
	}

	return false;
}

void updateRocket(float remainingDistance, float charLeft, float charRight, float charBottom, float charTop, bool& isLose, int currentLevel)
{
	if (currentLevel != 2) return;

	if (!rocket.active && (lastRocketDistance - remainingDistance >= 50.0f))
	{
		rocket.x = 1280.0f;
		rocket.y = (float)(130 + (rand() % 400));
		rocket.health = 4;
		rocket.active = true;
		lastRocketDistance = remainingDistance;
	}

	if (rocket.active)
	{
		rocket.x -= rocket.speed * 0.016f;

		if (rocket.x < -150) rocket.active = false;

		if (charLeft < rocket.x + rocket.width &&
			charRight > rocket.x &&
			charBottom < rocket.y + rocket.height &&
			charTop > rocket.y)
		{
			isLose = true;
		}
	}
}

void updateObstacles(
	float currentSpeed, float charLeft, float charRight, float charBottom, float charTop,
	int& score, float& currentFuel, float maxFuel, bool& isLose,
	int currentLevel = 1, float remainingDistance = 500.0f)
{
	for (int i = 0; i < MAX_OBSTACLES; i++)
	{
		obstacles[i].x -= currentSpeed * 0.016f;

		if (obstacles[i].x < -350)
		{
			int gap = 420 + (rand() % 150);
			int prevIndex = (i == 0) ? MAX_OBSTACLES - 1 : i - 1;

			obstacles[i].x = obstacles[prevIndex].x + gap;
			if (obstacles[i].x < 1280) obstacles[i].x = 1280.0f + gap;

			obstacles[i].type = nextObstacleType;
			nextObstacleType = (nextObstacleType + 1) % 4;
			applyObstacleType(obstacles[i], currentLevel);
		}

		if (obstacles[i].active && checkObstacleCollision(obstacles[i], charLeft, charRight, charBottom, charTop, currentLevel))
		{
			isLose = true;
		}
	}

	// COIN
	coinItem.x -= currentSpeed * 0.016f;
	if (coinItem.x < -50)
	{
		coinItem.x = obstacles[0].x + 250 + (rand() % 250);
		coinItem.y = (float)getRandomCollectibleY();
		coinItem.active = true;
	}

	if (coinItem.active && charLeft < coinItem.x + coinItem.width && charRight > coinItem.x && charBottom < coinItem.y + coinItem.height && charTop > coinItem.y)
	{
		score += 1;
		coinItem.active = false;
	}

	// FUEL (3 times miss logic is removed completely)
	fuelItem.x -= currentSpeed * 0.016f;
	if (fuelItem.x < -50)
	{
		fuelItem.x = obstacles[2].x + 250 + (rand() % 250);
		fuelItem.y = (float)getRandomCollectibleY();
		fuelItem.active = true;
	}

	if (fuelItem.active && charLeft < fuelItem.x + fuelItem.width && charRight > fuelItem.x && charBottom < fuelItem.y + fuelItem.height && charTop > fuelItem.y)
	{
		currentFuel += 35.0f;
		if (currentFuel > maxFuel) currentFuel = maxFuel;
		fuelItem.active = false;
	}

	updateRocket(remainingDistance, charLeft, charRight, charBottom, charTop, isLose, currentLevel);
}

void drawObstacles(int currentLevel = 1)
{
	for (int i = 0; i < MAX_OBSTACLES; i++)
	{
		if (!obstacles[i].active) continue;

		int imgToDraw;
		if (currentLevel == 2) imgToDraw = obstacleImagesLvl2[obstacles[i].type % 4];
		else                   imgToDraw = obstacleImagesLvl1[obstacles[i].type % 4];

		iShowImage((int)obstacles[i].x, (int)obstacles[i].y, obstacles[i].width, obstacles[i].height, imgToDraw);
	}

	if (coinItem.active) iShowImage((int)coinItem.x, (int)coinItem.y, coinItem.width, coinItem.height, coinImage);
	if (fuelItem.active) iShowImage((int)fuelItem.x, (int)fuelItem.y, fuelItem.width, fuelItem.height, fuelImage);

	if (currentLevel == 2 && rocket.active)
	{
		iShowImage((int)rocket.x, (int)rocket.y, rocket.width, rocket.height, rocketImage);
	}
}

void drawObstacleHitboxes()
{
	iSetColor(255, 0, 0);

	for (int i = 0; i < MAX_OBSTACLES; i++)
	{
		Obstacle& o = obstacles[i];
		if (!o.active) continue;

		float cx = o.x + o.width * 0.5f;
		float cy = o.y + o.height * 0.5f;
		float minDimension = (o.width < o.height) ? (float)o.width : (float)o.height;
		float r = (minDimension * 0.5f) * 0.80f;

		iCircle((int)cx, (int)cy, (int)r);
	}

	if (rocket.active) iRectangle((int)rocket.x, (int)rocket.y, rocket.width, rocket.height);
}

void loadObstacleImages()
{
	for (int i = 0; i < 4; i++)
	{
		char path[100];
		sprintf_s(path, "Images//obstacle %d.png", i + 1);
		obstacleImagesLvl1[i] = iLoadImage(path);
	}

	for (int i = 0; i < 4; i++)
	{
		char path[100];
		sprintf_s(path, "Images//Obstacle%d.png", 11 + i);
		obstacleImagesLvl2[i] = iLoadImage(path);
	}

	coinImage = iLoadImage("Images//coin.png");
	fuelImage = iLoadImage("Images//fuel.png");
	rocketImage = iLoadImage("Images//Rocket.png");
}

#endif // OBSTACLE_H
