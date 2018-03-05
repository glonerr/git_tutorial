#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <iostream>
#include "cleanup.h"

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

#define nullptr NULL

SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;

/*
 * Log an SDL error with some error message to the output stream of our choice
 * @param os The output stream to write the message too
 * @param msg The error message to write, format will be msg error: SDL_GetError()
 */
void logSDLError(std::ostream &os, const std::string &msg) {
	os << msg << " error: " << SDL_GetError() << std::endl;
}

/*
 * Draw an SDL_Texture to an SDL_Renderer at position x, y, with some desired
 * width and height
 * @param tex The source texture we want to draw
 * @param rend The renderer we want to draw too
 * @param x The x coordinate to draw too
 * @param y The y coordinate to draw too
 * @param w The width of the texture to draw
 * @param h The height of the texture to draw
 */
void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y, int w, int h) {
	//Setup the destination rectangle to be at the position we want
	SDL_Rect dst;
	dst.x = x;
	dst.y = y;
	dst.w = w;
	dst.h = h;
	SDL_RenderCopy(ren, tex, NULL, &dst);
}

SDL_Texture *renderText(std::string message, std::string fontFile, SDL_Color color, int fontSize) {
	TTF_Font *font = TTF_OpenFont(fontFile.c_str(), fontSize);
	if (font == nullptr)
		throw std::runtime_error("Failed to load font:" + fontFile + TTF_GetError());
	SDL_Surface *surface = TTF_RenderText_Blended(font, message.c_str(), color);
	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);
	TTF_CloseFont(font);
	return texture;
}

/*
 * Draw an SDL_Texture to an SDL_Renderer at position x, y, preserving
 * the texture's width and height
 * @param tex The source texture we want to draw
 * @param rend The renderer we want to draw too
 * @param x The x coordinate to draw too
 * @param y The y coordinate to draw too
 */
void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y) {
	int w, h;
	SDL_QueryTexture(tex, NULL, NULL, &w, &h);
	renderTexture(tex, ren, x, y, w, h);
}

//void ApplySurface(int x, int y, SDL_Texture *tex, SDL_Renderer *rend) {
//	SDL_Rect pos;
//	pos.x = x;
//	pos.y = y;
//	SDL_QueryTexture(tex, NULL, NULL, &pos.w, &pos.h);
//
//	SDL_RenderCopy(rend, tex, NULL, &pos);
//}

SDL_Texture *LoadImage(std::string file) {
	SDL_Texture *tex = IMG_LoadTexture(renderer, file.c_str());
	if (tex == nullptr)
		throw std::runtime_error("Failed to load image: " + file + IMG_GetError());
	return tex;
}

int main(int argc, char **argv) {
	if (SDL_Init(SDL_INIT_EVERYTHING) == -1) {
		std::cout << SDL_GetError() << std::endl;
		return 1;
	}

	if (TTF_Init() == -1) {
		std::cout << TTF_GetError() << std::endl;
		return 1;
	}

	window = SDL_CreateWindow("Lesson 2", SDL_WINDOWPOS_CENTERED,
	SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (window == nullptr) {
		logSDLError(std::cout, "CreateWindow");
		SDL_Quit();
		return 1;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == nullptr) {
		logSDLError(std::cout, "CreateRenderer");
		cleanup(window);
		SDL_Quit();
		return 1;
	}

//	SDL_Texture *image = LoadImage("res/image.png");
//	if (image == nullptr)
//	{
//		cleanup(image, renderer, window);
//		IMG_Quit();
//		SDL_Quit();
//		return 1;
//	}

	SDL_Color color = { 255, 255, 255, 255 };
	SDL_Texture *image = renderText("TTF fonts are cool!", "/home/lonerr/Tools/worktools/android/android-studio/plugins/android/lib/layoutlib/data/fonts/DroidSans.ttf", color, 64);
	if (image == nullptr) {
		cleanup(image, renderer, window);
		TTF_Quit();
		SDL_Quit();
		return 1;
	}

	SDL_RenderClear(renderer);

	//Our texture size won't change, so we can get it here
	//instead of constantly allocating/deleting ints in the loop
	int iW, iH;
	SDL_QueryTexture(image, NULL, NULL, &iW, &iH);
	int x = SCREEN_WIDTH / 2 - iW / 2;
	int y = SCREEN_HEIGHT / 2 - iH / 2;

	int x_offset = 0;
	int y_offset = 0;

	//Our event structure
	SDL_Event e;
	//For tracking if we want to quit
	bool quit = false;
	while (!quit) {
		//Read any events that occured, for now we'll just quit if any event occurs
		while (SDL_PollEvent(&e)) {
			//If user closes the window
			if (e.type == SDL_QUIT) {
				quit = true;
			}
			//If user presses any key
			if (e.type == SDL_KEYDOWN) {
				switch (e.key.keysym.scancode) {
				case SDL_SCANCODE_Q:
					quit = true;
					break;
				case SDL_SCANCODE_UP:
					y_offset -= 10;
					break;
				case SDL_SCANCODE_DOWN:
					y_offset += 10;
					break;
				case SDL_SCANCODE_LEFT:
					x_offset -= 10;
					break;
				case SDL_SCANCODE_RIGHT:
					x_offset += 10;
					break;
				default:
					break;
				}
			}
			//If user clicks the mouse
			if (e.type == SDL_MOUSEBUTTONDOWN) {
				quit = true;
			}
		}
		//Rendering
		SDL_RenderClear(renderer);
		//Draw the image
		renderTexture(image, renderer, x + x_offset, y + y_offset);
		//Update the screen
		SDL_RenderPresent(renderer);
	}

	//Destroy the various items
	cleanup(image, renderer, window);
	IMG_Quit();
	SDL_Quit();
	return 0;
}
