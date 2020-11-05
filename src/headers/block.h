#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <iostream>
#include <SDL2/SDL.h>


using namespace std;

class Tetro;

const string resPath = "../res/";

const int blockWidth = 32;
const int blockHeight = 32;

const int gridSizeX = 10;
const int gridSizeY = 20;

const int fieldWidth = gridSizeX * blockWidth;
const int fieldHeight = gridSizeY * blockHeight;

void logSDLError(ostream& os, const string& msg);

class Block{ // Blocks contains the texture to be rendered on screen
	public:
		Block();
		Block(const string& imagePath, SDL_Renderer* ren, Tetro* owner, const int& posX, const int& posY);
		~Block();
		
		bool loadFromFile(const string& imagePath, SDL_Renderer* ren, Tetro* owner, const int& posX, const int& posY);
		void free();
		void render(SDL_Rect* clip, SDL_Renderer* ren);
		bool isInitialized();
		int getWidth() const;
		int getHeight() const;
		void updateCoordinates();
		SDL_Texture* getTexPtr() const;
		void alphaMod(const Uint8& alpha);
		
		int x = 0;
		int y = 0;
		
		Tetro* tetroOwner = nullptr;
		
	private:
		SDL_Texture* tex = nullptr;
		int width = 0;
		int height = 0;
		int screenX = 0;
		int screenY = 0;

};

bool operator==(const Block& B1, const Block& B2);

SDL_Window* createWindow(const string& title, const int& pos_x, const int& pos_y, const int& width, const int& height);
SDL_Renderer* createRenderer(SDL_Window* win);

#endif
