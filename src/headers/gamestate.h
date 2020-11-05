#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "block.h"
#include "tetro.h"

#include <iostream>
#include <random>
#include <algorithm>
#include <string>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

using namespace std;

class Text{
	public:
		SDL_Color color = {0, 0, 0};
		SDL_Rect destRect = {0, 0, 0, 0};
		
		Text();
		Text(SDL_Renderer* ren, const string& fontPath, const SDL_Color& textColor, const string& text, const int& size, const int& x, const int& y);
		void load(SDL_Renderer* ren, const string& fontPath, const SDL_Color& textColor, const string& text, const int& size, const int& x, const int& y);
		void center(const int& windowWidth, const int& windowHeight);
		~Text();
		void render(SDL_Renderer* ren, const char* newText);
		void free();
	private:
		SDL_Texture* tex = nullptr;
		TTF_Font* font = nullptr;
};

class GameState{
	public:
		SDL_Window* win = nullptr;
		SDL_Renderer* ren = nullptr;
		SDL_Event e;
		Mix_Music* theme = nullptr;
		
		Text scoreText;
		Text levelText;
		Text linesText;
		Text countDownText;
		Text pauseText;
		
		bool quit = false;
		bool gameOver = false;
		bool gamePaused = false;
		bool mute = false;
		
		unsigned int score = 0;
		unsigned int level = 1;
		unsigned int linesCleared = 0;
		
		shared_ptr<Block> blockGrid[gridSizeY][gridSizeX] = {nullptr}; // A 2-array containing pointers to all the blocks in the game field
		vector<shared_ptr<Tetro>> vTetroP; // Vector containing all tetrominoes (pieces) in game
		
		shared_ptr<Tetro> ghost = nullptr; // The ghost predicts where the next piece will fall
		
		Uint32 oldTime = 0;
		Uint32 newTime = 0;
		Uint32 delay = 1000;
		
		const Uint8* currentKeyStates = nullptr;
		
		GameState();
		~GameState();
		
		void processInput(int& dx, int& dy, int& t, bool& setPause, bool& setMute);
		void updateGame(const int& dx, const int& dy, const int& t, const bool& setPause, const bool& setMute);
		void updateGraphics();

		
	private:
		void makeGhost(const Tetro& currentTetro);
		void updateGhost(const Tetro& currentTetro);
		
		bool checkGrid(const Tetro& tetro);
		void newTetro();
		void eraseLine();
		void packLines(vector<int>& lines);
};

#endif
