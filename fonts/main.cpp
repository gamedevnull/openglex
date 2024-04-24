#include <iostream>
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

const int SCREEN_WIDTH = 320;
const int SCREEN_HEIGHT = 240;

class FontApp
{
public:
    bool isRunning;
    SDL_Window *window;
    SDL_GLContext context;
    GLuint fontTexture;

    FontApp()
    {
        isRunning = 0;
        window = nullptr;
        context = nullptr;
        fontTexture = 0;
    }

    void run()
    {
        if (SDL_Init(SDL_INIT_VIDEO) < 0)
        {
            std::cout << "SDL init problem" << std::endl;
            return;
        }

        window = SDL_CreateWindow("Text demo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
        if (!window)
        {
            std::cout << "SDL window create problem" << std::endl;
            SDL_Quit();
            return;
        }

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

        context = SDL_GL_CreateContext(window);
        if (!context)
        {
            std::cout << "OpenGL context create problem" << std::endl;
            SDL_DestroyWindow(window);
            SDL_Quit();
            return;
        }

        fontTexture = loadTexture("pixfont.png"); // https://opengameart.org/content/16x12-terminal-bitmap-font

        isRunning = 1;
        while (isRunning)
        {
            WaitFrame(60);
            ProcessEvents();
            Update();
            Render();
        }
    }

    GLuint loadTexture(const char *filename)
    {
        SDL_Surface *surface = IMG_Load(filename);

        SDL_Surface *newSurface = SDL_CreateRGBSurfaceWithFormat(0, surface->w, surface->h, 32, SDL_PIXELFORMAT_RGBA32);
        if (!newSurface)
        {
            std::cerr << "Ccreate new surface problem" << std::endl;
            SDL_FreeSurface(surface);
            return 0;
        }

        Uint32 p32, *buf32;
        Uint8 r, g, b, a;
        
        for (int y = 0; y < surface->h; y++)
        {
            for (int x = 0; x < surface->w; x++)
            {
                buf32 = (Uint32 *)surface->pixels;
                p32 = buf32[y * surface->w + x];

                SDL_GetRGBA(p32, surface->format, &r, &g, &b, &a);

                if (r == 0 && g == 0 && b == 0)
                {
                    a = 0;
                }

                Uint8 *pixelPtr = static_cast<Uint8 *>(newSurface->pixels) + y * newSurface->pitch + x * newSurface->format->BytesPerPixel;
                Uint32 *pixel = reinterpret_cast<Uint32 *>(pixelPtr);
                *pixel = SDL_MapRGBA(newSurface->format, r, g, b, a);
            }
        }

        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, newSurface->w, newSurface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, newSurface->pixels);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        SDL_FreeSurface(newSurface);
        SDL_FreeSurface(surface);

        return textureID;
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

    void Update()
    {
    }

    void Render()
    {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0.0f, SCREEN_WIDTH, 0.0f, SCREEN_HEIGHT, -1.0f, 1.0f);

        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

        glClear(GL_COLOR_BUFFER_BIT);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        renderText("Test");

        SDL_GL_SwapWindow(window);
    }

    void renderText(const char *text)
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, fontTexture);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glBegin(GL_TRIANGLES);
        glTexCoord2f(0, 1);
        glVertex2f(100, 100);

        glTexCoord2f(1, 1);
        glVertex2f(292, 100);

        glTexCoord2f(1, 0);
        glVertex2f(292, 196);

        glTexCoord2f(1, 0);
        glVertex2f(292, 196);

        glTexCoord2f(0, 0);
        glVertex2f(100, 196);

        glTexCoord2f(0, 1);
        glVertex2f(100, 100);
        glEnd();

        glDisable(GL_BLEND);
        glDisable(GL_TEXTURE_2D);
    }

    ~FontApp()
    {
        if (context)
            SDL_GL_DeleteContext(context);
        if (window)
            SDL_DestroyWindow(window);
        SDL_Quit();
    }
};

int main()
{
    FontApp app;
    app.run();
    return 0;
}