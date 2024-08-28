#include <grrlib.h>
#include <stdlib.h>
#include <vector>
#include <wiiuse/wpad.h>
#include "BMfont2_png.h"

#define BLACK 0x000000FF
#define WHITE 0xFFFFFFFF
#define RED 0xFF0000FF
#define TEAL 0x008080FF

typedef struct
{
    float x;
    float y;
    float w;
    float h;
    unsigned int color;
    bool isDestroyed;
} Rectangle;

std::vector<Rectangle> createBricks()
{
    std::vector<Rectangle> bricks;

    int positionX;
    int positionY = 60;

    for (int row = 0; row < 8; row++)
    {
        positionX = 0;

        for (int column = 0; column < 15; column++)
        {
            unsigned int color = RED;

            if (row % 2 == 0)
            {
                color = TEAL;
            }

            Rectangle actualBrick = {(float)positionX, (float)positionY, 41, 16, color, false};

            bricks.push_back(actualBrick);
            positionX += 43;
        }

        positionY += 18;
    }

    return bricks;
}

bool hasCollision(Rectangle bounds, Rectangle ball)
{
    return bounds.x < ball.x + ball.w && bounds.x + bounds.w > ball.x &&
           bounds.y < ball.y + ball.h && bounds.y + bounds.h > ball.y;
}

int main(int argc, char **argv)
{
    const int SCREEN_WIDTH = 640;
    const int SCREEN_HEIGHT = 480;

    bool isAutoPlayMode = false;

    std::vector<Rectangle> bricks = createBricks();

    Rectangle player = {SCREEN_WIDTH / 2, SCREEN_HEIGHT - 16, 42, 16, WHITE};

    Rectangle ball = {SCREEN_WIDTH / 2 - 16, SCREEN_HEIGHT / 2 - 16, 16, 16, WHITE};

    const int playerSpeed = 6;

    int ballVelocityX = 4;
    int ballVelocityY = 4;

    GRRLIB_Init();
    WPAD_Init();

    GRRLIB_texImg *tex_BMfont2 = GRRLIB_LoadTexture(BMfont2_png);
    GRRLIB_InitTileSet(tex_BMfont2, 16, 16, 32);

    while (1)
    {
        WPAD_SetVRes(0, SCREEN_WIDTH, SCREEN_HEIGHT);
        WPAD_ScanPads();

        const u32 padDown = WPAD_ButtonsDown(0);
        const u32 padHeld = WPAD_ButtonsHeld(0);

        GRRLIB_FillScreen(BLACK);

        GRRLIB_Printf(300, 25, tex_BMfont2, WHITE, 1, "DEMO");

        if (padDown & WPAD_BUTTON_HOME)
        {
            break;
        }

        if (padDown & WPAD_BUTTON_A)
        {
            isAutoPlayMode = !isAutoPlayMode;
        }

        if (isAutoPlayMode && ball.x < SCREEN_WIDTH - player.w)
        {
            player.x = ball.x;
        }

        if (padHeld & WPAD_BUTTON_LEFT && player.x > 0)
        {
            player.x -= playerSpeed;
        }

        else if (padHeld & WPAD_BUTTON_RIGHT && player.x < SCREEN_WIDTH - player.w)
        {
            player.x += playerSpeed;
        }

        if (ball.y > SCREEN_HEIGHT + ball.h)
        {
            ball.x = SCREEN_WIDTH / 2 - ball.w;
            ball.y = SCREEN_HEIGHT / 2 - ball.h;

            ballVelocityX *= -1;
        }

        if (ball.x < 0 || ball.x > SCREEN_WIDTH - ball.w)
        {
            ballVelocityX *= -1;
        }

        if (hasCollision(player, ball) || ball.y < 0)
        {
            ballVelocityY *= -1;
        }

        for (Rectangle &brick : bricks)
        {
            if (!brick.isDestroyed && hasCollision(brick, ball))
            {
                ballVelocityY *= -1;
                brick.isDestroyed = true;
            }
        }

        ball.x += ballVelocityX;
        ball.y += ballVelocityY;

        for (Rectangle brick : bricks)
        {
            if (!brick.isDestroyed)
            {
                GRRLIB_Rectangle(brick.x, brick.y, brick.w, brick.h, brick.color, 1);
            }
        }

        GRRLIB_Rectangle(ball.x, ball.y, ball.w, ball.h, ball.color, 1);
        GRRLIB_Rectangle(player.x, player.y, player.w, player.h, player.color, 1);

        GRRLIB_Render();
    }

    GRRLIB_FreeTexture(tex_BMfont2);
    GRRLIB_Exit();
    exit(0);
}
