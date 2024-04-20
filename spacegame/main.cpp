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
    float throttle;
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

        bullet = new GameObject(2);
        bullet->status = 0;
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
        static const float maxThrottle = 5.0f;

        ship->forX = 0;
        ship->forY = 0;

        if (input.keyUp)
        {
            debugMsg("Up");

            if (ship->throttle < maxThrottle)
            {
                ship->throttle += 0.5;
            }

            ship->forX += ship->throttle * cos(deg2rad(ship->angle));
            ship->forY += ship->throttle * sin(deg2rad(ship->angle));
        }

        if (input.keyLeft)
        {
            debugMsg("Left");

            if (ship->throttle < maxThrottle)
            {
                ship->throttle += 0.5;
            }

            ship->angle += ship->throttle;
        }
        else if (input.keyRight)
        {
            debugMsg("Right");

            if (ship->throttle < maxThrottle)
            {
                ship->throttle += 0.5;
            }

            ship->angle -= ship->throttle;
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
        }

        ship->velX += ship->forX * forceFactor / ship->mass;
        ship->velY += ship->forY * forceFactor / ship->mass;

        ship->posX += ship->velX;
        ship->posY += ship->velY;

        if (input.keySpace)
        {
            shootBullet();
            input.keySpace = 0;
        }

        if (bullet->status)
        {
            bullet->posX += bullet->velX;
            bullet->posY += bullet->velY;
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
    }

    void shootBullet()
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
    void renderShip()
    {
        glPushMatrix();

        glTranslatef(ship->posX, ship->posY, 0.0f);
        glRotatef(ship->angle, 0.0f, 0.0f, 1.0f);

        if (input.keyUp)
        {
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