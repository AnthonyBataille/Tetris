#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "block.h"
#include "tetromino.h"

#include <iostream>
#include <memory>
#include <random>
#include <algorithm>
#include <string>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>


typedef std::array<std::array<std::shared_ptr<Block>, gridSizeX>, gridSizeY> block_grid_t;
typedef std::vector<std::shared_ptr<Block> > blocks_vector_ptr_t;
typedef std::vector<std::shared_ptr<Tetromino> > tetros_vector_ptr_t;

class Label{
	public:
		Label();
		Label(SDL_Renderer* ren, const std::string& fontPath, const SDL_Color& color, const std::string& text, const int& size, const int& x, const int& y);
		void load(SDL_Renderer* ren, const std::string& fontPath, const SDL_Color& color, const std::string& text, const int& size, const int& x, const int& y);
		void center(const int& windowWidth, const int& windowHeight);
		~Label();
		void render(SDL_Renderer* ren, const char* newLabel);
		void free();

	private:
		SDL_Color _color;
		SDL_Rect _destRect;
		SDL_Texture* _tex;
		TTF_Font* _font;
};

class GameState{
	public:
		SDL_Window* win;
		SDL_Renderer* ren;
		SDL_Event e;
		Mix_Music* theme;

		Label scoreLabel;
		Label levelLabel;
		Label linesLabel;
		Label countDownLabel;
		Label pauseLabel;
		Label gameOverLabel;

		bool quit;
		bool gameOver;
		bool gamePaused;
		bool mute;

		unsigned int score;
		unsigned int level;
		unsigned int linesCleared;

		block_grid_t blockGrid; // A 2-array containing pointers to all the blocks in the game field
		tetros_vector_ptr_t vTetroP; // Vector containing all tetrominoes (pieces) in game

		std::shared_ptr<Tetromino> ghost; // The ghost predicts where the next piece will fall

		Uint32 oldTime;
		Uint32 newTime;
		Uint32 delay;

		const Uint8* currentKeyStates;

		GameState();
		~GameState();

		void processInput(int& dx, int& dy, int& t, bool& setPause, bool& setMute);
		void updateGame(const int& dx, const int& dy, const int& t, const bool& setPause, const bool& setMute);
		void updateGraphics();


	private:
		void makeGhost(const Tetromino& currentTetro);
		void updateGhost(const Tetromino& currentTetro);

		bool checkGrid(const Tetromino& tetro);
		void newTetro();
		void eraseLine();
		void packLines(std::vector<int>& lines);
};

#endif
