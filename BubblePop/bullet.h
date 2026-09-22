


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

// BULLET AMMO SYSTEM (shared by Level 2 & Level 3)
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

	// Gun mechanic exists in Level 2 AND Level 3.
	bool hasGun = (currentLevel == 2 || currentLevel == 3);

	// While the "FINAL ROUND" banner is up, freeze auto-fire so the player
	// can't waste ammo before the security enemies actually arrive.
	if (hasGun && isShooting && currentAmmo > 0 && !showFinalRoundText)
	{
		bulletShootTimer++;
		if (bulletShootTimer % 8 == 0)
		{
			// Calibrated start position to emerge directly from the higher gun barrel
			spawnBullet(playerX + 148, playerY + 145);
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

		// SECURITY COLLISION (Level 3 final fight only)
		if (currentLevel == 3 && isFinalFight)
		{
			bool hitSecurity = false;

			for (int s = 0; s < MAX_SECURITY; s++)
			{
				if (!securities[s].active) continue;

				if (bullets[i].x < securities[s].x + securities[s].width &&
					bullets[i].x + bullets[i].width > securities[s].x &&
					bullets[i].y < securities[s].y + securities[s].height &&
					bullets[i].y + bullets[i].height > securities[s].y)
				{
					bullets[i].active = false;
					securities[s].health--;

					if (securities[s].health <= 0)
					{
						securities[s].active = false;
						securitiesAlive--;
					}

					hitSecurity = true;
					break;
				}
			}

			if (hitSecurity) continue;
		}

		// DESTROYABLE OBSTACLE COLLISION
		for (int obsIdx = 0; obsIdx < MAX_OBSTACLES; obsIdx++)
		{
			Obstacle& obs = obstacles[obsIdx];
			if (!obs.active) continue;

			if (obs.type == 1 && obs.health < 999)
			{
				float cx = obs.x + obs.width * 0.5f;
				float cy = obs.y + obs.height * 0.5f;
				const EllipseHitbox& e = obstacleEllipse[obs.type % 4];
				float rx = obs.width  * e.rx;
				float ry = obs.height * e.ry;

				float bLeft = bullets[i].x;
				float bRight = bullets[i].x + bullets[i].width;
				float bBottom = bullets[i].y;
				float bTop = bullets[i].y + bullets[i].height;

				if (checkEllipseBoxCollision(cx, cy, rx, ry, bLeft, bRight, bBottom, bTop))
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

	// 100M AMMO PICKUP SPAWN & COLLISION LOGIC (Level 2 & Level 3)
	if (hasGun)
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

// DRAW CUSTOM BULLET PICKUP ITEM
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