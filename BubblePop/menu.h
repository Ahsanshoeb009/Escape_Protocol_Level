#ifndef MENU_H
#define MENU_H

#include "iGraphics.h"

// -------------------------------------------------------
// MENU STATE
// inMenu is DEFINED in imain.cpp, only declared here
// -------------------------------------------------------
extern bool inMenu;

static int   menuBg[2]   = {0, 0};
static int   menuBgFrame = 0;

static int btnX    = 60;
static int btnW    = 280;
static int btnH    = 70;
static int btnNewY  = 430;
static int btnLoadY = 320;
static int btnExitY = 210;

// -------------------------------------------------------
// LOAD MENU IMAGES
// -------------------------------------------------------
static void loadMenuImages()
{
    menuBg[0] = iLoadImage("Images//menu_1.jpg");
    menuBg[1] = iLoadImage("Images//menu_2.jpg");
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
// DRAW A SINGLE BUTTON BOX + LABEL
// -------------------------------------------------------
static void drawMenuButton(int x, int y, int w, int h, const char* label)
{
    iSetColor(255, 255, 255);
    iRectangle(x, y, w, h);
    iSetColor(255, 255, 255);
    iText(x + 20, y + h / 2 - 14, label, GLUT_BITMAP_TIMES_ROMAN_24);
}

// -------------------------------------------------------
// DRAW MENU
// -------------------------------------------------------
static void drawMenu()
{
    iShowImage(0, 0, 1280, 720, menuBg[menuBgFrame]);
    drawMenuButton(btnX, btnNewY,  btnW, btnH, "New Game");
    drawMenuButton(btnX, btnLoadY, btnW, btnH, "Load Game");
    drawMenuButton(btnX, btnExitY, btnW, btnH, "Exit");
}

// -------------------------------------------------------
// HELPER
// -------------------------------------------------------
static bool insideButton(int mx, int my, int x, int y, int w, int h)
{
    return (mx >= x && mx <= x + w &&
            my >= y && my <= y + h);
}

// -------------------------------------------------------
// HANDLE MENU CLICK
// -------------------------------------------------------
static bool handleMenuClick(int mx, int my)
{
    if (!inMenu) return false;

    if (insideButton(mx, my, btnX, btnNewY, btnW, btnH))
    {
        inMenu = false;
        return true;
    }
    if (insideButton(mx, my, btnX, btnLoadY, btnW, btnH))
    {
        // Load Game -- not implemented yet
        return true;
    }
    if (insideButton(mx, my, btnX, btnExitY, btnW, btnH))
    {
        exit(0);
    }
    return false;
}

#endif // MENU_H