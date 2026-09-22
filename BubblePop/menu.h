#ifndef MENU_H
#define MENU_H
#include "iGraphics.h"

// -------------------------------------------------------
// MENU STATE
// inMenu is DEFINED in imain.cpp, only declared here
// -------------------------------------------------------
extern bool inMenu;
extern void stopMenuMusic();   // defined in imain.cpp
extern void startLevel1();     // defined in imain.cpp
extern void startLevel2();     // defined in imain.cpp

static int menuBg[2] = { 0, 0 };
static int menuBgFrame = 0;
static int menuTitleImage = 0;

static int btnX = 60;
static int btnW = 280;
static int btnH = 70;
static int btnNewY = 490;
static int btnLoadY = 390;
static int btnAboutY = 290;
static int btnExitY = 190;

// -------------------------------------------------------
// SINGLE highlight index (-1 = none)
// -------------------------------------------------------
static int menuSelected = -1;
static bool usingKeyboard = false;
static bool aboutOpen = false;

// -------------------------------------------------------
// LEVEL SELECT PAGE
// -------------------------------------------------------
static bool inLevelSelect = false;
static int levelSelected = -1;
static bool levelUsingKeyboard = false;

static int lvlBtnW = 300;
static int lvlBtnH = 80;
static int lvlBtn1X = 490;
static int lvlBtn2X = 490;
static int lvlBtn3X = 490;
static int lvlBtn1Y = 430;
static int lvlBtn2Y = 310;
static int lvlBtn3Y = 190;

// -------------------------------------------------------
// LOAD MENU IMAGES
// -------------------------------------------------------
static void loadMenuImages()
{
	menuBg[0] = iLoadImage("Images//menu_1.jpg");
	menuBg[1] = iLoadImage("Images//menu_2.jpg");
	menuTitleImage = iLoadImage("Images//menu_title.png");
	printf("Loaded menu background images\n");
}

// -------------------------------------------------------
// TIMER CALLBACK -- swap background every 500 ms
// -------------------------------------------------------
static void advanceMenuFrame()
{
	menuBgFrame = (menuBgFrame + 1) % 2;
}

// -------------------------------------------------------
// HELPER -- point-in-rect test
// -------------------------------------------------------
static bool insideButton(int mx, int my, int x, int y, int w, int h)
{
	return (mx >= x && mx <= x + w &&
		my >= y && my <= y + h);
}

// -------------------------------------------------------
// DRAW A SINGLE BUTTON
// -------------------------------------------------------
static void drawMenuButton(int x, int y, int w, int h,
	const char* label, bool highlighted, bool disabled = false)
{
	if (disabled)
	{
		iSetColor(60, 60, 60);
		iFilledRectangle(x, y, w, h);
	}
	else if (highlighted)
	{
		iSetColor(220, 60, 60);
		iFilledRectangle(x, y, w, h);
	}

	if (disabled) iSetColor(120, 120, 120);
	else          iSetColor(255, 255, 255);
	iRectangle(x, y, w, h);

	if (disabled) iSetColor(120, 120, 120);
	else          iSetColor(255, 255, 255);
	iText(x + 20, y + h / 2 - 14, label, GLUT_BITMAP_TIMES_ROMAN_24);
}

// -------------------------------------------------------
// ABOUT GAME OVERLAY
// -------------------------------------------------------
static void drawAboutOverlay()
{
	iSetColor(0, 0, 0);
	iFilledRectangle(200, 100, 880, 520);

	iSetColor(220, 60, 60);
	iRectangle(200, 100, 880, 520);

	iSetColor(255, 80, 80);
	iText(340, 585, "HOW TO PLAY", GLUT_BITMAP_TIMES_ROMAN_24);

	iSetColor(255, 255, 255);

	iText(230, 545, "CONTROLS", GLUT_BITMAP_HELVETICA_18);
	iText(230, 515, "Hold Left Mouse / Space - Fly upward (jetpack)", GLUT_BITMAP_HELVETICA_12);
	iText(230, 495, "Release - Fall down", GLUT_BITMAP_HELVETICA_12);

	iText(230, 455, "OBJECTIVE", GLUT_BITMAP_HELVETICA_18);
	iText(230, 425, "Survive 700 m using your jetpack.", GLUT_BITMAP_HELVETICA_12);
	iText(230, 405, "Dodge obstacles and collect fuel packs to stay alive.", GLUT_BITMAP_HELVETICA_12);

	iText(230, 365, "TIPS", GLUT_BITMAP_HELVETICA_18);
	iText(230, 340, "- Fuel drains constantly - grab fuel packs quickly.", GLUT_BITMAP_HELVETICA_12);
	iText(230, 320, "- Missing 3 fuel packs in a row ends the run.", GLUT_BITMAP_HELVETICA_12);
	iText(230, 300, "- Speed increases in the final 300 m.", GLUT_BITMAP_HELVETICA_12);
	iText(230, 280, "- Collect coins for score.", GLUT_BITMAP_HELVETICA_12);

	iSetColor(180, 180, 180);
	iText(230, 150, "Press ESC or ENTER to close", GLUT_BITMAP_HELVETICA_12);
}

// -------------------------------------------------------
// DRAW LEVEL SELECT PAGE
// -------------------------------------------------------
static void drawLevelSelect()
{
	// Black background
	iSetColor(0, 0, 0);
	iFilledRectangle(0, 0, 1280, 720);

	// Title
	iSetColor(255, 255, 255);
	iText(510, 610, "SELECT LEVEL", GLUT_BITMAP_TIMES_ROMAN_24);

	// Level 1 & 2 -- active
	drawMenuButton(lvlBtn1X, lvlBtn1Y, lvlBtnW, lvlBtnH, "Level 1", levelSelected == 0, false);
	drawMenuButton(lvlBtn2X, lvlBtn2Y, lvlBtnW, lvlBtnH, "Level 2", levelSelected == 1, false);

	// Level 3 -- disabled (greyed out)
	drawMenuButton(lvlBtn3X, lvlBtn3Y, lvlBtnW, lvlBtnH, "Level 3 (Coming Soon)", levelSelected == 2, true);

	// Back hint
	iSetColor(150, 150, 150);
	iText(490, 110, "Press ESC to go back", GLUT_BITMAP_HELVETICA_12);
}

// -------------------------------------------------------
// DRAW MENU
// -------------------------------------------------------
static void drawMenu()
{
	if (inLevelSelect)
	{
		drawLevelSelect();
		return;
	}

	iShowImage(0, 0, 1280, 720, menuBg[menuBgFrame]);

	iShowImage(btnX, btnNewY + 90, 280, 153, menuTitleImage);

	drawMenuButton(btnX, btnNewY, btnW, btnH, "New Game", menuSelected == 0);
	drawMenuButton(btnX, btnLoadY, btnW, btnH, "Load Game", menuSelected == 1);
	drawMenuButton(btnX, btnAboutY, btnW, btnH, "About Game", menuSelected == 2);
	drawMenuButton(btnX, btnExitY, btnW, btnH, "Exit", menuSelected == 3);

	if (aboutOpen)
		drawAboutOverlay();
}

// -------------------------------------------------------
// MOUSE MOVE -- hover
// -------------------------------------------------------
static void handleMenuMouseMove(int mx, int my)
{
	if (!inMenu) return;

	// Level select hover -- Level 1 and Level 2 are selectable
	if (inLevelSelect)
	{
		levelUsingKeyboard = false;
		if (insideButton(mx, my, lvlBtn1X, lvlBtn1Y, lvlBtnW, lvlBtnH))      levelSelected = 0;
		else if (insideButton(mx, my, lvlBtn2X, lvlBtn2Y, lvlBtnW, lvlBtnH)) levelSelected = 1;
		else                                                                 levelSelected = -1;
		return;
	}

	usingKeyboard = false;
	if (insideButton(mx, my, btnX, btnNewY, btnW, btnH)) menuSelected = 0;
	else if (insideButton(mx, my, btnX, btnLoadY, btnW, btnH)) menuSelected = 1;
	else if (insideButton(mx, my, btnX, btnAboutY, btnW, btnH)) menuSelected = 2;
	else if (insideButton(mx, my, btnX, btnExitY, btnW, btnH)) menuSelected = 3;
	else                                                         menuSelected = -1;
}

// -------------------------------------------------------
// CONFIRM main menu selection
// -------------------------------------------------------
static void confirmMenuSelection()
{
	switch (menuSelected)
	{
	case 0: inLevelSelect = true; levelSelected = -1; levelUsingKeyboard = false; break;
	case 1: /* Load Game */   break;
	case 2: aboutOpen = true; break;
	case 3: exit(0);
	default: break;
	}
}

// -------------------------------------------------------
// CONFIRM level selection
// -------------------------------------------------------
static void confirmLevelSelection()
{
	if (levelSelected == 0)
	{
		inLevelSelect = false;
		stopMenuMusic();       // cut the menu track immediately
		startLevel1();          // start Level 1 (sets inMenu = false)
	}
	else if (levelSelected == 1)
	{
		inLevelSelect = false;
		stopMenuMusic();       // cut the menu track immediately
		startLevel2();          // start Level 2 (sets inMenu = false)
	}
	// Level 3 does nothing (disabled)
}

// -------------------------------------------------------
// HANDLE MENU CLICK
// -------------------------------------------------------
static bool handleMenuClick(int mx, int my)
{
	if (!inMenu) return false;

	// Level select clicks
	if (inLevelSelect)
	{
		if (insideButton(mx, my, lvlBtn1X, lvlBtn1Y, lvlBtnW, lvlBtnH))
		{
			levelSelected = 0;
			confirmLevelSelection();
			return true;
		}
		if (insideButton(mx, my, lvlBtn2X, lvlBtn2Y, lvlBtnW, lvlBtnH))
		{
			levelSelected = 1;
			confirmLevelSelection();
			return true;
		}
		// Level 3 clicks intentionally ignored (disabled)
		return true;
	}

	if (aboutOpen) { aboutOpen = false; return true; }

	if (insideButton(mx, my, btnX, btnNewY, btnW, btnH)) { inLevelSelect = true; levelSelected = -1; levelUsingKeyboard = false; return true; }
	if (insideButton(mx, my, btnX, btnLoadY, btnW, btnH)) { return true; }
	if (insideButton(mx, my, btnX, btnAboutY, btnW, btnH)) { aboutOpen = true; return true; }
	if (insideButton(mx, my, btnX, btnExitY, btnW, btnH)) { exit(0); }

	return false;
}

// -------------------------------------------------------
// HANDLE REGULAR KEYS  (Enter=13, Escape=27)
// -------------------------------------------------------
static void handleMenuKey(unsigned char key)
{
	if (!inMenu) return;

	if (inLevelSelect)
	{
		if (key == 27) { inLevelSelect = false; menuSelected = 0; }  // ESC = back
		if (key == 13) confirmLevelSelection();
		return;
	}

	if (aboutOpen)
	{
		if (key == 13 || key == 27) aboutOpen = false;
		return;
	}

	if (key == 13)
		confirmMenuSelection();
}

// -------------------------------------------------------
// HANDLE SPECIAL KEYS  (arrow keys)
// -------------------------------------------------------
static void handleMenuSpecialKey(int key)
{
	if (!inMenu) return;

	// Level select keyboard nav -- Level 1 and Level 2 are navigable
	if (inLevelSelect)
	{
		if (key == GLUT_KEY_UP || key == GLUT_KEY_DOWN)
		{
			if (!levelUsingKeyboard)
			{
				levelUsingKeyboard = true;
				levelSelected = 0;
				return;
			}

			// Only two selectable options (0 and 1), so both
			// directions simply toggle between them.
			levelSelected = (levelSelected + 1) % 2;
		}
		return;
	}

	if (aboutOpen) return;

	if (key == GLUT_KEY_UP || key == GLUT_KEY_DOWN)
	{
		if (!usingKeyboard)
		{
			usingKeyboard = true;
			menuSelected = 0;
			return;
		}

		if (key == GLUT_KEY_UP)
			menuSelected = (menuSelected + 3) % 4;
		if (key == GLUT_KEY_DOWN)
			menuSelected = (menuSelected + 1) % 4;
	}
}

#endif // MENU_H