#include "iGraphics.h"

int images[4];
int currentFrame = 0;

int floorImage;

// ---------------- FLOOR ----------------

float floorX = 0;
float floorSpeed = 200.0f;

int floorWidth = 1280;
int floorHeight = 124;


// ---------------- BACKGROUND ANIMATION ----------------

void advanceFrame()
{
    currentFrame = (currentFrame + 1) % 4;
}


// ---------------- FLOOR MOVEMENT ----------------

void updateFloor()
{
    floorX -= floorSpeed * 0.016f;

    if (floorX <= -floorWidth)
    {
        floorX += floorWidth;
    }
}


// ---------------- DRAW ----------------

void iDraw()
{
    iClear();

    // Animated background
    iShowImage(0, 0, 1280, 720, images[currentFrame]);

    // First floor
    iShowImage(
        (int)floorX,
        0,
        floorWidth,
        floorHeight,
        floorImage
    );

    // Second floor
    iShowImage(
        (int)floorX + floorWidth,
        0,
        floorWidth,
        floorHeight,
        floorImage
    );
}


// ---------------- INPUT ----------------

void iMouseMove(int mx, int my) {}
void iPassiveMouseMove(int mx, int my) {}
void iMouse(int button, int state, int mx, int my) {}
void fixedUpdate() {}


// ---------------- LOAD IMAGES ----------------

void loadImages()
{
    // Load 4 background frames
    for (int i = 0; i < 4; i++)
    {
        char path[100];

        sprintf_s(path, "Images//balloon%d.png", i + 1);

        images[i] = iLoadImage(path);

        printf("Loaded: %s\n", path);
    }

    // Load floor
    floorImage = iLoadImage("Images//floor.png");

    printf("Loaded: Images//floor.png\n");
}


// ---------------- MAIN ----------------

int main()
{
    // Background animation
    iSetTimer(83, advanceFrame);

    // Floor movement
    iSetTimer(16, updateFloor);

    // Game window
    iInitialize(1280, 720, "Animation");

    loadImages();

    iStart();

    return 0;
}