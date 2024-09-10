#include "../window.h"
#include "modules/ui.c"

#define PADDLE_SPEED 10
#define PADDLE_WIDTH 20
#define PADDLE_HEIGHT 125
#define BALL_SIZE 20
#define BALL_SPEED 12
#define LERP 0.75
#define LERP_ENEMY 0.15

typedef struct {
    float x, y, width, height, speed;
} Paddle;
typedef struct {
    float x, y, size, speedX, speedY;
} Ball;

Font font;
Paddle leftPaddle, rightPaddle;
Ball ball;
int leftScore = 0, rightScore = 0;

void ResetBall() {
    ball.x = window.screen_width / 2 - BALL_SIZE / 2;
    ball.y = window.screen_height / 2 - BALL_SIZE / 2;
    ball.speedX = BALL_SPEED * ((rand() % 2 == 0) ? 1 : -1);
    ball.speedY = BALL_SPEED * ((rand() % 2 == 0) ? 1 : -1);
}

int main(int argc, char** argv) {
    window.fpslimit = 60;
    WindowInit(1920, 1080, "Grafenic - Pong");
    font = LoadFont("./res/fonts/Monocraft.ttf");
    leftPaddle = (Paddle){0,window.screen_height / 2 - PADDLE_HEIGHT / 2, PADDLE_WIDTH, PADDLE_HEIGHT, PADDLE_SPEED};
    rightPaddle = (Paddle){window.screen_width - (PADDLE_WIDTH), window.screen_height / 2 - PADDLE_HEIGHT / 2, PADDLE_WIDTH, PADDLE_HEIGHT, PADDLE_SPEED};
    ball = (Ball){window.screen_width/2,window.screen_height/2, BALL_SIZE, BALL_SPEED, BALL_SPEED};
    while (!WindowState()) {
        WindowClear();
        // Force Screen at 1920x1080
            window.screen_width = 1920;
            window.screen_height = 1080;
        // Enemy Ai
            rightPaddle.y = Lerp(rightPaddle.y, window.screen_height - ball.y - (rightPaddle.height/2), LERP_ENEMY);
        // Move paddles based on user input
            if (isKeyDown("w")) leftPaddle.y = Lerp(leftPaddle.y, (leftPaddle.y - leftPaddle.speed), LERP);
            if (isKeyDown("s")) leftPaddle.y = Lerp(leftPaddle.y, (leftPaddle.y + leftPaddle.speed), LERP);
        // Ensure paddles stay within screen bounds
            leftPaddle.y = fminf(fmaxf(leftPaddle.y, 0), window.screen_height - leftPaddle.height);
            rightPaddle.y = fminf(fmaxf(rightPaddle.y, 0), window.screen_height - rightPaddle.height);
        // Move ball
            ball.x = Lerp(ball.x, (ball.x + ball.speedX), LERP);
            ball.y = Lerp(ball.y, (ball.y + ball.speedY), LERP);
        // Check collision with top and bottom walls
            if (ball.y <= 0 || ball.y >= window.screen_height - ball.size) ball.speedY = -ball.speedY;
        // Check collision with the left paddle
            if ((ball.x - (ball.size / 2) <= leftPaddle.x + (Scaling(leftPaddle.width) / 2)) && 
                IsInside((ball.x + (ball.size / 2)),(window.screen_height - ball.y - (ball.size / 2)),leftPaddle.x, leftPaddle.y, Scaling(leftPaddle.width), Scaling(leftPaddle.height))) {
                ball.speedX = -ball.speedX;
            }
        // Check collision with the right paddle
            if ((ball.x + (ball.size / 2) >= rightPaddle.x - (Scaling(rightPaddle.width) / 2)) && 
                IsInside((ball.x + (ball.size / 2)),(window.screen_height - ball.y - (ball.size / 2)),rightPaddle.x, rightPaddle.y, Scaling(rightPaddle.width), Scaling(rightPaddle.height))) {
                ball.speedX = -ball.speedX;
            }
        // Check if the ball is out of bounds
            if (ball.x < 0) {
                rightScore++; // Increment adversary's score
                ResetBall();
            } 
            if (ball.x > window.screen_width - ball.size) {
                leftScore++; // Increment player's score
                ResetBall();
            }
        // Draw ball
            DrawCircle((ball.x + (ball.size / 2)), (window.screen_height - ball.y - (ball.size / 2)), (ball.size / 2), (Color){255, 255, 255, 255});
        // Draw paddles
            DrawRect(leftPaddle.x, leftPaddle.y, Scaling(leftPaddle.width), Scaling(leftPaddle.height), (Color){255, 255, 255, 255});
            DrawRect(rightPaddle.x, rightPaddle.y, Scaling(rightPaddle.width), Scaling(rightPaddle.height), (Color){255, 255, 255, 255});
        // Draw scores
            const char* scoreText = text("%d - %d", leftScore, rightScore);
            const int fontSize = Scaling(35);
            const TextSize textSize = GetTextSize(font, fontSize, scoreText);
            const int textX = (window.screen_width - textSize.width) / 2;
            const int textY = (window.screen_height - textSize.height);
            DrawText(textX, textY, font, fontSize, scoreText, (Color){255, 255, 255, 255});
        // Modular ui.h functions
            ExitPromt(font);
        WindowProcess();
    }
    WindowClose();
    return 0;
}