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
    GLuint fontTextures[95]; // ASCII printable characters as textures

    FontApp()
    {
        isRunning = 0;
        window = nullptr;
        context = nullptr;
    }

    void run()
    {
        if (SDL_Init(SDL_INIT_VIDEO) < 0)
        {
            std::cout << "SDL init problem" << std::endl;
            return;
        }

        window = SDL_CreateWindow("OpenGl text demo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
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

        /* 12x16 in 192x96 png with white ascii letters on black color as transparent
           https://opengameart.org/content/16x12-terminal-bitmap-font */
        createPrintableAsciiCharsTexturesFromPng("pixfont.png", 12, 16, 192, 96);

        isRunning = 1;
        while (isRunning)
        {
            WaitFrame(60);
            ProcessEvents();
            Update();
            Render();
        }
    }

    GLuint createTextureFromSurface(SDL_Surface *surface)
    {
        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        return textureID;
    }

    void createPrintableAsciiCharsTexturesFromPng(const char *filename, int symW, int symH, int srcW, int srcH)
    {
        SDL_Surface *surface = IMG_Load(filename);

        int offX = 0;
        int offY = 0;

        SDL_Surface *newSurface;
        newSurface = nullptr;

        GLuint textureID = 0;
        for (int i = 0; i < 95; ++i)
        {
            newSurface = getPartOfSurfaceAsNewSurface(surface, symW, symH, offX, offY, srcW, srcH);
            textureID = createTextureFromSurface(newSurface);
            fontTextures[i] = textureID;
            SDL_FreeSurface(newSurface);

            offX += symW;
            if (offX >= srcW)
            {
                offX = 0;
                offY += symH;
            }
        }
        SDL_FreeSurface(surface);
    }

    SDL_Surface *getPartOfSurfaceAsNewSurface(SDL_Surface *surface, int symW, int symH, int offX, int offY, int srcW, int srcH)
    {
        SDL_Surface *newSurface = SDL_CreateRGBSurfaceWithFormat(0, symW, symH, 32, SDL_PIXELFORMAT_RGBA32);

        Uint32 p32, *buf32;
        Uint8 r, g, b, a;

        for (int y = 0; y < symH; y++)
        {
            for (int x = 0; x < symW; x++)
            {
                buf32 = (Uint32 *)surface->pixels;
                p32 = buf32[(offY + y) * srcW + (offX + x)];

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

        return newSurface;
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

         renderText("T35T1N9", 100, 180);

        renderText("Rendering text in OpenGL", 10, 130);
        renderText("123 @#$ asdf ASDF ?!*&%", 10, 100);
        renderText("Hello World! :-)", 10, 70);

        int someScore = 21;
        char *myString = myIntToStr(someScore);

        renderText("Score: ", 10, 40);
        renderText(myString, 84, 40);

        SDL_GL_SwapWindow(window);
    }

    char *myIntToStr(int num)
    {
        // not ideal but works
        int digts = 1;
        int tmp = num;
        while (tmp /= 10)
        {
            digts++;
        }
        char *str = new char[digts + 1];
        for (int i = digts - 1; i >= 0; i--)
        {
            int digit = num % 10;
            str[i] = digit + '0';
            num /= 10;
        }
        str[digts] = '\0';
        return str;
    }

    void renderText(const char *str, int posX, int posY)
    {
        while (*str != '\0')
        {
            int asciiCode = static_cast<int>(*str);
            renderTextureBlock(fontTextures[asciiCode - 32], posX, posY, 12, 16);
            posX += 12;
            ++str;
        }
    }

    void renderTextureBlock(GLuint texture, float x, float y, float textureWidth, float textureHeight)
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glBegin(GL_TRIANGLES);
        glTexCoord2f(0, 1);
        glVertex2f(x, y);

        glTexCoord2f(1, 1);
        glVertex2f(x + textureWidth, y);

        glTexCoord2f(1, 0);
        glVertex2f(x + textureWidth, y + textureHeight);

        glTexCoord2f(1, 0);
        glVertex2f(x + textureWidth, y + textureHeight);

        glTexCoord2f(0, 0);
        glVertex2f(x, y + textureHeight);

        glTexCoord2f(0, 1);
        glVertex2f(x, y);
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