#include <iostream>
#include "raylib.h"
#include "raymath.h"
#include <deque>
#include <raymath.h>

Color green = {173, 204, 96, 255};
Color darkGreen = {43, 51, 24, 255};

int cellSize = 30;
int cellCount = 25;
int offset = 75;

double lastUpadateTime = 0;

bool elementInDeque(Vector2 element, std::deque<Vector2> deque)
{
    for (int i = 0; i < deque.size(); i++)
    {
        if (Vector2Equals(deque[i], element))
        {
            return true;
        }
    }
    return false;
}

bool eventTriggered(double interval)
{
    double currentTime = GetTime();
    if (currentTime - lastUpadateTime > interval)
    {
        lastUpadateTime = currentTime;
        return true;
    }
    else
    {
        return false;
    }
}

class Snake
{
public:
    std::deque<Vector2> body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
    Vector2 direction = {1, 0};
    bool addSegment = false;

    void Draw()
    {
        for (int i = 0; i < body.size(); i++)
        {
            int x = body[i].x;
            int y = body[i].y;
            Rectangle part = Rectangle{(float)offset + x * cellSize, (float)offset + y * cellSize, (float)cellSize, (float)cellSize};
            DrawRectangleRounded(part, 0.5, 6, darkGreen);
        }
    }
    void Update()
    {
        body.push_front(Vector2Add(body[0], direction));
        if (addSegment == true)
        {
            addSegment = false;
        }
        else
        {
            body.pop_back();
        }
    }
    void reset()
    {
        body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
        direction = {1, 0};
    }
};

class Food
{

public:
    Vector2 position;
    Texture2D texture;
    Food(std::deque<Vector2> snakeBody)
    {
        Image image = LoadImage("Graphics/apple.png");
        texture = LoadTextureFromImage(image);
        texture.width = 35;
        texture.height = 35;
        position = GenerateRandomPos(snakeBody);

        // da oslobodim memoriju
        UnloadImage(image);
    }
    ~Food()
    {
        UnloadTexture(texture);
    }
    Vector2 generateRandomCell()
    {
        float x = GetRandomValue(0, cellCount - 1);
        float y = GetRandomValue(0, cellCount - 1);
        return {x, y};
    }
    Vector2 GenerateRandomPos(std::deque<Vector2> snakeBody)
    {

        Vector2 position = generateRandomCell();
        while (elementInDeque(position, snakeBody))
        {
            position = generateRandomCell();
        }
        return position;
    }
    void draw()
    {
        DrawTexture(texture, offset + position.x * cellSize, offset + position.y * cellSize, WHITE);
    }
};

class Game
{
public:
    Snake snake = Snake();
    Food food = Food(snake.body);
    bool running = true;
    int score = 0;
    void Draw()
    {
        snake.Draw();
        food.draw();
    };
    void Update()
    {
        if (running)
        {

            snake.Update();
            checkCollisionWithTheFood();
            CheckCollisionWithTheEdges();
            CheckCollisionWithTail();
        }
    };
    void checkCollisionWithTheFood()
    {
        if (Vector2Equals(snake.body[0], food.position))
        {
            // add bodycell to the snake
            snake.addSegment = true;
            // generate new food obj
            food.position = food.GenerateRandomPos(snake.body);
            score++;
        }
    };
    void CheckCollisionWithTheEdges()
    {
        if (snake.body[0].x == cellCount || snake.body[0].x == -1)
        {
            gameOver();
        }
        if (snake.body[0].y == cellCount || snake.body[0].y == -1)
        {
            gameOver();
        }
    }
    void CheckCollisionWithTail()
    {
        std::deque<Vector2> headlessBody = snake.body;
        headlessBody.pop_front();
        if (elementInDeque(snake.body[0], headlessBody))
        {
            gameOver();
        }
    }
    void gameOver()
    {
        snake.reset();
        food.position = food.GenerateRandomPos(snake.body);
        running = false;
        score = 0;
    }
};

int main(void)
{
    InitWindow(2 * offset + cellCount * cellSize, 2 * offset + cellCount * cellSize, "Snake Game");
    SetTargetFPS(60);
    Game game = Game();
    // windowshouldclose() returns false and true - ako kliknemo x
    while (!WindowShouldClose())
    {
        BeginDrawing();

        // calling a function on every interval
        if (eventTriggered(0.2))
        {
            game.Update();
        }
        if (IsKeyPressed(KEY_DOWN) && game.snake.direction.y != -1)
        {
            game.running = true;
            game.snake.direction = {0, 1};
        }
        if (IsKeyPressed(KEY_UP) && game.snake.direction.y != 1)
        {
            game.running = true;
            game.snake.direction = {0, -1};
        }
        if (IsKeyPressed(KEY_LEFT) && game.snake.direction.x != 1)
        {
            game.running = true;
            game.snake.direction = {-1, 0};
        }
        if (IsKeyPressed(KEY_RIGHT) && game.snake.direction.x != -1)
        {
            game.running = true;
            game.snake.direction = {1, 0};
        }

        // Drawing
        ClearBackground(green);
        DrawRectangleLinesEx(Rectangle{
                                 (float)offset - 5,
                                 (float)offset - 5,
                                 (float)cellSize * cellCount + 10,
                                 (float)cellSize * cellCount + 10,
                             },
                             5, darkGreen);
        DrawText("Retro Snake", offset - 5, 20, 40, darkGreen);

        DrawText(TextFormat("Score: %d", game.score), offset - 5, offset + 10 + cellSize * cellCount, 40, darkGreen);
        game.Draw();

        EndDrawing();
    }
    CloseWindow();
    return 0;
}
