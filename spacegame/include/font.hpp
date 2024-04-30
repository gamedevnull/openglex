#pragma once
#include <iostream>
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

class FontRenderer
{
public:
    GLuint fontTextures[95]; // ASCII printable characters as textures

    FontRenderer();

    void createPrintableAsciiCharsTexturesFromPng(const char *filename, int symW, int symH, int srcW, int srcH);

    SDL_Surface *getPartOfSurfaceAsNewSurface(SDL_Surface *surface, int symW, int symH, int offX, int offY, int srcW, int srcH);

    GLuint createTextureFromSurface(SDL_Surface *surface);

    void renderTextureBlock(GLuint texture, float x, float y, float textureWidth, float textureHeight);

    void renderInt(int number, int posX, int posY);

    void renderText(const char *str, int posX, int posY);

    char *myIntToStr(int num);

    ~FontRenderer();
};