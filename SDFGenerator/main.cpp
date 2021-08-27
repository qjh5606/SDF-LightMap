#include <iostream>
using namespace std;
#include <string>
#include <chrono>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "SDL.h"
#include "original.h"


// SDL
SDL_Window* win = nullptr;
SDL_Renderer* ren = nullptr;
SDL_Texture* tex = nullptr;
SDL_Surface* surface = nullptr;

void DestorySDL(SDL_Window* window, SDL_Renderer* render, SDL_Texture* tex)
{
	if (tex)
	{
		SDL_DestroyTexture(tex);
		tex = nullptr;
	}
	if (render)
	{
		SDL_DestroyRenderer(render);
	}
	if (window)
	{
		SDL_DestroyWindow(window);
	}

	SDL_Quit();
}

int width, height, channel;
stbi_uc* image;


void set_pixel(SDL_Surface* surface, int x, int y, Uint32 pixel)
{
	Uint32* const target_pixel = (Uint32*)((Uint8*)surface->pixels
		+ y * surface->pitch
		+ x * surface->format->BytesPerPixel);
	*target_pixel = pixel;
}

void set_pixel(SDL_Surface* surface, int x, int y, Uint8 R,Uint8 G,Uint8 B)
{
	Uint32 color = SDL_MapRGB(surface->format, R, G, B);
	set_pixel(surface, x, y, color);
}


int main(int argc, char* argv[])
{
	//@TODO: parse args
	
	// every image to generate sdf
	Original Osdf1, Osdf2, Osdf3, Osdf4, Osdf5, Osdf6, Osdf7, Osdf8;
	bool bWriteSDFDisplay = false;
	Osdf1.run("../../example/a.png", bWriteSDFDisplay);
	Osdf2.run("../../example/b.png", bWriteSDFDisplay);
	Osdf3.run("../../example/c.png", bWriteSDFDisplay);
	Osdf4.run("../../example/d.png", bWriteSDFDisplay);
	Osdf5.run("../../example/e.png", bWriteSDFDisplay);
	Osdf6.run("../../example/f.png", bWriteSDFDisplay);
	Osdf7.run("../../example/g.png", bWriteSDFDisplay);
	Osdf8.run("../../example/h.png", bWriteSDFDisplay);

	SDFMerger merger;
	merger.images.push_back(&Osdf1);
	merger.images.push_back(&Osdf2);
	merger.images.push_back(&Osdf3);
	merger.images.push_back(&Osdf4);
	merger.images.push_back(&Osdf5);
	merger.images.push_back(&Osdf6);
	merger.images.push_back(&Osdf7);
	merger.images.push_back(&Osdf8);
	merger.MergeImage();

	image = merger.image;
	width = merger.imageWidth;
	height = merger.imageHeight;
	channel = merger.imageChannel;

	// SDL
	if (SDL_Init(SDL_INIT_VIDEO)) 
	{
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return 1;
	}

	win = SDL_CreateWindow("SDL Window", 100, 100, 512, 512, SDL_WINDOW_SHOWN);
	if (win == nullptr) 
	{
		std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		return 1;
	}

	ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (ren == nullptr) 
	{
		std::cout << "SDL_CreateRender Error: " << SDL_GetError() << std::endl;
		DestorySDL(win, ren, tex);
		return 1;
	}

	Uint32 rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xff000000;
	gmask = 0x00ff0000;
	bmask = 0x0000ff00;
	amask = 0x000000ff;
#else
	rmask = 0x000000ff;
	gmask = 0x0000ff00;
	bmask = 0x00ff0000;
	amask = 0xff000000;
#endif
	// https://wiki.libsdl.org/SDL_CreateRGBSurface
	surface = SDL_CreateRGBSurface(0, width, height, 32 ,rmask, gmask, bmask, amask);
	if (surface == nullptr)
	{
		std::cout << "SDL_CreateRGBSurface Error: " << SDL_GetError() << std::endl;
		DestorySDL(win, ren, tex);
	}

	// Write the pxiel
	SDL_LockSurface(surface);

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			Uint8 R = image[channel * width * y + x * channel];
			Uint8 G = image[channel * width * y + x * channel + 1];
			Uint8 B = image[channel * width * y + x * channel + 2];
			set_pixel(surface, x, y, R, G, B);
			//set_pixel(surface, x, y, 255, 0, 0);
		}
	}

	SDL_UnlockSurface(surface);

	// Create Texture for Rendering
	tex = SDL_CreateTextureFromSurface(ren, surface);
	SDL_FreeSurface(surface);
	if (tex == nullptr) 
	{
		std::cout << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
		DestorySDL(win, ren, tex);
		return 1;
	}
	// Copy to screen
	SDL_RenderClear(ren);
	SDL_RenderCopy(ren, tex, NULL, NULL);
	SDL_RenderPresent(ren);

	// Wait for a keypress
	SDL_Event event;
	while (true)
	{
		if (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
			case SDL_KEYDOWN:
				return true;
			}
		}
	}

	stbi_image_free(image);
	DestorySDL(win, ren, tex);
	
	return 0;
}