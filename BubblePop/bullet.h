

#ifndef BULLET_H
#define BULLET_H

#include "iGraphics.h"
#include "obstacle.h"
#include <cmath>

struct Bullet
{
	float x, y;
	float speed;
	int width, height;
	bool active;
};

#define MAX_BULLETS 20

Bullet bullets[MAX_BULLETS];

bool isShooting = false;
int bulletShootTimer = 0;

// BULLET AMMO SYSTEM
int currentAmmo = 50;
int maxAmmo = 50;

// BULLET COLLECTIBLE (Without External Image)
struct BulletItem {
	float x, y;
	int width, height;
	bool active;
};

BulletItem bulletItem;
float lastBulletReloadDistance = 500.0f;

void resetBullets()
{
	for (int i = 0; i < MAX_BULLETS; i++)
	{
		bullets[i].x = -100;
		bullets[i].y = -100;
		bullets[i].width = 24;
		bullets[i].height = 8;
		bullets[i].speed = 900.0f;
		bullets[i].active = false;
	}

	isShooting = false;
	bulletShootTimer = 0;
	currentAmmo = 50;

	bulletItem.width = 35;
	bulletItem.height = 35;
	bulletItem.x = 1600.0f;
	bulletItem.y = 250.0f;
	bulletItem.active = true;
	lastBulletReloadDistance = 500.0f;
}

void spawnBullet(float startX, float startY)
{
	if (currentAmmo <= 0) return; // Ammo limit check

	for (int i = 0; i < MAX_BULLETS; i++)
	{
		if (!bullets[i].active)
		{
			bullets[i].x = startX;
			bullets[i].y = startY;
			bullets[i].active = true;
			currentAmmo--; // Decrease ammo on fire
			break;
		}
	}
}

void updateBullets(bool inMenu, bool isWin, bool isLose, float playerX, float playerY, int currentLevel, float remainingDistance = 500.0f)
{
	if (inMenu || isWin || isLose) return;

	if (currentLevel == 2 && isShooting && currentAmmo > 0)
	{
		bulletShootTimer++;
		if (bulletShootTimer % 8 == 0)
		{
			spawnBullet(playerX + 110, playerY + 95);
		}
	}

	for (int i = 0; i < MAX_BULLETS; i++)
	{
		if (!bullets[i].active) continue;

		bullets[i].x += bullets[i].speed * 0.016f;

		if (bullets[i].x > 1300)
		{
			bullets[i].active = false;
			continue;
		}

		// ROCKET COLLISION
		if (rocket.active &&
			bullets[i].x < rocket.x + rocket.width &&
			bullets[i].x + bullets[i].width > rocket.x &&
			bullets[i].y < rocket.y + rocket.height &&
			bullets[i].y + bullets[i].height > rocket.y)
		{
			bullets[i].active = false;
			rocket.health--;

			if (rocket.health <= 0)
			{
				rocket.active = false;
			}
			continue;
		}

		// DESTROYABLE OBSTACLE COLLISION
		for (int obsIdx = 0; obsIdx < MAX_OBSTACLES; obsIdx++)
		{
			Obstacle& obs = obstacles[obsIdx];
			if (!obs.active) continue;

			if (obs.type == 1 && obs.health < 999)
			{
				if (bullets[i].x < obs.x + obs.width &&
					bullets[i].x + bullets[i].width > obs.x &&
					bullets[i].y < obs.y + obs.height &&
					bullets[i].y + bullets[i].height > obs.y)
				{
					bullets[i].active = false;
					obs.health--;

					if (obs.health <= 0)
					{
						obs.active = false;
					}
					break;
				}
			}
		}
	}

	// 100M AMMO PICKUP SPAWN & COLLISION LOGIC
	if (currentLevel == 2)
	{
		float speed = 220.0f;
		bulletItem.x -= speed * 0.016f;

		if (lastBulletReloadDistance - remainingDistance >= 100.0f)
		{
			bulletItem.x = 1280.0f + (rand() % 150);
			bulletItem.y = (float)(150 + (rand() % 300));
			bulletItem.active = true;
			lastBulletReloadDistance = remainingDistance;
		}

		float charLeft = playerX + 15;
		float charRight = playerX + 90;
		float charBottom = playerY;
		float charTop = playerY + 120;

		if (bulletItem.active &&
			charLeft < bulletItem.x + bulletItem.width &&
			charRight > bulletItem.x &&
			charBottom < bulletItem.y + bulletItem.height &&
			charTop > bulletItem.y)
		{
			currentAmmo = maxAmmo; // Collect and reload to full
			bulletItem.active = false;
		}
	}
}

// DRAW BULLETS
void drawBullets()
{
	for (int i = 0; i < MAX_BULLETS; i++)
	{
		if (!bullets[i].active) continue;

		int bx = (int)bullets[i].x;
		int by = (int)bullets[i].y;
		int bw = bullets[i].width;
		int bh = bullets[i].height;

		iSetColor(180, 130, 20);
		iFilledRectangle(bx, by, 4, bh);

		iSetColor(255, 200, 0);
		iFilledRectangle(bx + 4, by, 12, bh);

		iSetColor(255, 240, 150);
		iFilledRectangle(bx + 4, by + bh - 2, 12, 2);

		double tipX[3] = { (double)(bx + 16), (double)(bx + 16), (double)(bx + bw) };
		double tipY[3] = { (double)by, (double)(by + bh), (double)(by + (bh / 2.0)) };

		iSetColor(255, 215, 0);
		iFilledPolygon(tipX, tipY, 3);
	}
}

// DRAW CUSTOM BULLET PICKUP ITEM (WITH WATER-BLUE/WHITE SHIELD)
void drawBulletPickup()
{
	if (!bulletItem.active) return;

	int ix = (int)bulletItem.x;
	int iy = (int)bulletItem.y;
	int cx = ix + 17;
	int cy = iy + 17;

	// White/Water Blue Shield Aura
	iSetColor(180, 230, 255);
	iFilledCircle(cx, cy, 22);

	iSetColor(255, 255, 255);
	iCircle(cx, cy, 22);
	iCircle(cx, cy, 20);

	// Drawn Golden Bullet Inside
	int bx = ix + 6;
	int by = iy + 13;

	iSetColor(180, 130, 20);
	iFilledRectangle(bx, by, 3, 10);

	iSetColor(255, 215, 0);
	iFilledRectangle(bx + 3, by, 12, 10);

	double tipX[3] = { (double)(bx + 15), (double)(bx + 15), (double)(bx + 23) };
	double tipY[3] = { (double)by, (double)(by + 10), (double)(by + 5) };

	iSetColor(255, 230, 80);
	iFilledPolygon(tipX, tipY, 3);
}

#endif // BULLET_H