#include "texture.h"

LTexture::LTexture()
{
    // Initialize
    mTexture = nullptr;
    mWidth = 0;
    mHeight = 0;
}

LTexture::~LTexture()
{
    // Deallocate
    free();
}

bool LTexture::loadFromFile(std::string path)
{
    // Get rid of preexisting texture
    free();

    // The final texture
    SDL_Texture *newTexture = nullptr;

    // Load image at specified path
    SDL_Surface *loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == nullptr)
    {
        std::cout << "Unable to load image! SDL_image Error: " + std::string(IMG_GetError()) + "\n";
    }
    else
    {
        // Color key image
        // SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0xFF, 0xFF, 0xFF));

        // Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
        if (newTexture == nullptr)
        {
            std::cout << "Unable to create texture from! SDL Error: " + std::string(SDL_GetError()) + "\n";
        }
        else
        {
            // Get image dimensions
            mWidth = loadedSurface->w;
            mHeight = loadedSurface->h;
        }

        // Get rid of old loaded surface
        SDL_FreeSurface(loadedSurface);
    }

    // Return success
    mTexture = newTexture;
    return mTexture != nullptr;
}

void LTexture::free()
{
    // Free texture if it exists
    if (mTexture != nullptr)
    {
        SDL_DestroyTexture(mTexture);
        mTexture = nullptr;
        mWidth = 0;
        mHeight = 0;
    }
}

void LTexture::render(int x, int y, int w, int h, SDL_Rect *clip)
{
    // Set rendering space and render to screen
    SDL_Rect renderQuad = {x, y, w, h};
    if (w == 0 && h == 0)
    {
        renderQuad.w = clip->w;
        renderQuad.h = clip->h;
    }

    // Render to screen
    SDL_RenderCopy(gRenderer, mTexture, clip, &renderQuad);
}

int LTexture::getWidth()
{
    return mWidth;
}

int LTexture::getHeight()
{
    return mHeight;
}

SDL_Texture *LTexture::getTexture()
{
    return mTexture;
}