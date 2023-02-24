#ifndef TEXTURE_H
#define TEXTURE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>

// The window renderer
extern SDL_Renderer *gRenderer;

// Texture wrapper class
class LTexture
{
public:
    // Initializes variables
    LTexture();

    // Deallocates memory
    ~LTexture();

    // Loads image at specified path
    bool loadFromFile(std::string path);

    // Deallocates texture
    void free();

    // Renders texture at given point
    void render(int x, int y, int w, int h, SDL_Rect *clip = nullptr);

    // Gets image dimensions
    int getWidth();
    int getHeight();
    SDL_Texture *getTexture();

private:
    // The actual hardware texture
    SDL_Texture *mTexture;

    // Image dimensions
    int mWidth;
    int mHeight;
};

#endif

