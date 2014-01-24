#include "texture.h"

#include "glew.h"

#include "SDL.h"
#include "SDL_image.h"

#include <stdio.h>

SDL_Surface* inverserPixels(SDL_Surface *src);

GLuint chargerTexture(const char* filename, GLenum filter) {

    puts("\n----------- Texture -------");
    printf("Loading '%s' ... ", filename);

    SDL_Surface* raw = NULL;
    raw = SDL_LoadBMP(filename);

    if (raw == NULL)
        raw = IMG_Load(filename);

    if (raw == NULL)
    {
        puts("Error");
        return 0;
    }

    SDL_Surface* img = inverserPixels(raw);
    SDL_FreeSurface(raw);

    GLenum formatInterne;
    GLenum format;

    if (img->format->BytesPerPixel == 4) {

        formatInterne = GL_RGBA;

        if (img->format->Rmask == 0xff)
            format = GL_RGBA;
        else
            format = GL_BGRA;

    }
    else if (img->format->BytesPerPixel == 3) {

        formatInterne = GL_RGB;

        if (img->format->Rmask == 0xff)
            format = GL_RGB;
        else
            format = GL_BGR;
    }
    else {

        printf("Format d'image non supporté\n");

        return -1;
    }

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D, 0, formatInterne, img->w, img->h, 0, format, GL_UNSIGNED_BYTE, img->pixels);

//    GLuint sampler;
//    glGenSamplers(1, &sampler);
//    glBindSampler(1, sampler);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);

//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

    glBindTexture(GL_TEXTURE_2D, 0);

    SDL_FreeSurface(img);

    puts("Ok");

    return texture;
}

SDL_Surface* inverserPixels(SDL_Surface *src)
{
    // Copie conforme de l'image source sans les pixels

    SDL_Surface *imageInversee = SDL_CreateRGBSurface(0, src->w, src->h, src->format->BitsPerPixel,
                                                      src->format->Rmask, src->format->Gmask, src->format->Bmask, src->format->Amask);


    // Tableau intermédiaires permettant de manipuler les pixels

    unsigned char* pixelsSources = (unsigned char*) src->pixels;
    unsigned char* pixelsInverses = (unsigned char*) imageInversee->pixels;


    // Inversion des pixels

    int i;
    for(i = 0; i < src->h; i++)
    {
        int j;
        for(j = 0; j < src->w * src->format->BytesPerPixel; j++)
            pixelsInverses[(src->w * src->format->BytesPerPixel * (src->h - 1 - i)) + j] = pixelsSources[(src->w * src->format->BytesPerPixel * i) + j];
    }


    // Retour de l'image inversée

    return imageInversee;
}
