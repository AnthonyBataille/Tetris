#include "block.h"

#include <iostream>
#include <SDL2/SDL.h>


void logSDLError(ostream& os, const string& msg){
	os << msg << " error: " << SDL_GetError() << endl;
}


SDL_Window* createWindow(const string& title, const int& pos_x, const int& pos_y, const int& width, const int& height){
	const Uint32 flags = SDL_WINDOW_SHOWN;
	SDL_Window* win = SDL_CreateWindow(title.c_str(), pos_x, pos_y, width, height, flags);
	if(win == nullptr){
		logSDLError(cout, "SDL_CreateWindow");
	}
	return win;
}

SDL_Renderer* createRenderer(SDL_Window* win){
	const int index = -1;
	const Uint32 flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
	SDL_Renderer* ren = SDL_CreateRenderer(win, index, flags);

	if(ren == nullptr){
		logSDLError(cout, "SDL_CreateRenderer");
	}
	return ren;
}

// class Block

Block::Block(){
	tex = nullptr;
	width = 0;
	height = 0;
	x = 0;
	y = 0;
	tetroOwner = nullptr;
}

Block::Block(const string& imagePath, SDL_Renderer* ren, Tetro* owner, const int& gridX = 0, const int& gridY = 0){
	loadFromFile(imagePath, ren, owner, gridX, gridY);
}

Block::~Block(){
	free();
}

bool operator==(const Block& B1, const Block& B2){
	return (B1.getWidth() == B2.getWidth() && B1.getHeight() == B2.getHeight() && B1.getTexPtr() == B2.getTexPtr());
}

bool Block::loadFromFile(const string& imagePath, SDL_Renderer* ren, Tetro* owner, const int& gridX = 0, const int& gridY = 0){
	x = gridX;
	y = gridY;
	screenX = x * blockWidth;
	screenY = y * blockHeight;
	SDL_Surface* bmp = SDL_LoadBMP(imagePath.c_str());
	if(bmp == nullptr){
		logSDLError(cout, "SDL_LoadBMP");
		tex = nullptr;
		width = 0;
		height = 0;
		x = 0;
		y = 0;
		screenX = x * blockWidth;
		screenY = y * blockHeight;
		tetroOwner = nullptr;
		return false;
	}
	width = bmp->w;
	height = bmp->h;
	tex = SDL_CreateTextureFromSurface(ren, bmp);
	tetroOwner = owner;
	SDL_FreeSurface(bmp);
	if(tex == nullptr){
		logSDLError(cout, "SDL_CreateBlockFromSurface");
		width = 0;
		height = 0;
		x = 0;
		y = 0;
		screenX = x * blockWidth;
		screenY = y * blockHeight;
		tetroOwner = nullptr;
		return false;
	}
	return true;
}

void Block::free(){
	if(tex != nullptr){
		SDL_DestroyTexture(tex);
		tex = nullptr;
	}
	width = 0;
	height = 0;
	x = 0;
	y = 0;
	screenX = x * blockWidth;
	screenY = y * blockHeight;
	tetroOwner = nullptr;
}

void Block::render(SDL_Rect* clip, SDL_Renderer* ren){
	// clip: Source rectangle that contains the part of the block to be rendered
	if(tex == nullptr){
		cout << "No block to render" << endl;
		SDL_Delay(1000);
		return;
	}
	// RenderQuad: destination rectangle : has coordinates x and y of the block and the size of clip
	SDL_Rect renderQuad = {screenX, screenY, width, height};
	if(clip != nullptr){
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}
	if(SDL_RenderCopy(ren, tex, clip, &renderQuad)) logSDLError(cout, "SDL_RenderCopy");
}

bool Block::isInitialized(){
	return (tex != nullptr);
}

int Block::getWidth() const{
	return width;
}

int Block::getHeight() const{
	return height;
}

void Block::updateCoordinates(){
	screenX = x * blockWidth;
	screenY = y * blockHeight;
}

SDL_Texture* Block::getTexPtr() const{
	return tex;
}

void Block::alphaMod(const Uint8& alpha){
	if(SDL_SetTextureAlphaMod(tex, alpha)) logSDLError(cout, "SDL_SetTextureAlphaMod");
}
