#ifndef TETROMINO_H
#define TETROMINO_H

#include "block.h"

#include <vector>
#include <memory>


typedef std::array<std::array<std::shared_ptr<Block>, gridSizeX>, gridSizeY> block_grid_t;

enum tetroShape {O_TETRO, I_TETRO, J_TETRO, L_TETRO, S_TETRO, T_TETRO, Z_TETRO};

class Tetromino{ // every tetromino is made of four blocks stored in vBlockP
	public:
		blocks_vector_ptr_t vBlockP; // Contains the blocks of the tetromino
		double centralPoint[2] = {0.0};

		Tetromino();
		void genBlocks(SDL_Renderer* ren);
		Tetromino(const tetroShape shape, SDL_Renderer* ren);
		~Tetromino();
		void free();
		void moveDown(block_grid_t& blockGrid);
		void moveSide(block_grid_t& blockGrid, const int& dx);
		void render(SDL_Renderer* ren);
		bool detectCollisionBot(const block_grid_t& blockGrid);
		bool detectCollisionSide(const block_grid_t& blockGrid, const int& dx);
		bool turn(block_grid_t& blockGrid, const int& t);
		bool kickWall(block_grid_t& blockGrid, const int& t);
		tetroShape getShape() const;
		void alphaModBlocks(const Uint8& alpha);

	private:
		int _orientation;
		tetroShape _shape;
};

void moveDownGhost(const block_grid_t& blockGrid, Tetromino& ghost, const Tetromino& originalTetro);

#endif
