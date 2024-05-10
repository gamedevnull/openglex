#include <iostream>
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include "include/font.hpp"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

class GameState
{
public:
    int currentState;
    GameState()
    {
        currentState = 0;
    }

    void setState(int newState)
    {
        currentState = newState;
    }

    bool isInState(int stateToCheck)
    {
        return stateToCheck == currentState;
    }
};

class GameInput
{
public:
    bool keyUp, keyDown, keyLeft, keyRight, keySpace;
    GameInput() : keyUp(0), keyDown(0), keyLeft(0), keyRight(0), keySpace(0) {}
};

class GameObject
{
public:
    float posX, posY;
    float angle;
    float velX, velY;
    float forX, forY;
    float throttle, rotationThrottle;
    float mass;
    float size;
    char status;
    char ObjectId;
    GameObject(char id)
    {
        ObjectId = id;
    }
};

class SpaceGame
{
public:
    bool isRunning, isDebug;

    GameInput input;
    GameState stateController;

    enum GameStateID
    {
        TITLE,
        PLAYING,
        GAME_OVER
    };

    SDL_Window *window;
    SDL_GLContext context;

    GameObject *ship;
    GameObject *bullet;
    std::vector<GameObject *> targets;

    FontRenderer *fontRenderer;

    int score;
    int level;

    bool allowScreenBounce;

    SpaceGame()
    {
        srand(time(0));

        isDebug = 0;
        isRunning = 0;
        allowScreenBounce = 0;

        window = nullptr;
        context = nullptr;

        ship = new GameObject(1);
        ship->posX = 400.0f;
        ship->posY = 100.0f;
        ship->angle = 0.0f;
        ship->velX = 0.0f;
        ship->velY = 0.0f;
        ship->mass = 1.0f;
        ship->size = 20.0f;
        ship->status = 1;
        ship->throttle = 0;
        ship->rotationThrottle = 0;

        bullet = new GameObject(2);
        bullet->status = 0;

        spawnAsteroid();

        score = 0;
        level = 1;

        fontRenderer = new FontRenderer();

        stateController.setState(PLAYING);
    }

    void spawnAsteroid()
    {
        GameObject *asteroid;
        asteroid = new GameObject(3);
        asteroid->status = 1;
        asteroid->posX = (float)(rand() % 800);
        asteroid->posY = -1 * (float)(rand() % 600);
        asteroid->velX = ((float)(rand() % 300)) / 100.0f;
        asteroid->velY = ((float)(rand() % 300)) / 100.0f;

        int size = rand() % 5;
        switch (size)
        {
        case 0:
            asteroid->size = 10;
            break;
        case 1:
            asteroid->size = 15;
            break;
        case 2:
            asteroid->size = 20;
            break;
        case 3:
            asteroid->size = 25;
            break;
        case 4:
            asteroid->size = 30;
            break;
        default:
            break;
        }
        targets.push_back(asteroid);
    }

    void spawnMoreAsteroids()
    {
        int currentAsteroidsCount = targets.size();
        int maxAsteroidsCount = 1;
        if (score <= 3)
        {
            maxAsteroidsCount = 1;
            level = 1;
        }
        else if (score > 3 and score <= 6)
        {
            maxAsteroidsCount = 2;
            level = 2;
        }
        else if (score > 6 and score <= 10)
        {
            maxAsteroidsCount = 4;
            level = 3;
        }
        else if (score > 10)
        {
            maxAsteroidsCount = 6;
            level = 4;
        }

        for (int i = currentAsteroidsCount; i < maxAsteroidsCount; i++)
        {
            spawnAsteroid();
        }
    }

    void run()
    {
        debugMsg("Starting...");

        if (SDL_Init(SDL_INIT_VIDEO) < 0)
        {
            debugMsg("SDL init problem");
            return;
        }

        window = SDL_CreateWindow("Space Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
        if (!window)
        {
            debugMsg("SDL window create problem");
            SDL_Quit();
            return;
        }

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

        context = SDL_GL_CreateContext(window);
        if (!context)
        {
            debugMsg("OpenGL context create problem");
            SDL_DestroyWindow(window);
            SDL_Quit();
            return;
        }

        fontRenderer->createPrintableAsciiCharsTexturesFromPng("pixfont.png", 12, 16);

        isRunning = 1;
        while (isRunning)
        {
            WaitFrame(60);
            ProcessEvents();
            Update();
            Render();
        }
    }

    void WaitFrame(int fps)
    {
        static int nextTick = 0;
        int thisTick;

        thisTick = SDL_GetTicks();
        if (thisTick < nextTick)
        {
            SDL_Delay(nextTick - thisTick);
        }
        nextTick = SDL_GetTicks() + (1000 / fps);
    }

    void ProcessEvents()
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                isRunning = 0;
                break;

            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                case SDLK_ESCAPE:
                    isRunning = 0;
                    break;
                case SDLK_UP:
                    input.keyUp = 1;
                    break;
                case SDLK_DOWN:
                    input.keyDown = 1;
                    break;
                case SDLK_LEFT:
                    input.keyLeft = 1;
                    break;
                case SDLK_RIGHT:
                    input.keyRight = 1;
                    break;
                case SDLK_SPACE:
                    input.keySpace = 1;
                    break;
                default:
                    // std::cout << "SDL_KEYDOWN for : " << event.key.keysym.sym << std::endl;
                    break;
                }
                break;

            case SDL_KEYUP:
                switch (event.key.keysym.sym)
                {
                case SDLK_ESCAPE:
                    isRunning = 0;
                    break;
                case SDLK_UP:
                    input.keyUp = 0;
                    break;
                case SDLK_DOWN:
                    input.keyDown = 0;
                    break;
                case SDLK_LEFT:
                    input.keyLeft = 0;
                    break;
                case SDLK_RIGHT:
                    input.keyRight = 0;
                    break;
                case SDLK_SPACE:
                    input.keySpace = 0;
                    break;
                default:
                    // std::cout << "SDL_KEYUP for : " << event.key.keysym.sym << std::endl;
                    break;
                }
                break;
            default:
                break;
            }
        }
    }

    float deg2rad(float deg)
    {
        return deg * M_PI / 180.0f;
    }

    float rad2deg(float rad)
    {
        return rad * 180.0f / M_PI;
    }

    void Update()
    {

        if (stateController.isInState(PLAYING))
        {

            static const float forceFactor = 0.02f;
            static const float maxMainThrottle = 5.0f;
            static const float maxRotationThrottle = 3.0f;

            ship->forX = 0;
            ship->forY = 0;

            // move forward

            if (input.keyUp)
            {
                debugMsg("Up");

                if (ship->throttle < maxMainThrottle)
                {
                    ship->throttle += 0.5;
                }

                ship->forX += ship->throttle * cos(deg2rad(ship->angle));
                ship->forY += ship->throttle * sin(deg2rad(ship->angle));
            }

            // change angle

            if (input.keyLeft)
            {
                debugMsg("Left");

                if (ship->rotationThrottle < maxRotationThrottle)
                {
                    ship->rotationThrottle += 0.05;
                }

                ship->angle += ship->rotationThrottle;
            }
            else if (input.keyRight)
            {
                debugMsg("Right");

                if (ship->rotationThrottle < maxRotationThrottle)
                {
                    ship->rotationThrottle += 0.05;
                }

                ship->angle -= ship->rotationThrottle;
            }
            else
            {
                if (ship->throttle > 0)
                {
                    ship->throttle -= 0.2;
                    if (ship->throttle < 0)
                    {
                        ship->throttle = 0;
                    }
                }

                if (ship->rotationThrottle > 0)
                {
                    ship->rotationThrottle -= 0.1;
                    if (ship->rotationThrottle < 0)
                    {
                        ship->rotationThrottle = 0;
                    }
                }
            }

            ship->velX += ship->forX * forceFactor / ship->mass;

            if (ship->velX > 3)
            {
                ship->velX = 3;
            }
            else if (ship->velX < -3)
            {
                ship->velX = -3;
            }

            ship->velY += ship->forY * forceFactor / ship->mass;

            if (ship->velY > 3)
            {
                ship->velY = 3;
            }
            else if (ship->velY < -3)
            {
                ship->velY = -3;
            }

            ship->posX += ship->velX;
            ship->posY += ship->velY;

            // shot

            if (input.keySpace)
            {
                shotBullet();
                input.keySpace = 0;
            }

            // move bullet

            if (bullet->status)
            {
                bullet->posX += bullet->velX;
                bullet->posY += bullet->velY;
            }

            // move targets

            for (std::vector<GameObject *>::iterator it = targets.begin(); it != targets.end(); ++it)
            {
                if ((*it)->status)
                {
                    (*it)->posX += (*it)->velX;
                    (*it)->posY += (*it)->velY;
                }
            }

            // bullet out of screen

            if ((bullet->posX > 800.0f) ||
                (bullet->posX < 0.0f) ||
                (bullet->posY > 600.0f) ||
                (bullet->posY < 0.0f))
            {
                bullet->status = 0;
            }

            // ship out of screen

            if (allowScreenBounce)
            {

                if (ship->posX > 780.0f)
                {
                    ship->posX += 2 * (780.0f - ship->posX);
                    ship->velX = -ship->velX;
                }
                if (ship->posX < 20.0f)
                {
                    ship->posX += 2 * (20.0f - ship->posX);
                    ship->velX = -ship->velX;
                }
                if (ship->posY > 580.0f)
                {
                    ship->posY += 2 * (580.0f - ship->posY);
                    ship->velY = -ship->velY;
                }
                if (ship->posY < 20.0f)
                {
                    ship->posY += 2 * (20.0f - ship->posY);
                    ship->velY = -ship->velY;
                }
            }
            else
            {

                if (ship->posX > 800.0f)
                {
                    ship->posX = 0;
                }
                if (ship->posX < 0.0f)
                {
                    ship->posX = 800;
                }
                if (ship->posY > 600.0f)
                {
                    ship->posY = 0;
                }
                if (ship->posY < 0.0f)
                {
                    ship->posY = 600.0f;
                }
            }

            // targets out of screen

            if (allowScreenBounce)
            {
                for (std::vector<GameObject *>::iterator it = targets.begin(); it != targets.end(); ++it)
                {
                    if ((*it)->status)
                    {
                        if ((*it)->posX > 780.0f)
                        {
                            (*it)->posX += 2 * (780.0f - (*it)->posX);
                            (*it)->velX = -(*it)->velX;
                        }
                        if ((*it)->posX < 20.0f)
                        {
                            (*it)->posX += 2 * (20.0f - (*it)->posX);
                            (*it)->velX = -(*it)->velX;
                        }
                        if ((*it)->posY > 580.0f)
                        {
                            (*it)->posY += 2 * (580.0f - (*it)->posY);
                            (*it)->velY = -(*it)->velY;
                        }
                        if ((*it)->posY < 20.0f)
                        {
                            (*it)->posY += 2 * (20.0f - (*it)->posY);
                            (*it)->velY = -(*it)->velY;
                        }
                    }
                }
            }
            else
            {
                for (std::vector<GameObject *>::iterator it = targets.begin(); it != targets.end(); ++it)
                {
                    if ((*it)->status)
                    {
                        if ((*it)->posX > 800.0f)
                        {
                            (*it)->posX = 0;
                        }
                        if ((*it)->posX < 0.0f)
                        {
                            (*it)->posX = 800;
                        }
                        if ((*it)->posY > 600.0f)
                        {
                            (*it)->posY = 0;
                        }
                        if ((*it)->posY < 0.0f)
                        {
                            (*it)->posY = 800;
                        }
                    }
                }
            }

            // bullet vs asteroid

            if (bullet->status)
            {
                for (std::vector<GameObject *>::iterator it = targets.begin(); it != targets.end(); ++it)
                {
                    if ((*it)->status)
                    {
                        if ((bullet->posX + bullet->size >= (*it)->posX - (*it)->size) &&
                            (bullet->posX - bullet->size <= (*it)->posX + (*it)->size) &&
                            (bullet->posY + bullet->size >= (*it)->posY - (*it)->size) &&
                            (bullet->posY - bullet->size <= (*it)->posY + (*it)->size))
                        {
                            bullet->status = 0;
                            (*it)->status = 0;
                            score++;
                            debugMsg("score!");
                        }
                    }
                }
            }

            // ship vs asteroid

            if (ship->status)
            {
                for (std::vector<GameObject *>::iterator it = targets.begin(); it != targets.end(); ++it)
                {
                    if ((*it)->status)
                    {
                        if ((ship->posX + ship->size >= (*it)->posX - (*it)->size) &&
                            (ship->posX - ship->size <= (*it)->posX + (*it)->size) &&
                            (ship->posY + ship->size >= (*it)->posY - (*it)->size) &&
                            (ship->posY - ship->size <= (*it)->posY + (*it)->size))
                        {
                            ship->status = 0;
                            (*it)->status = 0;

                            debugMsg("game over!");
                            stateController.setState(GAME_OVER);
                        }
                    }
                }
            }

            for (std::vector<GameObject *>::iterator it = targets.begin(); it != targets.end(); ++it)
            {
                if (!(*it)->status)
                {
                    delete *it;
                    targets.erase(it);
                    spawnMoreAsteroids();
                    break;
                }
            }
        }
        else if (stateController.isInState(GAME_OVER))
        {

            if (input.keySpace)
            {
                stateController.setState(PLAYING);
                input.keySpace = 0;
                ship->status = 1;
                score = 0;
                level = 1;
                ship->posX = 400.0f;
                ship->posY = 100.0f;
                ship->angle = 0.0f;
                ship->velX = 0.0f;
                ship->velY = 0.0f;
                ship->mass = 1.0f;
                ship->status = 1;
                ship->throttle = 0;
                ship->rotationThrottle = 0;
            }
        }
    }

    void shotBullet()
    {
        if (!bullet->status)
        {
            bullet->status = 1;
            bullet->posX = ship->posX;
            bullet->posY = ship->posY;

            bullet->velX = 10.0f * cos(deg2rad(ship->angle));
            bullet->velY = 10.0f * sin(deg2rad(ship->angle));
        }
        return;
    }

    void Render()
    {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0.0f, SCREEN_WIDTH, 0.0f, SCREEN_HEIGHT, -1.0f, 1.0f);

        glClear(GL_COLOR_BUFFER_BIT);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        if (stateController.isInState(PLAYING))
        {

            renderBullet();

            renderShip();

            renderAsteroids();

            renderScore();
            renderLevel();
        }
        else if (stateController.isInState(GAME_OVER))
        {

            renderCentredText("GAME OVER");
        }

        SDL_GL_SwapWindow(window);
    }

    void renderCentredText(const char *text)
    {
        glPushMatrix();
        glTranslatef(0, 0, 0.0f);
        glColor3f(0.0f, 1.0f, 0.0f);

        fontRenderer->renderText(text, 330, 240);

        glPopMatrix();
    }

    void renderScore()
    {
        glPushMatrix();
        glTranslatef(0, 0, 0.0f);
        glColor3f(0.0f, 1.0f, 0.0f);

        fontRenderer->renderText("Score: ", 330, 540);
        fontRenderer->renderInt(score, 410, 540);

        glPopMatrix();
    }

    void renderLevel()
    {
        glPushMatrix();
        glTranslatef(0, 0, 0.0f);
        glColor3f(0.0f, 1.0f, 0.0f);

        fontRenderer->renderText("Level: ", 630, 540);
        fontRenderer->renderInt(level, 710, 540);

        glPopMatrix();
    }

    void renderBullet()
    {
        if (bullet->status)
        {
            glPushMatrix();
            glTranslatef(bullet->posX, bullet->posY, 0.0f);

            glColor3f(0.0f, 1.0f, 0.0f);
            glBegin(GL_TRIANGLE_STRIP);
            {
                glVertex2f(0.0f - 2.0f, 0.0f - 2.0f);
                glVertex2f(0.0f + 2.0f, 0.0f - 2.0f);
                glVertex2f(0.0f + 2.0f, 0.0f + 2.0f);
                glVertex2f(0.0f - 2.0f, 0.0f + 2.0f);
                glVertex2f(0.0f - 2.0f, 0.0f - 2.0f);
            }
            glEnd();
            glPopMatrix();
        }
    }

    void renderAsteroids()
    {
        for (std::vector<GameObject *>::iterator it = targets.begin(); it != targets.end(); ++it)
        {
            if ((*it)->status)
            {
                glPushMatrix();

                glTranslatef((*it)->posX, (*it)->posY, 0.0f);
                glRotatef((*it)->angle, 0.0f, 0.0f, 1.0f);
                glColor3f(0.0f, 1.0f, 1.0f);

                float rockSize = (*it)->size;

                glBegin(GL_TRIANGLE_STRIP);
                {
                    // asteroid body
                    glVertex2f(0.0f - rockSize, 0.0f - rockSize);
                    glVertex2f(0.0f + rockSize, 0.0f - rockSize);
                    glVertex2f(0.0f + rockSize, 0.0f + rockSize);
                    glVertex2f(0.0f - rockSize, 0.0f + rockSize);
                    glVertex2f(0.0f - rockSize, 0.0f - rockSize);
                }
                glEnd();

                glPopMatrix();
            }
        }
    }

    void renderShip()
    {
        glPushMatrix();

        glTranslatef(ship->posX, ship->posY, 0.0f);
        glRotatef(ship->angle, 0.0f, 0.0f, 1.0f);

        if (input.keyUp)
        {
            // spaceship throttle
            glColor3f(1.0f, 0.0f, 0.0f);
            glBegin(GL_LINES);
            {
                glVertex2f(0.0f, 0.0f);
                glVertex2f(-50.0f, 0.0f);
            }
            glEnd();
        }

        glColor3f(1.0f, 1.0f, 1.0f);

        float shipSize = ship->size;

        glBegin(GL_TRIANGLE_STRIP);
        {
            // spaceship body
            glVertex2f(0.0f - shipSize, 0.0f - shipSize);
            glVertex2f(0.0f + shipSize, 0.0f - shipSize);
            glVertex2f(0.0f + shipSize, 0.0f + shipSize);
            glVertex2f(0.0f - shipSize, 0.0f + shipSize);
            glVertex2f(0.0f - shipSize, 0.0f - shipSize);

            // spaceship head
            glVertex2f(0.0f + shipSize, 0.0f + shipSize);
            glVertex2f(0.0f + shipSize * 1.5f, 0.0f);
            glVertex2f(0.0f + shipSize, 0.0f - shipSize);
        }
        glEnd();

        glPopMatrix();
    }

    ~SpaceGame()
    {
        delete ship;
        delete bullet;

        if (context)
            SDL_GL_DeleteContext(context);
        if (window)
            SDL_DestroyWindow(window);
        SDL_Quit();
    }

    void debugMsg(const char *message)
    {
        if (isDebug)
        {
            std::cout << message << std::endl;
        }
    }
};

int main()
{
    SpaceGame game;
    game.run();
    return 0;
}