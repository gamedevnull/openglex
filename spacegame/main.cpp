#include <iostream>
#include <SDL2/SDL.h>
#include <GL/glew.h>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

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

    SDL_Window *window;
    SDL_GLContext context;

    GameObject *ship;
    GameObject *bullet;
    std::vector<GameObject *> targets;

    int score;

    SpaceGame()
    {
        isDebug = 1;
        isRunning = 0;

        window = nullptr;
        context = nullptr;

        ship = new GameObject(1);
        ship->posX = 400.0f;
        ship->posY = 300.0f;
        ship->angle = 0.0f;
        ship->velX = 0.0f;
        ship->velY = 0.0f;
        ship->mass = 1.0f;
        ship->throttle = 0;
        ship->rotationThrottle = 0;

        bullet = new GameObject(2);
        bullet->status = 0;

        spawnAsteroid();

        score = 0;
    }

    void spawnAsteroid()
    {
        GameObject *asteroid;
        asteroid = new GameObject(3);
        asteroid->status = 1;
        asteroid->posX = (float)(rand() % 800);
        asteroid->posY = (float)(rand() % 600);
        asteroid->velX = ((float)(rand() % 300)) / 100.0f;
        asteroid->velY = ((float)(rand() % 300)) / 100.0f;
        targets.push_back(asteroid);
    }

    void spawnMoreAsteroids()
    {
        int currentAsteroidsCount = targets.size();
        int maxAsteroidsCount = 1;
        if (score <= 2)
        {
            maxAsteroidsCount = 1;
        }
        else if (score > 2 and score <= 4)
        {
            maxAsteroidsCount = 2;
        }
        else if (score > 4 and score <= 6)
        {
            maxAsteroidsCount = 4;
        }
        else
        {
            maxAsteroidsCount = 6;
        }
        std::cout << "spawning" << maxAsteroidsCount << std::endl;
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
                    std::cout << "SDL_KEYDOWN for : " << event.key.keysym.sym << std::endl;
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
                    std::cout << "SDL_KEYUP for : " << event.key.keysym.sym << std::endl;
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

        // targets out of screen

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

        if (bullet->status)
        {
            for (std::vector<GameObject *>::iterator it = targets.begin(); it != targets.end(); ++it)
            {
                if ((*it)->status)
                {
                    if ((bullet->posX < (*it)->posX + 20.0f) &&
                        (bullet->posX > (*it)->posX - 20.0f) &&
                        (bullet->posY < (*it)->posY + 20.0f) &&
                        (bullet->posY > (*it)->posY - 20.0f))
                    {
                        bullet->status = 0;
                        (*it)->status = 0;
                        score++;
                        debugMsg("score!");
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

        renderBullet();

        renderShip();

        renderAsteroids();

        SDL_GL_SwapWindow(window);
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

                float rockSize = 12.0f;

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

        float shipSize = 20.0f;

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