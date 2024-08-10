#include <grrlib.h>
#include <stdlib.h>
#include <wiiuse/wpad.h>
#include "BMfont2_png.h"

#define GRRLIB_BLACK 0x000000FF
#define GRRLIB_WHITE 0xFFFFFFFF
#define GRRLIB_TEAL 0x008080FF
#define GRRLIB_RED 0xFF0000FF

const unsigned int BRICKS_SIZE = 120;

typedef struct
{
    float x;
    float y;
    float w;
    float h;
    _Bool isDestroyed;
    unsigned int color;
} Rectangle;

_Bool hasCollision(Rectangle bounds, Rectangle ball)
{
    return bounds.x < ball.x + ball.w && bounds.x + bounds.w > ball.x &&
           bounds.y < ball.y + ball.h && bounds.y + bounds.h > ball.y;
}

int main(int argc, char **argv)
{
    const int SCREEN_WIDTH = 640;
    const int SCREEN_HEIGHT = 480;

    _Bool isAutoPlayMode = false;

    Rectangle bricks[BRICKS_SIZE];

    int positionX;
    int positionY = 60;

    int initialIndex = 0;
    int actualLenght = 15;

    for (int i = 0; i < 8; i++)
    {
        positionX = 2;

        for (int j = initialIndex; j < actualLenght; j++)
        {
            unsigned int color = GRRLIB_RED;

            if (i % 2 == 0)
            {
                color = GRRLIB_TEAL;
            }

            Rectangle actualBrick = {positionX, positionY, 41, 16, 0, color};

            bricks[j] = actualBrick;

            positionX += 43;
        }

        initialIndex += 15;
        actualLenght += 15;

        positionY += 18;
    }

    Rectangle player = {SCREEN_WIDTH / 2, SCREEN_HEIGHT - 16, 42, 16};

    Rectangle ball = {SCREEN_WIDTH / 2 - 16, SCREEN_HEIGHT / 2 - 16, 16, 16};

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

        const u32 wpaddown = WPAD_ButtonsDown(0);
        const u32 wpadheld = WPAD_ButtonsHeld(0);

        GRRLIB_FillScreen(GRRLIB_BLACK);

        GRRLIB_Printf(300, 25, tex_BMfont2, GRRLIB_WHITE, 1, "DEMO");

        if (wpaddown & WPAD_BUTTON_HOME)
        {
            break;
        }

        if (wpaddown & WPAD_BUTTON_A)
        {
            isAutoPlayMode = !isAutoPlayMode;
        }

        if (isAutoPlayMode && ball.x < SCREEN_WIDTH - player.w)
        {
            player.x = ball.x;
        }

        if (wpadheld & WPAD_BUTTON_LEFT && player.x > 0)
        {
            player.x -= playerSpeed;
        }

        else if (wpadheld & WPAD_BUTTON_RIGHT && player.x < SCREEN_WIDTH - player.w)
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

        for (size_t i = 0; i < BRICKS_SIZE; i++)
        {
            if (!bricks[i].isDestroyed && hasCollision(bricks[i], ball))
            {
                ballVelocityY *= -1;
                bricks[i].isDestroyed = 1;
                break;
            }
        }

        ball.x += ballVelocityX;
        ball.y += ballVelocityY;

        for (size_t i = 0; i < BRICKS_SIZE; i++)
        {
            if (!bricks[i].isDestroyed)
            {
                GRRLIB_Rectangle(bricks[i].x, bricks[i].y, bricks[i].w, bricks[i].h, bricks[i].color, 1);
            }
        }

        GRRLIB_Rectangle(ball.x, ball.y, ball.w, ball.h, GRRLIB_WHITE, 1);
        GRRLIB_Rectangle(player.x, player.y, player.w, player.h, GRRLIB_WHITE, 1);

        GRRLIB_Render();
    }

    GRRLIB_Exit(); 

    exit(0); 
}
