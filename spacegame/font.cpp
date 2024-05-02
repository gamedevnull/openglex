
#include "include/font.hpp"

FontRenderer::FontRenderer()
{
}

void FontRenderer::createPrintableAsciiCharsTexturesFromPng(const char *filename, int symW, int symH)
    {
        SDL_Surface *surface = IMG_Load(filename);

        int offX = 0;
        int offY = 0;

        SDL_Surface *newSurface;
        newSurface = nullptr;

        int srcW = surface->w;
      
        GLuint textureID = 0;
        for (int i = 0; i < 95; ++i)
        {
            newSurface = getPartOfSurfaceAsNewSurface(surface, symW, symH, offX, offY);
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

SDL_Surface *FontRenderer::getPartOfSurfaceAsNewSurface(SDL_Surface *surface, int symW, int symH, int offX, int offY)
    {
        SDL_Surface *newSurface = SDL_CreateRGBSurfaceWithFormat(0, symW, symH, 32, SDL_PIXELFORMAT_RGBA32);

        Uint32 p32, *buf32;
        Uint8 r, g, b, a;

        int srcW = surface->w;
        
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

GLuint FontRenderer::createTextureFromSurface(SDL_Surface *surface)
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

void FontRenderer::renderTextureBlock(GLuint texture, float x, float y, float textureWidth, float textureHeight)
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

void FontRenderer::renderInt(int number, int posX, int posY)
{
    char *text = myIntToStr(number);
    renderText(text, posX, posY);
}

void FontRenderer::renderText(const char *str, int posX, int posY)
    {
        while (*str != '\0')
        {
            int asciiCode = static_cast<int>(*str);
            renderTextureBlock(fontTextures[asciiCode - 32], posX, posY, 12, 16);
            posX += 12;
            ++str;
        }
    }

char *FontRenderer::myIntToStr(int num)
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

FontRenderer::~FontRenderer()
{
}