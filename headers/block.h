#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <iostream>
#include <memory>
#include <vector>
#include <SDL2/SDL.h>


class Tetromino;

const std::string resPath = "../res/";

constexpr int blockWidth = 32;
constexpr int blockHeight = 32;

constexpr int gridSizeX = 10;
constexpr int gridSizeY = 20;

constexpr int fieldWidth = gridSizeX * blockWidth;
constexpr int fieldHeight = gridSizeY * blockHeight;

void logSDLError(std::ostream& os, const std::string& msg);

class Block{ // Blocks contains the texture to be rendered on screen
	public:
		Block();
		Block(const std::string& imagePath, SDL_Renderer* ren, Tetromino* owner, const int& x = 0, const int& y = 0);
		~Block();

		void loadFromFile(const std::string& imagePath, SDL_Renderer* ren, Tetromino* owner);
		void free();
		void render(SDL_Rect* clip, SDL_Renderer* ren);
		bool isInitialized();;
		void updateCoordinates();
		SDL_Texture* getTexPtr() const;
		void alphaMod(const Uint8& alpha);

		int getX() const;
		int getY() const;
		void addX(const int& dx);
		void addY(const int& dy = 1);
		void setX(const int& x);
		void setY(const int& y);

		Tetromino* getTetroOwner();

	private:
		Tetromino* _tetroOwner;
		SDL_Texture* _tex;
		int _x;
		int _y;
		int _screenX;
		int _screenY;

};

typedef std::vector<std::shared_ptr<Block> > blocks_vector_ptr_t;


bool operator==(const Block& B1, const Block& B2);

SDL_Window* createWindow(const std::string& title, const int& pos_x, const int& pos_y, const int& width, const int& height);
SDL_Renderer* createRenderer(SDL_Window* win);

#endif
