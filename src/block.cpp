#include "block.h"

#include <iostream>
#include <SDL2/SDL.h>


void logSDLError(std::ostream& os, const std::string& msg){
	os << msg << " error: " << SDL_GetError() << std::endl;
}


SDL_Window* createWindow(const std::string& title, const int& pos_x, const int& pos_y, const int& width, const int& height){
	const Uint32 flags = SDL_WINDOW_SHOWN;
	SDL_Window* win = SDL_CreateWindow(title.c_str(), pos_x, pos_y, width, height, flags);
	if(win == nullptr){
		logSDLError(std::cout, "SDL_CreateWindow");
	}
	return win;
}

SDL_Renderer* createRenderer(SDL_Window* win){
	const int index = -1;
	const Uint32 flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
	SDL_Renderer* ren = SDL_CreateRenderer(win, index, flags);

	if(ren == nullptr){
		logSDLError(std::cout, "SDL_CreateRenderer");
	}
	return ren;
}

// class Block

Block::Block():_tetroOwner(nullptr), _tex(nullptr), _x(0), _y(0), _screenX(0), _screenY(0){}

Block::Block(const std::string& imagePath, SDL_Renderer* ren, Tetromino* owner, const int& x, const int& y):
_tetroOwner(nullptr), _tex(nullptr), _x(x), _y(y), _screenX(x * blockWidth), _screenY(y * blockHeight){
	loadFromFile(imagePath, ren, owner);
}

Block::~Block(){
	free();
}

bool operator==(const Block& B1, const Block& B2){
	return (B1.getTexPtr() == B2.getTexPtr());
}

void Block::loadFromFile(const std::string& imagePath, SDL_Renderer* ren, Tetromino* owner){
	SDL_Surface* bmp = SDL_LoadBMP(imagePath.c_str());
	if(bmp == nullptr){
		logSDLError(std::cout, "SDL_LoadBMP");
		free();
	}
	_tex = SDL_CreateTextureFromSurface(ren, bmp);
	_tetroOwner = owner;
	SDL_FreeSurface(bmp);
	if(_tex == nullptr){
		logSDLError(std::cout, "SDL_CreateBlockFromSurface");
		free();
	}
}

void Block::free(){
	if(_tex != nullptr){
		SDL_DestroyTexture(_tex);
		_tex = nullptr;
	}
	_x = 0;
	_y = 0;
	_screenX = _x * blockWidth;
	_screenY = _y * blockHeight;
	_tetroOwner = nullptr;
}

void Block::render(SDL_Rect* clip, SDL_Renderer* ren){
	// clip: Source rectangle that contains the part of the block to be rendered
	if(_tex == nullptr){
		SDL_Delay(1000);
		return;
	}
	// RenderQuad: destination rectangle, has coordinates x and y of the block and the size of clip
	SDL_Rect renderQuad = {_screenX, _screenY, blockWidth, blockHeight};
	if(clip != nullptr){
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}
	if(SDL_RenderCopy(ren, _tex, clip, &renderQuad)) logSDLError(std::cout, "SDL_RenderCopy");
}

bool Block::isInitialized(){
	return (_tex != nullptr);
}

int Block::getX() const{
	return _x;
}

int Block::getY() const{
	return _y;
}

void Block::addX(const int& dx){
	_x += dx;
}
void Block::addY(const int& dy){
	_y += dy;
}

void Block::setX(const int& x){
	_x = x;
}
void Block::setY(const int& y){
	_y = y;
}

Tetromino* Block::getTetroOwner(){
	return _tetroOwner;
}

void Block::updateCoordinates(){
	_screenX = _x * blockWidth;
	_screenY = _y * blockHeight;
}

SDL_Texture* Block::getTexPtr() const{
	return _tex;
}

void Block::alphaMod(const Uint8& alpha){
	if(SDL_SetTextureAlphaMod(_tex, alpha)) logSDLError(std::cout, "SDL_SetTextureAlphaMod");
}
